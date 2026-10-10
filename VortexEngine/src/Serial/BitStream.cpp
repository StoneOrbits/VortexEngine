#include "BitStream.h"

#include "../Memory/Memory.h"
#include "../Log/Log.h"

#include <string.h>

void BitStream_make(BitStream *self)
{
  self->m_buf = NULL;
  self->m_buf_size = 0;
  self->m_bit_pos = 0;
  self->m_buf_eof = false;
  self->m_allocated = false;
}

void BitStream_makeBuf(BitStream *self, uint8_t *buf, uint32_t size)
{
  BitStream_make(self);
  BitStream_initBuf(self, buf, size);
}

void BitStream_makeSize(BitStream *self, uint32_t size)
{
  BitStream_make(self);
  BitStream_initSize(self, size);
}

void BitStream_destroy(BitStream *self)
{
  if (self->m_allocated) {
    vfree(self->m_buf);
  }
}

bool BitStream_initBuf(BitStream *self, uint8_t *buf, uint32_t size)
{
  self->m_buf = buf;
  self->m_buf_size = size;
  BitStream_resetPos(self);
  return true;
}

bool BitStream_initSize(BitStream *self, uint32_t size)
{
  if (self->m_buf) {
    vfree(self->m_buf);
  }
  self->m_buf = (uint8_t *)vcalloc(1, size);
  if (!self->m_buf) {
    ERROR_OUT_OF_MEMORY();
    return false;
  }
  self->m_buf_size = size;
  self->m_allocated = true;
  BitStream_resetPos(self);
  return true;
}

void BitStream_reset(BitStream *self)
{
  if (self->m_buf) {
    memset(self->m_buf, 0, self->m_buf_size);
  }
  BitStream_resetPos(self);
}

void BitStream_resetPos(BitStream *self)
{
  self->m_bit_pos = 0;
  self->m_buf_eof = false;
}

uint8_t BitStream_read1Bit(BitStream *self)
{
  if (self->m_buf_eof) {
    return 0;
  }
  if (self->m_bit_pos >= (self->m_buf_size * 8)) {
    self->m_buf_eof = true;
    return 0;
  }
  uint32_t rv = (self->m_buf[self->m_bit_pos / 8] >> (7 - (self->m_bit_pos % 8))) & 1;
  self->m_bit_pos++;
  if (self->m_bit_pos >= (self->m_buf_size * 8)) {
    self->m_buf_eof = true;
  }
  return rv;
}

void BitStream_write1Bit(BitStream *self, bool bit)
{
  if (self->m_buf_eof) {
    return;
  }
  if (self->m_bit_pos >= (self->m_buf_size * 8)) {
    self->m_buf_eof = true;
    return;
  }
  uint8_t bitVal = (uint8_t)bit & 1;
  self->m_buf[self->m_bit_pos / 8] |= bitVal << (7 - (self->m_bit_pos % 8));
  self->m_bit_pos++;
  if (self->m_bit_pos >= (self->m_buf_size * 8)) {
    self->m_buf_eof = true;
  }
}

uint8_t BitStream_readBits(BitStream *self, uint32_t numBits)
{
  uint32_t val = 0;
  if (self->m_buf_eof) {
    return 0;
  }
  for (uint32_t i = 0; i < numBits; ++i) {
    val = (val << 1) | BitStream_read1Bit(self);
    if (self->m_buf_eof) {
      break;
    }
  }
  if (self->m_bit_pos >= (self->m_buf_size * 8)) {
    self->m_buf_eof = true;
  }
  return val;
}

// writes numBits of the LSB in sequential order
void BitStream_writeBits(BitStream *self, uint32_t numBits, uint32_t val)
{
  for (uint32_t i = 0; i < numBits; ++i) {
    // write each bit in order, for ex if numBits is 4:
    //
    //  00000000 01010101
    //               ^^^^
    //            write these four bits from left to right
    BitStream_write1Bit(self, (val >> ((numBits - 1) - i)) & 1);
  }
}