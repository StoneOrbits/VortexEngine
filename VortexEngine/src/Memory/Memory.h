#ifndef MEMORY_H
#define MEMORY_H

#include <inttypes.h>
#include <stdbool.h>
#include <stdlib.h>

#include "../VortexConfig.h"

#if DEBUG_ALLOCATIONS == 1

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

// A fixed block memory pool, every block in the pool is the same size so
// allocating and freeing can never fragment memory. The backing storage is
// a static array (see MEMPOOL_STORAGE) so the memory used by every pool is
// reserved at build time and shows up in the size of the program's globals,
// blocks are handed out and returned through an intrusive free list.

// blocks are rounded up to the size of a pointer, this keeps every block
// aligned for any struct and leaves room for the free list link
#define MEMPOOL_ALIGN(size) ((((size) + sizeof(void *) - 1) / sizeof(void *)) * sizeof(void *))

// declare the static storage for a pool of numBlocks blocks of blockSize,
// it is declared as an array of pointers so that it is pointer aligned
#define MEMPOOL_STORAGE(name, blockSize, numBlocks) \
  void *name[(MEMPOOL_ALIGN(blockSize) * (numBlocks)) / sizeof(void *)]
typedef struct MemPool
{
  // the backing storage of all blocks
  uint8_t *m_storage;
  // the list of free blocks, each free block stores the next free block
  void *m_freeList;
  // the size of each block (rounded up for alignment)
  uint16_t m_blockSize;
  // the total number of blocks and the number currently in use
  uint8_t m_numBlocks;
  uint8_t m_numUsed;
  // the most blocks that were ever in use at once
  uint8_t m_highWater;
} MemPool;

// setup the pool in the given storage, this is only done once and
// calling it again on an initialized pool does nothing
void MemPool_init(MemPool *self, void *storage, uint16_t blockSize, uint8_t numBlocks);
// grab a block from the pool, returns NULL if the pool is exhausted
void *MemPool_alloc(MemPool *self);
// return a block to the pool
void MemPool_free(MemPool *self, void *ptr);
// whether a pointer is a block of this pool
bool MemPool_contains(const MemPool *self, const void *ptr);

#endif
