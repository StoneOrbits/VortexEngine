#include "ModeSharing.h"

#include "../../Serial/ByteStream.h"
#include "../../Serial/Serial.h"
#include "../../Time/TimeControl.h"
#include "../../Time/Timings.h"
#include "../../Wireless/IRReceiver.h"
#include "../../Wireless/VLSender.h"
#include "../../Wireless/IRSender.h"
#include "../../Buttons/Button.h"
#include "../../Modes/Modes.h"
#include "../../Modes/Mode.h"
#include "../../Leds/Leds.h"
#include "../../Log/Log.h"

ModeSharing::ModeSharing(const RGBColor &col, bool advanced) :
  Menu(col, advanced),
  m_sharingMode(ModeShareState::SHARE_RECEIVE),
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
  beginReceivingIR();
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
  case ModeShareState::SHARE_SEND_IR:
    // render the 'send mode' lights
    showSendModeIR();
    // continue sending any data as long as there is more to send
    continueSendingIR();
    break;
  case ModeShareState::SHARE_SEND_VL:
    // render the 'send mode' lights
    showSendModeVL();
    // continue sending any data as long as there is more to send
    continueSendingVL();
    break;
  case ModeShareState::SHARE_RECEIVE:
    // render the 'receive mode' lights
    showReceiveMode();
    // load any modes that are received
    receiveModeIR();
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
    IRReceiver::endReceiving();
    beginSendingIR();
    DEBUG_LOG("Switched to send mode");
    break;
  default:
    break;
  }
  Leds::clearAll();
}

void ModeSharing::onLongClick()
{
  Modes::updateCurMode(m_pCurMode);
  leaveMenu(true);
}

void ModeSharing::beginSendingVL()
{
  // if the sender is sending then cannot start again
  if (VLSender::isSending()) {
    ERROR_LOG("Cannot begin sending, sender is busy");
    return;
  }
  m_sharingMode = ModeShareState::SHARE_SEND_VL;
  // initialize it with the current mode data
  VLSender::loadMode(m_pCurMode);
  // send the first chunk of data, leave if we're done
  if (!VLSender::send()) {
    // when send has completed, stores time that last action was completed to calculate interval between sends
    beginReceivingIR();
  }
}

void ModeSharing::beginSendingIR()
{
  // if the sender is sending then cannot start again
  if (IRSender::isSending()) {
    ERROR_LOG("Cannot begin sending, sender is busy");
    return;
  }
  m_sharingMode = ModeShareState::SHARE_SEND_IR;
  // initialize it with the current mode data
  IRSender::loadMode(m_pCurMode);
  // send the first chunk of data, leave if we're done
  if (!IRSender::send()) {
    // when send has completed, stores time that last action was completed to calculate interval between sends
    beginReceivingIR();
  }
}

void ModeSharing::continueSendingVL()
{
  // if the sender isn't sending then nothing to do
  if (!VLSender::isSending()) {
    return;
  }
  if (!VLSender::send()) {
    // when send has completed, stores time that last action was completed to calculate interval between sends
    beginReceivingIR();
  }
}

void ModeSharing::continueSendingIR()
{
  // if the sender isn't sending then nothing to do
  if (!IRSender::isSending()) {
    return;
  }
  if (!IRSender::send()) {
    // when send has completed, stores time that last action was completed to calculate interval between sends
    beginReceivingIR();
  }
}

void ModeSharing::beginReceivingIR()
{
  m_sharingMode = ModeShareState::SHARE_RECEIVE;
  IRReceiver::beginReceiving();
}

void ModeSharing::receiveModeIR()
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
  // receive the VL mode into the current mode
  if (!IRReceiver::receiveMode(m_pCurMode)) {
    ERROR_LOG("Failed to receive mode");
    return;
  }
  DEBUG_LOGF("Success receiving mode: %u", m_pCurMode->getPatternID());
  if (!m_advanced) {
    // leave menu and save settings, even if the mode was the same whatever
    leaveMenu(true);
  }
}

void ModeSharing::showSendModeVL()
{
  // show a dim color when not sending
  Leds::clearAll();
}

void ModeSharing::showSendModeIR()
{
  // show a dim color when not sending
  Leds::clearAll();
}

void ModeSharing::showReceiveMode()
{
  if (IRReceiver::isReceiving()) {
    // using uint32_t to avoid overflow, the result should be within 10 to 255
    Leds::setAll(RGBColor(0, IRReceiver::percentReceived(), 0));
  } else {
    if (m_advanced && m_pCurMode) {
      m_pCurMode->play();
    } else {
      Leds::setAll(RGB_WHITE0);
    }
  }
}
