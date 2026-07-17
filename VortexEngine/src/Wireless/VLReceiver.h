#ifndef VL_RECEIVER_H
#define VL_RECEIVER_H

#include <inttypes.h>

#include "../Serial/BitStream.h"

#include "VLConfig.h"

#if VL_ENABLE_RECEIVER == 1

class VortexEngine;
class ByteStream;
class Mode;

class VLReceiver
{
public:
  VLReceiver(VortexEngine &engine);
  ~VLReceiver();

  // init and cleanup the receiver
  bool init();
  void cleanup();

  // check whether a full VL message is ready to read
  bool dataReady();
  // whether actively receiving
  bool isReceiving();
  // the percent of data received
  uint8_t percentReceived();
  uint16_t bytesReceived() { return m_vlData.bytepos(); }

  // receive the VL message into a target mode
  bool receiveMode(Mode *pMode);

  // turn the receiver on/off
  bool beginReceiving();
  bool endReceiving();
  // checks if there is new data since the last reset or check
  bool onNewData();
  // reset VL receiver buffer
  void resetVLState();

  // turn on/off the legacy receiver
  void setLegacyReceiver(bool legacy) { m_legacy = legacy; }

  // The handler called when the VL receiver flips between on/off
  // this has to be public because it's called from a global ISR
  // but technically it's an internal function for VLReceiver
  void recvPCIHandler();
private:
  // reading functions
  // PCI handler for when VL receiver pin changes states
  bool read(ByteStream &data);
  void handleVLTiming(uint16_t diff);
  void handleVLTimingLegacy(uint16_t diff);

  // ===================
  //  private data:

  // reference to engine
  VortexEngine &m_engine;

  // BitStream object that VL data is fed to bit by bit
  BitStream m_vlData;

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
  RecvState m_recvState;
  // used to track pin changes
  uint32_t m_prevTime;
  uint8_t m_pinState;

  // used to compare if received data has changed since last checking
  uint32_t m_previousBytes;

  // the determined time based on sync
  uint16_t m_vlMarkThreshold;
  // the determined time based on sync
  uint16_t m_vlSpaceThreshold;

  // count of the sync bits (similar length starter bits)
  uint8_t m_counter;

  uint8_t m_parityBit;

  // legacy mode
  bool m_legacy;

#ifdef VORTEX_LIB
  friend class Vortex;
#endif
};

#endif

#endif
