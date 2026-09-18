

#include "../c_types.h"
// file-scope globals (replaces static class members)
static bool s_loaded = false;
static uint8_t s_curMode = 0;
static uint8_t s_numModes = 0;
uint8_t Modes_globalFlags = 0;
static uint32_t s_lastSwitchTime = 0;
static Mode s_liveMode;
static bool s_liveValid = false;
static uint8_t s_modeStoreData[MAX_MODE_SIZE];
static ByteStream s_modeBuf;
static uint8_t s_headerStoreData[MAX_MODE_SIZE];
static ByteStream s_headerBuf;

static void Modes_resetModeBuf(void)
{
  ByteStream_initStatic(&s_modeBuf, s_modeStoreData, sizeof(s_modeStoreData));
}

static void Modes_resetHeaderBuf(void)
{
  ByteStream_initStatic(&s_headerBuf, s_headerStoreData, sizeof(s_headerStoreData));
}

static void Modes_unloadLive(void)
{
  if (s_liveValid) {
    Mode_cleanup(&s_liveMode);
    s_liveValid = false;
  }
}

static bool Modes_loadLive(void)
{
  if (s_liveValid) {
    return true;
  }
  if (!s_numModes) {
    return false;
  }
  Modes_resetModeBuf();
  if (!Storage_read(s_curMode + 1, &s_modeBuf)) {
    DEBUG_LOG("Failed to read current mode from storage");
    return false;
  }
  Mode_init(&s_liveMode);
  if (!Mode_loadFromBuffer(&s_liveMode, &s_modeBuf)) {
    Mode_cleanup(&s_liveMode);
    return false;
  }
  s_liveValid = true;
  return true;
}

bool Modes_init(void)
{
#if MODES_TEST == 1
  Mode_test();
  Modes_test();
  return true;
#endif
  Modes_resetHeaderBuf();
  if (!Storage_read(0, &s_headerBuf) || !Modes_unserializeSaveHeader(&s_headerBuf)) {
    Modes_globalFlags |= MODES_FLAG_NEW_FIRMWARE;
  }
  s_loaded = false;
#ifdef VORTEX_LIB
  Modes_globalFlags |= MODES_FLAG_ADV_MENUS;
#endif
  return true;
}

void Modes_cleanup(void)
{
  Modes_unloadLive();
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
  if (!s_liveValid && !Modes_loadLive()) {
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
  Mode_play(&s_liveMode);
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
  Modes_resetHeaderBuf();
  if (!Modes_serializeSaveHeader(&s_headerBuf)) {
    return false;
  }
  if (!Storage_write(0, &s_headerBuf)) {
    return false;
  }
  return true;
}

bool Modes_loadHeader(void)
{
  Modes_resetHeaderBuf();
  if (!Storage_read(0, &s_headerBuf) || !ByteStream_size(&s_headerBuf)) {
    DEBUG_LOG("Empty buffer read from storage");
    return false;
  }
  Modes_clearModes();
  if (!Modes_unserializeSaveHeader(&s_headerBuf)) {
    return false;
  }
  return true;
}

bool Modes_saveStorage(void)
{
  DEBUG_LOG("Saving modes...");
  Modes_saveHeader();
  Modes_saveCurMode();
  DEBUG_LOGF("Serialized num modes: %u", s_numModes);
  return true;
}

bool Modes_loadStorage(void)
{
  Modes_resetHeaderBuf();
  if (!Storage_read(0, &s_headerBuf) || !ByteStream_size(&s_headerBuf)) {
    DEBUG_LOG("Empty buffer read from storage");
    return false;
  }
  Modes_clearModes();
  if (!Modes_unserializeSaveHeader(&s_headerBuf)) {
    return false;
  }
  uint8_t numModes = 0;
  if (!ByteStream_unserialize8(&s_headerBuf, &numModes)) {
    return false;
  }
  if (!numModes) {
    DEBUG_LOG("Did not find any modes");
    return false;
  }
  if (numModes > MAX_MODES) {
    numModes = MAX_MODES;
  }
  s_numModes = numModes;
  if (Modes_oneClickModeEnabled()) {
    Modes_switchToStartupMode();
  }
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
  for (uint8_t i = 0; i < s_numModes; ++i) {
    Mode tmpMode;
    Modes_resetModeBuf();
    if (!Storage_read(i + 1, &s_modeBuf)) {
      return false;
    }
    Mode_init(&tmpMode);
    if (!Mode_loadFromBuffer(&tmpMode, &s_modeBuf)) {
      Mode_cleanup(&tmpMode);
      return false;
    }
    bool result = Mode_serialize(&tmpMode, modesBuffer, 0);
    Mode_cleanup(&tmpMode);
    if (!result) {
      return false;
    }
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
    bool result = Modes_addModeMode(&defMode);
    Mode_cleanup(&defMode);
    if (!result) {
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
    Mode_cleanup(&tmpMode);
    return false;
  }
  Mode_initMode(&tmpMode);
  bool result = Modes_addModeMode(&tmpMode);
  Mode_cleanup(&tmpMode);
  return result;
}

bool Modes_addModeFromBuffer(ByteStream *serializedMode)
{
#if MAX_MODES != 0
  if (s_numModes >= MAX_MODES) {
    return false;
  }
#endif
  if (!ByteStream_size(serializedMode)) {
    return false;
  }
  if (ByteStream_rawSize(serializedMode) > MAX_MODE_SIZE) {
    ERROR_LOG("Mode too big for storage space");
    return false;
  }
  if (!Storage_write(s_numModes + 1, serializedMode)) {
    return false;
  }
  s_numModes++;
  return true;
}

bool Modes_shiftCurMode(int32_t offset)
{
  uint32_t newPos = (uint32_t)((int32_t)s_curMode + offset);
  if (!s_numModes || offset == 0 || newPos >= s_numModes) {
    return false;
  }
  Modes_unloadLive();
  int8_t step = (offset > 0) ? 1 : -1;
  uint8_t pos = s_curMode;
  while (pos != (uint8_t)newPos) {
    uint8_t other = (uint8_t)((int8_t)pos + step);
    if (!Storage_read(pos + 1, &s_modeBuf)) {
      return false;
    }
    if (!Storage_read(other + 1, &s_headerBuf)) {
      return false;
    }
    if (!Storage_write(pos + 1, &s_headerBuf)) {
      return false;
    }
    if (!Storage_write(other + 1, &s_modeBuf)) {
      return false;
    }
    pos = other;
  }
  s_curMode = (uint8_t)newPos;
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
  bool result = Modes_addModeMode(&tmpMode);
  Mode_cleanup(&tmpMode);
  return result;
}

bool Modes_addModeMode(const Mode *mode)
{
  if (!mode) {
    return false;
  }
#if MAX_MODES != 0
  if (s_numModes >= MAX_MODES) {
    return false;
  }
#endif
  Modes_resetModeBuf();
  if (!Mode_saveToBuffer(mode, &s_modeBuf, 0)) {
    return false;
  }
  if (ByteStream_rawSize(&s_modeBuf) > MAX_MODE_SIZE) {
    ERROR_LOG("Mode too big for storage space");
    return false;
  }
  if (!Storage_write(s_numModes + 1, &s_modeBuf)) {
    return false;
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
  Modes_unloadLive();
  return Modes_curMode() != NULL;
}

Mode *Modes_setCurMode(uint8_t index)
{
  if (!s_numModes) {
    return NULL;
  }
  Leds_clearAll();
  Modes_unloadLive();
  int8_t newModeIdx = index % s_numModes;
  s_curMode = (uint8_t)newModeIdx;
  s_lastSwitchTime = Time_getCurtime();
  Modes_setStartupMode((uint8_t)newModeIdx);
  Mode *newCur = Modes_curMode();
  if (!newCur) {
    ERROR_OUT_OF_MEMORY();
    return NULL;
  }
  DEBUG_LOGF("Switch to Mode: %u / %u (pattern id: %u)",
    s_curMode, s_numModes - 1, Pattern_getPatternID(Mode_getPatternConst(newCur, LED_ANY)));
  return newCur;
}

Mode *Modes_curMode(void)
{
  if (!s_numModes) {
    return NULL;
  }
  if (!s_liveValid) {
    if (!Modes_loadLive()) {
      ERROR_LOG("Failed to initialize current mode");
      return NULL;
    }
  }
  return &s_liveMode;
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
  if (!s_numModes || s_curMode >= s_numModes) {
    return;
  }
  Modes_unloadLive();
  for (uint8_t i = s_curMode; (uint8_t)(i + 1) < s_numModes; ++i) {
    Modes_resetModeBuf();
    if (!Storage_read(i + 2, &s_modeBuf)) {
      return;
    }
    if (!Storage_write(i + 1, &s_modeBuf)) {
      return;
    }
  }
  s_numModes--;
  if (!s_numModes) {
    s_curMode = 0;
    Leds_clearAll();
    return;
  }
  if (s_curMode >= s_numModes) {
    s_curMode = s_numModes - 1;
  }
  Modes_loadLive();
}

void Modes_clearModes(void)
{
  Modes_unloadLive();
  s_numModes = 0;
  s_curMode = 0;
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
  Modes_resetHeaderBuf();
  if (!Storage_read(0, &s_headerBuf) || !ByteStream_size(&s_headerBuf)) {
    return Modes_saveHeader();
  }
  typedef struct {
    uint8_t vMajor;
    uint8_t vMinor;
    uint8_t globalFlags;
    uint8_t brightness;
    uint8_t numModes;
  } SaveHeader;
  SaveHeader *pHeader = (SaveHeader *)ByteStream_data(&s_headerBuf);
  pHeader->globalFlags = Modes_globalFlags;
  ByteStream_setCRCDirty(&s_headerBuf);
  return Storage_write(0, &s_headerBuf);
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

Mode *Modes_initCurMode(bool force)
{
  (void)force;
  Modes_unloadLive();
  return Modes_curMode();
}

bool Modes_saveCurMode(void)
{
  if (!s_liveValid) {
    return true;
  }
  Modes_resetModeBuf();
  if (!Mode_saveToBuffer(&s_liveMode, &s_modeBuf, 0)) {
    return false;
  }
  if (ByteStream_rawSize(&s_modeBuf) > MAX_MODE_SIZE) {
    ERROR_LOG("Mode too big for storage space");
    return false;
  }
  if (!Storage_write(s_curMode + 1, &s_modeBuf)) {
    return false;
  }
  return true;
}

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
