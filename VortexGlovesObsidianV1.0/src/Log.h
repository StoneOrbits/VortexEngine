#ifndef LOG_H
#define LOG_H

#include <stdarg.h>

#ifdef TEST_FRAMEWORK || DEBUG
// arduino compiler won't allow for ellipsis macro that's passed no args...
#define DEBUG(msg) DebugMsg(__FILE__, __FUNCTION__, __LINE__, msg)
#define DEBUGF(msg, ...) DebugMsg(__FILE__, __FUNCTION__, __LINE__, msg, __VA_ARGS__)
#else
#define DEBUG(msg) 
#define DEBUGF(msg, ...) 
#endif

void DebugMsg(const char *file, const char *func, int line, const char *msg, ...);

#endif
