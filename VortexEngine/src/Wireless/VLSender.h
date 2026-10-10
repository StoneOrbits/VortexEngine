#ifndef VL_SENDER_H
#define VL_SENDER_H

#include <stdbool.h>
#include "../Serial/ByteStream.h"
#include "../Serial/BitStream.h"

#include "VLConfig.h"

#if VL_ENABLE_SENDER == 1

typedef struct Mode Mode;

bool VLSender_init();
void VLSender_cleanup();

// send a mode
void VLSender_send(const Mode *targetMode);
void VLSender_sendLegacy(const Mode *targetMode);

bool VLSender_loadMode(const Mode *targetMode);
// send a full 8 bits in a tight loop
void VLSender_sendByte(uint8_t data);
// send full 8 bits legacy protocol
void VLSender_sendByteLegacy(uint8_t data);
// send a mark/space by turning PWM on/off
void VLSender_sendMarkSpace(uint16_t markTime, uint16_t spaceTime);
// turn the VL transmitter on/off in realtime
void VLSender_startPWM();
void VLSender_stopPWM();

#endif

#endif