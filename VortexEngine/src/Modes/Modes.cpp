#include "Modes.h"
#include "DefaultModes.h"

#include "../VortexEngine.h"

#include "../Patterns/Pattern.h"

#include "../Serial/ByteStream.h"
#include "../Time/TimeControl.h"
#include "../Colors/Colorset.h"
#include "../Storage/Storage.h"
#include "../Buttons/Buttons.h"
#include "../Modes/Mode.h"
#include "../Leds/Leds.h"
#include "../Log/Log.h"

// static members
uint8_t Modes::m_curMode = 0;
uint8_t Modes::m_numModes = 0;
Mode *Modes::m_pCurMode = nullptr;
ByteStream Modes::m_serializedModes[MAX_MODES];

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
#if SAVE_TEMPLATE == 1
  // generate the json data template
  saveTemplate();
#endif
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
  // shortclick on button 2 cycles to the previous mode
  if (g_pButton2->onShortClick()) {
    previousMode();
  }

  // check for empty mode list or missing cur mode
  if (!m_numModes || !m_pCurMode || !initCurMode()) {
    // just keep the leds cleared
    Leds::clearAll();
    return;
  }
  // play the current mode
  m_pCurMode->play();
}

// full save/load to/from buffer
bool Modes::saveToBuffer(ByteStream &modesBuffer)
{
  // serialize the engine version into the modes buffer
  VortexEngine::serializeVersion(modesBuffer);
  // serialize the global brightness
  modesBuffer.serialize((uint8_t)Leds::getBrightness());
  // serialize all modes data into the modesBuffer
  serialize(modesBuffer);
  DEBUG_LOGF("Serialized all modes, uncompressed size: %u", modesBuffer.size());
  if (!modesBuffer.compress()) {
    return false;
  }
  return true;
}

// load modes from a save buffer
bool Modes::loadFromBuffer(ByteStream &modesBuffer)
{
  if (!modesBuffer.decompress()) {
    // failed to decompress?
    return false;
  }
  // reset the unserializer index before unserializing anything
  modesBuffer.resetUnserializer();
  uint8_t major = 0;
  uint8_t minor = 0;
  // unserialize the vortex version
  modesBuffer.unserialize(&major);
  modesBuffer.unserialize(&minor);
  // check the version for incompatibility
  if (!VortexEngine::checkVersion(major, minor)) {
    // incompatible version
    ERROR_LOGF("Incompatible savefile version: %u.%u", major, minor);
    return false;
  }
  // unserialize the global brightness
  uint8_t brightness = 0;
  modesBuffer.unserialize(&brightness);
  if (brightness) {
    Leds::setBrightness(brightness);
  }
  // now just unserialize the list of modes
  return unserialize(modesBuffer);
}

bool Modes::loadStorage()
{
  // this is good on memory, but it erases what they have stored
  // before we know whether there is something actually saved
  clearModes();
  ByteStream modesBuffer;
  // only read storage if the modebuffer isn't filled
  if (!Storage::read(modesBuffer) || !modesBuffer.size()) {
    DEBUG_LOG("Empty buffer read from storage");
    // this kinda sucks whatever they had loaded is gone
    return false;
  }
  return loadFromBuffer(modesBuffer);
}

// NOTE: Flash storage is limited to about 10,000 writes so
//       use this function sparingly!
bool Modes::saveStorage()
{
  DEBUG_LOG("Saving modes...");
  // A ByteStream to hold all the serialized data
  ByteStream modesBuffer;
  // save data to the buffer
  if (!saveToBuffer(modesBuffer)) {
    return false;
  }
  // write the serial buffer to flash storage, this
  // will compress the buffer and include crc/flags
  if (!Storage::write(modesBuffer)) {
    DEBUG_LOG("Failed to write storage");
    return false;
  }
  DEBUG_LOG("Success saving modes to storage");
  return true;
}

// Save all of the modes to a serial buffer
void Modes::serialize(ByteStream &modesBuffer)
{
  // serialize the number of modes
  modesBuffer.serialize(m_numModes);
  // make sure the current mode is saved in case it has changed somehow
  saveCurMode();
  // iterate all of the modes and copy their serial data into the modesBuffer
  for (uint32_t i = 0; i < m_numModes; ++i) {
    // load the mode and initialize it
    Mode tmpMode;
    if (!tmpMode.unserialize(m_serializedModes[i])) {
      continue;
    }
    // initialize before serializing the data
    tmpMode.init();
    // serialize it into the modes buffer
    tmpMode.serialize(modesBuffer);
  }
  DEBUG_LOGF("Serialized num modes: %u", m_numModes);
}

// load all modes from a serial buffer
bool Modes::unserialize(ByteStream &modesBuffer)
{
  DEBUG_LOG("Loading modes...");
  // this is good on memory, but it erases what they have stored before we
  // know whether there is something actually saved in the serial buffer
  clearModes();
  // unserialize the number of modes next
  uint8_t numModes = 0;
  modesBuffer.unserialize(&numModes);
  if (!numModes) {
    DEBUG_LOG("Did not find any modes");
    // this kinda sucks whatever they had loaded is gone
    return false;
  }
  // foreach expected mode
  for (uint8_t i = 0; i < numModes; ++i) {
    // just copy the serialized mode into the internal storage because
    // we store the modes in a serialized manner so that they are smaller
    // then we unpack them when we instantiate the mode
    if (!addSerializedMode(modesBuffer)) {
      DEBUG_LOGF("Failed to add mode %u after unserialization", i);
      // clear work so far?
      clearModes();
      return false;
    }
  }
  DEBUG_LOGF("Loaded %u modes from storage (%u bytes)", numModes, modesBuffer.size());
  return (m_numModes == numModes);
}

#if SAVE_TEMPLATE == 1
// Generate the json template for the data format
void Modes::saveTemplate(int level)
{
  IndentMsg(level, "{");
  IndentMsg(level + 1, "\"Brightness\": %d,", Leds::getBrightness());
  // serialize the number of modes
  IndentMsg(level + 1, "\"NumModes\": %d,", m_numModes);
  IndentMsg(level + 1, "\"Modes\": [");
  for (uint32_t i = 0; i < m_numModes; ++i) {
    m_serializedModes[i].resetUnserializer();
    Mode *pMode = ModeBuilder::unserialize(m_serializedModes[i]);
    if (!pMode) {
      return;
    }
    // need to init the mode for example if it contains hybrid patterns like
    // flower then their child patterns get initialized in the init call
    pMode->init();
    // save the mode template
    IndentMsg(level + 2, "{");
    pMode->saveTemplate(level + 3);
    IndentMsg(level + 2, "},");
    // cleanup the mode
    delete pMode;
  }
  IndentMsg(level + 1, "]");
  IndentMsg(level, "}");
}
#endif

bool Modes::setDefaults()
{
  clearModes();
#if DEMO_ALL_PATTERNS == 1
  // RGB_RED, RGB_YELLOW, RGB_GREEN, RGB_CYAN, RGB_BLUE, RGB_PURPLE
  Colorset defaultSet(RGB_RED, RGB_GREEN, RGB_BLUE); //, RGB_TEAL, RGB_PURPLE, RGB_ORANGE);
  //Colorset defaultSet(HSVColor(254, 255, 255), HSVColor(1, 255, 255), HSVColor(245, 255, 255)); //, RGB_TEAL, RGB_PURPLE, RGB_ORANGE);
  PatternID default_start = PATTERN_MULTI_FIRST;
  PatternID default_end = PATTERN_LAST;
  //defaultSet.randomizeTriadic();
  //defaultSet.randomize(8);
  // initialize a mode for each pattern with an rgb colorset
  for (PatternID pattern = default_start; pattern <= default_end; ++pattern) {
    // add another mode with the given pattern and colorset
    if (!addMode(pattern, &defaultSet)) {
      ERROR_LOG("Failed to add mode");
      // return false?
    }
  }
  DEBUG_LOGF("Added default patterns %u through %u", default_start, default_end);
#else
  // add each default mode with each of the given colors
  for (uint32_t i = 0; i < num_default_modes; ++i) {
    addMode(default_modes[i].patternID, default_modes[i].cols[0], default_modes[i].cols[1],
      default_modes[i].cols[2], default_modes[i].cols[3], default_modes[i].cols[4],
      default_modes[i].cols[5], default_modes[i].cols[6], default_modes[i].cols[7]);
  }
#endif
  return true;
}

bool Modes::addSerializedMode(ByteStream &serializedMode, uint32_t numLeds)
{
  if (m_numModes >= MAX_MODES) {
    return false;
  }
  // we must unserialize then re-serialize here because the
  // input argument may contain other patterns in the buffer
  // so we cannot just assign the input arg to m_serializedModes
  Mode tmpMode;
  if (!tmpMode.unserialize(serializedMode)) {
    return false;
  }
  // initialize the mode
  tmpMode.init();
  m_serializedModes[m_numModes].clear();
  // re-serialize the mode into the storage buffer
  tmpMode.saveToBuffer(m_serializedModes[m_numModes]);
  // increment mode counter
  m_numModes++;
  return true;
}

bool Modes::addModeFromBuffer(ByteStream &serializedMode)
{
  if (m_numModes >= MAX_MODES) {
    return false;
  }
  m_serializedModes[m_numModes] = serializedMode;
  // increment mode counter
  m_numModes++;
  return true;
}

// shift the current mode to a different position relative to current position
// negative values for up, positive values for down, 0 for no move
// TODO: combine this with shiftCurModeDown and refactor
bool Modes::shiftCurModeUp(uint32_t offset)
{
  if (offset > m_curMode) {
    offset = m_curMode;
  }
  uint32_t newPos = m_curMode - offset;
  if (newPos >= m_numModes) {
    return false;
  }
  if (newPos == m_curMode) {
    return true;
  }
  // shift up, iterate from current up to target and shift down
  for (int32_t i = (int32_t)m_curMode; i >= (int32_t)newPos; --i) {
    if (i < 0) {
      break;
    }
    if (i == 0) {
      m_serializedModes[i].clear();
      continue;
    }
    m_serializedModes[i] = m_serializedModes[i - 1];
  }
  // now change the current mode index without changing the m_pCurMode
  // so our current mode index points at the new location, but our m_pCurMode
  // still has the old mode instantiated
  m_curMode = newPos;
  // now we save the m_pCurMode into the m_curMode position
  saveCurMode();
  // then we forcefully re-initialize the current mode
  return initCurMode(true);
}

// TODO: combine this with shiftCurModeUp and refactor
bool Modes::shiftCurModeDown(uint32_t offset)
{
  if ((offset + m_curMode) >= m_numModes) {
    offset = m_numModes - m_curMode;
  }
  uint32_t newPos = m_curMode + offset;
  if (newPos >= m_numModes) {
    return false;
  }
  if (newPos == m_curMode) {
    return true;
  }
  // shift each entry down, iterate from current down to target and shift up
  for (uint32_t i = m_curMode; i < newPos; ++i) {
    if (i == (m_numModes - 1)) {
      m_serializedModes[i].clear();
      continue;
    }
    m_serializedModes[i] = m_serializedModes[i + 1];
  }
  // now change the current mode index without changing the m_pCurMode
  // so our current mode index points at the new location, but our m_pCurMode
  // still has the old mode instantiated
  m_curMode = newPos;
  // now we save the m_pCurMode into the m_curMode position
  saveCurMode();
  // then we forcefully re-initialize the current mode
  return initCurMode(true);
}

bool Modes::addMode(PatternID id, RGBColor c1, RGBColor c2, RGBColor c3,
    RGBColor c4, RGBColor c5, RGBColor c6, RGBColor c7, RGBColor c8)
{
  Colorset set(c1, c2, c3, c4, c5, c6, c7, c8);
  return addMode(id, nullptr, &set);
}

bool Modes::addMode(PatternID id, const PatternArgs *args, const Colorset *set)
{
  // max modes
  if (m_numModes >= MAX_MODES || id > PATTERN_LAST) {
    return false;
  }
  Mode tmpMode(id, args, set);
  // must init the mode so that it can be serialized
  tmpMode.init();
  // not a very good way to do this but it ensures the mode is
  // added in the same way
  return addMode(&tmpMode);
}

bool Modes::addMode(const Mode *mode)
{
  // max modes
  if (m_numModes >= MAX_MODES) {
    return false;
  }
  m_serializedModes[m_numModes].clear();
  // serialize the mode so it can be instantiated anytime
  if (!mode->saveToBuffer(m_serializedModes[m_numModes])) {
    return false;
  }
  m_numModes++;
  return true;
}

// replace current mode with new one, destroying existing one
bool Modes::updateCurMode(PatternID id, const Colorset *set)
{
  if (id > PATTERN_LAST || !set) {
    ERROR_LOG("Invalid id or set");
    return false;
  }
  if (!m_pCurMode) {
    return addMode(id, nullptr, set);
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

bool Modes::updateCurMode(const Mode *mode)
{
  return updateCurMode(mode->getPatternID(), mode->getColorset());
}

// set the current active mode by index
Mode *Modes::setCurMode(uint32_t index)
{
  if (!m_numModes) {
    return nullptr;
  }
  // update the current mode and ensure it's within range
  m_curMode = (index) % m_numModes;
  // clear the LEDs when switching modes
  Leds::clearAll();
  // delete the current mode
  delete m_pCurMode;
  m_pCurMode = nullptr;
  // re-initialize the current mode
  if (!initCurMode()) {
    return nullptr;
  }
  // log the change
  DEBUG_LOGF("Switch to Mode: %u / %u (pattern id: %u)",
    m_curMode, m_numModes - 1, m_pCurMode->getPatternID());
  // return the new current mode
  return m_pCurMode;
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
  // iterate the cur mode forward
  return setCurMode(m_curMode + 1);
}

// iterate to next mode and return it
Mode* Modes::previousMode()
{
  if (!m_numModes) {
    return nullptr;
  }
  // iterate the cur mode backwards
  if (!m_curMode) {
    return setCurMode(numModes() - 1);
  }
  return setCurMode(m_curMode - 1);
}

void Modes::deleteCurMode()
{
  if (!m_numModes) {
    return;
  }
  if (m_pCurMode) {
    delete m_pCurMode;
    m_pCurMode = nullptr;
  }
  // if this is the last mode just clear 0 and return
  if (!--m_numModes) {
    m_serializedModes[0].clear();
    return;
  }
  for (uint32_t i = m_curMode; i < m_numModes; ++i) {
    // move the next entry down one
    m_serializedModes[i] = m_serializedModes[i + 1];
  }
  m_serializedModes[m_numModes].clear();
  // note m_numModes has been decremented now
  if (m_curMode >= m_numModes) {
    m_curMode = m_numModes - 1;
  }

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

bool Modes::initCurMode(bool force)
{
  // force will force the current mode to re-initialize
  if (force) {
    if (m_pCurMode) {
      delete m_pCurMode;
      m_pCurMode = nullptr;
    }
  }
  // if the current mode is already initialized, or we don't have
  // any modes at all then we're technically successful
  if (m_pCurMode || !m_numModes) {
    return true;
  }
  // make sure the unserializer is reset before trying to unserialize it
  m_serializedModes[m_curMode].resetUnserializer();
  DEBUG_LOGF("Current Mode size: %u", m_serializedModes[m_curMode].size());
  // use the mode builder to unserialize the mode
  m_pCurMode = new Mode();
  if (!m_pCurMode) {
    // failure
    return false;
  }
  if (!m_pCurMode->loadFromBuffer(m_serializedModes[m_curMode])) {
    return false;
  }
  // re-compress the buffer because loadFromBuffer will decompress it
  m_serializedModes[m_curMode].compress();
  if (!m_pCurMode) {
    // unable to unserialize a mode, empty modes?
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
  m_pCurMode->saveToBuffer(m_serializedModes[m_curMode]);
}