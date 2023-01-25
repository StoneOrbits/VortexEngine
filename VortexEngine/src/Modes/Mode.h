#ifndef MODE_H
#define MODE_H

#include "../Leds/LedTypes.h"
#include "../Patterns/Patterns.h"

class SingleLedPattern;
class MultiLedPattern;
class PatternArgs;
class ByteStream;
class Colorset;
class Pattern;

// Bitflags for saving modes
#define MODE_FLAG_NONE              0
// the mode is utilizing a multi-led pattern
#define MODE_FLAG_MULTI_LED         (1 << 0)
// the mode is utilizing the same single-led pattern on each finger
#define MODE_FLAG_ALL_SAME_SINGLE   (1 << 1)

// the keyword 'ALL_SLOTS' can be used to refer to all of the
// mode slots at once when using changePattern or changeColorset
#define ALL_SLOTS LED_COUNT

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
  Mode();
  Mode(PatternID id, const Colorset &set);
  Mode(PatternID id, const PatternArgs &args, const Colorset &set);
  virtual ~Mode();

  // initialize the mode to initial state
  virtual void init();

  // Play the mode
  virtual void play();

  // save/load from a buffer for individual mode sharing
  virtual bool saveToBuffer(ByteStream &saveBuffer) const;
  virtual bool loadFromBuffer(ByteStream &saveBuffer);

  // save the mode to serial
  virtual void serialize(ByteStream &buffer) const;
  // load the mode from serial (optional led count)
  virtual bool unserialize(ByteStream &buffer, uint32_t numLeds = LED_COUNT);

#if SAVE_TEMPLATE == 1
  // save the data template
  virtual void saveTemplate(int level = 0) const;
#endif

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
  bool setSinglePat(LedPos pos, SingleLedPattern *pat, const Colorset *set = nullptr);
  bool setMultiPat(PatternID pat, const PatternArgs *args = nullptr, const Colorset *set = nullptr);
  bool setMultiPat(MultiLedPattern *pat, const Colorset *set = nullptr);

  // get the flags associated with this mode
  uint32_t getFlags() const;

  // is this a multi-led pattern in the mode?
  bool isMultiLed() const;
  // are all the single led patterns and colorsets equal?
  bool isSameSingleLed() const;

  // erase any stored patterns or colorsets
  void clearPatterns();
  void clearPattern(LedPos pos);
  void clearColorsets();
private:
  // A mode simply contains a list of patterns for each LED, these can either
  // be each SingleLedPatterns up to LED_COUNT of them -- or the first entry
  // can be a MultiLedPattern, just one.
  union {
    // map of led positions => pattern entries
    Pattern *m_ledEntries[LED_COUNT];
    // accessors for single leds
    SingleLedPattern *m_singleLedEntries[LED_COUNT];
    // or the first one is also a multi led pat
    MultiLedPattern *m_multiPat;
  };
};

#endif
