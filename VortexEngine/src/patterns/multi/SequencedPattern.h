#ifndef SEQUENCED_PATTERN_H
#define SEQUENCED_PATTERN_H

#include "HybridPattern.h"

#include "../../Sequence.h"
#include "../../LedTypes.h"
#include "../../Timer.h"

class SerialBuffer;

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
  virtual void serialize(SerialBuffer &buffer) const override;
  virtual void unserialize(SerialBuffer &buffer) override;

protected:
  // static data
  Sequence m_sequence;

  // runtime data
  uint32_t m_curSequence;
  Timer m_timer;
};

#endif
