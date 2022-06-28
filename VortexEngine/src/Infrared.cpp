#include "Infrared.h"

#include "SerialBuffer.h"
#include "TimeControl.h"
#include "BitStream.h"
#include "Buttons.h"
#include "Leds.h"
#include "Log.h"

#include <Arduino.h>

using namespace std;

// the max number of DWORDs that will transfer
#define MAX_DWORDS_TRANSFER 1024
#define MAX_DATA_TRANSFER (MAX_DWORDS_TRANSFER * sizeof(uint32_t))

// the IR receiver buffer size in dwords
#define IR_RECV_BUF_SIZE MAX_DATA_TRANSFER

//#define IR_TIMING 564
#define IR_TIMING 1564
#define IR_TIMING_MIN ((uint32_t)(IR_TIMING * 0.1))

#define HEADER_MARK (IR_TIMING * 16)
#define HEADER_SPACE (IR_TIMING * 8)

#define HEADER_MARK_MIN ((uint32_t)(HEADER_MARK * 0.85))
#define HEADER_SPACE_MIN ((uint32_t)(HEADER_SPACE * 0.85))

#define HEADER_MARK_MAX ((uint32_t)(HEADER_MARK * 1.15))
#define HEADER_SPACE_MAX ((uint32_t)(HEADER_SPACE * 1.15))

#define IR_SEND_PWM_PIN 0
#define RECEIVER_PIN 2

#ifndef TEST_FRAMEWORK
// Timer used for PWM, is initialized in initpwm()
Tcc *IR_TCCx;
#endif

BitStream Infrared::m_irData;
Infrared::RecvState Infrared::m_recvState = WAITING_HEADER_MARK;
uint64_t Infrared::m_prevTime = 0;
uint8_t Infrared::m_pinState = HIGH;

Infrared::Infrared()
{
}

bool Infrared::init()
{
  // initialize the sender thing
  initpwm();
  pinMode(RECEIVER_PIN, INPUT_PULLUP);
  pinMode(IR_SEND_PWM_PIN, OUTPUT);
  digitalWrite(IR_SEND_PWM_PIN, LOW); // When not sending PWM, we want it low
  m_irData.init(IR_RECV_BUF_SIZE);
#ifdef TEST_FRAMEWORK
  // the test framework will feed timings directly to handleIRTiming
  // instead of calling the recvPCIHandler in intervals which might
  // be unreliable or problematic
  installIRCallback(handleIRTiming);
#endif
  return true;
}

void Infrared::cleanup()
{
}

bool Infrared::dataReady()
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
  return (m_irData.bytepos() == (uint32_t)(total + 1));
}

bool Infrared::read(SerialBuffer &data)
{
  if (!m_irData.bytepos() || m_irData.bytepos() > MAX_DATA_TRANSFER) {
    // nothing to read, or somehow read way too much
    return false;
  }
  // the first byte received should be the size of data to come
  uint8_t size = *m_irData.data();
  DEBUG_LOGF("Data size: %u", size);
  // validate the size
  if (size > MAX_DATA_TRANSFER) {
    DEBUG_LOGF("Received bad data size: %u", size);
    return false;
  }
  // the actual data starts 1 byte later because of the size byte
  const uint8_t *actualData = m_irData.data() + 1;
  if (!data.rawInit(actualData, size)) {
    DEBUG_LOG("Failed to init buffer for IR read");
    return false;
  }
  for (uint32_t i = 0; i < size; ++i) {
    DEBUG_LOGF("Read: 0x%x", actualData[i]);
  }
  // reset the IR state and receive buffer
  resetIRState();
  return true;
}

bool Infrared::write(SerialBuffer &data)
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
  // init sender before writing, is this necessary here? I think so
  initpwm();
  // wakeup the other receiver with a very quick mark/space
  mark(50);
  space(100);
  // now send the header
  mark(HEADER_MARK);
  space(HEADER_SPACE);
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
    uint32_t blocksize = (num_blocks == 1) ? remainder : 32;
    // iterate each byte in the block and write it
    for (uint32_t i = 0; i < blocksize; ++i) {
      write8(*buf_ptr);
      buf_ptr++;
    }
    // delay if there is more blocks
    if (block < (num_blocks - 1)) {
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
    if (block < (num_blocks - 1)) {
      DEBUG_LOG("Block");
    }
  }
  return true;
}

bool Infrared::beginReceiving()
{
  attachInterrupt(digitalPinToInterrupt(RECEIVER_PIN), Infrared::recvPCIHandler, CHANGE);
  resetIRState();
  return true;
}

bool Infrared::endReceiving()
{
  detachInterrupt(digitalPinToInterrupt(RECEIVER_PIN));
  resetIRState();
  return true;
}

// ===================
//  sending functions

// shamelessly stolen from IRLib2, thanks
void Infrared::initpwm()
{
#ifndef TEST_FRAMEWORK
  // just in case
  pinMode(IR_SEND_PWM_PIN, OUTPUT);
  digitalWrite(IR_SEND_PWM_PIN, LOW); // When not sending PWM, we want it low
  uint8_t port = g_APinDescription[IR_SEND_PWM_PIN].ulPort; // 0
  uint8_t pin = g_APinDescription[IR_SEND_PWM_PIN].ulPin;   // 8
  uint8_t IR_mapIndex = (port * 32) + pin; // 8
  ETCChannel IR_TCC_Channel = TCC0_CH0;
  int8_t IR_PER_EorF = PORT_PMUX_PMUXE_E;
  //println();Serial.print("Port:"); Serial.print(port,DEC); Serial.print(" Pin:"); Serial.println(pin,DEC);
  // Enable the port multiplexer for the PWM channel on pin   
  PORT->Group[port].PINCFG[pin].bit.PMUXEN = 1;
  
  // Connect the TCC timer to the port outputs - port pins are paired odd PMUXO and even PMUXEII 
  // F & E peripherals specify the timers: TCC0, TCC1 and TCC2
  PORT->Group[port].PMUX[pin >> 1].reg |= IR_PER_EorF; 
  
//  pinPeripheral (IR_SEND_PWM_PIN,PIO_TIMER_ALT);
  // Feed GCLK0 to TCC0 and TCC1
  REG_GCLK_CLKCTRL = GCLK_CLKCTRL_CLKEN |       // Enable GCLK0 to TCC0 and TCC1
                     GCLK_CLKCTRL_GEN_GCLK0 |   // Select GCLK0
                     GCLK_CLKCTRL_ID_TCC0_TCC1; // Feed GCLK0 to TCC0 and TCC1
  while (GCLK->STATUS.bit.SYNCBUSY);            // Wait for synchronization

  // Normal (single slope) PWM operation: timers countinuously count up to PER 
  // register value and then is reset to 0
  IR_TCCx = (Tcc*) GetTC(IR_TCC_Channel);
  IR_TCCx->WAVE.reg |= TCC_WAVE_WAVEGEN_NPWM;   // Setup single slope PWM on TCCx
  while (IR_TCCx->SYNCBUSY.bit.WAVE);           // Wait for synchronization

  // Each timer counts up to a maximum or TOP value set by the PER register,
  // this determines the frequency of the PWM operation.
  uint32_t cc = F_CPU/(38*1000) - 1;
  IR_TCCx->PER.reg = cc;      // Set the frequency of the PWM on IR_TCCx
  while(IR_TCCx->SYNCBUSY.bit.PER);

  // The CCx register value corresponds to the pulsewidth in microseconds (us) 
  // Set the duty cycle of the PWM on TCC0 to 33%
  IR_TCCx->CC[GetTCChannelNumber(IR_TCC_Channel)].reg = cc/3;      
  while (IR_TCCx->SYNCBUSY.reg & TCC_SYNCBUSY_MASK);
  //while(IR_TCCx->SYNCBUSY.bit.CC3);

  // Enable IR_TCCx timer but do not turn on PWM yet. Will turn it on later.
  IR_TCCx->CTRLA.reg |= TCC_CTRLA_PRESCALER_DIV1;     // Divide GCLK0 by 1
  while (IR_TCCx->SYNCBUSY.bit.ENABLE);
  IR_TCCx->CTRLA.reg &= ~TCC_CTRLA_ENABLE;            //initially off will turn on later
  while (IR_TCCx->SYNCBUSY.bit.ENABLE);
#endif
}

void Infrared::write8(uint8_t data)
{
  // Sends from left to right, MSB first
  for (int b = 0; b < 8; b++) {
    // grab the bit of data at the index
    uint32_t bit = (data >> (7 - b)) & 1;
    // send 3x timing size for 1s and 1x timing for 0
    mark(IR_TIMING + (IR_TIMING * (2 * bit)));
    // send 1x timing size for space
    space(IR_TIMING);
  }
}

void Infrared::mark(uint16_t time)
{
#ifdef TEST_FRAMEWORK
  // send mark timing over socket
  test_ir_mark(time);
#else
  // start the PWM
  IR_TCCx->CTRLA.reg |= TCC_CTRLA_ENABLE;
  while (IR_TCCx->SYNCBUSY.bit.ENABLE);
  delayMicroseconds(time);
#endif
}

void Infrared::space(uint16_t time)
{
#ifdef TEST_FRAMEWORK
  // send space timing over socket
  test_ir_space(time);
#else
  // stop the PWM
  IR_TCCx->CTRLA.reg &= ~TCC_CTRLA_ENABLE;
  while (IR_TCCx->SYNCBUSY.bit.ENABLE);
  delayMicroseconds(time);
#endif
}

// ===================
// receiver functions

void Infrared::recvPCIHandler()
{
  // toggle the tracked pin state no matter what
  m_pinState = (uint8_t)!m_pinState;
  // grab current time
  uint32_t now = micros();
  // check previous time for validity
  if (!m_prevTime || m_prevTime > now) {
    m_prevTime = now;
    resetIRState();
    return;
  }
  // calc time difference between previous change and now
  uint32_t diff = (uint32_t)(now - m_prevTime);
  // and update the previous changetime for next loop
  m_prevTime = now;
  // handle the difference in time
  handleIRTiming(diff);
}

void Infrared::handleIRTiming(uint32_t diff)
{
  // if the diff is too long or too short then it's not useful
  if (diff > HEADER_MARK_MAX || diff < IR_TIMING_MIN) {
    DEBUG_LOGF("bad delay: %u", diff);
    resetIRState();
    return;
  }
  switch (m_recvState) {
  case WAITING_HEADER_MARK:
    if (diff >= HEADER_MARK_MIN && diff <= HEADER_MARK_MAX) {
      m_recvState = WAITING_HEADER_SPACE;
    } else {
      resetIRState();
    }
    break;
  case WAITING_HEADER_SPACE:
    if (diff >= HEADER_SPACE_MIN && diff <= HEADER_SPACE_MAX) {
      m_recvState = READING_DATA_MARK;
    } else {
      resetIRState();
    }
    break;
  case READING_DATA_MARK:
    // classify mark/space based on the timing and write into buffer
    m_irData.write1Bit((diff > (IR_TIMING * 2)) ? 1 : 0);
    if (m_irData.bitpos() > 0 && ((m_irData.bitpos() + 1) % 32) == 0) {
      DEBUG_LOGF("Read: 0x%x", m_irData.dwData()[m_irData.dwordpos()]);
    }
    if (((m_irData.bytepos() + 1) % 32) == 0) {
      m_recvState = WAITING_HEADER_SPACE;
    } else {
      m_recvState = READING_DATA_SPACE;
    }
    break;
  case READING_DATA_SPACE:
    // skip spaces
    m_recvState = READING_DATA_MARK;
    break;
  default: // ??
    DEBUG_LOGF("Bad receive state: %u", m_recvState);
    break;
  }
}

void Infrared::resetIRState()
{
  m_recvState = WAITING_HEADER_MARK;
  // zero out the receive buffer and reset bit receiver position
  m_irData.reset();
}
