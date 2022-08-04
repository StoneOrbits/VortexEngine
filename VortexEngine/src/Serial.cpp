#include "Serial.h"

#include "VortexEngine.h"
#include "TimeControl.h"
#include "Log.h"

#include <Arduino.h>

bool SerialComs::m_serial_init = false;

char backlog[4096] = {0};

// private constructor
SerialComs::SerialComs()
{
}

// init serial
bool SerialComs::init()
{
  // Try connecting serial 
  checkSerial();
  return true;
}

void SerialComs::cleanup()
{
}

// check for any serial connection or messages
void SerialComs::checkSerial()
{
  if (m_serial_init) {
    return;
  }
  if (!Serial) {
    return;
  }
  m_serial_init = true;
  // Setup serial communications
  Serial.begin(9600);
  INFO_LOG("== Vortex Framework v" VORTEX_VERSION " (built " __TIMESTAMP__ ") ==");
}
