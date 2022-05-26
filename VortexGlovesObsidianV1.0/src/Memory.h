#ifndef MEMORY_H
#define MEMORY_H

#include <inttypes.h>
#include <stdlib.h>

// uncomment me to debug allocations
#define DEBUG_ALLOCATIONS

#ifndef DEBUG_ALLOCATIONS

// default memory allocation routines
#include <stdlib.h>
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

uint32_t cur_memory_usage();

void *operator new(size_t size);
void operator delete(void *ptr);
void *operator new[](size_t size);
void operator delete[](void *ptr);
#endif
#endif
