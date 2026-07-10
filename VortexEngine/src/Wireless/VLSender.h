#ifndef VL_SENDER_H
#define VL_SENDER_H

#include <inttypes.h>
#include <stdbool.h>

#include "../Serial/ByteStream.h"
#include "../Serial/BitStream.h"

#include "VLConfig.h"

#if VL_ENABLE_SENDER == 1

typedef struct Mode Mode;

typedef struct VLSenderCallbacks_s {
  void (*infraredWrite)(bool mark, uint32_t amount);
} VLSenderCallbacks;

extern VLSenderCallbacks *g_vlSenderCallbacks;

extern ByteStream VLSender_serialBuf;
extern BitStream VLSender_bitStream;
extern uint8_t VLSender_size;
extern uint8_t VLSender_parity;

bool VLSender_init();
void VLSender_cleanup();

void VLSender_send(const Mode *targetMode);
void VLSender_sendLegacy(const Mode *targetMode);

bool VLSender_loadMode(const Mode *targetMode);
void VLSender_sendByte(uint8_t data);
void VLSender_sendByteLegacy(uint8_t data);
void VLSender_sendMarkSpace(uint16_t markTime, uint16_t spaceTime);
void VLSender_startPWM();
void VLSender_stopPWM();

#endif

#endif
