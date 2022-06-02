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
  m_sharingMode(ModeShareState::SHARE_RECEIVE),
  m_last_action(0)
{
}

bool ModeSharing::init()
{
  if (!Menu::init()) {
    return false;
  }
  // just start spewing out modes everywhere
  startSending();
  DEBUG("Entering Mode Sharing");
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
    break;
  case ModeShareState::SHARE_RECEIVE:
    showReceiveMode();
    break;
  case ModeShareState::SHARE_QUIT:
    // blink red -> off
    Leds::setAll(RGB_RED);
    Leds::blinkAll(100,250);
    break;
  }
  return true;
}

// handlers for clicks
void ModeSharing::onShortClick()
{
  switch (m_sharingMode) {
  case ModeShareState::SHARE_SEND:
    // go to receiving
    startReceiving();
    break;
  case ModeShareState::SHARE_RECEIVE:
    // go to quit option
    m_sharingMode = ModeShareState::SHARE_QUIT;
    break;
  case ModeShareState::SHARE_QUIT:
  default:
    // just wrap around back to sending
    startSending();
    break;
  }
}

void ModeSharing::onLongClick()
{
  switch (m_sharingMode) {
  case ModeShareState::SHARE_SEND:
    sendMode();
    break;
  case ModeShareState::SHARE_RECEIVE:
    receiveMode();
    break;
  case ModeShareState::SHARE_QUIT:
    leaveMenu();
    break;
  }
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
  Modes::serialize(buf);
  if (!buf.compress()) {
    // tried
  }
  // too big
  if (buf.rawSize() > 8000) {
    DEBUGF("too big: %u", buf.rawSize());
    return;
  }
  DEBUGF("Writing %u buf", buf.rawSize());
  Infrared::write(buf.rawSize() | 0xb00b0000);
  uint32_t num_writes = (buf.rawSize() + 3) / 4;
  uint32_t *raw_buf = (uint32_t *)buf.rawData();
  for (uint32_t i = 0; i < num_writes; ++i) {
    if (!Infrared::write(raw_buf[i])) {
      DEBUGF("Failed to write %u dword", i);
    }
  }
  DEBUGF("Wrote %u buf", buf.rawSize());
}

// WARNING! Right now this function blocks till it gets data, 
// I can't get it to work any other way because of to timestep/timing
void ModeSharing::receiveMode()
{
  RGBColor col = RGB_TEAL;
  uint32_t val = 0;
  // lower 16 is the size of the data to follow
  SerialBuffer databuf;
  // wait for magic number
  do {
    val = Infrared::read();
    // upper 16 has magic number in it
  } while (((val >> 16) & 0xFFFF) != 0xb00b);
  uint32_t size = val & 0xFFFF;
  if (!size || size > 8096) {
    DEBUGF("wtf bad size %u", size);
    return;
  }
  databuf.init(size);
  uint32_t num_reads = size / 4;
  uint32_t *raw_buf = (uint32_t *)databuf.rawData();
  for (uint32_t i = 0; i < num_reads; ++i) {
    raw_buf[i] = Infrared::read();
  }
  DEBUGF("Received %u bytes", databuf.size());
  databuf.decompress();
  if (!Modes::unserialize(databuf)) {
    DEBUG("Failed to load modes");
  }
}

void ModeSharing::showSendMode()
{
  RGBColor col = RGB_TEAL;
  Leds::clearAll();
  LedPos pos = (LedPos)((Time::getCurtime() / Time::msToTicks(100)) % LED_COUNT);
  Leds::setRange(LED_FIRST, pos, col);
}

void ModeSharing::showReceiveMode()
{
  RGBColor col = RGB_TEAL;
  Leds::clearAll();
  LedPos pos = (LedPos)(LED_LAST - ((Time::getCurtime() / Time::msToTicks(100)) % LED_COUNT));
  Leds::setRange(LED_FIRST, pos, col);
}

void ModeSharing::startSending()
{
  m_sharingMode = ModeShareState::SHARE_SEND;
  DEBUG("Switched to send mode");
}

void ModeSharing::startReceiving()
{
  // start listening
  m_sharingMode = ModeShareState::SHARE_RECEIVE;
  DEBUG("Switched to receive mode");
}
