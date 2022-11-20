#include "IRReceiver.h"

#include "../Serial/ByteStream.h"
#include "../Serial/BitStream.h"
#include "../Log/Log.h"

#include "IRConfig.h"

#include <Arduino.h>

BitStream IRReceiver::m_irData;
IRReceiver::RecvState IRReceiver::m_recvState = WAITING_HEADER_MARK;
uint64_t IRReceiver::m_prevTime = 0;
uint8_t IRReceiver::m_pinState = HIGH;

bool IRReceiver::init()
{
  pinMode(RECEIVER_PIN, INPUT_PULLUP);
  m_irData.init(IR_RECV_BUF_SIZE);
#ifdef TEST_FRAMEWORK
  // the test framework will feed timings directly to handleIRTiming
  // instead of calling the recvPCIHandler in intervals which might
  // be unreliable or problematic
  installIRCallback(handleIRTiming);
#endif
  return true;
}

void IRReceiver::cleanup()
{
}

bool IRReceiver::dataReady()
{
  // not enough data
  if (m_irData.bytepos() < 3) {
    return false;
  }
  // read the size out
  uint8_t blocks = m_irData.data()[0];
  uint8_t remainder = m_irData.data()[1];
  uint32_t total = ((blocks - 1) * 32) + remainder;
  if (!total || total > MAX_DATA_TRANSFER) {
    DEBUG_LOGF("Bad IR Data size: %u", total);
    return false;
  }
  // if there are size + 1 bytes in the IRData receiver
  // then a full message is ready
  return (m_irData.bytepos() >= (uint32_t)(total + 1));
}

bool IRReceiver::read(ByteStream &data)
{
  if (!m_irData.bytepos() || m_irData.bytepos() > MAX_DATA_TRANSFER) {
    // nothing to read, or somehow read way too much
    return false;
  }
  // read the size out
  uint8_t blocks = m_irData.data()[0];
  uint8_t remainder = m_irData.data()[1];
  uint32_t size = ((blocks - 1) * 32) + remainder;
  if (!size || size > MAX_DATA_TRANSFER) {
    DEBUG_LOGF("Bad IR Data size: %u", size);
    return false;
  }
  // the actual data starts 1 byte later because of the size byte
  const uint8_t *actualData = m_irData.data() + 2;
  if (!data.rawInit(actualData, size)) {
    DEBUG_LOG("Failed to init buffer for IR read");
    return false;
  }
  for (uint32_t i = 0; i < size; ++i) {
    DEBUG_LOGF("Read %u: 0x%x", i, actualData[i]);
  }
  // reset the IR state and receive buffer
  DEBUG_LOG("Read data, resetting...");
  resetIRState();
  return true;
}

#if 0
bool IRReceiver::write(const ir_block *block, uint32_t blocksize)
{
  // iterate each byte in the block and write it
  const uint8_t *buf_ptr = (const uint8_t *)block;
  for (uint32_t i = 0; i < blocksize; ++i) {
    write8(*buf_ptr);
    buf_ptr++;
  }
  return true;
}

bool IRReceiver::write(ByteStream &data)
{
  uint32_t size = data.rawSize();
  // ensure the data isn't too big
  if (size > MAX_DATA_TRANSFER) {
    DEBUG_LOGF("Cannot transfer that much data: %u bytes", data.rawSize());
    return false;
  }
  // create a bitstream to walk the bits of the data
  uint8_t *buf = (uint8_t *)data.rawData();
  BitStream sendStream(buf, size);
  // begin the write
  startWrite();
  // the number of 32-byte blocks that will be sent
  uint8_t num_blocks = (size + 31) / 32;
  // the amount in the final block
  uint8_t remainder = size % 32;
  // write the number of blocks being sent, most likely just 1
  write8(num_blocks);
  // now write the number of bytes in the last block
  write8(remainder);
  uint8_t *buf_ptr = buf;
  // iterate each block
  for (uint32_t block = 0; block < num_blocks; ++block) {
    // write the block which is 32 bytes unless on the last block
    //uint32_t blocksize = (block == (num_blocks - 1)) ? remainder : 32;
    uint32_t blocksize = (num_blocks == 1) ? remainder : 32;
    // iterate each byte in the block and write it
    for (uint32_t i = 0; i < blocksize; ++i) {
      write8(*buf_ptr);
      buf_ptr++;
    }
    // delay if there is more blocks
    if (block < (uint32_t)(num_blocks - 1)) {
      //mark(HEADER_MARK);
      space(HEADER_SPACE);
    }
  }
  buf_ptr = buf;
  for (uint32_t block = 0; block < num_blocks; ++block) {
    // write the block which is 32 bytes unless on the last block
    uint32_t blocksize = (num_blocks == 1) ? remainder : 32;
    // iterate each byte in the block and write it
    for (uint32_t i = 0; i < blocksize; ++i) {
      DEBUG_LOGF("Wrote: 0x%x", *buf_ptr);
      buf_ptr++;
    }
    // delay if there is more blocks
    if (block < (uint32_t)(num_blocks - 1)) {
      DEBUG_LOG("Block");
    }
  }
  return true;
}
#endif

bool IRReceiver::beginReceiving()
{
  attachInterrupt(digitalPinToInterrupt(RECEIVER_PIN), IRReceiver::recvPCIHandler, CHANGE);
  resetIRState();
  return true;
}

bool IRReceiver::endReceiving()
{
  detachInterrupt(digitalPinToInterrupt(RECEIVER_PIN));
  resetIRState();
  return true;
}

// ===================
//  sending functions


// ===================
// receiver functions

// The recv PCI handler is called every time the pin state changes
void IRReceiver::recvPCIHandler()
{
  // toggle the tracked pin state no matter what
  m_pinState = (uint8_t)!m_pinState;
  // grab current time
  uint32_t now = micros();
  // check previous time for validity
  if (!m_prevTime || m_prevTime > now) {
    m_prevTime = now;
    DEBUG_LOG("Bad first time diff, resetting...");
    resetIRState();
    return;
  }
  // calc time difference between previous change and now
  uint32_t diff = (uint32_t)(now - m_prevTime);
  // and update the previous changetime for next loop
  m_prevTime = now;
  // handle the blink duration and process it
  handleIRTiming(diff);
}

// state machine that can be fed IR timings to parse them and interpret the intervals
void IRReceiver::handleIRTiming(uint32_t diff)
{
  // if the diff is too long or too short then it's not useful
  if ((diff > HEADER_MARK_MAX && m_recvState < READING_DATA_MARK) || diff < IR_TIMING_MIN) {
    DEBUG_LOGF("bad delay: %u, resetting...", diff);
    resetIRState();
    return;
  }
  static uint32_t counter = 0;
  //DEBUG_LOGF("timing[%u]: %u", counter++, diff);
  switch (m_recvState) {
  case WAITING_HEADER_MARK: // initial state
    if (diff >= HEADER_MARK_MIN && diff <= HEADER_MARK_MAX) {
      m_recvState = WAITING_HEADER_SPACE;
    } else {
      DEBUG_LOGF("Bad header mark %u, resetting...", diff);
      resetIRState();
    }
    break;
  case WAITING_HEADER_SPACE:
    if (diff >= HEADER_SPACE_MIN && diff <= HEADER_SPACE_MAX) {
      m_recvState = READING_DATA_MARK;
    } else {
      DEBUG_LOGF("Bad header space %u, resetting...", diff);
      resetIRState();
    }
    break;
  case READING_DATA_MARK:
    // classify mark/space based on the timing and write into buffer
    m_irData.write1Bit((diff > (IR_TIMING * 2)) ? 1 : 0);
#if 1
    { // logging:
      uint8_t bit = (diff > (IR_TIMING * 2)) ? 1 : 0;
      static uint8_t byte = 0;
      static uint8_t cc = 0;
      if (cc >= 8) {
        byte = 0;
        cc = 0;
      }
      if (cc > 0) {
        byte <<= 1;
      }
      byte |= bit;
      cc++;
      DEBUG_LOGF("  Read bit: %u", bit);
      if (cc == 8) {
        static uint32_t counter = 0;
        DEBUG_LOGF("Read byte[%u]: 0x%x", counter++, byte);
      }
    }
#endif
    m_recvState = READING_DATA_SPACE;
    break;
  case READING_DATA_SPACE:
    // every 32 bits expect a divider
    if (((m_irData.bitpos() + 1) % DEFAULT_IR_BLOCK_SIZE) == 0) {
      //m_recvState = READING_DATA_DIVIDER_MARK;
      //break;
    }
    m_recvState = READING_DATA_MARK;
    {
      static uint32_t cc = 0;
      cc++;
      // every 32 bytes (8 bits in byte)
      if (cc > 0 && (cc % (32 * 8)) == 0) {
        m_recvState = READING_DATA_DIVIDER_SPACE;
        DEBUG_LOG("RECV BLOCK");
      }
    }
    break;
  case READING_DATA_DIVIDER_MARK:
    m_recvState = READING_DATA_DIVIDER_SPACE;
    break;
  case READING_DATA_DIVIDER_SPACE:
    m_recvState = READING_DATA_MARK;
    break;
  default: // ??
    DEBUG_LOGF("Bad receive state: %u", m_recvState);
    break;
  }
}

void IRReceiver::resetIRState()
{
  m_recvState = WAITING_HEADER_MARK;
  DEBUG_LOG("Resetting IR State...");
  if (m_irData.bytepos()) {
    for (uint32_t i = 0; i < m_irData.bytepos(); ++i) {
      DEBUG_LOGF("(on reset) IR Buf %u: 0x%x", i, m_irData.data()[i]);
    }
  }
  // zero out the receive buffer and reset bit receiver position
  m_irData.reset();
  DEBUG_LOG("IR State Reset Complete");
}
