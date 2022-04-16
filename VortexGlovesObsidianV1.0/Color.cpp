#include "Color.h"


HSVColor::HSVColor() :
  hue(0), sat(0), val(0)
{
}

HSVColor::HSVColor(uint8_t hue, uint8_t sat, uint8_t val) :
  hue(hue), sat(sat), val(val)
{
}
