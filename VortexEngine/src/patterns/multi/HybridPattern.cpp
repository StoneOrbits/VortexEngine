#include "HybridPattern.h"

#include "../single/SingleLedPattern.h"

#include "../../PatternBuilder.h"
#include "../../SerialBuffer.h"
#include "../../Colorset.h"
#include "../../Log.h"

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
      m_ledPatterns[pos] = nullptr;
    }
  }
}

// init the pattern to initial state
void HybridPattern::init()
{
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
  //DEBUG_LOG("Serialize");
  MultiLedPattern::serialize(buffer);
  for (LedPos pos = LED_FIRST; pos <= LED_LAST; pos++) {
    if (!m_ledPatterns[pos]) {
      DEBUG_LOG("Could not serialize hybrid pattern!");
      return;
    }
    m_ledPatterns[pos]->serialize(buffer);
  }
}

// must override unserialize to load patterns
void HybridPattern::unserialize(SerialBuffer &buffer)
{
  //DEBUG_LOG("Unserialize");
  clearPatterns();
  MultiLedPattern::unserialize(buffer);
  for (LedPos pos = LED_FIRST; pos <= LED_LAST; pos++) {
    SingleLedPattern *pat = PatternBuilder::makeSingle((PatternID)buffer.unserialize8());
    if (!pat) {
      return;
    }
    pat->unserialize(buffer);
    m_ledPatterns[pos] = pat;
  }
}

#ifdef TEST_FRAMEWORK
void HybridPattern::saveTemplate() const
{
  MultiLedPattern::saveTemplate();
  InfoMsg("            \"SubPatterns\": [");
  for (LedPos pos = LED_FIRST; pos <= LED_LAST; pos++) {
    // this might be ugly
    if (m_ledPatterns[pos]) {
      InfoMsg("              {");
      m_ledPatterns[pos]->saveTemplate();
      // close the params of sub pattern
      InfoMsg("                }");
      InfoMsg("              },");
    }
  }
  InfoMsg("            ],");
}
#endif

void HybridPattern::clearPatterns()
{
  for (LedPos pos = LED_FIRST; pos <= LED_LAST; pos++) {
    if (!m_ledPatterns[pos]) {
      continue;
    }
    delete m_ledPatterns[pos];
    m_ledPatterns[pos] = nullptr;
  }
}

void HybridPattern::setPatternAt(LedPos pos, SingleLedPattern *pat, const Colorset *set)
{
  if (!pat || pos >= LED_COUNT) {
    return;
  }
  if (!set) {
    set = &m_colorset;
  }
  pat->bind(set, pos);
  pat->init();
  // handle re-initialization and prevent leaks
  if (m_ledPatterns[pos]) {
    delete m_ledPatterns[pos];
  }
  // store the pattern for the given led
  m_ledPatterns[pos] = pat;
}
