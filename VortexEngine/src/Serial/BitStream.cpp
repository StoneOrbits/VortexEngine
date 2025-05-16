#include "BitStream.h"

#include "../Memory/Memory.h"
#include "../Log/Log.h"

#include <string.h>

BitStream::BitStream() :
  m_buf(nullptr), m_buf_size(0),
  m_bit_pos(0), m_buf_eof(false),
  m_allocated(false)
{
}

BitStream::BitStream(uint8_t *buf, uint32_t size) :
  BitStream()
{
  init(buf, size);
}

BitStream::BitStream(uint32_t size) :
  BitStream()
{
  init(size);
}

BitStream::~BitStream()
{
  if (m_allocated) {
    vfree(m_buf);
  }
}

bool BitStream::init(uint8_t *buf, uint32_t size)
{
  m_buf = buf;
  m_buf_size = size;
  resetPos();
  return true;
}

bool BitStream::init(uint32_t size)
{
  if (m_buf) {
    vfree(m_buf);
  }
  m_buf = (uint8_t *)vcalloc(1, size);
  if (!m_buf) {
    ERROR_OUT_OF_MEMORY();
    return false;
  }
  m_buf_size = size;
  m_allocated = true;
  resetPos();
  return true;
}

void BitStream::reset()
{
  if (m_buf) {
    memset(m_buf, 0, m_buf_size);
  }
  resetPos();
}

void BitStream::resetPos()
{
  m_bit_pos = 0;
  m_buf_eof = false;
}

uint8_t BitStream::read1Bit()
{
  if (m_buf_eof) {
    return 0;
  }
  if (m_bit_pos >= (m_buf_size * 8)) {
    m_buf_eof = true;
    return 0;
  }
  uint32_t rv = (m_buf[m_bit_pos / 8] >> (7 - (m_bit_pos % 8))) & 1;
  m_bit_pos++;
  if (m_bit_pos >= (m_buf_size * 8)) {
    m_buf_eof = true;
  }
  return rv;
}

void BitStream::write1Bit(bool bit)
{
  if (m_buf_eof) {
    return;
  }
  if (m_bit_pos >= (m_buf_size * 8)) {
    m_buf_eof = true;
    return;
  }
  uint8_t bitVal = (uint8_t)bit & 1;
  m_buf[m_bit_pos / 8] |= bitVal << (7 - (m_bit_pos % 8));
  m_bit_pos++;
  if (m_bit_pos >= (m_buf_size * 8)) {
    m_buf_eof = true;
  }
}

uint8_t BitStream::readBits(uint32_t numBits)
{
  uint32_t val = 0;
  if (m_buf_eof) {
    return 0;
  }
  for (uint32_t i = 0; i < numBits; ++i) {
    val = (val << 1) | read1Bit();
    if (m_buf_eof) {
      break;
    }
  }
  if (m_bit_pos >= (m_buf_size * 8)) {
    m_buf_eof = true;
  }
  return val;
}

// writes numBits of the LSB in sequential order
void BitStream::writeBits(uint32_t numBits, uint32_t val)
{
  for (uint32_t i = 0; i < numBits; ++i) {
    // write each bit in order, for ex if numBits is 4:
    //
    //  00000000 01010101
    //               ^^^^
    //            write these four bits from left to right
    write1Bit((val >> ((numBits - 1) - i)) & 1);
  }
}