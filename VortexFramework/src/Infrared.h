#ifndef IR_CONTROL_H
#define IR_CONTROL_H

#include <inttypes.h>

class SerialBuffer;

class Infrared
{
  // private unimplemented constructor
  Infrared();

public:
  // opting for static class here because there should only ever be one
  // Led control object and I don't like singletons
  static bool init();
  static void cleanup();

  // write data to internal queue to send
  static bool write(SerialBuffer &data);
  // read data from internal buffer
  static bool read(SerialBuffer &data);

private:
  // writing functions
  static void initpwm();
  static void delayus(uint16_t time);
  static void write32(uint32_t data);
  static uint32_t mark(uint16_t time);
  static uint32_t space(uint16_t time);

  // reading functions
  static bool match_next_ir_data(uint32_t expected);
  static uint32_t next_ir_data();
  static uint32_t decode32();

  static void recvPCIHandler();

};

#endif
