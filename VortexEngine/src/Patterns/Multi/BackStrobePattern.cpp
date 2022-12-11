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
  m_patternID = PATTERN_BACKSTROBE;
}

BackStrobePattern::BackStrobePattern(const PatternArgs &args) :
  BackStrobePattern(args.arg1)
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

  // initialize the sub patterns one time first
  setTipsTops(PATTERN_STROBE, PATTERN_DOPS);
}

void BackStrobePattern::play()
{
  if (m_stepTimer.alarm() == 0) {
    // switch which patterns are displayed
    m_switch = !m_switch;
    // update the tip/top patterns based on the switch
    setTipsTops(m_switch ? PATTERN_DOPS : PATTERN_HYPERSTROBE,
                m_switch ? PATTERN_HYPERSTROBE : PATTERN_DOPS);
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

#if SAVE_TEMPLATE == 1
void BackStrobePattern::saveTemplate(int level) const
{
  HybridPattern::saveTemplate(level);
  IndentMsg(level + 1, "\"StepSpeed\": %d,", m_stepSpeed);
}
#endif
