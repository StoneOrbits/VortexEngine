#ifndef COLOR_H
#define COLOR_H

class HSVColor
{
  public:
    HSVColor();
    HSVColor(uint8_t hue, uint8_t sat, uint8_t val);

    // public members
    uint8_t hue;
    uint8_t sat;
    uint8_t val;
};

#endif
