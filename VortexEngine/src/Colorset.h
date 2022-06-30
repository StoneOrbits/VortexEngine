#ifndef COLORSET_H
#define COLORSET_H

#include "ColorTypes.h"

// the max number of colors in a colorset
#define MAX_COLOR_SLOTS 8

class SerialBuffer;

class Colorset
{
public:
  // empty colorset
  Colorset();
  // constructor for 1-8 color slots
  Colorset(RGBColor c1, RGBColor c2 = RGB_OFF, RGBColor c3 = RGB_OFF,
    RGBColor c4 = RGB_OFF, RGBColor c5 = RGB_OFF, RGBColor c6 = RGB_OFF,
    RGBColor c7 = RGB_OFF, RGBColor c8 = RGB_OFF);
  ~Colorset();

  // move constructor
  Colorset(Colorset &&other) noexcept;

  // copy and assignment operators
  Colorset(const Colorset &other);
  void operator=(const Colorset &other);

  // equality operators
  bool operator==(const Colorset &other) const;
  bool operator!=(const Colorset &other) const;

  // initialize the colorset
  void init();

  // clear the colorset
  void clear();

  // pointer comparison
  bool equals(const Colorset *set) const;

  // index operator to access color index
  RGBColor operator[](int index) const;

  // add a single color
  bool addColor(RGBColor col);
  bool addColorByHue(uint8_t hue);
  bool addColorByHueRandV(uint8_t hue);
  bool addColorByHueRandSV(uint8_t hue);
  void removeColor(uint32_t index);

  // randomize a colorset with a specific number of colors with 
  // various different randomization techniques
  void randomize(uint32_t numColors = 0);
  void randomizeColorTheory(uint32_t numColors = 0);
  void randomizeMonochromatic(uint32_t numColors = 0);

  // these randomizers have a set amount of colors and don't take any arguments
  void randomizeDoubleSplitComplimentary();
  void randomizeTetradic();

  // randomize a colorset with N evenly spaced colors
  void randomizeEvenlySpaced(uint32_t spaces = 0);

  // wrappers for various spacings
  void randomizeSolid() { randomizeEvenlySpaced(1); }
  void randomizeComplimentary() { randomizeEvenlySpaced(2); }
  void randomizeTriadic() { randomizeEvenlySpaced(3); }
  void randomizeSquare() { randomizeEvenlySpaced(4); }
  void randomizePentadic() { randomizeEvenlySpaced(5); }
  void randomizeRainbow() { randomizeEvenlySpaced(8); }

  // get a color from the colorset
  RGBColor get(uint32_t index = 0) const;

  // set an rgb color in a slot, or add a new color if you specify
  // a slot higher than the number of colors in the colorset
  void set(uint32_t index, RGBColor col);

  // skip some amount of colors
  void skip(int32_t amount = 1);

  // get current color in cycle
  RGBColor cur();

  // set the current index of the colorset
  void setCurIndex(uint8_t index);

  // the current index
  uint32_t curIndex() const { return m_curIndex; }

  // get the prev color in cycle
  RGBColor getPrev();

  // get the next color in cycle
  RGBColor getNext();

  // peek at the next color but don't iterate
  RGBColor peekNext() const;

  // the number of colors in the palette
  uint32_t numColors() const { return m_numColors; }

  // whether the colorset is currently on the first color or last color
  bool onStart() const;
  bool onEnd() const;

  // serialize the colorset to save/load
  void serialize(SerialBuffer &buffer) const;
  void unserialize(SerialBuffer &buffer);

private:
  // pre-allocate the palette
  void initPalette(uint32_t numColors);

  // palette of colors
  RGBColor *m_palette;
  // the current index, starts at UINT8_MAX so that
  // the very first call to getNext will iterate to 0
  uint8_t m_curIndex;
  // the actual number of colors in the set
  uint8_t m_numColors;
};

#endif
