#include "Colorset.h"

Colorset::Colorset() :
  m_palette(),
  m_numColors(0)
{
}

// create a new colorset with a list of colors
bool Colorset::init(uint32_t amount, RGBColor *colors)
{
  return true;
}

// get a color from the colorset
RGBColor get(uint32_t index)
{
  if (index >= m_palette.size()) {
    return RGBColor(0, 0, 0);
  }
  return m_palette[index];
}
