#include "VortexLib.h"

// VortexEngine includes
#include "VortexEngine.h"
#include "Buttons/Button.h"
#include "Serial/ByteStream.h"
#include "Infrared/IRReceiver.h"
#include "Patterns/PatternBuilder.h"
#include "Patterns/Pattern.h"
#include "Colors/Colorset.h"
#include "Time/TimeControl.h"
#include "Time/Timings.h"
#include "Menus/Menus.h"
#include "Menus/MenuList/EditorConnection.h"
#include "Menus/MenuList/Randomizer.h"
#include "Modes/Modes.h"
#include "Modes/Mode.h"

// for random()
#include "Arduino.h"

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
deque<ByteStream> Vortex::m_undoBuffer;
uint32_t Vortex::m_undoLimit = 0;
uint32_t Vortex::m_undoIndex = 0;
bool Vortex::m_undoEnabled = true;
VortexCallbacks *Vortex::m_storedCallbacks;
FILE *Vortex::m_consoleHandle = nullptr;
#if LOG_TO_FILE == 1
FILE *Vortex::m_logHandle = nullptr;
#endif
queue<VortexButtonEvent> Vortex::m_buttonEventQueue;
bool Vortex::m_initialized = false;
uint32_t Vortex::m_buttonsPressed = 0;

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
    oss << std::put_time(&tm, "%d-%m-%Y-%H-%M-%S");
    oss << "." << GetCurrentProcessId();
    string filename = VORTEX_LOG_NAME "." + oss.str() + ".txt";
    int err = fopen_s(&m_logHandle, filename.c_str(), "w");
    if (err != 0 || !m_logHandle) {
      MessageBox(NULL, "Failed to open logfile", to_string(err).c_str(), 0);
      return false;
    }
  }
#endif

  // init the arduino drop-in replacement
  init_arduino();
  // init the engine
  VortexEngine::init();
  // clear the modes
  //Modes::clearModes();
  // save and set undo buffer
  doSave();

  // check all custom params match the mapped list of words
  for (PatternID id = PATTERN_FIRST; id < PATTERN_COUNT; ++id) {
    vector<string> params = Vortex::getCustomParams(id);
    PatternArgs args = PatternBuilder::getDefaultArgs(id);
    if (params.size() != args.numArgs) {
      // Params not even!
      return false;
    }
  }

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
  // iterate the number of inputs on stdin and parse each letter
  // into a command for the engine
  for (uint32_t i = 0; i < numInputs; ++i) {
    switch (getchar()) {
    case 'a':
      Vortex::shortClick();
      break;
    case 's':
      Vortex::longClick();
      break;
    case 'd':
      Vortex::menuEnterClick();
      break;
    case 'q':
      Vortex::quitClick();
      break;
    case 'f':
      if (Vortex::isButtonPressed()) {
        Vortex::releaseButton();
      } else {
        Vortex::pressButton();
      }
      break;
    default:
      break;
    }
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

// send various clicks
void Vortex::shortClick(uint32_t buttonIndex)
{
  m_buttonEventQueue.push(VortexButtonEvent(buttonIndex, EVENT_SHORT_CLICK));
}

void Vortex::longClick(uint32_t buttonIndex)
{
  m_buttonEventQueue.push(VortexButtonEvent(buttonIndex, EVENT_LONG_CLICK));
}

void Vortex::menuEnterClick(uint32_t buttonIndex)
{
  m_buttonEventQueue.push(VortexButtonEvent(buttonIndex, EVENT_MENU_ENTER_CLICK));
}

void Vortex::pressButton(uint32_t buttonIndex)
{
  m_buttonsPressed |= (1 << buttonIndex);
}

void Vortex::releaseButton(uint32_t buttonIndex)
{
  m_buttonsPressed &= ~(1 << buttonIndex);
}

Mode *Vortex::getMenuDemoMode()
{
  void *pMenu = Menus::curMenu();
  if (!pMenu) {
    return nullptr;
  }
  MenuEntryID id = Menus::curMenuID();
  if (id == MENU_RANDOMIZER) {
    return &((Randomizer *)pMenu)->m_demoMode;
  } else if (id == MENU_EDITOR_CONNECTION) {
    return &((EditorConnection *)pMenu)->m_demoMode;
  }
  return nullptr;
}

bool Vortex::isButtonPressed(uint32_t buttonIndex)
{
  return (m_buttonsPressed & (1 << buttonIndex)) != 0;
}

void Vortex::quitClick()
{
  m_buttonEventQueue.push(VortexButtonEvent(0, EVENT_QUIT_CLICK));
}

void Vortex::IRDeliver(uint32_t timing)
{
  IRReceiver::handleIRTiming(timing);
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

uint32_t Vortex::curMode()
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

bool Vortex::addNewMode(bool save)
{
  Colorset set;
  set.randomize();
  // create a random pattern ID from all patterns
  PatternID randomPattern;
  do {
    // continuously re-randomize the pattern so we don't get solids
    randomPattern = (PatternID)random(PATTERN_FIRST, PATTERN_COUNT);
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
  if (!pMode->setPattern(id, args, set)) {
    return false;
  }
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
    return patternToString(getPatternID(LED_FIRST));
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

bool Vortex::setSinglePat(LedPos pos, PatternID id,
  const PatternArgs *args, const Colorset *set, bool save)
{
  Mode *pMode = Modes::curMode();
  if (!pMode) {
    return false;
  }
  if (!pMode->setSinglePat(pos, id, args, set)) {
    return false;
  }
  return !save || doSave();
}

bool Vortex::getColorset(LedPos pos, Colorset &set)
{
  Mode *pMode = Modes::curMode();
  if (!pMode) {
    return false;
  }
  const Colorset *pSet = pMode->getColorset(pos);
  if (!pSet) {
    return false;
  }
  set = *pSet;
  return true;
}

bool Vortex::setColorset(LedPos pos, const Colorset &set, bool save)
{
  Mode *pMode = Modes::curMode();
  if (!pMode) {
    return false;
  }
  Pattern *pat = pMode->getPattern(pos);
  if (!pat) {
    return false;
  }
  pat->setColorset(&set);
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
  Pattern *pat = pMode->getPattern(pos);
  if (!pat) {
    return false;
  }
  pat->setArgs(args);
  // re-initialize the mode after changing pattern args
  pMode->init();
  // save the new params
  return !save || doSave();
}

string Vortex::patternToString(PatternID id)
{
  if (id == PATTERN_NONE || id >= PATTERN_COUNT) {
    return "pattern_none";
  }
  // This is awful but idk how else to do it for now
  static const char *patternNames[PATTERN_COUNT] = {
    "basic", "strobe", "hyperstrobe", "dops", "dopish", "ultradops", "strobie",
    "ribbon", "miniribbon", "blinkie", "ghostcrush", "solid", "tracer",
    "dashdops", "advanced", "blend", "complementary blend", "brackets",
    "rabbit", "hueshift", "theater chase", /*"chaser",*/ "zigzag", "zipfade",
    "tiptop", "drip", "dripmorph", "crossdops", "doublestrobe", "meteor",
    "sparkletrace", "vortexwipe", "warp", "warpworm", "snowball", "lighthouse",
    "pulsish", "fill", "bounce", "impact", "splitstrobie", "backstrobe",
    "flowers", "jest", "materia",
  };
  return patternNames[id];
}

// this shouldn't change much so this is fine
string Vortex::ledToString(LedPos pos)
{
  if (numLedsInMode() != 10 || pos >= 10) {
    return "led " + to_string(pos);
  }
  static const char *ledNames[10] = {
    // tips       tops
    "pinkie tip", "pinkie top",
    "ring tip",   "ring top",
    "middle tip", "middle top",
    "index tip",  "index top",
    "thumb tip",  "thumb top",
  };
  return ledNames[pos];
}

// the number of custom parameters for any given pattern id
uint32_t Vortex::numCustomParams(PatternID id)
{
  Pattern *pat = PatternBuilder::make(id);
  if (!pat) {
    return 0;
  }
  PatternArgs args;
  pat->getArgs(args);
  uint32_t numArgs = args.numArgs;
  delete pat;
  return numArgs;
}

vector<string> Vortex::getCustomParams(PatternID id)
{
  switch (id) {
  case PATTERN_BASIC:
  case PATTERN_STROBE:
  case PATTERN_HYPERSTROBE:
  case PATTERN_DOPS:
  case PATTERN_DOPISH:
  case PATTERN_ULTRADOPS:
  case PATTERN_STROBIE:
  case PATTERN_RIBBON:
  case PATTERN_MINIRIBBON:
  case PATTERN_BLINKIE:
  case PATTERN_GHOSTCRUSH:
    return { "On Duration", "Off Duration", "Gap Duration" };
  case PATTERN_SOLID:
    return { "On Duration", "Off Duration", "Gap Duration", "Color Index" };
  case PATTERN_TRACER:
    return { "Tracer Duration", "Dot Duration" };
  case PATTERN_DASHDOPS:
    return { "Dash Duration", "Dot Duration", "Off Duration" };
  case PATTERN_ADVANCED:
    return { "On Duration", "Off Duration", "Gap Duration", "Group Size", "Skip Colors", "Repeat Group" };
  case PATTERN_BLEND:
  case PATTERN_COMPLEMENTARY_BLEND:
    return { "On Duration", "Off Duration", "Gap Duration", "Start Offset" };
  case PATTERN_BRACKETS:
    return { "Bracket Duration", "Mid Duration", "Off Duration" };
  case PATTERN_RABBIT:
  case PATTERN_FLOWERS:
  case PATTERN_TIPTOP:
    return { "On Duration 1", "Off Duration 1", "Gap Duration 1",
      "On Duration 2", "Off Duration 2", "Gap Duration 2" };
  case PATTERN_IMPACT:
    return { "On Duration 1", "Off Duration 1", "On Duration 2",
      "Off Duration 2", "On Duration 3", "Off Duration 3" };
  case PATTERN_JEST:
    return { "On Duration" , "Off Duration", "Gap 1 Duration", "Gap 2 Duration", "Group Size" };
  case PATTERN_HUESHIFT:
    return { "Speed", "Scale" };
  case PATTERN_THEATER_CHASE:
    return { "On Duration", "Off Duration", "Step Duration" };
  case PATTERN_ZIGZAG:
  case PATTERN_ZIPFADE:
    return { "On Duration", "Off Duration", "Step Duration", "Snake Size", "Fade Amount" };
  case PATTERN_DRIP:
  case PATTERN_CROSSDOPS:
  case PATTERN_DOUBLESTROBE:
  case PATTERN_SPARKLETRACE:
  case PATTERN_VORTEXWIPE:
  case PATTERN_WARP:
  case PATTERN_WARPWORM:
  case PATTERN_SNOWBALL:
  case PATTERN_FILL:
    return { "On Duration", "Off Duration", "Step Duration" };
  case PATTERN_BOUNCE:
    return { "On Duration", "Off Duration", "Step Duration", "Fade Amount" };
    //case PATTERN_CHASER:
  case PATTERN_DRIPMORPH:
    return { "On Duration", "Off Duration", "Speed" };
  case PATTERN_METEOR:
    return { "On Duration", "Off Duration", "Step Duration", "Fade Amount" };
  case PATTERN_LIGHTHOUSE:
    return { "On Duration", "Off Duration", "Step Duration", "Fade Amount", "Fade Rate" };
  case PATTERN_PULSISH:
  case PATTERN_MATERIA:
    return { "On Duration 1", "Off Duration 1", "On Duration 2", "Off Duration 2", "Step Duration" };
  case PATTERN_SPLITSTROBIE:
    return { "On Duration", "Off Duration", "Gap Duration", "Dash Duration",
      "Dot Duration", "Step Duration x 100ms" };
  case PATTERN_BACKSTROBE:
    return { "On Duration 1", "Off Duration 1", "Gap Duration 1",  "On Duration 2", "Off Duration 2",
      "Gap Duration 2", "Step Duration x 100ms" };
  case PATTERN_NONE:
  default:
    break;
  }
  return vector<string>();
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
  m_buttonEventQueue.pop();
  // make sure the button that is targeted is actually a valid index
  if (buttonEvent.target >= numButtons) {
    return;
  }
  // grab the target button for this event
  Button *pButton = buttons + buttonEvent.target;
  // switch on the type of event and then run the operation
  switch (buttonEvent.type) {
  case EVENT_NONE:
  default:
    break;
  case EVENT_SHORT_CLICK:
    pButton->m_newRelease = true;
    pButton->m_shortClick = true;
    pButton->m_pressTime = Time::getCurtime();
    pButton->m_holdDuration = 200;
    break;
  case EVENT_LONG_CLICK:
    pButton->m_newRelease = true;
    pButton->m_longClick = true;
    pButton->m_pressTime = Time::getCurtime();
    pButton->m_holdDuration = SHORT_CLICK_THRESHOLD_TICKS + 1;
    break;
  case EVENT_MENU_ENTER_CLICK:
    // to do this we simply press the button and set the press time
    // to something more than the menu trigger threshold that will make
    // us immediately enter the menus. But we need to unset the pressed
    // button right after so we push a reset click event to reset the button
    pButton->m_pressTime = Time::getCurtime();
    pButton->m_holdDuration = MENU_TRIGGER_THRESHOLD_TICKS + 1;
    pButton->m_isPressed = true;
    m_buttonEventQueue.push(VortexButtonEvent(0, EVENT_RESET_CLICK));
    break;
  case EVENT_TOGGLE_CLICK:
    if (pButton->isPressed()) {
      // re-calc all this stuff because there's no api in Button class to do it
      // I don't want to add the api there because it's useless besides for this
      pButton->m_buttonState = HIGH;
      pButton->m_isPressed = false;
      pButton->m_holdDuration = (uint32_t)(Time::getCurtime() - pButton->m_pressTime);
      pButton->m_releaseTime = Time::getCurtime();
      pButton->m_newRelease = true;
      pButton->m_shortClick = (pButton->m_holdDuration <= SHORT_CLICK_THRESHOLD_TICKS);
      pButton->m_longClick = !pButton->m_shortClick;
    } else {
      pButton->m_buttonState = LOW;
      pButton->m_isPressed = true;
      pButton->m_releaseDuration = (uint32_t)(Time::getCurtime() - pButton->m_releaseTime);
      pButton->m_pressTime = Time::getCurtime();
      pButton->m_newPress = true;
    }
    break;
  case EVENT_QUIT_CLICK:
    // just uninitialize so tick returns false
    m_initialized = false;
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
