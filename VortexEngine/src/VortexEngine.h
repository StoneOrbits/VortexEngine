#ifndef VORTEX_ENGINE_H
#define VORTEX_ENGINE_H

#include <inttypes.h>
#include <stdbool.h>

#include "VortexConfig.h"

typedef struct ByteStream ByteStream;
typedef struct Mode Mode;

// VortexEngine is a stateless namespace of free functions, the state
// lives in VortexEngine.cpp

// setup and initialization function
bool VortexEngine_init(void);
// cleanup
void VortexEngine_cleanup(void);

// tick function for each loop
void VortexEngine_tick(void);

// run the main logic of the engine
void VortexEngine_runMainLogic(void);

// serialize the version number to a byte stream
bool VortexEngine_serializeVersion(ByteStream *stream);
bool VortexEngine_checkVersion(uint8_t major, uint8_t minor);

// get the current mode
Mode *VortexEngine_curMode(void);

#ifdef VORTEX_LIB
// the total available storage space
uint32_t VortexEngine_totalStorageSpace(void);
// the size of the savefile
uint32_t VortexEngine_savefileSize(void);
// whether sleeping or not
bool VortexEngine_isSleeping(void);
#endif

// enter/leave sleep mode
void VortexEngine_enterSleep(bool save);
void VortexEngine_wakeup(bool reset);

// toggle force sleep from working
void VortexEngine_toggleForceSleep(bool enabled);
// toggle auto cycle enabled
void VortexEngine_setAutoCycle(bool enabled);

#ifdef VORTEX_EMBEDDED
// clear output pins
void VortexEngine_clearOutputPins(void);
// enable/disable the mosfet
void VortexEngine_enableMOSFET(bool enabled);
#endif

#if COMPRESSION_TEST == 1
void VortexEngine_compressionTest(void);
#endif
#if SERIALIZATION_TEST == 1
void VortexEngine_serializationTest(void);
#endif
#if TIMER_TEST == 1
void VortexEngine_timerTest(void);
#endif

#endif
