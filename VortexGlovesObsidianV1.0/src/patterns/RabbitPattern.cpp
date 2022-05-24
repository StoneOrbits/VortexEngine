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
void RabbitPattern::init()
{
  // only create the sub-patterns once
  if (!m_created) {
    // fill the sub patterns array with instances of patterns
    for (LedPos p = LED_FIRST; p <= LED_LAST; p++) {
      SingleLedPattern *pat = nullptr;
      if (((uint32_t)p % 2) == 0) { // tip
        pat = PatternBuilder::makeSingle(PATTERN_STROBE);
      } else { // top
        pat = PatternBuilder::makeSingle(PATTERN_STROBIE);
      }
      if (!pat) {
        return;
      }
      pat->bind(&m_colorset, p);
      // store the pattern for the given led
      m_ledPatterns[p] = pat;
    }
    m_created = true;
  }

  // call base hybrid pattern init to actually initialize sub patterns
  HybridPattern::init();
}

// pure virtual must override the play function
void RabbitPattern::play()
{
  HybridPattern::play();
}

// must override the serialize routine to save the pattern
void RabbitPattern::serialize(SerialBuffer &buffer) const
{
  HybridPattern::serialize(buffer);
}

// must override unserialize to load patterns
void RabbitPattern::unserialize(SerialBuffer &buffer)
{
  HybridPattern::unserialize(buffer);
}
