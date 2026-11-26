#include "VLReceiver.h"
#include "VLConfig.h"

#if VL_ENABLE_RECEIVER == 1

#include "../VortexEngine.h"

#include "../Serial/ByteStream.h"
#include "../Serial/BitStream.h"
#include "../Time/TimeControl.h"
#include "../Modes/Mode.h"
#include "../Leds/Leds.h"
#include "../Log/Log.h"

VLReceiver::VLReceiver(VortexEngine &engine) :
  m_engine(engine),
  m_vlData(),
  m_recvState(WAITING_HEADER_MARK),
  m_prevTime(0),
  m_pinState(0),
  m_previousBytes(0),
  m_vlMarkThreshold(0),
  m_vlSpaceThreshold(0),
  m_counter(0),
  m_parityBit(0),
  m_legacy(false)
{
}

VLReceiver::~VLReceiver()
{
}

bool VLReceiver::init()
{
  return m_vlData.init(VL_RECV_BUF_SIZE);
}

void VLReceiver::cleanup()
{
}

bool VLReceiver::dataReady()
{
  // is the receiver actually receiving data?
  if (!isReceiving()) {
    return false;
  }
  uint8_t size = m_vlData.peekData(1);
  // check if there are size + 1 bytes in the VLData receiver
  return (m_vlData.bytepos() >= ((uint32_t)size + 2));
}

// whether actively receiving
bool VLReceiver::isReceiving()
{
  // if there are at least 2 bytes in the data buffer then
  // the receiver is receiving a packet. If there is less
  // than 2 bytes then we're still waiting for the 'blocks'
  // and 'remainder' bytes which prefix a packet
  return (m_vlData.bytepos() > 2);
}

// the percent of data received
uint8_t VLReceiver::percentReceived()
{
  if (!isReceiving()) {
    return 0;
  }
  uint8_t size = m_vlData.peekData(1);
  // round by adding half of the total to the numerator
  return (uint8_t)((uint16_t)((m_vlData.bytepos() * 100 + (size / 2)) / size));
}

bool VLReceiver::receiveMode(Mode *pMode)
{
  ByteStream buf;
  // read from the receive buffer into the byte stream
  if (!read(buf)) {
    // no data to read right now, or an error
    DEBUG_LOG("No data available to read, or error reading");
    return false;
  }
  DEBUG_LOGF("Received %u bytes", buf.rawSize());
  // load the data into the target mode
  return pMode->loadFromBuffer(buf);
}

bool VLReceiver::beginReceiving()
{
  resetVLState();
  return true;
}

bool VLReceiver::endReceiving()
{
  resetVLState();
  return true;
}

bool VLReceiver::onNewData()
{
  if (bytesReceived() == m_previousBytes) {
    return false;
  }
  m_previousBytes = bytesReceived();
  return true;
}

bool VLReceiver::read(ByteStream &data)
{
  if (!m_vlData.bytepos() || m_vlData.bytepos() > VL_MAX_DATA_TRANSFER) {
    DEBUG_LOG("Nothing to read, or read too much");
    return false;
  }
  uint8_t size = m_vlData.peekData(1);
  const uint8_t *actualData = m_vlData.data() + 2;
  if (!data.rawInit(actualData, size)) {
    DEBUG_LOG("Failed to init buffer for VL read");
    return false;
  }
  // reset the VL state and receive buffer now
  resetVLState();
  return true;
}

// The recv PCI handler is called every time the analog value passes the
// 'threshold' which is dynamically established to be about half way between
// 'light' and 'dark' in the current environment. So in other words, it is
// called for every 'on' or 'off' blink
void VLReceiver::recvPCIHandler()
{
  // toggle the tracked pin state no matter what
  m_pinState = (uint8_t)!m_pinState;
  // grab current time
  uint32_t now = m_engine.time().microseconds();
  // calc time difference between previous change and now
  uint32_t diff = (uint32_t)(now - m_prevTime);
  m_prevTime = now;
  // filter out values much too large from being truncated
  if (diff > UINT16_MAX) {
    return;
  }
  // handle the blink duration and process it into data
  // legacy would normally be handled here but it is no longer supported
  //if (m_legacy) {
  //  handleVLTimingLegacy((uint16_t)diff);
  //  return;
  //}
  handleVLTiming((uint16_t)diff);
}

// state machine that can be fed VL timings to parse them and interpret the intervals
void VLReceiver::handleVLTimingLegacy(uint16_t diff)
{
  switch (m_recvState) {
  case WAITING_HEADER_MARK:
    // just look for a big blink timing
    if (diff >= VL_HEADER_SPACE_MIN_LEGACY && diff <= VL_HEADER_MARK_MAX_LEGACY) {
      // go straight to the header space
      m_recvState = WAITING_HEADER_SPACE;
    }
    break;
  case WAITING_HEADER_SPACE:
    // the header space is technically shorter but this check uses the same range
    // because being restrictive here isn't really beneficial
    if (diff >= VL_HEADER_SPACE_MIN_LEGACY && diff <= VL_HEADER_MARK_MAX_LEGACY) {
      // iterate to first data mark
      m_recvState = READING_DATA_MARK;
      // estimate the data threshold based on the length of the space
      m_vlMarkThreshold = (diff / 4);
    }
    break;
  case READING_DATA_MARK:
    // classify as 1 or 0 based on the mark threshold and write into buffer
    m_vlData.write1Bit(diff > m_vlMarkThreshold);
    m_recvState = READING_DATA_SPACE;
    break;
  case READING_DATA_SPACE:
    // in the legacy transmission the spaces didn't carry data
    m_recvState = READING_DATA_MARK;
    break;
  default: // ??
    DEBUG_LOGF("Bad receive state: %u", m_recvState);
    break;
  }
}

// state machine that can be fed VL timings to parse them and interpret the intervals
void VLReceiver::handleVLTiming(uint16_t diff)
{
  uint8_t bit;
  switch (m_recvState) {
  case WAITING_HEADER_MARK:
  case WAITING_HEADER_SPACE:
    // both cases are basically the same, just look for a big timing
    if (diff >= VL_HEADER_SPACE_MIN && diff <= VL_HEADER_MARK_MAX) {
      // iterate through first two states
      m_recvState = (RecvState)(m_recvState + 1);
    }
    break;
  case READING_BAUD_MARK:
    // accumulate the diff in the mark threshold for averaging later
    m_vlMarkThreshold += diff;
    m_recvState = READING_BAUD_SPACE;
    break;
  case READING_BAUD_SPACE:
    // couonter is used to count bauds till 4
    m_counter++;
    // accumulate the diff in the space threshold for averaging later
    m_vlSpaceThreshold += diff;
    // if not at 4 bauds yet keep reading baud marks
    if (m_counter < 4) {
      m_recvState = READING_BAUD_MARK;
      break;
    }
    // otherwise read all 4 bauds now proceed with processing them
    // average out the mark and space from the bauds
    m_vlMarkThreshold /= 4;
    m_vlSpaceThreshold /= 4;
    // reset counter and parity bit
    m_counter = 0;
    m_parityBit = 0;
    // advanced state to first data mark
    m_recvState = READING_DATA_MARK;
    break;
  case READING_DATA_MARK:
    // counter is now counting the marks for parity tracking
    m_counter++;
    // extract the bit of data based on the calculated mark threshold
    bit = (diff > m_vlMarkThreshold) ? 1 : 0;
    // accumulate the parity and write out the bit into the vldata
    m_parityBit = (m_parityBit ^ bit) & 1;
    m_vlData.write1Bit(bit);
    m_recvState = READING_DATA_SPACE;
    break;
  case READING_DATA_SPACE:
    // the spaces also transmit data in their lengths
    bit = (diff > m_vlSpaceThreshold) ? 1 : 0;
    // also accumulate this data into the parity
    m_parityBit = (m_parityBit ^ bit) & 1;
    // and then write out the bit into the vldata
    m_vlData.write1Bit(bit);
    // when counter is a multiple of 4 (since it only counts marks)
    if ((m_counter % 4) == 0) {
      // then go to the parity processing for the previous 8 bits of data
      m_recvState = READING_DATA_PARITY_MARK;
    } else {
      // otherwise continue reading out data bits
      m_recvState = READING_DATA_MARK;
    }
    break;
  case READING_DATA_PARITY_MARK:
    // the parity is also a bit of data using a mark
    bit = (diff > m_vlMarkThreshold) ? 1 : 0;
    // check the parity bit against the running accumulated parity
    if ((m_parityBit & 1) != bit) {
      // immediately reset the receiver if the parity doesn't match
      // could show a flash here but that would probably just make
      // receiving worse by introducing a delay
      resetVLState();
      break;
    }
    // the parity space is just an empty space after the parity bit
    m_recvState = READING_DATA_PARITY_SPACE;
    break;
  case READING_DATA_PARITY_SPACE:
    // after the parity it's back to regular mark
    m_recvState = READING_DATA_MARK;
    break;
  default: // ??
    DEBUG_LOGF("Bad receive state: %u", m_recvState);
    break;
  }
}

void VLReceiver::resetVLState()
{
  m_counter = 0;
  m_vlMarkThreshold = 0;
  m_vlSpaceThreshold = 0;
  m_previousBytes = 0;
  m_parityBit = 0;
  m_recvState = WAITING_HEADER_MARK;
  // zero out the receive buffer and reset bit receiver position
  m_vlData.reset();
#ifdef VORTEX_EMBEDDED
  // reset the threshold to a high value so that it can be pulled down again
  threshold = THRESHOLD_BEGIN;
#endif
  DEBUG_LOG("VL State Reset");
}

#endif
