#include "Log.h"

#include <stdarg.h>

#ifdef TEST_FRAMEWORK
#include "TestFramework.h"
#endif

void DebugMsg(const char *file, const char *func, int line, const char *msg, ...)
{
#ifdef TEST_FRAMEWORK
  va_list list;
  va_start(list, msg);
  TestFramework::printlog(file, func, line, msg, list);
  va_end(list);
#endif
}
