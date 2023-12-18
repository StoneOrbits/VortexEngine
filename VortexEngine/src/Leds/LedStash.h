#ifndef LEDSTASH_H
#define LEDSTASH_H

#include "../Colors/ColorTypes.h"
#include "LedTypes.h"

#if FIXED_LED_COUNT == 0
#include <vector>
#endif

class LedStash
{
  friend class Leds;
public:
  LedStash(VortexEngine &engine);
  ~LedStash();

  void setIndex(LedPos pos, RGBColor col);

  void clear();

  // index operator to access color index
  RGBColor &operator[](int index);

private:
  // reference to engine
  VortexEngine &m_engine;
#if FIXED_LED_COUNT == 1
  RGBColor m_ledColorsStash[LED_COUNT];
#else
  std::vector<RGBColor> m_ledColorsStash;
#endif
};

#endif