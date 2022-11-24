#include "HybridPattern.h"

#include "../single/SingleLedPattern.h"

#include "../PatternBuilder.h"
#include "../../Serial/ByteStream.h"
#include "../../Colors/Colorset.h"
#include "../../Log/Log.h"

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

// must override the serialize routine to save the pattern
void HybridPattern::serialize(ByteStream &buffer) const
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
void HybridPattern::unserialize(ByteStream &buffer)
{
  //DEBUG_LOG("Unserialize");
  clearPatterns();
  MultiLedPattern::unserialize(buffer);
  for (LedPos pos = LED_FIRST; pos <= LED_LAST; pos++) {
    SingleLedPattern *pat = PatternBuilder::makeSingle((PatternID)buffer.unserialize8());
    if (!pat) {
      ERROR_LOGF("Failed to unserialize hybrid pat %u", pos);
      return;
    }
    pat->unserialize(buffer);
    m_ledPatterns[pos] = pat;
  }
}

#if SAVE_TEMPLATE == 1
void HybridPattern::saveTemplate(int level) const
{
  MultiLedPattern::saveTemplate(level);
  IndentMsg(level + 1, "\"SubPatterns\": [");
  for (LedPos pos = LED_FIRST; pos <= LED_LAST; pos++) {
    // this might be ugly
    if (m_ledPatterns[pos]) {
      IndentMsg(level + 2, "{");
      m_ledPatterns[pos]->saveTemplate(level + 3);
      // close the params of sub pattern
      IndentMsg(level + 3, "}");
      IndentMsg(level + 2, "},");
    }
  }
  IndentMsg(level + 1, "],");
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

void HybridPattern::setTipsTops(PatternID tipPattern, PatternID topPattern)
{
  // Set the tipPattern on all fingerTips and topPattern on all fingerTops
  for (LedPos p = LED_FIRST; p <= LED_LAST; p++) {
    setPatternAt(p, PatternBuilder::makeSingle(isFingerTip(p) ? tipPattern : topPattern));
  }
}
