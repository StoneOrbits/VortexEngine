#include "HybridPattern.h"

#include "../single/SingleLedPattern.h"
#include "../../Colors/Colorset.h"
#include "../PatternBuilder.h"

HybridPattern::HybridPattern() :
  MultiLedPattern(),
  m_ledPatterns()
{
  // Hybrid is an abstract class it cannot be directly
  // instantiated so we do not need to assign a pattern id
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

void HybridPattern::setPatternAt(LedPos pos, SingleLedPattern *pat,
  const Colorset *set)
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

void HybridPattern::setPatternAt(LedPos pos, PatternID id,
  const PatternArgs *args, const Colorset *set)
{
  if (pos >= LED_COUNT) {
    return;
  }
  setPatternAt(pos, PatternBuilder::makeSingle(id, args), set);
}

void HybridPattern::setTipsTops(PatternID tipPattern, PatternID topPattern,
  const PatternArgs *tipArgs, const PatternArgs *topArgs)
{
  // Set the tipPattern on all fingerTips and topPattern on all fingerTops
  for (LedPos p = LED_FIRST; p <= LED_LAST; p++) {
    const PatternArgs *args = isFingerTip(p) ? tipArgs : topArgs;
    PatternID id = isFingerTip(p) ? tipPattern : topPattern;
    setPatternAt(p, PatternBuilder::makeSingle(id, args));
  }
}
