#include "ComplementaryBlendPattern.h"

#include "../../Colorset.h"
#include "../../Leds.h"

ComplementaryBlendPattern::ComplementaryBlendPattern(uint8_t onDuration, uint8_t offDuration, uint8_t blendSpeed) :
  BlendPattern(onDuration, offDuration, blendSpeed),
  m_showingComplement(false)
{
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
    // generate an inverse hue
    Leds::setIndex(m_ledPos, HSVColor((m_cur.hue + 128) % 256, m_cur.sat, m_cur.val));
  } else {
    BlendPattern::onBlinkOn();
  }
}