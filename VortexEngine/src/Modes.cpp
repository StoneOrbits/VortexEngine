#include "Modes.h"

#include "patterns/Pattern.h"

#include "SerialBuffer.h"
#include "TimeControl.h"
#include "ModeBuilder.h"
#include "Colorset.h"
#include "Storage.h"
#include "Buttons.h"
#include "Mode.h"
#include "Leds.h"
#include "Log.h"

// static members
uint8_t Modes::m_curMode = 0;
uint8_t Modes::m_numModes = 0;
Mode *Modes::m_pCurMode = nullptr;
SerialBuffer Modes::m_serializedModes[NUM_MODES];

bool Modes::init()
{
  // try to load the saved settings or set defaults
  if (!loadStorage()) {
    if (!setDefaults()) {
      return false;
    }
    if (!saveStorage()) {
      return false;
    }
  }
  return true;
}

void Modes::cleanup()
{
  clearModes();
}

void Modes::play()
{
  // should be able to initialize the current mode
  if (!m_pCurMode && !initCurMode()) {
    DEBUG_LOG("Error failed to load any modes!");
    return;
  }
  // shortclick cycles to the next mode
  if (g_pButton->onShortClick()) {
    nextMode();
  }
  // empty mode list
  if (!m_numModes) {
    return;
  }
  if (!m_pCurMode) {
    if (!initCurMode()) {
      return;
    }
  }
  // play the current mode
  m_pCurMode->play();
}

bool Modes::loadStorage()
{
  // this is good on memory, but it erases what they have stored
  // before we know whether there is something actually saved
  clearModes();
  SerialBuffer modesBuffer;
  // only read storage if the modebuffer isn't filled
  if (!Storage::read(modesBuffer) || !modesBuffer.size()) {
    DEBUG_LOG("Empty buffer read from storage");
    // this kinda sucks whatever they had loaded is gone
    return false;
  }
  return unserialize(modesBuffer);
}

// NOTE: Flash storage is limited to about 10,000 writes so
//       use this function sparingly!
bool Modes::saveStorage()
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
  //     led1..N [
  //      1 led (0 - 9)
  //      1 pattern id (0 - 255)
  //      colorset1..N [
  //       1 numColors (0 - 255)
  //       hsv1..N [
  //        1 hue (0-255)
  //        1 sat (0-255)
  //        1 val (0-255)
  //       ]
  //      ]

  DEBUG_LOG("Saving modes...");

  // make sure the current mode is saved to the serial mode storage
  saveCurMode();

  // now serialize the modes into the mode buffer
  SerialBuffer modesBuffer;
  serialize(modesBuffer);

  // write the serial buffer to flash storage
  if (!Storage::write(modesBuffer)) {
    DEBUG_LOG("Failed to write storage");
    return false;
  }

  DEBUG_LOGF("Wrote %u bytes to storage", modesBuffer.size());

  return true;
}

// save the mode to serial
void Modes::serialize(SerialBuffer &modesBuffer)
{
  // serialize the number of modes
  modesBuffer.serialize(m_numModes);
  DEBUG_LOGF("Serialized num modes: %u", m_numModes);
  for (uint32_t i = 0; i < m_numModes; ++i) {
    if (i == m_curMode && m_pCurMode) {
      // write the current up to date mode
      m_pCurMode->serialize(modesBuffer);
      continue;
    }
    // serialize each after decompressing
    //m_serializedModes[i].decompress();
    modesBuffer += m_serializedModes[i];
    //m_serializedModes[i].compress();
  }
}

// load the mode from serial
bool Modes::unserialize(SerialBuffer &modesBuffer)
{
  DEBUG_LOG("Loading modes...");
  // this is good on memory, but it erases what they have stored
  // before we know whether there is something actually saved
  clearModes();
  uint8_t numModes = 0;
  modesBuffer.resetUnserializer();
  modesBuffer.unserialize(&numModes);
  if (!numModes) {
    DEBUG_LOG("Did not find any modes");
    // this kinda sucks whatever they had loaded is gone
    return false;
  }
  // in case the one above is removed
  clearModes();
  for (uint8_t i = 0; i < numModes; ++i) {
    if (!addSerializedMode(modesBuffer)) {
      DEBUG_LOGF("Failed to add mode %u after unserialization", i);
      clearModes();
      return false;
    }
  }
  DEBUG_LOGF("Loaded %u modes from storage (%u bytes)", numModes, modesBuffer.size());
  // default can't load anything
  return (m_numModes == numModes);
}

bool Modes::setDefaults()
{
  clearModes();
  // RGB_RED, RGB_YELLOW, RGB_GREEN, RGB_CYAN, RGB_BLUE, RGB_PURPLE
  Colorset defaultSet(RGB_RED, RGB_GREEN, RGB_BLUE); //, RGB_TEAL, RGB_PURPLE, RGB_ORANGE);
  //Colorset defaultSet(HSVColor(254, 255, 255), HSVColor(1, 255, 255), HSVColor(245, 255, 255)); //, RGB_TEAL, RGB_PURPLE, RGB_ORANGE);
  PatternID default_start = PATTERN_CHASER;
  PatternID default_end = PATTERN_LAST;
  addMode(PATTERN_DOPS, &defaultSet);
  // initialize a mode for each pattern with an rgb colorset
  for (PatternID pattern = default_start; pattern <= default_end; ++pattern) {
    // randomize the colorset
    if ((int)pattern == 0) {
      defaultSet.randomize(8);
    }
    // add another mode with the given pattern and colorset
    if (!addMode(pattern, &defaultSet)) {
      ERROR_LOG("Failed to add mode");
      // return false?
    }
    break;
  }
  DEBUG_LOGF("Added default patterns %u through %u", default_start, default_end);

  return true;
}

bool Modes::addSerializedMode(SerialBuffer &serializedMode)
{
  Mode *mode = ModeBuilder::unserialize(serializedMode);
  if (!mode) {
    DEBUG_LOG("Failed to unserialize mode");
    return false;
  }
  mode->init();
  m_serializedModes[m_numModes].clear();
  // re-serialize the mode into the storage buffer
  mode->serialize(m_serializedModes[m_numModes]);
  //m_serializedModes[m_numModes].compress();
  // increment mode counter
  m_numModes++;
  // clean up the mode we used
  delete mode;
  return true;
}

bool Modes::addMode(PatternID id, const Colorset *set)
{
  // max modes
  if (m_numModes >= NUM_MODES || id > PATTERN_LAST || !set) {
    return false;
  }
  Mode *mode = ModeBuilder::make(id, set);
  if (!mode) {
    return false;
  }
  // must init the mode so that it can be serialized
  mode->init();
  // not a very good way to do this but it ensures the mode is
  // added in the same way
  addMode(mode);
  delete mode;
  return true;
}

bool Modes::addMode(const Mode *mode)
{
  // max modes
  if (m_numModes >= NUM_MODES) {
    return false;
  }
  m_serializedModes[m_numModes].clear();
  // serialize the mode so it can be instantiated anytime
  mode->serialize(m_serializedModes[m_numModes]);
  //m_serializedModes[m_numModes].compress();
  m_numModes++;
  return true;
}

// replace current mode with new one, destroying existing one
bool Modes::setCurMode(PatternID id, const Colorset *set)
{
  if (id > PATTERN_LAST || !set) {
    ERROR_LOG("Invalid id or set");
    return false;
  }
  if (!m_pCurMode) {
    return addMode(id, set);
  }
  if (!m_pCurMode->setPattern(id)) {
    DEBUG_LOG("Failed to set pattern of current mode");
    // failed to set pattern?
  }
  if (!m_pCurMode->setColorset(set)) {
    DEBUG_LOG("Failed to set colorset of current mode");
  }
  // initialize the mode with new pattern and colorset
  m_pCurMode->init();
  // save the current mode to the serialized modes storage
  saveCurMode();
  return true;
}

bool Modes::setCurMode(const Mode *mode)
{
  return setCurMode(mode->getPatternID(), mode->getColorset());
}

// the current mode
Mode *Modes::curMode()
{
  // empty mode list
  if (!m_numModes) {
    return nullptr;
  }
  if (!m_pCurMode) {
    if (!initCurMode()) {
      ERROR_LOG("Failed to initialize current mode");
      return nullptr;
    }
  }
  // get current mode
  return m_pCurMode;
}

// iterate to next mode and return it
Mode *Modes::nextMode()
{
  if (!m_numModes) {
    return nullptr;
  }
  // iterate curmode forward 1 till num modes
  m_curMode = (m_curMode + 1) % m_numModes;
  DEBUG_LOGF("Iterated to Next Mode: %u / %u", m_curMode, m_numModes - 1);
  // clear the LEDs when switching modes
  Leds::clearAll();
  // delete the current mode
  delete m_pCurMode;
  m_pCurMode = nullptr;
  if (!initCurMode()) {
    return nullptr;
  }
  // return the new current mode
  return m_pCurMode;
}

void Modes::clearModes()
{
  if (m_pCurMode) {
    delete m_pCurMode;
    m_pCurMode = nullptr;
  }
  for (uint32_t i = 0; i < m_numModes; ++i) {
    m_serializedModes[i].clear();
  }
  m_numModes = 0;
}

bool Modes::initCurMode()
{
  if (m_pCurMode) {
    return true;
  }
  //m_serializedModes[m_curMode].decompress();
  // make sure the unserializer is reset before trying to unserialize it
  m_serializedModes[m_curMode].resetUnserializer();
  DEBUG_LOGF("Current Mode size: %u", m_serializedModes[m_curMode].size());
  m_pCurMode = ModeBuilder::unserialize(m_serializedModes[m_curMode]);
  //m_serializedModes[m_curMode].compress();
  if (!m_pCurMode) {
    return false;
  }
  m_pCurMode->init();
  return true;
}

void Modes::saveCurMode()
{
  if (!m_pCurMode) {
    // if there's no loaded mode currently then there's nothing
    // to save so there's no error
    return;
  }
  // clear the current mode
  m_serializedModes[m_curMode].clear();
  // update the serialized storage
  m_pCurMode->serialize(m_serializedModes[m_curMode]);
  //m_serializedModes[m_numModes].compress();
}
