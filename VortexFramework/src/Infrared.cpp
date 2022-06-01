#include "Infrared.h"

#include "TimeControl.h"
#include "Log.h"

#include <Arduino.h>

#include <IRLibSendBase.h>
#include <IRLibDecodeBase.h>
#include <IRLib_P01_NEC.h>    
#include <IRLibCombo.h>
#include <IRLibRecv.h>

#define RECEIVER_PIN 2

IRdecode myDecoder;
IRrecv myReceiver(RECEIVER_PIN);
IRsend mySender;

Infrared::Infrared()
{
}

bool Infrared::init()
{
  return true;
}

void Infrared::cleanup()
{
}

bool Infrared::read()
{
  uint32_t result = 0;
  myReceiver.enableIRIn();
  if (!myReceiver.getResults()) {
    return false;
  }
  myDecoder.decode();
  myDecoder.dumpResults(true);
  result = myDecoder.value;
  DEBUGF("IR Read: %u", result);
  return true;
}

bool Infrared::write()
{
  mySender.send(NEC, 0xFFFFFFFF, 0);
  DEBUG("IR Send: ");
  return true;
}