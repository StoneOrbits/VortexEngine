#include "Colorset.h"

Colorset::Colorset() :
  m_palette(),
  m_numColors(0)
{
}

// create a new colorset with a list of colors
bool Colorset::init(uint32_t amount, CHSV *colors)
{
  return true;
}

// get a color from the colorset
CHSV get(uint32_t index)
{
  return CHSV(0, 0, 0);
}
