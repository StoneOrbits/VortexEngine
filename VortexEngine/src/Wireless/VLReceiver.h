#ifndef VL_RECEIVER_H
#define VL_RECEIVER_H

#include <stdbool.h>
#include <inttypes.h>

#include "../Serial/BitStream.h"

#include "VLConfig.h"

#if VL_ENABLE_RECEIVER == 1

typedef struct ByteStream ByteStream;
typedef struct Mode Mode;

// init and cleanup the receiver
bool VLReceiver_init();
void VLReceiver_cleanup();

// check whether a full VL message is ready to read
bool VLReceiver_dataReady();
// whether actively receiving
bool VLReceiver_isReceiving();
// the percent of data received
uint8_t VLReceiver_percentReceived();
uint16_t VLReceiver_bytesReceived();

// receive the VL message into a target mode
bool VLReceiver_receiveMode(Mode *pMode);

// turn the receiver on/off
bool VLReceiver_beginReceiving();
bool VLReceiver_endReceiving();
// checks if there is new data since the last reset or check
bool VLReceiver_onNewData();
// reset VL receiver buffer
void VLReceiver_resetVLState();

// turn on/off the legacy receiver
void VLReceiver_setLegacyReceiver(bool legacy);

// The handler called when the VL receiver flips between on/off
// this has to be public because it's called from a global ISR
// but technically it's an internal function for VLReceiver
void VLReceiver_recvPCIHandler();

// reading function
bool VLReceiver_read(ByteStream *data);
void VLReceiver_handleVLTiming(uint16_t diff);
void VLReceiver_handleVLTimingLegacy(uint16_t diff);

#endif

#endif