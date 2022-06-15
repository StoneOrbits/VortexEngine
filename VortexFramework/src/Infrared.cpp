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
#define MAX_DWORDS_TRANSFER 9
#define MAX_DATA_TRANSFER (MAX_DWORDS_TRANSFER * sizeof(uint32_t))

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
  pinMode(RECEIVER_PIN, INPUT_PULLUP);
  pinMode(IR_SEND_PWM_PIN, OUTPUT);
  digitalWrite(IR_SEND_PWM_PIN, LOW); // When not sending PWM, we want it low
  m_irData.init(IR_RECV_BUF_SIZE);
#ifdef TEST_FRAMEWORK
  installIRCallback(handleIRTiming);
#endif
  return true;
}

void Infrared::cleanup()
{
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
  // inintialize the serial buffer for the new data size
  if (!data.init(size)) {
    DEBUG_LOG("Failed to init buffer for IR read");
    return false;
  }
  // the actual data starts 1 byte later because of the size byte
  const uint8_t *actualData = m_irData.data() + 1;
  for (uint32_t i = 0; i < size; ++i) {
    DEBUG_LOGF("Read: 0x%x", actualData[i]);
  }
  // copy in the actual data from the serial buffer
  memcpy(data.rawData(), actualData, size);
  // reset the IR state and receive buffer
  resetIRState();
  return true;
}

bool Infrared::write(SerialBuffer &data)
{
  // initialize the thing
  initpwm();
  uint8_t *buf = (uint8_t *)data.rawData();
  uint32_t size = data.rawSize();
  // access the data in dwords
  if (size > MAX_DATA_TRANSFER) {
    DEBUG_LOGF("Cannot transfer that much data: %u bytes", data.rawSize());
    return false;
  }
  // create a bitstream to walk the bits of the data
  BitStream sendStream(buf, size);
  // wakeup the other receiver with a quick mark/space
  mark(50);
  space(100);
  // now send the header
  mark(HEADER_MARK);
  space(HEADER_SPACE);
  // write the number of bytes being sent, should't exceed MAX_DATA_TRANSFER
  write8((uint8_t)size);
  // now iterate the bytes of data
  for (uint32_t i = 0; i < size; ++i) {
    write8(buf[i]);
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

void Infrared::delayus(uint16_t time)
{
  if (!time) { 
    return;
  }
  if (time <= 16000) {
    delayMicroseconds(time);
    return;
  }
  delayMicroseconds(time % 1000);
  delay(time / 1000);
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
  DEBUG_LOGF("Wrote: 0x%x", data);
}

void Infrared::write32(uint32_t data)
{
  // the other end will pack the data with a bitstream
  write8(data & 0xFF);
  write8((data >> 8) & 0xFF);
  write8((data >> 16) & 0xFF);
  write8((data >> 24) & 0xFF);
}

uint32_t Infrared::mark(uint16_t time)
{
#ifdef TEST_FRAMEWORK
  // send mark timing over socket
  ir_mark(time);
#else
  // start the PWM
  IR_TCCx->CTRLA.reg |= TCC_CTRLA_ENABLE;
  while (IR_TCCx->SYNCBUSY.bit.ENABLE);
  delayus(time);
#endif
  return time;
}

uint32_t Infrared::space(uint16_t time)
{
#ifdef TEST_FRAMEWORK
  // send space timing over socket
  ir_space(time);
#else
  // stop the PWM
  IR_TCCx->CTRLA.reg &= ~TCC_CTRLA_ENABLE;
  while (IR_TCCx->SYNCBUSY.bit.ENABLE);
  delayus(time);
#endif
  return time;
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
    if (diff < HEADER_MARK_MIN) {
      resetIRState();
      return;
    }
    m_recvState = WAITING_HEADER_SPACE;
    return;
  case WAITING_HEADER_SPACE:
    if (diff < HEADER_SPACE_MIN) {
      // failed to read space
      resetIRState();
      return;
    }
    // success
    DEBUG_LOG("Matched header mark and space, receiving data...");
    m_recvState = READING_DATA_MARK;
    return;
  case READING_DATA_MARK:
    // classify mark/space based on the timing and write into buffer
    // serially one bit at a time
    m_irData.write1Bit((diff > (IR_TIMING * 2)) ? 1 : 0);
    m_recvState = READING_DATA_SPACE;
    break;
  case READING_DATA_SPACE:
    // skip spaces
    m_recvState = READING_DATA_MARK;
    return;
  default: // ??
    DEBUG_LOGF("Bad receive state: %u", m_recvState);
    return;
  }
}

void Infrared::resetIRState()
{
  m_recvState = WAITING_HEADER_MARK;
  // zero out the receive buffer and reset bit receiver position
  m_irData.reset();
}
