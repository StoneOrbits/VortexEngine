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
  pat->bind(pos);
  pat->setColorset(set);
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

void HybridPattern::setEvensOdds(PatternID evenPattern, PatternID oddPattern, const PatternArgs *evenArgs, const PatternArgs *oddArgs)
{
  // Set the evenPattern on all even leds and oddPattern on all odd leds
  for (LedPos p = LED_FIRST; p <= LED_LAST; p++) {
    const PatternArgs *args = isEven(p) ? evenArgs : oddArgs;
    PatternID id = isEven(p) ? evenPattern : oddPattern;
    setPatternAt(p, PatternBuilder::makeSingle(id, args));
  }
}
