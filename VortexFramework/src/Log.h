#ifndef LOG_H
#define LOG_H

#include <stdarg.h>

#ifdef TEST_FRAMEWORK
#define INFO_LOG(msg) InfoMsg(msg)
#define INFO_LOGF(msg, ...) InfoMsg(msg, __VA_ARGS__)
// arduino compiler won't allow for ellipsis macro that's passed no args...
#define DEBUG_LOG(msg) DebugMsg(__FILE__, __FUNCTION__, __LINE__, msg)
#define DEBUG_LOGF(msg, ...) DebugMsg(__FILE__, __FUNCTION__, __LINE__, msg, __VA_ARGS__)
// in test build errors are just debug messages
#define ERROR_LOG(msg) DebugMsg(__FILE__, __FUNCTION__, __LINE__, msg)
#define ERROR_LOGF(msg, ...) DebugMsg(__FILE__, __FUNCTION__, __LINE__, msg, __VA_ARGS__)
#else
#define INFO_LOG(msg) InfoMsg(msg)
#define INFO_LOGF(msg, ...) InfoMsg(msg, __VA_ARGS__)
// debug does nothing in final build
// TODO: fix this for final build because debug logging should be disabled
//#define DEBUG_LOG(msg)
//#define DEBUG_LOGF(msg, ...)
#define DEBUG_LOG(msg) DebugMsg(__FILE__, __FUNCTION__, __LINE__, msg)
#define DEBUG_LOGF(msg, ...) DebugMsg(__FILE__, __FUNCTION__, __LINE__, msg, __VA_ARGS__)
// but error messages still write to serial, but with just the function name
#define ERROR_LOG(msg) DebugMsg(__FILE__, __FUNCTION__, __LINE__, msg)
#define ERROR_LOGF(msg, ...) DebugMsg(__FILE__, __FUNCTION__, __LINE__, msg, __VA_ARGS__)
#endif

// report OOM
#define ERROR_OUT_OF_MEMORY() ERROR_LOG("Out of memory")

void checkSerial();

// debug messages are only in debug builds so they have full file + func + line info
void DebugMsg(const char *file, const char *func, int line, const char *msg, ...);

// errors are in final builds so they only have the function name
void ErrorMsg(const char *func, const char *msg, ...);

// infos are in final builds and they are meant to be standalone messages
void InfoMsg(const char *msg, ...);

#endif
