#ifndef LOG_H
#define LOG_H

#define DEBUG(msg, ...) \
  DebugMsg(__FILE__, __FUNCTION__, __LINE__, msg, __VA_ARGS__)

void DebugMsg(const char *file, const char *func, int line, const char *msg, ...);

#endif
