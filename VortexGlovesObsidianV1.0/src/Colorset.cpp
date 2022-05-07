#include "Colorset.h"

#include <Arduino.h>
#include <cstring>

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
  m_palette[0] = c1; if (!c1.empty()) m_numColors++;
  m_palette[1] = c2; if (!c2.empty()) m_numColors++;
  m_palette[2] = c3; if (!c3.empty()) m_numColors++;
  m_palette[3] = c4; if (!c4.empty()) m_numColors++;
  m_palette[4] = c5; if (!c5.empty()) m_numColors++;
  m_palette[5] = c6; if (!c6.empty()) m_numColors++;
  m_palette[6] = c7; if (!c7.empty()) m_numColors++;
  m_palette[7] = c8; if (!c8.empty()) m_numColors++;
}

Colorset::Colorset(const Colorset &other) :
  m_curIndex(UINT32_MAX),
  m_numColors(other.m_numColors)
{
  for (int i = 0; i < NUM_COLOR_SLOTS; ++i) {
    m_palette[i] = other.m_palette[i];
  }
}

void Colorset::init()
{
  m_curIndex = UINT32_MAX;
}

void Colorset::operator=(const Colorset &other)
{
  m_curIndex = UINT32_MAX;
  m_numColors = other.m_numColors;
  for (int i = 0; i < NUM_COLOR_SLOTS; ++i) {
    m_palette[i] = other.m_palette[i];
  }
}

bool Colorset::operator==(const Colorset &other)
{
  // only compare the palettes for equality
  return (memcmp(m_palette, other.m_palette, sizeof(m_palette)) == 0);
}

bool Colorset::operator!=(const Colorset &other)
{
  return !operator==(other);
}

RGBColor Colorset::operator[](int index) const
{
  return get(index);
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
  // TODO: colorset::add this is ghetto
  if (index > m_numColors) {
    index = m_numColors;
  }
  // warning! converstion from hsv to rgb here
  m_palette[index] = col;
  if (index == m_numColors) {
    m_numColors++;
  }
}

// skip some amount of colors
void Colorset::skip(int32_t amount)
{
  // if the colorset hasn't started yet
  if (m_curIndex == UINT32_MAX) {
    m_curIndex = 0;
  }

  // first modulate the amount to skip to be within +/- the number of colors
  amount %= (int32_t)m_numColors;

  // max = 3
  // m_curIndex = 2
  // amount = -10
  m_curIndex = ((int32_t)m_curIndex + (int32_t)amount) % (int32_t)m_numColors;
  if (m_curIndex > m_numColors) { // must have wrapped
    // simply wrap it back
    m_curIndex += m_numColors;
  }
}

RGBColor Colorset::cur()
{
  if (m_curIndex >= m_numColors) {
    return RGBColor(0, 0, 0);
  }
  if (m_curIndex == UINT32_MAX) {
    return m_palette[0];
  }
  return m_palette[m_curIndex];
}

void Colorset::setCurIndex(uint32_t index)
{
  if (index > (m_numColors - 1)) {
    return;
  }
  m_curIndex = index;
}

RGBColor Colorset::getPrev()
{
  // handle wrapping at 0
  if (m_curIndex == 0) {
    m_curIndex = numColors() - 1;
  } else {
    m_curIndex--;
  }
  // return the color
  return m_palette[m_curIndex];
}

RGBColor Colorset::getNext()
{
  // iterate to next col
  m_curIndex = (m_curIndex + 1) % (numColors());
  // return the color
  return m_palette[m_curIndex];
}


bool Colorset::onStart() const
{
  return (m_curIndex == 0);
}

bool Colorset::onEnd() const
{
  if (!m_numColors) {
    return false;
  }
  return (m_curIndex == m_numColors - 1);
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
