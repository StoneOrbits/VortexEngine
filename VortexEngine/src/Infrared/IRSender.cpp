#include "IRSender.h"

#include "../Modes/Mode.h"
#include "../Log/Log.h"

#include "IRConfig.h"

#include <Arduino.h>

// the serial buffer for the data
ByteStream IRSender::m_serialBuf;
// a bit walker for the serial data
BitStream IRSender::m_bitStream;
bool IRSender::m_isSending = false;
uint64_t IRSender::m_startTime = 0;
// some runtime meta info
uint32_t IRSender::m_size = 0;
// the number of blocks that will be sent
uint8_t IRSender::m_numBlocks = 0;
// the amount in the final block
uint8_t IRSender::m_remainder = 0;
// configuration options for the sender
uint32_t IRSender::m_blockSize = 0;
// write total
uint32_t IRSender::m_writeCounter = 0;

#ifndef TEST_FRAMEWORK
// Timer used for PWM, is initialized in initpwm()
Tcc *IR_TCCx;
#endif

bool IRSender::init()
{
  // initialize the IR device
  initPWM();
  pinMode(IR_SEND_PWM_PIN, OUTPUT);
  digitalWrite(IR_SEND_PWM_PIN, LOW); // When not sending PWM, we want it low
  return true;
}

void IRSender::cleanup()
{
}

bool IRSender::loadMode(const Mode *targetMode)
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
  m_remainder = m_size % DEFAULT_IR_BLOCK_SIZE;
  return true;
}

void IRSender::send()
{
  if (!m_isSending) {
    initSend();
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

// shamelessly stolen from IRLib2, thanks
void IRSender::initPWM()
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

void IRSender::startPWM()
{
#ifndef TEST_FRAMEWORK
  // start the PWM
  IR_TCCx->CTRLA.reg |= TCC_CTRLA_ENABLE;
  while (IR_TCCx->SYNCBUSY.bit.ENABLE);
#endif
}

void IRSender::stopPWM()
{
#ifndef TEST_FRAMEWORK
  // stop the PWM
  IR_TCCx->CTRLA.reg &= ~TCC_CTRLA_ENABLE;
  while (IR_TCCx->SYNCBUSY.bit.ENABLE);
#endif
}

void IRSender::initSend()
{
  m_isSending = true;
  uint64_t startTime = micros();
  DEBUG_LOGF("[%zu] Beginning send size %u (blocks: %u remainder: %u blocksize: %u)", 
    startTime, m_size, m_numBlocks, m_remainder, m_blockSize);
  // init sender before writing, is this necessary here? I think so
  initPWM();
  // wakeup the other receiver with a very quick mark/space
  mark(50);
  space(100);
  // now send the header
  mark(HEADER_MARK);
  space(HEADER_SPACE);
  // reset writeCounter
  m_writeCounter = 0;
  // write the number of blocks being sent, most likely just 1
  write8(m_numBlocks);
  // now write the number of bytes in the last block
  write8(m_remainder);
}

void IRSender::write8(uint8_t data)
{
  // Sends from left to right, MSB first
  for (int b = 0; b < 8; b++) {
    // grab the bit of data at the index
    uint32_t bit = (data >> (7 - b)) & 1;
    // send 3x timing size for 1s and 1x timing for 0
    mark(IR_TIMING + (IR_TIMING * (2 * bit)));
    // send 1x timing size for space
    space(IR_TIMING);
    DEBUG_LOGF(" bit: %u", bit);
  }
  DEBUG_LOGF("Sent byte[%u]: 0x%x", m_writeCounter, data);
  m_writeCounter++;
 }

void IRSender::mark(uint16_t time)
{
#ifdef TEST_FRAMEWORK
  // send mark timing over socket
  test_ir_mark(time);
#else
  Infrared::startPWM();
  delayMicroseconds(time);
#endif
}

void IRSender::space(uint16_t time)
{
#ifdef TEST_FRAMEWORK
  // send space timing over socket
  test_ir_space(time);
#else
  Infrared::stopPWM();
  delayMicroseconds(time);
#endif
}


