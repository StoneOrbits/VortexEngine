#include "Infrared.h"

#include "TimeControl.h"
#include "Buttons.h"
#include "Log.h"

#include <Arduino.h>

#include "Leds.h"

#include <IRLibSendBase.h>
#include <IRLibRecvLoop.h>
#include <IRLibDecodeBase.h>
#include <IRLib_P01_NEC.h>

#define RECEIVER_PIN 2
IRrecvLoop myReceiver(RECEIVER_PIN);
IRdecodeNEC myDecoder;
IRsendNEC mySender;

Infrared::Infrared()
{
}

bool Infrared::init()
{
  myReceiver.enableIRIn();
  return true;
}

void Infrared::cleanup()
{
  // turn off the infrared ISR
  myReceiver.disableIRIn();
}

uint32_t Infrared::read()
{
  uint32_t result = 0;
  if (!myReceiver.getResults()) {
    return false;
  }
  myDecoder.ignoreHeader=true;
  myDecoder.decode();
  myDecoder.dumpResults(true);
  result = myDecoder.value;
  myReceiver.enableIRIn();
  DEBUG_LOGF("IR Read: %u", result);
  return result;
}

bool Infrared::write(uint32_t val)
{
  mySender.send(val);
  return true;
}