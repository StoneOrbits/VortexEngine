#ifndef IR_SENDER_H
#define IR_SENDER_H

#include <inttypes.h>
#include <stdbool.h>

#include "../Serial/ByteStream.h"
#include "../Serial/BitStream.h"

#include "IRConfig.h"

#if IR_ENABLE_SENDER == 1

typedef struct Mode Mode;

typedef struct IRSenderCallbacks_s {
  void (*infraredWrite)(bool mark, uint32_t amount);
} IRSenderCallbacks;

extern IRSenderCallbacks *g_irSenderCallbacks;

extern ByteStream IRSender_serialBuf;
extern BitStream IRSender_bitStream;
extern bool IRSender_isSending;
extern uint32_t IRSender_lastSendTime;
extern uint32_t IRSender_size;
extern uint8_t IRSender_numBlocks;
extern uint8_t IRSender_remainder;
extern uint32_t IRSender_blockSize;
extern uint32_t IRSender_writeCounter;

bool IRSender_init();
void IRSender_cleanup();

bool IRSender_loadMode(const Mode *targetMode);
bool IRSender_send();

static inline bool IRSender_isSending_get() { return IRSender_isSending; }

static inline uint32_t IRSender_percentDone() {
  return (uint32_t)(((float)IRSender_writeCounter / (float)IRSender_size) * 100.0);
}

void IRSender_beginSend();
void IRSender_sendByte(uint8_t data);
void IRSender_sendMark(uint16_t time);
void IRSender_sendSpace(uint16_t time);
void IRSender_initPWM();
void IRSender_startPWM();
void IRSender_stopPWM();

#endif

#endif
