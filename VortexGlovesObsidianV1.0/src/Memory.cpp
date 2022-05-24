#include "Memory.h"

#include <stdlib.h>
#include <stdio.h>

#include "Log.h"

uint32_t cur_mem_usage = 0;

struct memory_block
{
  uint32_t size;
  uint8_t p[];
};

// Vortex allocation functions
void *_vmalloc(uint32_t size)
{
  memory_block *b = (memory_block *)malloc(size + sizeof(uint32_t));
  if (!b) {
    return nullptr;
  }
  b->size = size;
  cur_mem_usage += b->size;
  return b->p;
}

void *_vcalloc(uint32_t size, uint32_t amount)
{
  uint32_t real_amount = (size * amount);
  memory_block *b = (memory_block *)calloc(1, real_amount + sizeof(uint32_t));
  if (!b) {
    return nullptr;
  }
  b->size = real_amount;
  cur_mem_usage += b->size;
  return b->p;
}

void *_vrealloc(void *ptr, uint32_t size)
{
  uint32_t old_size = 0;
  memory_block *base = (memory_block *)ptr;
  if (ptr) {
    base = (memory_block *)(((uintptr_t)ptr) - sizeof(uint32_t));
    old_size = base->size;
  }
  memory_block *b = (memory_block *)realloc(base, size + sizeof(uint32_t));
  if (!b) {
    return nullptr;
  }
  b->size = size;
  cur_mem_usage -= old_size;
  cur_mem_usage += b->size;
  return b->p;
}

void _vfree(void *ptr)
{
  if (!ptr) {
    return;
  }
  memory_block *base = (memory_block *)(((uintptr_t)ptr) - sizeof(uint32_t));
  cur_mem_usage -= base->size;
  free(base);
}

uint32_t cur_memory_usage()
{
  return cur_mem_usage;
}

#ifdef DEBUG_ALLOCATIONS
void *operator new(size_t size)
{
  return _vmalloc(size);
}

void operator delete(void *ptr)
{
  _vfree(ptr);
}

void *operator new[](size_t size)
{
  return _vmalloc(size);
}

void operator delete[](void *ptr)
{
  _vfree(ptr);
}
#endif
