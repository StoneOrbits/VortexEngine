#include "LedStash.h"

LedStash::LedStash() :
  m_ledColorsStash() 
{
}

void LedStash::clear()
{
  for (LedPos pos = LED_FIRST; pos < LED_LAST; ++pos) {
    m_ledColorsStash[pos].clear();
  }
}

LedStash::~LedStash()
{
}