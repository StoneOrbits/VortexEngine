#include "HybridPattern.h"

#include "SingleLedPattern.h"

#include "../Colorset.h"
#include "../Log.h"

HybridPattern::HybridPattern() :
  MultiLedPattern(),
  m_ledPatterns(),
  m_ledColorsets()
{
}

HybridPattern::~HybridPattern()
{
  // clean up all the sub patterns
  for (LedPos pos = LED_FIRST; pos <= LED_LAST; pos++) {
    SingleLedPattern *pat = m_ledPatterns[pos];
    Colorset *set = m_ledColorsets[pos];
    if (pat) {
      delete pat;
    }
    if (set) {
      delete set;
    }
  }
}

// init the pattern to initial state
void HybridPattern::init(Colorset *colorset, LedPos pos)
{
  for (LedPos ledPos = LED_FIRST; ledPos <= LED_LAST; ledPos++) {
    SingleLedPattern *pat = m_ledPatterns[ledPos];
    Colorset *set = m_ledColorsets[ledPos];
    if (!pat || !set) {
      // fatal error!
      return;
    }
    // initialize the pattern with the new colorset in the led position
    pat->init(set, ledPos);
  }
  MultiLedPattern::init(colorset, pos);
}

// play the hybrid pattern (play all led patterns)
void HybridPattern::play()
{
  for (LedPos pos = LED_FIRST; pos <= LED_LAST; pos++) {
    SingleLedPattern *pat = m_ledPatterns[pos];
    if (!pat) {
      // should never happen, but just in case
      continue;
    }
    pat->play();
  }
}

// must override the serialize routine to save the pattern
void HybridPattern::serialize(SerialBuffer &buffer) const
{
  MultiLedPattern::serialize(buffer);
  for (LedPos pos = LED_FIRST; pos <= LED_LAST; pos++) {
    if (!m_ledPatterns[pos] || !m_ledColorsets[pos]) {
      DEBUG("Could not serialize hybrid pattern!");
      return;
    }
    m_ledPatterns[pos]->serialize(buffer);
    m_ledColorsets[pos]->serialize(buffer);
  }
}

// must override unserialize to load patterns
void HybridPattern::unserialize(SerialBuffer &buffer)
{
}
