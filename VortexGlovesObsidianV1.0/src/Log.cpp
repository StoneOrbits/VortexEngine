#include "Log.h"

#include <stdarg.h>

#ifdef TEST_FRAMEWORK
#include "TestFramework.h"
#endif

void DebugMsg(const char *file, const char *func, int line, const char *msg, ...)
{
  va_list list;
  va_start(list, msg);
#ifdef TEST_FRAMEWORK
  TestFramework::printlog(file, func, line, msg, list);
#else
  char fmt[2048] = {0};
  snprintf(fmt, sizeof(fmt), "%s:%d %s(): %s", file, line, func, msg);
  char buf[2048] = {0};
  vsnprintf(buf, sizeof(buf), fmt, list);
#endif
  va_end(list);
}
