#ifndef VL_SENDER_H
#define VL_SENDER_H

#include "../Serial/ByteStream.h"
#include "../Serial/BitStream.h"

#include "VLConfig.h"

#if VL_ENABLE_SENDER == 1

class Mode;

class VLSender
{
  VLSender();

public:
  static bool init();
  static void cleanup();

  // initialize the VL sender with a mode to send
  static bool loadMode(const Mode *targetMode);
  static bool send();

  static bool isSending() { return m_isSending; }

  static uint32_t percentDone() { return (uint32_t)(((float)m_writeCounter / (float)m_size) * 100.0); }

private:
  // sender functions
  static void beginSend();
  // send a full 8 bits in a tight loop
  static void sendByte(uint8_t data);
  // send a mark/space by turning PWM on/off
  static void sendMarkSpace(uint16_t markTime, uint16_t spaceTime);
  // turn the VL transmitter on/off in realtime
  static void startPWM();
  static void stopPWM();

  // the serial buffer for the data
  static ByteStream m_serialBuf;
  // a bit walker for the serial data
  static BitStream m_bitStream;
  static bool m_isSending;
  static uint32_t m_lastSendTime;

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

#endif

#endif
