#include "SequencedPattern.h"

#include "../../Single/SingleLedPattern.h"
#include "../../PatternBuilder.h"

#include "../../../Serial/ByteStream.h"
#include "../../../Leds/Leds.h"
#include "../../../Log/Log.h"
#include "../../../Memory/Memory.h"
#include "../../../Time/TimeControl.h"

#include <string.h>

static void playSequenceStep(SequencedPattern *self, const SequenceStep *step);
static bool SequencedPattern_stepAlarm(SequencedPattern *self);

const PatternVtable SequencedPattern_vtable = {
  .destroy = SequencedPattern_destroy,
  .bind = MultiLedPattern_bind,
  .init = SequencedPattern_initImpl,
  .play = SequencedPattern_play,
};

void SequencedPattern_init(SequencedPattern *self, const PatternArgs *args, const PatternVtable *vtable)
{
  CompoundPattern_init(&self->base, args, vtable);
  Sequence_make(&self->m_sequence);
  self->m_curSequence = 0;
  self->m_curAlarm = 0;
  Timer_make(&self->m_timer);
  // SequencedPattern is an abstract class it cannot be directly
  // instantiated so we do not need to assign a pattern id
}

void SequencedPattern_initWithSequence(SequencedPattern *self, const PatternArgs *args, const Sequence *sequence, const PatternVtable *vtable)
{
  SequencedPattern_init(self, args, vtable);
  Sequence_copy(&self->m_sequence, sequence);
}

void SequencedPattern_destroy(Pattern *pat)
{
  SequencedPattern *self = (SequencedPattern *)pat;
  Sequence_destroy(&self->m_sequence);
  CompoundPattern_destroy(pat);
}

// init the pattern to initial state
void SequencedPattern_initImpl(Pattern *pat)
{
  SequencedPattern *self = (SequencedPattern *)pat;
  CompoundPattern_initImpl(pat);

  // start the sequence at 0
  self->m_curSequence = 0;
  self->m_curAlarm = 0;
  Timer_reset(&self->m_timer);

  // one alarm that is switched to the duration of each step in turn
  if (Sequence_numSteps(&self->m_sequence) > 0) {
    Timer_addAlarm(&self->m_timer, self->m_sequence.m_sequenceSteps[0].m_duration);
  }

  Timer_start0(&self->m_timer);

  // Play first sequence step in init, if there is one
  if (Sequence_numSteps(&self->m_sequence) > 0) {
    playSequenceStep(self, &self->m_sequence.m_sequenceSteps[0]);
  }
}

// pure virtual must override the play function
void SequencedPattern_play(Pattern *pat)
{
  SequencedPattern *self = (SequencedPattern *)pat;
  if (SequencedPattern_stepAlarm(self)) {
    self->m_curSequence = (self->m_curSequence + 1) % Sequence_numSteps(&self->m_sequence);
  }
  // only index the sequence if the current sequence index is valid
  if (self->m_curSequence < Sequence_numSteps(&self->m_sequence)) {
    // play the sequence step
    playSequenceStep(self, &self->m_sequence.m_sequenceSteps[self->m_curSequence]);
  }
}

// works the same as a timer with an alarm for each step in the sequence
static bool SequencedPattern_stepAlarm(SequencedPattern *self)
{
  uint8_t numSteps = Sequence_numSteps(&self->m_sequence);
  if (!numSteps) {
    return false;
  }
  if (self->m_curAlarm >= numSteps) {
    self->m_curAlarm = 0;
  }
  // the timer reports the first alarm on the start tick without moving on
  bool starting = (Time_getCurtime() == Timer_getStartTime(&self->m_timer));
  self->m_timer.m_alarms[0] = self->m_sequence.m_sequenceSteps[self->m_curAlarm].m_duration;
  if (Timer_alarm(&self->m_timer) == ALARM_NONE) {
    return false;
  }
  if (!starting) {
    self->m_curAlarm = (self->m_curAlarm + 1) % numSteps;
  }
  return true;
}

static void playSequenceStep(SequencedPattern *self, const SequenceStep *step)
{
  for (LedPos pos = LED_FIRST; pos < LED_COUNT; ++pos) {
    // the current initialized pattern for this LED
    SingleLedPattern *curPat = self->base.m_ledPatterns[pos];
    // the intended pattern ID for this LED on this step
    PatternID stepPattern = PatternMap_at(&step->m_patternMap, pos);
    // the actual colorset being used might be the pattern's colorset
    const Colorset *curSet = &self->base.base.base.m_colorset;
    if (Colorset_numColors(&step->m_colorsetMap.m_colorsetMap[pos]) > 0) {
      // or it might be the steps' colorset if it's not empty
      curSet = ColorsetMap_at(&step->m_colorsetMap, pos);
    }
    // if there's no pattern, or pattern is wrong colorset or pattern ID
    if (!curPat || !Colorset_equals(&curPat->m_colorset, curSet) || Pattern_getPatternID(curPat) != stepPattern) {
      // destroy any existing pattern and re-create it
      if (curPat) {
        Pattern_destroy(curPat);
        PatternBuilder_freePattern(curPat);
        self->base.m_ledPatterns[pos] = NULL;
      }
      // create whichever pattern this step is (maybe PATTERN_NONE)
      curPat = self->base.m_ledPatterns[pos] = PatternBuilder_makeSingle(stepPattern, NULL);
      // if a pattern was created then bind and init it
      if (curPat) {
        Pattern_bind(curPat, pos);
        Pattern_setColorset(curPat, curSet);
        Pattern_init(curPat);
      }
    }
    // if there's no pattern set, or the intended pattern of this step is NONE
    if (!curPat || stepPattern == PATTERN_NONE) {
      // clear the LED and don't play the pattern
      Leds_clearIndex(pos);
      continue;
    }
    // otherwise play the pattern on this index
    Pattern_play(curPat);
  }
}

// apply a sequence to the pattern
void SequencedPattern_bindSequence(SequencedPattern *self, const Sequence *sequence)
{
  Sequence_copy(&self->m_sequence, sequence);
}
