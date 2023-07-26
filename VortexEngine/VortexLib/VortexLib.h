#pragma once

#if defined(_MSC_VER)
#include <Windows.h>
#endif

#include "Patterns/Patterns.h"
#include "Leds/LedTypes.h"

#include <inttypes.h>

#include <vector>
#include <string>
#include <deque>
#include <deque>

// ============================================================================
//  Vortex Engine Wrapper
//
// This file is the interface for the Vortex Engine.
//
// In cases where the engine is not running with the true arduino framework it
// will instead use the drop-in files in 'EngineDependencies' folder. These
// files provide the main arduino and library APIs that are used by the engine
// and instead provide hooks for a user to connect to them.
//
// The things you can hook into include:
//
//  - The serial input/output callbacks (editor connection)
//  - The Infrared input/output callbacks (mode sharing)
//  - The digital pin read callbacks (buttons press)
//  - The leds init, brightness, and display (render the lights)
//
// Then on top of being able to hook into the various actions of the engine
// the Vortex class provides all kinds of APIs for controlling and manipulating
// the engine while it is running.
//

class VortexCallbacks
{
public:
  VortexCallbacks() {}
  virtual ~VortexCallbacks() {}
  // called when engine reads digital pins, use this to feed button presses to the engine
  virtual long checkPinHook(uint32_t pin);
  // called when engine writes to ir, use this to read data from the vortex engine
  // the data received will be in timings of milliseconds
  // NOTE: to send data to IR use Vortex::IRDeliver at any time
  virtual void infraredWrite(bool mark, uint32_t amount) { }
  // called when engine checks for Serial, use this to indicate serial is connected
  virtual bool serialCheck() { return false; }
  // called when engine begins serial, use this to do any initialization of the connection
  virtual void serialBegin(uint32_t baud) { }
  // called when engine checks for data on serial, use this to tell the engine data is ready
  virtual int32_t serialAvail() { return 0; }
  // called when engine reads from serial, use this to deliver data to the vortex engine
  // TODO: buffer this in libengine and add a deliver api like IR
  virtual size_t serialRead(char *buf, size_t amt) { return 0; }
  // called when engine writes to serial, use this to read data from the vortex engine
  virtual uint32_t serialWrite(const uint8_t *buf, size_t amt) { return 0; }
  // called when the LED strip is initialized
  virtual void ledsInit(void *cl, int count) { }
  // called when the brightness is changed
  virtual void ledsBrightness(int brightness) { }
  // called when the leds are shown
  virtual void ledsShow() { }
};

// forward decls for various classes
class PatternArgs;
class ByteStream;
class Colorset;
class Random;
class Button;
class Mode;

// Vortex Engine wrapper class, use this to interface with
// the vortex engine as much as possible
class Vortex
{
  Vortex();
  ~Vortex();
  // internal initializer
  static bool init(VortexCallbacks *callbacks);
public:

  // public initializer, you must provide a derivation of the class VortexCallbacks
  template <typename T>
  static T *init()
  {
    if (!std::is_base_of<VortexCallbacks, T>()) {
      return nullptr;
    }
    T *callbacks = new T();
    if (!callbacks) {
      return nullptr;
    }
    if (!init(callbacks)) {
      return nullptr;
    }
    return callbacks;
  }
  static void cleanup();

  // tick the engine forward, return false if engine exits
  static bool tick();

  // install a callback for digital reads (button press)
  static void installCallbacks(VortexCallbacks *callbacks);

  // control whether the engine will tick instantly or not
  static void setInstantTimestep(bool timestep);

  // send various clicks
  static void shortClick(uint32_t buttonIndex = 0);
  static void longClick(uint32_t buttonIndex = 0);
  static void menuEnterClick(uint32_t buttonIndex = 0);
  static void advMenuEnterClick(uint32_t buttonIndex = 0);
  static void deleteColClick(uint32_t buttonIndex = 0);
  static void sleepClick(uint32_t buttonIndex = 0);
  static void forceSleepClick(uint32_t buttonIndex = 0);
  static void pressButton(uint32_t buttonIndex = 0);
  static void releaseButton(uint32_t buttonIndex = 0);
  static bool isButtonPressed(uint32_t buttonIndex = 0);

  // send a wait event, will let the engine run a tick if running in lockstep
  // for example when running the testing system
  static void sendWait(uint32_t amount = 0);

  // deliver some number of rapid clicks (only availbe for button0... sorry)
  static void rapidClick(uint32_t amount = 3);

  // get the current menu demo mode
  static Mode *getMenuDemoMode();

  // special 'click' that quits the engine
  static void quitClick();

  // deliver IR timing, the system expects mark first, but it doesn't matter
  // because the system will reset on bad data and then it can interpret any
  // timing either way
  static void IRDeliver(uint32_t timing);
  static void VLDeliver(uint32_t timing);

  // get total/used storage space
  static void getStorageStats(uint32_t *outTotal, uint32_t *outUsed);
  static void loadStorage();

  // open various menus on the core (if they exist!)
  static void openRandomizer();
  static void openColorSelect();
  static void openPatternSelect();
  static void openGlobalBrightness();
  static void openFactoryReset();
  static void openModeSharing();
  static void openEditorConnection();

  // convert modes to/from a bytestream
  static bool getModes(ByteStream &outStream);
  static bool setModes(ByteStream &stream, bool save = true);
  static bool getCurMode(ByteStream &stream);

  // functions to operate on the current mode selection
  static uint32_t curModeIndex();
  static uint32_t numModes();
  static uint32_t numLedsInMode();
  static bool addNewMode(Random *pRandCtx = nullptr, bool save = true);
  static bool addNewMode(ByteStream &stream, bool save = true);
  static bool setCurMode(uint32_t index, bool save = true);
  static bool nextMode(bool save = true);
  static bool delCurMode(bool save = true);
  static bool shiftCurMode(int8_t offset, bool save = true);

  // functions to operate on the current Mode
  static bool setPattern(PatternID id, const PatternArgs *args = nullptr,
    const Colorset *set = nullptr, bool save = true);
  static PatternID getPatternID(LedPos pos = LED_ANY);
  static std::string getPatternName(LedPos pos = LED_ANY);
  static std::string getModeName();
  static bool setPatternAt(LedPos pos, PatternID id,
    const PatternArgs *args = nullptr, const Colorset *set = nullptr,
    bool save = true);
  static bool getColorset(LedPos pos, Colorset &set);
  static bool setColorset(LedPos pos, const Colorset &set, bool save = true);
  static bool getPatternArgs(LedPos pos, PatternArgs &args);
  static bool setPatternArgs(LedPos pos, PatternArgs &args, bool save = true);

  // whether the current mode is a multi-led pattern
  static bool isCurModeMulti();

  // Helpers for converting pattern id and led id to string
  static std::string patternToString(PatternID id = PATTERN_NONE);
  static std::string ledToString(LedPos pos);
  static uint32_t numCustomParams(PatternID id);
  static std::vector<std::string> getCustomParams(PatternID id);

  // undo redo
  static void setUndoBufferLimit(uint32_t limit);
  static bool addUndoBuffer();
  static bool undo();
  static bool redo();

  // change tickrate of the engine if the engine was configured to support it
  static void setTickrate(uint32_t tickrate);
  static uint32_t getTickrate();

  // enable/disable undo
  static void enableUndo(bool enabled) { m_undoEnabled = enabled; }

  // access stored callbacks
  static VortexCallbacks *vcallbacks() { return m_storedCallbacks; }

  // printing to log system
  static void printlog(const char *file, const char *func, int line, const char *msg, va_list list);

  // injects a command into the engine, the engine will parse one command
  // per tick so multiple commands will be queued up
  static void doCommand(char c);

  // whether the engine has sleep enabled, if disabled it will always be awake
  static void setSleepEnabled(bool enable);
  static bool sleepEnabled();

  // whether the engine is sleeping, and/or to enter sleep
  static void enterSleep(bool save);
  static bool isSleeping();

  // enable, fetch and clear the internal command log
  static void enableCommandLog(bool enable) { m_commandLogEnabled = enable; }
  static const std::string &getCommandLog() { return m_commandLog; }
  static void clearCommandLog() { m_commandLog.clear(); }

  // enable/disable lockstep mode (one tick per input)
  static bool enableLockstep(bool enable) { return m_lockstepEnabled = enable; }
  static bool isLockstep() { return m_lockstepEnabled; }

  // enable disable storage
  static bool enableStorage(bool enable) { return m_storageEnabled = enable; }
  static bool storageEnabled() { return m_storageEnabled; }

  // control the storage
  static void setStorageFilename(const std::string &name);
  static std::string getStorageFilename();

  // enable or disable the 'lock'
  static void setLockEnabled(bool enable) { m_lockEnabled = enable; }
  static bool lockEnabled() { return m_lockEnabled; }

private:
  // the last command to have been executed
  static char m_lastCommand;

  // internal function to handle repeating commands
  static void handleRepeat(char c);

  // so that the buttons class can call handleInputQueue
  friend class Buttons;

  // called by the engine right after all buttons are checked, this will process
  // the input deque that is fed by the apis like shortClick() above and translate
  // those messages into actual button events by overwriting button data that tick
  static void handleInputQueue(Button *buttons, uint32_t numButtons);

  // The various different button events that can be injected into vortex
  enum VortexButtonEventType
  {
    // no event
    EVENT_NONE,

    // a short click
    EVENT_SHORT_CLICK,
    // a long click
    EVENT_LONG_CLICK,
    // a press that is just long enough to open the ring menu
    EVENT_MENU_ENTER_CLICK,
    // a press that is just long enough to enter adv menus
    EVENT_ADV_MENU_ENTER_CLICK,
    // a press that is long enough to delete a color from col select
    EVENT_DELETE_COL,
    // a press just long enough to put the device to sleep from main modes
    EVENT_SLEEP_CLICK,
    // a press very long so that the chip triggers it's force sleep
    EVENT_FORCE_SLEEP_CLICK,
    // just wait around a tick (mainly used for testing)
    EVENT_WAIT,
    // toggle the button (press or unpress it)
    EVENT_TOGGLE_CLICK,
    // rapid click the button x times
    EVENT_RAPID_CLICK,
    // quit the engine (not really a 'click')
    EVENT_QUIT_CLICK,

    // this is an automated event that will reset the button click
    // after the ring menu hold-to-enter sequence
    EVENT_RESET_CLICK,
  };

  class VortexButtonEvent
  {
  public:
    VortexButtonEvent(uint32_t target, VortexButtonEventType type) :
      target(target), type(type)
    {
    }
    // target button index
    uint32_t target;
    // the event to trigger
    VortexButtonEventType type;
  };

  // save and add undo buffer
  static bool doSave();
  static bool applyUndo();

  // undo buffer
  static std::deque<ByteStream> m_undoBuffer;
  // the undo limit
  static uint32_t m_undoLimit;
  // undo position in buffer
  static uint32_t m_undoIndex;
  // whether undo buffer is disabled recording
  static bool m_undoEnabled;
  // stored callbacks
  static VortexCallbacks *m_storedCallbacks;
  // handle to the console and logfile
  static FILE *m_consoleHandle;
#if LOG_TO_FILE == 1
  static FILE *m_logHandle;
#endif
  // queue of button events, deque so can push to front and back
  static std::deque<VortexButtonEvent> m_buttonEventQueue;
  // whether initialized
  static bool m_initialized;
  // whether each button is pressed (bitflags) so technically this only
  // supports 32 buttons but idc whoever adds 33 buttons can fix this
  static uint32_t m_buttonsPressed;
  // keeps a log of all the commands issued
  static std::string m_commandLog;
  // whether to record commands
  static bool m_commandLogEnabled;
  // whether to run in lockstep mode (one input per step)
  static bool m_lockstepEnabled;
  // whether storage is enabled
  static bool m_storageEnabled;
  // whether sleep is enabled
  static bool m_sleepEnabled;
  // whether lock is enabled
  static bool m_lockEnabled;
};
