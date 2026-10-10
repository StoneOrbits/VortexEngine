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
#define RGB_WHITE          ((RGBColor){ .red = 0xFF, .green = 0xFF, .blue = 0xFF })    // 255 255 255
#define RGB_BLUE           ((RGBColor){ .red = 0x00, .green = 0x00, .blue = 0xFF })    //   0   0 255
#define RGB_YELLOW         ((RGBColor){ .red = 0xFF, .green = 0xFF, .blue = 0x00 })    // 255 255   0
#define RGB_RED            ((RGBColor){ .red = 0xFF, .green = 0x00, .blue = 0x00 })    // 255   0   0
#define RGB_GREEN          ((RGBColor){ .red = 0x00, .green = 0xFF, .blue = 0x00 })    //   0 255   0
#define RGB_CYAN           ((RGBColor){ .red = 0x00, .green = 0xFF, .blue = 0xFF })    //   0 255 255
#define RGB_PURPLE         ((RGBColor){ .red = 0x99, .green = 0x33, .blue = 0xFF })    // 153  51 255
#define RGB_ORANGE         ((RGBColor){ .red = 0xFF, .green = 0x83, .blue = 0x00 })    // 255 131   0
#define RGB_PINK           ((RGBColor){ .red = 0xFF, .green = 0x00, .blue = 0x99 })    // 255   0 153
#define RGB_MAGENTA        ((RGBColor){ .red = 0xFF, .green = 0x00, .blue = 0xFF })    // 255   0 255
#define RGB_OFF            ((RGBColor){ .red = 0x00, .green = 0x00, .blue = 0x00 })    //   0   0   0

// ===========================================================
//  Steps of colors in increasing brightness from 0 to 9,
//  Where O is the lowest brightness and 9 is the highest
//  brightness before the pure colors above

#define RGB_WHITE0         ((RGBColor){ .red = 0x10, .green = 0x10, .blue = 0x10 })    //  16  16  16
#define RGB_WHITE1         ((RGBColor){ .red = 0x1C, .green = 0x1C, .blue = 0x1C })    //  28  28  28
#define RGB_WHITE2         ((RGBColor){ .red = 0x38, .green = 0x38, .blue = 0x38 })    //  56  56  56
#define RGB_WHITE3         ((RGBColor){ .red = 0x54, .green = 0x54, .blue = 0x54 })    //  84  84  84
#define RGB_WHITE4         ((RGBColor){ .red = 0x70, .green = 0x70, .blue = 0x70 })    // 112 112 112
#define RGB_WHITE5         ((RGBColor){ .red = 0x8C, .green = 0x8C, .blue = 0x8C })    // 140 140 140
#define RGB_WHITE6         ((RGBColor){ .red = 0xA8, .green = 0xA8, .blue = 0xA8 })    // 168 168 168
#define RGB_WHITE7         ((RGBColor){ .red = 0xC4, .green = 0xC4, .blue = 0xC4 })    // 196 196 196
#define RGB_WHITE8         ((RGBColor){ .red = 0xE0, .green = 0xE0, .blue = 0xE0 })    // 224 224 224
#define RGB_WHITE9         ((RGBColor){ .red = 0xFC, .green = 0xFC, .blue = 0xFC })    // 252 252 252

#define RGB_BLUE0          ((RGBColor){ .red = 0x00, .green = 0x00, .blue = 0x10 })    //   0   0  16
#define RGB_BLUE1          ((RGBColor){ .red = 0x00, .green = 0x00, .blue = 0x1C })    //   0   0  28
#define RGB_BLUE2          ((RGBColor){ .red = 0x00, .green = 0x00, .blue = 0x38 })    //   0   0  56
#define RGB_BLUE3          ((RGBColor){ .red = 0x00, .green = 0x00, .blue = 0x54 })    //   0   0  84
#define RGB_BLUE4          ((RGBColor){ .red = 0x00, .green = 0x00, .blue = 0x70 })    //   0   0 112
#define RGB_BLUE5          ((RGBColor){ .red = 0x00, .green = 0x00, .blue = 0x8C })    //   0   0 140
#define RGB_BLUE6          ((RGBColor){ .red = 0x00, .green = 0x00, .blue = 0xA8 })    //   0   0 168
#define RGB_BLUE7          ((RGBColor){ .red = 0x00, .green = 0x00, .blue = 0xC4 })    //   0   0 196
#define RGB_BLUE8          ((RGBColor){ .red = 0x00, .green = 0x00, .blue = 0xE0 })    //   0   0 224
#define RGB_BLUE9          ((RGBColor){ .red = 0x00, .green = 0x00, .blue = 0xFC })    //   0   0 252

#define RGB_YELLOW0        ((RGBColor){ .red = 0x10, .green = 0x10, .blue = 0x00 })    //  16  16   0
#define RGB_YELLOW1        ((RGBColor){ .red = 0x1C, .green = 0x1C, .blue = 0x00 })    //  28  28   0
#define RGB_YELLOW2        ((RGBColor){ .red = 0x38, .green = 0x38, .blue = 0x00 })    //  56  56   0
#define RGB_YELLOW3        ((RGBColor){ .red = 0x54, .green = 0x54, .blue = 0x00 })    //  84  84   0
#define RGB_YELLOW4        ((RGBColor){ .red = 0x70, .green = 0x70, .blue = 0x00 })    // 112 112   0
#define RGB_YELLOW5        ((RGBColor){ .red = 0x8C, .green = 0x8C, .blue = 0x00 })    // 140 140   0
#define RGB_YELLOW6        ((RGBColor){ .red = 0xA8, .green = 0xA8, .blue = 0x00 })    // 168 168   0
#define RGB_YELLOW7        ((RGBColor){ .red = 0xC4, .green = 0xC4, .blue = 0x00 })    // 196 196   0
#define RGB_YELLOW8        ((RGBColor){ .red = 0xE0, .green = 0xE0, .blue = 0x00 })    // 224 224   0
#define RGB_YELLOW9        ((RGBColor){ .red = 0xFC, .green = 0xFC, .blue = 0x00 })    // 252 252   0

#define RGB_RED0           ((RGBColor){ .red = 0x10, .green = 0x00, .blue = 0x00 })    //  16   0   0
#define RGB_RED1           ((RGBColor){ .red = 0x1C, .green = 0x00, .blue = 0x00 })    //  28   0   0
#define RGB_RED2           ((RGBColor){ .red = 0x38, .green = 0x00, .blue = 0x00 })    //  56   0   0
#define RGB_RED3           ((RGBColor){ .red = 0x54, .green = 0x00, .blue = 0x00 })    //  84   0   0
#define RGB_RED4           ((RGBColor){ .red = 0x70, .green = 0x00, .blue = 0x00 })    // 112   0   0
#define RGB_RED5           ((RGBColor){ .red = 0x8C, .green = 0x00, .blue = 0x00 })    // 140   0   0
#define RGB_RED6           ((RGBColor){ .red = 0xA8, .green = 0x00, .blue = 0x00 })    // 168   0   0
#define RGB_RED7           ((RGBColor){ .red = 0xC4, .green = 0x00, .blue = 0x00 })    // 196   0   0
#define RGB_RED8           ((RGBColor){ .red = 0xE0, .green = 0x00, .blue = 0x00 })    // 224   0   0
#define RGB_RED9           ((RGBColor){ .red = 0xFC, .green = 0x00, .blue = 0x00 })    // 252   0   0

#define RGB_GREEN0         ((RGBColor){ .red = 0x00, .green = 0x10, .blue = 0x00 })    //   0  16   0
#define RGB_GREEN1         ((RGBColor){ .red = 0x00, .green = 0x1C, .blue = 0x00 })    //   0  28   0
#define RGB_GREEN2         ((RGBColor){ .red = 0x00, .green = 0x38, .blue = 0x00 })    //   0  56   0
#define RGB_GREEN3         ((RGBColor){ .red = 0x00, .green = 0x54, .blue = 0x00 })    //   0  84   0
#define RGB_GREEN4         ((RGBColor){ .red = 0x00, .green = 0x70, .blue = 0x00 })    //   0 112   0
#define RGB_GREEN5         ((RGBColor){ .red = 0x00, .green = 0x8C, .blue = 0x00 })    //   0 140   0
#define RGB_GREEN6         ((RGBColor){ .red = 0x00, .green = 0xA8, .blue = 0x00 })    //   0 168   0
#define RGB_GREEN7         ((RGBColor){ .red = 0x00, .green = 0xC4, .blue = 0x00 })    //   0 196   0
#define RGB_GREEN8         ((RGBColor){ .red = 0x00, .green = 0xE0, .blue = 0x00 })    //   0 224   0
#define RGB_GREEN9         ((RGBColor){ .red = 0x00, .green = 0xFC, .blue = 0x00 })    //   0 252   0

#define RGB_CYAN0          ((RGBColor){ .red = 0x00, .green = 0x10, .blue = 0x10 })    //   0  16  16
#define RGB_CYAN1          ((RGBColor){ .red = 0x00, .green = 0x1C, .blue = 0x1C })    //   0  28  28
#define RGB_CYAN2          ((RGBColor){ .red = 0x00, .green = 0x38, .blue = 0x38 })    //   0  56  56
#define RGB_CYAN3          ((RGBColor){ .red = 0x00, .green = 0x54, .blue = 0x54 })    //   0  84  84
#define RGB_CYAN4          ((RGBColor){ .red = 0x00, .green = 0x70, .blue = 0x70 })    //   0 112 112
#define RGB_CYAN5          ((RGBColor){ .red = 0x00, .green = 0x8C, .blue = 0x8C })    //   0 140 140
#define RGB_CYAN6          ((RGBColor){ .red = 0x00, .green = 0xA8, .blue = 0xA8 })    //   0 168 168
#define RGB_CYAN7          ((RGBColor){ .red = 0x00, .green = 0xC4, .blue = 0xC4 })    //   0 196 196
#define RGB_CYAN8          ((RGBColor){ .red = 0x00, .green = 0xE0, .blue = 0xE0 })    //   0 224 224
#define RGB_CYAN9          ((RGBColor){ .red = 0x00, .green = 0xFC, .blue = 0xFC })    //   0 252 252

#define RGB_MAGENTA0       ((RGBColor){ .red = 0x10, .green = 0x00, .blue = 0x10 })    //  16   0  16
#define RGB_MAGENTA1       ((RGBColor){ .red = 0x1C, .green = 0x00, .blue = 0x1C })    //  28   0  28
#define RGB_MAGENTA2       ((RGBColor){ .red = 0x38, .green = 0x00, .blue = 0x38 })    //  56   0  56
#define RGB_MAGENTA3       ((RGBColor){ .red = 0x54, .green = 0x00, .blue = 0x54 })    //  84   0  84
#define RGB_MAGENTA4       ((RGBColor){ .red = 0x70, .green = 0x00, .blue = 0x70 })    // 112   0 112
#define RGB_MAGENTA5       ((RGBColor){ .red = 0x8C, .green = 0x00, .blue = 0x8C })    // 140   0 140
#define RGB_MAGENTA6       ((RGBColor){ .red = 0xA8, .green = 0x00, .blue = 0xA8 })    // 168   0 168
#define RGB_MAGENTA7       ((RGBColor){ .red = 0xC4, .green = 0x00, .blue = 0xC4 })    // 196   0 196
#define RGB_MAGENTA8       ((RGBColor){ .red = 0xE0, .green = 0x00, .blue = 0xE0 })    // 224   0 224
#define RGB_MAGENTA9       ((RGBColor){ .red = 0xFC, .green = 0x00, .blue = 0xFC })    // 252   0 252

#define RGB_ORANGE0        ((RGBColor){ .red = 0x10, .green = 0x08, .blue = 0x00 })    //  16   8   0
#define RGB_ORANGE1        ((RGBColor){ .red = 0x1C, .green = 0x0E, .blue = 0x00 })    //  28  14   0
#define RGB_ORANGE2        ((RGBColor){ .red = 0x38, .green = 0x1C, .blue = 0x00 })    //  56  28   0
#define RGB_ORANGE3        ((RGBColor){ .red = 0x54, .green = 0x2B, .blue = 0x00 })    //  84  43   0
#define RGB_ORANGE4        ((RGBColor){ .red = 0x70, .green = 0x39, .blue = 0x00 })    // 112  57   0
#define RGB_ORANGE5        ((RGBColor){ .red = 0x8C, .green = 0x48, .blue = 0x00 })    // 140  72   0
#define RGB_ORANGE6        ((RGBColor){ .red = 0xA8, .green = 0x56, .blue = 0x00 })    // 168  86   0
#define RGB_ORANGE7        ((RGBColor){ .red = 0xC4, .green = 0x65, .blue = 0x00 })    // 196 101   0
#define RGB_ORANGE8        ((RGBColor){ .red = 0xE0, .green = 0x73, .blue = 0x00 })    // 224 115   0
#define RGB_ORANGE9        ((RGBColor){ .red = 0xFC, .green = 0x82, .blue = 0x00 })    // 252 130   0
