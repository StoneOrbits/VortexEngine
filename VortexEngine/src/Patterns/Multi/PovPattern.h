#ifndef POV_PATTERN_H
#define POV_PATTERN_H

#include "MultiLedPattern.h"

struct PovTexture;

// Plays a POV texture (see PovTextures.h) one column at a time on every arm.
//
// Each column is shown for m_columnOn frames followed by m_columnOff dark
// frames (one frame per engine tick, ~1.6ms with 68 leds). After each copy of
// the texture there are m_loopGap blank columns before the next one.
//
// Colors: the digits in a texture are color 'roles'. Each copy of the texture
// moves on to the next colors in the colorset, so a texture with 2 roles and a
// colorset of 6 colors shows 1+2, then 3+4, then 5+6 and repeats. Any number of
// colors from 1 to 8 works with any texture.
//
// Every PATTERN_POV_ id uses this class, the id picks the texture.
class PovPattern : public MultiLedPattern
{
public:
  PovPattern(const PatternArgs &args);
  virtual ~PovPattern();

  // init the pattern to initial state
  virtual void init() override;

  // play the pattern
  virtual void play() override;

  // the default number of blank columns between copies for a POV pattern id
  static uint8_t defaultGap(PatternID id);

private:
  // the color for a pixel character in the texture on a given copy
  RGBColor pixelColor(char pixel, uint32_t copy, uint32_t step);

  // args
  uint8_t m_columnOn;
  uint8_t m_columnOff;
  uint8_t m_loopGap;

  // the texture being played, its width in columns and how many color roles
  // it uses (the highest digit)
  const PovTexture *m_texture;
  uint8_t m_width;
  uint8_t m_numRoles;

  // when the pattern started
  uint32_t m_startTime;
};

#endif
