#ifndef SETTINGS_H
#define SETTINGS_H

#include "../Serial/ByteStream.h"
#include "../Colors/ColorTypes.h"
#include "../Patterns/Patterns.h"
#include "../Leds/Leds.h"

#include "../VortexConfig.h"

#include <inttypes.h>

class PatternArgs;
class Colorset;
class Mode;

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

  // full save/load to/from buffer
  static bool saveToBuffer(ByteStream &saveBuffer);
  static bool loadFromBuffer(ByteStream &saveBuffer);

  // full save/load to/from storage
  static bool loadStorage();
  static bool saveStorage();

  // saves all modes to a buffer
  static void serialize(ByteStream &buffer);
  // load all modes from a buffer, optionally specify number of leds
  static bool unserialize(ByteStream &buffer);

  // set default settings (must save after)
  static bool setDefaults();

  // shift the current mode to a different position relative to current position
  // negative values for up, positive values for down, 0 for no move
  static bool shiftCurModeUp(uint32_t offset = 1);
  static bool shiftCurModeDown(uint32_t offset = 1);

  // add a new mode in various different ways
  static bool addMode(PatternID id, RGBColor c1, RGBColor c2 = RGB_OFF,
    RGBColor c3 = RGB_OFF, RGBColor c4 = RGB_OFF, RGBColor c5 = RGB_OFF,
    RGBColor c6 = RGB_OFF, RGBColor c7 = RGB_OFF, RGBColor c8 = RGB_OFF);
  static bool addMode(PatternID id, const PatternArgs *args, const Colorset *set);
  static bool addMode(const Mode *mode);

  // add a new mode by unserializing raw (optionally specify number of leds)
  static bool addSerializedMode(ByteStream &serializedMode, uint32_t numLeds = LED_COUNT);
  // add a new mode by loading from a save buffer
  static bool addModeFromBuffer(ByteStream &serializedMode);

  // update the current mode to match the given mode
  static bool updateCurMode(PatternID id, const Colorset *set);
  static bool updateCurMode(const Mode *mode);

  // set the current active mode by index
  static Mode *setCurMode(uint32_t index);

  // get the current mode
  static Mode *curMode();
  // iterate to next mode and return it
  static Mode *nextMode();

  // the number of modes
  static uint8_t numModes() { return m_numModes; }
  static uint8_t curModeIndex() { return m_curMode; }

  // delete the current mode
  static void deleteCurMode();

  // delete all modes in the list
  static void clearModes();

private:
  // linked list of internal mode storage
  class ModeLink {
    ByteStream storedMode;
    ModeLink *next;
  };

  // initialize current mode from ByteStream, optionally force re-init which
  // will destroy the current instantiated mode and re-load it from serial
  static bool initCurMode(bool force = false);
  static void saveCurMode();

  // the current mode we're on
  static uint8_t m_curMode;

  // the number of modes loaded
  static uint8_t m_numModes;

  // the current instantiated mode
  static Mode *m_pCurMode;

  // list of serialized version of bufers
  //static ByteStream m_serializedModes[MAX_MODES];
  static ModeLink *m_storedModes;
};

#endif
