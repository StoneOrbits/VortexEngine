#include "ByteStream.h"

#include "../Serial/BitStream.h"
#include "../Memory/Memory.h"
#include "../Log/Log.h"

#include <string.h>

#ifdef VORTEX_EMBEDDED
#include <avr/io.h>
#endif

#include "Compression.h"

// flags for saving buffer to disk
#define BUFFER_FLAG_COMRPESSED (1<<0) // buffer is compressed
#define BUFFER_FLAG_DIRTY      (1<<1) // buffer crc is dirty

// ===================================================================
//  ByteStream Arena
//
// The data of every ByteStream lives in a single arena. Each buffer is a
// chunk in the arena that records which
// ByteStream owns it, the chunks are always packed together at the front
// of the arena with all of the free space in one block at the end. When a
// chunk is released or resized the chunks after it are slid along to close
// or open the gap and their owners are updated to point at the new spot.
// This means the arena can never fragment, the only way an allocation can
// fail is if the total size of all live buffers exceeds the arena size.
//
// On the device the arena is all of the free ram between the end of the
// static data and the stack, exactly where the heap used to be. It grows
// up towards the stack and refuses to come within a margin of the stack
// pointer, the same way malloc does, so the stack and the ByteStreams share
// the free ram instead of each reserving their worst case.
//
// The catch is a ByteStream must not move in memory while it owns a buffer
// and it must be destroyed (or cleared) before it goes out of scope, and
// raw pointers into a buffer are only valid until another ByteStream is
// initialized, resized or cleared.

#ifdef VORTEX_LIB
typedef uint32_t arena_size_t;
#else
typedef uint16_t arena_size_t;
#endif

typedef struct ArenaChunk
{
  // the ByteStream that owns this chunk
  ByteStream *owner;
  // the total size of this chunk including this header
  arena_size_t size;
#if HEAP_TRACKING == 1
  // the size of this chunk on the device, 0 if it isn't tracked
  arena_size_t deviceSize;
#endif
} ArenaChunk;

// chunks are rounded up to keep every chunk header and RawBuffer aligned
#define ARENA_ALIGN(size) ((((size) + sizeof(void *) - 1) / sizeof(void *)) * sizeof(void *))
#define ARENA_CHUNK_HEADER ARENA_ALIGN(sizeof(ArenaChunk))
// the total chunk size for a buffer of the given capacity
#define ARENA_CHUNK_SIZE(capacity) ARENA_ALIGN(ARENA_CHUNK_HEADER + sizeof(RawBuffer) + (capacity))

#if HEAP_TRACKING == 1
// the chunk size on the device, the header is a 2 byte pointer and a 2 byte
// size and chunks are aligned to 2 bytes, the RawBuffer is the same size
#define ARENA_DEVICE_CHUNK_SIZE(capacity) (((4 + sizeof(RawBuffer) + (capacity)) + 1) & ~1)
#endif

#ifdef VORTEX_EMBEDDED
// the end of the static data, provided by the linker
extern uint8_t __heap_start;
#else
// the storage for the arena, declared as pointers so it is pointer aligned
static void *s_arenaStorage[ARENA_ALIGN(BYTESTREAM_ARENA_SIZE) / sizeof(void *)];
#endif
static uint8_t *s_arena = NULL;
static arena_size_t s_arenaUsed = 0;
static arena_size_t s_arenaHighWater = 0;

static ArenaChunk *ByteStream_chunk(const RawBuffer *buf)
{
  return (ArenaChunk *)((uint8_t *)buf - ARENA_CHUNK_HEADER);
}

static RawBuffer *ByteStream_chunkData(ArenaChunk *chunk)
{
  return (RawBuffer *)((uint8_t *)chunk + ARENA_CHUNK_HEADER);
}

bool ByteStream_initArena()
{
  if (s_arena) {
    // already initialized, buffers may still be in use so leave it be
    return true;
  }
#ifdef VORTEX_EMBEDDED
  s_arena = &__heap_start;
#else
  s_arena = (uint8_t *)s_arenaStorage;
#endif
  s_arenaUsed = 0;
  s_arenaHighWater = 0;
  return true;
}

uint32_t ByteStream_arenaUsed()
{
  return s_arenaUsed;
}

uint32_t ByteStream_arenaHighWater()
{
  return s_arenaHighWater;
}

// the amount of free space left in the arena
static arena_size_t ByteStream_arenaFree()
{
#ifdef VORTEX_EMBEDDED
  // the arena can grow up to the margin below the current stack pointer
  // but never into the space reserved for the deepest the stack can go
  uint8_t *limit = (uint8_t *)SP - BYTESTREAM_STACK_MARGIN;
  uint8_t *reserve = (uint8_t *)(RAMEND + 1 - BYTESTREAM_STACK_RESERVE);
  if (limit > reserve) {
    limit = reserve;
  }
  uint8_t *end = s_arena + s_arenaUsed;
  return (limit > end) ? (arena_size_t)(limit - end) : 0;
#else
  return BYTESTREAM_ARENA_SIZE - s_arenaUsed;
#endif
}

// resize the chunk to newSize by sliding every chunk after it, a newSize
// of 0 removes the chunk entirely, returns false if there isn't room
static bool ByteStream_resizeChunk(ArenaChunk *chunk, arena_size_t newSize)
{
  arena_size_t oldSize = chunk->size;
  if (newSize > oldSize && (newSize - oldSize) > ByteStream_arenaFree()) {
    ERROR_OUT_OF_MEMORY();
    return false;
  }
  uint8_t *end = (uint8_t *)chunk + oldSize;
  arena_size_t tail = (arena_size_t)((s_arena + s_arenaUsed) - end);
  // slide the following chunks to their new spot
  memmove((uint8_t *)chunk + newSize, end, tail);
  s_arenaUsed = s_arenaUsed - oldSize + newSize;
  if (s_arenaUsed > s_arenaHighWater) {
    s_arenaHighWater = s_arenaUsed;
  }
  uint8_t *next = (uint8_t *)chunk + newSize;
  if (newSize) {
    chunk->size = newSize;
  }
  // then point the owners of the chunks that moved at their new data
  while (next < s_arena + s_arenaUsed) {
    ArenaChunk *moved = (ArenaChunk *)next;
    moved->owner->m_pData = ByteStream_chunkData(moved);
    next += moved->size;
  }
  return true;
}

// allocate a buffer from the end of the arena for the given owner
static RawBuffer *ByteStream_allocBuffer(ByteStream *owner, uint32_t capacity)
{
  arena_size_t size = ARENA_CHUNK_SIZE(capacity);
  if (!s_arena || size > ByteStream_arenaFree()) {
    ERROR_OUT_OF_MEMORY();
    return NULL;
  }
#if HEAP_TRACKING == 1
  arena_size_t deviceSize = heap_tracking() ? ARENA_DEVICE_CHUNK_SIZE(capacity) : 0;
  if (!heap_arenaResize(0, deviceSize)) {
    ERROR_OUT_OF_MEMORY();
    return NULL;
  }
#endif
  ArenaChunk *chunk = (ArenaChunk *)(s_arena + s_arenaUsed);
  chunk->owner = owner;
  chunk->size = size;
#if HEAP_TRACKING == 1
  chunk->deviceSize = deviceSize;
#endif
  s_arenaUsed += size;
  if (s_arenaUsed > s_arenaHighWater) {
    s_arenaHighWater = s_arenaUsed;
  }
  return ByteStream_chunkData(chunk);
}

// release a buffer back to the arena, this compacts the arena
static void ByteStream_freeBuffer(RawBuffer *buf)
{
#if HEAP_TRACKING == 1
  heap_arenaResize(ByteStream_chunk(buf)->deviceSize, 0);
#endif
  ByteStream_resizeChunk(ByteStream_chunk(buf), 0);
}

// resize a buffer in place to hold the given capacity, this may move
// any buffer in the arena (including this one) so the owners are updated
static bool ByteStream_resizeBuffer(ByteStream *owner, uint32_t capacity)
{
#if HEAP_TRACKING == 1
  // a chunk stays tracked or untracked for as long as it lives
  ArenaChunk *chunk = ByteStream_chunk(owner->m_pData);
  arena_size_t oldDeviceSize = chunk->deviceSize;
  arena_size_t deviceSize = oldDeviceSize ? ARENA_DEVICE_CHUNK_SIZE(capacity) : 0;
  if (!heap_arenaResize(oldDeviceSize, deviceSize)) {
    ERROR_OUT_OF_MEMORY();
    return false;
  }
  if (!ByteStream_resizeChunk(chunk, ARENA_CHUNK_SIZE(capacity))) {
    heap_arenaResize(deviceSize, oldDeviceSize);
    return false;
  }
  chunk->deviceSize = deviceSize;
  return true;
#else
  return ByteStream_resizeChunk(ByteStream_chunk(owner->m_pData), ARENA_CHUNK_SIZE(capacity));
#endif
}

void ByteStream_make(ByteStream *self)
{
  self->m_pData = NULL;
  self->m_position = 0;
  self->m_capacity = 0;
  ByteStream_init(self, 0);
}

void ByteStream_makeBuf(ByteStream *self, uint32_t size, const uint8_t *buf)
{
  self->m_pData = NULL;
  self->m_position = 0;
  self->m_capacity = 0;
  ByteStream_initBuf(self, size, buf);
}

void ByteStream_destroy(ByteStream *self)
{
  ByteStream_clear(self);
}

// copy and assignment operators
void ByteStream_set(ByteStream *self, const ByteStream *other)
{
  // initialize first then copy, initializing may move the other buffer
  // in the arena so a pointer to its data cannot be taken beforehand
  if (!ByteStream_init(self, ByteStream_capacity(other)) || !self->m_pData) {
    return;
  }
  memcpy(self->m_pData->buf, other->m_pData->buf, ByteStream_capacity(other));
  self->m_pData->flags = other->m_pData->flags;
  self->m_pData->crc32 = other->m_pData->crc32;
  self->m_pData->size = other->m_pData->size;
}

// move operator since std::move not always available
void ByteStream_move(ByteStream *self, ByteStream *target)
{
  if (!target) {
    return;
  }
  ByteStream_clear(target);
  target->m_pData = self->m_pData;
  target->m_position = self->m_position;
  target->m_capacity = self->m_capacity;
  if (target->m_pData) {
    // the target now owns the chunk in the arena
    ByteStream_chunk(target->m_pData)->owner = target;
  }
  self->m_pData = NULL;
  self->m_position = 0;
  self->m_capacity = 0;
}


bool ByteStream_rawInit(ByteStream *self, const uint8_t *rawdata, uint32_t size)
{
  if (!rawdata || size < sizeof(RawBuffer)) {
    DEBUG_LOGF("Cannot rawInit: %p %u", rawdata, size);
    return false;
  }
  // release any existing buffer first so it doesn't linger in the arena
  ByteStream_clear(self);
  // round up to nearest 4
  self->m_capacity = (size + 4) - (size % 4);
  self->m_pData = ByteStream_allocBuffer(self, self->m_capacity);
  if (!self->m_pData) {
    self->m_capacity = 0;
    return false;
  }
  memset(self->m_pData, 0, self->m_capacity + sizeof(RawBuffer));
  // copy in the actual data from the serial buffer, this will fill the
  // members: size, flags, and crc32 of m_pData as well as the buf
  memcpy(self->m_pData, rawdata, size);
  // ensure the internal size doesn't exceed outer size
  ByteStream_sanity(self);
  return true;
}

// reset the buffer
bool ByteStream_init(ByteStream *self, uint32_t capacity)
{
  return ByteStream_initBuf(self, capacity, NULL);
}

bool ByteStream_initBuf(ByteStream *self, uint32_t size, const uint8_t *buf)
{
  ByteStream_clear(self);
  if (size) {
    // round up to nearest 4
    self->m_capacity = (size + 4) - (size % 4);
    self->m_pData = ByteStream_allocBuffer(self, self->m_capacity);
    if (!self->m_pData) {
      self->m_capacity = 0;
      return false;
    }
    self->m_pData->size = 0;
    self->m_pData->flags = 0;
    self->m_pData->crc32 = 0;
    memset(self->m_pData->buf, 0, self->m_capacity);
  }
  if (buf && self->m_pData) {
    memcpy(self->m_pData->buf, buf, size);
    self->m_pData->size = size;
    RawBuffer_recalcCRC(self->m_pData);
  }
  return true;
}

void ByteStream_clear(ByteStream *self)
{
  if (self->m_pData) {
    ByteStream_freeBuffer(self->m_pData);
    self->m_pData = NULL;
  }
  self->m_capacity = 0;
  self->m_position = 0;
}

bool ByteStream_shrink(ByteStream *self)
{
  if (!self->m_pData) {
    return false;
  }
  if (self->m_pData->size == self->m_capacity) {
    return true;
  }
  self->m_capacity = self->m_pData->size;
  return ByteStream_resizeBuffer(self, self->m_capacity);
}

// append another buffer
bool ByteStream_append(ByteStream *self, const ByteStream *other)
{
  if (!other->m_pData) {
    // nothing to append
    return true;
  }
  if (!self->m_pData || ByteStream_size(other) > (self->m_capacity - ByteStream_size(self))) {
    if (!ByteStream_extend(self, ByteStream_size(other))) {
      return false;
    }
  }
  memcpy(ByteStream_frontSerializer(self), ByteStream_data(other), ByteStream_size(other));
  self->m_pData->size += ByteStream_size(other);
  return true;
}

// trim some amount of bytes off the end of the stream
void ByteStream_trim(ByteStream *self, uint32_t bytes)
{
  if (!self->m_pData) {
    return;
  }
  // just adjust the data size no need to reallocate, this
  // will put the extra bytes into 'extra space' and shrink()
  // could be called to reallocate the buffer without them
  if (self->m_pData->size <= bytes) {
    self->m_pData->size = 0;
  } else {
    self->m_pData->size -= bytes;
  }
  // move the unserializer back if necessary
  if (self->m_position >= self->m_pData->size) {
    self->m_position = self->m_pData->size;
  }
}

// extend the storage without changing the size of the data
bool ByteStream_extend(ByteStream *self, uint32_t size)
{
  if (!size) {
    // ???
    return true;
  }
  // if the buffer is empty just initialize a new buffer
  if (!self->m_capacity) {
    return ByteStream_init(self, size);
  }
  // new size is current plus extension amount
  uint32_t buffer_size = self->m_capacity + size + 3;
  // round size up to nearest 4 and add the size of the base raw buffer
  buffer_size -= buffer_size % 4;
  // grow the buffer in the arena, this updates m_pData if it moves
  if (!ByteStream_resizeBuffer(self, buffer_size)) {
    return false;
  }
  // calc the region that was newly added to the buffer
  uint8_t *new_mem = ((uint8_t *)self->m_pData) + sizeof(RawBuffer) + self->m_capacity;
  size_t new_mem_size = buffer_size - self->m_capacity;
  // zero out the new memory
  memset(new_mem, 0, new_mem_size);
  // update the size of the serial buffer
  self->m_capacity = buffer_size;
  return true;
}

bool ByteStream_compress(ByteStream *self)
{
  // only compress if we have valid data
  if (!self->m_pData) {
    DEBUG_LOG("No data to compress");
    return false;
  }
  // recalculate the CRC even if we don't compress, this ensures the
  // CRC is always up to date as long as compress() is called
  ByteStream_recalcCRC(self, false);
  // don't actually perform any compression if VORTEX SLIM is enabled
#if VORTEX_SLIM == 0
  // check to see if the buffer is already compressed
  if (ByteStream_is_compressed(self)) {
    // already compressed
    return true;
  }
  // use LZ4_compressBound for the output buffer size, it may be larger
  // but it will allow for faster compression then we can shrink it after
  ByteStream compressedBuffer;
  ByteStream_makeBuf(&compressedBuffer, compress_size(self->m_pData->size), NULL);
  // compress the data
  int compressedSize = compress_buffer(self->m_pData->buf, compressedBuffer.m_pData->buf,
    self->m_pData->size, compressedBuffer.m_capacity);
  // check for compression error
  if (compressedSize < 0) {
    DEBUG_LOGF("Failed to compress, error: %d", compressedSize);
    ByteStream_destroy(&compressedBuffer);
    return false;
  }
  // check for useless compression
  if (!compressedSize || (uint32_t)compressedSize >= self->m_pData->size) {
    // cannot compress, or compressed no smaller than original
    ByteStream_destroy(&compressedBuffer);
    return true;
  }
  // update the buffer size
  compressedBuffer.m_pData->size = compressedSize;
  // buffer is now compressed
  compressedBuffer.m_pData->flags = (self->m_pData->flags | BUFFER_FLAG_COMRPESSED);
  // recalc the crc on the data buffer
  RawBuffer_recalcCRC(compressedBuffer.m_pData);
  // shrink buffer capacity to match size
  ByteStream_shrink(&compressedBuffer);
  // move into self
  ByteStream_move(&compressedBuffer, self);
#endif
  return true;
}

bool ByteStream_decompress(ByteStream *self)
{
  // ensure the internal buffer is sane after reading it out, this
  // prevents segfaults if the internal size reports larger than capacity
  ByteStream_sanity(self);
  // only decompress if we have valid data
  if (!self->m_pData || !RawBuffer_verify(self->m_pData)) {
    DEBUG_LOG("Cannot verify crc, not decompressing");
    return false;
  }
  // don't actually perform any compression if VORTEX SLIM is enabled
#if VORTEX_SLIM == 0
  if (!ByteStream_is_compressed(self)) {
    // already decompressed
    return true;
  }
  // first multiple will be 2x
  uint32_t multiple = 1;
  int decompressedSize = 0;
  ByteStream decompressedBuffer;
  ByteStream_make(&decompressedBuffer);
  do {
    // double the amount each time
    multiple *= 2;
    // create buffer to receive decompressed data
    ByteStream_init(&decompressedBuffer, self->m_pData->size * multiple);
    // decompress the data
    decompressedSize = decompress_buffer(self->m_pData->buf, decompressedBuffer.m_pData->buf,
      self->m_pData->size, decompressedBuffer.m_capacity);
  } while (decompressedSize < 0 && multiple < 512);
  // still error
  if (decompressedSize < 0) {
    ERROR_LOGF("Failed to decompress: %d", decompressedSize);
    ByteStream_destroy(&decompressedBuffer);
    return false;
  }
  // size changed
  decompressedBuffer.m_pData->size = decompressedSize;
  // data is no longer compressed
  decompressedBuffer.m_pData->flags = (self->m_pData->flags & ~BUFFER_FLAG_COMRPESSED);
  // recalc crc of buffer
  RawBuffer_recalcCRC(decompressedBuffer.m_pData);
  DEBUG_LOGF("Decompressed %u to %u bytes", self->m_pData->size, decompressedBuffer.m_pData->size);
  // shrink buffer capacity to match size
  ByteStream_shrink(&decompressedBuffer);
  // move into self
  ByteStream_move(&decompressedBuffer, self);
#endif
  return true;
}

uint32_t ByteStream_recalcCRC(ByteStream *self, bool force)
{
  if (!self->m_pData || !self->m_pData->size) {
    return 0;
  }
  // is the crc dirty?
  // or is the recalc being forced anyway?
  if (!force && !(self->m_pData->flags & BUFFER_FLAG_DIRTY)) {
    return 0;
  }
  // re-calculate the CRC on the buffer
  RawBuffer_recalcCRC(self->m_pData);
  // unset dirty flag
  self->m_pData->flags &= ~BUFFER_FLAG_DIRTY;
  // return the newly calculated crc
  return self->m_pData->crc32;
}

void ByteStream_sanity(ByteStream *self)
{
  // to ensure size never exceeds the buffer capacity
  if (self->m_pData && self->m_pData->size > self->m_capacity) {
    self->m_pData->size = self->m_capacity;
  }
}

bool ByteStream_checkCRC(const ByteStream *self)
{
  // don't check if dirty
  if (!self->m_pData || ByteStream_isCRCDirty(self)) {
    return false;
  }
  return RawBuffer_verify(self->m_pData);
}

bool ByteStream_isCRCDirty(const ByteStream *self)
{
  return (self->m_pData && (self->m_pData->flags & BUFFER_FLAG_DIRTY) != 0);
}

void ByteStream_setCRCDirty(ByteStream *self)
{
  if (!self->m_pData) {
    return;
  }
  // set the dirty flag
  self->m_pData->flags |= BUFFER_FLAG_DIRTY;
}

bool ByteStream_serialize8(ByteStream *self, uint8_t byte)
{
  //DEBUG_LOGF("Serialize8(): %u", byte);
  if (!self->m_pData || (self->m_pData->size + sizeof(uint8_t)) > self->m_capacity) {
    if (!ByteStream_extend(self, sizeof(uint32_t))) {
      return false;
    }
  }
  memcpy(self->m_pData->buf + self->m_pData->size, &byte, sizeof(uint8_t));
  // walk forward
  self->m_pData->size += sizeof(uint8_t);
  // dirty the crc
  ByteStream_setCRCDirty(self);
  return true;
}

bool ByteStream_serialize16(ByteStream *self, uint16_t bytes)
{
  //DEBUG_LOGF("Serialize16(): %u", bytes);
  if (!self->m_pData || (self->m_pData->size + sizeof(uint16_t)) > self->m_capacity) {
    if (!ByteStream_extend(self, sizeof(uint32_t))) {
      return false;
    }
  }
  memcpy(self->m_pData->buf + self->m_pData->size, &bytes, sizeof(uint16_t));
  // walk forward
  self->m_pData->size += sizeof(uint16_t);
  // dirty the crc
  ByteStream_setCRCDirty(self);
  return true;
}

bool ByteStream_serialize32(ByteStream *self, uint32_t bytes)
{
  //DEBUG_LOGF("Serialize32(): %u", bytes);
  if (!self->m_pData || (self->m_pData->size + sizeof(uint32_t)) > self->m_capacity) {
    if (!ByteStream_extend(self, sizeof(uint32_t))) {
      return false;
    }
  }
  memcpy(self->m_pData->buf + self->m_pData->size, &bytes, sizeof(uint32_t));
  // walk forward
  self->m_pData->size += sizeof(uint32_t);
  // dirty the crc
  ByteStream_setCRCDirty(self);
  return true;
}

// reset the unserializer index so that unserialization will
// begin from the start of the buffer
void ByteStream_resetUnserializer(ByteStream *self)
{
  ByteStream_moveUnserializer(self, 0);
}

// move the unserializer index manually
void ByteStream_moveUnserializer(ByteStream *self, uint32_t idx)
{
  if (!self->m_pData) {
    return;
  }
  if (idx >= self->m_pData->size) {
    idx = 0;
  }
  self->m_position = idx;
}

bool ByteStream_unserializerAtEnd(const ByteStream *self)
{
  // the serializer always points to the end of the buffer,
  // if the unserializer points there too then there's nothing
  // more for the unserializer to read
  return self->m_pData && self->m_position == self->m_pData->size;
}

// unserialize data and walk the buffer that many bytes
bool ByteStream_unserialize8(ByteStream *self, uint8_t *byte)
{
  if (!self->m_pData || self->m_position >= self->m_pData->size || (self->m_pData->size - self->m_position) < sizeof(uint8_t)) {
    return false;
  }
  memcpy(byte, self->m_pData->buf + self->m_position, sizeof(uint8_t));
  //DEBUG_LOGF("Unserialize8(): %u", *byte);
  self->m_position += sizeof(uint8_t);
  return true;
}

bool ByteStream_unserialize16(ByteStream *self, uint16_t *bytes)
{
  if (!self->m_pData || self->m_position >= self->m_pData->size || (self->m_pData->size - self->m_position) < sizeof(uint16_t)) {
    return false;
  }
  memcpy(bytes, self->m_pData->buf + self->m_position, sizeof(uint16_t));
  //DEBUG_LOGF("Unserialize16(): %u", *bytes);
  self->m_position += sizeof(uint16_t);
  return true;
}

bool ByteStream_unserialize32(ByteStream *self, uint32_t *bytes)
{
  if (!self->m_pData || self->m_position >= self->m_pData->size || (self->m_pData->size - self->m_position) < sizeof(uint32_t)) {
    return false;
  }
  memcpy(bytes, self->m_pData->buf + self->m_position, sizeof(uint32_t));
  //DEBUG_LOGF("Unserialize32(): %u", *bytes);
  self->m_position += sizeof(uint32_t);
  return true;
}

// unserialize data and erase it from the buffer
bool ByteStream_consume8(ByteStream *self, uint8_t *bytes)
{
  if (!self->m_pData || self->m_pData->size < sizeof(uint8_t)) {
    return false;
  }
  if (bytes) {
    // copy out the data
    memcpy(bytes, self->m_pData->buf, sizeof(uint8_t));
  }
  // adjust the size
  self->m_pData->size -= sizeof(uint8_t);
  // shift the data
  memmove(self->m_pData->buf, self->m_pData->buf + sizeof(uint8_t), self->m_pData->size);
  // dirty the crc
  ByteStream_setCRCDirty(self);
  return true;
}

bool ByteStream_consume16(ByteStream *self, uint16_t *bytes)
{
  if (!self->m_pData || self->m_pData->size < sizeof(uint16_t)) {
    return false;
  }
  if (bytes) {
    // copy out the data
    memcpy(bytes, self->m_pData->buf, sizeof(uint16_t));
  }
  // adjust the size
  self->m_pData->size -= sizeof(uint16_t);
  // shift the data
  memmove(self->m_pData->buf, self->m_pData->buf + sizeof(uint16_t), self->m_pData->size);
  // dirty the crc
  ByteStream_setCRCDirty(self);
  return true;
}

bool ByteStream_consume32(ByteStream *self, uint32_t *bytes)
{
  if (!self->m_pData || self->m_pData->size < sizeof(uint32_t)) {
    return false;
  }
  if (bytes) {
    // copy out the data
    memcpy(bytes, self->m_pData->buf, sizeof(uint32_t));
  }
  // adjust the size
  self->m_pData->size -= sizeof(uint32_t);
  // shift the data
  memmove(self->m_pData->buf, self->m_pData->buf + sizeof(uint32_t), self->m_pData->size);
  // dirty the crc
  ByteStream_setCRCDirty(self);
  return true;
}

bool ByteStream_consume(ByteStream *self, uint32_t size, void *bytes)
{
  if (!self->m_pData || self->m_position >= self->m_pData->size || (self->m_pData->size - self->m_position) < size) {
    return false;
  }
  if (bytes) {
    // copy out the data
    memcpy(bytes, self->m_pData->buf, size);
  }
  // adjust the size
  self->m_pData->size -= size;
  // shift the data
  memmove(self->m_pData->buf, self->m_pData->buf + size, self->m_pData->size);
  // dirty the crc
  ByteStream_setCRCDirty(self);
  return true;
}

uint8_t ByteStream_peek8(const ByteStream *self)
{
  if (!self->m_pData) {
    return 0;
  }
  return *(uint8_t *)ByteStream_frontUnserializer(self);
}

uint16_t ByteStream_peek16(const ByteStream *self)
{
  if (!self->m_pData) {
    return 0;
  }
  return *(uint16_t *)ByteStream_frontUnserializer(self);
}

uint32_t ByteStream_peek32(const ByteStream *self)
{
  if (!self->m_pData) {
    return 0;
  }
  return *(uint32_t *)ByteStream_frontUnserializer(self);
}

bool ByteStream_is_compressed(const ByteStream *self)
{
  if (!self->m_pData) {
    return false;
  }
  return (self->m_pData->flags & BUFFER_FLAG_COMRPESSED) != 0;
}

bool ByteStream_largeEnough(const ByteStream *self, uint32_t amount)
{
  if (!self->m_pData) {
    return false;
  }
  return ((self->m_pData->size + amount) <= self->m_capacity);
}

// get width of bits that makes up value
// ex: 3 -> 2 bits wide (11)
//     9 -> 4 bits wide (1001)
uint32_t ByteStream_getWidth(const ByteStream *self, uint32_t value)
{
  if (!value) {
    return 0;
  }
  for (uint32_t i = 0; i < 31; ++i) {
    if (value < (uint32_t)(1 << i)) {
      return i;
    }
  }
  return 0;
}