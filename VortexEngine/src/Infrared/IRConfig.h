#ifndef IR_CONFIG_H
#define IR_CONFIG_H

#define IR_TEST_BLOCK_SIZE 16

// the size of IR blocks in bits
#define DEFAULT_IR_BLOCK_SIZE 32
#define DEFAULT_IR_BLOCK_SPACING Time::msToTicks(300)

// the max number of DWORDs that will transfer
#define MAX_DWORDS_TRANSFER 1024
#define MAX_DATA_TRANSFER (MAX_DWORDS_TRANSFER * sizeof(uint32_t))

// the IR receiver buffer size in dwords
#define IR_RECV_BUF_SIZE MAX_DATA_TRANSFER

//#define IR_TIMING 564
#define IR_TIMING 1564
#define IR_TIMING_MIN ((uint32_t)(IR_TIMING * 0.1))

#define HEADER_MARK (IR_TIMING * 16)
#define HEADER_SPACE (IR_TIMING * 8)

#define HEADER_MARK_MIN ((uint32_t)(HEADER_MARK * 0.85))
#define HEADER_SPACE_MIN ((uint32_t)(HEADER_SPACE * 0.85))

#define HEADER_MARK_MAX ((uint32_t)(HEADER_MARK * 1.15))
#define HEADER_SPACE_MAX ((uint32_t)(HEADER_SPACE * 1.15))

#define DIVIDER_SPACE HEADER_MARK
#define DIVIDER_SPACE_MIN HEADER_MARK_MIN
#define DIVIDER_SPACE_MAX HEADER_MARK_MAX

#define IR_SEND_PWM_PIN 0
#define RECEIVER_PIN 2

#endif
