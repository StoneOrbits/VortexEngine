#include "IRSender.h"
#include "IRConfig.h"

#if IR_ENABLE_SENDER == 1

#include "../Time/TimeControl.h"
#include "../Modes/Mode.h"
#include "../Log/Log.h"

#ifdef VORTEX_LIB
#include "VortexLib.h"
#endif

IRSenderCallbacks *g_irSenderCallbacks;

ByteStream IRSender_serialBuf;
BitStream IRSender_bitStream;
bool IRSender_isSending = false;
uint32_t IRSender_lastSendTime = 0;
uint32_t IRSender_size = 0;
uint8_t IRSender_numBlocks = 0;
uint8_t IRSender_remainder = 0;
uint32_t IRSender_blockSize = 0;
uint32_t IRSender_writeCounter = 0;

#if defined(VORTEX_EMBEDDED)
Tcc *IR_TCCx;
#endif

bool IRSender_init()
{
  IRSender_initPWM();
  return true;
}

void IRSender_cleanup()
{
}

bool IRSender_loadMode(const Mode *targetMode)
{
  ByteStream_clear(&IRSender_serialBuf);
  if (!Mode_saveToBuffer(targetMode, &IRSender_serialBuf, 0)) {
    DEBUG_LOG("Failed to save mode to buffer");
    return false;
  }
  if (ByteStream_rawSize(&IRSender_serialBuf) > IR_MAX_DATA_TRANSFER) {
    DEBUG_LOGF("Cannot transfer that much data: %u bytes", ByteStream_rawSize(&IRSender_serialBuf));
    return false;
  }
  BitStream_initBuf(&IRSender_bitStream, (uint8_t *)ByteStream_rawData(&IRSender_serialBuf), ByteStream_rawSize(&IRSender_serialBuf));
  IRSender_size = ByteStream_rawSize(&IRSender_serialBuf);
  IRSender_numBlocks = (IRSender_size + (IR_DEFAULT_BLOCK_SIZE - 1)) / IR_DEFAULT_BLOCK_SIZE;
  IRSender_remainder = IRSender_size % (IR_DEFAULT_BLOCK_SIZE + 0);
  DEBUG_LOGF("Num blocks: %u", IRSender_numBlocks);
  DEBUG_LOGF("Remainder: %u", IRSender_remainder);
  DEBUG_LOGF("Size: %u", IRSender_size);
  IRSender_writeCounter = 0;
  return true;
}

bool IRSender_send()
{
  if (IRSender_numBlocks < 1) {
    IRSender_isSending = false;
    return IRSender_isSending;
  }
  if (!IRSender_isSending) {
    IRSender_beginSend();
  }
  if (IRSender_lastSendTime > 0 && IRSender_lastSendTime + IR_DEFAULT_BLOCK_SPACING > Time_getCurtime()) {
    return IRSender_isSending;
  }
  uint32_t blocksize = (IRSender_numBlocks == 1) ? IRSender_remainder : IR_DEFAULT_BLOCK_SIZE;
  DEBUG_LOGF("IRSender Sending block #%u", IRSender_numBlocks);
  const uint8_t *buf_ptr = BitStream_data(&IRSender_bitStream) + IRSender_writeCounter;
  for (uint32_t i = 0; i < blocksize; ++i) {
    IRSender_sendByte(*buf_ptr);
    buf_ptr++;
    IRSender_writeCounter++;
  }
  IRSender_numBlocks--;
  IRSender_isSending = (IRSender_numBlocks > 0);
  IRSender_lastSendTime = Time_getCurtime();
  return IRSender_isSending;
}

void IRSender_beginSend()
{
  IRSender_isSending = true;
  DEBUG_LOGF("[%zu] Beginning send size %u (blocks: %u remainder: %u blocksize: %u)",
    Time_microseconds(), IRSender_size, IRSender_numBlocks, IRSender_remainder, IRSender_blockSize);
  IRSender_initPWM();
  IRSender_sendMark(50);
  IRSender_sendSpace(100);
  IRSender_sendMark(IR_HEADER_MARK);
  IRSender_sendSpace(IR_HEADER_SPACE);
  IRSender_writeCounter = 0;
  IRSender_sendByte(IRSender_numBlocks);
  IRSender_sendByte(IRSender_remainder);
}

void IRSender_sendByte(uint8_t data)
{
  for (int b = 0; b < 8; b++) {
    uint32_t bit = (data >> (7 - b)) & 1;
    IRSender_sendMark(IR_TIMING + (IR_TIMING * (2 * bit)));
    IRSender_sendSpace(IR_TIMING);
  }
  DEBUG_LOGF("Sent byte[%u]: 0x%x", IRSender_writeCounter, data);
 }

void IRSender_sendMark(uint16_t time)
{
#ifdef VORTEX_LIB
  g_irSenderCallbacks->infraredWrite(true, time);
#else
  IRSender_startPWM();
  Time_delayMicroseconds(time);
#endif
}

void IRSender_sendSpace(uint16_t time)
{
#ifdef VORTEX_LIB
  g_irSenderCallbacks->infraredWrite(false, time);
#else
  IRSender_stopPWM();
  Time_delayMicroseconds(time);
#endif
}

void IRSender_initPWM()
{
#if defined(VORTEX_EMBEDDED)
  pinMode(IR_SEND_PWM_PIN, OUTPUT);
  digitalWrite(IR_SEND_PWM_PIN, LOW);
  uint8_t port = g_APinDescription[IR_SEND_PWM_PIN].ulPort;
  uint8_t pin = g_APinDescription[IR_SEND_PWM_PIN].ulPin;
  ETCChannel IR_TCC_Channel = TCC0_CH0;
  int8_t IR_PER_EorF = PORT_PMUX_PMUXE_E;
  PORT->Group[port].PINCFG[pin].bit.PMUXEN = 1;
  PORT->Group[port].PMUX[pin >> 1].reg |= IR_PER_EorF;
  REG_GCLK_CLKCTRL = GCLK_CLKCTRL_CLKEN |
                     GCLK_CLKCTRL_GEN_GCLK0 |
                     GCLK_CLKCTRL_ID_TCC0_TCC1;
  while (GCLK->STATUS.bit.SYNCBUSY);
  IR_TCCx = (Tcc*) GetTC(IR_TCC_Channel);
  IR_TCCx->WAVE.reg |= TCC_WAVE_WAVEGEN_NPWM;
  while (IR_TCCx->SYNCBUSY.bit.WAVE);
  uint32_t cc = F_CPU/(38*1000) - 1;
  IR_TCCx->PER.reg = cc;
  while(IR_TCCx->SYNCBUSY.bit.PER);
  IR_TCCx->CC[GetTCChannelNumber(IR_TCC_Channel)].reg = cc/3;
  while (IR_TCCx->SYNCBUSY.reg & TCC_SYNCBUSY_MASK);
  IR_TCCx->CTRLA.reg |= TCC_CTRLA_PRESCALER_DIV1;
  while (IR_TCCx->SYNCBUSY.bit.ENABLE);
  IR_TCCx->CTRLA.reg &= ~TCC_CTRLA_ENABLE;
  while (IR_TCCx->SYNCBUSY.bit.ENABLE);
#endif
}

void IRSender_startPWM()
{
#if defined(VORTEX_EMBEDDED)
  IR_TCCx->CTRLA.reg |= TCC_CTRLA_ENABLE;
  while (IR_TCCx->SYNCBUSY.bit.ENABLE);
#endif
}

void IRSender_stopPWM()
{
#if defined(VORTEX_EMBEDDED)
  IR_TCCx->CTRLA.reg &= ~TCC_CTRLA_ENABLE;
  while (IR_TCCx->SYNCBUSY.bit.ENABLE);
#endif
}

#endif
