#include "Memory.h"

#include <stdlib.h>
#include <stdio.h>

#include "../Log/Log.h"

#if HEAP_TRACKING == 1

#include <string.h>
#include <new>

// the { host sizeof, attiny3217 sizeof } of each class allocated with new
#include "AvrSizes.h"

// The simulated heap is a copy of the avr-libc malloc/free/realloc running on
// a fake 16 bit address space. Only the chunk headers and freelist live in the
// simulated memory, the actual data of each allocation lives in a normal host
// allocation that records which simulated chunk it owns.
#define SIM_HEAP_START  0x0100
#define SIM_HEAP_LIMIT  0xF000
// sizeof(size_t) and sizeof(struct __freelist) in avr-libc
#define SIM_HDR         2
#define SIM_FL_SIZE     4

static uint8_t sim[0x10000];
static uint16_t sim_flp = 0;
static uint16_t sim_brkval = 0;
static uint32_t sim_maxSize = 0;
static bool sim_tracking = false;
static HeapStats sim_stats;

static uint16_t sz(uint16_t a) { return sim[a] | (sim[a + 1] << 8); }
static uint16_t nx(uint16_t a) { return sim[a + 2] | (sim[a + 3] << 8); }
static void setsz(uint16_t a, uint16_t v) { sim[a] = v & 0xFF; sim[a + 1] = v >> 8; }
static void setnx(uint16_t a, uint16_t v) { sim[a + 2] = v & 0xFF; sim[a + 3] = v >> 8; }

// the equivalent of __malloc_heap_end or SP - __malloc_margin
static uint16_t sim_heapEnd()
{
  if (!sim_maxSize || sim_maxSize > (SIM_HEAP_LIMIT - SIM_HEAP_START)) {
    return SIM_HEAP_LIMIT;
  }
  return (uint16_t)(SIM_HEAP_START + sim_maxSize);
}

// avr-libc malloc()
static uint16_t sim_malloc(uint16_t len)
{
  if (len < SIM_FL_SIZE - SIM_HDR) {
    len = SIM_FL_SIZE - SIM_HDR;
  }
  // look for an exact fit, otherwise remember the smallest chunk that fits
  uint16_t s = 0, sfp1 = 0, sfp2 = 0;
  for (uint16_t fp1 = sim_flp, fp2 = 0; fp1; fp2 = fp1, fp1 = nx(fp1)) {
    if (sz(fp1) < len) {
      continue;
    }
    if (sz(fp1) == len) {
      if (fp2) {
        setnx(fp2, nx(fp1));
      } else {
        sim_flp = nx(fp1);
      }
      return fp1 + SIM_HDR;
    }
    if (!s || sz(fp1) < s) {
      s = sz(fp1);
      sfp1 = fp1;
      sfp2 = fp2;
    }
  }
  if (s) {
    // too small to split so use the whole chunk
    if (s - len < SIM_FL_SIZE) {
      if (sfp2) {
        setnx(sfp2, nx(sfp1));
      } else {
        sim_flp = nx(sfp1);
      }
      return sfp1 + SIM_HDR;
    }
    // split the top of the chunk off
    s -= len;
    uint16_t cp = sfp1 + s;
    setsz(cp, len);
    setsz(sfp1, s - SIM_HDR);
    return cp + SIM_HDR;
  }
  // nothing on the freelist, grow the heap
  if (!sim_brkval) {
    sim_brkval = SIM_HEAP_START;
  }
  uint16_t cp = sim_heapEnd();
  if (cp <= sim_brkval) {
    return 0;
  }
  uint16_t avail = cp - sim_brkval;
  if (avail >= len && avail >= len + SIM_HDR) {
    uint16_t fp1 = sim_brkval;
    sim_brkval += len + SIM_HDR;
    setsz(fp1, len);
    return fp1 + SIM_HDR;
  }
  return 0;
}

// avr-libc free()
static void sim_free(uint16_t p)
{
  if (!p) {
    return;
  }
  uint16_t fpnew = p - SIM_HDR;
  setnx(fpnew, 0);
  if (!sim_flp) {
    if (p + sz(fpnew) == sim_brkval) {
      sim_brkval = fpnew;
    } else {
      sim_flp = fpnew;
    }
    return;
  }
  // insert into the address ordered freelist and merge with the next chunk
  uint16_t fp1, fp2 = 0;
  for (fp1 = sim_flp; fp1; fp2 = fp1, fp1 = nx(fp1)) {
    if (fp1 < fpnew) {
      continue;
    }
    setnx(fpnew, fp1);
    if (fpnew + SIM_HDR + sz(fpnew) == fp1) {
      setsz(fpnew, sz(fpnew) + sz(fp1) + SIM_HDR);
      setnx(fpnew, nx(fp1));
    }
    break;
  }
  if (!fp2) {
    sim_flp = fpnew;
    return;
  }
  // merge with the previous chunk
  setnx(fp2, fpnew);
  if (fp2 + SIM_HDR + sz(fp2) == fpnew) {
    setsz(fp2, sz(fp2) + sz(fpnew) + SIM_HDR);
    setnx(fp2, nx(fpnew));
  }
  // if the last free chunk is at the top of the heap then lower the heap
  for (fp1 = sim_flp, fp2 = 0; nx(fp1); fp2 = fp1, fp1 = nx(fp1));
  if (fp1 + SIM_HDR + sz(fp1) == sim_brkval) {
    if (!fp2) {
      sim_flp = 0;
    } else {
      setnx(fp2, 0);
    }
    sim_brkval = fp1;
  }
}

// avr-libc realloc()
static uint16_t sim_realloc(uint16_t ptr, uint16_t len)
{
  if (!ptr) {
    return sim_malloc(len);
  }
  uint16_t fp1 = ptr - SIM_HDR;
  if (len <= sz(fp1)) {
    // shrink, only give back the end if it can be a chunk on its own
    if (sz(fp1) <= SIM_FL_SIZE || len > sz(fp1) - SIM_FL_SIZE) {
      return ptr;
    }
    uint16_t fp2 = ptr + len;
    setsz(fp2, sz(fp1) - len - SIM_HDR);
    setsz(fp1, len);
    sim_free(fp2 + SIM_HDR);
    return ptr;
  }
  // grow into the next chunk if it is free and big enough
  uint16_t incr = len - sz(fp1);
  uint16_t next = ptr + sz(fp1);
  uint16_t s = 0;
  for (uint16_t fp3 = sim_flp, ofp3 = 0; fp3; ofp3 = fp3, fp3 = nx(fp3)) {
    if (fp3 == next && sz(fp3) + SIM_HDR >= incr) {
      uint16_t fp3sz = sz(fp3);
      uint16_t fp3nx = nx(fp3);
      uint16_t fp2;
      if (fp3sz + SIM_HDR - incr > SIM_FL_SIZE) {
        fp2 = ptr + len;
        setnx(fp2, fp3nx);
        setsz(fp2, fp3sz - incr);
      } else {
        incr = fp3sz + SIM_HDR;
        fp2 = fp3nx;
      }
      if (ofp3) {
        setnx(ofp3, fp2);
      } else {
        sim_flp = fp2;
      }
      setsz(fp1, sz(fp1) + incr);
      return ptr;
    }
    if (sz(fp3) > s) {
      s = sz(fp3);
    }
  }
  // the topmost chunk can grow the heap in place
  if (sim_brkval == ptr + sz(fp1) && len > s) {
    if ((uint32_t)ptr + len < sim_heapEnd()) {
      sim_brkval = ptr + len;
      setsz(fp1, len);
      return ptr;
    }
    return 0;
  }
  // otherwise move it
  uint16_t memp = sim_malloc(len);
  if (!memp) {
    return 0;
  }
  sim_free(ptr);
  return memp;
}

void heap_getStats(HeapStats &stats)
{
  stats = sim_stats;
  stats.extent = sim_brkval ? sim_brkval - SIM_HEAP_START : 0;
  stats.holeBytes = 0;
  stats.numHoles = 0;
  stats.largestFree = 0;
  for (uint16_t fp = sim_flp; fp; fp = nx(fp)) {
    stats.holeBytes += sz(fp) + SIM_HDR;
    stats.numHoles++;
    if (sz(fp) > stats.largestFree) {
      stats.largestFree = sz(fp);
    }
  }
  uint16_t top = sim_brkval ? sim_brkval : SIM_HEAP_START;
  uint16_t end = sim_heapEnd();
  if (end > top + SIM_HDR && (uint32_t)(end - top - SIM_HDR) > stats.largestFree) {
    stats.largestFree = end - top - SIM_HDR;
  }
}

void heap_printStats(const char *label)
{
  HeapStats stats;
  heap_getStats(stats);
  fprintf(stderr, "HEAP [%s]: used %u, extent %u (peak %u) of %u, %u bytes in %u holes, largest free %u, %u failures (%u fragmented)\n",
    label, stats.used, stats.extent, stats.peakExtent, sim_maxSize, stats.holeBytes, stats.numHoles,
    stats.largestFree, stats.failures, stats.fragmentedFailures);
}

static void sim_failed(uint32_t len)
{
  HeapStats stats;
  heap_getStats(stats);
  uint32_t endFree = sim_heapEnd() - (sim_brkval ? sim_brkval : SIM_HEAP_START);
  uint32_t totalFree = stats.holeBytes + endFree;
  bool fragmented = (totalFree >= len + SIM_HDR);
  sim_stats.failures++;
  if (fragmented) {
    sim_stats.fragmentedFailures++;
  }
  fprintf(stderr, "HEAP: failed to allocate %u bytes with %u used, %u free (%u in %u holes + %u at the end) largest %u%s\n",
    len, stats.used, totalFree, stats.holeBytes, stats.numHoles, endFree, stats.largestFree,
    fragmented ? " -- FRAGMENTED" : "");
}

static void sim_account(uint16_t oldp, uint16_t newp, uint16_t oldsz)
{
  if (oldp) {
    sim_stats.used -= oldsz + SIM_HDR;
  }
  if (newp) {
    sim_stats.used += sz(newp - SIM_HDR) + SIM_HDR;
  }
  uint32_t extent = sim_brkval ? sim_brkval - SIM_HEAP_START : 0;
  if (extent > sim_stats.peakExtent) {
    sim_stats.peakExtent = extent;
  }
  if (sim_stats.used > sim_stats.peakUsed) {
    sim_stats.peakUsed = sim_stats.used;
  }
}

// every host allocation is prefixed with this, sim is 0 if not tracked
struct HostBlock
{
  uint32_t magic;
  uint16_t sim;
  uint16_t unused;
  uint64_t size;
};
static_assert(sizeof(HostBlock) == 16, "HostBlock must keep 16 byte alignment");
#define HOST_MAGIC 0x48454150

static HostBlock *host_block(void *ptr)
{
  HostBlock *block = (HostBlock *)ptr - 1;
  if (block->magic != HOST_MAGIC) {
    fprintf(stderr, "HEAP: bad pointer %p\n", ptr);
    abort();
  }
  return block;
}

static void *host_alloc(size_t size, uint16_t simp)
{
  HostBlock *block = (HostBlock *)malloc(sizeof(HostBlock) + size);
  if (!block) {
    return nullptr;
  }
  block->magic = HOST_MAGIC;
  block->sim = simp;
  block->unused = 0;
  block->size = size;
  return block + 1;
}

// allocate size host bytes that cost avrSize bytes on the device
static void *heap_alloc(size_t size, size_t avrSize)
{
  uint16_t simp = 0;
  if (sim_tracking) {
    if (avrSize > 0xFFFF || !(simp = sim_malloc((uint16_t)avrSize))) {
      sim_failed((uint32_t)avrSize);
      return nullptr;
    }
    sim_account(0, simp, 0);
  }
  void *ptr = host_alloc(size, simp);
  if (!ptr && simp) {
    sim_free(simp);
  }
  return ptr;
}

void *heap_malloc(size_t size)
{
  return heap_alloc(size, size);
}

void *heap_calloc(size_t count, size_t size)
{
  // arrays of pointers are the only arrays whose size differs on the device
  size_t avrSize = count * ((size == sizeof(void *)) ? 2 : size);
  void *ptr = heap_alloc(count * size, avrSize);
  if (ptr) {
    memset(ptr, 0, count * size);
  }
  return ptr;
}

void *heap_realloc(void *ptr, size_t size)
{
  if (!ptr) {
    return heap_malloc(size);
  }
  HostBlock *block = host_block(ptr);
  uint16_t simp = block->sim;
  if (simp) {
    uint16_t oldsz = sz(simp - SIM_HDR);
    if (size > 0xFFFF || !(simp = sim_realloc(simp, (uint16_t)size))) {
      sim_failed((uint32_t)size);
      return nullptr;
    }
    sim_account(block->sim, simp, oldsz);
  }
  void *newptr = host_alloc(size, simp);
  if (!newptr) {
    return nullptr;
  }
  memcpy(newptr, ptr, (block->size < size) ? block->size : size);
  free(block);
  return newptr;
}

void heap_free(void *ptr)
{
  if (!ptr) {
    return;
  }
  HostBlock *block = host_block(ptr);
  if (block->sim) {
    uint16_t oldsz = sz(block->sim - SIM_HDR);
    sim_free(block->sim);
    sim_account(block->sim, 0, oldsz);
  }
  block->magic = 0;
  free(block);
}

bool heap_setTracking(bool enabled)
{
  bool prev = sim_tracking;
  sim_tracking = enabled;
  return prev;
}

void heap_setMaxSize(uint32_t size)
{
  sim_maxSize = size;
}

uint32_t heap_maxSize()
{
  return sim_maxSize;
}

// the device size of an object allocated with new
static size_t avr_new_size(size_t size)
{
  if (!sim_tracking) {
    return size;
  }
  // if more than one class has this host size then use the smallest device
  // size so the simulated heap never claims more than the device would use
  size_t avrSize = 0;
  for (size_t i = 0; i < sizeof(avrSizes) / sizeof(avrSizes[0]); ++i) {
    if (avrSizes[i].host == size && (!avrSize || avrSizes[i].avr < avrSize)) {
      avrSize = avrSizes[i].avr;
    }
  }
  if (!avrSize) {
    // only warn once for each unknown size
    static uint8_t warned[0x1000 / 8];
    if (size < 0x1000 && !(warned[size / 8] & (1 << (size % 8)))) {
      warned[size / 8] |= 1 << (size % 8);
      fprintf(stderr, "HEAP: unknown class of size %zu allocated, run VortexLib/gen_avr_sizes.sh\n", size);
    }
    return size;
  }
  return avrSize;
}

// new and delete for everything in the process have to go through the
// tracker so that delete can tell which heap a pointer came from, only
// allocations made by the engine while tracking is enabled are simulated
void *operator new  (size_t size)
{
  void *ptr = heap_alloc(size, avr_new_size(size));
  if (!ptr && !sim_tracking) {
    throw std::bad_alloc();
  }
  // tracked allocations return null on failure like the device, the cli is
  // built with -fcheck-new so the engine sees the null instead of crashing
  return ptr;
}
void *operator new[](size_t size) { return operator new(size); }
void  operator delete  (void *ptr) noexcept { heap_free(ptr); }
void  operator delete[](void *ptr) noexcept { heap_free(ptr); }
void  operator delete  (void *ptr, size_t size) noexcept { heap_free(ptr); }
void  operator delete[](void *ptr, size_t size) noexcept { heap_free(ptr); }

#endif // HEAP_TRACKING == 1

// everything in here is only really used when allocations are being debugged
#if DEBUG_ALLOCATIONS == 1

static uint32_t cur_mem_usage = 0;
static uint32_t background_usage = 0;

struct memory_block
{
  uint32_t size;
  uint8_t p[];
};

// Vortex allocation functions
void *_vmalloc(uint32_t size)
{
  if ((cur_memory_usage_total() + size) >= MAX_MEMORY) {
    DEBUG_LOG("OVERMEM");
    return nullptr;
  }
  memory_block *b = (memory_block *)malloc(size + sizeof(memory_block));
  if (!b) {
    return nullptr;
  }
  b->size = size;
  cur_mem_usage += b->size;
  background_usage += sizeof(memory_block);
  //DEBUG_LOGF("malloc(): %u (%u) (%u)", b->size, cur_memory_usage, background_usage);
  return b->p;
}

void *_vcalloc(uint32_t size, uint32_t amount)
{
  if ((cur_memory_usage_total() + (size * amount)) >= MAX_MEMORY) {
    DEBUG_LOG("OVERMEM");
    return nullptr;
  }
  uint32_t real_amount = (size * amount);
  memory_block *b = (memory_block *)calloc(1, real_amount + sizeof(memory_block));
  if (!b) {
    return nullptr;
  }
  b->size = real_amount;
  cur_mem_usage += b->size;
  background_usage += sizeof(memory_block);
  //DEBUG_LOGF("calloc(): %u (%u) (%u)", b->size, cur_mem_usage, background_usage);
  return b->p;
}

static memory_block *get_base(void *b)
{
  return (memory_block *)(((uintptr_t)b) - sizeof(memory_block));
}

void *_vrealloc(void *ptr, uint32_t size)
{
  uint32_t old_size = 0;
  memory_block *base = (memory_block *)ptr;
  if (ptr) {
    base = get_base(ptr);
    old_size = base->size;
  }
  // same?
  if (size == old_size) {
    return ptr;
  }
  if ((cur_memory_usage_total() - old_size) + size >= MAX_MEMORY) {
    DEBUG_LOG("OVERMEM");
    return nullptr;
  }
  //DEBUG_LOGF("realloc(): %u -> %u (%u) (%u)", old_size, size, (cur_mem_usage - old_size) + size, background_usage);
  memory_block *b = (memory_block *)realloc(base, size + sizeof(memory_block));
  if (!b) {
    return nullptr;
  }
  b->size = size;
  cur_mem_usage -= old_size;
  cur_mem_usage += b->size;
  if (!old_size) {
    background_usage += sizeof(memory_block);
  }
  return b->p;
}

void _vfree(void *ptr)
{
  if (!ptr) {
    return;
  }
  memory_block *base = get_base(ptr);
  cur_mem_usage -= base->size;
  background_usage -= sizeof(memory_block);
  //DEBUG_LOGF("free(): %u (%u) (%u)", base->size, cur_mem_usage, background_usage);
  free(base);
}

uint32_t cur_memory_usage()
{
  return cur_mem_usage;
}

uint32_t cur_memory_usage_background()
{
  return background_usage;
}

uint32_t cur_memory_usage_total()
{
  return cur_mem_usage + background_usage;
}

#endif // DEBUG_ALLOCATIONS == 1

#ifndef VORTEX_LIB

// for C++11 need the following:
void *operator new  (size_t size) { return vmalloc(size); }
void *operator new[](size_t size) { return vmalloc(size); }
void  operator delete  (void *ptr) { vfree(ptr); }
void  operator delete[](void *ptr) { vfree(ptr); }
void *operator new  (size_t size, void *ptr) noexcept { return ptr; }
void *operator new[](size_t size, void *ptr) noexcept { return ptr; }
void  operator delete  (void *ptr, size_t size) noexcept { vfree(ptr); }
void  operator delete[](void *ptr, size_t size) noexcept { vfree(ptr); }
//void *operator new  (size_t size, std::align_val_t al) { return vmalloc(size); }
//void *operator new[](size_t size, std::align_val_t al) { return vmalloc(size); }
//void  operator delete  (void *ptr, std::align_val_t al) noexcept { vfree(ptr); }
//void  operator delete[](void *ptr, std::align_val_t al) noexcept { vfree(ptr); }
//void  operator delete  (void *ptr, size_t size, std::align_val_t al) noexcept { vfree(ptr); }
//void  operator delete[](void *ptr, size_t size, std::align_val_t al) noexcept { vfree(ptr); }

// needed for C++ virtual functions
extern "C" void __cxa_pure_virtual(void) {}
extern "C" void __cxa_deleted_virtual(void) {}

#endif
