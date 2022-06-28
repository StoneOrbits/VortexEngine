#ifndef MEMORY_H
#define MEMORY_H

#include <inttypes.h>
#include <stdlib.h>

// uncomment me to debug allocations
// NOTE: tracking allocations while using the test framework will be inaccurate
//       because the test framework allocations will be caught as well. Doing 
//       things like displaying a color for the first time will allocate space
//       and never really return it
#define DEBUG_ALLOCATIONS

#ifndef DEBUG_ALLOCATIONS

#define vmalloc(size) malloc(size)
#define vcalloc(size, amount) calloc(size, amount)
#define vrealloc(ptr, size) realloc(ptr, size)
#define vfree(ptr) free(ptr)

#else

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

void *operator new(size_t size);
void operator delete(void *ptr) noexcept;
void *operator new[](size_t size);
void operator delete[](void *ptr) noexcept;

#endif

#endif
