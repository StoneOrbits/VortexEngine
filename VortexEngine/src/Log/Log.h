#ifndef LOG_H
#define LOG_H

#include <stdarg.h>

#include "../VortexConfig.h"

#if LOGGING_LEVEL > 0
#define INFO_LOG(msg) InfoMsg(msg)
#define INFO_LOGF(msg, ...) InfoMsg(msg, __VA_ARGS__)
// infos are in final builds and they are meant to be standalone messages
void InfoMsg(const char *msg, ...);
#else
#define INFO_LOG(msg)
#define INFO_LOGF(msg, ...)
#endif

#if LOGGING_LEVEL > 1
// in test build errors are just debug messages
#define ERROR_LOG(msg) DebugMsg(__FILE__, __FUNCTION__, __LINE__, msg)
#define ERROR_LOGF(msg, ...) DebugMsg(__FILE__, __FUNCTION__, __LINE__, msg, __VA_ARGS__)
// errors are in final builds so they only have the function name
void ErrorMsg(const char *func, const char *msg, ...);
#else
#define ERROR_LOG(msg)
#define ERROR_LOGF(msg, ...)
#endif

#if LOGGING_LEVEL > 2
// arduino compiler won't allow for ellipsis macro that's passed no args...
#define DEBUG_LOG(msg) DebugMsg(__FILE__, __FUNCTION__, __LINE__, msg)
#define DEBUG_LOGF(msg, ...) DebugMsg(__FILE__, __FUNCTION__, __LINE__, msg, __VA_ARGS__)
// debug messages are only in debug builds so they have full file + func + line info
void DebugMsg(const char *file, const char *func, int line, const char *msg, ...);
#else
#define DEBUG_LOG(msg)
#define DEBUG_LOGF(msg, ...)
#endif

// report OOM
#define ERROR_OUT_OF_MEMORY() ERROR_LOG("Out of memory")

#if SAVE_TEMPLATE == 1
// A message with indentation used for json output
void IndentMsg(int level, const char *msg, ...);
#endif

#endif
