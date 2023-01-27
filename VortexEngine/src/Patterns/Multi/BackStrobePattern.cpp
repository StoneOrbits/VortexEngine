#include "BackStrobePattern.h"

#include "../PatternBuilder.h"
#include "../../Serial/ByteStream.h"
#include "../../Colors/Colorset.h"
#include "../../Log/Log.h"

BackStrobePattern::BackStrobePattern(uint8_t onDuration1, uint8_t offDuration1, uint8_t gapDuration1,
  uint8_t onDuration2, uint8_t offDuration2, uint8_t gapDuration2, uint8_t stepSpeed100ms) :
  HybridPattern(),
  m_stepSpeed(stepSpeed100ms),
  m_stepTimer(),
  m_switch(),
  m_firstPatternArgs(onDuration1, offDuration1, gapDuration1),
  m_secondPatternArgs(onDuration2, offDuration2, gapDuration2)
{
  m_patternID = PATTERN_BACKSTROBE;
}

BackStrobePattern::BackStrobePattern(const PatternArgs &args) :
  BackStrobePattern()
{
  setArgs(args);
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
  m_stepTimer.addAlarm(m_stepSpeed * 100);
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
                m_switch ? PATTERN_HYPERSTROBE : PATTERN_DOPS,
                m_switch ? &m_firstPatternArgs : &m_secondPatternArgs,
                m_switch ? &m_secondPatternArgs : &m_firstPatternArgs);
  }
  HybridPattern::play();
}

void BackStrobePattern::setArgs(const PatternArgs &args)
{
  HybridPattern::setArgs(args);
  m_firstPatternArgs.arg1 = args.arg1;
  m_firstPatternArgs.arg2 = args.arg2;
  m_firstPatternArgs.arg3 = args.arg3;
  m_secondPatternArgs.arg1 = args.arg4;
  m_secondPatternArgs.arg2 = args.arg5;
  m_secondPatternArgs.arg3 = args.arg6;
  m_stepSpeed = args.arg7;
}

void BackStrobePattern::getArgs(PatternArgs &args) const
{
  HybridPattern::getArgs(args);
  args.arg1 = m_firstPatternArgs.arg1;
  args.arg2 = m_firstPatternArgs.arg2;
  args.arg3 = m_firstPatternArgs.arg3;
  args.arg4 = m_secondPatternArgs.arg1;
  args.arg5 = m_secondPatternArgs.arg2;
  args.arg6 = m_secondPatternArgs.arg3;
  args.arg7 = m_stepSpeed;
  args.numArgs += 7;
}

#if SAVE_TEMPLATE == 1
void BackStrobePattern::saveTemplate(int level) const
{
  HybridPattern::saveTemplate(level);
  IndentMsg(level + 1, "\"StepSpeed\": %d,", m_stepSpeed);
}
#endif
