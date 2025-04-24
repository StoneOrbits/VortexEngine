#include "VLSender.h"
#include "IRConfig.h"

#if VL_ENABLE_SENDER == 1

#include "../Time/TimeControl.h"
#include "../Modes/Mode.h"
#include "../Leds/Leds.h"
#include "../Log/Log.h"

#ifdef VORTEX_LIB
#include "VortexLib.h"
#endif

// the serial buffer for the data
ByteStream VLSender::m_serialBuf;
// a bit walker for the serial data
BitStream VLSender::m_bitStream;
// some runtime meta info
uint8_t VLSender::m_size = 0;

bool VLSender::init()
{
  return true;
}

void VLSender::cleanup()
{
}

bool VLSender::loadMode(const Mode *targetMode)
{
  m_serialBuf.clear();
  // save the target mode to it's savefile buffer format
  if (!targetMode->saveToBuffer(m_serialBuf, 2)) {
    DEBUG_LOG("Failed to save mode to buffer");
    return false;
  }
  // decompress the mode on lifi, no compression here
  if (!m_serialBuf.decompress()) {
    DEBUG_LOG("Failed to decompress VL buf for sending");
    return false;
  }
  // ensure the data isn't too big
  if (m_serialBuf.rawSize() > VL_MAX_DATA_TRANSFER) {
    DEBUG_LOGF("Cannot transfer that much data: %u bytes", m_serialBuf.rawSize());
    return false;
  }
  // point the bitstream at the serial buffer
  m_bitStream.init((uint8_t *)m_serialBuf.rawData(), m_serialBuf.rawSize());
  // the size of the buf should never really exceed 256
  m_size = (uint8_t)m_serialBuf.rawSize();
  DEBUG_LOGF("Size: %u", m_size);
  return true;
}

void VLSender::send(const Mode *targetMode)
{
  loadMode(targetMode);
  // now send the header
  sendMarkSpace(VL_HEADER_MARK, VL_HEADER_SPACE);
  // send some sync bytes to let the receiver determine baudrate, this will
  // send: (zero mark - zero space - one mark - one space) x 2
  // then the receiver will add up all the marks and all the spaces and divide
  // each by 4 to get the middle point between the one/zero for both marks and spaces
  for (uint8_t b = 0; b < 2; b++) {
    sendMarkSpace(VL_TIMING_BIT_ZERO, VL_TIMING_BIT_ZERO);
    sendMarkSpace(VL_TIMING_BIT_ONE, VL_TIMING_BIT_ONE);
  }
  // send the size of the data first
  sendByte(m_size);
  // iterate each byte in the block and write it
  for (uint8_t i = 0; i < m_size; ++i) {
    // write the byte
    sendByte(m_bitStream.peekData(i));
  }
  // send one last blink because the previous data could end on a space
  // so the receiver needs a last blink to determine it's length
  sendMarkSpace(VL_TIMING_BIT_ZERO, VL_TIMING_BIT_ZERO);
}

void VLSender::sendByte(uint8_t data)
{
  // Sends from left to right, MSB first
  for (uint8_t i = 0; i < 8; i += 2) {
    uint8_t mark = (data >> (7 - i)) & 1;
    uint8_t space = (data >> (6 - i)) & 1;
    sendMarkSpace(VL_TIMING_BIT(mark), VL_TIMING_BIT(space));
  }
 }

void VLSender::sendMarkSpace(uint16_t markTime, uint16_t spaceTime)
{
#ifdef VORTEX_LIB
  // send mark timing over socket
  Vortex::vcallbacks()->infraredWrite(true, markTime);
  // send space timing over socket
  Vortex::vcallbacks()->infraredWrite(false, spaceTime);
#else
  startPWM();
  Time::delayMicroseconds(markTime);
  stopPWM();
  Time::delayMicroseconds(spaceTime);
#endif
}

void VLSender::startPWM()
{
#if defined(VORTEX_EMBEDDED)
  // brightness backup
  uint8_t oldBrightness = Leds::getBrightness();
  // ensure max brightness
  Leds::setBrightness(255);
  Leds::setAll(RGB_WHITE);
  Leds::update();
  // restore brightness
  Leds::setBrightness(oldBrightness);
#endif
}

void VLSender::stopPWM()
{
#if defined(VORTEX_EMBEDDED)
  Leds::clearAll();
  Leds::update();
#endif
}

#endif
