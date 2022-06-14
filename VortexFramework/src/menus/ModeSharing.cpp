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
    Infrared::beginReceiving();
    startReceiving();
    break;
  case ModeShareState::SHARE_RECEIVE:
    // go to quit option
    Infrared::endReceiving();
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
#if 0
  Infrared::write(buf.rawSize() | 0xb00b0000);
  uint32_t num_writes = (buf.rawSize() + 3) / 4;
  uint32_t *raw_buf = (uint32_t *)buf.rawData();
  for (uint32_t i = 0; i < num_writes; ++i) {
    if (!Infrared::write(raw_buf[i])) {
      DEBUG_LOGF("Failed to write %u dword", i);
    }
  }
#endif
  uint64_t endTime = micros();
  DEBUG_LOGF("Wrote %u buf (%u us)", buf.rawSize(), endTime - startTime);
  DEBUG_LOG("Success sending");
}

// WARNING! Right now this function blocks till it gets data,
// I can't get it to work any other way because of to timestep/timing
// I think the IR receiver needs to be polled in the timestep loop
// so that no data is lost, then data is just pushed to a list
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
#if 0
  // wait for magic number
  val = Infrared::read();
  // upper 16 has magic number in it
  if (((val >> 16) & 0xFFFF) != 0xb00b) {
    // nothing to do yet
    return;
  }
  uint32_t size = val & 0xFFFF;
  if (!size || size > 8096) {
    DEBUG_LOGF("bad mode size %u", size);
    return;
  }
  databuf.init(size);
  uint32_t num_reads = (size + 3) / 4;
  uint32_t *raw_buf = (uint32_t *)databuf.rawData();
  for (uint32_t i = 0; i < num_reads; ++i) {
    raw_buf[i] = Infrared::read();
  }
#endif
  uint64_t endTime = micros();
  DEBUG_LOGF("Received %u bytes (%u us)", buf.size(), endTime - startTime);
  // decompress and check crc at same time
  if (!buf.decompress()) {
    DEBUG_LOG("Failed to decompress, crc mismatch or bad data");
    return;
  }
  // if it decompressed the CRC was good and we can unserialize
  m_pCurMode->unserialize(buf);
  m_pCurMode->init();
  DEBUG_LOG("Success receiving mode");
  leaveMenu();
}

void ModeSharing::showSendMode()
{
  // gradually fill from pinkie to thumb
  RGBColor col = RGB_TEAL;
  Leds::clearAll();
  LedPos pos = (LedPos)((Time::getCurtime() / Time::msToTicks(100)) % LED_COUNT);
  Leds::setRange(LED_FIRST, pos, col);
}

void ModeSharing::showReceiveMode()
{
  // gradually empty from thumb to pinkie
  RGBColor col = RGB_PURPLE;
  Leds::clearAll();
  LedPos pos = (LedPos)(LED_LAST - ((Time::getCurtime() / Time::msToTicks(100)) % LED_COUNT));
  Leds::setRange(LED_FIRST, pos, col);
}

void ModeSharing::startSending()
{
  m_sharingMode = ModeShareState::SHARE_SEND;
  DEBUG_LOG("Switched to send mode");
}

void ModeSharing::startReceiving()
{
  // start listening
  m_sharingMode = ModeShareState::SHARE_RECEIVE;
  DEBUG_LOG("Switched to receive mode");
}
