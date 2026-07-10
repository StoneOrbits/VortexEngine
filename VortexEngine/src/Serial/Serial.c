#include "Serial.h"

#include "../Serial/ByteStream.h"
#include "../Time/TimeControl.h"
#include "../Time/Timings.h"
#include "../Log/Log.h"

#include "../VortexEngine.h"

#ifdef VORTEX_LIB
#include "VortexLib.h"
#include <stdio.h>
#endif

static bool m_serialConnected = false;
static uint32_t m_lastCheck = 0;

bool SerialComs_init()
{
  return true;
}

void SerialComs_cleanup()
{
}

bool SerialComs_isConnected()
{
  return m_serialConnected;
}

bool SerialComs_checkSerial()
{
#if VORTEX_SLIM == 0
  if (SerialComs_isConnected()) {
    return true;
  }
  uint32_t now = Time_getCurtime();
  if (m_lastCheck && (now - m_lastCheck) < MAX_SERIAL_CHECK_INTERVAL) {
    return false;
  }
  m_lastCheck = now;
#ifdef VORTEX_LIB
  if (!Vortex_vcallbacks()->serialCheck()) {
    return false;
  }
  Vortex_vcallbacks()->serialBegin(SERIAL_BAUD_RATE);
#else
  if (!Serial.available()) {
    return false;
  }
  Serial.begin(SERIAL_BAUD_RATE);
#endif
#endif
  m_serialConnected = true;
  return true;
}

void SerialComs_write(const char *msg, ...)
{
#if VORTEX_SLIM == 0
  if (!SerialComs_isConnected()) {
    return;
  }
  va_list list;
  va_start(list, msg);
  uint8_t buf[2048] = {0};
  int len = vsnprintf((char *)buf, sizeof(buf), msg, list);
#ifdef VORTEX_LIB
  Vortex_vcallbacks()->serialWrite(buf, len);
#else
  Serial.write(buf, len);
  Serial.flush();
#endif
  va_end(list);
#endif
}

void SerialComs_writeStream(ByteStream *byteStream)
{
#if VORTEX_SLIM == 0
  if (!SerialComs_isConnected()) {
    return;
  }
  ByteStream_recalcCRC(byteStream, false);
  uint32_t size = ByteStream_rawSize(byteStream);
#ifdef VORTEX_LIB
  Vortex_vcallbacks()->serialWrite((const uint8_t *)&size, sizeof(size));
  Vortex_vcallbacks()->serialWrite((const uint8_t *)ByteStream_rawData(byteStream), ByteStream_rawSize(byteStream));
#else
  Serial.write((const uint8_t *)&size, sizeof(size));
  Serial.write((const uint8_t *)ByteStream_rawData(byteStream), ByteStream_rawSize(byteStream));
  Serial.flush();
#endif
#endif
}

void SerialComs_read(ByteStream *byteStream)
{
#if VORTEX_SLIM == 0
  if (!SerialComs_isConnected()) {
    return;
  }
#ifdef VORTEX_LIB
  uint32_t amt = Vortex_vcallbacks()->serialAvail();
#else
  uint32_t amt = Serial.available();
#endif
  if (!amt) {
    return;
  }
  do {
    uint8_t byte = 0;
#ifdef VORTEX_LIB
    if (!Vortex_vcallbacks()->serialRead((char *)&byte, 1)) {
      return;
    }
#else
    byte = Serial.read();
#endif
    ByteStream_serialize8(byteStream, byte);
  } while (--amt > 0);
#endif
}

bool SerialComs_dataReady()
{
#if VORTEX_SLIM == 0
  if (!SerialComs_isConnected()) {
    return false;
  }
#ifdef VORTEX_LIB
  return (Vortex_vcallbacks()->serialAvail() > 0);
#else
  return (Serial.available() > 0);
#endif
#else
  return false;
#endif
}
