#pragma once

#include "VortexEngine.h"
#include "Patterns/Patterns.h"
#include "Leds/LedTypes.h"

#include <inttypes.h>

#include <vector>
#include <string>
#include <deque>

// ============================================================================
//  Vortex Engine Wrapper
//
// This file is the interface for the Vortex Engine.
//
// In cases where the engine is not running with the true arduino framework it
// will instead use the drop-in files in 'EngineDependencies' folder. These
// files provide the main arduino and library apis that are used by the engine
// and instead provide hooks for a user to connect to them.
//
// The things you can hook into include:
//
//  - The serial input/output callbacks (editor connection)
//  - The Infrared input/output callbacks (mode sharing)
//  - The digital pin input/output callbacks (buttons)
//  - The led strip and all the colors
//
//

// todo: maybe? led changed callback??

class VortexCallbacks
{
public:
  VortexCallbacks() {}
  virtual ~VortexCallbacks() {}

  // called when engine reads digital pins, use this to feed button presses to the engine
  virtual long readHook(uint32_t pin) { return 0; }
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
  virtual size_t serialRead(char *buf, size_t amt) { return 0; }
  // called when engine writes to serial, use this to read data from the vortex engine
  virtual uint32_t serialWrite(const uint8_t *buf, size_t amt) { return 0; }
};

class PatternArgs;
class ByteStream;
class Colorset;

// Vortex Engine wrapper class, use this to interface with
// the vortex engine as much as possible
class Vortex
{
  Vortex();
public:
  static bool init();
  static void cleanup();

  // install a callback for digital reads (button press)
  static void installCallbacks(VortexCallbacks *callbacks);

  // deliver IR timing, the system expects mark first, but it doesn't matter
  // because the system will reset on bad data and then it can interpret any 
  // timing either way
  static void IRDeliver(uint32_t timing);

  // get total/used storage space
  static void getStorageStats(uint32_t *outTotal, uint32_t *outUsed);

  static bool getModes(ByteStream &outStream);
  static bool setModes(ByteStream &stream, bool save = true);
  static bool getCurMode(ByteStream &stream);

  // functions to operate on the current mode selection
  static uint32_t curMode();
  static uint32_t numModes();
  static uint32_t numLedsInMode();
  static bool addNewMode(bool save = true);
  static bool addNewMode(ByteStream &stream, bool save = true);
  static bool setCurMode(uint32_t index, bool save = true);
  static bool nextMode(bool save = true);
  static bool delCurMode(bool save = true);
  static bool shiftCurMode(int8_t offset, bool save = true);

  // functions to operate on the current Mode
  static bool setPattern(PatternID id, const PatternArgs *args = nullptr,
    const Colorset *set = nullptr, bool save = true);
  static PatternID getPatternID(LedPos pos = LED_FIRST);
  static std::string getPatternName(LedPos pos = LED_FIRST);
  static std::string getModeName();
  static bool setSinglePat(LedPos pos, PatternID id,
    const PatternArgs *args = nullptr, const Colorset *set = nullptr,
    bool save = true);
  static bool getColorset(LedPos pos, Colorset &set);
  static bool setColorset(LedPos pos, const Colorset &set, bool save = true);
  static bool getPatternArgs(LedPos pos, PatternArgs &args);
  static bool setPatternArgs(LedPos pos, PatternArgs &args, bool save = true);

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

  // enable/disable undo
  static void enableUndo(bool enabled) { m_undoEnabled = enabled; }

  // access stored callbacks
  static VortexCallbacks *vcallbacks() { return m_storedCallbacks; }

private:
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
};
