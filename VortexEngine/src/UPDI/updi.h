#ifndef UPDI_H
#define UPDI_H

#include <inttypes.h>

class UPDI {
  public:
    UPDI(uint8_t txPin, uint8_t rxPin);
    void initializeUPDICommunication();

    // Reading and Writing High-Level APIs
    void readEEPROMAndUserRow();
    void writeEEPROMAndUserRow(const uint8_t* data, uint16_t size);
    void writeFirmwareToProgramFlash(const uint8_t* firmware, uint32_t size);

    // Key Types for sendKey method
    enum KeyType {
      CHIP_ERASE = 0,
      NVMPROG = 1,
      USERROW_WRITE = 2
    };
    void sendKey(KeyType keyType);

  private:
    void sendBreakFrame();
    void sendSynchCharacter();

    // Instruction-specific methods
    void sendLdsInstruction(uint32_t address, uint8_t addressSize);
    void sendStsInstruction(uint32_t address, uint8_t addressSize, uint8_t data);
    void sendLdcsInstruction(uint8_t csAddress);
    void sendStcsInstruction(uint8_t csAddress, uint8_t data);
    void sendKeyInstruction(uint64_t key);

    void executeInstruction();
    uint8_t receiveByte();
    void bufferByte(uint8_t value, bool includeParity = true);
    void bufferBit(bool bitValue);

    static const int UPDI_BUFFER_SIZE = 1024;
    uint8_t m_txPin;
    uint8_t m_rxPin;
    uint8_t m_buffer[UPDI_BUFFER_SIZE];
    uint16_t m_bufferIndex;
};

#endif // UPDI_H

