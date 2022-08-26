#include "SplitStrobiePattern.h"

#include "../../PatternBuilder.h"
#include "../../Colorset.h"

SplitStrobiePattern::SplitStrobiePattern() :
  HybridPattern(),
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
  m_stepTimer.addAlarm(1000);
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