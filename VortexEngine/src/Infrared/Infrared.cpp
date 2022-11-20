#include "Infrared.h"

#include "IRReceiver.h"
#include "IRSender.h"

#include "Log/Log.h"

bool Infrared::init()
{
  if (!IRReceiver::init()) {
    ERROR_LOG("Failed to initialize IR Reciever");
    return false;
  }
  if (!IRSender::init()) {
    ERROR_LOG("Failed to initialize IR Sender");
    return false;
  }
  return true;
}

void Infrared::cleanup()
{
  IRReceiver::cleanup();
  IRSender::cleanup();
}
