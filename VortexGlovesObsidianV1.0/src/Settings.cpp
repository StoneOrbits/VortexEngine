#include "Settings.h"

#include "ModeBuilder.h"
#include "Mode.h"

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
  return true;
}

bool Settings::setDefaults()
{
  // initialize the first mode (an rgb strobe)
  addMode(ModeBuilder::make(PATTERN_STROBE, RGB_RED, RGB_GREEN, RGB_BLUE));

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
  
#if 0
  importMode("0, 39, 5, 0, 255, 255, 96, 255, 255, 160, 255, 255, 64, 255, 255, 192, 255, 255, 0, 0, 0, 0, 0, 0, 0, 0, 0");
  importMode("1, 23, 3, 0, 255, 255, 96, 255, 255, 160, 255, 255, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0");
  importMode("2, 5, 3, 0, 255, 255, 96, 255, 255, 160, 255, 255, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0");
  importMode("3, 3, 2, 224, 255, 170, 192, 255, 170, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0");
  importMode("4, 32, 3, 0, 255, 170, 96, 255, 170, 160, 255, 170, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0");
  importMode("5, 5, 4, 0, 255, 120, 160, 255, 170, 64, 255, 170, 96, 255, 170, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0");
  importMode("6, 7, 3, 0, 255, 120, 192, 255, 170, 128, 255, 170, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0");
  importMode("7, 1, 3, 16, 255, 170, 96, 255, 255, 192, 255, 85, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0");
  importMode("8, 17, 3, 16, 255, 255, 128, 255, 85, 160, 255, 255, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0");
  importMode("9, 20, 1, 80, 255, 85, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0");
  importMode("10, 4, 8, 0, 255, 85, 32, 255, 170, 64, 255, 255, 96, 255, 85, 128, 255, 85, 160, 255, 85, 192, 255, 170, 224, 255, 85");
  importMode("11, 19, 3, 144, 0, 0, 144, 0, 255, 96, 255, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0");
  importMode("12, 2, 4, 192, 255, 85, 240, 255, 255, 64, 255, 85, 144, 170, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0");
  importMode("13, 8, 5, 16, 255, 0, 144, 255, 0, 16, 255, 85, 144, 255, 255, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0");
#endif
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
