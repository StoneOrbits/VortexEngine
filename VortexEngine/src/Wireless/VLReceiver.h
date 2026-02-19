#ifndef VL_RECEIVER_H
#define VL_RECEIVER_H

#include <inttypes.h>

#include "../Serial/BitStream.h"

#include "VLConfig.h"

#if VL_ENABLE_RECEIVER == 1

class ByteStream;
class Mode;

class VLReceiver
{
  VLReceiver();

public:
  // init and cleanup the receiver
  static bool init();
  static void cleanup();

  // check whether a full VL message is ready to read
  static bool dataReady();
  // whether actively receiving
  static bool isReceiving();
  // the percent of data received
  static uint8_t percentReceived();
  static uint16_t bytesReceived() { return m_vlData.bytepos(); }

  // receive the VL message into a target mode
  static bool receiveMode(Mode *pMode);

  // turn the receiver on/off
  static bool beginReceiving();
  static bool endReceiving();
  // checks if there is new data since the last reset or check
  static bool onNewData();
  // reset VL receiver buffer
  static void resetVLState();

  // turn on/off the legacy receiver
  static void setLegacyReceiver(bool legacy) { m_legacy = legacy; }

  // The handler called when the VL receiver flips between on/off
  // this has to be public because it's called from a global ISR
  // but technically it's an internal function for VLReceiver
  static void recvPCIHandler();
private:
  // reading functions
  // PCI handler for when VL receiver pin changes states
  static bool read(ByteStream &data);
  static void handleVLTiming(uint16_t diff);
  static void handleVLTimingLegacy(uint16_t diff);

  // ===================
  //  private data:

  // BitStream object that VL data is fed to bit by bit
  static BitStream m_vlData;

  // Receive state used for state machine in PCIhandler
  enum RecvState : uint8_t
  {
    WAITING_HEADER_MARK,
    WAITING_HEADER_SPACE,
    READING_BAUD_MARK,
    READING_BAUD_SPACE,
    READING_DATA_MARK,
    READING_DATA_SPACE,
    READING_DATA_PARITY_MARK,
    READING_DATA_PARITY_SPACE
  };

  // state information used by the PCIHandler
  static RecvState m_recvState;
  // used to track pin changes
  static uint32_t m_prevTime;
  static uint8_t m_pinState;

  // used to compare if received data has changed since last checking
  static uint16_t m_previousBytes;

  // the determined time based on sync
  static uint16_t m_vlMarkThreshold;
  // the determined time based on sync
  static uint16_t m_vlSpaceThreshold;

  // count of the sync bits (similar length starter bits)
  static uint8_t m_counter;

  static uint8_t m_parityBit;

  // legacy mode
  static bool m_legacy;

#ifdef VORTEX_LIB
  friend class Vortex;
#endif
};

#endif

#endif
