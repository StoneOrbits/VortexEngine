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
static ByteStream s_serialBuf;
// a bit walker for the serial data
static BitStream s_bitStream;
// some runtime meta info
static uint8_t s_size = 0;
static uint8_t s_parity = 0;

// release the serial buffer, it lives in the ByteStream arena and sending
// is blocking so there is no reason to hold it between sends
static void VLSender_unloadMode()
{
  ByteStream_clear(&s_serialBuf);
  BitStream_initBuf(&s_bitStream, NULL, 0);
  s_size = 0;
}

bool VLSender_init()
{
  return true;
}

void VLSender_cleanup()
{
  VLSender_unloadMode();
}

bool VLSender_loadMode(const Mode *targetMode)
{
  VLSender_unloadMode();
  // save the target mode to it's savefile buffer format
  if (!Mode_saveToBuffer(targetMode, &s_serialBuf, 2)) {
    DEBUG_LOG("Failed to save mode to buffer");
    return false;
  }
  // decompress the mode on lifi, no compression here
  if (!ByteStream_decompress(&s_serialBuf)) {
    DEBUG_LOG("Failed to decompress VL buf for sending");
    return false;
  }
  // ensure the data isn't too big
  if (ByteStream_rawSize(&s_serialBuf) > VL_MAX_DATA_TRANSFER) {
    DEBUG_LOGF("Cannot transfer that much data: %u bytes", ByteStream_rawSize(&s_serialBuf));
    return false;
  }
  // point the bitstream at the serial buffer
  BitStream_initBuf(&s_bitStream, (uint8_t *)ByteStream_rawData(&s_serialBuf), ByteStream_rawSize(&s_serialBuf));
  // the size of the buf should never really exceed 256
  s_size = (uint8_t)ByteStream_rawSize(&s_serialBuf);
  // reset parity
  s_parity = 0;
  DEBUG_LOGF("Size: %u", s_size);
  return true;
}

void VLSender_send(const Mode *targetMode)
{
  if (!VLSender_loadMode(targetMode)) {
    // don't send whatever stale data the bitstream was left pointing at
    VLSender_unloadMode();
    return;
  }
  // now send the header
  VLSender_sendMarkSpace(VL_HEADER_MARK, VL_HEADER_SPACE);
  // send some sync bytes to let the receiver determine baudrate, this will
  // send: (zero mark - zero space - one mark - one space) x 2
  // then the receiver will add up all the marks and all the spaces and divide
  // each by 4 to get the middle point between the one/zero for both marks and spaces
  for (uint8_t b = 0; b < 2; b++) {
    VLSender_sendMarkSpace(VL_TIMING_BIT_ZERO, VL_TIMING_BIT_ZERO);
    VLSender_sendMarkSpace(VL_TIMING_BIT_ONE, VL_TIMING_BIT_ONE);
  }
  // it ends up being way cheaper just to send this legacy byte than to
  // implement logic in the receiver to account for this byte being missing in
  // the new protocol, so we send it, originally it was the block count but
  // that always ended up being 1 anyway for the Duo because blocksize was 255
  // and the duo mode size max was no more than ~76 bytes? (I forget right now)
  VLSender_sendByte(1);
  // send the size of the data first (this was the blocksize)
  VLSender_sendByte(s_size);
  // iterate each byte in the block and write it
  for (uint8_t i = 0; i < s_size; ++i) {
    // write the byte
    VLSender_sendByte(BitStream_peekData(&s_bitStream, i));
  }
  // send one last blink because the previous data could end on a space
  // so the receiver needs a last blink to determine it's length
  VLSender_sendMarkSpace(VL_TIMING_BIT_ZERO, VL_TIMING_BIT_ZERO);
  VLSender_unloadMode();
}

void VLSender_sendLegacy(const Mode *targetMode)
{
  if (!VLSender_loadMode(targetMode)) {
    VLSender_unloadMode();
    return;
  }
  // wakeup receiver
  VLSender_sendMarkSpace(50, 50);
  // now send the header
  VLSender_sendMarkSpace(VL_HEADER_MARK_LEGACY, VL_HEADER_SPACE_LEGACY);
  // the number of blocks is always 1, previously block size would be for big
  // transfers like IR but in reality that never happens on a duo so it is 1
  VLSender_sendByteLegacy(1);
  // the next byte is the size or 'remainder' in the last block, so 1 block
  // means the size in this byte is the size of that block. But again on duo
  // this protocol ended up being impractical and only 1 block is ever needed.
  // The purpose of separating blocks was to put a space between blocks for
  // improved sender/receiver synchronization instead of one long stream
  VLSender_sendByteLegacy(s_size);
  // iterate each byte in the block and write it
  for (uint8_t i = 0; i < s_size; ++i) {
    // write the byte
    VLSender_sendByteLegacy(BitStream_peekData(&s_bitStream, i));
  }
  VLSender_unloadMode();
}

void VLSender_sendByte(uint8_t data)
{
  // Loop through bits 7-0 two at a time
  for (uint8_t i = 0; i < 8; i += 2) {
    // Extract the next two bits as a 2-bit value
    uint8_t pair = (data >> (6 - i)) & 0x3;
    // First bit (mark) is the higher bit
    uint8_t mark = (pair >> 1) & 1;
    // Second bit (space) is the lower bit
    uint8_t space = pair & 1;
    // Send both bits as a mark-space pair
    VLSender_sendMarkSpace(VL_TIMING_BIT(mark), VL_TIMING_BIT(space));
    // Update parity by XOR'ing both bits
    s_parity ^= mark ^ space;
  }
  // Send final parity bit as mark, followed by a 0 space
  VLSender_sendMarkSpace(VL_TIMING_BIT(s_parity & 1), VL_TIMING_BIT(0));
}

void VLSender_sendByteLegacy(uint8_t data)
{
  // Sends from left to right, MSB first
  for (uint8_t b = 0; b < 8; b++) {
    // grab the bit of data at the index
    uint8_t bit = (data >> (7 - b)) & 1;
    // send 3x timing size for 1s and 1x timing for 0
    VLSender_sendMarkSpace(VL_TIMING_BIT_LEGACY(bit), VL_TIMING);
  }
}

void VLSender_sendMarkSpace(uint16_t markTime, uint16_t spaceTime)
{
#ifdef VORTEX_LIB
  // send mark timing over socket
  Vortex::vcallbacks()->infraredWrite(true, markTime);
  // send space timing over socket
  Vortex::vcallbacks()->infraredWrite(false, spaceTime);
#else
  VLSender_startPWM();
  Time_delayMicroseconds(markTime);
  VLSender_stopPWM();
  Time_delayMicroseconds(spaceTime);
#endif
}

void VLSender_startPWM()
{
#if defined(VORTEX_EMBEDDED)
  // brightness backup
  uint8_t oldBrightness = Leds_getBrightness();
  // ensure max brightness
  Leds_setBrightness(255);
  Leds_setIndex(LED_0, RGB_WHITE);
  Leds_update();
  // restore brightness
  Leds_setBrightness(oldBrightness);
#endif
}

void VLSender_stopPWM()
{
#if defined(VORTEX_EMBEDDED)
  Leds_clearAll();
  Leds_update();
#endif
}

#endif