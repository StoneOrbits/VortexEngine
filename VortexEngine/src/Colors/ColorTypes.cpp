#include "ColorTypes.h"

#include "../Serial/ByteStream.h"

#include <Arduino.h>

HSVColor::HSVColor() :
  raw()
{
}

HSVColor::HSVColor(uint8_t hue, uint8_t sat, uint8_t val) :
  hue(hue), sat(sat), val(val)
{
}

HSVColor::HSVColor(uint32_t dwVal) :
  HSVColor()
{
  *this = dwVal;
}

// assignment from uint32_t
HSVColor &HSVColor::operator=(const uint32_t &rhs)
{
  // check for the HSV bit!
  if (!(rhs & HSV_BIT)) {
    // IT'S NOT AN HSV DWORD!!
    *this = RGBColor(rhs);
    return *this;
  }
  hue = ((rhs >> 16) & 0xFF);
  sat = ((rhs >> 8) & 0xFF);
  val = (rhs & 0xFF);
  return *this;
}

// copy construction
HSVColor::HSVColor(const HSVColor &rhs)
{
  hue = rhs.hue;
  sat = rhs.sat;
  val = rhs.val;
}

// assignment operator
HSVColor &HSVColor::operator=(const HSVColor &rhs)
{
  hue = rhs.hue;
  sat = rhs.sat;
  val = rhs.val;
  return *this;
}

  // construction/assignment from RGB
HSVColor::HSVColor(const RGBColor &rhs)
{
  *this = rhs;
}

HSVColor &HSVColor::operator=(const RGBColor &rhs)
{
  unsigned char rgbMin, rgbMax;
  rgbMin = rhs.red < rhs.green ? (rhs.red < rhs.blue ? rhs.red : rhs.blue) : (rhs.green < rhs.blue ? rhs.green : rhs.blue);
  rgbMax = rhs.red > rhs.green ? (rhs.red > rhs.blue ? rhs.red : rhs.blue) : (rhs.green > rhs.blue ? rhs.green : rhs.blue);

  val = rgbMax;
  if (val == 0) {
    hue = 0;
    sat = 0;
    return *this;
  }

  sat = 255 * (long)(rgbMax - rgbMin) / val;
  if (sat == 0) {
    hue = 0;
    return *this;
  }

  if (rgbMax == rhs.red) {
    hue = 0 + 43 * (rhs.green - rhs.blue) / (rgbMax - rgbMin);
  } else if (rgbMax == rhs.green) {
    hue = 85 + 43 * (rhs.blue - rhs.red) / (rgbMax - rgbMin);
  } else {
    hue = 171 + 43 * (rhs.red - rhs.green) / (rgbMax - rgbMin);
  }

  return *this;
}

bool HSVColor::empty() const
{
  return !hue && !sat && !val;
}

void HSVColor::clear()
{
  hue = 0;
  sat = 0;
  val = 0;
}

// ==========
//  RGBColor

RGBColor::RGBColor() :
  raw()
{
}

RGBColor::RGBColor(uint8_t red, uint8_t green, uint8_t blue) :
  red(red), green(green), blue(blue)
{
}

RGBColor::RGBColor(uint32_t dwVal) :
  RGBColor()
{
  *this = dwVal;
}

// assignment from uint32_t
RGBColor &RGBColor::operator=(const uint32_t &rhs)
{
  // check for the HSV bit!
  if ((rhs & HSV_BIT) != 0) {
    // IT'S NOT AN RGB DWORD!!
    *this = HSVColor(rhs);
    return *this;
  }

  red = ((rhs >> 16) & 0xFF);
  green = ((rhs >> 8) & 0xFF);
  blue = (rhs & 0xFF);
  return *this;
}

// copy construction
RGBColor::RGBColor(const RGBColor &rhs)
{
  red = rhs.red;
  green = rhs.green;
  blue = rhs.blue;
}

// assignment operator
RGBColor &RGBColor::operator=(const RGBColor &rhs)
{
  red = rhs.red;
  green = rhs.green;
  blue = rhs.blue;
  return *this;
}

RGBColor::RGBColor(const HSVColor &rhs)
{
  *this = rhs;
}

RGBColor &RGBColor::operator=(const HSVColor &rhs)
{
#if HSV_TO_RGB_ALGORITHM == 1
  *this = hsv_to_rgb_rainbow(rhs);
#elif HSV_TO_RGB_ALGORITHM == 2
  *this = hsv_to_rgb_raw_C(rhs);
#elif HSV_TO_RGB_ALGORITHM == 3
  *this = hsv_to_rgb_generic(rhs);
#endif
  return *this;
}

bool RGBColor::empty() const
{
  return !red && !green && !blue;
}

void RGBColor::clear()
{
  red = 0;
  green = 0;
  blue = 0;
}

void RGBColor::adjustBrightness(uint8_t fadeBy)
{
  red = (((int)red) * (int)(256 - fadeBy)) >> 8;
  green = (((int)green) * (int)(256 - fadeBy)) >> 8;
  blue = (((int)blue) * (int)(256 - fadeBy)) >> 8;
}

void RGBColor::serialize(ByteStream &buffer) const
{
  buffer.serialize(red);
  buffer.serialize(green);
  buffer.serialize(blue);
}

void RGBColor::unserialize(ByteStream &buffer)
{
  buffer.unserialize(&red);
  buffer.unserialize(&green);
  buffer.unserialize(&blue);
}

// Stolen from FastLED hsv to rgb full rainbox where all colours
// are given equal weight, this makes for-example yellow larger
// best to use this function as it is the legacy choice
#define SCALE8(i, scale)  (((uint16_t)i * (uint16_t)(scale)) >> 8)
RGBColor hsv_to_rgb_rainbow(const HSVColor &rhs)
{
  RGBColor col;
  // Yellow has a higher inherent brightness than
  // any other color; 'pure' yellow is perceived to
  // be 93% as bright as white.  In order to make
  // yellow appear the correct relative brightness,
  // it has to be rendered brighter than all other
  // colors.
  // Level Y1 is a moderate boost, the default.
  // Level Y2 is a strong boost.
  const uint8_t Y1 = 1;
  const uint8_t Y2 = 0;

  // G2: Whether to divide all greens by two.
  // Depends GREATLY on your particular LEDs
  const uint8_t G2 = 0;

  // Gscale: what to scale green down by.
  // Depends GREATLY on your particular LEDs
  const uint8_t Gscale = 0;


  uint8_t hue = rhs.hue;
  uint8_t sat = rhs.sat;
  uint8_t val = rhs.val;

  uint8_t offset = hue & 0x1F; // 0..31

  // offset8 = offset * 8
  uint8_t offset8 = offset;
  offset8 <<= 3;

  uint8_t third = SCALE8(offset8, (256 / 3)); // max = 85
  uint8_t r, g, b;
  if (!(hue & 0x80)) {
    // 0XX
    if (!(hue & 0x40)) {
      // 00X
      //section 0-1
      if (!(hue & 0x20)) {
        // 000
        //case 0: // R -> O
        r = 255 - third;
        g = third;
        b = 0;
      } else {
        // 001
        //case 1: // O -> Y
        if (Y1) {
          r = 171;
          g = 85 + third;
          b = 0;
        }
        if (Y2) {
          r = 170 + third;
          //uint8_t twothirds = (third << 1);
          uint8_t twothirds = SCALE8(offset8, ((256 * 2) / 3)); // max=170
          g = 85 + twothirds;
          b = 0;
        }
      }
    } else {
      //01X
      // section 2-3
      if (!(hue & 0x20)) {
        // 010
        //case 2: // Y -> G
        if (Y1) {
          //uint8_t twothirds = (third << 1);
          uint8_t twothirds = SCALE8(offset8, ((256 * 2) / 3)); // max=170
          r = 171 - twothirds;
          g = 170 + third;
          b = 0;
        }
        if (Y2) {
          r = 255 - offset8;
          g = 255;
          b = 0;
        }
      } else {
        // 011
        // case 3: // G -> A
        r = 0;
        g = 255 - third;
        b = third;
      }
    }
  } else {
    // section 4-7
    // 1XX
    if (!(hue & 0x40)) {
      // 10X
      if (!(hue & 0x20)) {
        // 100
        //case 4: // A -> B
        r = 0;
        //uint8_t twothirds = (third << 1);
        uint8_t twothirds = SCALE8(offset8, ((256 * 2) / 3)); // max=170
        g = 171 - twothirds; //170?
        b = 85 + twothirds;
      } else {
        // 101
        //case 5: // B -> P
        r = third;
        g = 0;
        b = 255 - third;
      }
    } else {
      if (!(hue & 0x20)) {
        // 110
        //case 6: // P -- K
        r = 85 + third;
        g = 0;
        b = 171 - third;
      } else {
        // 111
        //case 7: // K -> R
        r = 170 + third;
        g = 0;
        b = 85 - third;
      }
    }
  }

  // This is one of the good places to scale the green down,
  // although the client can scale green down as well.
  if (G2) g = g >> 1;
  if (Gscale) g = SCALE8(g, Gscale);

  // Scale down colors if we're desaturated at all
  // and add the brightness_floor to r, g, and b.
  if (sat != 255) {
    if (sat == 0) {
      r = 255; b = 255; g = 255;
    } else {
      if (r) r = SCALE8(r, sat) + 1;
      if (g) g = SCALE8(g, sat) + 1;
      if (b) b = SCALE8(b, sat) + 1;

      uint8_t desat = 255 - sat;
      desat = SCALE8(desat, desat);

      uint8_t brightness_floor = desat;
      r += brightness_floor;
      g += brightness_floor;
      b += brightness_floor;
    }
  }

  // Now scale everything down if we're at value < 255.
  if (val != 255) {
    val = SCALE8(val, val);
    if (val == 0) {
      r = 0; g = 0; b = 0;
    } else {
      // nSCALE8x3_video( r, g, b, val);
      if (r) r = SCALE8(r, val) + 1;
      if (g) g = SCALE8(g, val) + 1;
      if (b) b = SCALE8(b, val) + 1;
    }
  }

  // Here we have the old AVR "missing std X+n" problem again
  // It turns out that fixing it winds up costing more than
  // not fixing it.
  // To paraphrase Dr Bronner, profile! profile! profile!
  col.red = r;
  col.green = g;
  col.blue = b;
  return col;
}

// stolen from FastLED raw hsv to rgb conversion, idk what is special
// about this one but I think the hue stops at 190 maybe?
RGBColor hsv_to_rgb_raw_C(const HSVColor &rhs)
{
  RGBColor col;
  // Convert hue, saturation and brightness ( HSV/HSB ) to RGB
  // "Dimming" is used on saturation and brightness to make
  // the output more visually linear.

  // Apply dimming curves
  uint8_t value = rhs.val;
  uint8_t saturation = rhs.sat;

  // The brightness floor is minimum number that all of
  // R, G, and B will be set to.
  uint8_t invsat = 255 - saturation;
  uint8_t brightness_floor = (value * invsat) / 256;

  // The color amplitude is the maximum amount of R, G, and B
  // that will be added on top of the brightness_floor to
  // create the specific hue desired.
  uint8_t color_amplitude = value - brightness_floor;

  // Figure out which section of the hue wheel we're in,
  // and how far offset we are withing that section
  uint8_t section = rhs.hue / 0x40; // 0..2
  uint8_t offset = rhs.hue % 0x40;  // 0..63

  uint8_t rampup = offset; // 0..63
  uint8_t rampdown = (0x40 - 1) - offset; // 63..0

  // We now scale rampup and rampdown to a 0-255 range -- at least
  // in theory, but here's where architecture-specific decsions
  // come in to play:
  // To scale them up to 0-255, we'd want to multiply by 4.
  // But in the very next step, we multiply the ramps by other
  // values and then divide the resulting product by 256.
  // So which is faster?
  //   ((ramp * 4) * othervalue) / 256
  // or
  //   ((ramp    ) * othervalue) /  64
  // It depends on your processor architecture.
  // On 8-bit AVR, the "/ 256" is just a one-cycle register move,
  // but the "/ 64" might be a multicycle shift process. So on AVR
  // it's faster do multiply the ramp values by four, and then
  // divide by 256.
  // On ARM, the "/ 256" and "/ 64" are one cycle each, so it's
  // faster to NOT multiply the ramp values by four, and just to
  // divide the resulting product by 64 (instead of 256).
  // Moral of the story: trust your profiler, not your insticts.

  // Since there's an AVR assembly version elsewhere, we'll
  // assume what we're on an architecture where any number of
  // bit shifts has roughly the same cost, and we'll remove the
  // redundant math at the source level:

  //  // scale up to 255 range
  //  //rampup *= 4; // 0..252
  //  //rampdown *= 4; // 0..252

  // compute color-amplitude-scaled-down versions of rampup and rampdown
  uint8_t rampup_amp_adj = (rampup * color_amplitude) / (256 / 4);
  uint8_t rampdown_amp_adj = (rampdown * color_amplitude) / (256 / 4);

  // add brightness_floor offset to everything
  uint8_t rampup_adj_with_floor = rampup_amp_adj + brightness_floor;
  uint8_t rampdown_adj_with_floor = rampdown_amp_adj + brightness_floor;

  if (section) {
    if (section == 1) {
      // section 1: 0x40..0x7F
      col.red = brightness_floor;
      col.green = rampdown_adj_with_floor;
      col.blue = rampup_adj_with_floor;
    } else {
      // section 2; 0x80..0xBF
      col.red = rampup_adj_with_floor;
      col.green = brightness_floor;
      col.blue = rampdown_adj_with_floor;
    }
  } else {
    // section 0: 0x00..0x3F
    col.red = rampdown_adj_with_floor;
    col.green = rampup_adj_with_floor;
    col.blue = brightness_floor;
  }
  return col;
}

// generic hsv to rgb conversion nothing special
RGBColor hsv_to_rgb_generic(const HSVColor &rhs)
{
  unsigned char region, remainder, p, q, t;
  RGBColor col;

  if (rhs.sat == 0) {
    col.red = rhs.val;
    col.green = rhs.val;
    col.blue = rhs.val;
    return col;
  }

  region = rhs.hue / 43;
  remainder = (rhs.hue - (region * 43)) * 6;

  p = (rhs.val * (255 - rhs.sat)) >> 8;
  q = (rhs.val * (255 - ((rhs.sat * remainder) >> 8))) >> 8;
  t = (rhs.val * (255 - ((rhs.sat * (255 - remainder)) >> 8))) >> 8;

  switch (region) {
  case 0:
    col.red = rhs.val; col.green = t; col.blue = p;
    break;
  case 1:
    col.red = q; col.green = rhs.val; col.blue = p;
    break;
  case 2:
    col.red = p; col.green = rhs.val; col.blue = t;
    break;
  case 3:
    col.red = p; col.green = q; col.blue = rhs.val;
    break;
  case 4:
    col.red = t; col.green = p; col.blue = rhs.val;
    break;
  default:
    col.red = rhs.val; col.green = p; col.blue = q;
    break;
  }
  return col;
}
