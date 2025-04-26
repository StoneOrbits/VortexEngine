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
  if (!m_advanced) {
    // skip led selection
    m_ledSelected = true;
  }
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
  bool shouldSend = (g_pButton->isPressed() && g_pButton->holdDuration() >= CLICK_THRESHOLD);
  // load any modes that are received
  if (shouldSend) {
    if (m_sharingMode == ModeShareState::SHARE_SEND_RECEIVE) {
      // send or send legacy
      VLSender::send(&m_previewMode);
    } else {
      // send or send legacy
      VLSender::sendLegacy(&m_previewMode);
    }
  }
  // render the 'receive mode' lights whether legacy or not
  showReceiveMode();
  receiveMode();
  return MENU_CONTINUE;
}

void ModeSharing::onLedSelected()
{
  // if we selected leds that implies advanced mode
  if (m_targetLeds == MAP_LED(LED_1)) {
    // if we selected the top led then simply swap the two patterns
    // so that the top led is sent first -- if the receiver is receiving
    // into one slot then it will only use the first pattern to do so
    m_previewMode.swapPatterns(LED_0, LED_1);
  }
}

// handlers for clicks
void ModeSharing::onShortClick()
{
  if (g_pButton->holdDuration() >= CLICK_THRESHOLD) {
    return;
  }
  switch (m_sharingMode) {
  case ModeShareState::SHARE_SEND_RECEIVE:
    // stop receiving, send the mode, go back to receiving
    VLReceiver::setLegacyReceiver(true);
    m_sharingMode = ModeShareState::SHARE_SEND_RECEIVE_LEGACY;
    break;
  case ModeShareState::SHARE_SEND_RECEIVE_LEGACY:
    VLReceiver::setLegacyReceiver(false);
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
  uint32_t now = Time::getCurtime();
  // if reveiving new data set our last data time
  if (VLReceiver::onNewData()) {
    m_timeOutStartTime = now;
    // if our last data was more than time out duration reset the recveiver
  } else if (m_timeOutStartTime > 0 && (m_timeOutStartTime + MAX_TIMEOUT_DURATION) < now) {
    VLReceiver::resetVLState();
    m_timeOutStartTime = 0;
    return;
  }
  // check if the VLReceiver has a full packet available
  if (!VLReceiver::dataReady()) {
    uint8_t percent = VLReceiver::percentReceived();
    if (percent != m_lastPercent) {
      m_lastPercent = percent;
      m_lastPercentChange = now;
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
  if (m_advanced && m_targetLeds != MAP_LED_ALL) {
    LedPos target = ledmapGetFirstLed(m_targetLeds);
    LedPos other = LED_1;
    // if the user picked the top led to copy into then swap the patterns
    // in the incoming mode so the 0th pattern is on the top led
    if (target == LED_1) {
      other = LED_0;
      m_previewMode.swapPatterns(LED_0, LED_1);
    }
    m_previewMode.copyPatternFrom(Modes::curMode(), other, other);
  }
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
