#include "ModeSharing.h"

#include "../../Serial/ByteStream.h"
#include "../../Serial/Serial.h"
#include "../../Modes/ModeBuilder.h"
#include "../../Time/TimeControl.h"
#include "../../Time/Timings.h"
#include "../../Infrared/IRReceiver.h"
#include "../../Infrared/IRSender.h"
#include "../../Modes/Modes.h"
#include "../../Modes/Mode.h"
#include "../../Leds/Leds.h"
#include "../../Log/Log.h"

ModeSharing::ModeSharing() :
  Menu(),
  m_sharingMode(ModeShareState::SHARE_SEND),
  // This makes send mode begin with waiting instead of sending
  m_lastActionTime(Time::getCurtime() + 1),
  m_timeOutStartTime(0)
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
    if (!IRSender::isSending()) {
      if (!m_lastActionTime || ((m_lastActionTime + MAX_WAIT_DURATION) < Time::getCurtime())) {
        Leds::setAll(RGB_TEAL);
        Leds::update();
        beginSending();
      }
    }
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
  Leds::clearAll();
}

void ModeSharing::onLongClick()
{
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
    // when send has completed, stores time that last action was completed to calculate interval between sends
    m_lastActionTime = Time::getCurtime();
  }
}

void ModeSharing::continueSending()
{
  // if the sender isn't done then keep sending data
  if (IRSender::isSending()) {
    if (!IRSender::send()) {
      // when send has completed, stores time that last action was completed to calculate interval between sends
      m_lastActionTime = Time::getCurtime();
    }
  }
}

void ModeSharing::receiveMode()
{ 
  // if reveiving new data set our last data time
  if (IRReceiver::onNewData()) {
    m_timeOutStartTime = Time::getCurtime();
    // if our last data was more than time out duration reset the recveiver
  } else if (m_timeOutStartTime > 0 && (m_timeOutStartTime + MAX_TIMEOUT_DURATION) < Time::getCurtime()) {
    IRReceiver::resetIRState();
    m_timeOutStartTime = 0;
    return;
  }
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
  // leave menu and save settings, even if the mode was the same whatever
  leaveMenu(true);
}

void ModeSharing::showSendMode()
{
  // if it is sending
  if (IRSender::isSending()) {
    Leds::setAll(RGB_TEAL);
  } else {    
    Leds::setAll(RGB_BLANK);
    Leds::blinkAll(Time::getCurtime(), 250, 250);
  } 
}

void ModeSharing::showReceiveMode()
{
  Leds::clearAll();
  if (IRReceiver::isReceiving()) {
    // how much is sent?
    uint32_t percent = IRReceiver::percentReceived();
    LedPos l = (LedPos)(percent / 10);
    Leds::setRange(LED_FIRST, l, RGB_GREEN);
    return;
  }
  // gradually empty from thumb to pinkie
  LedPos pos = (LedPos)(LED_COUNT - (Time::getCurtime() / Time::msToTicks(200) % (LED_COUNT + 1)));
  if (pos == 10) return;
  Leds::setRange(LED_FIRST, pos, RGB_PURPLE);
}

// override showExit so it isn't displayed on thumb
void ModeSharing::showExit()
{
}
