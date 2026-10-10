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

void *MemPool_alloc(MemPool *self)
{
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
  return block;
}

void MemPool_free(MemPool *self, void *ptr)
{
  if (!ptr) {
    return;
  }
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

#ifndef VORTEX_LIB

// needed for C++ virtual functions
extern "C" void __cxa_pure_virtual(void) {}
extern "C" void __cxa_deleted_virtual(void) {}

#endif
