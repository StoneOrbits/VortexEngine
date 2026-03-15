#include "LedStash.h"

#include "../VortexEngine.h"

LedStash::LedStash(VortexEngine &engine) :
  m_engine(engine),
  m_ledColorsStash()
{
  m_ledColorsStash.resize(m_engine.leds().ledCount());
}

void LedStash::setIndex(LedPos pos, RGBColor col)
{
  if (pos > LED_LAST) {
    return;
  }
  m_ledColorsStash[pos] = col;
}

void LedStash::clear()
{
  for (LedPos pos = LED_FIRST; pos < LED_LAST; ++pos) {
    m_ledColorsStash[pos].clear();
  }
}

// index operator to access color index
RGBColor &LedStash::operator[](int index)
{
  return m_ledColorsStash[index];
}

const RGBColor &LedStash::operator[](int index) const
{
  return m_ledColorsStash[index];
}

LedStash::~LedStash()
{
}

void LedStash::blendStashes(VortexEngine &engine, LedStash &dst, const LedStash &cur, const LedStash &other, float t)
{
  if (t <= 0.0f) {
    for (int i = 0; i < engine.leds().ledCount(); ++i)
      dst[i] = cur[i];
    return;
  }

  if (t >= 1.0f) {
    for (int i = 0; i < engine.leds().ledCount(); ++i)
      dst[i] = other[i];
    return;
  }

  float inv = 1.0f - t;

  for (int i = 0; i < engine.leds().ledCount(); ++i) {
    const RGBColor &a = cur.m_ledColorsStash[i];
    const RGBColor &b = other.m_ledColorsStash[i];

    RGBColor &d = dst.m_ledColorsStash[i];

    d.red = (uint8_t)((a.red * inv) + (b.red * t));
    d.green = (uint8_t)((a.green * inv) + (b.green * t));
    d.blue = (uint8_t)((a.blue * inv) + (b.blue * t));
  }
}

void LedStash::addStashes(VortexEngine &engine, LedStash &dst, const LedStash &cur, const LedStash &other)
{
  for (int i = 0; i < engine.leds().ledCount(); ++i) {
    const RGBColor &a = cur.m_ledColorsStash[i];
    const RGBColor &b = other.m_ledColorsStash[i];

    RGBColor &d = dst.m_ledColorsStash[i];

    int r = a.red + b.red;
    int g = a.green + b.green;
    int b2 = a.blue + b.blue;

    if (r > 255) r = 255;
    if (g > 255) g = 255;
    if (b2 > 255) b2 = 255;

    d.red = (uint8_t)r;
    d.green = (uint8_t)g;
    d.blue = (uint8_t)b2;
  }
}
