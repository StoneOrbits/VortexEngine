#include "ColorTypes.h"
#include "../Serial/SerialBuffer.h"

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
  unsigned char region, remainder, p, q, t;

  if (rhs.sat == 0) {
    red = rhs.val;
    green = rhs.val;
    blue = rhs.val;
    return *this;
  }

  region = rhs.hue / 43;
  remainder = (rhs.hue - (region * 43)) * 6;

  p = (rhs.val * (255 - rhs.sat)) >> 8;
  q = (rhs.val * (255 - ((rhs.sat * remainder) >> 8))) >> 8;
  t = (rhs.val * (255 - ((rhs.sat * (255 - remainder)) >> 8))) >> 8;

  switch (region) {
  case 0:
    red = rhs.val; green = t; blue = p;
    break;
  case 1:
    red = q; green = rhs.val; blue = p;
    break;
  case 2:
    red = p; green = rhs.val; blue = t;
    break;
  case 3:
    red = p; green = q; blue = rhs.val;
    break;
  case 4:
    red = t; green = p; blue = rhs.val;
    break;
  default:
    red = rhs.val; green = p; blue = q;
    break;
  }
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

