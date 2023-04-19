#ifndef MODE_H
#define MODE_H

#include "../Leds/LedTypes.h"
#include "../Patterns/Patterns.h"
#include "../VortexConfig.h"

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
// the mode is utilizing at least one single-led pattern
#define MODE_FLAG_SINGLE_LED        (1 << 1)
// the mode is utilizing the same single-led pattern on each finger
// optimization will be done when saving and only one led is saved
#define MODE_FLAG_ALL_SAME_SINGLE   (1 << 2)
// the mode is missing leds on some single slots, an led map is
// present in savefile in order to convey which leds are missing
#define MODE_FLAG_SPARSE_SINGLES    (1 << 3)

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
  virtual ~Mode();

  // copy and assignment operators
  Mode(const Mode &other);
  void operator=(const Mode &other);

  // equality operators
  bool operator==(const Mode &other) const;
  bool operator!=(const Mode &other) const;

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
  virtual bool unserialize(ByteStream &buffer);

#if FIXED_LED_COUNT == 0
  // change the internal pattern count in the mode object
  void setLedCount(uint8_t numLeds);
#endif
  uint8_t getLedCount() const;

  // get the effective pattern on a mode
  const Pattern *getPattern() const;
  Pattern *getPattern();
  // get the pattern at a position
  const Pattern *getPatternAt(LedPos pos) const;
  Pattern *getPatternAt(LedPos pos);
  // get a pointer to the multi pattern
  const Pattern *getMultiPat() const;
  Pattern *getMultiPat();
  // get the effective colorset on a mode
  const Colorset *getColorset() const;
  Colorset *getColorset();
  // get the colorset at a position
  const Colorset *getColorsetAt(LedPos pos) const;
  Colorset *getColorsetAt(LedPos pos);
  // get a pointer to the multi colorset
  const Colorset *getMultiColorset() const;
  Colorset *getMultiColorset();
  // get the effective pattern id on a mode
  PatternID getPatternID() const;
  // get the pattern ID of the given pattern
  PatternID getPatternIDAt(LedPos pos) const;
  // get the multi LED pattern ID
  PatternID getMultiPatID() const;

  // mode comparison
  bool equals(const Mode *other) const;

  // set the pattern/colorset of the mode, if a single-led pattern is provided
  // then it will be applied to all single-led pattern slots. If a multi-led
  // is provided then it will be applied to the multi-led pattern slot
  bool setPattern(PatternID pat, const PatternArgs *args = nullptr, const Colorset *set = nullptr);
  // This will apply the colorset to all patterns in the mode
  bool setColorset(const Colorset *set);

  // change a single led colorset
  bool setColorsetAt(LedPos pos, const Colorset *set);
  bool setColorsetAt(LedMap map, const Colorset *set);

  // change the multi led colorset
  bool setMultiColorset(const Colorset *set);

  // change a single led pattern
  bool setPatternAt(LedPos pos, PatternID pat, const PatternArgs *args = nullptr, const Colorset *set = nullptr);
  bool setPatternAt(LedPos pos, SingleLedPattern *pat, const Colorset *set = nullptr);
  bool setPatternAt(LedMap pos, PatternID pat, const PatternArgs *args = nullptr, const Colorset *set = nullptr);
  bool setPatternAt(LedMap pos, SingleLedPattern *pat, const Colorset *set = nullptr);

  // change the multi pattern
  bool setMultiPat(PatternID pat, const PatternArgs *args = nullptr, const Colorset *set = nullptr);
  bool setMultiPat(MultiLedPattern *pat, const Colorset *set = nullptr);

  // clear stored patterns in various ways
  void clearPatterns();
  void clearPatternAt(LedPos pos);
  void clearPatternAt(LedMap map);
  void clearMultiPat();

  // clear colorset in various ways
  void clearColorsets();
  void clearColorsetAt(LedPos pos);
  void clearColorsetAt(LedMap map);
  void clearMultiColorset();

  // get the flags associated with this mode
  uint32_t getFlags() const;

  // whether a multi-led pattern is present in the mode
  bool hasMultiLed() const;
  // whether at least one single-led pattern is present in the mode
  bool hasSingleLed() const;
  // whether this mode has all same single-led patterns
  bool hasSameSingleLed() const;
  // whether this mode has sparse single-led patterns (missing some slots)
  bool hasSparseSingleLed() const;

  // get the led map of which singles are set
  LedMap getSingleLedMap() const;

  // whether this mode purely a multi-led pattern with no singles
  bool isMultiLed() const;

#if MODES_TEST == 1
  static void test();
#endif

private:
  // the multi-led pattern slot
  Pattern *m_multiPat;
  // a list of slots for each single led pattern
#if FIXED_LED_COUNT == 0
  // the number of leds the mode is targetting
  uint8_t m_numLeds;
  // list of pointers to Patterns, one for each led
  Pattern **m_singlePats;
#else
  Pattern *m_singlePats[LED_COUNT];
#endif
};

#endif
