#include "ColorTypes.h"

#include <Arduino.h>

void RGBColor::serialize() const
{
  Serial.print(red);
  Serial.print(green);
  Serial.print(blue);
}

void RGBColor::unserialize()
{
}

RGBColor::RGBColor(const HSVColor &rhs)
{
  unsigned char region, remainder, p, q, t;

  if (rhs.sat == 0) {
    red = rhs.val;
    green = rhs.val;
    blue = rhs.val;
    return;
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
}
