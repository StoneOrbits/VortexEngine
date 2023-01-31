#ifndef SEQUENCED_PATTERN_H
#define SEQUENCED_PATTERN_H

#include "../HybridPattern.h"

#include "../../../Leds/LedTypes.h"
#include "../../../Time/Timer.h"

#include "Sequence.h"

class ByteStream;

class SequencedPattern : public HybridPattern
{
protected:
  // SequencedPattern is an abstract class it cannot be directly
  SequencedPattern();
  SequencedPattern(const Sequence &sequence);

public:
  // initialize a sequence pattern with a list of pointers to sequencesteps and the
  // number of entries in that list
  virtual ~SequencedPattern();

  // init the pattern to initial state
  virtual void init() override;

  // pure virtual must override the play function
  virtual void play() override;

  // change the sequence of the pattern
  void bindSequence(const Sequence &sequence);

protected:
  // static data
  Sequence m_sequence;

  // runtime data
  uint32_t m_curSequence;
  Timer m_timer;
};

#endif
