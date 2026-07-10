#ifndef IR_RECEIVER_H
#define IR_RECEIVER_H

#include <inttypes.h>
#include <stdbool.h>

#include "../Serial/BitStream.h"

#include "IRConfig.h"

#if IR_ENABLE_RECEIVER == 1

typedef struct ByteStream ByteStream;
typedef struct Mode Mode;

typedef enum {
  IR_WAITING_HEADER_MARK,
  IR_WAITING_HEADER_SPACE,
  IR_READING_DATA_MARK,
  IR_READING_DATA_SPACE
} IRRecvState;

extern BitStream IRReceiver_irData;
extern IRRecvState IRReceiver_recvState;
extern uint32_t IRReceiver_prevTime;
extern uint8_t IRReceiver_pinState;
extern uint32_t IRReceiver_previousBytes;

bool IRReceiver_init();
void IRReceiver_cleanup();

bool IRReceiver_dataReady();
bool IRReceiver_isReceiving();
uint8_t IRReceiver_percentReceived();

static inline uint16_t IRReceiver_bytesReceived() {
  return BitStream_bytepos(&IRReceiver_irData);
}

bool IRReceiver_receiveMode(Mode *pMode);

bool IRReceiver_beginReceiving();
bool IRReceiver_endReceiving();
bool IRReceiver_onNewData();
void IRReceiver_resetIRState();

bool IRReceiver_read(ByteStream *data);
void IRReceiver_recvPCIHandler();
void IRReceiver_handleIRTiming(uint32_t diff);

#endif

#endif
