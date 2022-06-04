#include "BitStream.h"

BitStream::BitStream() :
  m_buf(nullptr), m_buf_size(0),
  m_bit_pos(0), m_buf_eof(false)
{
}

BitStream::BitStream(uint8_t *buf, uint32_t size) :
  BitStream::BitStream()
{
  init(buf, size);
}

void BitStream::init(uint8_t *buf, uint32_t size)
{
  m_buf = buf;
  m_buf_size = size;
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
  return rv;
}

void BitStream::write1Bit(uint8_t bit)
{
  if (m_buf_eof) {
    return;
  }
  if (m_bit_pos >= (m_buf_size * 8)) {
    m_buf_eof = true;
    return;
  }
  m_buf[m_bit_pos / 8] |= (bit & 1) << (7 - (m_bit_pos % 8));
  m_bit_pos++;
  if (m_bit_pos >= (m_buf_size * 8)) {
    m_buf_eof = true;
  }
}

uint8_t BitStream::readBits(uint32_t numBits)
{
  uint32_t val = 0;
  for (uint32_t i = 0; i < numBits; ++i) {
    if (i > 0) {
      val <<= 1;
    }
    val |= read1Bit();
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