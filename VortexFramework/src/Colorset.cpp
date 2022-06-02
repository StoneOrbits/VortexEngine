#include "Colorset.h"

#include "SerialBuffer.h"
#include "Memory.h"

#include "Log.h"

#include <Arduino.h>
#include <cstring>

#define INDEX_NONE UINT8_MAX

Colorset::Colorset() :
  m_palette(nullptr),
  m_curIndex(INDEX_NONE),
  m_numColors(0)
{
  init();
}

Colorset::Colorset(RGBColor c1, RGBColor c2, RGBColor c3, RGBColor c4,
  RGBColor c5, RGBColor c6, RGBColor c7, RGBColor c8) :
  Colorset()
{
  // would be nice if we could do this another way
  if (!c1.empty()) addColor(c1);
  if (!c2.empty()) addColor(c2);
  if (!c3.empty()) addColor(c3);
  if (!c4.empty()) addColor(c4);
  if (!c5.empty()) addColor(c5);
  if (!c6.empty()) addColor(c6);
  if (!c7.empty()) addColor(c7);
  if (!c8.empty()) addColor(c8);
  init();
}

Colorset::Colorset(const Colorset &other) :
  m_curIndex(INDEX_NONE),
  m_numColors(other.m_numColors)
{
  clear();
  initPalette(other.m_numColors);
  for (uint32_t i = 0; i < other.m_numColors; ++i) {
    m_palette[i] = other.m_palette[i];
  }
  init();
}

Colorset::~Colorset()
{
  clear();
}

void Colorset::operator=(const Colorset &other)
{
  clear();
  initPalette(other.m_numColors);
  for (uint32_t i = 0; i < other.numColors(); ++i) {
    m_palette[i] = other.m_palette[i];
  }
  init();
}

bool Colorset::operator==(const Colorset &other)
{
  // only compare the palettes for equality
  return (m_numColors == other.m_numColors) && 
         (memcmp(m_palette, other.m_palette, m_numColors * sizeof(RGBColor)) == 0);
}

bool Colorset::operator!=(const Colorset &other)
{
  return !operator==(other);
}

void Colorset::init()
{
  m_curIndex = INDEX_NONE;
}

void Colorset::clear()
{
  if (m_palette) {
    vfree(m_palette);
    m_palette = nullptr;
  }
  m_numColors = 0;
  init();
}

bool Colorset::equals(const Colorset *set)
{
  if (!set) {
    return false;
  }
  if (set->m_numColors != m_numColors) {
    return false;
  }
  return (memcmp(m_palette, set->m_palette, m_numColors * sizeof(RGBColor)) == 0);
}

RGBColor Colorset::operator[](int index) const
{
  return get(index);
}

// add a single color
bool Colorset::addColor(RGBColor col)
{
  if (m_numColors >= MAX_COLOR_SLOTS) {
    return false;
  }
  void *temp = vrealloc(m_palette, sizeof(RGBColor) * (m_numColors + 1));
  if (!temp) {
    return false;
  }
  m_palette = (RGBColor *)temp;
  m_palette[m_numColors++] = col;
  return true;
}

void Colorset::removeColor(uint32_t index)
{
  if (index >= m_numColors) {
    return;
  }
  for (uint8_t i = index; i < (m_numColors - 1); ++i) {
    m_palette[i] = m_palette[i + 1];
  }
  m_palette[--m_numColors].clear();
  if (!m_numColors) {
    vfree(m_palette);
    m_palette = nullptr;
  }
}

void Colorset::randomize(uint32_t numColors)
{
  clear();
  if (!numColors) {
    numColors = random(2, 6);
  }
  for (uint32_t i = 0; i < numColors; ++i) {
    addColor(RGBColor((uint8_t)random(0, 255),
                      (uint8_t)random(0, 255),
                      (uint8_t)random(0, 255)));
  }
}

// get a color from the colorset
RGBColor Colorset::get(uint32_t index) const
{
  if (index >= m_numColors || !m_palette) {
    return RGBColor(0, 0, 0);
  }
  return m_palette[index];
}

// set an rgb color in a slot
void Colorset::set(uint32_t index, RGBColor col)
{
  if (index >= m_numColors || !m_palette) {
    return;
  }
  m_palette[index] = col;
}

// skip some amount of colors
void Colorset::skip(int32_t amount)
{
  if (!m_numColors || !m_palette) {
    return;
  }
  // if the colorset hasn't started yet
  if (m_curIndex == INDEX_NONE) {
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
  if (m_curIndex >= m_numColors || !m_palette) {
    return RGBColor(0, 0, 0);
  }
  if (m_curIndex == INDEX_NONE) {
    return m_palette[0];
  }
  return m_palette[m_curIndex];
}

void Colorset::setCurIndex(uint8_t index)
{
  if (!m_numColors) {
    return;
  }
  if (index > (m_numColors - 1)) {
    return;
  }
  m_curIndex = index;
}

RGBColor Colorset::getPrev()
{
  if (!m_numColors || !m_palette) {
    return RGB_OFF;
  }
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
  if (!m_numColors || !m_palette) {
    return RGB_OFF;
  }
  // iterate current index, let it wrap at max uint8
  m_curIndex++;
  // then modulate the result within max colors
  m_curIndex %= numColors();
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

void Colorset::serialize(SerialBuffer &buffer) const
{
  buffer.serialize(m_numColors);
  for (uint32_t i = 0; i < m_numColors; ++i) {
    m_palette[i].serialize(buffer);
  }
}

void Colorset::unserialize(SerialBuffer &buffer)
{
  buffer.unserialize(&m_numColors);
  initPalette(m_numColors);
  for (uint32_t i = 0; i < m_numColors; ++i) {
    m_palette[i].unserialize(buffer);
  }
}

void Colorset::initPalette(uint32_t numColors)
{
  if (m_palette) {
    vfree(m_palette);
  }
  m_palette = (RGBColor *)vcalloc(numColors, sizeof(RGBColor));
  if (!m_palette) {
    ERROR_OUT_OF_MEMORY();
    return;
  }
  m_numColors = numColors;
}

