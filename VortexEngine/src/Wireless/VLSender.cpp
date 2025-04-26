#include "VLSender.h"
#include "IRConfig.h"

#if VL_ENABLE_SENDER == 1

#include "../Time/TimeControl.h"
#include "../Modes/Mode.h"
#include "../Leds/Leds.h"
#include "../Log/Log.h"

#ifdef VORTEX_LIB
#include "VortexLib.h"
#endif

// the serial buffer for the data
ByteStream VLSender::m_serialBuf;
// a bit walker for the serial data
BitStream VLSender::m_bitStream;
// some runtime meta info
uint8_t VLSender::m_size = 0;

bool VLSender::init()
{
  return true;
}

void VLSender::cleanup()
{
}

bool VLSender::loadMode(const Mode *targetMode)
{
  m_serialBuf.clear();
  // save the target mode to it's savefile buffer format
  if (!targetMode->saveToBuffer(m_serialBuf, 2)) {
    DEBUG_LOG("Failed to save mode to buffer");
    return false;
  }
  // decompress the mode on lifi, no compression here
  if (!m_serialBuf.decompress()) {
    DEBUG_LOG("Failed to decompress VL buf for sending");
    return false;
  }
  // ensure the data isn't too big
  if (m_serialBuf.rawSize() > VL_MAX_DATA_TRANSFER) {
    DEBUG_LOGF("Cannot transfer that much data: %u bytes", m_serialBuf.rawSize());
    return false;
  }
  // point the bitstream at the serial buffer
  m_bitStream.init((uint8_t *)m_serialBuf.rawData(), m_serialBuf.rawSize());
  // the size of the buf should never really exceed 256
  m_size = (uint8_t)m_serialBuf.rawSize();
  DEBUG_LOGF("Size: %u", m_size);
  return true;
}

void VLSender::send(const Mode *targetMode)
{
  loadMode(targetMode);
  // now send the header
  sendMarkSpace(VL_HEADER_MARK, VL_HEADER_SPACE);
  // send some sync bytes to let the receiver determine baudrate, this will
  // send: (zero mark - zero space - one mark - one space) x 2
  // then the receiver will add up all the marks and all the spaces and divide
  // each by 4 to get the middle point between the one/zero for both marks and spaces
  for (uint8_t b = 0; b < 2; b++) {
    sendMarkSpace(VL_TIMING_BIT_ZERO, VL_TIMING_BIT_ZERO);
    sendMarkSpace(VL_TIMING_BIT_ONE, VL_TIMING_BIT_ONE);
  }
  // it ends up being way cheaper just to send this legacy byte than to
  // implement logic in the receiver to account for this byte being missing in
  // the new protocol, so we send it, originally it was the block count but
  // that always ended up being 1 anyway for the Duo because blocksize was 255
  // and the duo mode size max was no more than ~76 bytes? (I forget right now)
  sendByte(1);
  // send the size of the data first (this was the blocksize)
  sendByte(m_size);
  // iterate each byte in the block and write it
  for (uint8_t i = 0; i < m_size; ++i) {
    // write the byte
    sendByte(m_bitStream.peekData(i));
  }
  // send one last blink because the previous data could end on a space
  // so the receiver needs a last blink to determine it's length
  sendMarkSpace(VL_TIMING_BIT_ZERO, VL_TIMING_BIT_ZERO);
}

void VLSender::sendLegacy(const Mode *targetMode)
{
  loadMode(targetMode);
  // wakeup receiver
  sendMarkSpace(50, 50);
  // now send the header
  sendMarkSpace(VL_HEADER_MARK_LEGACY, VL_HEADER_SPACE_LEGACY);
  // the number of blocks is always 1, previously block size would be for big
  // transfers like IR but in reality that never happens on a duo so it is 1
  sendByteLegacy(1);
  // the next byte is the size or 'remainder' in the last block, so 1 block
  // means the size in this byte is the size of that block. But again on duo
  // this protocol ended up being impractical and only 1 block is ever needed.
  // The purpose of separating blocks was to put a space between blocks for
  // improved sender/receiver synchronization instead of one long stream
  sendByteLegacy(m_size);
  // iterate each byte in the block and write it
  for (uint8_t i = 0; i < m_size; ++i) {
    // write the byte
    sendByteLegacy(m_bitStream.peekData(i));
  }
}

void VLSender::sendByte(uint8_t data)
{
  uint8_t parity = 0;
  // Loop through bits 7-0 two at a time
  for (uint8_t i = 0; i < 8; i += 2) {
    // Extract the next two bits as a 2-bit value
    uint8_t pair = (data >> (6 - i)) & 0x3;
    // First bit (mark) is the higher bit
    uint8_t mark = (pair >> 1) & 1;
    // Second bit (space) is the lower bit
    uint8_t space = pair & 1;
    // Send both bits as a mark-space pair
    sendMarkSpace(VL_TIMING_BIT(mark), VL_TIMING_BIT(space));
    // Update parity by XOR'ing both bits
    parity ^= mark ^ space;
  }
  // Send final parity bit as mark, followed by a 0 space
  sendMarkSpace(VL_TIMING_BIT(parity & 1), VL_TIMING_BIT(0));
}

void VLSender::sendByteLegacy(uint8_t data)
{
  // Sends from left to right, MSB first
  for (uint8_t b = 0; b < 8; b++) {
    // grab the bit of data at the index
    uint8_t bit = (data >> (7 - b)) & 1;
    // send 3x timing size for 1s and 1x timing for 0
    sendMarkSpace(VL_TIMING_BIT_LEGACY(bit), VL_TIMING);
  }
}

void VLSender::sendMarkSpace(uint16_t markTime, uint16_t spaceTime)
{
#ifdef VORTEX_LIB
  // send mark timing over socket
  Vortex::vcallbacks()->infraredWrite(true, markTime);
  // send space timing over socket
  Vortex::vcallbacks()->infraredWrite(false, spaceTime);
#else
  startPWM();
  Time::delayMicroseconds(markTime);
  stopPWM();
  Time::delayMicroseconds(spaceTime);
#endif
}

void VLSender::startPWM()
{
#if defined(VORTEX_EMBEDDED)
  // brightness backup
  uint8_t oldBrightness = Leds::getBrightness();
  // ensure max brightness
  Leds::setBrightness(255);
  Leds::setIndex(LED_0, RGB_WHITE);
  Leds::update();
  // restore brightness
  Leds::setBrightness(oldBrightness);
#endif
}

void VLSender::stopPWM()
{
#if defined(VORTEX_EMBEDDED)
  Leds::clearAll();
  Leds::update();
#endif
}

#endif
