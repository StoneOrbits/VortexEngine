#include "Serial.h"

#include "../Time/TimeControl.h"
#include "../Log/Log.h"

#include "../VortexEngine.h"

#include <Arduino.h>

bool SerialComs::m_serialConnected = false;

// private constructor
SerialComs::SerialComs()
{
}

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

// check for any serial connection or messages
bool SerialComs::checkSerial()
{
  if (m_serialConnected) {
    return false;
  }
  if (!Serial) {
    return false;
  }
  m_serialConnected = true;
  // Setup serial communications
  Serial.begin(9600);
  INFO_LOG("== Vortex Framework v" VORTEX_VERSION " (built " __TIMESTAMP__ ") ==");
  return true;
}

bool SerialComs::isConnected()
{
  //if (!m_serialConnected) {
  //  return checkSerial();
  //}
  return m_serialConnected;
}


