#ifndef IR_RECEIVER_H
#define IR_RECEIVER_H

#include <stdbool.h>
#include <inttypes.h>

#include "../Serial/BitStream.h"

#include "IRConfig.h"

#if IR_ENABLE_RECEIVER == 1

typedef struct ByteStream ByteStream;
typedef struct Mode Mode;

// init and cleanup the receiver
bool IRReceiver_init();
void IRReceiver_cleanup();

// check whether a full IR message is ready to read
bool IRReceiver_dataReady();
// whether actively receiving
bool IRReceiver_isReceiving();
// the percent of data received
uint8_t IRReceiver_percentReceived();
uint16_t IRReceiver_bytesReceived();

// receive the IR message into a target mode
bool IRReceiver_receiveMode(Mode *pMode);

// turn the receiver on/off
bool IRReceiver_beginReceiving();
bool IRReceiver_endReceiving();
// checks if there is new data since the last reset or check
bool IRReceiver_onNewData();
// reset IR receiver buffer
void IRReceiver_resetIRState();

// The handler called when the IR receiver pin changes states
// this has to be public because it's called from a global ISR
// but technically it's an internal function for IRReceiver
void IRReceiver_recvPCIHandler();

// reading functions
bool IRReceiver_read(ByteStream *data);
void IRReceiver_handleIRTiming(uint32_t diff);

#endif

#endif