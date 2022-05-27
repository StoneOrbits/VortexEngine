#include "Memory.h"

#include <stdlib.h>
#include <stdio.h>

#include "Log.h"

static uint32_t cur_mem_usage = 0;
static uint32_t background_usage = 0;

#define MAX_MEMORY 50000

struct memory_block
{
  uint32_t size;
  uint8_t p[];
};

// Vortex allocation functions
void *_vmalloc(uint32_t size)
{
  if ((cur_mem_usage + size) >= MAX_MEMORY) {
    DEBUG("OVERMEM");
    return nullptr;
  }
  memory_block *b = (memory_block *)malloc(size + sizeof(memory_block));
  if (!b) {
    return nullptr;
  }
  b->size = size;
  cur_mem_usage += b->size;
  background_usage += sizeof(memory_block);
  //DEBUGF("malloc(): %u (%u) (%u)", b->size, cur_memory_usage, background_usage);
  return b->p;
}

void *_vcalloc(uint32_t size, uint32_t amount)
{
  if ((cur_mem_usage + (size * amount)) >= MAX_MEMORY) {
    DEBUG("OVERMEM");
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
  //DEBUGF("calloc(): %u (%u) (%u)", b->size, cur_mem_usage, background_usage);
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
  if ((cur_mem_usage - old_size) + size >= MAX_MEMORY) {
    DEBUG("OVERMEM");
    return nullptr;
  }
  //DEBUGF("realloc(): %u -> %u (%u) (%u)", old_size, size, (cur_mem_usage - old_size) + size, background_usage);
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
  //DEBUGF("free(): %u (%u) (%u)", base->size, cur_mem_usage, background_usage);
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
