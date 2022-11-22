#include "ModeSharing.h"

#include "../../Serial/ByteStream.h"
#include "../../Modes/ModeBuilder.h"
#include "../../Time/TimeControl.h"
#include "../../Infrared/IRReceiver.h"
#include "../../Infrared/IRSender.h"
#include "../../Modes/Modes.h"
#include "../../Modes/Mode.h"
#include "../../Leds/Leds.h"
#include "../../Log/Log.h"

// comment this out to use manual click-to-send
#define AUTO_SEND_MODE
#ifdef AUTO_SEND_MODE
bool sent_once = false;
#endif

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

  // TODO: removeme
#ifdef TEST_FRAMEWORK
  if (is_ir_server()) {
    m_sharingMode = ModeShareState::SHARE_RECEIVE;
  }
#endif

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
#ifdef AUTO_SEND_MODE
    // begin sending the mode every 3 seconds
    if ((Time::getCurtime() % Time::secToTicks(3)) == 0 && sent_once == false) {
      sent_once = true;
      beginSending();
    }
#endif
    // continue sending any data as long as there is more to send
    continueSending();
    break;
  case ModeShareState::SHARE_RECEIVE:
    // render the 'receive mode' lights
    showReceiveMode();
    // load any modes that are received
    receiveMode();
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
    IRReceiver::beginReceiving();
    m_sharingMode = ModeShareState::SHARE_RECEIVE;
    DEBUG_LOG("Switched to receive mode");
    break;
  case ModeShareState::SHARE_RECEIVE:
  default:
    // click while on receive -> end receive, start sending
    IRReceiver::endReceiving();
    m_sharingMode = ModeShareState::SHARE_SEND;
    DEBUG_LOG("Switched to send mode");
    break;
  }
}

void ModeSharing::onLongClick()
{
  // long click on sender option to manually send the mode
  if (m_sharingMode == ModeShareState::SHARE_SEND) {
    beginSending();
    return;
  }
  leaveMenu();
}

void ModeSharing::beginSending()
{
  // if the sender is sending then cannot start again
  if (IRSender::isSending()) {
    ERROR_LOG("Cannot begin sending, sender is busy");
    return;
  }
  // initialize it with the current mode data
  IRSender::loadMode(m_pCurMode);
  // send the first chunk of data, leave if we're done
  if (!IRSender::send()) {
    leaveMenu();
  }
}

void ModeSharing::continueSending()
{
  // if the sender isn't done then keep sending data
  if (IRSender::isSending()) {
    if (!IRSender::send()) {
      leaveMenu();
    }
  }
}

void ModeSharing::receiveMode()
{
  // check if the IRReceiver has a full packet available
  if (!IRReceiver::dataReady()) {
    // nothing available yet
    return;
  }
  DEBUG_LOG("Mode ready to receive! Receiving...");
  // receive the IR mode into the current mode
  if (!IRReceiver::receiveMode(m_pCurMode)) {
    ERROR_LOG("Failed to receive mode");
    leaveMenu();
    return;
  }
  DEBUG_LOGF("Success receiving mode: %u", m_pCurMode->getPatternID());
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
