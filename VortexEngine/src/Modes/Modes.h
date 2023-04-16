#ifndef MODES_H
#define MODES_H

#include "../Serial/ByteStream.h"
#include "../Colors/ColorTypes.h"
#include "../Patterns/Patterns.h"
#include "../Leds/Leds.h"

#include "../VortexConfig.h"

#include <inttypes.h>

class PatternArgs;
class Colorset;
class Mode;

class Modes
{
  // private unimplemented constructor
  Modes();

public:
  // opting for static class here because there should only ever be one
  // Modes control object and I don't like singletons
  static bool init();
  static void cleanup();

  // play the current mode
  static void play();

  // full save/load to/from buffer
  static bool saveToBuffer(ByteStream &saveBuffer);
  static bool loadFromBuffer(ByteStream &saveBuffer);

  // full save/load to/from storage
  static bool loadStorage();
  static bool saveStorage();

  // saves all modes to a buffer
  static void serialize(ByteStream &buffer);
  // load all modes from a buffer, optionally specify number of leds
  static bool unserialize(ByteStream &buffer);

  // set default settings (must save after)
  static bool setDefaults();

  // shift the current mode to a different position relative to current position
  // negative values for up, positive values for down, 0 for no move
  static bool shiftCurMode(int32_t offset = 1);

  // add a new mode in various different ways
  static bool addMode(PatternID id, RGBColor c1, RGBColor c2 = RGB_OFF,
    RGBColor c3 = RGB_OFF, RGBColor c4 = RGB_OFF, RGBColor c5 = RGB_OFF,
    RGBColor c6 = RGB_OFF, RGBColor c7 = RGB_OFF, RGBColor c8 = RGB_OFF);
  static bool addMode(PatternID id, const PatternArgs *args, const Colorset *set);
  static bool addMode(const Mode *mode);

  // add a new mode by unserializing raw
  static bool addSerializedMode(ByteStream &serializedMode);
  // add a new mode by loading from a save buffer
  static bool addModeFromBuffer(ByteStream &serializedMode);

  // update the current mode to match the given mode
  static bool updateCurMode(PatternID id, const Colorset *set = nullptr);
  static bool updateCurMode(const Mode *mode);

  // set the current active mode by index
  static Mode *setCurMode(uint32_t index);

  // get the current mode
  static Mode *curMode();
  // iterate to next mode and return it
  static Mode *nextMode();
  // iterate to previous mode and return it
  Mode *Modes::previousMode();

  // the number of modes
  static uint8_t numModes() { return m_numModes; }
  static uint8_t curModeIndex() { return m_curMode; }

  // delete the current mode
  static void deleteCurMode();

  // delete all modes in the list
  static void clearModes();

#if MODES_TEST == 1
  static void test();
#endif

private:
  // linked list of internal mode storage
  class ModeLink {
  public:
    // construct a link and optionally instantiate the link
    ModeLink(const Mode *src = nullptr, bool inst = false);
    ModeLink(const ByteStream &src, bool inst = false);
    ~ModeLink();

    // init the link and append another link
    bool init(const Mode *mode = nullptr);
    bool append(const Mode *next);
    bool append(const ByteStream &next);

    // play the instantiated mode inside
    void play();

    // unlink self from the chain, returns link that takes position
    ModeLink *unlinkSelf();

    // link in a node before or after self
    void linkAfter(ModeLink *link);
    void linkBefore(ModeLink *link);

    // instantiate/destroy the mode
    Mode *instantiate();
    void uninstantiate();

    // if the mode is instantiated and the instantiated version
    // has changed at all then save will re-save it to the buffer
    void save();

    // accessors
    ByteStream &buffer() { return m_storedMode; }
    Mode *mode() { return m_pInstantiatedMode; }
    ModeLink *next() { return m_next; }
    ModeLink *prev() { return m_prev; }

    operator ByteStream &() { return m_storedMode; }
    operator ByteStream() { return m_storedMode; }
    operator Mode *() { return m_pInstantiatedMode; }
  private:
    Mode *m_pInstantiatedMode;
    ByteStream m_storedMode;
    ModeLink *m_next;
    ModeLink *m_prev;
  };

  // fetch a link from the chain by index
  static ModeLink *getModeLink(uint32_t index);

  // initialize current mode from ByteStream, optionally force re-init which
  // will destroy the current instantiated mode and re-load it from serial
  static Mode *initCurMode(bool force = false);
  static void saveCurMode();

  // the current mode we're on
  static uint8_t m_curMode;

  // the number of modes loaded
  static uint8_t m_numModes;

  // the current instantiated mode and it's respective link
  static ModeLink *m_pCurModeLink;

  // list of serialized version of bufers
  static ModeLink *m_storedModes;
};

#endif
