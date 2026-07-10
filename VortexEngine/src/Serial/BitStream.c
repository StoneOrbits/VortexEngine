#include "BitStream.h"

#include "../Memory/Memory.h"
#include "../Log/Log.h"

#include <string.h>

void BitStream_init(BitStream *self)
{
  self->buf = NULL;
  self->buf_size = 0;
  self->bit_pos = 0;
  self->buf_eof = false;
  self->allocated = false;
}

void BitStream_initBuf(BitStream *self, uint8_t *buf, uint32_t size)
{
  BitStream_init(self);
  self->buf = buf;
  self->buf_size = (uint16_t)size;
  BitStream_resetPos(self);
}

bool BitStream_initAlloc(BitStream *self, uint32_t size)
{
  if (self->buf) {
    vfree(self->buf);
  }
  self->buf = (uint8_t *)vcalloc(1, size);
  if (!self->buf) {
    ERROR_OUT_OF_MEMORY();
    return false;
  }
  self->buf_size = (uint16_t)size;
  self->allocated = true;
  BitStream_resetPos(self);
  return true;
}

void BitStream_destroy(BitStream *self)
{
  if (self->allocated) {
    vfree(self->buf);
  }
}

void BitStream_reset(BitStream *self)
{
  if (self->buf) {
    memset(self->buf, 0, self->buf_size);
  }
  BitStream_resetPos(self);
}

void BitStream_resetPos(BitStream *self)
{
  self->bit_pos = 0;
  self->buf_eof = false;
}

uint8_t BitStream_read1Bit(BitStream *self)
{
  if (self->buf_eof) {
    return 0;
  }
  if (self->bit_pos >= (self->buf_size * 8)) {
    self->buf_eof = true;
    return 0;
  }
  uint32_t rv = (self->buf[self->bit_pos / 8] >> (7 - (self->bit_pos % 8))) & 1;
  self->bit_pos++;
  if (self->bit_pos >= (self->buf_size * 8)) {
    self->buf_eof = true;
  }
  return (uint8_t)rv;
}

void BitStream_write1Bit(BitStream *self, bool bit)
{
  if (self->buf_eof) {
    return;
  }
  if (self->bit_pos >= (self->buf_size * 8)) {
    self->buf_eof = true;
    return;
  }
  uint8_t bitVal = (uint8_t)bit & 1;
  self->buf[self->bit_pos / 8] |= bitVal << (7 - (self->bit_pos % 8));
  self->bit_pos++;
  if (self->bit_pos >= (self->buf_size * 8)) {
    self->buf_eof = true;
  }
}

uint8_t BitStream_readBits(BitStream *self, uint32_t numBits)
{
  uint32_t val = 0;
  if (self->buf_eof) {
    return 0;
  }
  for (uint32_t i = 0; i < numBits; ++i) {
    val = (val << 1) | BitStream_read1Bit(self);
    if (self->buf_eof) {
      break;
    }
  }
  if (self->bit_pos >= (self->buf_size * 8)) {
    self->buf_eof = true;
  }
  return (uint8_t)val;
}

void BitStream_writeBits(BitStream *self, uint32_t numBits, uint32_t val)
{
  for (uint32_t i = 0; i < numBits; ++i) {
    BitStream_write1Bit(self, (val >> ((numBits - 1) - i)) & 1);
  }
}
