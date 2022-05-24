#ifndef COLORSET_H
#define COLORSET_H

#include "ColorTypes.h"

// the number of colors in a colorset
#define NUM_COLOR_SLOTS 8

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

  // copy and assignment operators
  Colorset(const Colorset &other);
  void operator=(const Colorset &other);

  // equality operators
  bool operator==(const Colorset &other);
  bool operator!=(const Colorset &other);

  // initialize the colorset
  void init();

  void clear();

  // index operator to access color index
  RGBColor operator[](int index) const;

  // add a single color
  bool addColor(RGBColor col);
  void removeColor(uint32_t index);

  // get a color from the colorset
  RGBColor get(uint32_t index = 0) const;

  // set an rgb color at in a slot
  void set(uint32_t index, RGBColor col);
  // set an hsv color in a slot (expensive)
  void set(uint32_t index, HSVColor col);

  // skip some amount of colors
  void skip(int32_t amount = 1);

  // get current color in cycle
  RGBColor cur();

  // set the current index of the colorset
  void setCurIndex(uint32_t index);

  // the current index
  uint32_t curIndex() const { return m_curIndex; }

  // get the prev color in cycle
  RGBColor getPrev();

  // get the next color in cycle
  RGBColor getNext();

  // the number of colors in the palette
  uint32_t numColors() const { return m_numColors; }

  // whether the colorset is currently on the first color or last color
  bool onStart() const;
  bool onEnd() const;

  // serialize the colorset to save/load
  void serialize(SerialBuffer &buffer) const;
  void unserialize(SerialBuffer &buffer);

private:
  // the current index, starts at UINT32_MAX so that
  // the very first call to getNext will iterate to 0
  uint32_t m_curIndex;
  // the actual number of colors in the set
  uint32_t m_numColors;
  // palette of colors
  RGBColor m_palette[NUM_COLOR_SLOTS];
};

#endif
