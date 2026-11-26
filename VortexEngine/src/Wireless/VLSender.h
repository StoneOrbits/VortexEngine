#ifndef VL_SENDER_H
#define VL_SENDER_H

#include "../Serial/ByteStream.h"
#include "../Serial/BitStream.h"

#include "VLConfig.h"

#if VL_ENABLE_SENDER == 1

class VortexEngine;
class Mode;

class VLSender
{
public:
  VLSender(VortexEngine &engine);
  ~VLSender();

  bool init();
  void cleanup();

  // send a mode
  void send(const Mode *targetMode);
  void sendLegacy(const Mode *targetMode);

private:
  bool loadMode(const Mode *targetMode);
  // send a full 8 bits in a tight loop
  void sendByte(uint8_t data);
  // send full 8 bits legacy protocol
  void sendByteLegacy(uint8_t data);
  // send a mark/space by turning PWM on/off
  void sendMarkSpace(uint16_t markTime, uint16_t spaceTime);
  // turn the VL transmitter on/off in realtime
  void startPWM();
  void stopPWM();

  // reference to engine
  VortexEngine &m_engine;

  // the serial buffer for the data
  ByteStream m_serialBuf;
  // a bit walker for the serial data
  BitStream m_bitStream;

  // some runtime meta info
  uint8_t m_size;
  uint8_t m_parity;
};

#endif

#endif
