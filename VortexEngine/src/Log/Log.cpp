#include "Log.h"

#include "../Time/TimeControl.h"

#include <stdarg.h>
#include <string.h>
#include <stdio.h>

#ifdef VORTEX_LIB
#include "VortexLib.h"
#endif

#if LOGGING_LEVEL > 0
void InfoMsg(const char *msg, ...)
{
#ifdef VORTEX_EMBEDDED
  return;
#endif
  va_list list;
  va_start(list, msg);
#ifndef VORTEX_EMBEDDED
  Vortex::printlog(NULL, NULL, 0, msg, list);
#endif
  va_end(list);
}
#endif

#if LOGGING_LEVEL > 1
void ErrorMsg(const char *func, const char *msg, ...)
{
#ifdef VORTEX_EMBEDDED
  return;
#endif
  va_list list;
  va_start(list, msg);
#ifndef VORTEX_EMBEDDED
  Vortex::printlog(NULL, func, 0, msg, list);
#endif
  va_end(list);
}
#endif

#if LOGGING_LEVEL > 2
void DebugMsg(const char *file, const char *func, int line, const char *msg, ...)
{
#ifdef VORTEX_EMBEDDED
  return;
#endif
  va_list list;
  va_start(list, msg);
  const char *ptr = file + strlen(file);
  // go backwards till either start or backslash
  while (ptr > file) {
    if (*ptr == '\\') {
      // file starts after the backslash
      file = ptr + 1;
      break;
    }
    ptr--;
  }
#ifndef VORTEX_EMBEDDED
  Vortex::printlog(file, func, line, msg, list);
#endif
  va_end(list);
}
#endif
