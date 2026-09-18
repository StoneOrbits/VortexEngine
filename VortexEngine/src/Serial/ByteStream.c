

#include <string.h>


#include "../c_types.h"
#define BUFFER_FLAG_COMRPESSED (1<<0)
#define BUFFER_FLAG_DIRTY      (1<<1)

#define BYTESTREAM_SCRATCH_BYTES 96
static uint32_t s_scratch[BYTESTREAM_SCRATCH_BYTES / sizeof(uint32_t)];
static bool s_scratchBusy = false;

static bool ByteStream_usesScratch(const ByteStream *self)
{
  return self->pData == (const RawBuffer *)s_scratch;
}

// --- RawBuffer free functions ---

uint32_t RawBuffer_hash(const RawBuffer *self)
{
  uint32_t hash = 5381;
  for (uint32_t i = 0; i < self->size; ++i) {
    hash = ((hash << 5) + hash) + self->buf[i];
  }
  return hash;
}

void RawBuffer_accumulate(RawBuffer *self, uint32_t val)
{
  self->crc32 = ((self->crc32 << 5) + self->crc32) + val;
}

bool RawBuffer_verify(const RawBuffer *self)
{
  if (!self->size) {
    return true;
  }
  if (RawBuffer_hash(self) != self->crc32) {
    DEBUG_LOGF("CRC mismatch: %x should be %x", RawBuffer_hash(self), self->crc32);
    return false;
  }
  return true;
}

void RawBuffer_recalcCRC(RawBuffer *self)
{
  self->crc32 = RawBuffer_hash(self);
}

// --- ByteStream functions ---

void ByteStream_destroy(ByteStream *self)
{
  ByteStream_clear(self);
}

void ByteStream_copy(ByteStream *self, const ByteStream *other)
{
  ByteStream_init(self, other->capacity, ByteStream_data(other));
  if (self->pData && other->pData) {
    self->pData->flags = other->pData->flags;
    self->pData->crc32 = other->pData->crc32;
    self->pData->size = other->pData->size;
  }
}

void ByteStream_assign(ByteStream *self, const ByteStream *other)
{
  ByteStream_init(self, other->capacity, ByteStream_data(other));
  if (self->pData && other->pData) {
    self->pData->flags = other->pData->flags;
    self->pData->crc32 = other->pData->crc32;
    self->pData->size = other->pData->size;
  }
}

void ByteStream_move(ByteStream *self, ByteStream *target)
{
  if (!target) {
    return;
  }
  bool wasScratch = ByteStream_usesScratch(self);
  ByteStream_clear(target);
  target->pData = self->pData;
  target->position = self->position;
  target->capacity = self->capacity;
  self->pData = NULL;
  self->position = 0;
  self->capacity = 0;
  if (wasScratch) {
    s_scratchBusy = true;
  }
}

void ByteStream_resetData(ByteStream *self)
{
  if (!self->pData) {
    return;
  }
  self->pData->size = 0;
  self->pData->flags = 0;
  self->pData->crc32 = 0;
  self->position = 0;
}

void ByteStream_initStatic(ByteStream *self, uint8_t *storage, uint32_t storageSize)
{
  if (ByteStream_usesScratch(self)) {
    s_scratchBusy = false;
  }
  if (!storage || storageSize <= sizeof(RawBuffer)) {
    self->pData = NULL;
    self->capacity = 0;
    self->position = 0;
    return;
  }
  self->pData = (RawBuffer *)storage;
  self->capacity = (uint16_t)((storageSize - sizeof(RawBuffer)) & ~3u);
  ByteStream_resetData(self);
}

bool ByteStream_rawInit(ByteStream *self, const uint8_t *rawdata, uint32_t size)
{
  if (!rawdata || size < sizeof(RawBuffer)) {
    DEBUG_LOGF("Cannot rawInit: %p %u", (const void *)rawdata, (unsigned)size);
    return false;
  }
  uint16_t capacity = (uint16_t)((size + 4) - (size % 4));
  if (ByteStream_capacity(self) < capacity) {
    if (!ByteStream_init(self, capacity, NULL)) {
      return false;
    }
  } else {
    ByteStream_resetData(self);
  }
  memcpy(self->pData, rawdata, size);
  ByteStream_sanity(self);
  return true;
}

bool ByteStream_init(ByteStream *self, uint32_t size, const uint8_t *buf)
{
  ByteStream_clear(self);
  uint16_t capacity;
  if (size) {
    capacity = (uint16_t)((size + 4) - (size % 4));
  } else {
    capacity = (uint16_t)((sizeof(s_scratch) - sizeof(RawBuffer)) & ~3u);
  }
  if (((uint32_t)capacity + sizeof(RawBuffer) > sizeof(s_scratch)) || s_scratchBusy) {
    ERROR_OUT_OF_MEMORY();
    self->capacity = 0;
    return false;
  }
  s_scratchBusy = true;
  self->capacity = capacity;
  self->pData = (RawBuffer *)s_scratch;
  self->pData->size = 0;
  self->pData->flags = 0;
  self->pData->crc32 = 0;
  memset(self->pData->buf, 0, capacity);
  if (buf) {
    memcpy(self->pData->buf, buf, size);
    self->pData->size = size;
    RawBuffer_recalcCRC(self->pData);
  }
  return true;
}

void ByteStream_clear(ByteStream *self)
{
  if (ByteStream_usesScratch(self)) {
    s_scratchBusy = false;
  }
  self->pData = NULL;
  self->capacity = 0;
  self->position = 0;
}

bool ByteStream_shrink(ByteStream *self)
{
  if (!self->pData) {
    return false;
  }
  self->capacity = (uint16_t)self->pData->size;
  return true;
}

bool ByteStream_append(ByteStream *self, const ByteStream *other)
{
  if (!other->pData) {
    return true;
  }
  if (!self->pData || other->pData->size > (self->capacity - ByteStream_size(self))) {
    if (!ByteStream_extend(self, other->pData->size)) {
      return false;
    }
  }
  memcpy(ByteStream_frontSerializer(self), ByteStream_data(other), other->pData->size);
  self->pData->size += other->pData->size;
  return true;
}

void ByteStream_trim(ByteStream *self, uint32_t bytes)
{
  if (!self->pData) {
    return;
  }
  if (self->pData->size <= bytes) {
    self->pData->size = 0;
  } else {
    self->pData->size -= bytes;
  }
  if (self->position >= self->pData->size) {
    self->position = (uint16_t)self->pData->size;
  }
}

bool ByteStream_extend(ByteStream *self, uint32_t size)
{
  if (!size) {
    return true;
  }
  if (!self->capacity) {
    return ByteStream_init(self, size, NULL);
  }
  ERROR_OUT_OF_MEMORY();
  return false;
}

bool ByteStream_compress(ByteStream *self)
{
  if (!self->pData) {
    DEBUG_LOG("No data to compress");
    return false;
  }
  ByteStream_recalcCRC(self, false);
#if VORTEX_SLIM == 0
  if (ByteStream_is_compressed(self)) {
    return true;
  }
  ByteStream compressedBuffer;
  ByteStream_init(&compressedBuffer, compress_size(self->pData->size), NULL);
  int compressedSize = compress_buffer(self->pData->buf, compressedBuffer.pData->buf,
    (int)self->pData->size, (int)compressedBuffer.capacity);
  if (compressedSize < 0) {
    DEBUG_LOGF("Failed to compress, error: %d", compressedSize);
    ByteStream_destroy(&compressedBuffer);
    return false;
  }
  if (!compressedSize || (uint32_t)compressedSize >= self->pData->size) {
    ByteStream_destroy(&compressedBuffer);
    return true;
  }
  compressedBuffer.pData->size = (uint32_t)compressedSize;
  compressedBuffer.pData->flags = (self->pData->flags | BUFFER_FLAG_COMRPESSED);
  RawBuffer_recalcCRC(compressedBuffer.pData);
  ByteStream_shrink(&compressedBuffer);
  ByteStream_move(&compressedBuffer, self);
#endif
  return true;
}

bool ByteStream_decompress(ByteStream *self)
{
  ByteStream_sanity(self);
  if (!self->pData || !RawBuffer_verify(self->pData)) {
    DEBUG_LOG("Cannot verify crc, not decompressing");
    return false;
  }
#if VORTEX_SLIM == 0
  if (!ByteStream_is_compressed(self)) {
    return true;
  }
  uint32_t multiple = 1;
  int decompressedSize = 0;
  ByteStream decompressedBuffer;
  ByteStream_init(&decompressedBuffer, 0, NULL);
  do {
    multiple *= 2;
    ByteStream_init(&decompressedBuffer, self->pData->size * multiple, NULL);
    decompressedSize = decompress_buffer(self->pData->buf, decompressedBuffer.pData->buf,
      (int)self->pData->size, (int)decompressedBuffer.capacity);
  } while (decompressedSize < 0 && multiple < 512);
  if (decompressedSize < 0) {
    ERROR_LOGF("Failed to decompress: %d", decompressedSize);
    ByteStream_destroy(&decompressedBuffer);
    return false;
  }
  decompressedBuffer.pData->size = (uint32_t)decompressedSize;
  decompressedBuffer.pData->flags = (self->pData->flags & ~BUFFER_FLAG_COMRPESSED);
  RawBuffer_recalcCRC(decompressedBuffer.pData);
  DEBUG_LOGF("Decompressed %u to %u bytes", self->pData->size, decompressedBuffer.pData->size);
  ByteStream_shrink(&decompressedBuffer);
  ByteStream_move(&decompressedBuffer, self);
#endif
  return true;
}

uint32_t ByteStream_recalcCRC(ByteStream *self, bool force)
{
  if (!self->pData || !self->pData->size) {
    return 0;
  }
  if (!force && !(self->pData->flags & BUFFER_FLAG_DIRTY)) {
    return 0;
  }
  RawBuffer_recalcCRC(self->pData);
  self->pData->flags &= ~BUFFER_FLAG_DIRTY;
  return self->pData->crc32;
}

void ByteStream_sanity(ByteStream *self)
{
  if (self->pData && self->pData->size > self->capacity) {
    self->pData->size = self->capacity;
  }
}

bool ByteStream_checkCRC(const ByteStream *self)
{
  if (!self->pData || ByteStream_isCRCDirty(self)) {
    return false;
  }
  return RawBuffer_verify(self->pData);
}

bool ByteStream_isCRCDirty(const ByteStream *self)
{
  return (self->pData && (self->pData->flags & BUFFER_FLAG_DIRTY) != 0);
}

void ByteStream_setCRCDirty(ByteStream *self)
{
  if (!self->pData) {
    return;
  }
  self->pData->flags |= BUFFER_FLAG_DIRTY;
}

bool ByteStream_serialize8(ByteStream *self, uint8_t byte)
{
  if (!self->pData || (self->pData->size + sizeof(uint8_t)) > self->capacity) {
    if (!ByteStream_extend(self, sizeof(uint32_t))) {
      return false;
    }
  }
  memcpy(self->pData->buf + self->pData->size, &byte, sizeof(uint8_t));
  self->pData->size += sizeof(uint8_t);
  ByteStream_setCRCDirty(self);
  return true;
}

bool ByteStream_serialize16(ByteStream *self, uint16_t bytes)
{
  if (!self->pData || (self->pData->size + sizeof(uint16_t)) > self->capacity) {
    if (!ByteStream_extend(self, sizeof(uint32_t))) {
      return false;
    }
  }
  memcpy(self->pData->buf + self->pData->size, &bytes, sizeof(uint16_t));
  self->pData->size += sizeof(uint16_t);
  ByteStream_setCRCDirty(self);
  return true;
}

bool ByteStream_serialize32(ByteStream *self, uint32_t bytes)
{
  if (!self->pData || (self->pData->size + sizeof(uint32_t)) > self->capacity) {
    if (!ByteStream_extend(self, sizeof(uint32_t))) {
      return false;
    }
  }
  memcpy(self->pData->buf + self->pData->size, &bytes, sizeof(uint32_t));
  self->pData->size += sizeof(uint32_t);
  ByteStream_setCRCDirty(self);
  return true;
}

void ByteStream_resetUnserializer(ByteStream *self)
{
  ByteStream_moveUnserializer(self, 0);
}

void ByteStream_moveUnserializer(ByteStream *self, uint32_t idx)
{
  if (!self->pData) {
    return;
  }
  if (idx >= self->pData->size) {
    idx = 0;
  }
  self->position = (uint16_t)idx;
}

bool ByteStream_unserializerAtEnd(const ByteStream *self)
{
  return self->pData && self->position == self->pData->size;
}

bool ByteStream_unserialize8(ByteStream *self, uint8_t *byte)
{
  if (!self->pData || self->position >= self->pData->size || (self->pData->size - self->position) < sizeof(uint8_t)) {
    return false;
  }
  memcpy(byte, self->pData->buf + self->position, sizeof(uint8_t));
  self->position += sizeof(uint8_t);
  return true;
}

bool ByteStream_unserialize16(ByteStream *self, uint16_t *bytes)
{
  if (!self->pData || self->position >= self->pData->size || (self->pData->size - self->position) < sizeof(uint16_t)) {
    return false;
  }
  memcpy(bytes, self->pData->buf + self->position, sizeof(uint16_t));
  self->position += sizeof(uint16_t);
  return true;
}

bool ByteStream_unserialize32(ByteStream *self, uint32_t *bytes)
{
  if (!self->pData || self->position >= self->pData->size || (self->pData->size - self->position) < sizeof(uint32_t)) {
    return false;
  }
  memcpy(bytes, self->pData->buf + self->position, sizeof(uint32_t));
  self->position += sizeof(uint32_t);
  return true;
}

bool ByteStream_consume8(ByteStream *self, uint8_t *bytes)
{
  if (!self->pData || self->pData->size < sizeof(uint8_t)) {
    return false;
  }
  if (bytes) {
    memcpy(bytes, self->pData->buf, sizeof(uint8_t));
  }
  self->pData->size -= sizeof(uint8_t);
  memmove(self->pData->buf, self->pData->buf + sizeof(uint8_t), self->pData->size);
  ByteStream_setCRCDirty(self);
  return true;
}

bool ByteStream_consume16(ByteStream *self, uint16_t *bytes)
{
  if (!self->pData || self->pData->size < sizeof(uint16_t)) {
    return false;
  }
  if (bytes) {
    memcpy(bytes, self->pData->buf, sizeof(uint16_t));
  }
  self->pData->size -= sizeof(uint16_t);
  memmove(self->pData->buf, self->pData->buf + sizeof(uint16_t), self->pData->size);
  ByteStream_setCRCDirty(self);
  return true;
}

bool ByteStream_consume32(ByteStream *self, uint32_t *bytes)
{
  if (!self->pData || self->pData->size < sizeof(uint32_t)) {
    return false;
  }
  if (bytes) {
    memcpy(bytes, self->pData->buf, sizeof(uint32_t));
  }
  self->pData->size -= sizeof(uint32_t);
  memmove(self->pData->buf, self->pData->buf + sizeof(uint32_t), self->pData->size);
  ByteStream_setCRCDirty(self);
  return true;
}

bool ByteStream_consume(ByteStream *self, uint32_t size, void *bytes)
{
  if (!self->pData || self->position >= self->pData->size || (self->pData->size - self->position) < size) {
    return false;
  }
  if (bytes) {
    memcpy(bytes, self->pData->buf, size);
  }
  self->pData->size -= size;
  memmove(self->pData->buf, self->pData->buf + size, self->pData->size);
  ByteStream_setCRCDirty(self);
  return true;
}

uint8_t ByteStream_peek8(const ByteStream *self)
{
  if (!self->pData) {
    return 0;
  }
  return *(const uint8_t *)ByteStream_frontUnserializer(self);
}

uint16_t ByteStream_peek16(const ByteStream *self)
{
  if (!self->pData) {
    return 0;
  }
  return *(const uint16_t *)ByteStream_frontUnserializer(self);
}

uint32_t ByteStream_peek32(const ByteStream *self)
{
  if (!self->pData) {
    return 0;
  }
  return *(const uint32_t *)ByteStream_frontUnserializer(self);
}

uint8_t *ByteStream_get(ByteStream *self, uint32_t index)
{
  return &self->pData->buf[index];
}

uint8_t *ByteStream_getPtr(ByteStream *self)
{
  return self->pData ? self->pData->buf : NULL;
}

const uint8_t *ByteStream_getConstPtr(const ByteStream *self)
{
  return self->pData ? self->pData->buf : NULL;
}

const uint8_t *ByteStream_data(const ByteStream *self)
{
  return self->pData ? self->pData->buf : NULL;
}

void *ByteStream_rawData(const ByteStream *self)
{
  return (void *)self->pData;
}

uint16_t ByteStream_rawSize(const ByteStream *self)
{
  return self->pData ? (uint16_t)(self->pData->size + sizeof(RawBuffer)) : 0;
}

uint32_t ByteStream_size(const ByteStream *self)
{
  return self->pData ? self->pData->size : 0;
}

uint32_t ByteStream_capacity(const ByteStream *self)
{
  return self->capacity;
}

bool ByteStream_is_compressed(const ByteStream *self)
{
  if (!self->pData) {
    return false;
  }
  return (self->pData->flags & BUFFER_FLAG_COMRPESSED) != 0;
}

uint32_t ByteStream_CRC(const ByteStream *self)
{
  return self->pData ? self->pData->crc32 : 0;
}

uint8_t *ByteStream_frontUnserializer(const ByteStream *self)
{
  return self->pData ? self->pData->buf + self->position : NULL;
}

uint8_t *ByteStream_frontSerializer(const ByteStream *self)
{
  return self->pData ? self->pData->buf + self->pData->size : NULL;
}
