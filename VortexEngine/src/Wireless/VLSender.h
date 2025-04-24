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

  // send a mode
  static void send(const Mode *targetMode);

private:
  static bool loadMode(const Mode *targetMode);
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

  // some runtime meta info
  static uint8_t m_size;
};

#endif

#endif
