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

  static bool write(uint32_t val);
  static uint32_t read();

private:
  // writing functions
  static void initpwm();
  static void irsend(uint32_t data);
  static void delayus(uint16_t time);
  static uint32_t mark(uint16_t time);
  static uint32_t space(uint16_t time);

  // reading functions
  static bool poll();
  static bool getResults();
  static bool decode(uint32_t &data);

};

#endif
