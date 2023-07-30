#include "VortexLib.h"

// VortexEngine includes
#include "VortexEngine.h"
#include "Buttons/Button.h"
#include "Serial/ByteStream.h"
#include "Wireless/IRReceiver.h"
#include "Wireless/VLReceiver.h"
#include "Patterns/PatternBuilder.h"
#include "Time/TimeControl.h"
#include "Patterns/Pattern.h"
#include "Colors/Colorset.h"
#include "Storage/Storage.h"
#include "Random/Random.h"
#include "Time/Timings.h"
#include "Menus/Menus.h"
#include "Modes/Modes.h"
#include "Menus/Menu.h"
#include "Modes/Mode.h"

#ifndef _MSC_VER
#include <sys/ioctl.h>
#include <unistd.h>
#include <stdio.h>
#endif

#ifdef WASM
#include <emscripten/bind.h>
#include <emscripten/val.h>

using namespace emscripten;

#if 0
EMSCRIPTEN_BINDINGS(vortex_engine)
{
EMSCRIPTEN_BINDINGS(vortex_engine) {
  value_object<ByteStream>("ByteStream")
    .field("data"
      .element(&Point2f::x)
      .element(&Point2f::y)
      ;
  class_<ByteStream>("ByteStream")
    .constructor<uint32_t, const uint8_t *>()
    .function("data", &ByteStream::data)
    ;
  class_<Vortex>("Vortex")
    .class_function("init", &Vortex::init)
    .class_function("cleanup", &Vortex::cleanup)
    //.class_function("getStorageStats", &Vortex::getStorageStats)
    ;
}
#endif
#endif

using namespace std;

// static vortex data
char Vortex::m_lastCommand = 0;
deque<ByteStream> Vortex::m_undoBuffer;
uint32_t Vortex::m_undoLimit = 0;
uint32_t Vortex::m_undoIndex = 0;
bool Vortex::m_undoEnabled = true;
VortexCallbacks *Vortex::m_storedCallbacks;
FILE *Vortex::m_consoleHandle = nullptr;
#if LOG_TO_FILE == 1
FILE *Vortex::m_logHandle = nullptr;
#endif
deque<Vortex::VortexButtonEvent> Vortex::m_buttonEventQueue;
bool Vortex::m_initialized = false;
uint32_t Vortex::m_buttonsPressed = 0;
string Vortex::m_commandLog;
bool Vortex::m_commandLogEnabled = false;
bool Vortex::m_lockstepEnabled = false;
bool Vortex::m_storageEnabled = false;
bool Vortex::m_sleepEnabled = true;
bool Vortex::m_lockEnabled = true;

#ifdef _MSC_VER
#include <Windows.h>
BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
  // Perform actions based on the reason for calling.
  switch (fdwReason) {
  case DLL_PROCESS_ATTACH:
    break;
  case DLL_THREAD_ATTACH:
    break;
  case DLL_THREAD_DETACH:
    break;
  case DLL_PROCESS_DETACH:
    break;
  }
  return TRUE;  // Successful DLL_PROCESS_ATTACH.
}
#endif

// called when engine reads digital pins, use this to feed button presses to the engine
long VortexCallbacks::checkPinHook(uint32_t pin)
{
  // leaving the pin checking to user-overrides rather than
  // checking the default pins here
  return 1; // HIGH
}

Vortex::Vortex()
{
  // default callbacks pointer that can be replaced with a derivative
  // of the VortexCallbacks class
  m_storedCallbacks = new VortexCallbacks;
  if (!m_storedCallbacks) {
    // error! out of memory
  }
}

Vortex::~Vortex()
{
  cleanup();
}

bool Vortex::init(VortexCallbacks *callbacks)
{
  if (m_storedCallbacks) {
    delete m_storedCallbacks;
  }
  // store the callbacks object
  m_storedCallbacks = callbacks;

#if LOG_TO_CONSOLE == 1
  if (!m_consoleHandle) {
#ifdef _MSC_VER
    AllocConsole();
    freopen_s(&m_consoleHandle, "CONOUT$", "w", stdout);
#else
    m_consoleHandle = stdout;
#endif
  }
#endif
#if LOG_TO_FILE == 1
  if (!m_logHandle) {
    time_t t = time(nullptr);
    tm tm;
    localtime_s(&tm, &t);
    ostringstream oss;
    oss << put_time(&tm, "%d-%m-%Y-%H-%M-%S");
    oss << "." << GetCurrentProcessId();
    string filename = VORTEX_LOG_NAME "." + oss.str() + ".txt";
    int err = fopen_s(&m_logHandle, filename.c_str(), "w");
    if (err != 0 || !m_logHandle) {
      MessageBox(NULL, "Failed to open logfile", to_string(err).c_str(), 0);
      return false;
    }
  }
#endif

  // init the engine
  VortexEngine::init();
  // clear the modes
  //Modes::clearModes();
  // save and set undo buffer
  doSave();

  m_initialized = true;

  return true;
}

void Vortex::cleanup()
{
  VortexEngine::cleanup();
  if (m_storedCallbacks) {
    delete m_storedCallbacks;
    m_storedCallbacks = nullptr;
  }
  m_initialized = false;
}

void Vortex::handleRepeat(char c)
{
  if (!isdigit(c) || !m_lastCommand) {
    return;
  }
  int repeatAmount = c - '0';
  char newc = 0;
  // read the digits into the repeatAmount
  while (1) {
    newc = getchar();
    if (!isdigit(newc)) {
      // stop once we reach a non digit
      break;
    }
    // accumulate the digits into the repeat amount
    repeatAmount = (repeatAmount * 10) + (newc - '0');
  }
  if (repeatAmount > 0) {
    // offset repeat amount by exactly 1 because it's already done
    repeatAmount--;
  }
  // shove the last non-digit back into the stream
  ungetc(newc, stdin);
  DEBUG_LOGF("Repeating last command (%c) x%u times", m_lastCommand, repeatAmount);
  m_commandLog += to_string(repeatAmount);
  // check to see if we are repeating a 'rapid click' which is a special case
  // because the rapid click command itself is composed or 'r' and a repeat count
  // to designate how many rapid clicks to deliver
  if (m_lastCommand == 'r') {
    // the repeat amount is normally decremented to account for the first command
    // so we add 1 to counter-act that logic above because the initial 'r' does nothing
    // unlike most other commands that are repeated
    Vortex::rapidClick(repeatAmount + 1);
    // don't actually repeat the command just return
    return;
  }
  // repeat the last command that many times
  while (repeatAmount > 0) {
    doCommand(m_lastCommand);
    repeatAmount--;
  }
}

// injects a command into the engine, the engine will parse one command
// per tick so multiple commands will be queued up
void Vortex::doCommand(char c)
{
  if (!isprint(c)) {
    return;
  }

  m_commandLog += c;

  switch (c) {
  case 'c':
    if (m_lastCommand != c) {
      DEBUG_LOG("Injecting short click");
    }
    Vortex::shortClick();
    break;
  case 'l':
    if (m_lastCommand != c) {
      DEBUG_LOG("Injecting long click");
    }
    Vortex::longClick();
    break;
  case 'm':
    if (m_lastCommand != c) {
      DEBUG_LOG("Injecting menu enter click");
    }
    Vortex::menuEnterClick();
    break;
  case 'a':
    if (m_lastCommand != c) {
      DEBUG_LOG("Injecting adv menu enter click");
    }
    Vortex::advMenuEnterClick();
    break;
  case 'd':
    if (m_lastCommand != c) {
      DEBUG_LOG("Injecting adv menu enter click");
    }
    Vortex::deleteColClick();
    break;
  case 's':
    if (m_lastCommand != c) {
      DEBUG_LOG("Injecting sleep click");
    }
    Vortex::sleepClick();
    break;
  case 'f':
    if (m_lastCommand != c) {
      DEBUG_LOG("Injecting force sleep click");
    }
    Vortex::forceSleepClick();
    break;
  case 'q':
    //case '\n':
    if (m_lastCommand != c) {
      DEBUG_LOG("Injecting quit click");
    }
    Vortex::quitClick();
    break;
  case 't':
    if (Vortex::isButtonPressed()) {
      if (m_lastCommand != c) {
        DEBUG_LOG("Injecting release");
      }
      Vortex::releaseButton();
    } else {
      if (m_lastCommand != c) {
        DEBUG_LOG("Injecting press");
      }
      Vortex::pressButton();
    }
    break;
  case 'r':
    // do nothing for the initial 'r', handleRepeat() will handle the numeric
    // repeat count that follows this letter and issue the rapidClick(amt) call
    break;
  case 'w':
    if (m_lastCommand != c) {
      DEBUG_LOG("Injecting wait");
    }
    Vortex::sendWait();
    break;
  default:
    handleRepeat(c);
    // return instead of break because this isn't a command
    return;
  }
  m_lastCommand = c;
}

// whether the engine has sleep enabled, if disabled it will always be awake
void Vortex::setSleepEnabled(bool enable)
{
  m_sleepEnabled = enable;
}

bool Vortex::sleepEnabled()
{
  return m_sleepEnabled;
}

// whether the engine is sleeping, and/or to enter sleep
void Vortex::enterSleep(bool save)
{
  VortexEngine::enterSleep(save);
}

bool Vortex::isSleeping()
{
  return VortexEngine::isSleeping();
}

bool Vortex::tick()
{
  if (!m_initialized) {
    cleanup();
    return false;
  }
  // On linux we need to poll stdin for input to handle commands
#if !defined(_MSC_VER) && !defined(WASM)
  // use ioctl to determine how many characters are on stdin so that
  // we don't call getchar() too many times and accidentally block
  uint32_t numInputs = 0;
  ioctl(STDIN_FILENO, FIONREAD, &numInputs);
  if (m_lockstepEnabled && !numInputs) {
    // don't tick till we have input
    return true;
  }
  // iterate the number of inputs on stdin and parse each letter
  // into a command for the engine, this will inject all of the commands
  // that are available into the engine but that doesn't necessarily
  // mean that the engine will do anything with them right away
  for (uint32_t i = 0; i < numInputs; ++i) {
    doCommand(getchar());
  }
#endif
  // tick the vortex engine forward
  VortexEngine::tick();
  return true;
}


void Vortex::installCallbacks(VortexCallbacks *callbacks)
{
  m_storedCallbacks = callbacks;
}

void Vortex::setInstantTimestep(bool timestep)
{
  Time::setInstantTimestep(timestep);
}

// send various clicks
void Vortex::shortClick(uint32_t buttonIndex)
{
  m_buttonEventQueue.push_back(VortexButtonEvent(buttonIndex, EVENT_SHORT_CLICK));
}

void Vortex::longClick(uint32_t buttonIndex)
{
  m_buttonEventQueue.push_back(VortexButtonEvent(buttonIndex, EVENT_LONG_CLICK));
}

void Vortex::menuEnterClick(uint32_t buttonIndex)
{
  m_buttonEventQueue.push_back(VortexButtonEvent(buttonIndex, EVENT_MENU_ENTER_CLICK));
}

void Vortex::advMenuEnterClick(uint32_t buttonIndex)
{
  m_buttonEventQueue.push_back(VortexButtonEvent(buttonIndex, EVENT_ADV_MENU_ENTER_CLICK));
}

void Vortex::deleteColClick(uint32_t buttonIndex)
{
  m_buttonEventQueue.push_back(VortexButtonEvent(buttonIndex, EVENT_DELETE_COL));
}

void Vortex::sleepClick(uint32_t buttonIndex)
{
  m_buttonEventQueue.push_back(VortexButtonEvent(buttonIndex, EVENT_SLEEP_CLICK));
}

void Vortex::forceSleepClick(uint32_t buttonIndex)
{
  m_buttonEventQueue.push_back(VortexButtonEvent(buttonIndex, EVENT_FORCE_SLEEP_CLICK));
}

void Vortex::pressButton(uint32_t buttonIndex)
{
  m_buttonsPressed |= (1 << buttonIndex);
}

void Vortex::releaseButton(uint32_t buttonIndex)
{
  m_buttonsPressed &= ~(1 << buttonIndex);
}

bool Vortex::isButtonPressed(uint32_t buttonIndex)
{
  return (m_buttonsPressed & (1 << buttonIndex)) != 0;
}

void Vortex::sendWait(uint32_t amount)
{
  // reusing the button index as the wait amount
  m_buttonEventQueue.push_back(VortexButtonEvent(amount, EVENT_WAIT));
}

void Vortex::rapidClick(uint32_t amount)
{
  // reusing the button index as the consecutive press amount
  m_buttonEventQueue.push_back(VortexButtonEvent(amount, EVENT_RAPID_CLICK));
}

Mode *Vortex::getMenuDemoMode()
{
  Menu *pMenu = Menus::curMenu();
  if (!pMenu) {
    return nullptr;
  }
  return &pMenu->m_previewMode;
}

void Vortex::quitClick()
{
  m_buttonEventQueue.push_back(VortexButtonEvent(0, EVENT_QUIT_CLICK));
}

void Vortex::IRDeliver(uint32_t timing)
{
#if IR_ENABLE_RECEIVER == 1
  IRReceiver::handleIRTiming(timing);
#endif
}

void Vortex::VLDeliver(uint32_t timing)
{
#if VL_ENABLE_RECEIVER == 1
  VLReceiver::handleVLTiming(timing);
#endif
}

void Vortex::getStorageStats(uint32_t *outTotal, uint32_t *outUsed)
{
  if (outTotal) {
    *outTotal = VortexEngine::totalStorageSpace();
  }
  if (outUsed) {
    *outUsed = VortexEngine::savefileSize();
  }
}

void Vortex::loadStorage()
{
  Modes::loadStorage();
}

void Vortex::openRandomizer()
{
  Menus::openMenu(MENU_RANDOMIZER);
}

void Vortex::openColorSelect()
{
  Menus::openMenu(MENU_COLOR_SELECT);
}

void Vortex::openPatternSelect()
{
  Menus::openMenu(MENU_PATTERN_SELECT);
}

void Vortex::openGlobalBrightness()
{
  Menus::openMenu(MENU_GLOBAL_BRIGHTNESS);
}

void Vortex::openFactoryReset()
{
  Menus::openMenu(MENU_FACTORY_RESET);
}

void Vortex::openModeSharing()
{
  Menus::openMenu(MENU_MODE_SHARING);
}

void Vortex::openEditorConnection()
{
#if ENABLE_EDITOR_CONNECTION == 1
  Menus::openMenu(MENU_EDITOR_CONNECTION);
#endif
}

bool Vortex::getModes(ByteStream &outStream)
{
  // now serialize all the modes
  Modes::saveToBuffer(outStream);
  return true;
}

bool Vortex::setModes(ByteStream &stream, bool save)
{
  // now unserialize the stream of data that was read
  if (!Modes::loadFromBuffer(stream)) {
    //printf("Unserialize failed\n");
    return false;
  }
  return !save || doSave();
}

bool Vortex::getCurMode(ByteStream &outStream)
{
  Mode *pMode = Modes::curMode();
  if (!pMode) {
    return false;
  }
  // save to ensure we get the correct mode, not using doSave() because it causes
  // an undo buffer entry to be added
  if (!Modes::saveStorage()) {
    return false;
  }
  return pMode->saveToBuffer(outStream);
}

uint32_t Vortex::curModeIndex()
{
  return Modes::curModeIndex();
}

uint32_t Vortex::numModes()
{
  return Modes::numModes();
}

uint32_t Vortex::numLedsInMode()
{
  Mode *pMode = Modes::curMode();
  if (!pMode) {
    return false;
  }
  return pMode->getLedCount();
}

bool Vortex::addNewMode(Random *pRandCtx, bool save)
{
  Colorset set;
  Random ctx;
  if (!pRandCtx) {
    pRandCtx = &ctx;
  }
  set.randomize(*pRandCtx);
  // create a random pattern ID from all patterns
  PatternID randomPattern;
  do {
    // continuously re-randomize the pattern so we don't get solids
    randomPattern = (PatternID)pRandCtx->next16(PATTERN_FIRST, PATTERN_COUNT);
  } while (randomPattern == PATTERN_SOLID);
  if (!Modes::addMode(randomPattern, nullptr, &set)) {
    return false;
  }
  return !save || doSave();
}

bool Vortex::addNewMode(ByteStream &stream, bool save)
{
  if (!Modes::addModeFromBuffer(stream)) {
    return false;
  }
  return !save || doSave();
}

bool Vortex::setCurMode(uint32_t index, bool save)
{
  if (index >= Modes::numModes()) {
    return true;
  }
  Modes::setCurMode(index);
  return !save || doSave();
}

bool Vortex::nextMode(bool save)
{
  if (!Modes::numModes()) {
    return true;
  }
  Modes::nextMode();
  return !save || doSave();
}

bool Vortex::delCurMode(bool save)
{
  if (!Modes::numModes()) {
    return true;
  }
  Modes::deleteCurMode();
  return !save || doSave();
}

bool Vortex::shiftCurMode(int8_t offset, bool save)
{
  if (!Modes::numModes()) {
    return true;
  }
  if (offset == 0) {
    return true;
  }
  Modes::shiftCurMode(offset);
  return !save || doSave();
}

bool Vortex::setPattern(PatternID id, const PatternArgs *args, const Colorset *set, bool save)
{
  Mode *pMode = Modes::curMode();
  if (!pMode) {
    return false;
  }
  if (!pMode->setPattern(id, LED_ANY, args, set)) {
    return false;
  }
  pMode->init();
  return !save || doSave();
}

PatternID Vortex::getPatternID(LedPos pos)
{
  Mode *pMode = Modes::curMode();
  if (!pMode) {
    return PATTERN_NONE;
  }
  return pMode->getPatternID(pos);
}

string Vortex::getPatternName(LedPos pos)
{
  return patternToString(getPatternID(pos));
}

string Vortex::getModeName()
{
  Mode *pMode = Modes::curMode();
  if (!pMode) {
    return patternToString(PATTERN_NONE);
  }
  if (pMode->isMultiLed()) {
    return patternToString(pMode->getPatternID(LED_MULTI));
  }
  // can't use isSampleSingleLed because that will compare the entire
  // pattern for differences in any single led pattern, we only care
  // about the pattern id being different
  bool all_same_id = true;
  PatternID first = pMode->getPatternID(LED_FIRST);
  for (uint32_t i = LED_FIRST + 1; i < numLedsInMode(); ++i) {
    // if any don't match 0 then no good
    if (pMode->getPatternID((LedPos)i) != first) {
      all_same_id = false;
      break;
    }
  }
  // if they're all the same we can return just the first led pattern name
  if (all_same_id) {
    return patternToString(getPatternID(LED_FIRST));
  }
  // mixed single led pattern with different pattern names
  return "custom";
}

bool Vortex::setPatternAt(LedPos pos, PatternID id,
  const PatternArgs *args, const Colorset *set, bool save)
{
  Mode *pMode = Modes::curMode();
  if (!pMode) {
    return false;
  }
  if (!pMode->setPattern(id, pos, args, set)) {
    return false;
  }
  pMode->init();
  return !save || doSave();
}

bool Vortex::getColorset(LedPos pos, Colorset &set)
{
  Mode *pMode = Modes::curMode();
  if (!pMode) {
    return false;
  }
  set = pMode->getColorset(pos);
  return true;
}

bool Vortex::setColorset(LedPos pos, const Colorset &set, bool save)
{
  Mode *pMode = Modes::curMode();
  if (!pMode) {
    return false;
  }
  if (!pMode->setColorset(set, pos)) {
    return false;
  }
  pMode->init();
  return !save || doSave();
}

bool Vortex::getPatternArgs(LedPos pos, PatternArgs &args)
{
  Mode *pMode = Modes::curMode();
  if (!pMode) {
    return false;
  }
  Pattern *pat = pMode->getPattern(pos);
  if (!pat) {
    return false;
  }
  pat->getArgs(args);
  return true;
}

bool Vortex::setPatternArgs(LedPos pos, PatternArgs &args, bool save)
{
  Mode *pMode = Modes::curMode();
  if (!pMode) {
    return false;
  }
  Pattern *pat = nullptr;
  switch (pos) {
  case LED_ANY:
  case LED_ALL:
    // fallthrough
#if VORTEX_SLIM == 0
  case LED_MULTI:
    pat = pMode->getPattern(LED_MULTI);
    if (pat) {
      pat->setArgs(args);
    }
    if (pos == LED_MULTI) {
      // don't fallthrough if actually multi, it's possible
      // we got here by falling through from LED_ALL
      pMode->init();
      return !save || doSave();
    }
    // fall through if LED_ALL and change the single leds
#endif
  case LED_ALL_SINGLE:
    for (LedPos pos = LED_FIRST; pos < LED_COUNT; ++pos) {
      pat = pMode->getPattern(pos);
      if (pat) {
        pat->setArgs(args);
      }
    }
    pMode->init();
    // actually break here
    return !save || doSave();
  default:
    if (pos >= LED_COUNT) {
      return false;
    }
    pat = pMode->getPattern(pos);
    if (!pat) {
      return false;
    }
    pat->setArgs(args);
    pMode->init();
    return !save || doSave();
  }
  return false;
}

bool Vortex::isCurModeMulti()
{
  Mode *pMode = Modes::curMode();
  if (!pMode) {
    return false;
  }
  return pMode->isMultiLed();
}

string Vortex::patternToString(PatternID id)
{
  // I wish there was a way to do this automatically but it would be
  // quite messy and idk if it's worth it
  static const char *patternNames[] = {
    "strobe", "hyperstrobe", "dops", "strobie", "dopy", "ultradops", "strobegap",
    "hypergap", "dopgap", "strobiegap", "dopygap", "ultragap", "blinkie",
    "ghostcrush", "doubledops", "chopper", "dashgap", "dashdops", "dashcrush",
    "ultradash", "gapcycle", "dashcycle", "tracer", "ribbon", "miniribbon",
    "blend", "blendstrobe", "blendstrobegap", "complementary_blend",
    "complementary_blendstrobe", "complementary_blendstrobegap", "solid",
    "hueshift", "theater_chase", "chaser", "zigzag", "zipfade", "drip",
    "dripmorph", "crossdops", "doublestrobe", "meteor", "sparkletrace",
    "vortexwipe", "warp", "warpworm", "snowball", "lighthouse", "pulsish",
    "fill", "bounce", "splitstrobie", "backstrobe", "materia",
  };
  if (sizeof(patternNames) / sizeof(patternNames[0]) != PATTERN_COUNT) {
    // if you see this it means the list of strings above is not equal to
    // the number of patterns in the enum, so you need to update the list
    // above to match the enum.
    return "fix patternToString()";
  }
  if (id == PATTERN_NONE || id >= PATTERN_COUNT) {
    return "none";
  }
  return patternNames[id];
}

// this shouldn't change much so this is fine
string Vortex::ledToString(LedPos pos)
{
  return "led " + to_string(pos);
}

// the number of custom parameters for any given pattern id
uint32_t Vortex::numCustomParams(PatternID id)
{
  return PatternBuilder::numDefaultArgs(id);
}

vector<string> Vortex::getCustomParams(PatternID id)
{
  Pattern *pat = PatternBuilder::make(id);
  vector<string> params;
  if (!pat) {
    return params;
  }
  for (uint32_t i = 0; i < pat->getNumArgs(); ++i) {
    params.push_back(pat->getArgName(i));
  }
  delete pat;
  return params;
}

void Vortex::setUndoBufferLimit(uint32_t limit)
{
  m_undoLimit = limit;
}

bool Vortex::addUndoBuffer()
{
  if (!m_undoEnabled) {
    return true;
  }
  ByteStream modes;
  getModes(modes);
  if (!modes.size()) {
    return false;
  }
  // only save an undo if the buffer is different
  if (m_undoBuffer.size() > 0 && modes.CRC() == m_undoBuffer.back().CRC()) {
    //printf("Not saving duplicate undo buffer\n");
    return false;
  }
  // must rewind to that step before pushing next step
  while (m_undoIndex > 0) {
    m_undoBuffer.pop_back();
    m_undoIndex--;
  }
  m_undoBuffer.push_back(modes);
  //printf("Pushing undo buffer (pos: %u)\n", m_undoIndex);
  // make sure list doesn't grow too big
  if (m_undoLimit && m_undoBuffer.size() > m_undoLimit) {
    //printf("Popping front of undo buffer\n");
    m_undoBuffer.pop_front();
  }
#if 0
  printf("Buffer:\n");
  for (uint32_t i = 0; i < m_undoBuffer.size(); ++i) {
    printf("\t%u: %x", i, m_undoBuffer[i].CRC());
    if ((m_undoBuffer.size() - 1) - m_undoIndex == i) {
      printf(" <--\n");
    } else {
      printf("\n");
    }
  }
#endif
  return true;
}

bool Vortex::applyUndo()
{
  if (!m_undoBuffer.size()) {
    return false;
  }
  uint32_t highestIndex = (uint32_t)m_undoBuffer.size() - 1;
  if (m_undoIndex > highestIndex) {
    m_undoIndex = highestIndex;
  }
  //printf("Undo position: %u / %u\n", m_undoIndex, highestIndex);
  //printf("Buffer:\n");
#if 0
  for (uint32_t i = 0; i < m_undoBuffer.size(); ++i) {
    printf("\t%u: %x", i, m_undoBuffer[i].CRC());
    if ((m_undoBuffer.size() - 1) - m_undoIndex == i) {
      printf(" <--\n");
    } else {
      printf("\n");
    }
  }
#endif
  // index from the back instead of the front
  uint32_t backIndex = highestIndex - m_undoIndex;
  m_undoBuffer[backIndex].resetUnserializer();
  Vortex::setModes(m_undoBuffer[backIndex], false);
  return true;
}

bool Vortex::undo()
{
  if (!m_undoBuffer.size()) {
    return false;
  }
  uint32_t highestIndex = (uint32_t)m_undoBuffer.size() - 1;
  // cannot undo further into history
  if (m_undoIndex > highestIndex) {
    m_undoIndex = highestIndex;
  } else {
    m_undoIndex++;
  }
  return applyUndo();
}

bool Vortex::redo()
{
  if (!m_undoBuffer.size()) {
    return false;
  }
  // cannot undo further into history
  if (m_undoIndex > 0) {
    m_undoIndex--;
  }
  return applyUndo();
}

void Vortex::setTickrate(uint32_t tickrate)
{
  Time::setTickrate(tickrate);
}

uint32_t Vortex::getTickrate()
{
  return Time::getTickrate();
}

bool Vortex::doSave()
{
  return Modes::saveStorage() && addUndoBuffer();
}

void Vortex::handleInputQueue(Button *buttons, uint32_t numButtons)
{
  // if there's nothing in the queue just return
  if (!m_buttonEventQueue.size()) {
    return;
  }
  // pop the event from the front of the queue
  VortexButtonEvent buttonEvent = m_buttonEventQueue.front();
  m_buttonEventQueue.pop_front();
  // the target button for this event (no target if event is just 'wait')
  Button *pButton = buttons;
  if (buttonEvent.type != EVENT_WAIT && buttonEvent.type != EVENT_RAPID_CLICK) {
    // make sure the button that is targeted is actually a valid index
    if (buttonEvent.target >= numButtons) {
      return;
    }
    // assigned the button based on the array index target
    pButton = buttons + buttonEvent.target;
  }
  // switch on the type of event and then run the operation
  switch (buttonEvent.type) {
  case EVENT_NONE:
  default:
    break;
  case EVENT_SHORT_CLICK:
    pButton->m_newRelease = true;
    pButton->m_shortClick = true;
    pButton->m_pressTime = Time::getCurtime();
    pButton->m_holdDuration = SHORT_CLICK_THRESHOLD_TICKS - 1;
    DEBUG_LOG("Injecting short click");
    break;
  case EVENT_LONG_CLICK:
    pButton->m_newRelease = true;
    pButton->m_longClick = true;
    pButton->m_pressTime = Time::getCurtime();
    pButton->m_holdDuration = SHORT_CLICK_THRESHOLD_TICKS + 1;
    DEBUG_LOG("Injecting long click");
    break;
  case EVENT_MENU_ENTER_CLICK:
    // to do this we simply press the button and set the press time
    // to something more than the menu trigger threshold that will make
    // us immediately enter the menus. But we need to unset the pressed
    // button right after so we push a reset click event to reset the button
    pButton->m_pressTime = Time::getCurtime();
#if defined(SLEEP_ENTER_THRESHOLD_TICKS) && defined(SLEEP_ENTER_THRESHOLD_TICKS) && defined(SLEEP_WINDOW_THRESHOLD_TICKS)
    // microlight must hold longer (past sleep time)
    pButton->m_holdDuration = MENU_TRIGGER_THRESHOLD_TICKS + SLEEP_ENTER_THRESHOLD_TICKS + SLEEP_WINDOW_THRESHOLD_TICKS + 1;
#else
    pButton->m_holdDuration = MENU_TRIGGER_THRESHOLD_TICKS + 1;
#endif
    pButton->m_isPressed = true;
    m_buttonEventQueue.push_front(VortexButtonEvent(0, EVENT_RESET_CLICK));
    DEBUG_LOG("Injecting menu enter click");
    break;
  case EVENT_ADV_MENU_ENTER_CLICK:
    // to do this we simply press the button and set the press time
    // to something more than the menu trigger threshold that will make
    // us immediately enter the menus. But we need to unset the pressed
    // button right after so we push a reset click event to reset the button
    pButton->m_pressTime = Time::getCurtime();
    pButton->m_holdDuration = ADV_MENU_DURATION_TICKS + 1;
    pButton->m_longClick = true;
    pButton->m_newRelease = true;
    DEBUG_LOG("Injecting adv menu enter click");
    break;
  case EVENT_DELETE_COL:
    // to do this we simply press the button and set the press time
    // to something more than the menu trigger threshold that will make
    // us immediately enter the menus. But we need to unset the pressed
    // button right after so we push a reset click event to reset the button
    pButton->m_pressTime = Time::getCurtime();
    pButton->m_holdDuration = DELETE_THRESHOLD_TICKS + DELETE_CYCLE_TICKS + 1;
    pButton->m_longClick = true;
    pButton->m_newRelease = true;
    DEBUG_LOG("Injecting delete color click");
    break;
  case EVENT_SLEEP_CLICK:
    // to do this we simply press the button and set the press time
    // to something more than the menu trigger threshold that will make
    // us immediately enter the menus. But we need to unset the pressed
    // button right after so we push a reset click event to reset the button
#ifdef SLEEP_ENTER_THRESHOLD_TICKS
    pButton->m_pressTime = Time::getCurtime();
    pButton->m_holdDuration = SLEEP_ENTER_THRESHOLD_TICKS + 1;
    pButton->m_longClick = true;
    pButton->m_newRelease = true;
    DEBUG_LOG("Injecting sleep click");
#endif
    break;
  case EVENT_FORCE_SLEEP_CLICK:
    // to do this we simply press the button and set the press time
    // to something more than the menu trigger threshold that will make
    // us immediately enter the menus. But we need to unset the pressed
    // button right after so we push a reset click event to reset the button
#ifdef FORCE_SLEEP_THRESHOLD_TICKS
    pButton->m_pressTime = Time::getCurtime();
    pButton->m_holdDuration = FORCE_SLEEP_THRESHOLD_TICKS + 1;
    pButton->m_longClick = true;
    pButton->m_newRelease = true;
    DEBUG_LOG("Injecting force sleep click");
#endif
    break;
  case EVENT_WAIT:
    if (buttonEvent.target) {
      // backup the event queue and clear it
      deque<Vortex::VortexButtonEvent> backup;
      swap(backup, m_buttonEventQueue);
      // ticks the engine forward some number of ticks, the event queue is empty
      // so the engine won't process any input events while doing this
      for (uint32_t i = 0; i < buttonEvent.target; ++i) {
        VortexEngine::tick();
      }
      // then restore the event queue so that events are processed like normal
      swap(backup, m_buttonEventQueue);
    }
    break;
  case EVENT_TOGGLE_CLICK:
    if (pButton->isPressed()) {
      // re-calc all this stuff because there's no api in Button class to do it
      // I don't want to add the api there because it's useless besides for this
      pButton->m_buttonState = false;
      pButton->m_isPressed = false;
      pButton->m_holdDuration = (uint32_t)(Time::getCurtime() - pButton->m_pressTime);
      pButton->m_releaseTime = Time::getCurtime();
      pButton->m_newRelease = true;
      pButton->m_shortClick = (pButton->m_holdDuration <= SHORT_CLICK_THRESHOLD_TICKS);
      pButton->m_longClick = !pButton->m_shortClick;
      DEBUG_LOG("Injecting release");
    } else {
      pButton->m_buttonState = true;
      pButton->m_isPressed = true;
      pButton->m_releaseDuration = (uint32_t)(Time::getCurtime() - pButton->m_releaseTime);
      pButton->m_pressTime = Time::getCurtime();
      pButton->m_newPress = true;
      DEBUG_LOG("Injecting press");
    }
    break;
  case EVENT_RAPID_CLICK:
    pButton->m_consecutivePresses = buttonEvent.target;
    pButton->m_newRelease = true;
    pButton->m_shortClick = true;
    pButton->m_pressTime = Time::getCurtime();
    pButton->m_holdDuration = 1;
    DEBUG_LOGF("Injecting %u x rapid click", buttonEvent.target);
    break;
  case EVENT_QUIT_CLICK:
    // just uninitialize so tick returns false
    m_initialized = false;
    DEBUG_LOG("Injecting quit");
    break;
  case EVENT_RESET_CLICK:
    pButton->m_isPressed = false;
    break;
  }
}

void Vortex::printlog(const char *file, const char *func, int line, const char *msg, va_list list)
{
  if (!Vortex::m_consoleHandle) {
    return;
  }
  string strMsg;
  if (file) {
    strMsg = file;
    if (strMsg.find_last_of('\\') != string::npos) {
      strMsg = strMsg.substr(strMsg.find_last_of('\\') + 1);
    }
    strMsg += ":";
    strMsg += to_string(line);
  }
  if (func) {
    strMsg += " ";
    strMsg += func;
    strMsg += "(): ";
  }
  strMsg += msg;
  strMsg += "\n";
  vfprintf(Vortex::m_consoleHandle, strMsg.c_str(), list);
#if LOG_TO_FILE == 1
  vfprintf(Vortex::m_logHandle, strMsg.c_str(), list);
#endif
}

void Vortex::setStorageFilename(const string &name)
{
  Storage::setStorageFilename(name);
}

string Vortex::getStorageFilename()
{
  return Storage::getStorageFilename();
}
