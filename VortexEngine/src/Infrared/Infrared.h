#ifndef IR_CONTROL_H
#define IR_CONTROL_H

#include <inttypes.h>

#include "../Serial/SerialBuffer.h"
#include "../Serial/BitStream.h"

class Mode;

class IRSender
{
public:
  IRSender();
  IRSender(const Mode *targetMode);
  ~IRSender();

  // initialize the IR sender with a serialized mode to send
  bool init(const Mode *targetMode);
  void send();

  bool isSending() const { return m_isSending; }

private:
  bool initSend();

  SerialBuffer m_serialBuf;
  bool m_isSending;
  uint64_t m_startTime;
};

struct ir_block;

class Infrared
{
  friend class IRSender;
  // private unimplemented constructor
  Infrared();

public:
  // opting for static class here because there should only ever be one
  // Led control object and I don't like singletons
  static bool init();
  static void cleanup();

  // check whether a full IR message is ready to read
  static bool dataReady();

  // read any received data from internal buffer
  static bool read(SerialBuffer &data);
  // write data to internal to queue for send
  //static bool write(SerialBuffer &data);
  static bool write(const ir_block *block, uint32_t blocksize = 32);

  // turn the receiver on/off
  static bool beginReceiving();
  static bool endReceiving();

private:
  // writing functions
  static void initpwm();
  static void startWrite();
  static void write8(uint8_t data);
  static void mark(uint16_t time);
  static void space(uint16_t time);

  // reading functions
  // PCI handler for when IR receiver pin changes states
  static void recvPCIHandler();
  static void handleIRTiming(uint32_t diff);
  static void resetIRState();

  // ===================
  //  private data:

  // BitStream object that IR data is fed to bit by bit
  static BitStream m_irData;

  // Receive state used for state machine in PCIhandler
  enum RecvState : uint8_t
  {
    WAITING_HEADER_MARK,
    WAITING_HEADER_SPACE,
    READING_DATA_MARK,
    READING_DATA_SPACE,
    READING_DATA_DIVIDER_MARK,
    READING_DATA_DIVIDER_SPACE,
  };

  // state information used by the PCIHandler
  static RecvState m_recvState;
  // used to track pin changes
  static uint64_t m_prevTime;
  static uint8_t m_pinState;
  static uint32_t m_writeCounter;
};

#endif
