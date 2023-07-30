#include "LedStash.h"

LedStash::LedStash() :
  m_ledColorsStash()
{
#if FIXED_LED_COUNT == 0
  m_ledColorsStash.resize(LED_COUNT);
#endif
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

LedStash::~LedStash()
{
}
