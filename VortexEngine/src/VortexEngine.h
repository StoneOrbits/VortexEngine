#ifndef VORTEX_ENGINE_H
#define VORTEX_ENGINE_H

#include <inttypes.h>
#include <stdbool.h>
#include "VortexConfig.h"

typedef struct ByteStream ByteStream;
typedef struct Mode Mode;

bool VortexEngine_init(void);
void VortexEngine_cleanup(void);
void VortexEngine_tick(void);
void VortexEngine_runMainLogic(void);
bool VortexEngine_serializeVersion(ByteStream *stream);
bool VortexEngine_checkVersion(uint8_t major, uint8_t minor);
Mode *VortexEngine_curMode(void);

#ifdef VORTEX_LIB
uint32_t VortexEngine_totalStorageSpace(void);
uint32_t VortexEngine_savefileSize(void);
bool VortexEngine_isSleeping(void);
#endif

void VortexEngine_enterSleep(bool save);
void VortexEngine_wakeup(bool reset);
void VortexEngine_toggleForceSleep(bool enabled);
void VortexEngine_setAutoCycle(bool enabled);
void VortexEngine_compressionTest(void);
void VortexEngine_serializationTest(void);
void VortexEngine_timerTest(void);

#ifdef VORTEX_EMBEDDED
void VortexEngine_clearOutputPins(void);
void VortexEngine_enableMOSFET(bool enabled);
#endif

#endif
