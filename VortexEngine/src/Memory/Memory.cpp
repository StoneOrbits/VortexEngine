#include "Memory.h"

#include <stdlib.h>
#include <stdio.h>

#include "../Log/Log.h"

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

void MemPool_init(MemPool *self, void *storage, uint16_t blockSize, uint8_t numBlocks)
{
  if (self->m_storage) {
    // already initialized, blocks may still be in use so leave it be
    return;
  }
  self->m_storage = (uint8_t *)storage;
  self->m_blockSize = MEMPOOL_ALIGN(blockSize);
  self->m_numBlocks = numBlocks;
  self->m_numUsed = 0;
  self->m_highWater = 0;
  self->m_freeList = NULL;
  // thread every block onto the free list, last block first so the
  // first block ends up at the front of the list
  for (uint8_t i = numBlocks; i > 0; --i) {
    void **block = (void **)(self->m_storage + ((uint32_t)self->m_blockSize * (i - 1)));
    *block = self->m_freeList;
    self->m_freeList = block;
  }
}

#if HEAP_TRACKING == 1
static bool sim_tracking = false;
static uint32_t sim_maxSize = 0;
// device bytes used by tracked arena chunks and the most ever used
static uint32_t sim_arenaUsed = 0;
static uint32_t sim_arenaPeak = 0;
// the list of tracked pools
static MemPool *sim_pools = NULL;

static uint8_t MemPool_index(const MemPool *self, const void *ptr)
{
  return (uint8_t)(((const uint8_t *)ptr - self->m_storage) / self->m_blockSize);
}
#endif

void *MemPool_alloc(MemPool *self)
{
#if HEAP_TRACKING == 1
  bool tracked = sim_tracking && self->m_tracked;
  if (tracked && sim_maxSize && self->m_trackedUsed >= self->m_deviceBlocks) {
    fprintf(stderr, "HEAP: failed to allocate from the %s pool, all %u device blocks are in use\n",
      self->m_name, self->m_deviceBlocks);
    ERROR_OUT_OF_MEMORY();
    return NULL;
  }
#endif
  void **block = (void **)self->m_freeList;
  if (!block) {
    ERROR_OUT_OF_MEMORY();
    return NULL;
  }
  self->m_freeList = *block;
  self->m_numUsed++;
  if (self->m_numUsed > self->m_highWater) {
    self->m_highWater = self->m_numUsed;
  }
#if HEAP_TRACKING == 1
  if (tracked) {
    self->m_tracked[MemPool_index(self, block)] = 1;
    self->m_trackedUsed++;
    if (self->m_trackedUsed > self->m_trackedPeak) {
      self->m_trackedPeak = self->m_trackedUsed;
    }
  }
#endif
  return block;
}

void MemPool_free(MemPool *self, void *ptr)
{
  if (!ptr) {
    return;
  }
#if HEAP_TRACKING == 1
  // a tracked block counts until it is freed even if tracking is off now
  if (self->m_tracked && self->m_tracked[MemPool_index(self, ptr)]) {
    self->m_tracked[MemPool_index(self, ptr)] = 0;
    self->m_trackedUsed--;
  }
#endif
  void **block = (void **)ptr;
  *block = self->m_freeList;
  self->m_freeList = block;
  self->m_numUsed--;
}

bool MemPool_contains(const MemPool *self, const void *ptr)
{
  const uint8_t *p = (const uint8_t *)ptr;
  return self->m_storage && p >= self->m_storage &&
    p < self->m_storage + ((uint32_t)self->m_blockSize * self->m_numBlocks);
}

#if HEAP_TRACKING == 1

void MemPool_track(MemPool *self, const char *name, uint8_t deviceBlocks)
{
  if (self->m_tracked) {
    // already tracked
    return;
  }
  self->m_name = name;
  self->m_deviceBlocks = deviceBlocks;
  self->m_tracked = (uint8_t *)calloc(self->m_numBlocks, 1);
  self->m_nextTracked = sim_pools;
  sim_pools = self;
}

bool heap_setTracking(bool enabled)
{
  bool prev = sim_tracking;
  sim_tracking = enabled;
  return prev;
}

bool heap_tracking()
{
  return sim_tracking;
}

void heap_setMaxSize(uint32_t size)
{
  sim_maxSize = size;
}

uint32_t heap_maxSize()
{
  return sim_maxSize;
}

bool heap_arenaResize(uint32_t oldSize, uint32_t newSize)
{
  uint32_t used = sim_arenaUsed - oldSize + newSize;
  if (newSize > oldSize && sim_maxSize && used > sim_maxSize) {
    fprintf(stderr, "HEAP: failed to allocate %u bytes in the ByteStream arena with %u of %u used\n",
      newSize - oldSize, sim_arenaUsed, sim_maxSize);
    return false;
  }
  sim_arenaUsed = used;
  if (sim_arenaUsed > sim_arenaPeak) {
    sim_arenaPeak = sim_arenaUsed;
  }
  return true;
}

void heap_printStats(const char *label)
{
  fprintf(stderr, "HEAP [%s]: arena used %u (peak %u) of ", label, sim_arenaUsed, sim_arenaPeak);
  if (sim_maxSize) {
    fprintf(stderr, "%u", sim_maxSize);
  } else {
    fprintf(stderr, "unlimited");
  }
  fprintf(stderr, ", pool blocks used (peak) of device blocks:");
  for (MemPool *pool = sim_pools; pool; pool = pool->m_nextTracked) {
    fprintf(stderr, " %s %u (%u) of %u%s", pool->m_name, pool->m_trackedUsed,
      pool->m_trackedPeak, pool->m_deviceBlocks, pool->m_nextTracked ? "," : "");
  }
  fprintf(stderr, "\n");
}

#endif // HEAP_TRACKING == 1

#ifndef VORTEX_LIB

// needed for C++ virtual functions
extern "C" void __cxa_pure_virtual(void) {}
extern "C" void __cxa_deleted_virtual(void) {}

#endif
