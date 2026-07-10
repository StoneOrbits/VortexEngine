#ifndef COLOR_CONSTANTS_H
#define COLOR_CONSTANTS_H

#define HSV_HUE_RED     0
#define HSV_HUE_ORANGE  32
#define HSV_HUE_YELLOW  64
#define HSV_HUE_GREEN   96
#define HSV_HUE_AQUA    128
#define HSV_HUE_BLUE    160
#define HSV_HUE_PURPLE  192
#define HSV_HUE_PINK    224

#define HSV_BIT ((uint32_t)1 << 31)

#define HSV(h, s, v) (HSV_BIT | ((uint32_t)h << 16) | ((uint32_t)s << 8) | (uint32_t)v)

#define HSV_WHITE   HSV_BIT | (uint32_t)0x00006E
#define HSV_BLUE    HSV_BIT | (uint32_t)0xA0FF6E
#define HSV_YELLOW  HSV_BIT | (uint32_t)0x3CFF6E
#define HSV_RED     HSV_BIT | (uint32_t)0x00FF6E
#define HSV_GREEN   HSV_BIT | (uint32_t)0x55FF6E
#define HSV_CYAN    HSV_BIT | (uint32_t)0x78FF6E
#define HSV_PURPLE  HSV_BIT | (uint32_t)0xD4FF6E
#define HSV_ORANGE  HSV_BIT | (uint32_t)0x14FF6E
#define HSV_OFF     HSV_BIT | (uint32_t)0x000000

#define RGB_WHITE       (uint32_t)0xFFFFFF
#define RGB_BLUE        (uint32_t)0x0000FF
#define RGB_YELLOW      (uint32_t)0xFFFF00
#define RGB_RED         (uint32_t)0xFF0000
#define RGB_GREEN       (uint32_t)0x00FF00
#define RGB_CYAN        (uint32_t)0x00FFFF
#define RGB_PURPLE      (uint32_t)0x9933FF
#define RGB_ORANGE      (uint32_t)0xFF8300
#define RGB_PINK        (uint32_t)0xFF0099
#define RGB_MAGENTA     (uint32_t)0xFF00FF
#define RGB_OFF         (uint32_t)0x000000

#define RGB_WHITE0      (uint32_t)0x101010
#define RGB_WHITE1      (uint32_t)0x1C1C1C
#define RGB_WHITE2      (uint32_t)0x383838
#define RGB_WHITE3      (uint32_t)0x545454
#define RGB_WHITE4      (uint32_t)0x707070
#define RGB_WHITE5      (uint32_t)0x8C8C8C
#define RGB_WHITE6      (uint32_t)0xA8A8A8
#define RGB_WHITE7      (uint32_t)0xC4C4C4
#define RGB_WHITE8      (uint32_t)0xE0E0E0
#define RGB_WHITE9      (uint32_t)0xFCFCFC

#define RGB_BLUE0       (uint32_t)0x000010
#define RGB_BLUE1       (uint32_t)0x00001C
#define RGB_BLUE2       (uint32_t)0x000038
#define RGB_BLUE3       (uint32_t)0x000054
#define RGB_BLUE4       (uint32_t)0x000070
#define RGB_BLUE5       (uint32_t)0x00008C
#define RGB_BLUE6       (uint32_t)0x0000A8
#define RGB_BLUE7       (uint32_t)0x0000C4
#define RGB_BLUE8       (uint32_t)0x0000E0
#define RGB_BLUE9       (uint32_t)0x0000FC

#define RGB_YELLOW0     (uint32_t)0x101000
#define RGB_YELLOW1     (uint32_t)0x1C1C00
#define RGB_YELLOW2     (uint32_t)0x383800
#define RGB_YELLOW3     (uint32_t)0x545400
#define RGB_YELLOW4     (uint32_t)0x707000
#define RGB_YELLOW5     (uint32_t)0x8C8C00
#define RGB_YELLOW6     (uint32_t)0xA8A800
#define RGB_YELLOW7     (uint32_t)0xC4C400
#define RGB_YELLOW8     (uint32_t)0xE0E000
#define RGB_YELLOW9     (uint32_t)0xFCFC00

#define RGB_RED0        (uint32_t)0x100000
#define RGB_RED1        (uint32_t)0x1C0000
#define RGB_RED2        (uint32_t)0x380000
#define RGB_RED3        (uint32_t)0x540000
#define RGB_RED4        (uint32_t)0x700000
#define RGB_RED5        (uint32_t)0x8C0000
#define RGB_RED6        (uint32_t)0xA80000
#define RGB_RED7        (uint32_t)0xC40000
#define RGB_RED8        (uint32_t)0xE00000
#define RGB_RED9        (uint32_t)0xFC0000

#define RGB_GREEN0      (uint32_t)0x001000
#define RGB_GREEN1      (uint32_t)0x001C00
#define RGB_GREEN2      (uint32_t)0x003800
#define RGB_GREEN3      (uint32_t)0x005400
#define RGB_GREEN4      (uint32_t)0x007000
#define RGB_GREEN5      (uint32_t)0x008C00
#define RGB_GREEN6      (uint32_t)0x00A800
#define RGB_GREEN7      (uint32_t)0x00C400
#define RGB_GREEN8      (uint32_t)0x00E000
#define RGB_GREEN9      (uint32_t)0x00FC00

#define RGB_CYAN0       (uint32_t)0x001010
#define RGB_CYAN1       (uint32_t)0x001C1C
#define RGB_CYAN2       (uint32_t)0x003838
#define RGB_CYAN3       (uint32_t)0x005454
#define RGB_CYAN4       (uint32_t)0x007070
#define RGB_CYAN5       (uint32_t)0x008C8C
#define RGB_CYAN6       (uint32_t)0x00A8A8
#define RGB_CYAN7       (uint32_t)0x00C4C4
#define RGB_CYAN8       (uint32_t)0x00E0E0
#define RGB_CYAN9       (uint32_t)0x00FCFC

#define RGB_MAGENTA0    (uint32_t)0x100010
#define RGB_MAGENTA1    (uint32_t)0x1C001C
#define RGB_MAGENTA2    (uint32_t)0x380038
#define RGB_MAGENTA3    (uint32_t)0x540054
#define RGB_MAGENTA4    (uint32_t)0x700070
#define RGB_MAGENTA5    (uint32_t)0x8C008C
#define RGB_MAGENTA6    (uint32_t)0xA800A8
#define RGB_MAGENTA7    (uint32_t)0xC400C4
#define RGB_MAGENTA8    (uint32_t)0xE000E0
#define RGB_MAGENTA9    (uint32_t)0xFC00FC

#define RGB_ORANGE0     (uint32_t)0x100800
#define RGB_ORANGE1     (uint32_t)0x1C0E00
#define RGB_ORANGE2     (uint32_t)0x381C00
#define RGB_ORANGE3     (uint32_t)0x542B00
#define RGB_ORANGE4     (uint32_t)0x703900
#define RGB_ORANGE5     (uint32_t)0x8C4800
#define RGB_ORANGE6     (uint32_t)0xA85600
#define RGB_ORANGE7     (uint32_t)0xC46500
#define RGB_ORANGE8     (uint32_t)0xE07300
#define RGB_ORANGE9     (uint32_t)0xFC8200

#endif
