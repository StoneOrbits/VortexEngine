#include "RabbitPattern.h"

#include "../PatternBuilder.h"
#include "../Colorset.h"

RabbitPattern::RabbitPattern() :
  HybridPattern()
{
}

RabbitPattern::~RabbitPattern()
{
}

// init the pattern to initial state
void RabbitPattern::init(Colorset *colorset, LedPos pos)
{
  // fill the sub patterns array with instances of patterns
  for (LedPos pos = LED_FIRST; pos <= LED_LAST; pos++) {
    Pattern *pat = nullptr;
    if (((uint32_t)pos % 2) == 0) { // tip
      pat = PatternBuilder::make(PATTERN_STROBE);
    } else { // top
      pat = PatternBuilder::make(PATTERN_STROBIE);
    }
    // make a copy of the colorset so each LED has it's own instance of the colorset
    Colorset *set = new Colorset(*colorset);
    if (!set || !pat) {
      // fatal error
      return;
    }
    // store the pattern/colorset for the given led
    m_ledPatterns[pos] = pat;
    m_ledColorsets[pos] = set;
  }

  // call base hybrid pattern init to actually initialize sub patterns
  HybridPattern::init(colorset, pos);
}

// pure virtual must override the play function
void RabbitPattern::play()
{
  HybridPattern::play();
}

// must override the serialize routine to save the pattern
void RabbitPattern::serialize() const
{
  HybridPattern::serialize();
}

// must override unserialize to load patterns
void RabbitPattern::unserialize()
{
  HybridPattern::unserialize();
}
