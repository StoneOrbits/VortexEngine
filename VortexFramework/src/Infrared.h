#ifndef IR_CONTROL_H
#define IR_CONTROL_H

#include <inttypes.h>

class Infrared
{
  // private unimplemented constructor
  Infrared();

public:
  // opting for static class here because there should only ever be one
  // Led control object and I don't like singletons
  static bool init();
  static void cleanup();

  static uint32_t read();
  static bool write(uint32_t val);

private:
};

#endif
