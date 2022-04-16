#ifndef COLORSET_H
#define COLORSET_H

#include "Color.h"

class Colorset
{
  public:
    Colorset();

    // create a new colorset with a list of colors
    bool init(uint32_t amount, HSVColor *colors);

    // get a color from the colorset
    HSVColor get(uint32_t index);

  private:
    // palette of colors
    HSVColor *m_pPalette;
    // the number of colors in the palette
    uint32_t m_numColors;
};

#endif
