#ifndef VL_CONFIG_H
#define VL_CONFIG_H

#include "../VortexConfig.h"

// Visible Light Enable
//
// Whether to enable the Visible Light system as a whole
//
#define VL_ENABLE_SENDER          1
#define VL_ENABLE_RECEIVER        1

// the size of IR blocks in bits
#define VL_DEFAULT_BLOCK_SIZE 256
#define VL_DEFAULT_BLOCK_SPACING MS_TO_TICKS(5)

// the max number of DWORDs that will transfer
#define VL_MAX_DWORDS_TRANSFER 128
#define VL_MAX_DATA_TRANSFER (VL_MAX_DWORDS_TRANSFER * sizeof(uint32_t))

// the IR receiver buffer size in dwords
#define VL_RECV_BUF_SIZE VL_MAX_DATA_TRANSFER

#define VL_THRESHOLD  0.5
#define VL_THRES_UP   (1 + VL_THRESHOLD)
#define VL_THRES_DOWN (1 - VL_THRESHOLD)

#define VL_TIMING (uint16_t)(2230)

#define VL_HEADER_MARK (uint16_t)(VL_TIMING * 16)
#define VL_HEADER_SPACE (uint16_t)(VL_TIMING * 8)

#define VL_HEADER_MARK_MIN ((uint16_t)(VL_HEADER_MARK * VL_THRES_DOWN))
#define VL_HEADER_SPACE_MIN ((uint16_t)(VL_HEADER_SPACE * VL_THRES_DOWN))

#define VL_HEADER_MARK_MAX ((uint16_t)(VL_HEADER_MARK * VL_THRES_UP))
#define VL_HEADER_SPACE_MAX ((uint16_t)(VL_HEADER_SPACE * VL_THRES_UP))

#define VL_TIMING_BIT_ONE (uint16_t)(VL_TIMING * 3)
#define VL_TIMING_BIT_ZERO (uint16_t)(VL_TIMING)
#define VL_TIMING_BIT(bit) (bit ? VL_TIMING_BIT_ONE : VL_TIMING_BIT_ZERO)


#define VL_TIMING_LEGACY (uint16_t)(3230)

#define VL_HEADER_MARK_LEGACY (uint16_t)(VL_TIMING_LEGACY * 16)
#define VL_HEADER_SPACE_LEGACY (uint16_t)(VL_TIMING_LEGACY * 8)

#define VL_HEADER_MARK_MIN_LEGACY ((uint16_t)(VL_HEADER_MARK_LEGACY * VL_THRES_DOWN))
#define VL_HEADER_SPACE_MIN_LEGACY ((uint16_t)(VL_HEADER_SPACE_LEGACY * VL_THRES_DOWN))

#define VL_HEADER_MARK_MAX_LEGACY ((uint16_t)(VL_HEADER_MARK_LEGACY * VL_THRES_UP))
#define VL_HEADER_SPACE_MAX_LEGACY ((uint16_t)(VL_HEADER_SPACE_LEGACY * VL_THRES_UP))

#define VL_TIMING_BIT_ONE_LEGACY (uint16_t)(VL_TIMING_LEGACY * 3)
#define VL_TIMING_BIT_ZERO_LEGACY (uint16_t)(VL_TIMING_LEGACY)
#define VL_TIMING_BIT_LEGACY(bit) (bit ? VL_TIMING_BIT_ONE_LEGACY : VL_TIMING_BIT_ZERO_LEGACY)


#define VL_SEND_PWM_PIN 0
#define VL_RECEIVER_PIN 0

#endif
