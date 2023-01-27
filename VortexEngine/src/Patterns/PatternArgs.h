#ifndef PATTERN_ARGS_H
#define PATTERN_ARGS_H

#include <inttypes.h>

// maximum number of args, this isn't really configurable so I'm
// not putting it in VortexConfig.h
#define MAX_ARGS 8

class ByteStream;

// a structured method for passing arguments to patterns, there's definitely more dynamic
// ways to approach this but I'm aiming for simple and lightweight here
class PatternArgs
{
public:
  // constructor for each number of arguments, automatically fills the 'numArgs' field accordingly
  PatternArgs();
  PatternArgs(uint8_t a1);
  PatternArgs(uint8_t a1, uint8_t a2);
  PatternArgs(uint8_t a1, uint8_t a2, uint8_t a3);
  PatternArgs(uint8_t a1, uint8_t a2, uint8_t a3, uint8_t a4);
  PatternArgs(uint8_t a1, uint8_t a2, uint8_t a3, uint8_t a4, uint8_t a5);
  PatternArgs(uint8_t a1, uint8_t a2, uint8_t a3, uint8_t a4, uint8_t a5, uint8_t a6);
  PatternArgs(uint8_t a1, uint8_t a2, uint8_t a3, uint8_t a4, uint8_t a5, uint8_t a6, uint8_t a7);
  PatternArgs(uint8_t a1, uint8_t a2, uint8_t a3, uint8_t a4, uint8_t a5, uint8_t a6, uint8_t a7, uint8_t a8);

  void init();
  void init(uint8_t a1);
  void init(uint8_t a1, uint8_t a2);
  void init(uint8_t a1, uint8_t a2, uint8_t a3);
  void init(uint8_t a1, uint8_t a2, uint8_t a3, uint8_t a4);
  void init(uint8_t a1, uint8_t a2, uint8_t a3, uint8_t a4, uint8_t a5);
  void init(uint8_t a1, uint8_t a2, uint8_t a3, uint8_t a4, uint8_t a5, uint8_t a6);
  void init(uint8_t a1, uint8_t a2, uint8_t a3, uint8_t a4, uint8_t a5, uint8_t a6, uint8_t a7);
  void init(uint8_t a1, uint8_t a2, uint8_t a3, uint8_t a4, uint8_t a5, uint8_t a6, uint8_t a7, uint8_t a8);

  bool operator!=(const PatternArgs &rhs);
  bool operator==(const PatternArgs &rhs);
  uint8_t operator[](int index) const;

  // must override the serialize routine to save the pattern
  void serialize(ByteStream &buffer) const;
  // must override unserialize to load patterns
  void unserialize(ByteStream &buffer);

  // TODO: apis around arg access?
  // public access to args
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
