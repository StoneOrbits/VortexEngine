#ifndef IR_RECEIVER_H
#define IR_RECEIVER_H

#include <inttypes.h>

#include "../Serial/BitStream.h"

#include "IRConfig.h"

#if IR_ENABLE_RECEIVER == 1

class ByteStream;
class Mode;

class IRReceiver
{
  IRReceiver();

public:
  // init and cleanup the receiver
  static bool init();
  static void cleanup();

  // check whether a full IR message is ready to read
  static bool dataReady();
  // whether actively receiving
  static bool isReceiving();
  // the percent of data received
  static uint8_t percentReceived();
  static uint16_t bytesReceived() { return m_irData.bytepos(); }

  // receive the IR message into a target mode
  static bool receiveMode(Mode *pMode);

  // turn the receiver on/off
  static bool beginReceiving();
  static bool endReceiving();
  // checks if there is new data since the last reset or check
  static bool onNewData();
  // reset IR receiver buffer
  static void resetIRState();

private:

  // reading functions
  // PCI handler for when IR receiver pin changes states
  static bool read(ByteStream &data);
  static void recvPCIHandler();
  static void handleIRTiming(uint32_t diff);

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
    READING_DATA_SPACE
  };

  // state information used by the PCIHandler
  static RecvState m_recvState;
  // used to track pin changes
  static uint32_t m_prevTime;
  static uint8_t m_pinState;

  // used to compare if received data has changed since last checking
  static uint32_t m_previousBytes;

#ifdef VORTEX_LIB
  friend class Vortex;
#endif
};

#endif

#endif
