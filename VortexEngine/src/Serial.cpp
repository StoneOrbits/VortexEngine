#include "Serial.h"

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
  // only try to check serial every 1000 ticks otherwise lag
  if ((Time::getCurtime() % 1000) != 0) {
    return;
  }
  if (!Serial) {
    return;
  }
  m_serial_init = true;
  // Setup serial communications
  Serial.begin(9600);
}
