#pragma once

#include "VortexEngine.h"
#include "Patterns/Patterns.h"
#include "Leds/LedTypes.h"

#include <inttypes.h>

#include <vector>
#include <string>
#include <deque>

class PatternArgs;
class ByteStream;
class Colorset;

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


// Vortex Engine wrapper class, use this to interface with
// the vortex engine as much as possible
class VEngine
{
  VEngine();
public:
  // a read hook callback
  typedef long (*readHookFn)(uint32_t pin);

  static bool init();
  static void cleanup();

  // install a callback for digital reads (button press)
  static void installDigitalReadCallback(readHookFn readHook);

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

  // call the digital read callback with a value
  static long digitalReadCallback(uint32_t pin);

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

  // callback for digital reads
  static readHookFn m_digitalReadCallback;
};
