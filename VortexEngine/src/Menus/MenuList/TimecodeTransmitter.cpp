#include "TimecodeTransmitter.h"
#include "../../Wireless/VLSender.h"
#include "../../Time/TimeControl.h"

TimecodeTransmitter::TimecodeTransmitter(const RGBColor &col, bool advanced) : 
  Menu(col, advanced)
{
}

TimecodeTransmitter::~TimecodeTransmitter() {
}

bool TimecodeTransmitter::init() {
  return true;
}

Menu::MenuAction TimecodeTransmitter::run() {
  // initialize it with the current mode data
  VLSender::loadData((Time::getCurtime() / 100) % 256);
  // send the first chunk of data, leave if we're done
  VLSender::send();
  return MENU_CONTINUE;
}

// handlers for clicks
void TimecodeTransmitter::onShortClickM()
{
}
void TimecodeTransmitter::onShortClickL()
{
}
void TimecodeTransmitter::onShortClickR()
{
}
void TimecodeTransmitter::onLongClickM()
{
}

void TimecodeTransmitter::beginSendingVL()
{
}

void TimecodeTransmitter::continueSendingVL()
{
}
