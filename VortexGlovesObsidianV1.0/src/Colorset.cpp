#include "Colorset.h"

#include <Arduino.h>

Colorset::Colorset() :
  m_curIndex(UINT32_MAX),
  m_numColors(0),
  m_palette()
{
}

Colorset::Colorset(RGBColor c1, RGBColor c2, RGBColor c3, RGBColor c4, 
    RGBColor c5, RGBColor c6, RGBColor c7, RGBColor c8) :
  Colorset()
{
  // would be nice if we could do this another way
  m_palette[0] = c1; if (c1.raw_dword) m_numColors++;
  m_palette[1] = c2; if (c2.raw_dword) m_numColors++;
  m_palette[2] = c3; if (c3.raw_dword) m_numColors++;
  m_palette[3] = c4; if (c4.raw_dword) m_numColors++;
  m_palette[4] = c5; if (c5.raw_dword) m_numColors++;
  m_palette[5] = c6; if (c6.raw_dword) m_numColors++;
  m_palette[6] = c7; if (c7.raw_dword) m_numColors++;
  m_palette[7] = c8; if (c8.raw_dword) m_numColors++;
}

// add a single color
bool Colorset::addColor(RGBColor col)
{
  if (m_numColors >= NUM_COLOR_SLOTS) {
    return false;
  }
  m_palette[m_numColors] = col;
  m_numColors++;
  return true;
}

// get a color from the colorset
RGBColor Colorset::get(uint32_t index) const
{
  if (index >= m_numColors) {
    return RGBColor(0, 0, 0);
  }
  return m_palette[index];
}

// set an rgb color in a slot
void Colorset::set(uint32_t index, RGBColor col)
{
  if (index >= m_numColors) {
    return;
  }
  m_palette[index] = col;
}

// set an hsv color in a slot (expensive)
void Colorset::set(uint32_t index, HSVColor col)
{
  if (index >= m_numColors) {
    return;
  }
  // warning! converstion from hsv to rgb here
  m_palette[index] = col;
}

RGBColor Colorset::cur()
{
  if (m_curIndex == UINT32_MAX) {
    m_curIndex = 0;
  }
  if (m_curIndex >= m_numColors) {
    return RGBColor(0, 0, 0);
  }
  return m_palette[m_curIndex];
}

RGBColor Colorset::getNext()
{
    // iterate to next col
    m_curIndex = (m_curIndex + 1) % numColors();
    // return the color
    return m_palette[m_curIndex];
}

void Colorset::serialize() const
{
  Serial.print(m_numColors);
  for (uint32_t i = 0; i < m_numColors; ++i) {
    m_palette[i].serialize();
  }
}

void Colorset::unserialize()
{
}
