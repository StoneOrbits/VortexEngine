#include "BracketsPattern.h"

#include "../../Serial/ByteStream.h"
#include "../../Colors/Colorset.h"
#include "../../Leds/Leds.h"
#include "../../Log/Log.h"

BracketsPattern::BracketsPattern(uint8_t bracketDuration, uint8_t midDuration, uint8_t offDuration) :
  SingleLedPattern(),
  m_bracketDuration(bracketDuration),
  m_midDuration(midDuration),
  m_offDuration(offDuration)
{
  m_patternID = PATTERN_BRACKETS;
}

BracketsPattern::BracketsPattern(const PatternArgs &args) :
  BracketsPattern()
{
  setArgs(args);
}

BracketsPattern::~BracketsPattern()
{
}

void BracketsPattern::init()
{
  // run basic pattern init logic
  SingleLedPattern::init();

  // reset the blink timer entirely
  m_blinkTimer.reset();

  // add the alarms for on then off
  m_blinkTimer.addAlarm(m_bracketDuration);
  m_blinkTimer.addAlarm(m_midDuration);
  m_blinkTimer.addAlarm(m_bracketDuration);
  m_blinkTimer.addAlarm(m_offDuration);

  // start the blink timer from the next frame
  m_blinkTimer.start();
}

void BracketsPattern::play()
{
  // check the alarm to toggle the light
  switch (m_blinkTimer.alarm()) {
  case 0: // gap ended
    // begin first bracket
    Leds::setIndex(m_ledPos, m_colorset.getNext());
    break;
  case 1: // first bracket ended
    // begin mid
    Leds::setIndex(m_ledPos, m_colorset.getNext());
    break;
  case 2: // mid ended
    // begin second bracket
    Leds::setIndex(m_ledPos, m_colorset.getPrev());
    break;
  case 3: // second bracket ended
    // begin gap
    Leds::clearIndex(m_ledPos);
    break;
  }
}

void BracketsPattern::serialize(ByteStream &buffer) const
{
  SingleLedPattern::serialize(buffer);
  buffer.serialize(m_bracketDuration);
  buffer.serialize(m_midDuration);
  buffer.serialize(m_offDuration);
}

void BracketsPattern::unserialize(ByteStream &buffer)
{
  SingleLedPattern::unserialize(buffer);
  buffer.unserialize(&m_bracketDuration);
  buffer.unserialize(&m_midDuration);
  buffer.unserialize(&m_offDuration);
}

void BracketsPattern::setArgs(const PatternArgs &args)
{
  SingleLedPattern::setArgs(args);
  m_bracketDuration = args.arg1;
  m_midDuration = args.arg2;
  m_offDuration = args.arg3;
}

void BracketsPattern::getArgs(PatternArgs &args) const
{
  SingleLedPattern::getArgs(args);
  args.arg1 = m_bracketDuration;
  args.arg2 = m_midDuration;
  args.arg3 = m_offDuration;
  args.numArgs += 3;
}

#if SAVE_TEMPLATE == 1
void BracketsPattern::saveTemplate(int level) const
{
  SingleLedPattern::saveTemplate(level);
  IndentMsg(level + 1, "\"BracketDuration\": %d,", m_bracketDuration);
  IndentMsg(level + 1, "\"MidDuration\": %d,", m_midDuration);
  IndentMsg(level + 1, "\"OffDuration\": %d,", m_offDuration);
}
#endif


