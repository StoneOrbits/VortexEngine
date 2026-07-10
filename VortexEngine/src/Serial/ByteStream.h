#ifndef BYTE_STREAM_H
#define BYTE_STREAM_H

#include <inttypes.h>
#include <stdbool.h>
#include <stddef.h>
#include "../Log/Log.h"

#ifdef _MSC_VER
#pragma warning(disable : 4200)
#endif

struct RawBuffer
{
  uint32_t size;
  uint32_t flags;
  uint32_t crc32;
  uint8_t buf[];
};

typedef struct RawBuffer RawBuffer;

typedef struct ByteStream
{
  RawBuffer *pData;
  uint16_t position;
  uint16_t capacity;
} ByteStream;

struct FlashClass;

// init/destroy
bool ByteStream_init(ByteStream *self, uint32_t size, const uint8_t *buf);
void ByteStream_destroy(ByteStream *self);
void ByteStream_copy(ByteStream *self, const ByteStream *other);
void ByteStream_assign(ByteStream *self, const ByteStream *other);

void ByteStream_move(ByteStream *self, ByteStream *target);

bool ByteStream_rawInit(ByteStream *self, const uint8_t *rawdata, uint32_t size);
bool ByteStream_init(ByteStream *self, uint32_t capacity, const uint8_t *buf);

void ByteStream_clear(ByteStream *self);
bool ByteStream_shrink(ByteStream *self);
bool ByteStream_append(ByteStream *self, const ByteStream *other);
bool ByteStream_extend(ByteStream *self, uint32_t size);
void ByteStream_trim(ByteStream *self, uint32_t bytes);

bool ByteStream_compress(ByteStream *self);
bool ByteStream_decompress(ByteStream *self);

uint32_t ByteStream_recalcCRC(ByteStream *self, bool force);
void ByteStream_sanity(ByteStream *self);
bool ByteStream_checkCRC(const ByteStream *self);
bool ByteStream_isCRCDirty(const ByteStream *self);
void ByteStream_setCRCDirty(ByteStream *self);

bool ByteStream_serialize8(ByteStream *self, uint8_t byte);
bool ByteStream_serialize16(ByteStream *self, uint16_t bytes);
bool ByteStream_serialize32(ByteStream *self, uint32_t bytes);

void ByteStream_resetUnserializer(ByteStream *self);
void ByteStream_moveUnserializer(ByteStream *self, uint32_t idx);
bool ByteStream_unserializerAtEnd(const ByteStream *self);

bool ByteStream_unserialize8(ByteStream *self, uint8_t *byte);
bool ByteStream_unserialize16(ByteStream *self, uint16_t *bytes);
bool ByteStream_unserialize32(ByteStream *self, uint32_t *bytes);

bool ByteStream_consume8(ByteStream *self, uint8_t *byte);
bool ByteStream_consume16(ByteStream *self, uint16_t *bytes);
bool ByteStream_consume32(ByteStream *self, uint32_t *bytes);
bool ByteStream_consume(ByteStream *self, uint32_t size, void *bytes);

uint8_t ByteStream_peek8(const ByteStream *self);
uint16_t ByteStream_peek16(const ByteStream *self);
uint32_t ByteStream_peek32(const ByteStream *self);

uint8_t *ByteStream_get(ByteStream *self, uint32_t index);
uint8_t *ByteStream_getPtr(ByteStream *self);
const uint8_t *ByteStream_getConstPtr(const ByteStream *self);

const uint8_t *ByteStream_data(const ByteStream *self);
void *ByteStream_rawData(const ByteStream *self);
uint16_t ByteStream_rawSize(const ByteStream *self);
uint32_t ByteStream_size(const ByteStream *self);
uint32_t ByteStream_capacity(const ByteStream *self);
bool ByteStream_is_compressed(const ByteStream *self);
uint32_t ByteStream_CRC(const ByteStream *self);

uint8_t *ByteStream_frontUnserializer(const ByteStream *self);

// -- private helpers (exposed for internal use) --
uint8_t *ByteStream_frontSerializer(const ByteStream *self);

// RawBuffer free functions
uint32_t RawBuffer_hash(const RawBuffer *self);
void RawBuffer_accumulate(RawBuffer *self, uint32_t val);
bool RawBuffer_verify(const RawBuffer *self);
void RawBuffer_recalcCRC(RawBuffer *self);

#endif
