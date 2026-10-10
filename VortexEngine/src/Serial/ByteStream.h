#ifndef BYTE_STREAM_H
#define BYTE_STREAM_H

#include <inttypes.h>
#include <stdbool.h>
#include <stddef.h>
#include "../Log/Log.h"

// disable warning about using flexible array at end of structure
// otherwise visual studio dumps a million warnings about the buf[]
// flexible array member at the end of the inner data structure.
// Yes I know it's scary Visual Studio, please shut up now.
#ifdef _MSC_VER
#pragma warning(disable : 4200)
#endif

typedef struct FlashClass FlashClass;
typedef struct ByteStream ByteStream;

// the structure of raw data that's written to storage
typedef struct RawBuffer {
  // the size of the buf of data
  uint32_t size;
  // any special flags about the data (compression etc)
  uint32_t flags;
  // the crc32 for the data
  uint32_t crc32;
  // the start of the buf of data
  uint8_t buf[];
} RawBuffer;

// hash the raw buffer into crc
static inline uint32_t RawBuffer_hash(const RawBuffer *self)
{
  uint32_t hash = 5381;
  for (uint32_t i = 0; i < self->size; ++i) {
    hash = ((hash << 5) + hash) + self->buf[i];
  }
  return hash;
}

// accumulate the data into the hash
static inline void RawBuffer_accumulate(RawBuffer *self, uint32_t val)
{
  self->crc32 = ((self->crc32 << 5) + self->crc32) + val;
}

// veryify the crc
static inline bool RawBuffer_verify(const RawBuffer *self)
{
  // if the buffer is empty then 'verify' should just return true
  if (!self->size) {
    return true;
  }
  if (RawBuffer_hash(self) != self->crc32) {
    DEBUG_LOGF("CRC mismatch: %x should be %x", RawBuffer_hash(self), self->crc32);
    return false;
  }
  return true;
}

// re-calculate the crc
static inline void RawBuffer_recalcCRC(RawBuffer *self)
{
  self->crc32 = RawBuffer_hash(self);
}

// The raw buffer of data along with size and flags
struct ByteStream {
  RawBuffer *m_pData;
  // the index in the raw buffer for unserialization
  uint16_t m_position;
  // the actual size of the buffer raw buffer
  uint16_t m_capacity;
};

// setup the arena that all ByteStream buffers live in, this is
// done once at startup
bool ByteStream_initArena(void);
// the bytes of the arena currently used and the most ever used
uint32_t ByteStream_arenaUsed(void);
uint32_t ByteStream_arenaHighWater(void);

void ByteStream_make(ByteStream *self);
void ByteStream_makeBuf(ByteStream *self, uint32_t size, const uint8_t *buf);
void ByteStream_destroy(ByteStream *self);

// copy and assignment operators
void ByteStream_set(ByteStream *self, const ByteStream *other);

// move operator since std::move not always available
void ByteStream_move(ByteStream *self, ByteStream *target);

// used to initialize the full RawBuffer with a chunk of data,
// this includes the size, flags, crc, and data
bool ByteStream_rawInit(ByteStream *self, const uint8_t *rawdata, uint32_t size);

// reset the buffer to a specific capcity
bool ByteStream_init(ByteStream *self, uint32_t capacity);
bool ByteStream_initBuf(ByteStream *self, uint32_t capacity, const uint8_t *buf);

// clear the buffer
void ByteStream_clear(ByteStream *self);

// shrink capacity down to size, to free unused space
bool ByteStream_shrink(ByteStream *self);

// append another buffer
bool ByteStream_append(ByteStream *self, const ByteStream *other);

// extend the storage without changing the size of the data
bool ByteStream_extend(ByteStream *self, uint32_t size);

// trim the storage and erase some bytes from the end
void ByteStream_trim(ByteStream *self, uint32_t bytes);

// badly written in-place compression and decompression
// which just uses a table of values then compresses the
// data into indexes in the table packed into a buffer.
// Unfortunately sometimes compression does not yield a
// smaller buffer size, incases like this compress returns
// true but the data isn't compressed. Similarly, if that
// data is passed to decompress it will return true.
bool ByteStream_compress(ByteStream *self);
bool ByteStream_decompress(ByteStream *self);

// re-calculate the built-in CRC on the bytestream if needed
// if the crc is up to date this will do nothing. Optionally
// force a re-calculation even if the dirty flag is missing.
// Will return the new CRC value.
uint32_t ByteStream_recalcCRC(ByteStream *self, bool force);

// helper func to ensure the internal buffer is sane, use this
// after reading into the rawdata of the buffer
void ByteStream_sanity(ByteStream *self);
// check the CRC without re-calculating, note, if the CRC is
// dirty then this will simply return false
bool ByteStream_checkCRC(const ByteStream *self);
// check whether the data in the buffer has changed since the
// crc has been re-calculated, if it is dirty, call recalcCRC
bool ByteStream_isCRCDirty(const ByteStream *self);
// mark the CRC as dirty (manually modified the buffer or something)
void ByteStream_setCRCDirty(ByteStream *self);

// serialize a byte into the buffer
bool ByteStream_serialize8(ByteStream *self, uint8_t byte);
bool ByteStream_serialize16(ByteStream *self, uint16_t bytes);
bool ByteStream_serialize32(ByteStream *self, uint32_t bytes);

// reset the unserializer index
void ByteStream_resetUnserializer(ByteStream *self);
// move the unserializer index manually
void ByteStream_moveUnserializer(ByteStream *self, uint32_t idx);
// check if the unserializer is at the end
bool ByteStream_unserializerAtEnd(const ByteStream *self);

// unserialize data out of the buffer
bool ByteStream_unserialize8(ByteStream *self, uint8_t *byte);
bool ByteStream_unserialize16(ByteStream *self, uint16_t *bytes);
bool ByteStream_unserialize32(ByteStream *self, uint32_t *bytes);

// unserialize data and erase it from the buffer
bool ByteStream_consume8(ByteStream *self, uint8_t *byte);
bool ByteStream_consume16(ByteStream *self, uint16_t *bytes);
bool ByteStream_consume32(ByteStream *self, uint32_t *bytes);

// unserialize and consume a whole chunk
bool ByteStream_consume(ByteStream *self, uint32_t size, void *bytes);

uint8_t ByteStream_peek8(const ByteStream *self);
uint16_t ByteStream_peek16(const ByteStream *self);
uint32_t ByteStream_peek32(const ByteStream *self);

// overload [] for array access (no bounds check lol)
uint8_t *ByteStream_at(ByteStream *self, uint32_t index);

// return the members
static inline const uint8_t *ByteStream_data(const ByteStream *self)
{
  return self->m_pData ? self->m_pData->buf : NULL;
}

static inline void *ByteStream_rawData(const ByteStream *self)
{
  return self->m_pData;
}

static inline uint16_t ByteStream_rawSize(const ByteStream *self)
{
  return self->m_pData ? (uint16_t)self->m_pData->size + sizeof(RawBuffer) : 0;
}

static inline uint32_t ByteStream_size(const ByteStream *self)
{
  return self->m_pData ? self->m_pData->size : 0;
}

static inline uint32_t ByteStream_capacity(const ByteStream *self)
{
  return self->m_capacity;
}

bool ByteStream_is_compressed(const ByteStream *self);

static inline uint32_t ByteStream_CRC(const ByteStream *self)
{
  return self->m_pData ? self->m_pData->crc32 : 0;
}

static inline uint8_t *ByteStream_frontUnserializer(const ByteStream *self)
{
  return self->m_pData ? self->m_pData->buf + self->m_position : NULL;
}

// fetch pointers that walk the buffer to serialize/unserialize
static inline uint8_t *ByteStream_frontSerializer(const ByteStream *self)
{
  return self->m_pData ? self->m_pData->buf + self->m_pData->size : NULL;
}

// check if the buffer is large enough for the amount
bool ByteStream_largeEnough(const ByteStream *self, uint32_t amount);
// helper to get width of bits that makes up value
uint32_t ByteStream_getWidth(const ByteStream *self, uint32_t value);

#endif
