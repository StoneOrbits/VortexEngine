#include "HybridPattern.h"

#include "SingleLedPattern.h"

#include "../Colorset.h"
#include "../Log.h"

HybridPattern::HybridPattern() :
  MultiLedPattern(),
  m_ledPatterns()
{
}

HybridPattern::~HybridPattern()
{
  // clean up all the sub patterns
  for (LedPos pos = LED_FIRST; pos <= LED_LAST; pos++) {
    if (m_ledPatterns[pos]) {
      delete m_ledPatterns[pos];
    }
  }
}

// init the pattern to initial state
void HybridPattern::init()
{
  for (LedPos ledPos = LED_FIRST; ledPos <= LED_LAST; ledPos++) {
    SingleLedPattern *pat = m_ledPatterns[ledPos];
    if (!pat) {
      continue;
    }
    pat->init();
  }
  MultiLedPattern::init();
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
    if (!m_ledPatterns[pos]) {
      DEBUG("Could not serialize hybrid pattern!");
      return;
    }
    m_ledPatterns[pos]->serialize(buffer);
  }
}

// must override unserialize to load patterns
void HybridPattern::unserialize(SerialBuffer &buffer)
{
}
