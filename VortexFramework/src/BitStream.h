#ifndef BITSTREAM_H
#define BITSTREAM_H

#include <inttypes.h>

// A class to read/write a buffer of bits one bit at a time
class BitStream
{
public:
  BitStream();
  BitStream(uint8_t *buf, uint32_t size);

  // init the stream with a buffer
  void init(uint8_t *buf, uint32_t size);

  // reset the reader/writer position
  void resetPos();

  // read write a single bit in LSB
  uint8_t read1Bit();
  void write1Bit(uint8_t bit);
  // read/write multiple bits from left to right at LSB
  uint8_t readBits(uint32_t numBits);
  void writeBits(uint32_t numBits, uint32_t val);

  // metainfo about the bit stream
  bool eof() const { return m_buf_eof; }
  uint32_t size() const { return m_buf_size; }
  const uint8_t *data() const { return m_buf; }
  uint32_t bytepos() const { return m_bit_pos / 8; }
  uint32_t bitpos() const { return m_bit_pos; }

private:
  uint8_t *m_buf;
  uint32_t m_buf_size;
  uint32_t m_bit_pos;
  bool m_buf_eof;
};

#endif
