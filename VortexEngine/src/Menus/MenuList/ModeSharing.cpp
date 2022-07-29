#include "ModeSharing.h"

#include "../../Serial/SerialBuffer.h"
#include "../../Time/TimeControl.h"
#include "../../Infrared/Infrared.h"
#include "../../Modes/Modes.h"
#include "../../Modes/Mode.h"
#include "../../Leds/Leds.h"
#include "../../Log/Log.h"

// comment this out to use manual click-to-send
//#define AUTO_MODE_SENDING

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
    // render the 'send mode' lights
    showSendMode();
    // if already sending a mode then continue that operation
    if (m_irSender.isSending()) {
      sendMode();
      return true;
    }
#ifdef AUTO_MODE_SENDING
    // send the mode every 3 seconds
    if ((Time::getCurtime() % Time::secToTicks(3)) == 0) {
      sendMode();
    }
#endif
    break;
  case ModeShareState::SHARE_RECEIVE:
    // render the 'receive mode' lights
    showReceiveMode();
    // if the infrared receiver has received a full packet with a mode
    if (Infrared::dataReady()) {
      // then read the mode out of the IR receiver and load it
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
    // click while on send -> start listening
    Infrared::beginReceiving();
    m_sharingMode = ModeShareState::SHARE_RECEIVE;
    DEBUG_LOG("Switched to receive mode");
    break;
  case ModeShareState::SHARE_RECEIVE:
  default:
    // click while on receive -> end receive, start sending
    Infrared::endReceiving();
    m_sharingMode = ModeShareState::SHARE_SEND;
    DEBUG_LOG("Switched to send mode");
    break;
  }
}

void ModeSharing::onLongClick()
{
#ifndef AUTO_MODE_SENDING
  // long click on sender option to manually send the mode
  if (m_sharingMode == ModeShareState::SHARE_SEND) {
    sendMode();
    return;
  }
#endif
  leaveMenu();
}

void ModeSharing::sendMode()
{
  // if the sender isn't sending yet
  if (!m_irSender.isSending()) {
    // initialize it with the current mode data
    m_irSender.init(m_pCurMode);
  }
  // send the data
  m_irSender.send();
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
  m_pCurMode->unserialize(buf);
  m_pCurMode->init();
  DEBUG_LOG("Success receiving mode");
  // leave menu and save settings
  leaveMenu(true);
}

void ModeSharing::showSendMode()
{
  // gradually fill from thumb to pinkie
  Leds::clearAll();
  LedPos pos = (LedPos)(LED_COUNT - (Time::getCurtime() / Time::msToTicks(100) % (LED_COUNT + 1)));
  if (pos == 10) return;
  Leds::setRange(pos, LED_LAST, RGB_TEAL);
}

void ModeSharing::showReceiveMode()
{
  // gradually empty from thumb to pinkie
  Leds::clearAll();
  LedPos pos = (LedPos)(LED_COUNT - (Time::getCurtime() / Time::msToTicks(200) % (LED_COUNT + 1)));
  if (pos == 10) return;
  Leds::setRange(LED_FIRST, pos, RGB_PURPLE);
}
