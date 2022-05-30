#include "InfraredControl.h"

#include "TimeControl.h"
#include "Log.h"

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
  myReceiver.enableIRIn();
  return true;
}

bool Infrared::read()
{
  if (myReceiver.getResults()) {
    myDecoder.decode();
    myDecoder.dumpResults(true);
    DEBUGF("Decoded: %u", myDecoder.value);
    myReceiver.enableIRIn();      //Restart receiver
  }

  return true;
}

bool Infrared::write()
{
  if (Time::getCurtime() % 100) {
    mySender.send(NEC, (uint32_t)Time::getCurtime());
  }
  return true;
}