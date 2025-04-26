#include "ModeSharing.h"

#include "../../VortexEngine.h"
#include "../../Serial/ByteStream.h"
#include "../../Serial/Serial.h"
#include "../../Time/TimeControl.h"
#include "../../Time/Timings.h"
#include "../../Wireless/VLReceiver.h"
#include "../../Wireless/VLSender.h"
#include "../../Patterns/Pattern.h"
#include "../../Buttons/Button.h"
#include "../../Modes/Modes.h"
#include "../../Modes/Mode.h"
#include "../../Leds/Leds.h"
#include "../../Log/Log.h"

ModeSharing::ModeSharing(const RGBColor &col, bool advanced) :
  Menu(col, advanced),
  m_sharingMode(ModeShareState::SHARE_SEND_RECEIVE),
  m_timeOutStartTime(0),
  m_lastPercentChange(0),
  m_lastPercent(0)

{
}

ModeSharing::~ModeSharing()
{
  VLReceiver::endReceiving();
  VortexEngine::toggleForceSleep(true);
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
  VLReceiver::beginReceiving();
  // turn off force sleep while using modesharing
  VortexEngine::toggleForceSleep(false);
  DEBUG_LOG("Entering Mode Sharing");
  return true;
}

Menu::MenuAction ModeSharing::run()
{
  MenuAction result = Menu::run();
  if (result != MENU_CONTINUE) {
    return result;
  }
  if (m_sharingMode == ModeShareState::SHARE_EXIT) {
    showExit();
    return MENU_CONTINUE;
  }
  // render the 'receive mode' lights whether legacy or not
  showReceiveMode();
  // load any modes that are received
  receiveMode();
  if (g_pButton->isPressed() && g_pButton->holdDuration() >= CLICK_THRESHOLD) {
    // send or send legacy
    VLSender::send(&m_previewMode);
  }
  return MENU_CONTINUE;
}

void ModeSharing::onLedSelected()
{
}

// handlers for clicks
void ModeSharing::onShortClick()
{
  switch (m_sharingMode) {
  case ModeShareState::SHARE_SEND_RECEIVE:
    // stop receiving, send the mode, go back to receiving
    VLReceiver::setLegacyReceiver(true);
    VLSender::setLegacySender(true);
    m_sharingMode = ModeShareState::SHARE_SEND_RECEIVE_LEGACY;
    break;
  case ModeShareState::SHARE_SEND_RECEIVE_LEGACY:
    VLReceiver::setLegacyReceiver(false);
    VLSender::setLegacySender(false);
    m_sharingMode = ModeShareState::SHARE_EXIT;
    break;
  case ModeShareState::SHARE_EXIT:
    m_sharingMode = ModeShareState::SHARE_SEND_RECEIVE;
    break;
  default:
    break;
  }
  Leds::clearAll();
}

void ModeSharing::onLongClick()
{
  if (m_sharingMode == ModeShareState::SHARE_EXIT) {
    leaveMenu();
  }
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
    uint8_t percent = VLReceiver::percentReceived();
    if (percent != m_lastPercent) {
      m_lastPercent = percent;
      m_lastPercentChange = Time::getCurtime();
    }
    // nothing available yet
    return;
  }
  DEBUG_LOG("Mode ready to receive! Receiving...");
  // receive the VL mode into the current mode
  if (!VLReceiver::receiveMode(&m_previewMode)) {
    ERROR_LOG("Failed to receive mode");
    return;
  }
  DEBUG_LOGF("Success receiving mode: %u", m_previewMode.getPatternID());
  Modes::updateCurMode(&m_previewMode);
  // leave menu and save settings, even if the mode was the same whatever
  leaveMenu(true);
}

void ModeSharing::showReceiveMode()
{
  if (VLReceiver::isReceiving()) {
    // using uint32_t to avoid overflow, the result should be within 10 to 255
    uint32_t diff = (Time::getCurtime() - m_lastPercentChange);
    if (diff > (IR_RECEIVER_TIMEOUT_DURATION - 400)) {
      Leds::setIndex(LED_0, RGB_RED3);
    } else {
      Leds::setIndex(LED_0, RGBColor(0, VLReceiver::percentReceived(), 0));
    }
    Leds::clearIndex(LED_1);
  } else {
    Leds::setAll((m_sharingMode == ModeShareState::SHARE_SEND_RECEIVE) ? RGB_CYAN0 : RGB_WHITE0);
  }
}
