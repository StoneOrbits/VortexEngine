#include "VLReceiver.h"
#include "IRConfig.h"

#if VL_ENABLE_RECEIVER == 1

#include "../Serial/ByteStream.h"
#include "../Serial/BitStream.h"
#include "../Time/TimeControl.h"
#include "../Modes/Mode.h"
#include "../Leds/Leds.h"
#include "../Log/Log.h"

BitStream VLReceiver::m_vlData;
VLReceiver::RecvState VLReceiver::m_recvState = WAITING_HEADER_MARK;
uint32_t VLReceiver::m_prevTime = 0;
uint8_t VLReceiver::m_pinState = 0;
uint16_t VLReceiver::m_previousBytes = 0;
// the determined time based on sync
uint16_t VLReceiver::m_vlMarkThreshold = 0;
uint16_t VLReceiver::m_vlSpaceThreshold = 0;
// counter is reused for two purposes
uint8_t VLReceiver::m_counter = 0;
// parity is for each byte of data received
uint8_t VLReceiver::m_parityBit = 0;
// legacy is for old receiving method (sucks)
bool VLReceiver::m_legacy = false;

#ifdef VORTEX_EMBEDDED
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/adc.h"
#include "esp_adc_cal.h"
#include "esp_log.h"
#include "esp_timer.h"

#include "../Serial/Serial.h"

// ADC and timer configuration
#define ADC_CHANNEL ADC1_CHANNEL_1 // Update this based on the actual ADC channel used
#define ADC_ATTEN ADC_ATTEN_DB_0
#define ADC_WIDTH ADC_WIDTH_BIT_12
#define TIMER_INTERVAL_MICRO_SEC 1000 // Check every 10ms, adjust as needed for your application

// Timer handle as a global variable for control in beginReceiving and endReceiving
esp_timer_handle_t periodic_timer = nullptr;
esp_adc_cal_characteristics_t adc_chars;

#define MIN_THRESHOLD   200
#define BASE_OFFSET     100
#define THRESHOLD_BEGIN (MIN_THRESHOLD + BASE_OFFSET)
// the threshold needs to start high then it will be automatically pulled down
uint32_t threshold = THRESHOLD_BEGIN;
void VLReceiver::adcCheckTimerCallback(void *arg)
{
  static bool wasAboveThreshold = false;
  uint32_t raw = adc1_get_raw(ADC_CHANNEL);
  uint32_t val = esp_adc_cal_raw_to_voltage(raw, &adc_chars);

  if (val > MIN_THRESHOLD && val < (threshold + BASE_OFFSET)) {
    threshold = val + BASE_OFFSET;
  }
  bool isAboveThreshold = (val > threshold);
  if (wasAboveThreshold != isAboveThreshold) {
    wasAboveThreshold = isAboveThreshold;
    VLReceiver::recvPCIHandler();
  }
}
#endif

bool VLReceiver::init()
{
#ifdef VORTEX_EMBEDDED
  // Initialize ADC for GPIO1 (or appropriate pin connected to your light sensor)
  adc1_config_width(ADC_WIDTH);
  adc1_config_channel_atten(ADC_CHANNEL, ADC_ATTEN);
  memset(&adc_chars, 0, sizeof(adc_chars));
  esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN, ADC_WIDTH, 0, &adc_chars);
#endif
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
#ifdef VORTEX_EMBEDDED
  if (periodic_timer) {
    DEBUG_LOG("VL Reception already running.");
    return false; // Timer is already running
  }
  // Initialize timer for periodic ADC checks
  const esp_timer_create_args_t periodic_timer_args = {
      .callback = &VLReceiver::adcCheckTimerCallback,
      .name = "adc_check_timer",
  };
  ESP_ERROR_CHECK(esp_timer_create(&periodic_timer_args, &periodic_timer));
  ESP_ERROR_CHECK(esp_timer_start_periodic(periodic_timer, TIMER_INTERVAL_MICRO_SEC));
#endif
  resetVLState();
  return true;
}

bool VLReceiver::endReceiving()
{
#ifdef VORTEX_EMBEDDED
  if (periodic_timer == nullptr) {
    DEBUG_LOG("VL Reception was not running.");
    return false; // Timer was not running
  }
  // Stop and delete the timer
  ESP_ERROR_CHECK(esp_timer_stop(periodic_timer));
  ESP_ERROR_CHECK(esp_timer_delete(periodic_timer));
  periodic_timer = nullptr;
  DEBUG_LOG("VL Reception stopped.");
#endif
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
  uint32_t now = Time::microseconds();
  // calc time difference between previous change and now
  uint32_t diff = (uint32_t)(now - m_prevTime);
  m_prevTime = now;
  // filter out values much too large from being truncated
  if (diff > UINT16_MAX) {
    return;
  }
  // handle the blink duration and process it into data
  if (m_legacy) {
    handleVLTimingLegacy((uint16_t)diff);
  } else {
    handleVLTiming((uint16_t)diff);
  }
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
