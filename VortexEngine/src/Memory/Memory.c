#include "Memory.h"

#include <stdlib.h>
#include <stdio.h>

#include "../Log/Log.h"

#if DEBUG_ALLOCATIONS == 1

static uint32_t cur_mem_usage = 0;
static uint32_t background_usage = 0;

struct memory_block
{
  uint32_t size;
  uint8_t p[];
};

void *_vmalloc(uint32_t size)
{
  if ((cur_memory_usage_total() + size) >= MAX_MEMORY) {
    DEBUG_LOG("OVERMEM");
    return NULL;
  }
  struct memory_block *b = (struct memory_block *)malloc(size + sizeof(struct memory_block));
  if (!b) {
    return NULL;
  }
  b->size = size;
  cur_mem_usage += b->size;
  background_usage += sizeof(struct memory_block);
  return b->p;
}

void *_vcalloc(uint32_t size, uint32_t amount)
{
  if ((cur_memory_usage_total() + (size * amount)) >= MAX_MEMORY) {
    DEBUG_LOG("OVERMEM");
    return NULL;
  }
  uint32_t real_amount = (size * amount);
  struct memory_block *b = (struct memory_block *)calloc(1, real_amount + sizeof(struct memory_block));
  if (!b) {
    return NULL;
  }
  b->size = real_amount;
  cur_mem_usage += b->size;
  background_usage += sizeof(struct memory_block);
  return b->p;
}

static struct memory_block *get_base(void *b)
{
  return (struct memory_block *)(((uintptr_t)b) - sizeof(struct memory_block));
}

void *_vrealloc(void *ptr, uint32_t size)
{
  uint32_t old_size = 0;
  struct memory_block *base = (struct memory_block *)ptr;
  if (ptr) {
    base = get_base(ptr);
    old_size = base->size;
  }
  if (size == old_size) {
    return ptr;
  }
  if ((cur_memory_usage_total() - old_size) + size >= MAX_MEMORY) {
    DEBUG_LOG("OVERMEM");
    return NULL;
  }
  struct memory_block *b = (struct memory_block *)realloc(base, size + sizeof(struct memory_block));
  if (!b) {
    return NULL;
  }
  b->size = size;
  cur_mem_usage -= old_size;
  cur_mem_usage += b->size;
  if (!old_size) {
    background_usage += sizeof(struct memory_block);
  }
  return b->p;
}

void _vfree(void *ptr)
{
  if (!ptr) {
    return;
  }
  struct memory_block *base = get_base(ptr);
  cur_mem_usage -= base->size;
  background_usage -= sizeof(struct memory_block);
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

#endif
