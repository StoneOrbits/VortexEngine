#include "Log.h"

#include "../Time/TimeControl.h"
#include "../Serial/Serial.h"

#include <stdarg.h>
#include <string.h>
#include <stdio.h>

#ifdef VORTEX_LIB
#include "VortexLib.h"
#endif

void Vortex_printlog(const char *file, const char *func, int line, const char *msg, va_list list);

#if LOGGING_LEVEL > 0
void InfoMsg(const char *msg, ...)
{
  va_list list;
  va_start(list, msg);
  Vortex_printlog(NULL, NULL, 0, msg, list);
  va_end(list);
}
#endif

#if LOGGING_LEVEL > 1
void ErrorMsg(const char *func, const char *msg, ...)
{
  va_list list;
  va_start(list, msg);
  Vortex_printlog(NULL, func, 0, msg, list);
  va_end(list);
}
#endif

#if LOGGING_LEVEL > 2
void DebugMsg(const char *file, const char *func, int line, const char *msg, ...)
{
  va_list list;
  va_start(list, msg);
  const char *ptr = file + strlen(file);
  while (ptr > file) {
    if (*ptr == '\\') {
      file = ptr + 1;
      break;
    }
    ptr--;
  }
  Vortex_printlog(file, func, line, msg, list);
  va_end(list);
}
#endif
