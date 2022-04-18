#ifndef COLORSET_H
#define COLORSET_H

#include <vector>

#include "Color.h"

class Colorset
{
  public:
    Colorset();

    // create a new colorset with a list of colors
    bool init(uint32_t amount, RGBColor *colors);

    // get a color from the colorset
    RGBColor get(uint32_t index);

    // the number of colors in the palette
    uint32_t numColors() { return m_numColors; }

  private:
    // palette of colors
    std::vector<RGBColor> m_palette;

    // the number of colors in the palette
    uint32_t m_numColors;
};

#endif
