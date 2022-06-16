#include "ModeSharing.h"

#include "../SerialBuffer.h"
#include "../TimeControl.h"
#include "../Infrared.h"
#include "../Modes.h"
#include "../Mode.h"
#include "../Leds.h"
#include "../Log.h"

ModeSharing::ModeSharing() :
  Menu(),
  m_sharingMode(ModeShareState::SHARE_SEND),
  m_last_action(0)
{
}

bool ModeSharing::init()
{
  if (!Menu::init()) {
    return false;
  }
  // just start spewing out modes everywhere
  m_sharingMode = ModeShareState::SHARE_SEND;
  DEBUG_LOG("Entering Mode Sharing");
  return true;
}

bool ModeSharing::run()
{
  if (!Menu::run()) {
    return false;
  }
  switch (m_sharingMode) {
  case ModeShareState::SHARE_SEND:
    showSendMode();
    // send the mode every 3 seconds
    if ((Time::getCurtime() % Time::secToTicks(3)) == 0) {
      sendMode();
    }
    break;
  case ModeShareState::SHARE_RECEIVE:
    showReceiveMode();
    // wait till a full mode has been received
    if (Infrared::dataReady()) {
      // read the mode out and load it
      receiveMode();
    }
    break;
  }
  return true;
}

// handlers for clicks
void ModeSharing::onShortClick()
{
  switch (m_sharingMode) {
  case ModeShareState::SHARE_SEND:
    // start listening
    Infrared::beginReceiving();
    m_sharingMode = ModeShareState::SHARE_RECEIVE;
    DEBUG_LOG("Switched to receive mode");
    break;
  case ModeShareState::SHARE_RECEIVE:
  default:
    // go to quit option
    Infrared::endReceiving();
    m_sharingMode = ModeShareState::SHARE_SEND;
    DEBUG_LOG("Switched to send mode");
    break;
  }
}

void ModeSharing::onLongClick()
{
  leaveMenu();
}

void ModeSharing::sendMode()
{
  SerialBuffer buf;
  static uint64_t last_time = 0;
  uint64_t now = Time::getCurtime();
  if (last_time && (last_time - now) < Time::msToTicks(300)) {
    return;
  }
  last_time = now;
  m_pCurMode->serialize(buf);
  if (!buf.compress()) {
    DEBUG_LOG("Failed to compress, aborting send");
    return;
    // tried
  }
  // too big
  if (buf.rawSize() > 8000) {
    DEBUG_LOGF("too big: %u", buf.rawSize());
    return;
  }
  DEBUG_LOGF("Writing %u buf", buf.rawSize());
  uint64_t startTime = micros();
  Infrared::write(buf);
  uint64_t endTime = micros();
  DEBUG_LOGF("Wrote %u buf (%u us)", buf.rawSize(), endTime - startTime);
  DEBUG_LOG("Success sending");
}

void ModeSharing::receiveMode()
{
  //uint32_t val = 0;
  // lower 16 is the size of the data to follow
  SerialBuffer buf;
  DEBUG_LOG("Receiving...");
  uint64_t startTime = micros();
  if (!Infrared::read(buf)) {
    // no mode to receive right now
    //DEBUG_LOG("Failed to receive mode");
    return;
  }
  uint64_t endTime = micros();
  DEBUG_LOGF("Received %u bytes (%u us)", buf.rawSize(), endTime - startTime);
  // decompress and check crc at same time
  if (!buf.decompress()) {
    DEBUG_LOG("Failed to decompress, crc mismatch or bad data");
    return;
  }
  buf.resetUnserializer();
  Modes::updateCurMode(buf);
  DEBUG_LOG("Success receiving mode");
  leaveMenu();
}

void ModeSharing::showSendMode()
{
  // gradually fill from pinkie to thumb
  Leds::clearAll();
  // position increases once every 100 ticks to a max of LED_LAST
  LedPos pos = (LedPos)((Time::getCurtime() / Time::msToTicks(100)) % LED_COUNT);
  Leds::setRange(LED_FIRST, pos, RGB_TEAL);
#if 0
  // gradually fill from middle outward
  Leds::clearAll();
  int counter = ((Time::getCurtime() / Time::msToTicks(150)) % 5);
  LedPos pos1 = (LedPos)(MIDDLE_TIP - counter);
  LedPos pos2 = (LedPos)(MIDDLE_TOP + counter);
  Leds::setRange(pos1, pos2, RGB_TEAL);
#endif
}

void ModeSharing::showReceiveMode()
{
  // gradually empty from thumb to pinkie
  Leds::clearAll();
  // position decreases once every 100 ticks from LED_LAST to 0
  LedPos pos = (LedPos)(LED_LAST - ((Time::getCurtime() / Time::msToTicks(100)) % LED_COUNT));
  Leds::setRange(LED_FIRST, pos, RGB_PURPLE);
#if 0
  // gradually empty from outward to middle
  Leds::clearAll();
  int counter = ((Time::getCurtime() / Time::msToTicks(150)) % 5);
  LedPos pos1 = (LedPos)(MIDDLE_TIP - (4 - counter));
  LedPos pos2 = (LedPos)(MIDDLE_TOP + (4 - counter));
  Leds::setRange(pos1, pos2, RGB_PURPLE);
#endif
}
