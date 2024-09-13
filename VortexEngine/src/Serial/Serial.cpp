#include "Serial.h"

#include "../Serial/ByteStream.h"
#include "../Time/TimeControl.h"
#include "../Time/Timings.h"
#include "../Modes/Modes.h"
#include "../Menus/MainMenu.h"
#include "../Menus/Menus.h"
#include "../Log/Log.h"

#include "../VortexEngine.h"

#ifdef VORTEX_LIB
#include "VortexLib.h"
#include <stdio.h>
#endif

#ifdef VORTEX_EMBEDDED
#include <Arduino.h>
#include "soc/usb_serial_jtag_reg.h"
#include "HWCDC.h"
#endif

bool SerialComs::m_serialConnected = false;
uint32_t SerialComs::m_lastCheck = 0;
uint32_t SerialComs::m_lastConnected = 0;

// init serial
bool SerialComs::init()
{
  return true;
}

void SerialComs::cleanup()
{
}

bool SerialComs::isConnected()
{
#ifdef VORTEX_EMBEDDED
  if (!isConnectedReal()) {
    m_serialConnected = false;
    return false;
  }
#endif
  return m_serialConnected;
}

bool SerialComs::isConnectedReal()
{
#ifdef VORTEX_EMBEDDED
  //return usb_serial_jtag_is_connected();
  return HWCDCSerial.isConnected();
#else
  return true;
#endif
}

// check for any serial connection or messages
bool SerialComs::checkSerial()
{
#if VORTEX_SLIM == 0
  if (isConnected()) {
    // already connected
    return true;
  }
  uint32_t now = Time::getCurtime();
  // don't check for serial too fast
  if (m_lastCheck && (now - m_lastCheck) < MAX_SERIAL_CHECK_INTERVAL) {
    // can't check yet too soon
    return false;
  }
  m_lastCheck = now;
#ifdef VORTEX_LIB
  if (!Vortex::vcallbacks()->serialCheck()) {
    return false;
  }
  Vortex::vcallbacks()->serialBegin(SERIAL_BAUD_RATE);
#else
  // Begin serial communications (turns out this is actually a NO-OP in trinket source)
  Serial.begin(SERIAL_BAUD_RATE);
  // This will check if the serial communication is open
  if (!Serial && !Serial.available()) {
    // serial is not connected
    return false;
  }
#endif
#endif
  // serial is now connected
  m_serialConnected = true;
  // rely on the low level 'real' connection now
  return isConnectedReal();
}

void SerialComs::write(const char *msg, ...)
{
#if VORTEX_SLIM == 0
  if (!isConnected()) {
    return;
  }
  va_list list;
  va_start(list, msg);
  uint8_t buf[2048] = {0};
  int len = vsnprintf((char *)buf, sizeof(buf), msg, list);
#ifdef VORTEX_LIB
  Vortex::vcallbacks()->serialWrite(buf, len);
#else
  Serial.write(buf, len);
#endif
  va_end(list);
#endif
}

void SerialComs::write(ByteStream &byteStream)
{
#if VORTEX_SLIM == 0
  if (!isConnected()) {
    return;
  }
  byteStream.recalcCRC();
  uint32_t size = byteStream.rawSize();
#ifdef VORTEX_LIB
  Vortex::vcallbacks()->serialWrite((const uint8_t *)&size, sizeof(size));
  Vortex::vcallbacks()->serialWrite((const uint8_t *)byteStream.rawData(), byteStream.rawSize());
#else
  Serial.write((const uint8_t *)&size, sizeof(size));
  Serial.write((const uint8_t *)byteStream.rawData(), byteStream.rawSize());
#endif
#endif
}

void SerialComs::read(ByteStream &byteStream)
{
#if VORTEX_SLIM == 0
  if (!isConnected()) {
    return;
  }
#ifdef VORTEX_LIB
  uint32_t amt = Vortex::vcallbacks()->serialAvail();
#else
  uint32_t amt = Serial.available();
#endif
  if (!amt) {
    return;
  }
  do {
    uint8_t byte = 0;
#ifdef VORTEX_LIB
    if (!Vortex::vcallbacks()->serialRead((char *)&byte, 1)) {
      return;
    }
#else
    byte = Serial.read();
#endif
    byteStream.serialize8(byte);
  } while (--amt > 0);
#endif
}

bool SerialComs::dataReady()
{
#if VORTEX_SLIM == 0
  if (!isConnected()) {
    return false;
  }
#ifdef VORTEX_LIB
  return (Vortex::vcallbacks()->serialAvail() > 0);
#else
  return (Serial.available() > 0);
#endif
#else
  return false;
#endif
}
