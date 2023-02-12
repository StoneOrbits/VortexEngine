#include "VortexLib.h"

// VortexEngine includes
#include "VortexEngine.h"
#include "Buttons/Button.h"
#include "Serial/ByteStream.h"
#include "Patterns/PatternBuilder.h"
#include "Patterns/Pattern.h"
#include "Colors/Colorset.h"
#include "Time/TimeControl.h"
#include "Time/Timings.h"
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
queue<Vortex::VortexButtonEvent> Vortex::m_buttonEventQueue;
bool Vortex::m_initialized = false;
bool Vortex::m_buttonPressed = false;

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
  if (Vortex::isButtonPressed(pin)) {
    return 0; // LOW
  }
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
  m_buttonPressed = true;
}

void Vortex::releaseButton(uint32_t buttonIndex)
{
  m_buttonPressed = false;
}

Mode *Vortex::getMenuDemoMode()
{
  return nullptr;
}

bool Vortex::isButtonPressed(uint32_t buttonIndex)
{
  return m_buttonPressed;
}

void Vortex::quitClick()
{
  m_buttonEventQueue.push(VortexButtonEvent(0, EVENT_QUIT_CLICK));
}

void Vortex::IRDeliver(uint32_t timing)
{
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
  return true;
}

bool Vortex::setModes(ByteStream &stream, bool save)
{
  return !save || doSave();
}

bool Vortex::getCurMode(ByteStream &outStream)
{
  return false;
}

uint32_t Vortex::curMode()
{
  return 0;
}

uint32_t Vortex::numModes()
{
  return 0;
}

uint32_t Vortex::numLedsInMode()
{
  return 0;
}

bool Vortex::addNewMode(bool save)
{
  return false;
}

bool Vortex::addNewMode(ByteStream &stream, bool save)
{
  return false;
}

bool Vortex::setCurMode(uint32_t index, bool save)
{
  return false;
}

bool Vortex::nextMode(bool save)
{
  return false;
}

bool Vortex::delCurMode(bool save)
{
  return false;
}

bool Vortex::shiftCurMode(int8_t offset, bool save)
{
  return false;
}

bool Vortex::setPattern(PatternID id, const PatternArgs *args, const Colorset *set, bool save)
{
  return false;
}

PatternID Vortex::getPatternID(LedPos pos)
{
  return PATTERN_ADVANCED;
}

string Vortex::getPatternName(LedPos pos)
{
  return patternToString(getPatternID(pos));
}

string Vortex::getModeName()
{
  return "";
}

bool Vortex::setSinglePat(LedPos pos, PatternID id,
  const PatternArgs *args, const Colorset *set, bool save)
{
  return false;
}

bool Vortex::getColorset(LedPos pos, Colorset &set)
{
  return false;
}

bool Vortex::setColorset(LedPos pos, const Colorset &set, bool save)
{
  return false;
}

bool Vortex::getPatternArgs(LedPos pos, PatternArgs &args)
{
  return false;
}

bool Vortex::setPatternArgs(LedPos pos, PatternArgs &args, bool save)
{
  return false;
}

string Vortex::patternToString(PatternID id)
{
  return "";
}

// this shouldn't change much so this is fine
string Vortex::ledToString(LedPos pos)
{
  return "";
}

// the number of custom parameters for any given pattern id
uint32_t Vortex::numCustomParams(PatternID id)
{
  return 0;
}

vector<string> Vortex::getCustomParams(PatternID id)
{
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

uint32_t Vortex::getTickrate()
{
  return Time::getTickrate();
}

bool Vortex::doSave()
{
  return true;
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
