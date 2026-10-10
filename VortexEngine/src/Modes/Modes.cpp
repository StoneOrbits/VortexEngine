#include "Modes.h"
#include "DefaultModes.h"

#include "../VortexEngine.h"
#include "../Memory/Memory.h"

#include "../Patterns/Pattern.h"

#include "../Serial/ByteStream.h"
#include "../Time/TimeControl.h"
#include "../Colors/Colorset.h"
#include "../Storage/Storage.h"
#include "../Buttons/Buttons.h"
#include "../Time/Timings.h"
#include "../Modes/Mode.h"
#include "../Leds/Leds.h"
#include "../Log/Log.h"

// static members
bool Modes_m_loaded = false;
uint8_t Modes_m_curMode = 0;
uint8_t Modes_m_numModes = 0;
// the current instantiated mode and it's respective link
ModeLink *Modes_m_pCurModeLink = NULL;
// list of serialized version of bufers
ModeLink *Modes_m_storedModes = NULL;
// global flags for all modes
uint32_t Modes_m_globalFlags = 0;
// the last switch time of the modes
uint32_t Modes_m_lastSwitchTime = 0;

// the pools the mode links and the instantiated modes are allocated from
static MemPool s_modeLinkPool;
static MEMPOOL_STORAGE(s_modeLinkStorage, sizeof(ModeLink), MODELINK_POOL_SIZE);
static MemPool s_modePool;
static MEMPOOL_STORAGE(s_modeStorage, sizeof(Mode), MODE_POOL_SIZE);

bool Modes_init()
{
  MemPool_init(&s_modeLinkPool, s_modeLinkStorage, sizeof(ModeLink), MODELINK_POOL_SIZE);
  MemPool_init(&s_modePool, s_modeStorage, sizeof(Mode), MODE_POOL_SIZE);
#if MODES_TEST == 1
  Mode_test();
  Modes_test();
  return true;
#endif
  ByteStream headerBuffer;
  ByteStream_make(&headerBuffer);
  if (!Storage_read(0, &headerBuffer) || !Modes_unserializeSaveHeader(&headerBuffer)) {
    // cannot read or load header? corrupted header?
    // just mark this as a new firmware and it will trigger a fresh header
    // write later in the main loop when modes get loaded
    Modes_m_globalFlags |= MODES_FLAG_NEW_FIRMWARE;
  }
  ByteStream_destroy(&headerBuffer);
  Modes_m_loaded = false;
#ifdef VORTEX_LIB
  // enable the adv menus by default in vortex lib
  Modes_m_globalFlags |= MODES_FLAG_ADV_MENUS;
#endif
  return true;
}

void Modes_cleanup()
{
  Modes_clearModes();
}

bool Modes_load()
{
  if (Modes_m_loaded) {
    return true;
  }
  // try to load the saved settings or set defaults
  if (!Modes_loadStorage()) {
    if (!Modes_setDefaults()) {
      return false;
    }
    if (!Modes_saveStorage()) {
      return false;
    }
  }
  Modes_m_loaded = true;
  return true;
}

void Modes_play()
{
  if (!Modes_m_numModes) {
    // nothing to do just keep the leds cleared
    Leds_clearAll();
    return;
  }
  // should be able to initialize the current mode
  if (!Modes_m_pCurModeLink && !Modes_initCurMode(false)) {
    DEBUG_LOG("Error failed to load any modes!");
    return;
  }
  // shortclick either turns off the lights, cycles to the next mode
  // or possibly locks the lights based on the situation
  if (Button_onShortClick(g_pButton)) {
    if (Modes_oneClickModeEnabled()) {
      // enter sleep doesn't return on arduino, but it does on vortexlib
      // so we need to return right after -- we can't just use an else
      // don't need to save when switching on/off in one click mode
      VortexEngine_enterSleep(false);
      return;
    }
    Modes_nextMode();
  }
  // play the current mode
  ModeLink_play(Modes_m_pCurModeLink);
}

// full save/load to/from buffer
bool Modes_saveToBuffer(ByteStream *modesBuffer)
{
  // This API is removed for the DUO
  return true;
}

// load modes from a save buffer
bool Modes_loadFromBuffer(ByteStream *modesBuffer)
{
  if (!ByteStream_decompress(modesBuffer)) {
    // failed to decompress?
    return false;
  }
  if (!Modes_unserializeSaveHeader(modesBuffer)) {
    return false;
  }
  // now just unserialize the list of modes
  if (!Modes_unserialize(modesBuffer)) {
    return false;
  }
  if (Modes_oneClickModeEnabled()) {
    // set the current mode to the startup mode
    Modes_switchToStartupMode();
  }
  return true;
}

bool Modes_saveHeader()
{
  ByteStream headerBuffer;
  ByteStream_make(&headerBuffer);
  ByteStream_init(&headerBuffer, MAX_MODE_SIZE);
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

bool Modes_loadHeader()
{
  ByteStream headerBuffer;
  ByteStream_make(&headerBuffer);
  // only read storage if the modebuffer isn't filled
  if (!Storage_read(0, &headerBuffer) || !headerBuffer.m_pData->size) {
    DEBUG_LOG("Empty buffer read from storage");
    // this kinda sucks whatever they had loaded is gone
    ByteStream_destroy(&headerBuffer);
    return false;
  }
  // this erases what is stored before we know whether there is data
  // but it's the easiest way to just re-load new data from storage
  Modes_clearModes();
  // read the header and load the data
  if (!Modes_unserializeSaveHeader(&headerBuffer)) {
    ByteStream_destroy(&headerBuffer);
    return false;
  }
  ByteStream_destroy(&headerBuffer);
  // NOTE: We do not bother loading the number of modes because
  //       we can't really do anything with it anyway
  return true;
}

// NOTE: Flash storage is limited to about 10,000 writes so
//       use this function sparingly!
bool Modes_saveStorage()
{
  DEBUG_LOG("Saving modes...");
  Modes_saveHeader();
  // make sure the current mode is saved in case it has changed somehow
  Modes_saveCurMode();
  // uninstantiate cur mode so we have stack space to serialize
  if (Modes_m_pCurModeLink) {
    ModeLink_uninstantiate(Modes_m_pCurModeLink);
  }
  uint8_t i = 0;
  ModeLink *ptr = Modes_m_storedModes;
  while (ptr && i < MAX_MODES) {
    ByteStream modeBuffer;
    ByteStream_make(&modeBuffer);
    ByteStream_init(&modeBuffer, MAX_MODE_SIZE);
    // instantiate the mode temporarily
    Mode *mode = ModeLink_instantiate(ptr);
    if (!mode) {
      ERROR_OUT_OF_MEMORY();
      ByteStream_destroy(&modeBuffer);
      return false;
    }
    // serialize it into the target modes buffer
    if (!Mode_serialize(mode, &modeBuffer, 0)) {
      ByteStream_destroy(&modeBuffer);
      return false;
    }
    // just uninstansiate the mode after serializing
    ModeLink_uninstantiate(ptr);
    // next mode
    ptr = ModeLink_next(ptr);
    // now write this mode into a storage slot (skip first slot, that's header)
    if (!Storage_write(++i, &modeBuffer)) {
      ByteStream_destroy(&modeBuffer);
      return false;
    }
    ByteStream_destroy(&modeBuffer);
  }
  // reinstanstiate the current mode
  if (Modes_m_pCurModeLink && !ModeLink_instantiate(Modes_m_pCurModeLink)) {
    return false;
  }
  DEBUG_LOGF("Serialized num modes: %u", Modes_m_numModes);
  return true;
}

bool Modes_loadStorage()
{
  // NOTE: We could call loadHeader here but then we wouldn't have the headerBuffer
  //       and in turn wouldn't be able to unserialize the number of modes. The number
  //       of modes is a weird case, it's technically part of the mode list not the
  //       header but it is stored in the same storage slot as the header
  ByteStream headerBuffer;
  ByteStream_make(&headerBuffer);
  // only read storage if the modebuffer isn't filled
  if (!Storage_read(0, &headerBuffer) || !headerBuffer.m_pData->size) {
    DEBUG_LOG("Empty buffer read from storage");
    // this kinda sucks whatever they had loaded is gone
    ByteStream_destroy(&headerBuffer);
    return false;
  }
  // this erases what is stored before we know whether there is data
  // but it's the easiest way to just re-load new data from storage
  Modes_clearModes();
  // read the header and load the data
  if (!Modes_unserializeSaveHeader(&headerBuffer)) {
    ByteStream_destroy(&headerBuffer);
    return false;
  }
  // unserialize the number of modes next
  uint8_t numModes = 0;
  if (!ByteStream_unserialize8(&headerBuffer, &numModes)) {
    ByteStream_destroy(&headerBuffer);
    return false;
  }
  ByteStream_destroy(&headerBuffer);
  if (!numModes) {
    DEBUG_LOG("Did not find any modes");
    // this kinda sucks whatever they had loaded is gone
    return false;
  }
  // iterate each mode and read it out of it's storage slot then add it
  for (uint8_t i = 0; i < numModes; ++i) {
    ByteStream modeBuffer;
    ByteStream_make(&modeBuffer);
    ByteStream_init(&modeBuffer, MAX_MODE_SIZE);
    // read each mode from a storage slot and load it
    if (!Storage_read(i + 1, &modeBuffer) || !Modes_addSerializedMode(&modeBuffer)) {
      ByteStream_destroy(&modeBuffer);
      return false;
    }
    ByteStream_destroy(&modeBuffer);
  }
  if (Modes_oneClickModeEnabled()) {
    // set the current mode to the startup mode
    Modes_switchToStartupMode();
  }
  return true;
}

bool Modes_serializeSaveHeader(ByteStream *saveBuffer)
{
  // serialize the engine version into the modes buffer
  if (!VortexEngine_serializeVersion(saveBuffer)) {
    return false;
  }
  if (!ByteStream_serialize8(saveBuffer, Modes_m_globalFlags)) {
    return false;
  }
  // serialize the global brightness
  if (!ByteStream_serialize8(saveBuffer, (uint8_t)Leds_getBrightness())) {
    return false;
  }
  // the number of modes
  if (!ByteStream_serialize8(saveBuffer, Modes_m_numModes)) {
    return false;
  }
  // only on the duo just save some extra stuff to the header slot
#ifdef VORTEX_EMBEDDED
  // Duo also saves the build number to the save header so the chromalink can
  // read it out, other devices just have the version hardcoded into their
  // editor connection hello message. Don't do this in VortexLib because
  // it will alter the savefile format and break compatibility
  if (!ByteStream_serialize8(saveBuffer, (uint8_t)VORTEX_BUILD_NUMBER)) {
    return false;
  }
#endif
  DEBUG_LOGF("Serialized all modes, uncompressed size: %u", saveBuffer.m_pData->size());
  return true;
}

bool Modes_unserializeSaveHeader(ByteStream *saveHeader)
{
  if (!ByteStream_decompress(saveHeader)) {
    // failed to decompress?
    return false;
  }
  // reset the unserializer index before unserializing anything
  ByteStream_resetUnserializer(saveHeader);
  uint8_t major = 0;
  uint8_t minor = 0;
  // unserialize the vortex version
  if (!ByteStream_unserialize8(saveHeader, &major)) {
    return false;
  }
  if (!ByteStream_unserialize8(saveHeader, &minor)) {
    return false;
  }
  // check the version for incompatibility
  if (!VortexEngine_checkVersion(major, minor)) {
    // incompatible version
    ERROR_LOGF("Incompatible savefile version: %u.%u", major, minor);
    return false;
  }
  uint8_t flags8 = 0;
  if (!ByteStream_unserialize8(saveHeader, &flags8)) {
    return false;
  }
  Modes_m_globalFlags = flags8;
  // unserialize the global brightness
  uint8_t brightness = 0;
  if (!ByteStream_unserialize8(saveHeader, &brightness)) {
    return false;
  }
  if (brightness) {
    Leds_setBrightness(brightness);
  }
  return true;
}

// Save all of the modes to a serial buffer
bool Modes_serialize(ByteStream *modesBuffer)
{
  // serialize the number of modes
  if (!ByteStream_serialize8(modesBuffer, Modes_m_numModes)) {
    return false;
  }
  // make sure the current mode is saved in case it has changed somehow
  Modes_saveCurMode();
  // uninstantiate cur mode so we have stack space to serialize
  if (Modes_m_pCurModeLink) {
    ModeLink_uninstantiate(Modes_m_pCurModeLink);
  }
  ModeLink *ptr = Modes_m_storedModes;
  while (ptr) {
    // instantiate the mode temporarily
    Mode *mode = ModeLink_instantiate(ptr);
    if (!mode) {
      ERROR_OUT_OF_MEMORY();
      return false;
    }
    // serialize it into the target modes buffer
    if (!Mode_serialize(mode, modesBuffer, 0)) {
      return false;
    }
    // just uninstansiate the mode after serializing
    ModeLink_uninstantiate(ptr);
    // next mode
    ptr = ModeLink_next(ptr);
  }
  // reinstanstiate the current mode
  if (Modes_m_pCurModeLink && !ModeLink_instantiate(Modes_m_pCurModeLink)) {
    return false;
  }
  DEBUG_LOGF("Serialized num modes: %u", Modes_m_numModes);
  return true;
}

// load all modes from a serial buffer
bool Modes_unserialize(ByteStream *modesBuffer)
{
  DEBUG_LOG("Loading modes...");
  // this is good on memory, but it erases what they have stored before we
  // know whether there is something actually saved in the serial buffer
  Modes_clearModes();
  // unserialize the number of modes next
  uint8_t numModes = 0;
  if (!ByteStream_unserialize8(modesBuffer, &numModes)) {
    return false;
  }
  if (!numModes) {
    DEBUG_LOG("Did not find any modes");
    // this kinda sucks whatever they had loaded is gone
    return false;
  }
  // foreach expected mode
  for (uint8_t i = 0; i < numModes; ++i) {
    // just copy the serialized mode into the internal storage because
    // we store the modes in a serialized manner so that they are smaller
    // then we unpack them when we instantiate the mode
    if (!Modes_addSerializedMode(modesBuffer)) {
      DEBUG_LOGF("Failed to add mode %u after unserialization", i);
      // clear work so far?
      Modes_clearModes();
      return false;
    }
  }
  DEBUG_LOGF("Loaded %u modes from storage (%u bytes)", numModes, modesBuffer.m_pData->size());
  return (Modes_m_numModes == numModes);
}

bool Modes_setDefaults()
{
  Modes_clearModes();
  // add each default mode with each of the given colors
  for (uint8_t i = 0; i < MAX_MODES; ++i) {
    Mode defMode;
    Mode_initDefault(&defMode, &defaultModes[i]);
    if (!Modes_addMode(&defMode)) {
      ERROR_LOGF("Failed to add default mode %u", i);
      Mode_destroy(&defMode);
      return false;
    }
    Mode_destroy(&defMode);
  }
  return true;
}

bool Modes_addSerializedMode(ByteStream *serializedMode)
{
#if MAX_MODES != 0
  if (Modes_m_numModes >= MAX_MODES) {
    return false;
  }
#endif
  // we must unserialize then re-serialize here because the
  // input argument may contain other patterns in the buffer
  // so we cannot just append the input arg to Modes_m_storedModes
  Mode tmpMode;
  Mode_init(&tmpMode);
  if (!Mode_unserialize(&tmpMode, serializedMode)) {
    Mode_destroy(&tmpMode);
    return false;
  }
  // initialize the mode because a pattern could theoretically serialize
  // differently after it has initialized
  Mode_initPatterns(&tmpMode);
  bool rv = Modes_addMode(&tmpMode);
  Mode_destroy(&tmpMode);
  return rv;
}

bool Modes_addModeFromBuffer(ByteStream *serializedMode)
{
#if MAX_MODES != 0
  if (Modes_m_numModes >= MAX_MODES) {
    return false;
  }
#endif
  if (!Modes_m_storedModes) {
    Modes_m_storedModes = (ModeLink *)MemPool_alloc(&s_modeLinkPool);
    if (!Modes_m_storedModes) {
      ERROR_OUT_OF_MEMORY();
      return false;
    }
    ModeLink_initBuf(Modes_m_storedModes, serializedMode, false);
  } else {
    if (!ModeLink_appendBuf(Modes_m_storedModes, serializedMode)) {
      ERROR_OUT_OF_MEMORY();
      return false;
    }
  }
  // increment mode counter
  Modes_m_numModes++;
  return true;
}

// shift the current mode to a different position relative to current position
// negative values for up, positive values for down, 0 for no move
bool Modes_shiftCurMode(int32_t offset)
{
  uint32_t newPos = (uint32_t)((int32_t)Modes_m_curMode + offset);
  if (newPos >= Modes_m_numModes) {
    return false;
  }
  if (newPos == Modes_m_curMode) {
    return true;
  }
  // get the target mode at the position of the shift
  ModeLink *target = Modes_getModeLink(newPos);
  if (!target) {
    // invalid new position?
    return false;
  }
  // special case for moving first in list forward
  if (!Modes_m_curMode && offset > 0) {
    // update main list ptr
    Modes_m_storedModes = ModeLink_next(Modes_m_storedModes);
  }
  // unlink the current link
  ModeLink_unlinkSelf(Modes_m_pCurModeLink);
  // update the current position to reflect our new pos
  Modes_m_curMode = newPos;
  // then re-link the mode at the new spot
  if (offset < 0) {
    // link the link before our target link
    ModeLink_linkBefore(target, Modes_m_pCurModeLink);
    // special case for moving into first in list
    if (!Modes_m_curMode) {
      Modes_m_storedModes = Modes_m_pCurModeLink;
    }
  } else {
    // link the link after our target link
    ModeLink_linkAfter(target, Modes_m_pCurModeLink);
  }
  return true;
}

bool Modes_addMode(PatternID id, RGBColor c1, RGBColor c2, RGBColor c3,
    RGBColor c4, RGBColor c5, RGBColor c6, RGBColor c7, RGBColor c8)
{
  Colorset set;
  Colorset_makeRGB(&set, c1, c2, c3, c4, c5, c6, c7, c8);
  return Modes_addMode(id, NULL, &set);
}

bool Modes_addMode(PatternID id, const PatternArgs *args, const Colorset *set)
{
  // max modes
#if MAX_MODES != 0
  if (Modes_m_numModes >= MAX_MODES) {
    return false;
  }
#endif
  if (id >= PATTERN_COUNT) {
    return false;
  }
  Mode tmpMode;
  Mode_initPatternArgsSet(&tmpMode, id, args, set);
  // must init the mode so that it can be serialized
  Mode_initPatterns(&tmpMode);
  // not a very good way to do this but it ensures the mode is
  // added in the same way
  bool rv = Modes_addMode(&tmpMode);
  Mode_destroy(&tmpMode);
  return rv;
}

bool Modes_addMode(const Mode *mode)
{
#if MAX_MODES != 0
  // max modes
  if (Modes_m_numModes >= MAX_MODES) {
    return false;
  }
#endif
  if (!Modes_m_storedModes) {
    Modes_m_storedModes = (ModeLink *)MemPool_alloc(&s_modeLinkPool);
    if (!Modes_m_storedModes) {
      ERROR_OUT_OF_MEMORY();
      return false;
    }
    ModeLink_init(Modes_m_storedModes, mode, false);
  } else {
    if (!ModeLink_appendMode(Modes_m_storedModes, mode)) {
      ERROR_OUT_OF_MEMORY();
      return false;
    }
  }
  Modes_m_numModes++;
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
  // utilize copy operator
  Mode_assign(pCur, mode);
  // immediately save this mode to the internal mode storage
  if (!Modes_saveCurMode()) {
    return false;
  }
  // initialize the new mode
  return Modes_initCurMode(false);
}

// set the current active mode by index
Mode *Modes_setCurMode(uint8_t index)
{
  if (!Modes_m_numModes) {
    return NULL;
  }
  // clear the LEDs when switching modes
  Leds_clearAll();
  // if we have a current mode open, close it
  if (Modes_m_pCurModeLink) {
    ModeLink_uninstantiate(Modes_m_pCurModeLink);
  }
  int8_t newModeIdx = index % Modes_m_numModes;
  // lookup the new mode link
  ModeLink *newCurLink = Modes_getModeLink(newModeIdx);
  if (!newCurLink) {
    // what
    return NULL;
  }
  // instantiate the new mode so it is ready, also so it can be checked for PATTERN_NONE
  Mode *newCur = ModeLink_instantiate(newCurLink);
  if (!newCur) {
    ERROR_OUT_OF_MEMORY();
    return NULL;
  }
  // update to the new mode
  Modes_m_curMode = newModeIdx;
  Modes_m_pCurModeLink = newCurLink;
  // record the current time as the last switch time
  Modes_m_lastSwitchTime = Time_getCurtime();
  // update the global startup mode to be this mode, if we turn off
  // and turn back on into one click mode it will select this one
  Modes_setStartupMode(newModeIdx);
  // log the change
  DEBUG_LOGF("Switch to Mode: %u / %u (pattern id: %u)",
    Modes_m_curMode, Modes_m_numModes - 1, Pattern_getPatternID(newCur));
  // return the new current mode
  return newCur;
}

// the current mode
Mode *Modes_curMode()
{
  // empty mode list
  if (!Modes_m_numModes) {
    return NULL;
  }
  if (!Modes_m_pCurModeLink) {
    if (!Modes_initCurMode(false)) {
      ERROR_LOG("Failed to initialize current mode");
      return NULL;
    }
  }
  // get current mode, instantiate it if necessary
  return ModeLink_instantiate(Modes_m_pCurModeLink);
}

// iterate to next mode and return it
Mode *Modes_nextMode()
{
  if (!Modes_m_numModes) {
    return NULL;
  }
  // iterate the cur mode forward
  return Modes_setCurMode(Modes_m_curMode + 1);
}

// iterate to previous mode and return it
Mode *Modes_previousMode()
{
  if (!Modes_m_numModes) {
    return NULL;
  }
  // iterate the cur mode backwards
  if (!Modes_m_curMode) {
    return Modes_setCurMode(Modes_numModes() - 1);
  }
  return Modes_setCurMode(Modes_m_curMode - 1);
}

Mode *Modes_nextModeSkipEmpty()
{
  do {
    if (Modes_setCurMode(Modes_m_curMode + 1) && !Mode_isEmpty(Modes_curMode())) {
      break;
    }
  } while (Modes_m_curMode != 0);
  return Modes_curMode();
}

void Modes_deleteCurMode()
{
  if (!Modes_m_numModes || !Modes_m_pCurModeLink) {
    return;
  }
  // unlink the current mode so it can be deleted and
  // update the current mode link accordingly
  ModeLink *newCur = ModeLink_unlinkSelf(Modes_m_pCurModeLink);
  ModeLink_destroy(Modes_m_pCurModeLink);
  MemPool_free(&s_modeLinkPool, Modes_m_pCurModeLink);
  Modes_m_pCurModeLink = newCur;
  if (Modes_m_curMode) {
    Modes_m_curMode--;
  } else {
    Modes_m_storedModes = Modes_m_pCurModeLink;
  }
  Modes_m_numModes--;
  if (!Modes_m_numModes) {
    Modes_m_storedModes = NULL;
  }
}

void Modes_clearModes()
{
  if (!Modes_m_numModes || !Modes_m_storedModes) {
    return;
  }
  // delete the first node and it will delete the entire chain
  ModeLink_destroy(Modes_m_storedModes);
  MemPool_free(&s_modeLinkPool, Modes_m_storedModes);
  Modes_m_pCurModeLink = NULL;
  Modes_m_storedModes = NULL;
  Modes_m_numModes = 0;
  // might as well clear the leds
  Leds_clearAll();
}

void Modes_setStartupMode(uint8_t index)
{
  // zero out the upper nibble to clear it
  Modes_m_globalFlags &= 0x0F;
  // OR in the index value shifted into the upper nibble
  Modes_m_globalFlags |= (index << 4) & 0xF0;
}

uint8_t Modes_startupMode()
{
  // return the upper nibble of the global flags
  return (Modes_m_globalFlags & 0xF0) >> 4;
}

Mode *Modes_switchToStartupMode()
{
  return Modes_setCurMode(Modes_startupMode());
}

bool Modes_setFlag(uint8_t flag, bool enable, bool save)
{
  // then actually if it's enabled ensure the upper nibble is set
  if (enable) {
    // set the cur mode index as the upper nibble
    Modes_m_globalFlags |= flag;
  } else {
    Modes_m_globalFlags &= ~flag;
  }
  DEBUG_LOGF("Toggled instant on/off to %s", enable ? "on" : "off");
  if (!save) {
    // if save is not requested then just return here
    return true;
  }
  // otherwise need to update the global flags field of the save header in storage
  ByteStream headerBuffer;
  ByteStream_make(&headerBuffer);
  // read out the storage header so we can update the flag field
  if (!Storage_read(0, &headerBuffer) || !headerBuffer.m_pData->size) {
    ByteStream_destroy(&headerBuffer);
    // if cannot read the save header then just save it normally
    return Modes_saveHeader();
  }
  // layout of the save header, this struct is never really used anywhere else
  // except here the actual layout of the save header is dictated by
  // Modes_saveHeader() and Modes_serializeSaveHeader()
  struct SaveHeader {
    uint8_t vMajor;
    uint8_t vMinor;
    uint8_t globalFlags;
    uint8_t brightness;
    uint8_t numModes;
  };
  // data cannot be NULL since size is non zero
  SaveHeader *pHeader = (SaveHeader *)headerBuffer.m_pData->buf;
  // update the global flags field in the header
  pHeader->globalFlags = Modes_m_globalFlags;
  // need to force the crc to recalc since we modified the data, just mark the
  // CRC as dirty and Storage_write() will re-calculate the CRC if it's dirty
  ByteStream_setCRCDirty(&headerBuffer);
  // write the save header back to storage
  bool rv = Storage_write(0, &headerBuffer);
  ByteStream_destroy(&headerBuffer);
  return rv;
}

#ifdef VORTEX_LIB
#include "Patterns/PatternBuilder.h"
// get the maximum size a mode can occupy
uint32_t Modes_maxModeSize()
{
  Mode maxMode;
  Mode_init(&maxMode);
  uint8_t x = 0;
  for (LedPos p = LED_FIRST; p < LED_COUNT; ++p) {
    // blend takes up 8 params
    PatternArgs maxArgs;
    PatternArgs_make8(&maxArgs, (uint8_t)p + 0xd2, (uint8_t)p + 0xd3, (uint8_t)p + 0xd4, (uint8_t)p + 0xd5,
      (uint8_t)p + 0xd6, (uint8_t)p + 0xd7, (uint8_t)p + 0xd8, (uint8_t)p + 0xd9);
    //PatternArgs typicalArgs = PatternBuilder_getDefaultArgs(PATTERN_BLEND);
    Colorset maxSet;
    Colorset_make(&maxSet);
    for (uint8_t i = 0; i < MAX_COLOR_SLOTS; ++i) {
      // different color in each slot
      RGBColor col;
      col.red = ++x;
      col.green = ++x;
      col.blue = ++x;
      Colorset_addColor(&maxSet, col);
    }
    Mode_setPattern(&maxMode, PATTERN_BLEND, p, &maxArgs, &maxSet);
  }
  ByteStream stream;
  ByteStream_make(&stream);
  Mode_saveToBuffer(&maxMode, &stream, 0);
  uint32_t size = stream.m_pData->size;
  ByteStream_destroy(&stream);
  Mode_destroy(&maxMode);
  return size;
}

// get the maximum size a savefile can occupy
uint32_t Modes_maxSaveSize()
{
#if MAX_MODES == 0
  // unbounded
  return 0;
#else
  ByteStream backupModes;
  ByteStream_make(&backupModes);
  Modes_saveToBuffer(&backupModes);
  for (uint32_t i = 0; i < MAX_MODES; ++i) {
    Mode maxMode;
    Mode_init(&maxMode);
    for (LedPos p = LED_FIRST; p < LED_COUNT; ++p) {
      // blend takes up 8 params
      PatternArgs maxArgs;
      PatternArgs_make8(&maxArgs, (uint8_t)p + 2, (uint8_t)p + 3, (uint8_t)p + 4, (uint8_t)p + 5,
        (uint8_t)p + 6, (uint8_t)p + 7, (uint8_t)p + 8, (uint8_t)p + 9);
      Colorset maxSet;
      Colorset_make(&maxSet);
      for (uint32_t i = 0; i < MAX_COLOR_SLOTS; ++i) {
        // different color in each slot
        Colorset_addColor(&maxSet, RGBColor_makeRGB((uint8_t)p + (i * 3), (uint8_t)p + (i * 3) + 1, (uint8_t)p + (i * 3) + 2));
      }
      Mode_setPattern(&maxMode, PATTERN_BLEND, p, &maxArgs, &maxSet);
    }
    Modes_addMode(&maxMode);
    Mode_destroy(&maxMode);
  }
  // grab the size of the new buffer
  ByteStream stream;
  ByteStream_make(&stream);
  Modes_saveToBuffer(&stream);
  uint32_t size = stream.m_pData->size;
  ByteStream_destroy(&stream);
  // restore backed up modes
  Modes_loadFromBuffer(&backupModes);
  ByteStream_destroy(&backupModes);
  return size;
#endif
}

uint8_t Modes_globalFlags()
{
  return Modes_m_globalFlags;
}
#endif

// fetch a link from the chain by index
ModeLink *Modes_getModeLink(uint32_t index)
{
  if (index >= Modes_m_numModes) {
    return NULL;
  }
  ModeLink *ptr = Modes_m_storedModes;
  while (index > 0 && ptr) {
    ptr = ModeLink_next(ptr);
    index--;
  }
  return ptr;
}

Mode *Modes_initCurMode(bool force)
{
  if (!Modes_m_numModes) {
    return NULL;
  }
  // cleanup the current mode link
  if (Modes_m_pCurModeLink) {
    ModeLink_uninstantiate(Modes_m_pCurModeLink);
  }
  // update the current mode link based on the curmode index
  Modes_m_pCurModeLink = Modes_getModeLink(Modes_m_curMode);
  if (!Modes_m_pCurModeLink) {
    return NULL;
  }
  if (force) {
    ModeLink_uninstantiate(Modes_m_pCurModeLink);
  }
  return ModeLink_instantiate(Modes_m_pCurModeLink);
}

bool Modes_saveCurMode()
{
  if (!Modes_m_pCurModeLink) {
    // if there's no loaded mode currently then there's nothing
    // to save so there's no error
    return false;
  }
  // force the current mode to save back to serial to catch changes
  return ModeLink_save(Modes_m_pCurModeLink);
}

void ModeLink_init(ModeLink *self, const Mode *src, bool inst)
{
  self->m_pInstantiatedMode = NULL;
  ByteStream_make(&self->m_storedMode);
  self->m_next = NULL;
  self->m_prev = NULL;
  if (src) {
    ModeLink_initMode(self, src);
  }
  if (src && inst) {
    ModeLink_instantiate(self);
  }
}

void ModeLink_initBuf(ModeLink *self, const ByteStream *src, bool inst)
{
  self->m_pInstantiatedMode = NULL;
  ByteStream_make(&self->m_storedMode);
  ByteStream_set(&self->m_storedMode, src);
  self->m_next = NULL;
  self->m_prev = NULL;
  if (src->m_pData->size && inst) {
    ModeLink_instantiate(self);
  }
}

void ModeLink_destroy(ModeLink *self)
{
  ModeLink *next = self->m_next;
  if (next) {
    ModeLink_destroy(next);
  }
  MemPool_free(&s_modeLinkPool, next);
  if (self->m_pInstantiatedMode) {
    Mode_destroy(self->m_pInstantiatedMode);
    MemPool_free(&s_modePool, self->m_pInstantiatedMode);
  }
  ByteStream_destroy(&self->m_storedMode);
}

bool ModeLink_initMode(ModeLink *self, const Mode *mode)
{
  if (!mode) {
    return false;
  }
  ByteStream_clear(&self->m_storedMode);
  // serialize the mode so it can be instantiated anytime
  if (!Mode_saveToBuffer(mode, &self->m_storedMode, 0)) {
    return false;
  }
  return true;
}

bool ModeLink_appendMode(ModeLink *self, const Mode *next)
{
  if (!next) {
    return false;
  }
  // if not end of chain, recurse on next link
  if (self->m_next) {
    return ModeLink_appendMode(self->m_next, next);
  }
  self->m_next = (ModeLink *)MemPool_alloc(&s_modeLinkPool);
  if (!self->m_next) {
    ERROR_OUT_OF_MEMORY();
    return false;
  }
  ModeLink_init(self->m_next, next, false);
  self->m_next->m_prev = self;
  return true;
}

bool ModeLink_appendBuf(ModeLink *self, const ByteStream *next)
{
  if (!next->m_pData->size) {
    return false;
  }
  // if not end of chain, recurse on next link
  if (self->m_next) {
    return ModeLink_appendBuf(self->m_next, next);
  }
  self->m_next = (ModeLink *)MemPool_alloc(&s_modeLinkPool);
  if (!self->m_next) {
    ERROR_OUT_OF_MEMORY();
    return false;
  }
  ModeLink_initBuf(self->m_next, next, false);
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

ModeLink *ModeLink_next(ModeLink *self)
{
  return self->m_next;
}

ModeLink *ModeLink_unlinkSelf(ModeLink *self)
{
  // unlink this node from the chain
  if (self->m_prev) {
    self->m_prev->m_next = self->m_next;
  }
  if (self->m_next) {
    self->m_next->m_prev = self->m_prev;
  }
  // grab the new link that will take this place
  ModeLink *newLink = self->m_prev ? self->m_prev : self->m_next;
  // clear the links of this node
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
  Mode *newMode = (Mode *)MemPool_alloc(&s_modePool);
  if (!newMode) {
    ERROR_OUT_OF_MEMORY();
    return NULL;
  }
  Mode_init(newMode);
  ByteStream_resetUnserializer(&self->m_storedMode);
  if (!Mode_loadFromBuffer(newMode, &self->m_storedMode)) {
    Mode_destroy(newMode);
    MemPool_free(&s_modePool, newMode);
    return NULL;
  }
  self->m_pInstantiatedMode = newMode;
  return self->m_pInstantiatedMode;
}

void ModeLink_uninstantiate(ModeLink *self)
{
  if (self->m_pInstantiatedMode) {
    Mode_destroy(self->m_pInstantiatedMode);
    MemPool_free(&s_modePool, self->m_pInstantiatedMode);
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

#if MODES_TEST == 1
#include <assert.h>
#include <stdio.h>

#include "../Patterns/PatternBuilder.h"

void Modes_test()
{
  INFO_LOG("== Beginning Modes Test ==\n");

  RGBColor col = RGB_RED;
  assert(!Modes_addMode(PATTERN_COUNT, col));
  for (PatternID pat = PATTERN_FIRST; pat < PATTERN_COUNT; ++pat) {
    assert(Modes_addMode(pat, col));
  }
  assert(Modes_numModes() == PATTERN_COUNT);
  Modes_clearModes();
  assert(Modes_numModes() == 0);

  Colorset set(RGB_RED, RGB_GREEN, RGB_BLUE);
  assert(!Modes_addMode(PATTERN_COUNT, NULL, &set));
  for (PatternID pat = PATTERN_FIRST; pat < PATTERN_COUNT; ++pat) {
    assert(Modes_addMode(pat, NULL, &set));
  }
  assert(Modes_numModes() == PATTERN_COUNT);
  Modes_clearModes();
  assert(Modes_numModes() == 0);

  // add a new mode in various different ways
  assert(!Modes_addMode(PATTERN_COUNT, col));
  Colorset set2(RGB_RED, RGB_GREEN, RGB_BLUE);
  for (PatternID pat = PATTERN_FIRST; pat < PATTERN_COUNT; ++pat) {
    Mode tmpMode(pat, NULL, &set2);
    assert(Modes_addMode(&tmpMode));
  }
  assert(Modes_numModes() == PATTERN_COUNT);
  Modes_clearModes();
  assert(Modes_numModes() == 0);

  INFO_LOG("Modes_addMode(): success\n");

  ByteStream modebuf;
  ByteStream modesave;
  PatternArgs args = PatternBuilder_getDefaultArgs(PATTERN_BASIC);
  Mode tmpMode(PATTERN_BASIC, &args, &set);
  tmpMode.Mode_serialize(&modebuf);
  tmpMode.saveToBuffer(modesave);
  assert(Modes_addSerializedMode(&modebuf));
  assert(Modes_numModes() == 1);
  assert(Modes_addModeFromBuffer(&modesave));
  assert(Modes_numModes() == 2);
  assert(Modes_getModeLink(0) != NULL);
  Mode *mode1 = Modes_getModeLink(0)->instantiate();
  assert(mode1 != NULL);
  Mode *mode2 = Modes_getModeLink(1)->instantiate();
  assert(mode2 != NULL);
  assert(Mode_equals(mode1, mode2));

  INFO_LOG("Modes_addSerializedMode(): success\n");

  Colorset newset(RGB_BLUE, RGB_RED, RGB_GREEN);
  assert(Modes_updateCurMode(PATTERN_HYPERSTROBE, NULL));
  assert(Modes_getModeLink(0)->mode()->getPatternID() == PATTERN_HYPERSTROBE);
  assert(Modes_setCurMode(1));
  // update the current mode to match the given mode
  assert(Modes_updateCurMode(PATTERN_DOPS, &newset));
  assert(Modes_getModeLink(1)->mode()->getPatternID() == PATTERN_DOPS);
  Mode newTmp(PATTERN_BLEND, PatternBuilder_getDefaultArgs(PATTERN_BLEND),
    Colorset(RGB_YELLOW, RGB_ORANGE, RGB_CYAN, RGB_BLUE, RGB_WHITE, RGB_RED));

  INFO_LOG("Modes_updateCurMode(): success\n");

  assert(Modes_shiftCurMode(-1));
  assert(Modes_m_curMode == 0);
  assert(Modes_getModeLink(0)->instantiate()->getPatternID() == PATTERN_DOPS);
  assert(Modes_getModeLink(1)->instantiate()->getPatternID() == PATTERN_HYPERSTROBE);
  assert(Modes_shiftCurMode(0));
  assert(Modes_m_curMode == 0);
  assert(Modes_getModeLink(0)->instantiate()->getPatternID() == PATTERN_DOPS);
  assert(Modes_getModeLink(1)->instantiate()->getPatternID() == PATTERN_HYPERSTROBE);
  assert(Modes_shiftCurMode(1));
  assert(Modes_m_curMode == 1);
  assert(Modes_getModeLink(0)->instantiate()->getPatternID() == PATTERN_HYPERSTROBE);
  assert(Modes_getModeLink(1)->instantiate()->getPatternID() == PATTERN_DOPS);

  INFO_LOG("Modes_shiftCurMode(): success\n");

  Modes_deleteCurMode();
  assert(Modes_m_numModes == 1);
  assert(Modes_m_curMode == 0);
  Modes_deleteCurMode();
  assert(Modes_m_numModes == 0);
  assert(Modes_m_curMode == 0);

  INFO_LOG("Modes_deleteCurMode(): success\n");

  INFO_LOG("== Success Running Modes Test ==\n");
}
#endif
