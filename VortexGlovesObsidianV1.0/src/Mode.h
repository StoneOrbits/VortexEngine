#ifndef MODE_H
#define MODE_H

#include "LedConfig.h"

class Pattern;
class Colorset;

// Bitflags for the current mode
enum ModeFlags : uint32_t
{
  MODE_FLAG_NONE = 0,

  // the current mode has multiple patterns
  MODE_FLAG_MULTI_PATTERN = (1 << 0),
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
  bool bind(Pattern *pat, Colorset *set, LedPos pos = LED_FIRST);
  // bind a pattern and colorset to a range of LEDs
  bool bindRange(Pattern *pat, Colorset *set, LedPos first, LedPos last);
  // bind a pattern and colorset to all LEDs
  bool bindAll(Pattern *pat, Colorset *set);

  // set and get the mode flags
  void setFlags(ModeFlags flags) { m_flags = flags; }
  ModeFlags getFlags() const { return m_flags; }
  bool hasFlags(ModeFlags flags) const { return (m_flags & flags) == flags; }

  // Get pointer to a pattern/colorset
  Pattern *getPattern(LedPos pos = LED_FIRST) const;
  Colorset *getColorset(LedPos pos = LED_FIRST) const;

  // this will in-place change the pattern or colorset on
  // slot to a copy of the given pattern or colorset
  bool changePattern(const Pattern *pat, LedPos pos);
  bool changeColorset(const Colorset *set, LedPos pos);

  // this will in-place change the pattern or colorset al
  // slots to a copy of the given pattern or colorset
  bool changeAllPatterns(const Pattern *pat);
  bool changeAllColorsets(const Colorset *set);

  // reset a mode back to it's initial state
  void reset();

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

  // ==================
  //  private routine


  // A set of flags for the mode
  ModeFlags m_flags;

  // each led entry has a pattern/colorset entry
  struct LedEntry
  {
    LedEntry() :
      pattern(nullptr), colorset(nullptr)
    {
    }
    LedEntry(Pattern *p, Colorset *c) :
      pattern(p), colorset(c)
    {
    }
    // members
    Pattern *pattern;
    Colorset *colorset;
  };

  // map of led positions => pattern/colorset entries
  LedEntry m_ledEntries[LED_COUNT];
};

#endif
