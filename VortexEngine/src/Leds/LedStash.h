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

  void clear();

private:
  RGBColor m_ledColorsStash[LED_COUNT];
};

#endif