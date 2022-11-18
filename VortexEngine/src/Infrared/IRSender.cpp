#include "IRSender.h"
#include "Infrared.h"

#include "../Modes/Mode.h"
#include "../Log/Log.h"

#include <Arduino.h>

IRSender::IRSender(uint32_t block_size_bits) :
  m_serialBuf(),
  m_bitStream(),
  m_isSending(false),
  m_startTime(0),
  m_size(0),
  m_numBlocks(0),
  m_remainderBlocks(0),
  m_blockSize(block_size_bits)
{
}

IRSender::IRSender(const Mode *targetMode) :
  IRSender()
{
  init(targetMode);
}

IRSender::~IRSender()
{
}

bool IRSender::init(const Mode *targetMode)
{
  targetMode->serialize(m_serialBuf);
  if (!m_serialBuf.compress()) {
    DEBUG_LOG("Failed to compress, aborting send");
    return false;
    // tried
  }
  // ensure the data isn't too big
  if (m_serialBuf.rawSize() > MAX_DATA_TRANSFER) {
    DEBUG_LOGF("Cannot transfer that much data: %u bytes", m_serialBuf.rawSize());
    return false;
  }
  // point the bitstream at the serial buffer
  m_bitStream.init((uint8_t *)m_serialBuf.rawData(), m_serialBuf.rawSize());
  // the size of the packet
  m_size = m_serialBuf.rawSize();
  // the number of blocks that will be sent
  m_numBlocks = (m_size + (DEFAULT_IR_BLOCK_SIZE - 1)) / DEFAULT_IR_BLOCK_SIZE;
  // the amount in the final block
  m_remainderBlocks = m_size % DEFAULT_IR_BLOCK_SIZE;
  return true;
}

bool IRSender::initSend()
{
  uint64_t startTime = micros();
  m_isSending = true;
  DEBUG_LOGF("[%zu] Beginning send size %u (blocks: %u remainder: %u blocksize: %u)", 
    startTime, m_size, m_numBlocks, m_remainderBlocks, m_blockSize);
  // begin the write
  Infrared::startWrite();
  // write the number of blocks being sent, most likely just 1
  Infrared::write8(m_numBlocks);
  // now write the number of bytes in the last block
  Infrared::write8(m_remainderBlocks);
  return true;
}

void IRSender::send()
{
  if (!m_isSending) {
    if (!initSend()) {
      return;
    }
  }
#if 0
  uint8_t *buf_ptr = buf;
  // iterate each block
  for (uint32_t block = 0; block < num_blocks; ++block) {
    // write the block which is 32 bytes unless on the last block
    uint32_t blocksize = (block == (num_blocks - 1)) ? remainder : 32;
    // iterate each byte in the block and write it
    for (uint32_t i = 0; i < blocksize; ++i) {
      write8(*buf_ptr);
      buf_ptr++;
    }
  }
  //=============================================
  buf_ptr = buf;
  for (uint32_t block = 0; block < num_blocks; ++block) {
    uint32_t blocksize = (num_blocks == 1) ? remainder : 32;
    for (uint32_t i = 0; i < blocksize; ++i) {
      DEBUG_LOGF("Wrote %u: 0x%x", i, *buf_ptr);
      buf_ptr++;
    }
    // delay if there is more blocks
    if (block < (num_blocks - 1)) {
      DEBUG_LOG("Block");
    }
  }
#endif

  DEBUG_LOGF("Wrote %u buf (%u us)", m_serialBuf.rawSize(), micros() - m_startTime);
  DEBUG_LOG("Success sending");
}

void IRSender::write8(uint8_t data)
{
  // Sends from left to right, MSB first
  for (int b = 0; b < 8; b++) {
    // grab the bit of data at the index
    uint32_t bit = (data >> (7 - b)) & 1;
    // send 3x timing size for 1s and 1x timing for 0
    Infrared::mark(IR_TIMING + (IR_TIMING * (2 * bit)));
    // send 1x timing size for space
    Infrared::space(IR_TIMING);
    //DEBUG_LOGF(" bit: %u", bit);
  }
  //DEBUG_LOGF("Sent byte[%u]: 0x%x", m_writeCounter, data);
  //m_writeCounter++;
 }


