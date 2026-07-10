#ifndef VL_RECEIVER_H
#define VL_RECEIVER_H

#include <inttypes.h>
#include <stdbool.h>

#include "../Serial/BitStream.h"

#include "VLConfig.h"

#if VL_ENABLE_RECEIVER == 1

typedef struct ByteStream ByteStream;
typedef struct Mode Mode;

typedef enum {
  VL_WAITING_HEADER_MARK,
  VL_WAITING_HEADER_SPACE,
  VL_READING_BAUD_MARK,
  VL_READING_BAUD_SPACE,
  VL_READING_DATA_MARK,
  VL_READING_DATA_SPACE,
  VL_READING_DATA_PARITY_MARK,
  VL_READING_DATA_PARITY_SPACE
} VLRecvState;

extern BitStream VLReceiver_vlData;
extern VLRecvState VLReceiver_recvState;
extern uint32_t VLReceiver_prevTime;
extern uint8_t VLReceiver_pinState;
extern uint16_t VLReceiver_previousBytes;
extern uint16_t VLReceiver_vlMarkThreshold;
extern uint16_t VLReceiver_vlSpaceThreshold;
extern uint8_t VLReceiver_counter;
extern uint8_t VLReceiver_parityBit;
extern bool VLReceiver_legacy;

bool VLReceiver_init();
void VLReceiver_cleanup();

bool VLReceiver_dataReady();
bool VLReceiver_isReceiving();
uint8_t VLReceiver_percentReceived();

static inline uint16_t VLReceiver_bytesReceived() {
  return BitStream_bytepos(&VLReceiver_vlData);
}

bool VLReceiver_receiveMode(Mode *pMode);

bool VLReceiver_beginReceiving();
bool VLReceiver_endReceiving();
bool VLReceiver_onNewData();
void VLReceiver_resetVLState();

void VLReceiver_setLegacyReceiver(bool legacy);

void VLReceiver_recvPCIHandler();

bool VLReceiver_read(ByteStream *data);
void VLReceiver_handleVLTiming(uint16_t diff);
void VLReceiver_handleVLTimingLegacy(uint16_t diff);

#endif

#endif
