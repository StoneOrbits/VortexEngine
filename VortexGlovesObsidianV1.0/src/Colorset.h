#ifndef COLORSET_H
#define COLORSET_H

#include "ColorTypes.h"

// the number of colors in a colorset
#define NUM_COLOR_SLOTS 8

class Colorset
{
  public:
    // empty colorset
    Colorset();

    // constructor for 1-8 color slots
    Colorset(RGBColor c1, RGBColor c2 = RGB_OFF, RGBColor c3 = RGB_OFF, 
        RGBColor c4 = RGB_OFF, RGBColor c5 = RGB_OFF, RGBColor c6 = RGB_OFF,
        RGBColor c7 = RGB_OFF, RGBColor c8 = RGB_OFF);

    // add a single color
    bool addColor(RGBColor col);

    // get a color from the colorset
    RGBColor get(uint32_t index = 0) const;

    // set an rgb color at in a slot
    void set(uint32_t index, RGBColor col);
    // set an hsv color in a slot (expensive)
    void set(uint32_t index, HSVColor col);

    // get current color in cycle
    RGBColor cur();

    // get the next color in cycle
    RGBColor getNext();

    // the number of colors in the palette
    uint32_t numColors() const { return m_numColors; }

    // serialize the colorset to save/load
    void serialize() const;
    void unserialize();

  private:
    // the current index
    uint32_t m_curIndex;
    // the actual number of colors in the set
    uint32_t m_numColors;
    // palette of colors
    RGBColor m_palette[NUM_COLOR_SLOTS];
};

#endif
