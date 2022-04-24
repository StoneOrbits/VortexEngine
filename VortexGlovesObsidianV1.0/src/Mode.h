#ifndef MODE_H
#define MODE_H

#include "LedConfig.h"

#include <map>

class Pattern;
class Colorset;

// Bitflags for the current mode
enum ModeFlags : uint32_t {

  MODE_FLAG_NONE = 0,

  // the current mode has multiple patterns
  MODE_FLAG_MULTI_PATTERN = (1 << 0),
};

class Mode
{
  public:
    Mode();
    ~Mode();

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

    // replace just the pattern or colorset
    bool setPattern(Pattern *pat, LedPos pos = LED_FIRST);
    bool setColorset(Colorset *set, LedPos pos = LED_FIRST);

    // Get patterns/colorsets
    Pattern *getPattern(LedPos pos = LED_FIRST) const;
    Colorset *getColorset(LedPos pos = LED_FIRST) const;

    // Play the mode
    void play();

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

    // A set of flags for the mode
    ModeFlags m_flags;

    // each led entry has a pattern/colorset entry
    struct LedEntry {
      LedEntry() :
        pattern(nullptr), colorset(nullptr) {}
      LedEntry(Pattern *p, Colorset *c) :
        pattern(p), colorset(c) {}
      // members
      Pattern *pattern;
      Colorset *colorset;
    };

    // map of led positions => pattern/colorset entries
    LedEntry m_patternEntries[LED_COUNT];
};

#endif
