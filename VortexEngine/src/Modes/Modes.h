#ifndef MODES_H
#define MODES_H

#include <stdbool.h>
#include "../Serial/ByteStream.h"
#include "../Colors/ColorTypes.h"
#include "../Patterns/Patterns.h"
#include "../Leds/Leds.h"

#include "../VortexConfig.h"

#include <inttypes.h>

typedef struct PatternArgs PatternArgs;
typedef struct Colorset Colorset;
typedef struct Mode Mode;

// the device is locked and needs to be unlocked to turn on
#define MODES_FLAG_LOCKED     (1 << 0)
// the device will turn on and off with a single click
#define MODES_FLAG_ONE_CLICK  (1 << 1)
// toggles the advanced menus being accessible
#define MODES_FLAG_ADV_MENUS  (1 << 2)
// keychain light mode (behave like a keychain)
#define MODES_FLAG_KEYCHAIN   (1 << 3)
// WARNING!! The upper 4 bits of the flags are taken by the startup mode id,
//           you can only use 4 global flags!

// This is a new special feature used by the Chromadeck when it flashes a new
// firmware to the Duo it will set the global flags to 0xF0 like this:
//
//   0 = button lock enabled
//   0 = one click mode enabled
//   0 = advanced menus enabled
//   0 = keychain mode enabled
//   1111 = Startup Mode Index 15 (impossible)
//
// If this flag is present then the Duo needs to turn on and write out it's
// save header because a new firmware has been flashed and the save header
// will still have the old version number saved inside
#define MODES_FLAG_NEW_FIRMWARE 0xF0

typedef struct ModeLink ModeLink;

extern bool Modes_m_loaded;
extern uint8_t Modes_m_curMode;
extern uint8_t Modes_m_numModes;
extern ModeLink * Modes_m_pCurModeLink;
extern ModeLink * Modes_m_storedModes;
extern uint32_t Modes_m_globalFlags;
extern uint32_t Modes_m_lastSwitchTime;

// Everything here is a free function operating on a single set of globals;
// there should only ever be one
  // Modes control object and I don't like singletons
bool Modes_init();
void Modes_cleanup();

  // load modes so they are ready to play
bool Modes_load();

  // play the current mode
void Modes_play();

  // full save/load to/from buffer
bool Modes_saveToBuffer(ByteStream *saveBuffer);
bool Modes_loadFromBuffer(ByteStream *saveBuffer);

  // save/load the global settings to/from storage
bool Modes_saveHeader();
bool Modes_loadHeader();

  // full save/load to/from storage
bool Modes_saveStorage();
bool Modes_loadStorage();

  // save load the savefile header from storage
bool Modes_serializeSaveHeader(ByteStream *saveBuffer);
bool Modes_unserializeSaveHeader(ByteStream *saveBuffer);

  // saves all modes to a buffer
bool Modes_serialize(ByteStream *buffer);
  // load all modes from a buffer, optionally specify number of leds
bool Modes_unserialize(ByteStream *buffer);

  // set default settings (must save after)
bool Modes_setDefaults();

  // shift the current mode to a different position relative to current position
  // negative values for up, positive values for down, 0 for no move
bool Modes_shiftCurMode(int32_t offset);

  // add a new mode in various different ways
bool Modes_addMode(PatternID id, RGBColor c1, RGBColor c2,
    RGBColor c3, RGBColor c4, RGBColor c5,
    RGBColor c6, RGBColor c7, RGBColor c8);
bool Modes_addMode(PatternID id, const PatternArgs *args, const Colorset *set);
bool Modes_addMode(const Mode *mode);

  // add a new mode by unserializing raw
bool Modes_addSerializedMode(ByteStream *serializedMode);
  // add a new mode by loading from a save buffer
bool Modes_addModeFromBuffer(ByteStream *serializedMode);

  // update the current mode to match the given mode, optionally save
bool Modes_updateCurMode(const Mode *mode);

  // set the current active mode by index
Mode *Modes_setCurMode(uint8_t index);

  // get the current mode
Mode *Modes_curMode();
  // iterate to next mode and return it
Mode *Modes_nextMode();
  // iterate to previous mode and return it
Mode *Modes_previousMode();

  // iterate to the next mode but skip empty modes, will not skip mode 0
  // to prevent possibly skipping all modes and ending in an infinite loop
Mode *Modes_nextModeSkipEmpty();

  // the number of modes
static inline uint8_t Modes_numModes() { return Modes_m_numModes; }
static inline uint8_t Modes_curModeIndex() { return Modes_m_curMode; }

  // the last time the modes switched to a new mode
static inline uint32_t Modes_lastSwitchTime() { return Modes_m_lastSwitchTime; }

  // delete the current mode
void Modes_deleteCurMode();

  // delete all modes in the list
void Modes_clearModes();

  // set the startup mode index (which mode will be displayed on startup)
void Modes_setStartupMode(uint8_t index);
uint8_t Modes_startupMode();
Mode *Modes_switchToStartupMode();

  // set or get flags
bool Modes_setFlag(uint8_t flag, bool enable, bool save);
static inline bool Modes_getFlag(uint8_t flag) { return ((Modes_m_globalFlags & flag) == flag); }

  // reset flags to factory default (must save after)
static inline void Modes_resetFlags() { Modes_m_globalFlags = 0; }

  // inline functions to toggle the various flags
static inline bool Modes_setOneClickMode(bool enable, bool save) {
    return Modes_setFlag(MODES_FLAG_ONE_CLICK, enable, save);
  }
static inline bool Modes_oneClickModeEnabled() {
    return Modes_getFlag(MODES_FLAG_ONE_CLICK);
  }
  // toggle the locked state
static inline bool Modes_setLocked(bool locked, bool save) {
    return Modes_setFlag(MODES_FLAG_LOCKED, locked, save);
  }
static inline bool Modes_locked() {
    return Modes_getFlag(MODES_FLAG_LOCKED);
  }
  // toggle advanced menus
static inline bool Modes_setAdvancedMenus(bool active, bool save) {
    return Modes_setFlag(MODES_FLAG_ADV_MENUS, active, save);
  }
static inline bool Modes_advancedMenusEnabled() {
    return Modes_getFlag(MODES_FLAG_ADV_MENUS);
  }
  // toggle the keychain light mode
static inline bool Modes_setKeychainMode(bool active, bool save) {
    return Modes_setFlag(MODES_FLAG_KEYCHAIN, active, save);
  }
static inline bool Modes_keychainModeEnabled() {
    return Modes_getFlag(MODES_FLAG_KEYCHAIN);
  }

#if MODES_TEST == 1
void Modes_test();
#endif

#ifdef VORTEX_LIB
  // get the maximum size a mode can occupy
uint32_t Modes_maxModeSize();
  // get the maximum size a savefile can occupy
uint32_t Modes_maxSaveSize();
  // global flags value
uint8_t Modes_globalFlags();
#endif

// a linked list node of internal mode storage
struct ModeLink
{
  Mode *m_pInstantiatedMode;
  ByteStream m_storedMode;
  ModeLink *m_next;
  ModeLink *m_prev;
};


void ModeLink_init(ModeLink *self, const Mode *src, bool inst);
void ModeLink_initBuf(ModeLink *self, const ByteStream *src, bool inst);
void ModeLink_destroy(ModeLink *self);
bool ModeLink_initMode(ModeLink *self, const Mode *mode);
bool ModeLink_appendMode(ModeLink *self, const Mode *next);
bool ModeLink_appendBuf(ModeLink *self, const ByteStream *next);
void ModeLink_play(ModeLink *self);
ModeLink *ModeLink_unlinkSelf(ModeLink *self);
void ModeLink_linkAfter(ModeLink *self, ModeLink *link);
void ModeLink_linkBefore(ModeLink *self, ModeLink *link);
Mode *ModeLink_instantiate(ModeLink *self);
void ModeLink_uninstantiate(ModeLink *self);
bool ModeLink_save(ModeLink *self);
ByteStream *ModeLink_buffer(ModeLink *self);
Mode *ModeLink_mode(ModeLink *self);
ModeLink *ModeLink_next(ModeLink *self);
ModeLink *ModeLink_prev(ModeLink *self);


  // fetch a link from the chain by index
ModeLink *Modes_getModeLink(uint32_t index);

  // initialize current mode from ByteStream, optionally force re-init which
  // will destroy the current instantiated mode and re-load it from serial
Mode *Modes_initCurMode(bool force);
bool Modes_saveCurMode();

  // whether modes have been loaded

  // the current mode we're on

  // the number of modes loaded

  // the current instantiated mode and it's respective link

  // list of serialized version of bufers

  // global flags for all modes

  // the last switch time of the modes


#endif
