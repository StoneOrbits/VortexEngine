#include "VLReceiver.h"
#include "IRConfig.h"

#if VL_ENABLE_RECEIVER == 1

#include "../Serial/ByteStream.h"
#include "../Serial/BitStream.h"
#include "../Time/TimeControl.h"
#include "../Modes/Mode.h"
#include "../Leds/Leds.h"
#include "../Log/Log.h"

#ifdef VORTEX_EMBEDDED
#include <avr/interrupt.h>
#include <avr/io.h>
#endif

BitStream VLReceiver_vlData;
VLRecvState VLReceiver_recvState = VL_WAITING_HEADER_MARK;
uint32_t VLReceiver_prevTime = 0;
uint8_t VLReceiver_pinState = 0;
uint16_t VLReceiver_previousBytes = 0;
uint16_t VLReceiver_vlMarkThreshold = 0;
uint16_t VLReceiver_vlSpaceThreshold = 0;
uint8_t VLReceiver_counter = 0;
uint8_t VLReceiver_parityBit = 0;
bool VLReceiver_legacy = false;

#ifdef VORTEX_EMBEDDED
#define MIN_THRESHOLD   200
#define BASE_OFFSET     100
#define THRESHOLD_BEGIN (MIN_THRESHOLD + BASE_OFFSET)
#define SAMPLE_COUNT    5
uint16_t threshold = THRESHOLD_BEGIN;
ISR(ADC0_WCOMP_vect)
{
  static bool wasAboveThreshold = false;
  uint16_t val = (ADC0.RES >> SAMPLE_COUNT);
  if (val > MIN_THRESHOLD && val < (threshold + BASE_OFFSET)) {
    threshold = val + BASE_OFFSET;
  }
  bool isAboveThreshold = (val > threshold);
  if (wasAboveThreshold != isAboveThreshold) {
    VLReceiver_recvPCIHandler();
    wasAboveThreshold = isAboveThreshold;
  }
  ADC0.INTFLAGS = ADC_WCMP_bm;
}
#endif

bool VLReceiver_init()
{
#ifdef VORTEX_EMBEDDED
  PORTB.PIN1CTRL &= ~PORT_ISC_gm;
  PORTB.PIN1CTRL |= PORT_ISC_INPUT_DISABLE_gc;
#endif
  return BitStream_initAlloc(&VLReceiver_vlData, VL_RECV_BUF_SIZE);
}

void VLReceiver_cleanup()
{
}

bool VLReceiver_dataReady()
{
  if (!VLReceiver_isReceiving()) {
    return false;
  }
  uint8_t size = BitStream_peekData(&VLReceiver_vlData, 1);
  return (BitStream_bytepos(&VLReceiver_vlData) >= ((uint32_t)size + 2));
}

bool VLReceiver_isReceiving()
{
  return (BitStream_bytepos(&VLReceiver_vlData) > 2);
}

uint8_t VLReceiver_percentReceived()
{
  if (!VLReceiver_isReceiving()) {
    return 0;
  }
  uint8_t size = BitStream_peekData(&VLReceiver_vlData, 1);
  return (uint8_t)((uint16_t)((BitStream_bytepos(&VLReceiver_vlData) * 100 + (size / 2)) / size));
}

bool VLReceiver_receiveMode(Mode *pMode)
{
  ByteStream buf;
  if (!VLReceiver_read(&buf)) {
    DEBUG_LOG("No data available to read, or error reading");
    return false;
  }
  DEBUG_LOGF("Received %u bytes", ByteStream_rawSize(&buf));
  return Mode_loadFromBuffer(pMode, &buf);
}

bool VLReceiver_beginReceiving()
{
#ifdef VORTEX_EMBEDDED
  ADC0.CTRLC = ADC_SAMPCAP_bm | ADC_REFSEL_VDDREF_gc | ADC_PRESC_DIV2_gc;
  ADC0.CTRLD = 0;
  ADC0.SAMPCTRL = 1;
  ADC0.MUXPOS = ADC_MUXPOS_AIN10_gc;
  ADC0.CTRLE = ADC_WINCM_ABOVE_gc;
  ADC0.WINHT = 0x1;
  ADC0.WINLT = 0;
  ADC0.CTRLB = SAMPLE_COUNT;
  ADC0.INTCTRL = ADC_WCMP_bm;
  ADC0.CTRLA = ADC_ENABLE_bm | ADC_FREERUN_bm;
  ADC0.COMMAND = ADC_STCONV_bm;
#endif
  VLReceiver_resetVLState();
  return true;
}

bool VLReceiver_endReceiving()
{
#ifdef VORTEX_EMBEDDED
  ADC0.CTRLA &= ~(ADC_ENABLE_bm | ADC_FREERUN_bm);
  ADC0.INTCTRL = 0;
#endif
  return true;
}

bool VLReceiver_onNewData()
{
  if (VLReceiver_bytesReceived() == VLReceiver_previousBytes) {
    return false;
  }
  VLReceiver_previousBytes = VLReceiver_bytesReceived();
  return true;
}

bool VLReceiver_read(ByteStream *data)
{
  if (!BitStream_bytepos(&VLReceiver_vlData) || BitStream_bytepos(&VLReceiver_vlData) > VL_MAX_DATA_TRANSFER) {
    DEBUG_LOG("Nothing to read, or read too much");
    return false;
  }
  uint8_t size = BitStream_peekData(&VLReceiver_vlData, 1);
  const uint8_t *actualData = BitStream_data(&VLReceiver_vlData) + 2;
  if (!ByteStream_rawInit(data, actualData, size)) {
    DEBUG_LOG("Failed to init buffer for VL read");
    return false;
  }
  VLReceiver_resetVLState();
  return true;
}

void VLReceiver_recvPCIHandler()
{
  VLReceiver_pinState = (uint8_t)!VLReceiver_pinState;
  uint32_t now = Time_microseconds();
  uint32_t diff = (uint32_t)(now - VLReceiver_prevTime);
  VLReceiver_prevTime = now;
  if (diff > UINT16_MAX) {
    return;
  }
  if (VLReceiver_legacy) {
    VLReceiver_handleVLTimingLegacy((uint16_t)diff);
  } else {
    VLReceiver_handleVLTiming((uint16_t)diff);
  }
}

void VLReceiver_handleVLTimingLegacy(uint16_t diff)
{
  switch (VLReceiver_recvState) {
  case VL_WAITING_HEADER_MARK:
    if (diff >= VL_HEADER_SPACE_MIN_LEGACY && diff <= VL_HEADER_MARK_MAX_LEGACY) {
      VLReceiver_recvState = VL_WAITING_HEADER_SPACE;
    }
    break;
  case VL_WAITING_HEADER_SPACE:
    if (diff >= VL_HEADER_SPACE_MIN_LEGACY && diff <= VL_HEADER_MARK_MAX_LEGACY) {
      VLReceiver_recvState = VL_READING_DATA_MARK;
      VLReceiver_vlMarkThreshold = (diff / 4);
    }
    break;
  case VL_READING_DATA_MARK:
    BitStream_write1Bit(&VLReceiver_vlData, diff > VLReceiver_vlMarkThreshold);
    VLReceiver_recvState = VL_READING_DATA_SPACE;
    break;
  case VL_READING_DATA_SPACE:
    VLReceiver_recvState = VL_READING_DATA_MARK;
    break;
  default:
    DEBUG_LOGF("Bad receive state: %u", VLReceiver_recvState);
    break;
  }
}

void VLReceiver_handleVLTiming(uint16_t diff)
{
  uint8_t bit;
  switch (VLReceiver_recvState) {
  case VL_WAITING_HEADER_MARK:
  case VL_WAITING_HEADER_SPACE:
    if (diff >= VL_HEADER_SPACE_MIN && diff <= VL_HEADER_MARK_MAX) {
      VLReceiver_recvState = (VLRecvState)(VLReceiver_recvState + 1);
    }
    break;
  case VL_READING_BAUD_MARK:
    VLReceiver_vlMarkThreshold += diff;
    VLReceiver_recvState = VL_READING_BAUD_SPACE;
    break;
  case VL_READING_BAUD_SPACE:
    VLReceiver_counter++;
    VLReceiver_vlSpaceThreshold += diff;
    if (VLReceiver_counter < 4) {
      VLReceiver_recvState = VL_READING_BAUD_MARK;
      break;
    }
    VLReceiver_vlMarkThreshold /= 4;
    VLReceiver_vlSpaceThreshold /= 4;
    VLReceiver_counter = 0;
    VLReceiver_parityBit = 0;
    VLReceiver_recvState = VL_READING_DATA_MARK;
    break;
  case VL_READING_DATA_MARK:
    VLReceiver_counter++;
    bit = (diff > VLReceiver_vlMarkThreshold) ? 1 : 0;
    VLReceiver_parityBit = (VLReceiver_parityBit ^ bit) & 1;
    BitStream_write1Bit(&VLReceiver_vlData, bit);
    VLReceiver_recvState = VL_READING_DATA_SPACE;
    break;
  case VL_READING_DATA_SPACE:
    bit = (diff > VLReceiver_vlSpaceThreshold) ? 1 : 0;
    VLReceiver_parityBit = (VLReceiver_parityBit ^ bit) & 1;
    BitStream_write1Bit(&VLReceiver_vlData, bit);
    if ((VLReceiver_counter % 4) == 0) {
      VLReceiver_recvState = VL_READING_DATA_PARITY_MARK;
    } else {
      VLReceiver_recvState = VL_READING_DATA_MARK;
    }
    break;
  case VL_READING_DATA_PARITY_MARK:
    bit = (diff > VLReceiver_vlMarkThreshold) ? 1 : 0;
    if ((VLReceiver_parityBit & 1) != bit) {
      VLReceiver_resetVLState();
      break;
    }
    VLReceiver_recvState = VL_READING_DATA_PARITY_SPACE;
    break;
  case VL_READING_DATA_PARITY_SPACE:
    VLReceiver_recvState = VL_READING_DATA_MARK;
    break;
  default:
    DEBUG_LOGF("Bad receive state: %u", VLReceiver_recvState);
    break;
  }
}

void VLReceiver_resetVLState()
{
  VLReceiver_counter = 0;
  VLReceiver_vlMarkThreshold = 0;
  VLReceiver_vlSpaceThreshold = 0;
  VLReceiver_previousBytes = 0;
  VLReceiver_parityBit = 0;
  VLReceiver_recvState = VL_WAITING_HEADER_MARK;
  BitStream_reset(&VLReceiver_vlData);
#ifdef VORTEX_EMBEDDED
  threshold = THRESHOLD_BEGIN;
#endif
  DEBUG_LOG("VL State Reset");
}

#endif // VL_ENABLE_RECEIVER == 1

void VLReceiver_setLegacyReceiver(bool legacy)
{
  (void)legacy;
}

