#include "Modes.h"

#include "ModeBuilder.h"
#include "LedControl.h"
#include "Buttons.h"
#include "Mode.h"
#include "Log.h"

// static members
uint32_t Modes::m_curMode = 0;
uint32_t Modes::m_numModes = 0;
Mode *Modes::m_modeList[NUM_MODES] = { nullptr };

bool Modes::init()
{
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
  // initialize the current mode
  if (curMode()) {
    curMode()->init();
  }
  return true;
}

void Modes::play()
{
  // shortclick cycles to the next mode
  if (g_pButton->onShortClick()) {
    nextMode();
  }
  // empty mode list
  if (!m_numModes) {
    return;
  }
  // play the current mode
  m_modeList[m_curMode]->play();
}

bool Modes::load()
{
  // default can't load anything
  return false;
}

bool Modes::save()
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
  for (uint32_t i = 0; i < m_numModes; ++i) {
    m_modeList[i]->serialize();
  }

  return true;
}

bool Modes::setDefaults()
{
  for (PatternID pattern = PATTERN_FIRST; pattern < PATTERN_COUNT; ++pattern) {
    // initialize each pattern with an rgb strobe
    // RGB_RED, RGB_YELLOW, RGB_GREEN, RGB_CYAN, RGB_BLUE, RGB_PURPLE
    if (!addMode(ModeBuilder::make(pattern, RGB_RED, RGB_GREEN, RGB_BLUE))) { //, RGB_TEAL, RGB_PURPLE, RGB_ORANGE))) {
      // error? return false?
    }
  }

  return true;
}

bool Modes::addMode(Mode *mode)
{
  // max modes
  if (m_numModes >= NUM_MODES || !mode) {
    return false;
  }
  m_modeList[m_numModes++] = mode;
  return true;
}

// replace current mode with new one, destroying existing one
bool Modes::setCurMode(Mode *mode)
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
Mode *Modes::curMode()
{
  // empty mode list
  if (!m_numModes) {
    return nullptr;
  }
  // get current mode
  return m_modeList[m_curMode];
}

// iterate to next mode and return it
Mode *Modes::nextMode()
{
  // iterate curmode forward 1 till num modes
  m_curMode = (m_curMode + 1) % m_numModes;
  DEBUGF("Iterated to Next Mode: %d", m_curMode);
  // clear the LEDs when switching modes
  Leds::clearAll();
  // initialize the new mode
  curMode()->init();
  // return the new current mode
  return curMode();
}
