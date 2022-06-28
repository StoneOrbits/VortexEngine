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
  Colorset()
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

Colorset::Colorset(Colorset &&other) noexcept :
  m_palette(other.m_palette),
  m_curIndex(INDEX_NONE),
  m_numColors(other.m_numColors)
{
  other.m_palette = nullptr;
  other.m_numColors = 0;
  other.m_curIndex = INDEX_NONE;
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
    delete[] m_palette;
    m_palette = nullptr;
  }
  m_numColors = 0;
  init();
}

bool Colorset::equals(const Colorset *set) const
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
  // allocate a new palette one larger than before
  RGBColor *temp = new RGBColor[m_numColors + 1];
  if (!temp) {
    return false;
  }
  // if there is already some colors in the palette
  if (m_numColors && m_palette) {
    // copy over existing colors
    for (uint32_t i = 0; i < m_numColors; ++i) { 
      temp[i] = m_palette[i];
    }
    // and delete the existing palette
    delete[] m_palette;
  }
  // reassign new palette
  m_palette = temp;
  // insert new color and increment number of colors
  m_palette[m_numColors] = col;
  m_numColors++;
  return true;
}

// add a single color with maximum hue and staturation
bool Colorset::addColorByHue(uint8_t hue)
{
  return addColor(HSVColor(hue, 255, 255));
}

bool Colorset::addColorByHueRandV(uint8_t hue)
{
  return addColor(HSVColor(hue, 255, 85 * random(1, 4)));
}

bool Colorset::addColorByHueRandSV(uint8_t hue)
{
  return addColor(HSVColor(hue, random(0, 256), random(0, 256)));
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
    delete[] m_palette;
    m_palette = nullptr;
  }
}

// create a set of truely random colors
void Colorset::randomize(uint32_t numColors)
{
  clear();
  if (!numColors) {
    numColors = random(2, 9);
  }
  for (uint32_t i = 0; i < numColors; ++i) {
    addColorByHueRandSV(random(0, 256));
  }
}

// create a set according to the rules of color theory
void Colorset::randomizeColorTheory(uint32_t numColors)
{
  clear();
  if (!numColors) {
    numColors = random(1, 9);
  }
  uint8_t randomizedHue = random(0, 256);
  uint8_t colorGap = 0;
  if (numColors > 1) colorGap = random(16, 256/(numColors - 1));
  for (uint32_t i = 0; i < numColors; i++) {
    addColorByHueRandV((randomizedHue + (i * colorGap)) % 256);
  }
}

// create a set of colors that share a single hue
void Colorset::randomizeMonochromatic(uint32_t numColors)
{
  clear();
  if (!numColors) {
    numColors = random(2, 9);
  }
  uint8_t randomizedHue = random(0, 256);
  addColorByHue(randomizedHue);
  for (uint32_t i = 1; i < numColors; i++) {
    addColorByHueRandSV(randomizedHue);
  }
}

// create a set of 5 colors with 2 pairs of opposing colors with the same spacing from the central color
void Colorset::randomizeDoubleSplitComplimentary()
{
  clear();
  uint8_t randomizedHue = random(0, 256);
  uint8_t splitComplimentaryGap = random(1, 64);
  addColorByHueRandV((randomizedHue + splitComplimentaryGap + 128) % 256);
  addColorByHueRandV((randomizedHue - splitComplimentaryGap) % 256);
  addColorByHueRandV(randomizedHue);
  addColorByHueRandV((randomizedHue + splitComplimentaryGap) % 256);
  addColorByHueRandV((randomizedHue - splitComplimentaryGap + 128) % 256);
}

// create a set of 2 pairs of oposing colors
void Colorset::randomizeTetradic()
{
  clear();
  uint8_t randomizedHue = random(0, 256);
  uint8_t randomizedHue2 = random(0, 256);
  addColorByHueRandV(randomizedHue);
  addColorByHueRandV(randomizedHue2);
  addColorByHueRandV((randomizedHue + 128) % 256);
  addColorByHueRandV((randomizedHue2 + 128) % 256);
}

void Colorset::randomizeEvenlySpaced(uint32_t spaces)
{
  clear();
  if (!spaces) {
    spaces = random(1, 9);
  }
  uint8_t randomizedHue = random(0, 256);
  for (uint32_t i = 0; i < spaces; i++) {
    addColorByHueRandV((randomizedHue + (256 / spaces) * i) % 256);
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

// set an rgb color in a slot, or add a new color if you specify
// a slot higher than the number of colors in the colorset
void Colorset::set(uint32_t index, RGBColor col)
{
  // special case for 'setting' a color at the edge of the palette,
  // ie adding a new color when you set an index higher than the max
  if (index >= m_numColors) {
    if (!addColor(col)) {
      ERROR_LOGF("Failed to add new color at index %u", index);
    }
    return;
  }
  if (!m_palette) {
    // should be impossible because if the index is less than
    // the number of colors then there must be non-zero number
    // of colors which means the palette should be initialized
    ERROR_LOGF("Programmer error setting color index %u with no palette", index);
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

// peek at the next color but don't iterate
RGBColor Colorset::peekNext() const
{
  if (!m_numColors || !m_palette) {
    return RGB_OFF;
  }
  // get index of the next color
  uint32_t nextIndex = (m_curIndex + 1) % numColors();
  // return the color
  return m_palette[nextIndex];
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
    delete[] m_palette;
  }
  //m_palette = (RGBColor *)vcalloc(numColors, sizeof(RGBColor));
  m_palette = new RGBColor[numColors];
  if (!m_palette) {
    ERROR_OUT_OF_MEMORY();
    return;
  }
  m_numColors = numColors;
}

