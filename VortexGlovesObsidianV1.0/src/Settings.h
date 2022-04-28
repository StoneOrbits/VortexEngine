#ifndef SETTINGS_H
#define SETTINGS_H

#include <inttypes.h>

class Mode;

// the maximum number of modes that can be stored
#define NUM_MODES     16

// the starting default brightness
#define DEFAULT_BRIGHTNESS 255

class Settings
{
public:
  // opting for static class here because there should only ever be one
  // Settings control object and I don't like singletons
  static bool init();

  // settings save/load
  static bool load();
  static bool save();

  // set default settings (must save after)
  static bool setDefaults();

  // global brightness
  static uint32_t getBrightness() { return m_brightness; }
  static void setBrightness(uint32_t brightness) { m_brightness = brightness; }

  // import a mode and add it to the mode list
  static bool addMode(Mode *mode);

  // replace current mode with new one, destroying existing one
  static bool setCurMode(Mode *mode);

  // get the current mode
  static Mode *curMode();
  // iterate to next mode and get it
  static Mode *nextMode();

private:
  // the global brightness
  static uint32_t m_brightness;

  // the current mode we're on
  static uint32_t m_curMode;

  // the number of modes loaded
  static uint32_t m_numModes;

  // list of all modes in the gloveset
  static Mode *m_modeList[NUM_MODES];
};

#endif
