#ifndef MEMORY_H
#define MEMORY_H

#include <inttypes.h>
#include <stdlib.h>

#include "../VortexConfig.h"

#if DEBUG_ALLOCATIONS == 1

#define vmalloc(size) _vmalloc(size)
#define vcalloc(size, amount) _vcalloc(size, amount)
#define vrealloc(ptr, size) _vrealloc(ptr, size)
#define vfree(ptr) _vfree(ptr)

void *_vmalloc(uint32_t size);
void *_vcalloc(uint32_t size, uint32_t amount);
void *_vrealloc(void *ptr, uint32_t size);
void _vfree(void *ptr);

uint32_t cur_memory_usage();
uint32_t cur_memory_usage_background();
uint32_t cur_memory_usage_total();

#else

#define vmalloc(size) malloc(size)
#define vcalloc(size, amount) calloc(size, amount)
#define vrealloc(ptr, size) realloc(ptr, size)
#define vfree(ptr) free(ptr)

#endif

#endif
