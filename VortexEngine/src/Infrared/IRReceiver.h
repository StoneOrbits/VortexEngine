#pragma once

#include <inttypes.h>

#include "Infrared.h"

#include "../Serial/BitStream.h"

class ByteStream;

class IRReceiver
{
  IRReceiver();

public:
  // init and cleanup the receiver
  static bool init();
  static void cleanup();

  // check whether a full IR message is ready to read
  // then read the data into a ByteStream
  static bool dataReady();
  static bool read(ByteStream &data);

  // turn the receiver on/off
  static bool beginReceiving();
  static bool endReceiving();

private:

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

};

