#ifndef MODE_H
#define MODE_H

#include "LedConfig.h"
#include "Patterns.h"

// minimal stl usage
#include <vector>
using namespace std;

class MultiLedPattern;
class SingleLedPattern;
class SerialBuffer;
class Pattern;
class Colorset;

// Bitflags for the current mode
enum ModeFlags : uint8_t
{
  MODE_FLAG_NONE = 0,

  // the mode is utilizing a multi-led pattern
  MODE_FLAG_MULTI_LED = (1 << 0),
};

// the keyword 'ALL_SLOTS' can be used to refer to all of the
// mode slots at once when using changePattern or changeColorset
#define ALL_SLOTS LED_COUNT

class Mode
{
public:
  Mode();
  ~Mode();

  // initialize the mode to initial state
  void init();

  // Play the mode
  void play();

  // save the mode to serial
  void serialize(SerialBuffer &buffer) const;
  // load the mode from serial
  void unserialize(SerialBuffer &buffer);

  // bind a pattern and colorset to individual LED
  bool bindSingle(SingleLedPattern *pat, const Colorset *set, LedPos pos = LED_FIRST);
  // bind a multi led pattern and colorset to all of the LEDs
  bool bindMulti(MultiLedPattern *pat, const Colorset *set);

  // unbind a single pattern and colorset from the mode
  void unbindSingle(LedPos pos = LED_FIRST);
  // unbind a multi pattern and colorset from the mode
  void unbindMulti();
  // clear and delete all patterns and colorsets from the mode
  void unbindAll();

  // set and get the mode flags
  void setFlags(ModeFlags flags) { m_flags = flags; }
  void addFlags(ModeFlags flags) { m_flags = (ModeFlags)(m_flags | flags); }
  void clearFlags(ModeFlags flags) { m_flags = (ModeFlags)(m_flags & ~flags); }
  ModeFlags getFlags() const { return m_flags; }
  bool hasFlags(ModeFlags flags) const { return (m_flags & flags) == flags; }

  // Get pointer to an individual pattern/colorset
  const Pattern *getPattern(LedPos pos = LED_FIRST) const;
  // get a pointer to a colorset
  const Colorset *getColorset(LedPos pos = LED_FIRST) const;
  // get the pattern ID of the given pattern
  PatternID getPatternID(LedPos pos = LED_FIRST) const;

  // set the pattern/colorset of the mode, if a multi-led pattern is provided then the pos
  // is ignored. If a single led pattern is provided then it will be applied to all LEDS
  // unless a specific LED is provided
  bool setPattern(PatternID pat, LedPos pos = LED_COUNT);
  bool setColorset(const Colorset *set, LedPos pos = LED_COUNT);

private:
  bool setSinglePat(PatternID pat, LedPos pos);
  bool setMultiPat(PatternID pat);

  // erase any stored patterns or colorsets
  void clearPatterns();
  void clearPattern(LedPos pos);
  void clearColorsets();

  // NOTE: Modes *ALLOW* for one pattern and one colorset on each LED
  //       but we are not intending to expose that functionality through
  //       the menus or UI. Instead users will have to customize save
  //       files with separate software to produce custom multi-pattern
  //       or multi-colorset modes
  //
  //       This means in practice all m_pPatterns and m_pColorsets are
  //       separate instances of the same class unless somebody has loaded
  //       a custom savefile
  //
  //       Alternatively a Mode can contain a single MultiLedPattern and 
  //       Colorset, where the single pattern will be responsible for all
  //       of the leds.

  // A set of flags for the mode
  ModeFlags m_flags;

  union {
    // map of led positions => pattern entries
    SingleLedPattern *m_ledEntries[LED_COUNT];
    // or the first one is also a multi led pat
    MultiLedPattern *m_multiPat;
  };
};

#endif
