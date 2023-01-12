#include "ComplementaryBlendPattern.h"

#include "../../Colors/Colorset.h"
#include "../../Leds/Leds.h"

ComplementaryBlendPattern::ComplementaryBlendPattern(uint8_t onDuration, uint8_t offDuration,
  uint8_t gapDuration) :
  BlendPattern(onDuration, offDuration, gapDuration),
  m_showingComplement(false)
{
  m_patternID = PATTERN_COMPLEMENTARY_BLEND;
}

ComplementaryBlendPattern::ComplementaryBlendPattern(const PatternArgs &args) :
  ComplementaryBlendPattern()
{
  setArgs(args);
}

ComplementaryBlendPattern::~ComplementaryBlendPattern()
{
}

void ComplementaryBlendPattern::init()
{
  BlendPattern::init();
  m_showingComplement = false;
}

void ComplementaryBlendPattern::onBlinkOn()
{
  // every other tick show the complement color
  m_showingComplement = !m_showingComplement;
  if (m_showingComplement) {
    // generate an inverse hue based on the current hue position
    HSVColor hsvCol((m_cur.hue + 128) % 256, m_cur.sat, m_cur.val);
    // convert the HSV to RGB with the generic function because
    // this will generate a different appearance from using the
    // default hsv_to_rgb_rainbow()
    Leds::setIndex(m_ledPos, hsv_to_rgb_generic(hsvCol));
  } else {
    BlendPattern::onBlinkOn();
  }
}
