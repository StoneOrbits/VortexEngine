#include "SplitStrobiePattern.h"

#include "../../PatternBuilder.h"
#include "../../SerialBuffer.h"
#include "../../Colorset.h"
#include "../../Log.h"

SplitStrobiePattern::SplitStrobiePattern(uint16_t stepDuration) :
  HybridPattern(),
  m_stepDuration(stepDuration),
  m_stepTimer(),
  m_switch()
{
}

SplitStrobiePattern::~SplitStrobiePattern()
{
}

// init the pattern to initial state
void SplitStrobiePattern::init()
{
  HybridPattern::init();

  // timer for switch
  m_stepTimer.reset();
  m_stepTimer.addAlarm(m_stepDuration);
  m_stepTimer.start();
}

void SplitStrobiePattern::play()
{
  if (m_stepTimer.alarm() == 0) {
    m_switch = !m_switch;
    for (LedPos p = LED_FIRST; p <= LED_LAST; p++) {
      if (isFingerTip(p)) {
        if (m_switch) {
          setPatternAt(p, PatternBuilder::makeSingle(PATTERN_DOPS));
        } else {
          setPatternAt(p, PatternBuilder::makeSingle(PATTERN_STROBE));
        }
      } else {
        if (m_switch) {
          setPatternAt(p, PatternBuilder::makeSingle(PATTERN_STROBE));
        } else {
          setPatternAt(p, PatternBuilder::makeSingle(PATTERN_DOPS));
        }
      }
    }
  }
  HybridPattern::play();
}

// must override the serialize routine to save the pattern
void SplitStrobiePattern::serialize(SerialBuffer& buffer) const
{
  HybridPattern::serialize(buffer);
  buffer.serialize(m_stepDuration);
}

void SplitStrobiePattern::unserialize(SerialBuffer& buffer)
{
  HybridPattern::unserialize(buffer);
  buffer.unserialize(&m_stepDuration);
}

#ifdef TEST_FRAMEWORK
void SplitStrobiePattern::saveTemplate(int level) const
{
  HybridPattern::saveTemplate(level);
  IndentMsg(level + 1, "\"StepDuration\": %d,", m_stepDuration);
}
#endif
