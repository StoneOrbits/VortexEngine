#include "Modes.h"
#include "DefaultModes.h"
#include "Mode.h"

#include "../Patterns/Patterns.h"
#include "../Patterns/PatternArgs.h"
#include "../Patterns/Pattern.h"
#include "../Patterns/PatternBuilder.h"
#include "../Colors/Colorset.h"
#include "../Serial/ByteStream.h"
#include "../Time/TimeControl.h"
#include "../Storage/Storage.h"
#include "../Buttons/Buttons.h"
#include "../Buttons/Button.h"
#include "../Time/Timings.h"
#include "../Leds/Leds.h"
#include "../Log/Log.h"
#include "../Memory/Memory.h"

// file-scope globals (replaces static class members)
static bool s_loaded = false;
static uint8_t s_curMode = 0;
static uint8_t s_numModes = 0;
static ModeLink *s_pCurModeLink = NULL;
static ModeLink *s_storedModes = NULL;
uint8_t Modes_globalFlags = 0;
static uint32_t s_lastSwitchTime = 0;

bool Modes_init(void)
{
#if MODES_TEST == 1
  Mode_test();
  Modes_test();
  return true;
#endif
  ByteStream headerBuffer;
  ByteStream_init(&headerBuffer, 0, NULL);
  if (!Storage_read(0, &headerBuffer) || !Modes_unserializeSaveHeader(&headerBuffer)) {
    Modes_globalFlags |= MODES_FLAG_NEW_FIRMWARE;
  }
  ByteStream_destroy(&headerBuffer);
  s_loaded = false;
#ifdef VORTEX_LIB
  Modes_globalFlags |= MODES_FLAG_ADV_MENUS;
#endif
  return true;
}

void Modes_cleanup(void)
{
  Modes_clearModes();
}

bool Modes_load(void)
{
  if (s_loaded) {
    return true;
  }
  if (!Modes_loadStorage()) {
    if (!Modes_setDefaults()) {
      return false;
    }
    if (!Modes_saveStorage()) {
      return false;
    }
  }
  s_loaded = true;
  return true;
}

void Modes_play(void)
{
  if (!s_numModes) {
    Leds_clearAll();
    return;
  }
  if (!s_pCurModeLink && !Modes_initCurMode(false)) {
    DEBUG_LOG("Error failed to load any modes!");
    return;
  }
  if (Button_onShortClick(g_pButton)) {
    if (Modes_oneClickModeEnabled()) {
      extern void VortexEngine_enterSleep(bool save);
      VortexEngine_enterSleep(false);
      return;
    }
    Modes_nextMode();
  }
  ModeLink_play(s_pCurModeLink);
}

bool Modes_saveToBuffer(ByteStream *modesBuffer)
{
  (void)modesBuffer;
  return true;
}

bool Modes_loadFromBuffer(ByteStream *modesBuffer)
{
  if (!ByteStream_decompress(modesBuffer)) {
    return false;
  }
  if (!Modes_unserializeSaveHeader(modesBuffer)) {
    return false;
  }
  if (!Modes_unserialize(modesBuffer)) {
    return false;
  }
  if (Modes_oneClickModeEnabled()) {
    Modes_switchToStartupMode();
  }
  return true;
}

bool Modes_saveHeader(void)
{
  ByteStream headerBuffer;
  ByteStream_init(&headerBuffer, MAX_MODE_SIZE, NULL);
  if (!Modes_serializeSaveHeader(&headerBuffer)) {
    ByteStream_destroy(&headerBuffer);
    return false;
  }
  if (!Storage_write(0, &headerBuffer)) {
    ByteStream_destroy(&headerBuffer);
    return false;
  }
  ByteStream_destroy(&headerBuffer);
  return true;
}

bool Modes_loadHeader(void)
{
  ByteStream headerBuffer;
  ByteStream_init(&headerBuffer, 0, NULL);
  if (!Storage_read(0, &headerBuffer) || !ByteStream_size(&headerBuffer)) {
    DEBUG_LOG("Empty buffer read from storage");
    ByteStream_destroy(&headerBuffer);
    return false;
  }
  Modes_clearModes();
  if (!Modes_unserializeSaveHeader(&headerBuffer)) {
    ByteStream_destroy(&headerBuffer);
    return false;
  }
  ByteStream_destroy(&headerBuffer);
  return true;
}

bool Modes_saveStorage(void)
{
  DEBUG_LOG("Saving modes...");
  Modes_saveHeader();
  Modes_saveCurMode();
  if (s_pCurModeLink) {
    ModeLink_uninstantiate(s_pCurModeLink);
  }
  uint8_t i = 0;
  ModeLink *ptr = s_storedModes;
  while (ptr && i < MAX_MODES) {
    ByteStream modeBuffer;
    ByteStream_init(&modeBuffer, MAX_MODE_SIZE, NULL);
    Mode *mode = ModeLink_instantiate(ptr);
    if (!mode) {
      ERROR_OUT_OF_MEMORY();
      ByteStream_destroy(&modeBuffer);
      return false;
    }
    if (!Mode_serialize(mode, &modeBuffer, 0)) {
      ByteStream_destroy(&modeBuffer);
      return false;
    }
    ModeLink_uninstantiate(ptr);
    ptr = ModeLink_next(ptr);
    if (!Storage_write(++i, &modeBuffer)) {
      ByteStream_destroy(&modeBuffer);
      return false;
    }
    ByteStream_destroy(&modeBuffer);
  }
  if (s_pCurModeLink && !ModeLink_instantiate(s_pCurModeLink)) {
    return false;
  }
  DEBUG_LOGF("Serialized num modes: %u", s_numModes);
  return true;
}

bool Modes_loadStorage(void)
{
  ByteStream headerBuffer;
  ByteStream_init(&headerBuffer, 0, NULL);
  if (!Storage_read(0, &headerBuffer) || !ByteStream_size(&headerBuffer)) {
    DEBUG_LOG("Empty buffer read from storage");
    ByteStream_destroy(&headerBuffer);
    return false;
  }
  Modes_clearModes();
  if (!Modes_unserializeSaveHeader(&headerBuffer)) {
    ByteStream_destroy(&headerBuffer);
    return false;
  }
  uint8_t numModes = 0;
  if (!ByteStream_unserialize8(&headerBuffer, &numModes)) {
    ByteStream_destroy(&headerBuffer);
    return false;
  }
  if (!numModes) {
    DEBUG_LOG("Did not find any modes");
    ByteStream_destroy(&headerBuffer);
    return false;
  }
  for (uint8_t i = 0; i < numModes; ++i) {
    ByteStream modeBuffer;
    ByteStream_init(&modeBuffer, MAX_MODE_SIZE, NULL);
    if (!Storage_read(i + 1, &modeBuffer) || !Modes_addSerializedMode(&modeBuffer)) {
      ByteStream_destroy(&modeBuffer);
      ByteStream_destroy(&headerBuffer);
      return false;
    }
    ByteStream_destroy(&modeBuffer);
  }
  if (Modes_oneClickModeEnabled()) {
    Modes_switchToStartupMode();
  }
  ByteStream_destroy(&headerBuffer);
  return true;
}

bool Modes_serializeSaveHeader(ByteStream *saveBuffer)
{
  extern bool VortexEngine_serializeVersion(ByteStream *);
  if (!VortexEngine_serializeVersion(saveBuffer)) {
    return false;
  }
  if (!ByteStream_serialize8(saveBuffer, Modes_globalFlags)) {
    return false;
  }
  if (!ByteStream_serialize8(saveBuffer, (uint8_t)Leds_getBrightness())) {
    return false;
  }
  if (!ByteStream_serialize8(saveBuffer, s_numModes)) {
    return false;
  }
#ifdef VORTEX_EMBEDDED
  if (!ByteStream_serialize8(saveBuffer, (uint8_t)VORTEX_BUILD_NUMBER)) {
    return false;
  }
#endif
  DEBUG_LOGF("Serialized all modes, uncompressed size: %u", ByteStream_size(saveBuffer));
  return true;
}

bool Modes_unserializeSaveHeader(ByteStream *saveHeader)
{
  if (!ByteStream_decompress(saveHeader)) {
    return false;
  }
  ByteStream_resetUnserializer(saveHeader);
  uint8_t major = 0;
  uint8_t minor = 0;
  if (!ByteStream_unserialize8(saveHeader, &major)) {
    return false;
  }
  if (!ByteStream_unserialize8(saveHeader, &minor)) {
    return false;
  }
  extern bool VortexEngine_checkVersion(uint8_t major, uint8_t minor);
  if (!VortexEngine_checkVersion(major, minor)) {
    ERROR_LOGF("Incompatible savefile version: %u.%u", major, minor);
    return false;
  }
  if (!ByteStream_unserialize8(saveHeader, &Modes_globalFlags)) {
    return false;
  }
  uint8_t brightness = 0;
  if (!ByteStream_unserialize8(saveHeader, &brightness)) {
    return false;
  }
  if (brightness) {
    Leds_setBrightness(brightness);
  }
  return true;
}

bool Modes_serialize(ByteStream *modesBuffer)
{
  if (!ByteStream_serialize8(modesBuffer, s_numModes)) {
    return false;
  }
  Modes_saveCurMode();
  if (s_pCurModeLink) {
    ModeLink_uninstantiate(s_pCurModeLink);
  }
  ModeLink *ptr = s_storedModes;
  while (ptr) {
    Mode *mode = ModeLink_instantiate(ptr);
    if (!mode) {
      ERROR_OUT_OF_MEMORY();
      return false;
    }
    if (!Mode_serialize(mode, modesBuffer, 0)) {
      return false;
    }
    ModeLink_uninstantiate(ptr);
    ptr = ModeLink_next(ptr);
  }
  if (s_pCurModeLink && !ModeLink_instantiate(s_pCurModeLink)) {
    return false;
  }
  DEBUG_LOGF("Serialized num modes: %u", s_numModes);
  return true;
}

bool Modes_unserialize(ByteStream *modesBuffer)
{
  DEBUG_LOG("Loading modes...");
  Modes_clearModes();
  uint8_t numModes = 0;
  if (!ByteStream_unserialize8(modesBuffer, &numModes)) {
    return false;
  }
  if (!numModes) {
    DEBUG_LOG("Did not find any modes");
    return false;
  }
  for (uint8_t i = 0; i < numModes; ++i) {
    if (!Modes_addSerializedMode(modesBuffer)) {
      DEBUG_LOGF("Failed to add mode %u after unserialization", i);
      Modes_clearModes();
      return false;
    }
  }
  DEBUG_LOGF("Loaded %u modes from storage (%u bytes)", numModes, ByteStream_size(modesBuffer));
  return (s_numModes == numModes);
}

bool Modes_setDefaults(void)
{
  Modes_clearModes();
  for (uint8_t i = 0; i < MAX_MODES; ++i) {
    Mode defMode;
    Mode_initFromEntry(&defMode, &defaultModes[i]);
    if (!Modes_addModeMode(&defMode)) {
      ERROR_LOGF("Failed to add default mode %u", i);
      return false;
    }
  }
  return true;
}

bool Modes_addSerializedMode(ByteStream *serializedMode)
{
#if MAX_MODES != 0
  if (s_numModes >= MAX_MODES) {
    return false;
  }
#endif
  Mode tmpMode;
  Mode_init(&tmpMode);
  if (!Mode_unserialize(&tmpMode, serializedMode)) {
    return false;
  }
  Mode_initMode(&tmpMode);
  return Modes_addModeMode(&tmpMode);
}

bool Modes_addModeFromBuffer(ByteStream *serializedMode)
{
#if MAX_MODES != 0
  if (s_numModes >= MAX_MODES) {
    return false;
  }
#endif
  if (!s_storedModes) {
    s_storedModes = (ModeLink *)vmalloc(sizeof(ModeLink));
    if (!s_storedModes) {
      ERROR_OUT_OF_MEMORY();
      return false;
    }
    ModeLink_initFromStream(s_storedModes, serializedMode, false);
  } else {
    if (!ModeLink_appendStream(s_storedModes, serializedMode)) {
      ERROR_OUT_OF_MEMORY();
      return false;
    }
  }
  s_numModes++;
  return true;
}

bool Modes_shiftCurMode(int32_t offset)
{
  uint32_t newPos = (uint32_t)((int32_t)s_curMode + offset);
  if (newPos >= s_numModes) {
    return false;
  }
  if (newPos == s_curMode) {
    return true;
  }
  ModeLink *target = Modes_getModeLink(newPos);
  if (!target) {
    return false;
  }
  if (!s_curMode && offset > 0) {
    s_storedModes = ModeLink_next(s_storedModes);
  }
  ModeLink_unlinkSelf(s_pCurModeLink);
  s_curMode = (uint8_t)newPos;
  if (offset < 0) {
    ModeLink_linkBefore(target, s_pCurModeLink);
    if (!s_curMode) {
      s_storedModes = s_pCurModeLink;
    }
  } else {
    ModeLink_linkAfter(target, s_pCurModeLink);
  }
  return true;
}

bool Modes_addMode(PatternID id, RGBColor c1, RGBColor c2, RGBColor c3,
    RGBColor c4, RGBColor c5, RGBColor c6, RGBColor c7, RGBColor c8)
{
  Colorset set;
  Colorset_initColors(&set, c1, c2, c3, c4, c5, c6, c7, c8);
  return Modes_addModeArgsSet(id, NULL, &set);
}

bool Modes_addModeArgsSet(PatternID id, const PatternArgs *args, const Colorset *set)
{
#if MAX_MODES != 0
  if (s_numModes >= MAX_MODES) {
    return false;
  }
#endif
  if (id >= PATTERN_COUNT) {
    return false;
  }
  Mode tmpMode;
  Mode_initFromIDArgsSetPtr(&tmpMode, id, args, set);
  Mode_initMode(&tmpMode);
  return Modes_addModeMode(&tmpMode);
}

bool Modes_addModeMode(const Mode *mode)
{
#if MAX_MODES != 0
  if (s_numModes >= MAX_MODES) {
    return false;
  }
#endif
  if (!s_storedModes) {
    s_storedModes = (ModeLink *)vmalloc(sizeof(ModeLink));
    if (!s_storedModes) {
      ERROR_OUT_OF_MEMORY();
      return false;
    }
    ModeLink_init(s_storedModes, mode, false);
  } else {
    if (!ModeLink_appendMode(s_storedModes, mode)) {
      ERROR_OUT_OF_MEMORY();
      return false;
    }
  }
  s_numModes++;
  return true;
}

bool Modes_updateCurMode(const Mode *mode)
{
  if (!mode) {
    return false;
  }
  Mode *pCur = Modes_curMode();
  if (!pCur) {
    return false;
  }
  Mode_assign(pCur, mode);
  if (!Modes_saveCurMode()) {
    return false;
  }
  return Modes_initCurMode(false) != NULL;
}

Mode *Modes_setCurMode(uint8_t index)
{
  if (!s_numModes) {
    return NULL;
  }
  Leds_clearAll();
  if (s_pCurModeLink) {
    ModeLink_uninstantiate(s_pCurModeLink);
  }
  int8_t newModeIdx = index % s_numModes;
  ModeLink *newCurLink = Modes_getModeLink((uint32_t)newModeIdx);
  if (!newCurLink) {
    return NULL;
  }
  Mode *newCur = ModeLink_instantiate(newCurLink);
  if (!newCur) {
    ERROR_OUT_OF_MEMORY();
    return NULL;
  }
  s_curMode = (uint8_t)newModeIdx;
  s_pCurModeLink = newCurLink;
  s_lastSwitchTime = Time_getCurtime();
  Modes_setStartupMode((uint8_t)newModeIdx);
  DEBUG_LOGF("Switch to Mode: %u / %u (pattern id: %u)",
    s_curMode, s_numModes - 1, Pattern_getPatternID(Mode_getPatternConst(newCur, LED_ANY)));
  return newCur;
}

Mode *Modes_curMode(void)
{
  if (!s_numModes) {
    return NULL;
  }
  if (!s_pCurModeLink) {
    if (!Modes_initCurMode(false)) {
      ERROR_LOG("Failed to initialize current mode");
      return NULL;
    }
  }
  return ModeLink_instantiate(s_pCurModeLink);
}

Mode *Modes_nextMode(void)
{
  if (!s_numModes) {
    return NULL;
  }
  return Modes_setCurMode(s_curMode + 1);
}

Mode *Modes_previousMode(void)
{
  if (!s_numModes) {
    return NULL;
  }
  if (!s_curMode) {
    return Modes_setCurMode(s_numModes - 1);
  }
  return Modes_setCurMode(s_curMode - 1);
}

Mode *Modes_nextModeSkipEmpty(void)
{
  do {
    if (Modes_setCurMode(s_curMode + 1) && !Mode_isEmpty(Modes_curMode())) {
      break;
    }
  } while (s_curMode != 0);
  return Modes_curMode();
}

void Modes_deleteCurMode(void)
{
  if (!s_numModes || !s_pCurModeLink) {
    return;
  }
  ModeLink *newCur = ModeLink_unlinkSelf(s_pCurModeLink);
  ModeLink_cleanup(s_pCurModeLink);
  vfree(s_pCurModeLink);
  s_pCurModeLink = newCur;
  if (s_curMode) {
    s_curMode--;
  } else {
    s_storedModes = s_pCurModeLink;
  }
  s_numModes--;
  if (!s_numModes) {
    s_storedModes = NULL;
  }
}

void Modes_clearModes(void)
{
  if (!s_numModes || !s_storedModes) {
    return;
  }
  ModeLink_cleanup(s_storedModes);
  vfree(s_storedModes);
  s_pCurModeLink = NULL;
  s_storedModes = NULL;
  s_numModes = 0;
  Leds_clearAll();
}

void Modes_setStartupMode(uint8_t index)
{
  Modes_globalFlags &= 0x0F;
  Modes_globalFlags |= (index << 4) & 0xF0;
}

uint8_t Modes_startupMode(void)
{
  return (Modes_globalFlags & 0xF0) >> 4;
}

Mode *Modes_switchToStartupMode(void)
{
  return Modes_setCurMode(Modes_startupMode());
}

bool Modes_setFlag(uint8_t flag, bool enable, bool save)
{
  if (enable) {
    Modes_globalFlags |= flag;
  } else {
    Modes_globalFlags &= ~flag;
  }
  DEBUG_LOGF("Toggled instant on/off to %s", enable ? "on" : "off");
  if (!save) {
    return true;
  }
  ByteStream headerBuffer;
  ByteStream_init(&headerBuffer, 0, NULL);
  if (!Storage_read(0, &headerBuffer) || !ByteStream_size(&headerBuffer)) {
    ByteStream_destroy(&headerBuffer);
    return Modes_saveHeader();
  }
  typedef struct {
    uint8_t vMajor;
    uint8_t vMinor;
    uint8_t globalFlags;
    uint8_t brightness;
    uint8_t numModes;
  } SaveHeader;
  SaveHeader *pHeader = (SaveHeader *)ByteStream_data(&headerBuffer);
  pHeader->globalFlags = Modes_globalFlags;
  ByteStream_setCRCDirty(&headerBuffer);
  bool result = Storage_write(0, &headerBuffer);
  ByteStream_destroy(&headerBuffer);
  return result;
}

bool Modes_getFlag(uint8_t flag)
{
  return ((Modes_globalFlags & flag) == flag);
}

void Modes_resetFlags(void)
{
  Modes_globalFlags = 0;
}

bool Modes_setOneClickMode(bool enable, bool save)
{
  return Modes_setFlag(MODES_FLAG_ONE_CLICK, enable, save);
}

bool Modes_oneClickModeEnabled(void)
{
  return Modes_getFlag(MODES_FLAG_ONE_CLICK);
}

bool Modes_setLocked(bool locked, bool save)
{
  return Modes_setFlag(MODES_FLAG_LOCKED, locked, save);
}

bool Modes_locked(void)
{
  return Modes_getFlag(MODES_FLAG_LOCKED);
}

bool Modes_setAdvancedMenus(bool active, bool save)
{
  return Modes_setFlag(MODES_FLAG_ADV_MENUS, active, save);
}

bool Modes_advancedMenusEnabled(void)
{
  return Modes_getFlag(MODES_FLAG_ADV_MENUS);
}

bool Modes_setKeychainMode(bool active, bool save)
{
  return Modes_setFlag(MODES_FLAG_KEYCHAIN, active, save);
}

bool Modes_keychainModeEnabled(void)
{
  return Modes_getFlag(MODES_FLAG_KEYCHAIN);
}

uint8_t Modes_numModes(void) { return s_numModes; }
uint8_t Modes_curModeIndex(void) { return s_curMode; }
uint32_t Modes_lastSwitchTime(void) { return s_lastSwitchTime; }

#ifdef VORTEX_LIB
uint32_t Modes_maxModeSize(void)
{
  Mode maxMode;
  Mode_init(&maxMode);
  uint8_t x = 0;
  for (LedPos p = LED_FIRST; p < LED_COUNT; ++p) {
    PatternArgs maxArgs;
    PatternArgs_init8(&maxArgs,
      (uint8_t)p + 0xd2, (uint8_t)p + 0xd3, (uint8_t)p + 0xd4, (uint8_t)p + 0xd5,
      (uint8_t)p + 0xd6, (uint8_t)p + 0xd7, (uint8_t)p + 0xd8, (uint8_t)p + 0xd9);
    Colorset maxSet;
    Colorset_init(&maxSet);
    for (uint8_t i = 0; i < MAX_COLOR_SLOTS; ++i) {
      RGBColor col;
      col.red = ++x;
      col.green = ++x;
      col.blue = ++x;
      Colorset_addColor(&maxSet, col);
    }
    Mode_setPattern(&maxMode, PATTERN_BLEND, p, &maxArgs, &maxSet);
  }
  ByteStream stream;
  ByteStream_init(&stream, 0, NULL);
  Mode_saveToBuffer(&maxMode, &stream, 0);
  uint32_t size = ByteStream_size(&stream);
  ByteStream_destroy(&stream);
  return size;
}

uint32_t Modes_maxSaveSize(void)
{
#if MAX_MODES == 0
  return 0;
#else
  ByteStream backupModes;
  ByteStream_init(&backupModes, 0, NULL);
  Modes_saveToBuffer(&backupModes);
  for (uint32_t i = 0; i < MAX_MODES; ++i) {
    Mode maxMode;
    Mode_init(&maxMode);
    for (LedPos p = LED_FIRST; p < LED_COUNT; ++p) {
      PatternArgs maxArgs;
      PatternArgs_init8(&maxArgs,
        (uint8_t)p + 2, (uint8_t)p + 3, (uint8_t)p + 4, (uint8_t)p + 5,
        (uint8_t)p + 6, (uint8_t)p + 7, (uint8_t)p + 8, (uint8_t)p + 9);
      Colorset maxSet;
      Colorset_init(&maxSet);
      for (uint32_t j = 0; j < MAX_COLOR_SLOTS; ++j) {
        RGBColor col;
        col.red = (uint8_t)p + (j * 3);
        col.green = (uint8_t)p + (j * 3) + 1;
        col.blue = (uint8_t)p + (j * 3) + 2;
        Colorset_addColor(&maxSet, col);
      }
      Mode_setPattern(&maxMode, PATTERN_BLEND, p, &maxArgs, &maxSet);
    }
    Modes_addModeMode(&maxMode);
  }
  ByteStream stream;
  ByteStream_init(&stream, 0, NULL);
  Modes_saveToBuffer(&stream);
  uint32_t size = ByteStream_size(&stream);
  Modes_loadFromBuffer(&backupModes);
  ByteStream_destroy(&stream);
  ByteStream_destroy(&backupModes);
  return size;
#endif
}

uint8_t Modes_getGlobalFlags(void)
{
  return Modes_globalFlags;
}
#endif

ModeLink *Modes_getModeLink(uint32_t index)
{
  if (index >= s_numModes) {
    return NULL;
  }
  ModeLink *ptr = s_storedModes;
  while (index > 0 && ptr) {
    ptr = ModeLink_next(ptr);
    index--;
  }
  return ptr;
}

Mode *Modes_initCurMode(bool force)
{
  if (!s_numModes) {
    return NULL;
  }
  if (s_pCurModeLink) {
    ModeLink_uninstantiate(s_pCurModeLink);
  }
  s_pCurModeLink = Modes_getModeLink(s_curMode);
  if (!s_pCurModeLink) {
    return NULL;
  }
  if (force) {
    ModeLink_uninstantiate(s_pCurModeLink);
  }
  return ModeLink_instantiate(s_pCurModeLink);
}

bool Modes_saveCurMode(void)
{
  if (!s_pCurModeLink) {
    return false;
  }
  return ModeLink_save(s_pCurModeLink);
}

// ModeLink implementation
void ModeLink_init(ModeLink *self, const Mode *src, bool inst)
{
  self->m_pInstantiatedMode = NULL;
  ByteStream_init(&self->m_storedMode, 0, NULL);
  self->m_next = NULL;
  self->m_prev = NULL;
  if (src) {
    ModeLink_initMode(self, src);
  }
  if (src && inst) {
    ModeLink_instantiate(self);
  }
}

void ModeLink_initFromStream(ModeLink *self, const ByteStream *src, bool inst)
{
  self->m_pInstantiatedMode = NULL;
  ByteStream_copy(&self->m_storedMode, src);
  self->m_next = NULL;
  self->m_prev = NULL;
  if (ByteStream_size(&self->m_storedMode) && inst) {
    ModeLink_instantiate(self);
  }
}

void ModeLink_cleanup(ModeLink *self)
{
  if (self->m_next) {
    ModeLink_cleanup(self->m_next);
    vfree(self->m_next);
  }
  if (self->m_pInstantiatedMode) {
    Mode_cleanup(self->m_pInstantiatedMode);
    vfree(self->m_pInstantiatedMode);
  }
  ByteStream_destroy(&self->m_storedMode);
}

bool ModeLink_initMode(ModeLink *self, const Mode *mode)
{
  if (!mode) {
    return false;
  }
  ByteStream_clear(&self->m_storedMode);
  return Mode_saveToBuffer(mode, &self->m_storedMode, 0);
}

bool ModeLink_appendMode(ModeLink *self, const Mode *next)
{
  if (!next) {
    return false;
  }
  if (self->m_next) {
    return ModeLink_appendMode(self->m_next, next);
  }
  self->m_next = (ModeLink *)vmalloc(sizeof(ModeLink));
  if (!self->m_next) {
    ERROR_OUT_OF_MEMORY();
    return false;
  }
  ModeLink_init(self->m_next, next, false);
  self->m_next->m_prev = self;
  return true;
}

bool ModeLink_appendStream(ModeLink *self, const ByteStream *next)
{
  if (!ByteStream_size(next)) {
    return false;
  }
  if (self->m_next) {
    return ModeLink_appendStream(self->m_next, next);
  }
  self->m_next = (ModeLink *)vmalloc(sizeof(ModeLink));
  if (!self->m_next) {
    ERROR_OUT_OF_MEMORY();
    return false;
  }
  ModeLink_initFromStream(self->m_next, next, false);
  self->m_next->m_prev = self;
  return true;
}

void ModeLink_play(ModeLink *self)
{
  if (!self->m_pInstantiatedMode) {
    return;
  }
  Mode_play(self->m_pInstantiatedMode);
}

ModeLink *ModeLink_unlinkSelf(ModeLink *self)
{
  if (self->m_prev) {
    self->m_prev->m_next = self->m_next;
  }
  if (self->m_next) {
    self->m_next->m_prev = self->m_prev;
  }
  ModeLink *newLink = self->m_prev ? self->m_prev : self->m_next;
  self->m_prev = NULL;
  self->m_next = NULL;
  return newLink;
}

void ModeLink_linkAfter(ModeLink *self, ModeLink *link)
{
  if (!link) {
    return;
  }
  if (self->m_next) {
    self->m_next->m_prev = link;
    link->m_next = self->m_next;
  }
  self->m_next = link;
  link->m_prev = self;
}

void ModeLink_linkBefore(ModeLink *self, ModeLink *link)
{
  if (!link) {
    return;
  }
  if (self->m_prev) {
    self->m_prev->m_next = link;
    link->m_prev = self->m_prev;
  }
  self->m_prev = link;
  link->m_next = self;
}

Mode *ModeLink_instantiate(ModeLink *self)
{
  if (self->m_pInstantiatedMode) {
    return self->m_pInstantiatedMode;
  }
  Mode *newMode = (Mode *)vmalloc(sizeof(Mode));
  if (!newMode) {
    ERROR_OUT_OF_MEMORY();
    return NULL;
  }
  Mode_init(newMode);
  ByteStream_resetUnserializer(&self->m_storedMode);
  if (!Mode_loadFromBuffer(newMode, &self->m_storedMode)) {
    vfree(newMode);
    return NULL;
  }
  self->m_pInstantiatedMode = newMode;
  return self->m_pInstantiatedMode;
}

void ModeLink_uninstantiate(ModeLink *self)
{
  if (self->m_pInstantiatedMode) {
    Mode_cleanup(self->m_pInstantiatedMode);
    vfree(self->m_pInstantiatedMode);
    self->m_pInstantiatedMode = NULL;
  }
}

bool ModeLink_save(ModeLink *self)
{
  if (!self->m_pInstantiatedMode) {
    return false;
  }
  ByteStream_clear(&self->m_storedMode);
  return Mode_saveToBuffer(self->m_pInstantiatedMode, &self->m_storedMode, 0);
}

ByteStream *ModeLink_buffer(ModeLink *self) { return &self->m_storedMode; }
Mode *ModeLink_mode(ModeLink *self) { return self->m_pInstantiatedMode; }
ModeLink *ModeLink_next(ModeLink *self) { return self->m_next; }
ModeLink *ModeLink_prev(ModeLink *self) { return self->m_prev; }

#if MODES_TEST == 1
#include <assert.h>
#include <stdio.h>

void Modes_test(void)
{
  INFO_LOG("== Beginning Modes Test ==\n");

  RGBColor col = RGB_RED;
  assert(!Modes_addMode(PATTERN_COUNT, col, RGB_OFF, RGB_OFF, RGB_OFF, RGB_OFF, RGB_OFF, RGB_OFF, RGB_OFF));
  Modes_clearModes();
  for (PatternID pat = PATTERN_FIRST; pat < PATTERN_COUNT; ++pat) {
    assert(Modes_addMode(pat, col, RGB_OFF, RGB_OFF, RGB_OFF, RGB_OFF, RGB_OFF, RGB_OFF, RGB_OFF));
  }
  assert(Modes_numModes() == PATTERN_COUNT);
  Modes_clearModes();
  assert(Modes_numModes() == 0);

  Colorset set;
  Colorset_initColors(&set, (RGBColor){255, 0, 0}, (RGBColor){0, 255, 0}, (RGBColor){0, 0, 255});
  assert(!Modes_addModeArgsSet(PATTERN_COUNT, NULL, &set));
  for (PatternID pat = PATTERN_FIRST; pat < PATTERN_COUNT; ++pat) {
    assert(Modes_addModeArgsSet(pat, NULL, &set));
  }
  assert(Modes_numModes() == PATTERN_COUNT);
  Modes_clearModes();
  assert(Modes_numModes() == 0);

  Colorset set2;
  Colorset_initColors(&set2, (RGBColor){255, 0, 0}, (RGBColor){0, 255, 0}, (RGBColor){0, 0, 255});
  for (PatternID pat = PATTERN_FIRST; pat < PATTERN_COUNT; ++pat) {
    Mode tmpMode;
    Mode_initFromIDArgsSetPtr(&tmpMode, pat, NULL, &set2);
    assert(Modes_addModeMode(&tmpMode));
  }
  assert(Modes_numModes() == PATTERN_COUNT);
  Modes_clearModes();
  assert(Modes_numModes() == 0);

  INFO_LOG("addMode(): success\n");

  ByteStream modebuf;
  ByteStream_init(&modebuf, 0, NULL);
  ByteStream modesave;
  ByteStream_init(&modesave, 0, NULL);
  PatternArgs args = PatternBuilder_getDefaultArgs(PATTERN_BASIC);
  Mode tmpMode;
  Mode_initFromIDArgsSet(&tmpMode, PATTERN_BASIC, &args, &set);
  Mode_serialize(&tmpMode, &modebuf, 0);
  Mode_saveToBuffer(&tmpMode, &modesave, 0);
  assert(Modes_addSerializedMode(&modebuf));
  assert(Modes_numModes() == 1);
  assert(Modes_addModeFromBuffer(&modesave));
  assert(Modes_numModes() == 2);
  assert(Modes_getModeLink(0) != NULL);
  Mode *mode1 = ModeLink_instantiate(Modes_getModeLink(0));
  assert(mode1 != NULL);
  Mode *mode2 = ModeLink_instantiate(Modes_getModeLink(1));
  assert(mode2 != NULL);
  assert(Mode_equals(mode1, mode2));

  INFO_LOG("addSerializedMode(): success\n");

  INFO_LOG("== Success Running Modes Test ==\n");
}
#endif
