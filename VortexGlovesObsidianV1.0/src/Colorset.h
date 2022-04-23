#ifndef COLORSET_H
#define COLORSET_H

#include <vector>

#include "ColorTypes.h"

class Colorset
{
  public:
    Colorset();
    Colorset(RGBColor c1);
    Colorset(RGBColor c1, RGBColor c2);
    Colorset(RGBColor c1, RGBColor c2, RGBColor c3);
    Colorset(RGBColor c1, RGBColor c2, RGBColor c3, RGBColor c4);
    Colorset(RGBColor c1, RGBColor c2, RGBColor c3, RGBColor c4, 
      RGBColor c5);
    Colorset(RGBColor c1, RGBColor c2, RGBColor c3, RGBColor c4, 
      RGBColor c5, RGBColor c6);
    Colorset(RGBColor c1, RGBColor c2, RGBColor c3, RGBColor c4, 
      RGBColor c5, RGBColor c6, RGBColor c7);
    Colorset(RGBColor c1, RGBColor c2, RGBColor c3, RGBColor c4, 
      RGBColor c5, RGBColor c6, RGBColor c7, RGBColor c8);

    // add a single color
    bool addColor(RGBColor col);

    // get a color from the colorset
    RGBColor get(uint32_t index) const;

    // set an rgb color at in a slot
    void set(uint32_t index, RGBColor col);
    // set an hsv color in a slot (expensive)
    void set(uint32_t index, HSVColor col);

    // get the next color in cycle
    RGBColor getNext();

    // the number of colors in the palette
    uint32_t numColors() const { return m_palette.size(); }

  private:
    // palette of colors
    std::vector<RGBColor> m_palette;
    // the current index
    uint32_t m_curIndex;
};

#endif
