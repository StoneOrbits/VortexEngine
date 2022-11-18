#ifndef SEQUENCED_PATTERN_H
#define SEQUENCED_PATTERN_H

#include "../HybridPattern.h"

#include "../../../Leds/LedTypes.h"
#include "../../../Time/Timer.h"

#include "Sequence.h"

class ByteStream;

class SequencedPattern : public HybridPattern
{
public:
  // initialize a sequence pattern with a list of pointers to sequencesteps and the 
  // number of entries in that list
  SequencedPattern(const Sequence &sequence);
  virtual ~SequencedPattern();

  // init the pattern to initial state
  virtual void init() override;

  // pure virtual must override the play function
  virtual void play() override;

  // must override the serialize routine to save the pattern
  virtual void serialize(ByteStream &buffer) const override;
  virtual void unserialize(ByteStream &buffer) override;

#ifdef TEST_FRAMEWORK
  virtual void saveTemplate(int level = 0) const override;
#endif

protected:
  // static data
  Sequence m_sequence;

  // runtime data
  uint32_t m_curSequence;
  Timer m_timer;
};

#endif
