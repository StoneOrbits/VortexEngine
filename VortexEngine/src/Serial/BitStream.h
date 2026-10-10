#ifndef BITSTREAM_H
#define BITSTREAM_H

#include <inttypes.h>
#include <stdbool.h>

// A struct to read/write a buffer of bits one bit at a time
typedef struct BitStream {
  uint8_t *m_buf;
  uint16_t m_buf_size;
  uint16_t m_bit_pos;
  bool m_buf_eof;
  bool m_allocated;
} BitStream;

void BitStream_make(BitStream *self);
void BitStream_makeSize(BitStream *self, uint32_t size);
void BitStream_makeBuf(BitStream *self, uint8_t *buf, uint32_t size);
void BitStream_destroy(BitStream *self);

// init the stream with a buffer
bool BitStream_initBuf(BitStream *self, uint8_t *buf, uint32_t size);
// init the stream and allocate a buffer
bool BitStream_initSize(BitStream *self, uint32_t size);

// clear the target buffer to 0 and reset position
void BitStream_reset(BitStream *self);
// reset the reader/writer position
void BitStream_resetPos(BitStream *self);

// read/write a single bit in LSB
uint8_t BitStream_read1Bit(BitStream *self);
void BitStream_write1Bit(BitStream *self, bool bit);
// read/write multiple bits from left to right at LSB
uint8_t BitStream_readBits(BitStream *self, uint32_t numBits);
void BitStream_writeBits(BitStream *self, uint32_t numBits, uint32_t val);

// metainfo about the bit stream
static inline bool BitStream_eof(const BitStream *self) { return self->m_buf_eof; }
static inline bool BitStream_allocated(const BitStream *self) { return self->m_allocated; }
static inline uint16_t BitStream_size(const BitStream *self) { return self->m_buf_size; }
static inline const uint8_t *BitStream_data(const BitStream *self) { return self->m_buf; }
static inline uint8_t BitStream_peekData(const BitStream *self, uint8_t pos) { return self->m_buf[pos]; }
static inline const uint32_t *BitStream_dwData(const BitStream *self) { return (const uint32_t *)self->m_buf; }
static inline uint16_t BitStream_dwordpos(const BitStream *self) { return self->m_bit_pos / 32; }
static inline uint16_t BitStream_bytepos(const BitStream *self) { return self->m_bit_pos / 8; }
static inline uint16_t BitStream_bitpos(const BitStream *self) { return self->m_bit_pos; }

#endif
