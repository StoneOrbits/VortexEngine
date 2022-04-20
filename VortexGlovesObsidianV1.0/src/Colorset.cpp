#include "Colorset.h"

Colorset::Colorset() :
  m_palette() {}
Colorset::Colorset(RGBColor c1) :
  m_palette{ c1 } {}
Colorset::Colorset(RGBColor c1, RGBColor c2) :
  m_palette{ c1, c2 } {}
Colorset::Colorset(RGBColor c1, RGBColor c2, RGBColor c3) :
  m_palette{ c1, c2, c3 } {}
Colorset::Colorset(RGBColor c1, RGBColor c2, RGBColor c3, RGBColor c4) :
  m_palette{ c1, c2, c3, c4 } {}
Colorset::Colorset(RGBColor c1, RGBColor c2, RGBColor c3, RGBColor c4, RGBColor c5) :
  m_palette{ c1, c2, c3, c4, c5 } {}
Colorset::Colorset(RGBColor c1, RGBColor c2, RGBColor c3, RGBColor c4, RGBColor c5, RGBColor c6) :
  m_palette{ c1, c2, c3, c4, c5, c6 } {}
Colorset::Colorset(RGBColor c1, RGBColor c2, RGBColor c3, RGBColor c4, RGBColor c5, RGBColor c6, RGBColor c7) :
  m_palette{ c1, c2, c3, c4, c5, c6, c7 } {}
Colorset::Colorset(RGBColor c1, RGBColor c2, RGBColor c3, RGBColor c4, RGBColor c5, RGBColor c6, RGBColor c7, RGBColor c8) :
  m_palette{ c1, c2, c3, c4, c5, c6, c7, c8 } {}

// add a single color
bool Colorset::addColor(RGBColor col)
{
  m_palette.push_back(col);
  return true;
}

// get a color from the colorset
RGBColor Colorset::get(uint32_t index) const
{
  if (index >= m_palette.size()) {
    return RGBColor(0, 0, 0);
  }
  return m_palette[index];
}

RGBColor Colorset::getNext()
{
    RGBColor rv = m_palette[m_curIndex];
    // iterate to next col
    m_curIndex = (m_curIndex + 1) % numColors();
    // return the color
    return rv;
}
