#ifndef SETTINGS_H
#define SETTINGS_H

#include <inttypes.h>

class Mode;

// the maximum number of modes that can be stored
#define NUM_MODES     16

class Settings
{
public:
  Settings();
  ~Settings();

  bool init();

  // settings save/load
  bool load();
  bool save();

  // set default settings (must save after)
  bool setDefaults();

  // global brightness
  uint32_t getBrightness() const { return m_brightness; }
  void setBrightness(uint32_t brightness) { m_brightness = brightness; }

  // import a mode and add it to the mode list
  bool addMode(Mode *mode);

  // replace current mode with new one, destroying existing one
  bool setCurMode(Mode *mode);

  // get the current mode
  Mode *curMode();
  // iterate to next mode and get it
  Mode *nextMode();

private:
  // the global brightness
  uint32_t m_brightness;

  // the current mode we're on
  uint32_t m_curMode;

  // the number of modes loaded
  uint32_t m_numModes;

  // list of all modes in the gloveset
  Mode *m_modeList[NUM_MODES];
};

// should only be one settings object
extern Settings *g_pSettings;

#endif
