#include "IRReceiver.h"
#include "IRConfig.h"

#if IR_ENABLE_RECEIVER == 1

#include "../Serial/ByteStream.h"
#include "../Serial/BitStream.h"
#include "../Time/TimeControl.h"
#include "../Modes/Mode.h"
#include "../Log/Log.h"

// bit stream that IR data is fed to bit by bit
static BitStream IRReceiver_m_irData;

// receive state used for the state machine in the PCI handler
typedef enum {
  WAITING_HEADER_MARK,
  WAITING_HEADER_SPACE,
  READING_DATA_MARK,
  READING_DATA_SPACE
} RecvState;

// state information used by the PCI handler
static RecvState IRReceiver_m_recvState = WAITING_HEADER_MARK;
// used to track pin changes
static uint32_t IRReceiver_m_prevTime = 0;
static uint8_t IRReceiver_m_pinState = 0;

// used to compare if received data has changed since last checking
static uint32_t IRReceiver_m_previousBytes = 0;

uint16_t IRReceiver_bytesReceived()
{
  return BitStream_bytepos(&IRReceiver_m_irData);
}

bool IRReceiver_init()
{
  // receive into a static buffer so the receiver never touches the heap
  static uint8_t s_irDataBuf[IR_RECV_BUF_SIZE];
  return BitStream_initBuf(&IRReceiver_m_irData, s_irDataBuf, IR_RECV_BUF_SIZE);
}

void IRReceiver_cleanup()
{
}

bool IRReceiver_dataReady()
{
  // is the receiver actually receiving data?
  if (!IRReceiver_isReceiving()) {
    return false;
  }
  uint8_t blocks = BitStream_data(&IRReceiver_m_irData)[0];
  uint8_t remainder = BitStream_data(&IRReceiver_m_irData)[1];
  uint32_t total = ((blocks - 1) * 32) + remainder;
  if (!total || total > IR_MAX_DATA_TRANSFER) {
    DEBUG_LOGF("Bad IR Data size: %u", total);
    return false;
  }
  // if there are size + 2 bytes in the IRData receiver
  // then a full message is ready, the + 2 is from the
  // two bytes for blocks + remainder that are sent first
  return (BitStream_bytepos(&IRReceiver_m_irData) >= (uint32_t)(total + 2));
}

// whether actively receiving
bool IRReceiver_isReceiving()
{
  // if there are at least 2 bytes in the data buffer then
  // the receiver is receiving a packet. If there is less
  // than 2 bytes then we're still waiting for the 'blocks'
  // and 'remainder' bytes which prefix a packet
  return (BitStream_bytepos(&IRReceiver_m_irData) > 2);
}

// the percent of data received
uint8_t IRReceiver_percentReceived()
{
  if (!IRReceiver_isReceiving()) {
    return 0;
  }
  uint8_t blocks = BitStream_data(&IRReceiver_m_irData)[0];
  uint8_t remainder = BitStream_data(&IRReceiver_m_irData)[1];
  uint16_t total = ((blocks - 1) * 32) + remainder;
  // round by adding half of the total to the numerator
  return (uint8_t)((uint16_t)((BitStream_bytepos(&IRReceiver_m_irData) * 100 + (total / 2)) / total));
}

bool IRReceiver_receiveMode(Mode *pMode)
{
  ByteStream buf;
  ByteStream_make(&buf);
  // read from the receive buffer into the byte stream
  if (!IRReceiver_read(&buf)) {
    // no data to read right now, or an error
    DEBUG_LOG("No data available to read, or error reading");
    ByteStream_destroy(&buf);
    return false;
  }
  DEBUG_LOGF("Received %u bytes", ByteStream_rawSize(&buf));
  // load the data into the target mode
  bool rv = Mode_loadFromBuffer(pMode, &buf);
  ByteStream_destroy(&buf);
  return rv;
}

bool IRReceiver_beginReceiving()
{
  IRReceiver_resetIRState();
  return true;
}

bool IRReceiver_endReceiving()
{
  IRReceiver_resetIRState();
  return true;
}

bool IRReceiver_onNewData()
{
  if (IRReceiver_bytesReceived() == IRReceiver_m_previousBytes) {
    return false;
  }
  IRReceiver_m_previousBytes = IRReceiver_bytesReceived();
  return true;
}

bool IRReceiver_read(ByteStream *data)
{
  if (!BitStream_bytepos(&IRReceiver_m_irData) || BitStream_bytepos(&IRReceiver_m_irData) > IR_MAX_DATA_TRANSFER) {
    DEBUG_LOG("Nothing to read, or read too much");
    return false;
  }
  // read the size out (blocks + remainder)
  uint8_t blocks = BitStream_data(&IRReceiver_m_irData)[0];
  uint8_t remainder = BitStream_data(&IRReceiver_m_irData)[1];
  // calculate size from blocks + remainder
  uint32_t size = ((blocks - 1) * 32) + remainder;
  if (!size || size > IR_MAX_DATA_TRANSFER) {
    DEBUG_LOGF("Bad IR Data size: %u", size);
    return false;
  }
  // the actual data starts 2 bytes later because of the size byte
  const uint8_t *actualData = BitStream_data(&IRReceiver_m_irData) + 2;
  if (!ByteStream_rawInit(data, actualData, size)) {
    DEBUG_LOG("Failed to init buffer for IR read");
    return false;
  }
  // reset the IR state and receive buffer now
  IRReceiver_resetIRState();
  return true;
}

// The recv PCI handler is called every time the pin state changes
void IRReceiver_recvPCIHandler()
{
  // toggle the tracked pin state no matter what
  IRReceiver_m_pinState = (uint8_t)!IRReceiver_m_pinState;
  // grab current time
  uint32_t now = Time_microseconds();
  // check previous time for validity
  if (!IRReceiver_m_prevTime || IRReceiver_m_prevTime > now) {
    IRReceiver_m_prevTime = now;
    DEBUG_LOG("Bad first time diff, resetting...");
    IRReceiver_resetIRState();
    return;
  }
  // calc time difference between previous change and now
  uint32_t diff = (uint32_t)(now - IRReceiver_m_prevTime);
  // and update the previous changetime for next loop
  IRReceiver_m_prevTime = now;
  // handle the blink duration and process it
  IRReceiver_handleIRTiming(diff);
}

// state machine that can be fed IR timings to parse them and interpret the intervals
void IRReceiver_handleIRTiming(uint32_t diff)
{
  // if the diff is too long or too short then it's not useful
  if ((diff > IR_HEADER_MARK_MAX && IRReceiver_m_recvState < READING_DATA_MARK) || diff < IR_TIMING_MIN) {
    DEBUG_LOGF("bad delay: %u, resetting...", diff);
    IRReceiver_resetIRState();
    return;
  }
  switch (IRReceiver_m_recvState) {
  case WAITING_HEADER_MARK: // initial state
    if (diff >= IR_HEADER_MARK_MIN && diff <= IR_HEADER_MARK_MAX) {
      IRReceiver_m_recvState = WAITING_HEADER_SPACE;
    } else {
      DEBUG_LOGF("Bad header mark %u, resetting...", diff);
      IRReceiver_resetIRState();
    }
    break;
  case WAITING_HEADER_SPACE:
    if (diff >= IR_HEADER_SPACE_MIN && diff <= IR_HEADER_SPACE_MAX) {
      IRReceiver_m_recvState = READING_DATA_MARK;
    } else {
      DEBUG_LOGF("Bad header space %u, resetting...", diff);
      IRReceiver_resetIRState();
    }
    break;
  case READING_DATA_MARK:
    // classify mark/space based on the timing and write into buffer
    BitStream_write1Bit(&IRReceiver_m_irData, (diff > (IR_TIMING * 2)) ? true : false);
    IRReceiver_m_recvState = READING_DATA_SPACE;
    break;
  case READING_DATA_SPACE:
    // the space could be just a regular space, or a gap in between blocks
    IRReceiver_m_recvState = READING_DATA_MARK;
    break;
  default: // ??
    DEBUG_LOGF("Bad receive state: %u", IRReceiver_m_recvState);
    break;
  }
}

void IRReceiver_resetIRState()
{
  IRReceiver_m_previousBytes = 0;
  IRReceiver_m_recvState = WAITING_HEADER_MARK;
  // zero out the receive buffer and reset bit receiver position
  BitStream_reset(&IRReceiver_m_irData);
  DEBUG_LOG("IR State Reset");
}

#endif