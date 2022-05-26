#ifndef SETTINGS_H
#define SETTINGS_H

#include "Patterns.h"

#include <inttypes.h>

class Mode;
class Colorset;

// the maximum number of modes that can be stored
#define NUM_MODES     16

class Modes
{
  // private unimplemented constructor
  Modes();

public:
  // opting for static class here because there should only ever be one
  // Modes control object and I don't like singletons
  static bool init();

  // play the current mode
  static void play();

  // settings save/load
  static bool load();
  static bool save();

  // set default settings (must save after)
  static bool setDefaults();

  // add a new mode with a given pattern and colorset
  static bool addMode(PatternID id, const Colorset *set);
  static bool addMode(Mode *mode);

  // update the current mode to match the given mode
  static bool setCurMode(PatternID id, const Colorset *set);
  static bool setCurMode(const Mode *mode);

  // get the current mode
  static Mode *curMode();
  // iterate to next mode and return it
  static Mode *nextMode();

  // delete all modes in the list
  static void clearModes();

private:
  // the current mode we're on
  static uint8_t m_curMode;

  // the number of modes loaded
  static uint8_t m_numModes;

  // list of all modes in the gloveset
  static Mode *m_modeList[NUM_MODES];
};

#endif
