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

VLSenderCallbacks *g_vlSenderCallbacks;

ByteStream VLSender_serialBuf;
BitStream VLSender_bitStream;
uint8_t VLSender_size = 0;
uint8_t VLSender_parity = 0;

bool VLSender_init()
{
  return true;
}

void VLSender_cleanup()
{
}

bool VLSender_loadMode(const Mode *targetMode)
{
  ByteStream_clear(&VLSender_serialBuf);
  if (!Mode_saveToBuffer(targetMode, &VLSender_serialBuf, 2)) {
    DEBUG_LOG("Failed to save mode to buffer");
    return false;
  }
  if (!ByteStream_decompress(&VLSender_serialBuf)) {
    DEBUG_LOG("Failed to decompress VL buf for sending");
    return false;
  }
  if (ByteStream_rawSize(&VLSender_serialBuf) > VL_MAX_DATA_TRANSFER) {
    DEBUG_LOGF("Cannot transfer that much data: %u bytes", ByteStream_rawSize(&VLSender_serialBuf));
    return false;
  }
  BitStream_initBuf(&VLSender_bitStream, (uint8_t *)ByteStream_rawData(&VLSender_serialBuf), ByteStream_rawSize(&VLSender_serialBuf));
  VLSender_size = (uint8_t)ByteStream_rawSize(&VLSender_serialBuf);
  VLSender_parity = 0;
  DEBUG_LOGF("Size: %u", VLSender_size);
  return true;
}

void VLSender_send(const Mode *targetMode)
{
  VLSender_loadMode(targetMode);
  VLSender_sendMarkSpace(VL_HEADER_MARK, VL_HEADER_SPACE);
  for (uint8_t b = 0; b < 2; b++) {
    VLSender_sendMarkSpace(VL_TIMING_BIT_ZERO, VL_TIMING_BIT_ZERO);
    VLSender_sendMarkSpace(VL_TIMING_BIT_ONE, VL_TIMING_BIT_ONE);
  }
  VLSender_sendByte(1);
  VLSender_sendByte(VLSender_size);
  for (uint8_t i = 0; i < VLSender_size; ++i) {
    VLSender_sendByte(BitStream_peekData(&VLSender_bitStream, i));
  }
  VLSender_sendMarkSpace(VL_TIMING_BIT_ZERO, VL_TIMING_BIT_ZERO);
}

void VLSender_sendLegacy(const Mode *targetMode)
{
  VLSender_loadMode(targetMode);
  VLSender_sendMarkSpace(50, 50);
  VLSender_sendMarkSpace(VL_HEADER_MARK_LEGACY, VL_HEADER_SPACE_LEGACY);
  VLSender_sendByteLegacy(1);
  VLSender_sendByteLegacy(VLSender_size);
  for (uint8_t i = 0; i < VLSender_size; ++i) {
    VLSender_sendByteLegacy(BitStream_peekData(&VLSender_bitStream, i));
  }
}

void VLSender_sendByte(uint8_t data)
{
  for (uint8_t i = 0; i < 8; i += 2) {
    uint8_t pair = (data >> (6 - i)) & 0x3;
    uint8_t mark = (pair >> 1) & 1;
    uint8_t space = pair & 1;
    VLSender_sendMarkSpace(VL_TIMING_BIT(mark), VL_TIMING_BIT(space));
    VLSender_parity ^= mark ^ space;
  }
  VLSender_sendMarkSpace(VL_TIMING_BIT(VLSender_parity & 1), VL_TIMING_BIT(0));
}

void VLSender_sendByteLegacy(uint8_t data)
{
  for (uint8_t b = 0; b < 8; b++) {
    uint8_t bit = (data >> (7 - b)) & 1;
    VLSender_sendMarkSpace(VL_TIMING_BIT_LEGACY(bit), VL_TIMING);
  }
}

void VLSender_sendMarkSpace(uint16_t markTime, uint16_t spaceTime)
{
#ifdef VORTEX_LIB
  g_vlSenderCallbacks->infraredWrite(true, markTime);
  g_vlSenderCallbacks->infraredWrite(false, spaceTime);
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
  uint8_t oldBrightness = Leds_getBrightness();
  Leds_setBrightness(255);
  Leds_setIndex(LED_0, RGB_COLOR(RGB_WHITE));
  Leds_update();
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
