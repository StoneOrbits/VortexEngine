#pragma once

// ====================================================================================================
//  Color Constants
//
// This file defines constants for colors as we best see them for the Vortex Engine

// Some Pre-defined hue values
// TODO: remove HSV_ underscore once FastLED is gone
#define HSV_HUE_RED     0
#define HSV_HUE_ORANGE  32
#define HSV_HUE_YELLOW  64
#define HSV_HUE_GREEN   96
#define HSV_HUE_AQUA    128
#define HSV_HUE_BLUE    160
#define HSV_HUE_PURPLE  192
#define HSV_HUE_PINK    224

// if this bit is present it's an HSV constant
#define HSV_BIT ((uint32_t)1 << 31)

// produce a DWORD HSV constant
#define HSV(h, s, v) (HSV_BIT | ((uint32_t)h << 16) | ((uint32_t)s << 8) | (uint32_t)v)

// Pre defined hex HSV values
#define HSV_WHITE   HSV_BIT | (uint32_t)0x00006E    //   0   0 110
#define HSV_BLUE    HSV_BIT | (uint32_t)0xA0FF6E    // 160 255 110
#define HSV_YELLOW  HSV_BIT | (uint32_t)0x3CFF6E    //  60 255 110
#define HSV_RED     HSV_BIT | (uint32_t)0x00FF6E    //   0 255 110
#define HSV_GREEN   HSV_BIT | (uint32_t)0x55FF6E    //  85 255 110
#define HSV_CYAN    HSV_BIT | (uint32_t)0x78FF6E    // 120 255 110
#define HSV_PURPLE  HSV_BIT | (uint32_t)0xD4FF6E    // 212 255 110
#define HSV_ORANGE  HSV_BIT | (uint32_t)0x14FF6E    //  20 255 110
#define HSV_OFF     HSV_BIT | (uint32_t)0x000000    //   0   0   0

// Pure RGB colors (brightest)
#define RGB_WHITE       (uint32_t)0xFFFFFF    // 255 255 255
#define RGB_BLUE        (uint32_t)0x0000FF    //   0   0 255
#define RGB_YELLOW      (uint32_t)0xFFFF00    // 255 255   0
#define RGB_RED         (uint32_t)0xFF0000    // 255   0   0
#define RGB_GREEN       (uint32_t)0x00FF00    //   0 255   0
#define RGB_CYAN        (uint32_t)0x00FFFF    //   0 255 255
#define RGB_PURPLE      (uint32_t)0x9933FF    // 153  51 255
#define RGB_ORANGE      (uint32_t)0xFF8300    // 255 131   0
#define RGB_OFF         (uint32_t)0x000000    //   0   0   0

// ===========================================================
//  Steps of colors in increasing brightness from 0 to 9,
//  Where O is the lowest brightness and 9 is the highest
//  brightness before the pure colors above

#define RGB_WHITE0      (uint32_t)0x101010    //  16  16  16
#define RGB_WHITE1      (uint32_t)0x1C1C1C    //  28  28  28
#define RGB_WHITE2      (uint32_t)0x383838    //  56  56  56
#define RGB_WHITE3      (uint32_t)0x545454    //  84  84  84
#define RGB_WHITE4      (uint32_t)0x707070    // 112 112 112
#define RGB_WHITE5      (uint32_t)0x8C8C8C    // 140 140 140
#define RGB_WHITE6      (uint32_t)0xA8A8A8    // 168 168 168
#define RGB_WHITE7      (uint32_t)0xC4C4C4    // 196 196 196
#define RGB_WHITE8      (uint32_t)0xE0E0E0    // 224 224 224
#define RGB_WHITE9      (uint32_t)0xFCFCFC    // 252 252 252

#define RGB_BLUE0       (uint32_t)0x000010    //   0   0  16
#define RGB_BLUE1       (uint32_t)0x00001C    //   0   0  28
#define RGB_BLUE2       (uint32_t)0x000038    //   0   0  56
#define RGB_BLUE3       (uint32_t)0x000054    //   0   0  84
#define RGB_BLUE4       (uint32_t)0x000070    //   0   0 112
#define RGB_BLUE5       (uint32_t)0x00008C    //   0   0 140
#define RGB_BLUE6       (uint32_t)0x0000A8    //   0   0 168
#define RGB_BLUE7       (uint32_t)0x0000C4    //   0   0 196
#define RGB_BLUE8       (uint32_t)0x0000E0    //   0   0 224
#define RGB_BLUE9       (uint32_t)0x0000FC    //   0   0 252

#define RGB_YELLOW0     (uint32_t)0x101000    //  16  16   0
#define RGB_YELLOW1     (uint32_t)0x1C1C00    //  28  28   0
#define RGB_YELLOW2     (uint32_t)0x383800    //  56  56   0
#define RGB_YELLOW3     (uint32_t)0x545400    //  84  84   0
#define RGB_YELLOW4     (uint32_t)0x707000    // 112 112   0
#define RGB_YELLOW5     (uint32_t)0x8C8C00    // 140 140   0
#define RGB_YELLOW6     (uint32_t)0xA8A800    // 168 168   0
#define RGB_YELLOW7     (uint32_t)0xC4C400    // 196 196   0
#define RGB_YELLOW8     (uint32_t)0xE0E000    // 224 224   0
#define RGB_YELLOW9     (uint32_t)0xFCFC00    // 252 252   0

#define RGB_RED0        (uint32_t)0x100000    //  16   0   0
#define RGB_RED1        (uint32_t)0x1C0000    //  28   0   0
#define RGB_RED2        (uint32_t)0x380000    //  56   0   0
#define RGB_RED3        (uint32_t)0x540000    //  84   0   0
#define RGB_RED4        (uint32_t)0x700000    // 112   0   0
#define RGB_RED5        (uint32_t)0x8C0000    // 140   0   0
#define RGB_RED6        (uint32_t)0xA80000    // 168   0   0
#define RGB_RED7        (uint32_t)0xC40000    // 196   0   0
#define RGB_RED8        (uint32_t)0xE00000    // 224   0   0
#define RGB_RED9        (uint32_t)0xFC0000    // 252   0   0

#define RGB_GREEN0      (uint32_t)0x001000    //   0  16   0
#define RGB_GREEN1      (uint32_t)0x001C00    //   0  28   0
#define RGB_GREEN2      (uint32_t)0x003800    //   0  56   0
#define RGB_GREEN3      (uint32_t)0x005400    //   0  84   0
#define RGB_GREEN4      (uint32_t)0x007000    //   0 112   0
#define RGB_GREEN5      (uint32_t)0x008C00    //   0 140   0
#define RGB_GREEN6      (uint32_t)0x00A800    //   0 168   0
#define RGB_GREEN7      (uint32_t)0x00C400    //   0 196   0
#define RGB_GREEN8      (uint32_t)0x00E000    //   0 224   0
#define RGB_GREEN9      (uint32_t)0x00FC00    //   0 252   0

#define RGB_CYAN0       (uint32_t)0x001010    //   0  16  16
#define RGB_CYAN1       (uint32_t)0x001C1C    //   0  28  28
#define RGB_CYAN2       (uint32_t)0x003838    //   0  56  56
#define RGB_CYAN3       (uint32_t)0x005454    //   0  84  84
#define RGB_CYAN4       (uint32_t)0x007070    //   0 112 112
#define RGB_CYAN5       (uint32_t)0x008C8C    //   0 140 140
#define RGB_CYAN6       (uint32_t)0x00A8A8    //   0 168 168
#define RGB_CYAN7       (uint32_t)0x00C4C4    //   0 196 196
#define RGB_CYAN8       (uint32_t)0x00E0E0    //   0 224 224
#define RGB_CYAN9       (uint32_t)0x00FCFC    //   0 252 252

#define RGB_PURPLE0     (uint32_t)0x090310    //   9   3  16
#define RGB_PURPLE1     (uint32_t)0x1C001C    //  28   0  28
#define RGB_PURPLE2     (uint32_t)0x380038    //  56   0  56
#define RGB_PURPLE3     (uint32_t)0x540054    //  84   0  84
#define RGB_PURPLE4     (uint32_t)0x700070    // 112   0 112
#define RGB_PURPLE5     (uint32_t)0x8C008C    // 140   0 140
#define RGB_PURPLE6     (uint32_t)0xA800A8    // 168   0 168
#define RGB_PURPLE7     (uint32_t)0xC400C4    // 196   0 196
#define RGB_PURPLE8     (uint32_t)0xE000E0    // 224   0 224
#define RGB_PURPLE9     (uint32_t)0xFC00FC    // 252   0 252

#define RGB_ORANGE0     (uint32_t)0x100800    //  16   8   0
#define RGB_ORANGE1     (uint32_t)0x1C0E00    //  28  14   0
#define RGB_ORANGE2     (uint32_t)0x381C00    //  56  28   0
#define RGB_ORANGE3     (uint32_t)0x542B00    //  84  43   0
#define RGB_ORANGE4     (uint32_t)0x703900    // 112  57   0
#define RGB_ORANGE5     (uint32_t)0x8C4800    // 140  72   0
#define RGB_ORANGE6     (uint32_t)0xA85600    // 168  86   0
#define RGB_ORANGE7     (uint32_t)0xC46500    // 196 101   0
#define RGB_ORANGE8     (uint32_t)0xE07300    // 224 115   0
#define RGB_ORANGE9     (uint32_t)0xFC8200    // 252 130   0
