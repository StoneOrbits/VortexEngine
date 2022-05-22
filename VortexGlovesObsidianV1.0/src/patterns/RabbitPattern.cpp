#include "RabbitPattern.h"

#include "SingleLedPattern.h"
#include "MultiLedPattern.h"

#include "../PatternBuilder.h"
#include "../Colorset.h"
#include "../Log.h"

RabbitPattern::RabbitPattern() :
  HybridPattern(),
  m_created(false)
{
}

RabbitPattern::~RabbitPattern()
{
}

// init the pattern to initial state
void RabbitPattern::init(Colorset *colorset, LedPos pos)
{
  // only create the sub-patterns once
  if (!m_created) {
    // fill the sub patterns array with instances of patterns
    for (LedPos pos = LED_FIRST; pos <= LED_LAST; pos++) {
      SingleLedPattern *pat = nullptr;
      if (((uint32_t)pos % 2) == 0) { // tip
        pat = PatternBuilder::makeSingle(PATTERN_STROBE);
      } else { // top
        pat = PatternBuilder::makeSingle(PATTERN_STROBIE);
      }
      if (!pat) {
        return;
      }
      Colorset *set = new Colorset(*colorset);
      if (!set) {
        ERROR_OUT_OF_MEMORY();
        delete pat;
        return;
      }
      // store the pattern for the given led
      m_ledColorsets[pos] = set;
      m_ledPatterns[pos] = pat;
    }
    m_created = true;
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
