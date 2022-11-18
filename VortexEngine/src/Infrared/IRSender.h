#pragma once

#include "../Serial/BitStream.h"

#include "IRConfig.h"

class Mode;

class IRSender
{
public:
  IRSender(uint32_t block_size_bits = DEFAULT_IR_BLOCK_SIZE);
  IRSender(const Mode *targetMode);
  ~IRSender();

  // initialize the IR sender with a serialized mode to send
  bool init(const Mode *targetMode);
  void send();

  bool isSending() const { return m_isSending; }

private:
  bool initSend();

  // the serial buffer for the data
  ByteStream m_serialBuf;
  // a bit walker for the serial data
  BitStream m_bitStream;
  bool m_isSending;
  uint64_t m_startTime;

  // some runtime meta info
  uint32_t m_size;
  // the number of blocks that will be sent
  uint8_t m_numBlocks;
  // the amount in the final block
  uint8_t m_remainderBlocks;

  // configuration options for the sender
  uint32_t m_blockSize;
};
