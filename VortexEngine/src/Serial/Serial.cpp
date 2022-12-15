#include "Serial.h"

#include "../Serial/ByteStream.h"
#include "../Time/TimeControl.h"
#include "../Time/Timings.h"
#include "../Log/Log.h"

#include "../VortexEngine.h"

#include <Arduino.h>
#include <stdio.h>

bool SerialComs::m_serialConnected = false;
uint32_t SerialComs::m_lastCheck = 0;

// init serial
bool SerialComs::init()
{
  // Try connecting serial ?
  //checkSerial();
  return true;
}

void SerialComs::cleanup()
{
}

bool SerialComs::isConnected()
{
  return m_serialConnected;
}

// check for any serial connection or messages
bool SerialComs::checkSerial()
{
  if (isConnected()) {
    // already connected
    return true;
  }
  uint64_t now = Time::getCurtime();
  // don't check for serial too fast
  if (m_lastCheck && (now - m_lastCheck) < MAX_SERIAL_CHECK_INTERVAL) {
    // can't check yet too soon
    return false;
  }
  m_lastCheck = now;
  // This will check if the serial communication is open
  if (!Serial) {
    // serial is not connected
    return false;
  }
  // serial is now connected
  m_serialConnected = true;
  // Begin serial communications
  Serial.begin(9600);
  return true;
}

void SerialComs::write(const char *msg, ...)
{
  if (!isConnected()) {
    return;
  }
  va_list list;
  va_start(list, msg);
  char buf[2048] = {0};
  vsnprintf(buf, sizeof(buf), msg, list);
  Serial.print(buf);
  va_end(list);
}

void SerialComs::write(ByteStream &byteStream)
{
  if (!isConnected()) {
    return;
  }
  byteStream.recalcCRC();
  uint32_t size = byteStream.rawSize();
  Serial.write((const uint8_t *)&size, sizeof(size));
  Serial.write((const uint8_t *)byteStream.rawData(), byteStream.rawSize());
}

void SerialComs::read(ByteStream &byteStream)
{
  if (!isConnected()) {
    return;
  }
  uint32_t amt = Serial.available();
  if (!amt) {
    return;
  }
  do {
    uint8_t byte = Serial.read();
    byteStream.serialize(byte);
  } while (--amt > 0);
}

bool SerialComs::dataReady()
{
  if (!isConnected()) {
    return false;
  }
  return (Serial.available() > 0);
}
