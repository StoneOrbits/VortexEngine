

#include <string.h>

#include "../c_types.h"

#define BITSTREAM_STATIC_BYTES VL_RECV_BUF_SIZE
static uint32_t s_bitStreamStatic[(BITSTREAM_STATIC_BYTES + 3) / sizeof(uint32_t)];

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
  if (size > sizeof(s_bitStreamStatic)) {
    ERROR_OUT_OF_MEMORY();
    return false;
  }
  BitStream_initBuf(self, (uint8_t *)s_bitStreamStatic, size);
  memset(self->buf, 0, size);
  return true;
}

void BitStream_destroy(BitStream *self)
{
  (void)self;
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

bool BitStream_eof(const BitStream *self)
{
  return self->buf_eof;
}

bool BitStream_allocated(const BitStream *self)
{
  return self->allocated;
}

uint16_t BitStream_size(const BitStream *self)
{
  return self->buf_size;
}

const uint8_t *BitStream_data(const BitStream *self)
{
  return self->buf;
}

uint8_t BitStream_peekData(const BitStream *self, uint8_t pos)
{
  return self->buf[pos];
}

uint16_t BitStream_dwordpos(const BitStream *self)
{
  return (uint16_t)(self->bit_pos / 32);
}

uint16_t BitStream_bytepos(const BitStream *self)
{
  return (uint16_t)(self->bit_pos / 8);
}

uint16_t BitStream_bitpos(const BitStream *self)
{
  return self->bit_pos;
}
