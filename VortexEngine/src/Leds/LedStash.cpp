#include "LedStash.h"

LedStash::LedStash() :
  m_ledColorsStash()
{
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

// index operator to access color index
const RGBColor &LedStash::operator[](int index) const
{
  return m_ledColorsStash[index];
}

LedStash::~LedStash()
{
}

void LedStash::blendStashes(LedStash &dst, const LedStash &cur, const LedStash &other, float t)
{
  if (t <= 0.0f) {
    for (int i = 0; i < LED_COUNT; ++i)
      dst[i] = cur[i];
    return;
  }

  if (t >= 1.0f) {
    for (int i = 0; i < LED_COUNT; ++i)
      dst[i] = other[i];
    return;
  }

  float inv = 1.0f - t;

  for (int i = 0; i < LED_COUNT; ++i) {
    const RGBColor &a = cur.m_ledColorsStash[i];
    const RGBColor &b = other.m_ledColorsStash[i];

    RGBColor &d = dst.m_ledColorsStash[i];

    d.red = (uint8_t)((a.red * inv) + (b.red * t));
    d.green = (uint8_t)((a.green * inv) + (b.green * t));
    d.blue = (uint8_t)((a.blue * inv) + (b.blue * t));
  }
}
