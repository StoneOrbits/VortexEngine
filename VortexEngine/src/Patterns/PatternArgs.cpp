#include "PatternArgs.h"
#include "../Serial/ByteStream.h"
void PatternArgs_make8(PatternArgs *self, uint8_t a1, uint8_t a2, uint8_t a3, uint8_t a4, uint8_t a5, uint8_t a6, uint8_t a7, uint8_t a8)
{
  PatternArgs_init8(self, a1, a2, a3, a4, a5, a6, a7, a8);
}
void PatternArgs_init8(PatternArgs *self, uint8_t a1, uint8_t a2, uint8_t a3, uint8_t a4, uint8_t a5, uint8_t a6, uint8_t a7, uint8_t a8)
{
  self->arg1 = a1;
  self->arg2 = a2;
  self->arg3 = a3;
  self->arg4 = a4;
  self->arg5 = a5;
  self->arg6 = a6;
  self->arg7 = a7;
  self->arg8 = a8;
  self->numArgs = 8;
}
void PatternArgs_make0(PatternArgs *self)
{
  PatternArgs_init0(self);
}
void PatternArgs_init0(PatternArgs *self)
{
  self->arg1 = 0;
  self->arg2 = 0;
  self->arg3 = 0;
  self->arg4 = 0;
  self->arg5 = 0;
  self->arg6 = 0;
  self->arg7 = 0;
  self->arg8 = 0;
  self->numArgs = 0;
}
void PatternArgs_make1(PatternArgs *self, uint8_t a1)
{
  PatternArgs_init1(self, a1);
}
void PatternArgs_init1(PatternArgs *self, uint8_t a1)
{
  self->arg1 = a1;
  self->arg2 = 0;
  self->arg3 = 0;
  self->arg4 = 0;
  self->arg5 = 0;
  self->arg6 = 0;
  self->arg7 = 0;
  self->arg8 = 0;
  self->numArgs = 1;
}
void PatternArgs_make2(PatternArgs *self, uint8_t a1, uint8_t a2)
{
  PatternArgs_init2(self, a1, a2);
}
void PatternArgs_init2(PatternArgs *self, uint8_t a1, uint8_t a2)
{
  self->arg1 = a1;
  self->arg2 = a2;
  self->arg3 = 0;
  self->arg4 = 0;
  self->arg5 = 0;
  self->arg6 = 0;
  self->arg7 = 0;
  self->arg8 = 0;
  self->numArgs = 2;
}
void PatternArgs_make3(PatternArgs *self, uint8_t a1, uint8_t a2, uint8_t a3)
{
  PatternArgs_init3(self, a1, a2, a3);
}
void PatternArgs_init3(PatternArgs *self, uint8_t a1, uint8_t a2, uint8_t a3)
{
  self->arg1 = a1;
  self->arg2 = a2;
  self->arg3 = a3;
  self->arg4 = 0;
  self->arg5 = 0;
  self->arg6 = 0;
  self->arg7 = 0;
  self->arg8 = 0;
  self->numArgs = 3;
}
void PatternArgs_make4(PatternArgs *self, uint8_t a1, uint8_t a2, uint8_t a3, uint8_t a4)
{
  PatternArgs_init4(self, a1, a2, a3, a4);
}
void PatternArgs_init4(PatternArgs *self, uint8_t a1, uint8_t a2, uint8_t a3, uint8_t a4)
{
  self->arg1 = a1;
  self->arg2 = a2;
  self->arg3 = a3;
  self->arg4 = a4;
  self->arg5 = 0;
  self->arg6 = 0;
  self->arg7 = 0;
  self->arg8 = 0;
  self->numArgs = 4;
}
void PatternArgs_make5(PatternArgs *self, uint8_t a1, uint8_t a2, uint8_t a3, uint8_t a4, uint8_t a5)
{
  PatternArgs_init5(self, a1, a2, a3, a4, a5);
}
void PatternArgs_init5(PatternArgs *self, uint8_t a1, uint8_t a2, uint8_t a3, uint8_t a4, uint8_t a5)
{
  self->arg1 = a1;
  self->arg2 = a2;
  self->arg3 = a3;
  self->arg4 = a4;
  self->arg5 = a5;
  self->arg6 = 0;
  self->arg7 = 0;
  self->arg8 = 0;
  self->numArgs = 5;
}
void PatternArgs_make6(PatternArgs *self, uint8_t a1, uint8_t a2, uint8_t a3, uint8_t a4, uint8_t a5, uint8_t a6)
{
  PatternArgs_init6(self, a1, a2, a3, a4, a5, a6);
}
void PatternArgs_init6(PatternArgs *self, uint8_t a1, uint8_t a2, uint8_t a3, uint8_t a4, uint8_t a5, uint8_t a6)
{
  self->arg1 = a1;
  self->arg2 = a2;
  self->arg3 = a3;
  self->arg4 = a4;
  self->arg5 = a5;
  self->arg6 = a6;
  self->arg7 = 0;
  self->arg8 = 0;
  self->numArgs = 6;
}
void PatternArgs_make7(PatternArgs *self, uint8_t a1, uint8_t a2, uint8_t a3, uint8_t a4, uint8_t a5, uint8_t a6, uint8_t a7)
{
  PatternArgs_init7(self, a1, a2, a3, a4, a5, a6, a7);
}
void PatternArgs_init7(PatternArgs *self, uint8_t a1, uint8_t a2, uint8_t a3, uint8_t a4, uint8_t a5, uint8_t a6, uint8_t a7)
{
  self->arg1 = a1;
  self->arg2 = a2;
  self->arg3 = a3;
  self->arg4 = a4;
  self->arg5 = a5;
  self->arg6 = a6;
  self->arg7 = a7;
  self->arg8 = 0;
  self->numArgs = 7;
}
void PatternArgs_addArgs1(PatternArgs *self, uint8_t a1)
{
  self->args[self->numArgs++] = a1;
}
void PatternArgs_addArgs2(PatternArgs *self, uint8_t a1, uint8_t a2)
{
  self->args[self->numArgs++] = a1;
  self->args[self->numArgs++] = a2;
}
void PatternArgs_addArgs3(PatternArgs *self, uint8_t a1, uint8_t a2, uint8_t a3)
{
  self->args[self->numArgs++] = a1;
  self->args[self->numArgs++] = a2;
  self->args[self->numArgs++] = a3;
}
void PatternArgs_addArgs4(PatternArgs *self, uint8_t a1, uint8_t a2, uint8_t a3, uint8_t a4)
{
  self->args[self->numArgs++] = a1;
  self->args[self->numArgs++] = a2;
  self->args[self->numArgs++] = a3;
  self->args[self->numArgs++] = a4;
}
void PatternArgs_addArgs5(PatternArgs *self, uint8_t a1, uint8_t a2, uint8_t a3, uint8_t a4, uint8_t a5)
{
  self->args[self->numArgs++] = a1;
  self->args[self->numArgs++] = a2;
  self->args[self->numArgs++] = a3;
  self->args[self->numArgs++] = a4;
  self->args[self->numArgs++] = a5;
}
void PatternArgs_addArgs6(PatternArgs *self, uint8_t a1, uint8_t a2, uint8_t a3, uint8_t a4, uint8_t a5, uint8_t a6)
{
  self->args[self->numArgs++] = a1;
  self->args[self->numArgs++] = a2;
  self->args[self->numArgs++] = a3;
  self->args[self->numArgs++] = a4;
  self->args[self->numArgs++] = a5;
  self->args[self->numArgs++] = a6;
}
void PatternArgs_addArgs7(PatternArgs *self, uint8_t a1, uint8_t a2, uint8_t a3, uint8_t a4, uint8_t a5, uint8_t a6, uint8_t a7)
{
  self->args[self->numArgs++] = a1;
  self->args[self->numArgs++] = a2;
  self->args[self->numArgs++] = a3;
  self->args[self->numArgs++] = a4;
  self->args[self->numArgs++] = a5;
  self->args[self->numArgs++] = a6;
  self->args[self->numArgs++] = a7;
}
void PatternArgs_addArgs8(PatternArgs *self, uint8_t a1, uint8_t a2, uint8_t a3, uint8_t a4, uint8_t a5, uint8_t a6, uint8_t a7, uint8_t a8)
{
  self->args[self->numArgs++] = a1;
  self->args[self->numArgs++] = a2;
  self->args[self->numArgs++] = a3;
  self->args[self->numArgs++] = a4;
  self->args[self->numArgs++] = a5;
  self->args[self->numArgs++] = a6;
  self->args[self->numArgs++] = a7;
  self->args[self->numArgs++] = a7;
}
bool PatternArgs_notEqual(const PatternArgs *self, const PatternArgs *rhs)
{
  return !PatternArgs_equal(self, rhs);
}

bool PatternArgs_equal(const PatternArgs *self, const PatternArgs *rhs)
{
  // just check number of args first
  if (rhs->numArgs != self->numArgs) {
    return false;
  }
  // compare num args
  for (uint8_t i = 0; i < self->numArgs; ++i) {
    if (self->args[i] != rhs->args[i]) {
      return false;
    }
  }
  return true;
}

uint8_t PatternArgs_at(const PatternArgs *self, int index)
{
  if (index > 7) {
    return 0;
  }
  return self->args[index];
}

bool PatternArgs_serialize(const PatternArgs *self, ByteStream *buffer, ArgMap argmap)
{
  if (!ByteStream_serialize8(buffer, argmap)) {
    return false;
  }
  for (uint8_t i = 0; i < MAX_ARGS; ++i) {
    if (ARGMAP_ISSET(argmap, i)) {
      if (!ByteStream_serialize8(buffer, self->args[i])) {
        return false;
      }
    }
  }
  return true;
}

bool PatternArgs_serializeAll(const PatternArgs *self, ByteStream *buffer)
{
  return PatternArgs_serialize(self, buffer, ARG_ALL);
}

ArgMap PatternArgs_unserialize(PatternArgs *self, ByteStream *buffer)
{
  ArgMap argmap = ARG_NONE;
  if (!ByteStream_unserialize8(buffer, &argmap)) {
    return ARG_NONE;
  }
  for (uint8_t i = 0; i < MAX_ARGS; ++i) {
    if (ARGMAP_ISSET(argmap, i)) {
      if (!ByteStream_unserialize8(buffer, self->args + i)) {
        break;
      }
    }
  }
  return argmap;
}
