#include "VLSender.h"

#include "../Time/TimeControl.h"
#include "../Modes/Mode.h"
#include "../Log/Log.h"

#include "VLConfig.h"

#include <Arduino.h>

// the serial buffer for the data
ByteStream VLSender::m_serialBuf;
// a bit walker for the serial data
BitStream VLSender::m_bitStream;
// whether actively sending
bool VLSender::m_isSending = false;
// the time of the last sent chunk
uint64_t VLSender::m_lastSendTime = 0;
// some runtime meta info
uint32_t VLSender::m_size = 0;
// the number of blocks that will be sent
uint8_t VLSender::m_numBlocks = 0;
// the amount in the final block
uint8_t VLSender::m_remainder = 0;
// configuration options for the sender
uint32_t VLSender::m_blockSize = 0;
// write total
uint32_t VLSender::m_writeCounter = 0;

#if defined(VORTEX_ARDUINO) && VL_ENABLE == 1
// Timer used for PWM, is initialized in initpwm()
Tcc *VL_TCCx;
#endif

bool VLSender::init()
{
  // initialize the VL device
  initPWM();
  pinMode(VL_SEND_PWM_PIN, OUTPUT);
  digitalWrite(VL_SEND_PWM_PIN, LOW); // When not sending PWM, we want it low
  return true;
}

void VLSender::cleanup()
{
}

bool VLSender::loadMode(const Mode *targetMode)
{
  m_serialBuf.clear();
  // save the target mode to it's savefile buffer format
  if (!targetMode->saveToBuffer(m_serialBuf)) {
    DEBUG_LOG("Failed to save mode to buffer");
    return false;
  }
  // ensure the data isn't too big
  if (m_serialBuf.rawSize() > VL_MAX_DATA_TRANSFER) {
    DEBUG_LOGF("Cannot transfer that much data: %u bytes", m_serialBuf.rawSize());
    return false;
  }
  // point the bitstream at the serial buffer
  m_bitStream.init((uint8_t *)m_serialBuf.rawData(), m_serialBuf.rawSize());
  // the size of the packet
  m_size = m_serialBuf.rawSize();
  // the number of blocks that will be sent (possibly just 1 with less than 32 bytes)
  m_numBlocks = (m_size + (VL_DEFAULT_BLOCK_SIZE - 1)) / VL_DEFAULT_BLOCK_SIZE;
  // the amount in the final block (possibly the only block)
  m_remainder = m_size % (VL_DEFAULT_BLOCK_SIZE + 0);
  DEBUG_LOGF("Num blocks: %u", m_numBlocks);
  DEBUG_LOGF("Remainder: %u", m_remainder);
  DEBUG_LOGF("Size: %u", m_size);
  // reset write counter
  m_writeCounter = 0;
  return true;
}

bool VLSender::send()
{
  // must have at least one block to send
  if (m_numBlocks < 1) {
    m_isSending = false;
    return m_isSending;
  }
  if (!m_isSending) {
    beginSend();
  }
  if (m_lastSendTime > 0 && m_lastSendTime + VL_DEFAULT_BLOCK_SPACING > Time::getCurtime()) {
    // don't send yet
    return m_isSending;
  }
  // blocksize is default unless it's the last block, then it's the remainder
  uint32_t blocksize = (m_numBlocks == 1) ? m_remainder : VL_DEFAULT_BLOCK_SIZE;
  DEBUG_LOGF("VLSender Sending block #%u", m_numBlocks);
  // pointer to the data at the correct location
  const uint8_t *buf_ptr = m_bitStream.data() + m_writeCounter;
  // iterate each byte in the block and write it
  for (uint32_t i = 0; i < blocksize; ++i) {
    // write the byte
    sendByte(*buf_ptr);
    // increment to next byte in buffer
    buf_ptr++;
    // record the written byte
    m_writeCounter++;
  }
  // wrote one block
  m_numBlocks--;
  // still sending if we have more blocks
  m_isSending = (m_numBlocks > 0);
  // the curtime
  m_lastSendTime = Time::getCurtime();
  // return whether still sending
  return m_isSending;
}

void VLSender::beginSend()
{
  m_isSending = true;
  DEBUG_LOGF("[%zu] Beginning send size %u (blocks: %u remainder: %u blocksize: %u)",
    micros(), m_size, m_numBlocks, m_remainder, m_blockSize);
  // init sender before writing, is this necessary here? I think so
  initPWM();
  // wakeup the other receiver with a very quick mark/space
  sendMark(50);
  sendSpace(100);
  // now send the header
  sendMark(VL_HEADER_MARK);
  sendSpace(VL_HEADER_SPACE);
  // reset writeCounter
  m_writeCounter = 0;
  // write the number of blocks being sent, most likely just 1
  sendByte(m_numBlocks);
  // now write the number of bytes in the last block
  sendByte(m_remainder);
}

void VLSender::sendByte(uint8_t data)
{
  // Sends from left to right, MSB first
  for (int b = 0; b < 8; b++) {
    // grab the bit of data at the indexspace
    uint32_t bit = (data >> (7 - b)) & 1;
    // send 3x timing size for 1s and 1x timing for 0
    sendMark(VL_TIMING + (VL_TIMING * (2 * bit)));
    // send 1x timing size for space
    sendSpace(VL_TIMING);
  }
  DEBUG_LOGF("Sent byte[%u]: 0x%x", m_writeCounter, data);
 }

void VLSender::sendMark(uint16_t time)
{
#ifdef VORTEX_LIB
  // send mark timing over socket
  send_ir(true, time);
#else
  startPWM();
  delayMicroseconds(time);
#endif
}

void VLSender::sendSpace(uint16_t time)
{
#ifdef VORTEX_LIB
  // send space timing over socket
  send_ir(false, time);
#else
  stopPWM();
  delayMicroseconds(time);
#endif
}

// shamelessly stolen from VLLib2, thanks
void VLSender::initPWM()
{
#if defined(VORTEX_ARDUINO) && VL_ENABLE == 1
  // just in case
  pinMode(VL_SEND_PWM_PIN, OUTPUT);
  digitalWrite(VL_SEND_PWM_PIN, LOW); // When not sending PWM, we want it low
  uint8_t port = g_APinDescription[VL_SEND_PWM_PIN].ulPort; // 0
  uint8_t pin = g_APinDescription[VL_SEND_PWM_PIN].ulPin;   // 8
  uint8_t VL_mapIndex = (port * 32) + pin; // 8
  ETCChannel VL_TCC_Channel = TCC0_CH0;
  int8_t VL_PER_EorF = PORT_PMUX_PMUXE_E;
  //println();Serial.print("Port:"); Serial.print(port,DEC); Serial.print(" Pin:"); Serial.println(pin,DEC);
  // Enable the port multiplexer for the PWM channel on pin
  PORT->Group[port].PINCFG[pin].bit.PMUXEN = 1;

  // Connect the TCC timer to the port outputs - port pins are paired odd PMUXO and even PMUXEII
  // F & E peripherals specify the timers: TCC0, TCC1 and TCC2
  PORT->Group[port].PMUX[pin >> 1].reg |= VL_PER_EorF;

//  pinPeripheral (VL_SEND_PWM_PIN,PIO_TIMER_ALT);
  // Feed GCLK0 to TCC0 and TCC1
  REG_GCLK_CLKCTRL = GCLK_CLKCTRL_CLKEN |       // Enable GCLK0 to TCC0 and TCC1
                     GCLK_CLKCTRL_GEN_GCLK0 |   // Select GCLK0
                     GCLK_CLKCTRL_ID_TCC0_TCC1; // Feed GCLK0 to TCC0 and TCC1
  while (GCLK->STATUS.bit.SYNCBUSY);            // Wait for synchronization

  // Normal (single slope) PWM operation: timers countinuously count up to PER
  // register value and then is reset to 0
  VL_TCCx = (Tcc*) GetTC(VL_TCC_Channel);
  VL_TCCx->WAVE.reg |= TCC_WAVE_WAVEGEN_NPWM;   // Setup single slope PWM on TCCx
  while (VL_TCCx->SYNCBUSY.bit.WAVE);           // Wait for synchronization

  // Each timer counts up to a maximum or TOP value set by the PER register,
  // this determines the frequency of the PWM operation.
  uint32_t cc = F_CPU/(38*1000) - 1;
  VL_TCCx->PER.reg = cc;      // Set the frequency of the PWM on VL_TCCx
  while(VL_TCCx->SYNCBUSY.bit.PER);

  // The CCx register value corresponds to the pulsewidth in microseconds (us)
  // Set the duty cycle of the PWM on TCC0 to 33%
  VL_TCCx->CC[GetTCChannelNumber(VL_TCC_Channel)].reg = cc/3;
  while (VL_TCCx->SYNCBUSY.reg & TCC_SYNCBUSY_MASK);
  //while(VL_TCCx->SYNCBUSY.bit.CC3);

  // Enable VL_TCCx timer but do not turn on PWM yet. Will turn it on later.
  VL_TCCx->CTRLA.reg |= TCC_CTRLA_PRESCALER_DIV1;     // Divide GCLK0 by 1
  while (VL_TCCx->SYNCBUSY.bit.ENABLE);
  VL_TCCx->CTRLA.reg &= ~TCC_CTRLA_ENABLE;            //initially off will turn on later
  while (VL_TCCx->SYNCBUSY.bit.ENABLE);
#endif
}

void VLSender::startPWM()
{
#if defined(VORTEX_ARDUINO) && VL_ENABLE == 1
  // start the PWM
  VL_TCCx->CTRLA.reg |= TCC_CTRLA_ENABLE;
  while (VL_TCCx->SYNCBUSY.bit.ENABLE);
#endif
}

void VLSender::stopPWM()
{
#if defined(VORTEX_ARDUINO) && VL_ENABLE == 1
  // stop the PWM
  VL_TCCx->CTRLA.reg &= ~TCC_CTRLA_ENABLE;
  while (VL_TCCx->SYNCBUSY.bit.ENABLE);
#endif
}
