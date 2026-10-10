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

// the receive state machine used for the PCI handler
typedef uint8_t RecvState;
enum {
  WAITING_HEADER_MARK,
  WAITING_HEADER_SPACE,
  READING_BAUD_MARK,
  READING_BAUD_SPACE,
  READING_DATA_MARK,
  READING_DATA_SPACE,
  READING_DATA_PARITY_MARK,
  READING_DATA_PARITY_SPACE
};

// BitStream object that VL data is fed to bit by bit
static BitStream s_vlData;
// state information used by the PCIHandler
static RecvState s_recvState = WAITING_HEADER_MARK;
// used to track pin changes
static uint32_t s_prevTime = 0;
static uint8_t s_pinState = 0;
// used to compare if received data has changed since last checking
static uint16_t s_previousBytes = 0;
// the determined time based on sync
static uint16_t s_vlMarkThreshold = 0;
static uint16_t s_vlSpaceThreshold = 0;
// counter is reused for two purposes
static uint8_t s_counter = 0;
// parity is for each byte of data received
static uint8_t s_parityBit = 0;
// legacy is for old receiving method (sucks)
static bool s_legacy = false;

#ifdef VORTEX_EMBEDDED
#define MIN_THRESHOLD   200
#define BASE_OFFSET     100
#define THRESHOLD_BEGIN (MIN_THRESHOLD + BASE_OFFSET)
// the sample count exponent, so 5 means 2^5 = 32 samples
//   0 NONE No accumulation > doesn't work
//   1 ACC2 2 results accumulated  > doesn't work
//   2 ACC4 4 results accumulated  > works okay
//   3 ACC8 8 results accumulated  > works decent
//   4 ACC16 16 results accumulated > works very well
//   5 ACC32 32 results accumulated > works best
//   6 ACC64 64 results accumulated > doesn't work
#define SAMPLE_COUNT    5
// the threshold needs to start high then it will be automatically pulled down
static uint16_t s_threshold = THRESHOLD_BEGIN;
ISR(ADC0_WCOMP_vect)
{
  // this will store the last known state
  static bool wasAboveThreshold = false;
  // grab the current analog value but divide it by 8 (the number of samples)
  uint16_t val = (ADC0.RES >> SAMPLE_COUNT);
  // calculate a threshold by using the baseline minimum value that is above 0
  // with a static offset, this ensures whatever the baseline light level and/or
  // hardware sensitivity is it will always pick a threshold just above the 'off'
  if (val > MIN_THRESHOLD && val < (s_threshold + BASE_OFFSET)) {
    s_threshold = val + BASE_OFFSET;
  }
  // compare the current analog value to the light threshold
  bool isAboveThreshold = (val > s_threshold);
  if (wasAboveThreshold != isAboveThreshold) {
    VLReceiver_recvPCIHandler();
    wasAboveThreshold = isAboveThreshold;
  }
  // Clear the Window Comparator interrupt flag
  ADC0.INTFLAGS = ADC_WCMP_bm;
}
#endif

bool VLReceiver_init()
{
#ifdef VORTEX_EMBEDDED
  // Disable digital input buffer on the pin to save power
  PORTB.PIN1CTRL &= ~PORT_ISC_gm;
  PORTB.PIN1CTRL |= PORT_ISC_INPUT_DISABLE_gc;
#endif
  // receive into a static buffer so the receiver never touches the heap
  static uint8_t s_vlDataBuf[VL_RECV_BUF_SIZE];
  return BitStream_initBuf(&s_vlData, s_vlDataBuf, VL_RECV_BUF_SIZE);
}

void VLReceiver_cleanup()
{
}

bool VLReceiver_dataReady()
{
  // is the receiver actually receiving data?
  if (!VLReceiver_isReceiving()) {
    return false;
  }
  uint8_t size = BitStream_peekData(&s_vlData, 1);
  // check if there are size + 1 bytes in the VLData receiver
  return (BitStream_bytepos(&s_vlData) >= ((uint32_t)size + 2));
}

// whether actively receiving
bool VLReceiver_isReceiving()
{
  // if there are at least 2 bytes in the data buffer then
  // the receiver is receiving a packet. If there is less
  // than 2 bytes then we're still waiting for the 'blocks'
  // and 'remainder' bytes which prefix a packet
  return (BitStream_bytepos(&s_vlData) > 2);
}

// the percent of data received
uint8_t VLReceiver_percentReceived()
{
  if (!VLReceiver_isReceiving()) {
    return 0;
  }
  uint8_t size = BitStream_peekData(&s_vlData, 1);
  // round by adding half of the total to the numerator
  return (uint8_t)((uint16_t)((BitStream_bytepos(&s_vlData) * 100 + (size / 2)) / size));
}

bool VLReceiver_receiveMode(Mode *pMode)
{
  ByteStream buf;
  ByteStream_make(&buf);
  // read from the receive buffer into the byte stream
  if (!VLReceiver_read(&buf)) {
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

bool VLReceiver_beginReceiving()
{
#ifdef VORTEX_EMBEDDED
  // Set up the ADC
  // sample campacitance, VDD reference, prescaler division
  // Options are:
  //  0x0 DIV2 CLK_PER divided by 2 > works
  //  0x1 DIV4 CLK_PER divided by 4 > works
  //  0x2 DIV8 CLK_PER divided by 8 > works
  //  0x3 DIV16 CLK_PER divided by 16 > works
  //  0x4 DIV32 CLK_PER divided by 32 > doesn't work
  //  0x5 DIV64 CLK_PER divided by 64 > doesn't work
  //  0x6 DIV128 CLK_PER divided by 128 > doesn't work
  //  0x7 DIV256 CLK_PER divided by 256 > doesn't work
  ADC0.CTRLC = ADC_SAMPCAP_bm | ADC_REFSEL_VDDREF_gc | ADC_PRESC_DIV2_gc;
  // no sampling delay and no delay variation
  ADC0.CTRLD = 0;
  // sample length
  //  0 = doesn't work
  //  1+ = works
  ADC0.SAMPCTRL = 1;
  // Select the analog pin input PB1 (AIN10)
  ADC0.MUXPOS = ADC_MUXPOS_AIN10_gc;
  // Initialize the Window Comparator Mode in above
  ADC0.CTRLE = ADC_WINCM_ABOVE_gc;
  // Set the threshold value very low
  ADC0.WINHT = 0x1;
  ADC0.WINLT = 0;
  // set sampling amount
  //   0x0 NONE No accumulation > doesn't work
  //   0x1 ACC2 2 results accumulated  > doesn't work
  //   0x2 ACC4 4 results accumulated  > works okay
  //   0x3 ACC8 8 results accumulated  > works decent
  //   0x4 ACC16 16 results accumulated > works very well
  //   0x5 ACC32 32 results accumulated > works best
  //   0x6 ACC64 64 results accumulated > doesn't work
  ADC0.CTRLB = SAMPLE_COUNT;
  // Enable Window Comparator interrupt
  ADC0.INTCTRL = ADC_WCMP_bm;
  // Enable the ADC and start continuous conversions
  ADC0.CTRLA = ADC_ENABLE_bm | ADC_FREERUN_bm;
  // start the first conversion
  ADC0.COMMAND = ADC_STCONV_bm;
#endif
  VLReceiver_resetVLState();
  return true;
}

bool VLReceiver_endReceiving()
{
#ifdef VORTEX_EMBEDDED
  // Stop conversions and disable the ADC
  ADC0.CTRLA &= ~(ADC_ENABLE_bm | ADC_FREERUN_bm);
  ADC0.INTCTRL = 0;
#endif
  return true;
}

bool VLReceiver_onNewData()
{
  if (VLReceiver_bytesReceived() == s_previousBytes) {
    return false;
  }
  s_previousBytes = VLReceiver_bytesReceived();
  return true;
}

uint16_t VLReceiver_bytesReceived()
{
  return BitStream_bytepos(&s_vlData);
}

bool VLReceiver_read(ByteStream *data)
{
  if (!BitStream_bytepos(&s_vlData) || BitStream_bytepos(&s_vlData) > VL_MAX_DATA_TRANSFER) {
    DEBUG_LOG("Nothing to read, or read too much");
    return false;
  }
  uint8_t size = BitStream_peekData(&s_vlData, 1);
  const uint8_t *actualData = BitStream_data(&s_vlData) + 2;
  if (!ByteStream_rawInit(data, actualData, size)) {
    DEBUG_LOG("Failed to init buffer for VL read");
    return false;
  }
  // reset the VL state and receive buffer now
  VLReceiver_resetVLState();
  return true;
}

// The recv PCI handler is called every time the analog value passes the
// 'threshold' which is dynamically established to be about half way between
// 'light' and 'dark' in the current environment. So in other words, it is
// turn on/off the legacy receiver
void VLReceiver_setLegacyReceiver(bool legacy)
{
  s_legacy = legacy;
}

// called for every 'on' or 'off' blink
void VLReceiver_recvPCIHandler()
{
  // toggle the tracked pin state no matter what
  s_pinState = (uint8_t)!s_pinState;
  // grab current time
  uint32_t now = Time_microseconds();
  // calc time difference between previous change and now
  uint32_t diff = (uint32_t)(now - s_prevTime);
  s_prevTime = now;
  // filter out values much too large from being truncated
  if (diff > UINT16_MAX) {
    return;
  }
  // handle the blink duration and process it into data
  if (s_legacy) {
    VLReceiver_handleVLTimingLegacy((uint16_t)diff);
  } else {
    VLReceiver_handleVLTiming((uint16_t)diff);
  }
}

// state machine that can be fed VL timings to parse them and interpret the intervals
void VLReceiver_handleVLTimingLegacy(uint16_t diff)
{
  switch (s_recvState) {
  case WAITING_HEADER_MARK:
    // just look for a big blink timing
    if (diff >= VL_HEADER_SPACE_MIN_LEGACY && diff <= VL_HEADER_MARK_MAX_LEGACY) {
      // go straight to the header space
      s_recvState = WAITING_HEADER_SPACE;
    }
    break;
  case WAITING_HEADER_SPACE:
    // the header space is technically shorter but this check uses the same range
    // because being restrictive here isn't really beneficial
    if (diff >= VL_HEADER_SPACE_MIN_LEGACY && diff <= VL_HEADER_MARK_MAX_LEGACY) {
      // iterate to first data mark
      s_recvState = READING_DATA_MARK;
      // estimate the data threshold based on the length of the space
      s_vlMarkThreshold = (diff / 4);
    }
    break;
  case READING_DATA_MARK:
    // classify as 1 or 0 based on the mark threshold and write into buffer
    BitStream_write1Bit(&s_vlData, diff > s_vlMarkThreshold);
    s_recvState = READING_DATA_SPACE;
    break;
  case READING_DATA_SPACE:
    // in the legacy transmission the spaces didn't carry data
    s_recvState = READING_DATA_MARK;
    break;
  default: // ??
    DEBUG_LOGF("Bad receive state: %u", s_recvState);
    break;
  }
}

// state machine that can be fed VL timings to parse them and interpret the intervals
void VLReceiver_handleVLTiming(uint16_t diff)
{
  uint8_t bit;
  switch (s_recvState) {
  case WAITING_HEADER_MARK:
  case WAITING_HEADER_SPACE:
    // both cases are basically the same, just look for a big timing
    if (diff >= VL_HEADER_SPACE_MIN && diff <= VL_HEADER_MARK_MAX) {
      // iterate through first two states
      s_recvState = (RecvState)(s_recvState + 1);
    }
    break;
  case READING_BAUD_MARK:
    // accumulate the diff in the mark threshold for averaging later
    s_vlMarkThreshold += diff;
    s_recvState = READING_BAUD_SPACE;
    break;
  case READING_BAUD_SPACE:
    // couonter is used to count bauds till 4
    s_counter++;
    // accumulate the diff in the space threshold for averaging later
    s_vlSpaceThreshold += diff;
    // if not at 4 bauds yet keep reading baud marks
    if (s_counter < 4) {
      s_recvState = READING_BAUD_MARK;
      break;
    }
    // otherwise read all 4 bauds now proceed with processing them
    // average out the mark and space from the bauds
    s_vlMarkThreshold /= 4;
    s_vlSpaceThreshold /= 4;
    // reset counter and parity bit
    s_counter = 0;
    s_parityBit = 0;
    // advanced state to first data mark
    s_recvState = READING_DATA_MARK;
    break;
  case READING_DATA_MARK:
    // counter is now counting the marks for parity tracking
    s_counter++;
    // extract the bit of data based on the calculated mark threshold
    bit = (diff > s_vlMarkThreshold) ? 1 : 0;
    // accumulate the parity and write out the bit into the vldata
    s_parityBit = (s_parityBit ^ bit) & 1;
    BitStream_write1Bit(&s_vlData, bit);
    s_recvState = READING_DATA_SPACE;
    break;
  case READING_DATA_SPACE:
    // the spaces also transmit data in their lengths
    bit = (diff > s_vlSpaceThreshold) ? 1 : 0;
    // also accumulate this data into the parity
    s_parityBit = (s_parityBit ^ bit) & 1;
    // and then write out the bit into the vldata
    BitStream_write1Bit(&s_vlData, bit);
    // when counter is a multiple of 4 (since it only counts marks)
    if ((s_counter % 4) == 0) {
      // then go to the parity processing for the previous 8 bits of data
      s_recvState = READING_DATA_PARITY_MARK;
    } else {
      // otherwise continue reading out data bits
      s_recvState = READING_DATA_MARK;
    }
    break;
  case READING_DATA_PARITY_MARK:
    // the parity is also a bit of data using a mark
    bit = (diff > s_vlMarkThreshold) ? 1 : 0;
    // check the parity bit against the running accumulated parity
    if ((s_parityBit & 1) != bit) {
      // immediately reset the receiver if the parity doesn't match
      // could show a flash here but that would probably just make
      // receiving worse by introducing a delay
      VLReceiver_resetVLState();
      break;
    }
    // the parity space is just an empty space after the parity bit
    s_recvState = READING_DATA_PARITY_SPACE;
    break;
  case READING_DATA_PARITY_SPACE:
    // after the parity it's back to regular mark
    s_recvState = READING_DATA_MARK;
    break;
  default: // ??
    DEBUG_LOGF("Bad receive state: %u", s_recvState);
    break;
  }
}

void VLReceiver_resetVLState()
{
  s_counter = 0;
  s_vlMarkThreshold = 0;
  s_vlSpaceThreshold = 0;
  s_previousBytes = 0;
  s_parityBit = 0;
  s_recvState = WAITING_HEADER_MARK;
  // zero out the receive buffer and reset bit receiver position
  BitStream_reset(&s_vlData);
#ifdef VORTEX_EMBEDDED
  // reset the threshold to a high value so that it can be pulled down again
  s_threshold = THRESHOLD_BEGIN;
#endif
  DEBUG_LOG("VL State Reset");
}

#endif