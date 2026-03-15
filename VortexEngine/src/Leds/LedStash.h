#ifndef LEDSTASH_H
#define LEDSTASH_H

#include "../Colors/ColorTypes.h"
#include "LedTypes.h"

#include <vector>

class LedStash
{
  friend class Leds;
public:
  LedStash(VortexEngine &engine);
  ~LedStash();

  void setIndex(LedPos pos, RGBColor col);

  void clear();

  // TODO: make these members and avoid passing first param
  static void blendStashes(VortexEngine &engine, LedStash &dst, const LedStash &cur, const LedStash &other, float t);
  static void addStashes(VortexEngine &engine, LedStash &dst, const LedStash &cur, const LedStash &other);

  // index operator to access color index
  RGBColor &operator[](int index);
  const RGBColor &operator[](int index) const;

private:
  // reference to engine
  VortexEngine &m_engine;
  std::vector<RGBColor> m_ledColorsStash;
};

#endif
