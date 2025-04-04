#ifndef BITSTREAM_H
#define BITSTREAM_H

#include <inttypes.h>

// A class to read/write a buffer of bits one bit at a time
class BitStream
{
public:
  BitStream();
  BitStream(uint32_t size);
  BitStream(uint8_t *buf, uint32_t size);
  ~BitStream();

  // init the stream with a buffer
  bool init(uint8_t *buf, uint32_t size);
  // init the stream and allocate a buffer
  bool init(uint32_t size);

  // clear the target buffer to 0 and reset position
  void reset();
  // reset the reader/writer position
  void resetPos();

  // read/write a single bit in LSB
  uint8_t read1Bit();
  void write1Bit(bool bit);
  // read/write multiple bits from left to right at LSB
  uint8_t readBits(uint32_t numBits);
  void writeBits(uint32_t numBits, uint32_t val);

  // metainfo about the bit stream
  bool eof() const { return m_buf_eof; }
  bool allocated() const { return m_allocated; }
  uint16_t size() const { return m_buf_size; }
  const uint8_t *data() const { return m_buf; }
  const uint32_t *dwData() const { return (uint32_t *)m_buf; }
  uint16_t dwordpos() const { return m_bit_pos / 32; }
  uint16_t bytepos() const { return m_bit_pos / 8; }
  uint16_t bitpos() const { return m_bit_pos; }

private:
  uint8_t *m_buf;
  uint16_t m_buf_size;
  uint16_t m_bit_pos;
  bool m_buf_eof;
  bool m_allocated;
};

#endif
