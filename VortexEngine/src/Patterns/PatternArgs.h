#ifndef PATTERN_ARGS_H
#define PATTERN_ARGS_H

#include <inttypes.h>

// maximum number of args, this isn't really configurable so I'm
// not putting it in VortexConfig.h
#define MAX_ARGS 8

// a structured method for passing arguments to patterns, there's definitely more dynamic
// ways to approach this but I'm aiming for simple and lightweight here
class PatternArgs
{
public:
  // constructor for each number of arguments, automatically fills the 'numArgs' field accordingly
  PatternArgs() : 
    arg1(0), arg2(0), arg3(0), arg4(0), arg5(0), arg6(0), arg7(0), arg8(0), numArgs(0) { }
  PatternArgs(uint8_t a1) :
    arg1(a1), arg2(0), arg3(0), arg4(0), arg5(0), arg6(0), arg7(0), arg8(0), numArgs(1) { }
  PatternArgs(uint8_t a1, uint8_t a2) :
    arg1(a1), arg2(a2), arg3(0), arg4(0), arg5(0), arg6(0), arg7(0), arg8(0), numArgs(2) { }
  PatternArgs(uint8_t a1, uint8_t a2, uint8_t a3) :
    arg1(a1), arg2(a2), arg3(a3), arg4(0), arg5(0), arg6(0), arg7(0), arg8(0), numArgs(3) { }
  PatternArgs(uint8_t a1, uint8_t a2, uint8_t a3, uint8_t a4) :
    arg1(a1), arg2(a2), arg3(a3), arg4(a4), arg5(0), arg6(0), arg7(0), arg8(0), numArgs(4) { }
  PatternArgs(uint8_t a1, uint8_t a2, uint8_t a3, uint8_t a4, uint8_t a5) :
    arg1(a1), arg2(a2), arg3(a3), arg4(a4), arg5(a5), arg6(0), arg7(0), arg8(0), numArgs(5) { }
  PatternArgs(uint8_t a1, uint8_t a2, uint8_t a3, uint8_t a4, uint8_t a5, uint8_t a6) :
    arg1(a1), arg2(a2), arg3(a3), arg4(a4), arg5(a5), arg6(a6), arg7(0), arg8(0), numArgs(6) { }
  PatternArgs(uint8_t a1, uint8_t a2, uint8_t a3, uint8_t a4, uint8_t a5, uint8_t a6, uint8_t a7) :
    arg1(a1), arg2(a2), arg3(a3), arg4(a4), arg5(a5), arg6(a6), arg7(a7), arg8(0), numArgs(7) { }
  PatternArgs(uint8_t a1, uint8_t a2, uint8_t a3, uint8_t a4, uint8_t a5, uint8_t a6, uint8_t a7, uint8_t a8) :
    arg1(a1), arg2(a2), arg3(a3), arg4(a4), arg5(a5), arg6(a6), arg7(a7), arg8(a8), numArgs(8) { }

  void init() {
    *this = PatternArgs();
  }
  void init(uint8_t a1) {
    *this = PatternArgs(a1);
  }
  void init(uint8_t a1, uint8_t a2) {
    *this = PatternArgs(a1, a2);
  }
  void init(uint8_t a1, uint8_t a2, uint8_t a3) {
    *this = PatternArgs(a1, a2, a3);
  }
  void init(uint8_t a1, uint8_t a2, uint8_t a3, uint8_t a4) {
    *this = PatternArgs(a1, a2, a3, a4);
  }
  void init(uint8_t a1, uint8_t a2, uint8_t a3, uint8_t a4, uint8_t a5) {
    *this = PatternArgs(a1, a2, a3, a4, a5);
  }
  void init(uint8_t a1, uint8_t a2, uint8_t a3, uint8_t a4, uint8_t a5, uint8_t a6) {
    *this = PatternArgs(a1, a2, a3, a4, a5, a6);
  }
  void init(uint8_t a1, uint8_t a2, uint8_t a3, uint8_t a4, uint8_t a5, uint8_t a6, uint8_t a7) {
    *this = PatternArgs(a1, a2, a3, a4, a5, a6, a7);
  }
  void init(uint8_t a1, uint8_t a2, uint8_t a3, uint8_t a4, uint8_t a5, uint8_t a6, uint8_t a7, uint8_t a8) {
    *this = PatternArgs(a1, a2, a3, a4, a5, a6, a7, a8);
  }

  bool operator!=(const PatternArgs &rhs) {
    return !(*this == rhs);
  }
  bool operator==(const PatternArgs &rhs) {
    // just check number of args first
    if (rhs.numArgs != numArgs) {
      return false;
    }
    // compare all args
    return (arg1 == rhs.arg1 && arg2 == rhs.arg2 &&
            arg3 == rhs.arg3 && arg4 == rhs.arg4 &&
            arg5 == rhs.arg5 && arg6 == rhs.arg6 &&
            arg7 == rhs.arg7 && arg8 == rhs.arg8);
  }

  uint8_t operator[](int index) const {
    if (index > 7) {
      return 0;
    }
    return args[index];
  }

  union {
    uint8_t args[8];
    struct {
      uint8_t arg1;
      uint8_t arg2;
      uint8_t arg3;
      uint8_t arg4;
      uint8_t arg5;
      uint8_t arg6;
      uint8_t arg7;
      uint8_t arg8;
    };
  };
  // indicates how many of the above args are filled
  uint8_t numArgs;
};

#endif
