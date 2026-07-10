#ifndef BITSTREAM_H
#define BITSTREAM_H

#include <inttypes.h>
#include <stdbool.h>

typedef struct BitStream
{
  uint8_t *buf;
  uint16_t buf_size;
  uint16_t bit_pos;
  bool buf_eof;
  bool allocated;
} BitStream;

void BitStream_init(BitStream *self);
void BitStream_initBuf(BitStream *self, uint8_t *buf, uint32_t size);
bool BitStream_initAlloc(BitStream *self, uint32_t size);
void BitStream_destroy(BitStream *self);

void BitStream_reset(BitStream *self);
void BitStream_resetPos(BitStream *self);

uint8_t BitStream_read1Bit(BitStream *self);
void BitStream_write1Bit(BitStream *self, bool bit);
uint8_t BitStream_readBits(BitStream *self, uint32_t numBits);
void BitStream_writeBits(BitStream *self, uint32_t numBits, uint32_t val);

static inline bool BitStream_eof(const BitStream *self) { return self->buf_eof; }
static inline bool BitStream_allocated(const BitStream *self) { return self->allocated; }
static inline uint16_t BitStream_size(const BitStream *self) { return self->buf_size; }
static inline const uint8_t *BitStream_data(const BitStream *self) { return self->buf; }
static inline uint8_t BitStream_peekData(const BitStream *self, uint8_t pos) { return self->buf[pos]; }
static inline const uint32_t *BitStream_dwData(const BitStream *self) { return (const uint32_t *)self->buf; }
static inline uint16_t BitStream_dwordpos(const BitStream *self) { return self->bit_pos / 32; }
static inline uint16_t BitStream_bytepos(const BitStream *self) { return self->bit_pos / 8; }
static inline uint16_t BitStream_bitpos(const BitStream *self) { return self->bit_pos; }

#endif
