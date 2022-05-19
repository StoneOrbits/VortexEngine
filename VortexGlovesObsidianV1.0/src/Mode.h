#ifndef MODE_H
#define MODE_H

#include "LedConfig.h"

class MultiLedPattern;
class SingleLedPattern;
class Colorset;

// Bitflags for the current mode
enum ModeFlags : uint32_t
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
  void serialize() const;
  // load the mode from serial
  void unserialize();

  // bind a pattern and colorset to individual LED
  bool bindSingle(SingleLedPattern *pat, Colorset *set, LedPos pos = LED_FIRST);
  // bind a multi led pattern and colorset to all of the LEDs
  bool bindMulti(MultiLedPattern *pat, Colorset *set);

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
  SingleLedPattern *getSinglePattern(LedPos pos = LED_FIRST) const;
  Colorset *getSingleColorset(LedPos pos = LED_FIRST) const;

  // get pointer to a multi-pattern or the multi-pattern colorset
  MultiLedPattern *getMultiPattern() const;
  Colorset *getMultiColorset() const;

  // this will in-place change the pattern or colorset on
  // slot to a copy of the given pattern or colorset
  bool changePattern(const SingleLedPattern *pat, LedPos pos);
  bool changeColorset(const Colorset *set, LedPos pos);

  // this will in-place change the pattern or colorset al
  // slots to a copy of the given pattern or colorset
  bool changeAllPatterns(const SingleLedPattern *pat);
  bool changeAllColorsets(const Colorset *set);

private:
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

  // each led entry has a SingleLedPattern/colorset entry
  struct LedEntry
  {
    LedEntry() :
      pattern(nullptr), colorset(nullptr)
    {
    }
    LedEntry(SingleLedPattern *p, Colorset *c) :
      pattern(p), colorset(c)
    {
    }
    // members
    SingleLedPattern *pattern;
    Colorset *colorset;
  };

  union {
    // map of led positions => pattern/colorset entries
    LedEntry m_ledEntries[LED_COUNT];
    // just a single multi led pattern and colorset
    struct {
      MultiLedPattern *m_multiPat;
      Colorset *m_multiColorset;
    };
  };
};

#endif
