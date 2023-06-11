#include "IRReceiver.h"

#include "../Serial/ByteStream.h"
#include "../Serial/BitStream.h"
#include "../Time/TimeControl.h"
#include "../Modes/Mode.h"
#include "../Leds/Leds.h"
#include "../Log/Log.h"

#include "IRConfig.h"

#ifdef VORTEX_ARDUINO
#include <avr/interrupt.h>
#include <avr/io.h>
#endif

BitStream IRReceiver::m_irData;
IRReceiver::RecvState IRReceiver::m_recvState = WAITING_HEADER_MARK;
uint64_t IRReceiver::m_prevTime = 0;
uint8_t IRReceiver::m_pinState = 0;
uint32_t IRReceiver::m_previousBytes = 0;

#ifdef VORTEX_ARDUINO
bool wasAboveThreshold = false;  // this will store the last known state
uint16_t threshold = 500;
#endif

bool IRReceiver::init()
{
#ifdef VORTEX_ARDUINO
  // Disable digital input buffer on the pin to save power
  PORTB.PIN1CTRL &= ~PORT_ISC_gm;
  PORTB.PIN1CTRL |= PORT_ISC_INPUT_DISABLE_gc;
#endif
  return m_irData.init(IR_RECV_BUF_SIZE);
}

void IRReceiver::cleanup()
{
}

bool IRReceiver::dataReady()
{
  // is the receiver actually receiving data?
  if (!isReceiving()) {
    return false;
  }
  uint8_t blocks = m_irData.data()[0];
  uint8_t remainder = m_irData.data()[1];
  uint32_t total = ((blocks - 1) * 32) + remainder;
  if (!total || total > MAX_DATA_TRANSFER) {
    DEBUG_LOGF("Bad IR Data size: %u", total);
    return false;
  }
  // if there are size + 2 bytes in the IRData receiver
  // then a full message is ready, the + 2 is from the
  // two bytes for blocks + remainder that are sent first
  return (m_irData.bytepos() >= (uint32_t)(total + 2));
}

// whether actively receiving
bool IRReceiver::isReceiving()
{
  // if there are at least 2 bytes in the data buffer then
  // the receiver is receiving a packet. If there is less
  // than 2 bytes then we're still waiting for the 'blocks'
  // and 'remainder' bytes which prefix a packet
  return (m_irData.bytepos() > 2);
}

// the percent of data received
uint32_t IRReceiver::percentReceived()
{
  if (!isReceiving()) {
    return 0;
  }
  uint8_t blocks = m_irData.data()[0];
  uint8_t remainder = m_irData.data()[1];
  uint32_t total = ((blocks - 1) * 32) + remainder;
  return (uint32_t)((m_irData.bytepos() * 100) / (total * 100));
}

bool IRReceiver::receiveMode(Mode *pMode)
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

bool IRReceiver::beginReceiving()
{
#ifdef VORTEX_ARDUINO
  // Set up the ADC
  // sample campacitance, VDD reference, prescaler division
  //  0x0 DIV2 CLK_PER divided by 2
  //  0x1 DIV4 CLK_PER divided by 4
  //  0x2 DIV8 CLK_PER divided by 8
  //  0x3 DIV16 CLK_PER divided by 16
  //  0x4 DIV32 CLK_PER divided by 32 > doesn't work
  //  0x5 DIV64 CLK_PER divided by 64 > doesn't work
  //  0x6 DIV128 CLK_PER divided by 128 > works
  //  0x7 DIV256 CLK_PER divided by 256 > works
  ADC0.CTRLC = ADC_SAMPCAP_bm | ADC_REFSEL_VDDREF_gc | ADC_PRESC_DIV128_gc;
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
  //   0x3 ACC8 8 results accumulated
  //   0x4 ACC16 16 results accumulated
  //   0x5 ACC32 32 results accumulated
  //   0x6 ACC64 64 results accumulated
  ADC0.CTRLB = ADC_SAMPNUM_ACC4_gc;
  // Enable Window Comparator interrupt
  ADC0.INTCTRL = ADC_WCMP_bm;
  // Enable the ADC and start continuous conversions
  ADC0.CTRLA = ADC_ENABLE_bm | ADC_FREERUN_bm;
  // start the first conversion
  ADC0.COMMAND = ADC_STCONV_bm;
#endif
  resetIRState();
  return true;
}

bool IRReceiver::endReceiving()
{
#ifdef VORTEX_ARDUINO
  // Stop conversions and disable the ADC
  ADC0.CTRLA &= ~(ADC_ENABLE_bm | ADC_FREERUN_bm);
  ADC0.INTCTRL = 0;
#endif
  resetIRState();
  return true;
}

#ifdef VORTEX_ARDUINO
ISR(ADC0_WCOMP_vect)
{
  uint16_t val = (ADC0.RES >> 2);
  bool isAboveThreshold = (val > 250);
  //Leds::setIndex(LED_0, RGBColor(val < 256 ? val : 0, (val - 256) < 256 ? val - 256 : 0, (val - 512) < 256 ? val - 512 : 0));
  if (wasAboveThreshold != isAboveThreshold) {
    IRReceiver::recvPCIHandler();
    wasAboveThreshold = isAboveThreshold;
  }
  // Clear the Window Comparator interrupt flag
  ADC0.INTFLAGS = ADC_WCMP_bm;
}
#endif

bool IRReceiver::onNewData()
{
  if (bytesReceived() == m_previousBytes) {
    return false;
  }
  m_previousBytes = bytesReceived();
  return true;
}

bool IRReceiver::read(ByteStream &data)
{
  if (!m_irData.bytepos() || m_irData.bytepos() > MAX_DATA_TRANSFER) {
    DEBUG_LOG("Nothing to read, or read too much");
    return false;
  }
  // read the size out (blocks + remainder)
  uint8_t blocks = m_irData.data()[0];
  uint8_t remainder = m_irData.data()[1];
  // calculate size from blocks + remainder
  uint32_t size = ((blocks - 1) * 32) + remainder;
  if (!size || size > MAX_DATA_TRANSFER) {
    DEBUG_LOGF("Bad IR Data size: %u", size);
    return false;
  }
  // the actual data starts 2 bytes later because of the size byte
  const uint8_t *actualData = m_irData.data() + 2;
  if (!data.rawInit(actualData, size)) {
    DEBUG_LOG("Failed to init buffer for IR read");
    return false;
  }
  // reset the IR state and receive buffer now
  resetIRState();
  return true;
}

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
  // handle the bliank duration and process it
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
  switch (m_recvState) {
  case WAITING_HEADER_MARK: // initial state
    if (diff >= HEADER_SPACE_MIN && diff <= HEADER_MARK_MAX) {
      m_recvState = WAITING_HEADER_SPACE;
    } else {
      DEBUG_LOGF("Bad header mark %u, resetting...", diff);
      resetIRState();
    }
    break;
  case WAITING_HEADER_SPACE:
    if (diff >= HEADER_SPACE_MIN && diff <= HEADER_MARK_MAX) {
      m_recvState = READING_DATA_MARK;
    } else {
      DEBUG_LOGF("Bad header space %u, resetting...", diff);
      resetIRState();
    }
    break;
  case READING_DATA_MARK:
    // classify mark/space based on the timing and write into buffer
	m_irData.write1Bit((diff > (IR_TIMING * 2)) ? 1 : 0);
    m_recvState = READING_DATA_SPACE;
    break;
  case READING_DATA_SPACE:
    // the space could be just a regular space, or a gap in between blocks
    m_recvState = READING_DATA_MARK;
    break;
  default: // ??
    DEBUG_LOGF("Bad receive state: %u", m_recvState);
    break;
  }
}

void IRReceiver::resetIRState()
{
  m_previousBytes = 0;
  m_recvState = WAITING_HEADER_MARK;
  // zero out the receive buffer and reset bit receiver position
  m_irData.reset();
  DEBUG_LOG("IR State Reset");
}
