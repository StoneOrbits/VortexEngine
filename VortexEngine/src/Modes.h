#ifndef SETTINGS_H
#define SETTINGS_H

#include "SerialBuffer.h"
#include "Patterns.h"

#include <inttypes.h>

class Mode;
class Colorset;

// the maximum number of modes that can be stored
// TODO: change this back to 16
#define NUM_MODES     32

class Modes
{
  // private unimplemented constructor
  Modes();

public:
  // opting for static class here because there should only ever be one
  // Modes control object and I don't like singletons
  static bool init();
  static void cleanup();

  // play the current mode
  static void play();

  // settings save/load
  static bool loadStorage();
  static bool saveStorage();

  // saves all modes to a buffer
  static void serialize(SerialBuffer &buffer);
  // load all modes from a buffer
  static bool unserialize(SerialBuffer &buffer);

  // set default settings (must save after)
  static bool setDefaults();

  // add a new mode with a given pattern and colorset
  static bool addMode(PatternID id, const Colorset *set);
  static bool addMode(const Mode *mode);

  // add a new mode by unserializing from a buffer
  static bool addSerializedMode(SerialBuffer &serializedMode);

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
  static bool initCurMode();
  static void saveCurMode();

  // the current mode we're on
  static uint8_t m_curMode;

  // the number of modes loaded
  static uint8_t m_numModes;

  // the current instantiated mode
  static Mode *m_pCurMode;

  // list of serialized version of bufers
  static SerialBuffer m_serializedModes[NUM_MODES];
};

#endif
