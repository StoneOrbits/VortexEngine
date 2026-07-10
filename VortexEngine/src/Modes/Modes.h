#ifndef MODES_H
#define MODES_H

#include <inttypes.h>
#include <stdbool.h>
#include "../Serial/ByteStream.h"
#include "../Colors/ColorTypes.h"
#include "../Patterns/Patterns.h"
#include "../Leds/Leds.h"
#include "../VortexConfig.h"

typedef struct PatternArgs_s PatternArgs;
typedef struct Colorset_s Colorset;
typedef struct Mode Mode;

#define MODES_FLAG_LOCKED     (1 << 0)
#define MODES_FLAG_ONE_CLICK  (1 << 1)
#define MODES_FLAG_ADV_MENUS  (1 << 2)
#define MODES_FLAG_KEYCHAIN   (1 << 3)
#define MODES_FLAG_NEW_FIRMWARE 0xF0

typedef struct ModeLink {
  Mode *m_pInstantiatedMode;
  ByteStream m_storedMode;
  struct ModeLink *m_next;
  struct ModeLink *m_prev;
} ModeLink;

void ModeLink_init(ModeLink *self, const Mode *src, bool inst);
void ModeLink_initFromStream(ModeLink *self, const ByteStream *src, bool inst);
void ModeLink_cleanup(ModeLink *self);

bool ModeLink_initMode(ModeLink *self, const Mode *mode);
bool ModeLink_appendMode(ModeLink *self, const Mode *next);
bool ModeLink_appendStream(ModeLink *self, const ByteStream *next);

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

// Modes module-level functions
bool Modes_init(void);
void Modes_cleanup(void);
bool Modes_load(void);
void Modes_play(void);

bool Modes_serialize(ByteStream *modesBuffer);
bool Modes_unserialize(ByteStream *modesBuffer);
bool Modes_saveToBuffer(ByteStream *saveBuffer);
bool Modes_loadFromBuffer(ByteStream *saveBuffer);
bool Modes_saveHeader(void);
bool Modes_loadHeader(void);
bool Modes_saveStorage(void);
bool Modes_loadStorage(void);

bool Modes_serializeSaveHeader(ByteStream *saveBuffer);
bool Modes_unserializeSaveHeader(ByteStream *saveHeader);

bool Modes_setDefaults(void);
bool Modes_shiftCurMode(int32_t offset);
bool Modes_addMode(PatternID id, RGBColor c1, RGBColor c2, RGBColor c3, RGBColor c4, RGBColor c5, RGBColor c6, RGBColor c7, RGBColor c8);
bool Modes_addModeArgsSet(PatternID id, const PatternArgs *args, const Colorset *set);
bool Modes_addModeMode(const Mode *mode);
bool Modes_addSerializedMode(ByteStream *serializedMode);
bool Modes_addModeFromBuffer(ByteStream *serializedMode);
bool Modes_updateCurMode(const Mode *mode);
Mode *Modes_setCurMode(uint8_t index);
Mode *Modes_curMode(void);
Mode *Modes_nextMode(void);
Mode *Modes_previousMode(void);
Mode *Modes_nextModeSkipEmpty(void);

uint8_t Modes_numModes(void);
uint8_t Modes_curModeIndex(void);
uint32_t Modes_lastSwitchTime(void);

void Modes_deleteCurMode(void);
void Modes_clearModes(void);

void Modes_setStartupMode(uint8_t index);
uint8_t Modes_startupMode(void);
Mode *Modes_switchToStartupMode(void);

bool Modes_setFlag(uint8_t flag, bool enable, bool save);
bool Modes_getFlag(uint8_t flag);
void Modes_resetFlags(void);

bool Modes_setOneClickMode(bool enable, bool save);
bool Modes_oneClickModeEnabled(void);
bool Modes_setLocked(bool locked, bool save);
bool Modes_locked(void);
bool Modes_setAdvancedMenus(bool active, bool save);
bool Modes_advancedMenusEnabled(void);
bool Modes_setKeychainMode(bool active, bool save);
bool Modes_keychainModeEnabled(void);

#if MODES_TEST == 1
void Modes_test(void);
#endif

#ifdef VORTEX_LIB
uint32_t Modes_maxModeSize(void);
uint32_t Modes_maxSaveSize(void);
uint8_t Modes_getGlobalFlags(void);
#endif

// internal functions
ModeLink *Modes_getModeLink(uint32_t index);
Mode *Modes_initCurMode(bool force);
bool Modes_saveCurMode(void);

// internal globals (accessed only through functions)
extern uint8_t Modes_globalFlags;

#endif
