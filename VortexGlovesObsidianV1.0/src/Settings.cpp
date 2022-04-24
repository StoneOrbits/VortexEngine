#include "Settings.h"

#include "ModeBuilder.h"
#include "Mode.h"

#include <Arduino.h>

Settings *g_pSettings = nullptr;

Settings::Settings() :
  m_brightness(0),
  m_curMode(0),
  m_numModes(0),
  m_modeList()
{
}

Settings::~Settings()
{
  g_pSettings = nullptr;
}

bool Settings::init()
{
  if (g_pSettings) {
    return false;
  }

  // try to load the saved settings
  if (!load()) {
    // if nothing can be loaded, try to set defaults
    if (!setDefaults()) {
      // fatal error
      return false;
    }
    // save the newly set defaults
    if (!save()) {
      // error
    }
  }

  g_pSettings = this;
  return true;
}

bool Settings::load()
{
  // default can't load anything
  return false;
}

bool Settings::save()
{
  // legacy data format:
  //  4 num
  //  4 pattern num
  //  4 num colors
  //   4 hue
  //   4 sat
  //   4 val
  //
  // new data format:
  //  1 num modes (1-255) 
  //   4 mode flags (*)
  //   1 num leds (1 - 10)
  //     led1..N {
  //      1 led (0 - 9)
  //      1 pattern id (0 - 255)
  //      colorset1..N {
  //       1 numColors (0 - 255)
  //       hsv1..N {
  //        1 hue (0-255)
  //        1 sat (0-255)
  //        1 val (0-255)
  //       }
  //      }

  Serial.print(m_numModes);
  for(uint32_t i = 0; i < m_numModes; ++i) {
    m_modeList[i]->serialize();
  }

  return true;
}

bool Settings::setDefaults()
{
  for (PatternID pattern = PATTERN_FIRST; pattern < PATTERN_COUNT; ++pattern) {
    // initialize each pattern with an rgb strobe
    if (!addMode(ModeBuilder::make(pattern, RGB_RED, RGB_GREEN, RGB_BLUE))) {
      // error? return false?
    }
  }

  return true;
}

bool Settings::addMode(Mode *mode)
{
  // max modes
  if (m_numModes >= NUM_MODES) {
    return false;
  }
  m_modeList[m_numModes++] = mode;
  return true;
}

// replace current mode with new one, destroying existing one
bool Settings::setCurMode(Mode *mode)
{
  if (!mode) {
    return false;
  }
  if (m_modeList[m_curMode]) {
    // TODO: better management, this is messy
    delete m_modeList[m_curMode];
  }
  m_modeList[m_curMode] = mode;
  return true;
}

// the current mode
Mode *Settings::curMode()
{
  // empty mode list
  if (!m_numModes) {
    return nullptr;
  }
  // get current mode
  return m_modeList[m_curMode];
}

// iterate to next mode and return it
Mode *Settings::nextMode()
{
  // iterate curmode forward 1 till num modes
  m_curMode = (m_curMode + 1) % m_numModes;
  // return the new current mode
  return curMode();
}
