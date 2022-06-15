#ifndef IR_CONTROL_H
#define IR_CONTROL_H

#include <inttypes.h>

#include "BitStream.h"

// the IR receiver buffer size in dwords
#define IR_RECV_BUF_SIZE 32

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

  // read data from internal buffer
  static bool read(SerialBuffer &data);
  // write data to internal queue to send
  static bool write(SerialBuffer &data);

  static bool beginReceiving();
  static bool endReceiving();

private:
  // writing functions
  static void initpwm();
  static void delayus(uint16_t time);
  static void write8(uint8_t data);
  static void write32(uint32_t data);
  static uint32_t mark(uint16_t time);
  static uint32_t space(uint16_t time);

  // reading functions
  // PCI handler for when IR receiver pin changes states
  static void recvPCIHandler();
  static void handleIRTiming(uint32_t diff);
  static void resetIRState();


  // ===================
  //  private data:

  // BitStream object that feeds bits to the ir data buf
  static BitStream m_irData;

  // Receive state used for state machine in PCIhandler
  enum RecvState : uint8_t
  {
    WAITING_HEADER_MARK,
    WAITING_HEADER_SPACE,
    READING_DATA_MARK,
    READING_DATA_SPACE,
  };

  // state information used by the PCIHandler
  static RecvState m_recvState;
  // used to track pin changes
  static uint64_t m_prevTime;
  static uint8_t m_pinState;
};

#endif
