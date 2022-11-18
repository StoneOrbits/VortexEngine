#include "BackStrobePattern.h"

#include "../PatternBuilder.h"
#include "../../Serial/ByteStream.h"
#include "../../Colors/Colorset.h"
#include "../../Log/Log.h"

BackStrobePattern::BackStrobePattern(uint16_t stepSpeed) :
  HybridPattern(),
  m_stepSpeed(stepSpeed),
  m_stepTimer(),
  m_switch()
{
}

BackStrobePattern::~BackStrobePattern()
{
}

// init the pattern to initial state
void BackStrobePattern::init()
{
  HybridPattern::init();

  // timer for switch
  m_stepTimer.reset();
  m_stepTimer.addAlarm(m_stepSpeed);
  m_stepTimer.start();
}

void BackStrobePattern::play()
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
void BackStrobePattern::serialize(ByteStream& buffer) const
{
  HybridPattern::serialize(buffer);
  buffer.serialize(m_stepSpeed);
}

void BackStrobePattern::unserialize(ByteStream& buffer)
{
  HybridPattern::unserialize(buffer);
  buffer.unserialize(&m_stepSpeed);
}

#ifdef TEST_FRAMEWORK
void BackStrobePattern::saveTemplate(int level) const
{
  HybridPattern::saveTemplate(level);
  IndentMsg(level + 1, "\"StepSpeed\": %d,", m_stepSpeed);
}
#endif
