#pragma once

#include "../Serial/ByteStream.h"
#include "../Serial/BitStream.h"

class Mode;

class IRSender
{
  IRSender();

public:
  static bool init();
  static void cleanup();

  // initialize the IR sender with a mode to send
  static bool loadMode(const Mode *targetMode);
  static void send();

  static bool isSending() { return m_isSending; }

private:
  // Pulse-Width Modulator (IR Transmitter)
  static void initPWM();
  static void startPWM();
  static void stopPWM();
  static void initSend();
  static void write8(uint8_t data);
  static void mark(uint16_t time);
  static void space(uint16_t time);

  // the serial buffer for the data
  static ByteStream m_serialBuf;
  // a bit walker for the serial data
  static BitStream m_bitStream;
  static bool m_isSending;
  static uint64_t m_startTime;

  // some runtime meta info
  static uint32_t m_size;
  // the number of blocks that will be sent
  static uint8_t m_numBlocks;
  // the amount in the final block
  static uint8_t m_remainder;

  // configuration options for the sender
  static uint32_t m_blockSize;

  // write total
  static uint32_t m_writeCounter;
};
