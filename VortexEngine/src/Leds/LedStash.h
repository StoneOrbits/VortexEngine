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

  static void blendStashes(LedStash &dst, const LedStash &cur, const LedStash &other, float t);

  // index operator to access color index
  RGBColor &operator[](int index);
  const RGBColor &operator[](int index) const;

private:
  RGBColor m_ledColorsStash[LED_COUNT];
};

#endif