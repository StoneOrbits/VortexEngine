#ifndef IR_SENDER_H
#define IR_SENDER_H

#include <stdbool.h>
#include <inttypes.h>

#include "../Serial/ByteStream.h"
#include "../Serial/BitStream.h"

#include "IRConfig.h"

#if IR_ENABLE_SENDER == 1

typedef struct Mode Mode;

// init and cleanup the sender
bool IRSender_init();
void IRSender_cleanup();

// initialize the IR sender with a mode to send
bool IRSender_loadMode(const Mode *targetMode);
bool IRSender_send();

bool IRSender_isSending();
uint32_t IRSender_percentDone();

#endif

#endif