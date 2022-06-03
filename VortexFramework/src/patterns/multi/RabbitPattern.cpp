#include "RabbitPattern.h"

#include "../single/SingleLedPattern.h"
#include "MultiLedPattern.h"

#include "../../PatternBuilder.h"
#include "../../Colorset.h"
#include "../../Log.h"

RabbitPattern::RabbitPattern() :
  HybridPattern()
{
}

RabbitPattern::~RabbitPattern()
{
}

// init the pattern to initial state
void RabbitPattern::init()
{
  // call base hybrid pattern init to actually initialize sub patterns
  HybridPattern::init();
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
    pat->init(); // TODO: does bind() call init()? not atm
    if (m_ledPatterns[p]) {
      delete m_ledPatterns[p];
    }
    // store the pattern for the given led
    m_ledPatterns[p] = pat;
  }
}

// pure virtual must override the play function
void RabbitPattern::play()
{
  HybridPattern::play();
}

// must override the serialize routine to save the pattern
void RabbitPattern::serialize(SerialBuffer &buffer) const
{
  //DEBUG_LOG("Serialize");
  HybridPattern::serialize(buffer);
}

// must override unserialize to load patterns
void RabbitPattern::unserialize(SerialBuffer &buffer)
{
  //DEBUG_LOG("Unserialize");
  HybridPattern::unserialize(buffer);
}
