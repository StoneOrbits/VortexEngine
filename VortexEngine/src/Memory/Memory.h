#ifndef MEMORY_H
#define MEMORY_H

#include <inttypes.h>
#include <stdlib.h>

#include "../VortexConfig.h"

#if HEAP_TRACKING == 1

// allocations go through the simulated attiny3217 heap, see Memory.cpp
#define vmalloc(size) heap_malloc(size)
#define vcalloc(size, amount) heap_calloc(size, amount)
#define vrealloc(ptr, size) heap_realloc(ptr, size)
#define vfree(ptr) heap_free(ptr)

void *heap_malloc(size_t size);
void *heap_calloc(size_t count, size_t size);
void *heap_realloc(void *ptr, size_t size);
void heap_free(void *ptr);

// Only allocations made while tracking is enabled are placed in the simulated
// heap, this is turned on while the engine itself runs so that allocations by
// the vortexlib or cli (strings, containers, undo buffer) aren't counted
bool heap_setTracking(bool enabled);

// limit the simulated heap to this many bytes, 0 is unlimited
void heap_setMaxSize(uint32_t size);
uint32_t heap_maxSize();

struct HeapStats
{
  // bytes the engine has allocated right now, including the 2 byte chunk headers
  uint32_t used;
  // the end of the heap (__brkval), this is used plus any free holes below it
  uint32_t extent;
  // the highest extent ever reached
  uint32_t peakExtent;
  // the highest used ever reached
  uint32_t peakUsed;
  // free bytes in holes below the end of the heap
  uint32_t holeBytes;
  uint32_t numHoles;
  // the biggest single allocation that would succeed right now
  uint32_t largestFree;
  // allocations that failed, and how many of those failed only because the
  // free space was split into pieces that were each too small
  uint32_t failures;
  uint32_t fragmentedFailures;
};
void heap_getStats(HeapStats &stats);
void heap_printStats(const char *label);

// enables or disables heap tracking for a scope then restores it
class HeapTrackScope
{
public:
  HeapTrackScope(bool enabled) : m_prev(heap_setTracking(enabled)) {}
  ~HeapTrackScope() { heap_setTracking(m_prev); }
private:
  bool m_prev;
};
#define HEAP_TRACK_SCOPE(enabled) HeapTrackScope heapTrackScope(enabled)

#elif DEBUG_ALLOCATIONS == 1

// monitored allocation routines
#define vmalloc(size) _vmalloc(size)
#define vcalloc(size, amount) _vcalloc(size, amount)
#define vrealloc(ptr, size) _vrealloc(ptr, size)
#define vfree(ptr) _vfree(ptr)

// Vortex allocation functions
void *_vmalloc(uint32_t size);
void *_vcalloc(uint32_t size, uint32_t amount);
void *_vrealloc(void *ptr, uint32_t size);
void _vfree(void *ptr);

// memory used by regular code
uint32_t cur_memory_usage();
// memory used by the memory tracker itself
uint32_t cur_memory_usage_background();
// memory used by everything
uint32_t cur_memory_usage_total();

#else // if DEBUG_ALLOCATIONS

#define vmalloc(size) malloc(size)
#define vcalloc(size, amount) calloc(size, amount)
#define vrealloc(ptr, size) realloc(ptr, size)
#define vfree(ptr) free(ptr)

#endif

#if HEAP_TRACKING != 1
#define HEAP_TRACK_SCOPE(enabled)
#endif

#ifndef VORTEX_LIB
void *operator new  (size_t size);
void *operator new[](size_t size);
void  operator delete  (void *ptr);
void  operator delete[](void *ptr);
void *operator new  (size_t size, void *ptr) noexcept;
void *operator new[](size_t size, void *ptr) noexcept;
void  operator delete  (void *ptr, size_t size) noexcept;
void  operator delete[](void *ptr, size_t size) noexcept;
//void *operator new  (size_t size, std::align_val_t al);
//void *operator new[](size_t size, std::align_val_t al);
//void  operator delete  (void *ptr, std::align_val_t al) noexcept;
//void  operator delete[](void *ptr, std::align_val_t al) noexcept;
//void  operator delete  (void *ptr, size_t size, std::align_val_t al) noexcept;
//void  operator delete[](void *ptr, size_t size, std::align_val_t al) noexcept;
#endif

#endif
