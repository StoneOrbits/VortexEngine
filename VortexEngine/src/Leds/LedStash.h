#ifndef LEDSTASH_H
#define LEDSTASH_H

#include "../Colors/ColorTypes.h"
#include "LedTypes.h"

class LedStash
{
  friend class Leds;
public:
  LedStash();
  ~LedStash();

  void setIndex(LedPos pos, RGBColor col);

  void clear();

  // index operator to access color index
  RGBColor &operator[](int index);

private:
  RGBColor m_ledColorsStash[LED_COUNT];
};

#endif