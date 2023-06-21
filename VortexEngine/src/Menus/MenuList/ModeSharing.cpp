#include "ModeSharing.h"

#include "../../Serial/ByteStream.h"
#include "../../Serial/Serial.h"
#include "../../Time/TimeControl.h"
#include "../../Time/Timings.h"
#include "../../Wireless/VLReceiver.h"
#include "../../Wireless/VLSender.h"
#include "../../Modes/Modes.h"
#include "../../Modes/Mode.h"
#include "../../Leds/Leds.h"
#include "../../Log/Log.h"

ModeSharing::ModeSharing(const RGBColor &col) :
  Menu(col),
  m_sharingMode(ModeShareState::SHARE_SEND),
  m_timeOutStartTime(0)
{
}

ModeSharing::~ModeSharing()
{
}

bool ModeSharing::init()
{
  if (!Menu::init()) {
    return false;
  }
  // skip led selection
  m_ledSelected = true;
  // start on receive because it's the more responsive of the two
  // the odds of opening receive and then accidentally receiving
  // a mode that is being broadcast nearby is completely unlikely
  beginReceiving();
  DEBUG_LOG("Entering Mode Sharing");
  return true;
}

Menu::MenuAction ModeSharing::run()
{
  MenuAction result = Menu::run();
  if (result != MENU_CONTINUE) {
    return result;
  }
  switch (m_sharingMode) {
  case ModeShareState::SHARE_SEND:
    // render the 'send mode' lights
    showSendMode();
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
  return MENU_CONTINUE;
}

// handlers for clicks
void ModeSharing::onShortClick()
{
  switch (m_sharingMode) {
  case ModeShareState::SHARE_RECEIVE:
    // click while on receive -> end receive, start sending
    VLReceiver::endReceiving();
    beginSending();
    DEBUG_LOG("Switched to send mode");
    break;
  default:
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
  if (VLSender::isSending()) {
    ERROR_LOG("Cannot begin sending, sender is busy");
    return;
  }
  m_sharingMode = ModeShareState::SHARE_SEND;
  // initialize it with the current mode data
  VLSender::loadMode(m_pCurMode);
  // send the first chunk of data, leave if we're done
  if (!VLSender::send()) {
    // when send has completed, stores time that last action was completed to calculate interval between sends
    beginReceiving();
  }
}

void ModeSharing::continueSending()
{
  // if the sender isn't sending then nothing to do
  if (!VLSender::isSending()) {
    return;
  }
  if (!VLSender::send()) {
    // when send has completed, stores time that last action was completed to calculate interval between sends
    beginReceiving();
  }
}

void ModeSharing::beginReceiving()
{
  m_sharingMode = ModeShareState::SHARE_RECEIVE;
  VLReceiver::beginReceiving();
}

void ModeSharing::receiveMode()
{
  // if reveiving new data set our last data time
  if (VLReceiver::onNewData()) {
    m_timeOutStartTime = Time::getCurtime();
    // if our last data was more than time out duration reset the recveiver
  } else if (m_timeOutStartTime > 0 && (m_timeOutStartTime + MAX_TIMEOUT_DURATION) < Time::getCurtime()) {
    VLReceiver::resetVLState();
    m_timeOutStartTime = 0;
    return;
  }
  // check if the VLReceiver has a full packet available
  if (!VLReceiver::dataReady()) {
    // nothing available yet
    return;
  }
  DEBUG_LOG("Mode ready to receive! Receiving...");
  // receive the VL mode into the current mode
  if (!VLReceiver::receiveMode(m_pCurMode)) {
    ERROR_LOG("Failed to receive mode");
    return;
  }
  DEBUG_LOGF("Success receiving mode: %u", m_pCurMode->getPatternID());
  // leave menu and save settings, even if the mode was the same whatever
  leaveMenu(true);
}

void ModeSharing::showSendMode()
{
  // show a dim color when not sending
  if (!VLSender::isSending()) {
    Leds::setAll(RGBColor(0, 20, 20));
  }
}

void ModeSharing::showReceiveMode()
{
  // using uint32_t to avoid overflow, the result should be within 10 to 255
  Leds::setIndex(LED_0, VLReceiver::isReceiving() ? RGBColor(0, VLReceiver::percentReceived(), 0) : RGB_BLANK);
  Leds::clearIndex(LED_1);
}
