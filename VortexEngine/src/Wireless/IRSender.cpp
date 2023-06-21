#include "IRSender.h"

#include "../Time/TimeControl.h"
#include "../Modes/Mode.h"
#include "../Leds/Leds.h"
#include "../Log/Log.h"

#include "IRConfig.h"

#ifdef VORTEX_LIB
#include "Arduino.h"
#endif

#if IR_ENABLE_SENDER == 1

// the serial buffer for the data
ByteStream IRSender::m_serialBuf;
// a bit walker for the serial data
BitStream IRSender::m_bitStream;
// whether actively sending
bool IRSender::m_isSending = false;
// the time of the last sent chunk
uint64_t IRSender::m_lastSendTime = 0;
// some runtime meta info
uint32_t IRSender::m_size = 0;
// the number of blocks that will be sent
uint8_t IRSender::m_numBlocks = 0;
// the amount in the final block
uint8_t IRSender::m_remainder = 0;
// configuration options for the sender
uint32_t IRSender::m_blockSize = 0;
// write total
uint32_t IRSender::m_writeCounter = 0;

bool IRSender::init()
{
  return true;
}

void IRSender::cleanup()
{
}

bool IRSender::loadMode(const Mode *targetMode)
{
  m_serialBuf.clear();
  // save the target mode to it's savefile buffer format
  if (!targetMode->saveToBuffer(m_serialBuf)) {
    DEBUG_LOG("Failed to save mode to buffer");
    return false;
  }
  // ensure the data isn't too big
  if (m_serialBuf.rawSize() > IR_MAX_DATA_TRANSFER) {
    DEBUG_LOGF("Cannot transfer that much data: %u bytes", m_serialBuf.rawSize());
    return false;
  }
  // point the bitstream at the serial buffer
  m_bitStream.init((uint8_t *)m_serialBuf.rawData(), m_serialBuf.rawSize());
  // the size of the packet
  m_size = m_serialBuf.rawSize();
  // the number of blocks that will be sent (possibly just 1 with less than 32 bytes)
  m_numBlocks = (m_size + (IR_DEFAULT_BLOCK_SIZE - 1)) / IR_DEFAULT_BLOCK_SIZE;
  // the amount in the final block (possibly the only block)
  m_remainder = m_size % (IR_DEFAULT_BLOCK_SIZE + 0);
  DEBUG_LOGF("Num blocks: %u", m_numBlocks);
  DEBUG_LOGF("Remainder: %u", m_remainder);
  DEBUG_LOGF("Size: %u", m_size);
  // reset write counter
  m_writeCounter = 0;
  return true;
}

bool IRSender::send()
{
  // must have at least one block to send
  if (m_numBlocks < 1) {
    m_isSending = false;
    return m_isSending;
  }
  if (!m_isSending) {
    beginSend();
  }
  if (m_lastSendTime > 0 && m_lastSendTime + IR_DEFAULT_BLOCK_SPACING > Time::getCurtime()) {
    // don't send yet
    return m_isSending;
  }
  // blocksize is default unless it's the last block, then it's the remainder
  uint32_t blocksize = (m_numBlocks == 1) ? m_remainder : IR_DEFAULT_BLOCK_SIZE;
  DEBUG_LOGF("IRSender Sending block #%u", m_numBlocks);
  // pointer to the data at the correct location
  const uint8_t *buf_ptr = m_bitStream.data() + m_writeCounter;
  // iterate each byte in the block and write it
  for (uint32_t i = 0; i < blocksize; ++i) {
    // write the byte
    sendByte(*buf_ptr);
    // increment to next byte in buffer
    buf_ptr++;
    // record the written byte
    m_writeCounter++;
  }
  // wrote one block
  m_numBlocks--;
  // still sending if we have more blocks
  m_isSending = (m_numBlocks > 0);
  // the curtime
  m_lastSendTime = Time::getCurtime();
  // return whether still sending
  return m_isSending;
}

void IRSender::beginSend()
{
  m_isSending = true;
  DEBUG_LOGF("[%zu] Beginning send size %u (blocks: %u remainder: %u blocksize: %u)",
    micros(), m_size, m_numBlocks, m_remainder, m_blockSize);
  // wakeup the other receiver with a very quick mark/space
  sendMark(50);
  sendSpace(100);
  // now send the header
  sendMark(IR_HEADER_MARK);
  sendSpace(IR_HEADER_SPACE);
  // reset writeCounter
  m_writeCounter = 0;
  // write the number of blocks being sent, most likely just 1
  sendByte(m_numBlocks);
  // now write the number of bytes in the last block
  sendByte(m_remainder);
}

void IRSender::sendByte(uint8_t data)
{
  // Sends from left to right, MSB first
  for (int b = 0; b < 8; b++) {
    // grab the bit of data at the indexspace
    uint32_t bit = (data >> (7 - b)) & 1;
    // send 3x timing size for 1s and 1x timing for 0
    sendMark(IR_TIMING + (IR_TIMING * (2 * bit)));
    // send 1x timing size for space
    sendSpace(IR_TIMING);
  }
  DEBUG_LOGF("Sent byte[%u]: 0x%x", m_writeCounter, data);
 }

void IRSender::sendMark(uint16_t time)
{
#ifdef VORTEX_LIB
  // send mark timing over socket
  send_ir(true, time);
#else
  startPWM();
  delayMicroseconds(time);
#endif
}

void IRSender::sendSpace(uint16_t time)
{
#ifdef VORTEX_LIB
  // send space timing over socket
  send_ir(false, time);
#else
  stopPWM();
  delayMicroseconds(time);
#endif
}

void IRSender::startPWM()
{
#if defined(VORTEX_ARDUINO) && IR_ENABLE == 1
  // brightness backup
  uint8_t oldBrightness = Leds::getBrightness();
  // ensure max brightness
  Leds::setBrightness(255);
  Leds::setAll(RGB_WHITE);
  Leds::update();
  // restore brightness
  Leds::setBrightness(oldBrightness);
#endif
}

void IRSender::stopPWM()
{
#if defined(VORTEX_ARDUINO) && IR_ENABLE == 1
  Leds::clearAll();
  Leds::update();
#endif
}

#endif
