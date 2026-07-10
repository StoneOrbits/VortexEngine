#include "IRReceiver.h"
#include "IRConfig.h"

#if IR_ENABLE_RECEIVER == 1

#include "../Serial/ByteStream.h"
#include "../Serial/BitStream.h"
#include "../Time/TimeControl.h"
#include "../Modes/Mode.h"
#include "../Log/Log.h"

BitStream IRReceiver_irData;
IRRecvState IRReceiver_recvState = IR_WAITING_HEADER_MARK;
uint32_t IRReceiver_prevTime = 0;
uint8_t IRReceiver_pinState = 0;
uint32_t IRReceiver_previousBytes = 0;

bool IRReceiver_init()
{
  BitStream_initAlloc(&IRReceiver_irData, IR_RECV_BUF_SIZE);
  return true;
}

void IRReceiver_cleanup()
{
}

bool IRReceiver_dataReady()
{
  if (!IRReceiver_isReceiving()) {
    return false;
  }
  uint8_t blocks = BitStream_data(&IRReceiver_irData)[0];
  uint8_t remainder = BitStream_data(&IRReceiver_irData)[1];
  uint32_t total = ((blocks - 1) * 32) + remainder;
  if (!total || total > IR_MAX_DATA_TRANSFER) {
    DEBUG_LOGF("Bad IR Data size: %u", total);
    return false;
  }
  return (BitStream_bytepos(&IRReceiver_irData) >= (uint32_t)(total + 2));
}

bool IRReceiver_isReceiving()
{
  return (BitStream_bytepos(&IRReceiver_irData) > 2);
}

uint8_t IRReceiver_percentReceived()
{
  if (!IRReceiver_isReceiving()) {
    return 0;
  }
  uint8_t blocks = BitStream_data(&IRReceiver_irData)[0];
  uint8_t remainder = BitStream_data(&IRReceiver_irData)[1];
  uint16_t total = ((blocks - 1) * 32) + remainder;
  return (uint8_t)((uint16_t)((BitStream_bytepos(&IRReceiver_irData) * 100 + (total / 2)) / total));
}

bool IRReceiver_receiveMode(Mode *pMode)
{
  ByteStream buf;
  if (!IRReceiver_read(&buf)) {
    DEBUG_LOG("No data available to read, or error reading");
    return false;
  }
  DEBUG_LOGF("Received %u bytes", ByteStream_rawSize(&buf));
  return Mode_loadFromBuffer(pMode, &buf);
}

bool IRReceiver_beginReceiving()
{
  IRReceiver_resetIRState();
  return true;
}

bool IRReceiver_endReceiving()
{
  IRReceiver_resetIRState();
  return true;
}

bool IRReceiver_onNewData()
{
  if (IRReceiver_bytesReceived() == IRReceiver_previousBytes) {
    return false;
  }
  IRReceiver_previousBytes = IRReceiver_bytesReceived();
  return true;
}

bool IRReceiver_read(ByteStream *data)
{
  if (!BitStream_bytepos(&IRReceiver_irData) || BitStream_bytepos(&IRReceiver_irData) > IR_MAX_DATA_TRANSFER) {
    DEBUG_LOG("Nothing to read, or read too much");
    return false;
  }
  uint8_t blocks = BitStream_data(&IRReceiver_irData)[0];
  uint8_t remainder = BitStream_data(&IRReceiver_irData)[1];
  uint32_t size = ((blocks - 1) * 32) + remainder;
  if (!size || size > IR_MAX_DATA_TRANSFER) {
    DEBUG_LOGF("Bad IR Data size: %u", size);
    return false;
  }
  const uint8_t *actualData = BitStream_data(&IRReceiver_irData) + 2;
  if (!ByteStream_rawInit(data, actualData, size)) {
    DEBUG_LOG("Failed to init buffer for IR read");
    return false;
  }
  IRReceiver_resetIRState();
  return true;
}

void IRReceiver_recvPCIHandler()
{
  IRReceiver_pinState = (uint8_t)!IRReceiver_pinState;
  uint32_t now = Time_microseconds();
  if (!IRReceiver_prevTime || IRReceiver_prevTime > now) {
    IRReceiver_prevTime = now;
    DEBUG_LOG("Bad first time diff, resetting...");
    IRReceiver_resetIRState();
    return;
  }
  uint32_t diff = (uint32_t)(now - IRReceiver_prevTime);
  IRReceiver_prevTime = now;
  IRReceiver_handleIRTiming(diff);
}

void IRReceiver_handleIRTiming(uint32_t diff)
{
  if ((diff > IR_HEADER_MARK_MAX && IRReceiver_recvState < IR_READING_DATA_MARK) || diff < IR_TIMING_MIN) {
    DEBUG_LOGF("bad delay: %u, resetting...", diff);
    IRReceiver_resetIRState();
    return;
  }
  switch (IRReceiver_recvState) {
  case IR_WAITING_HEADER_MARK:
    if (diff >= IR_HEADER_MARK_MIN && diff <= IR_HEADER_MARK_MAX) {
      IRReceiver_recvState = IR_WAITING_HEADER_SPACE;
    } else {
      DEBUG_LOGF("Bad header mark %u, resetting...", diff);
      IRReceiver_resetIRState();
    }
    break;
  case IR_WAITING_HEADER_SPACE:
    if (diff >= IR_HEADER_SPACE_MIN && diff <= IR_HEADER_SPACE_MAX) {
      IRReceiver_recvState = IR_READING_DATA_MARK;
    } else {
      DEBUG_LOGF("Bad header space %u, resetting...", diff);
      IRReceiver_resetIRState();
    }
    break;
  case IR_READING_DATA_MARK:
    BitStream_write1Bit(&IRReceiver_irData, (diff > (IR_TIMING * 2)) ? 1 : 0);
    IRReceiver_recvState = IR_READING_DATA_SPACE;
    break;
  case IR_READING_DATA_SPACE:
    IRReceiver_recvState = IR_READING_DATA_MARK;
    break;
  default:
    DEBUG_LOGF("Bad receive state: %u", IRReceiver_recvState);
    break;
  }
}

void IRReceiver_resetIRState()
{
  IRReceiver_previousBytes = 0;
  IRReceiver_recvState = IR_WAITING_HEADER_MARK;
  BitStream_reset(&IRReceiver_irData);
  DEBUG_LOG("IR State Reset");
}

#endif
