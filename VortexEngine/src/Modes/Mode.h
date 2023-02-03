#ifndef MODE_H
#define MODE_H

#include "../Leds/LedTypes.h"
#include "../Patterns/Patterns.h"
#include "../VortexConfig.h"

class PatternArgs;
class ByteStream;
class Colorset;
class Pattern;

// Bitflags for saving modes
#define MODE_FLAG_NONE              0
// the mode is utilizing the same single-led pattern on each finger
#define MODE_FLAG_ALL_SAME_SINGLE   (1 << 1)

// A mode is the container for instances of patterns. A pattern
// must be bound to a colorset and led position with bind() and
// the Mode class allows that to be done in one step. Other things
// can be done in the Mode class such as accelerometer reaction,
// microphone reaction, timed colorset or pattern switches, or any
// other kind of logic that would involve manipulating the pattern
// or colorset as a whole
class Mode
{
public:
#if FIXED_LED_COUNT == 0
  Mode(uint32_t numLeds);
#endif
  Mode();

  Mode(PatternID id, const Colorset &set);
  Mode(PatternID id, const PatternArgs &args, const Colorset &set);
  Mode(PatternID id, const PatternArgs *args, const Colorset *set);
  Mode(const Mode *other);
  ~Mode();

  // copy and assignment operators
  Mode(const Mode &other);
  void operator=(const Mode &other);

  // equality operators
  bool operator==(const Mode &other) const;
  bool operator!=(const Mode &other) const;

  // initialize the mode to initial state
  void init();
  void clear();

  // Play the mode
  void play();

  // save/load from a buffer for individual mode sharing
  bool saveToBuffer(ByteStream &saveBuffer) const;
  bool loadFromBuffer(ByteStream &saveBuffer);

  // save the mode to serial
  void serialize(ByteStream &buffer) const;
  // load the mode from serial (optional led count)
  bool unserialize(ByteStream &buffer);

#if FIXED_LED_COUNT == 0
  // change the internal pattern count in the mode object
  void setLedCount(uint8_t numLeds);
#endif
  uint8_t getLedCount() const;

  // iterate all patterns to the next id
  void nextPat();
  void rollColorset(uint32_t numColors = 3);

  // Get pointer to an individual pattern/colorset
  const Pattern *getPattern(LedPos pos = LED_FIRST) const;
  Pattern *getPattern(LedPos pos = LED_FIRST);
  // get a pointer to a colorset
  const Colorset *getColorset(LedPos pos = LED_FIRST) const;
  Colorset *getColorset(LedPos pos = LED_FIRST);
  // get the pattern ID of the given pattern
  PatternID getPatternID(LedPos pos = LED_FIRST) const;

  // mode comparison
  bool equals(const Mode *other) const;

  // set the pattern/colorset of the mode, if a multi-led pattern is provided then the pos
  // is ignored. If a single led pattern is provided then it will be applied to all LEDS
  // unless a specific LED is provided
  bool setPattern(PatternID pat, const PatternArgs *args = nullptr, const Colorset *set = nullptr);
  bool setColorset(const Colorset *set);

  bool setColorsetAt(const Colorset *set, LedPos pos);

  // change a single or multi pattern
  bool setSinglePat(LedPos pos, PatternID pat, const PatternArgs *args = nullptr, const Colorset *set = nullptr);

  // get the flags associated with this mode
  uint32_t getFlags() const;

  // are all the single led patterns and colorsets equal?
  bool isSameSingleLed() const;

  // erase any stored patterns or colorsets
  void clearPattern(LedPos pos);
  void clearColorsets();
private:
#if FIXED_LED_COUNT == 0
  // the number of leds the mode is targetting
  uint8_t m_numLeds;
  // list of pointers to Patterns, one for each led or if it
  // is a multi-led pattern then there is only one total
  Pattern **m_ledEntries;
#else
  Pattern *m_ledEntries[LED_COUNT];
#endif
};

#endif
