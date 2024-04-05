#include "UPDI.h"

#ifdef VORTEX_EMBEDDED
#include <Arduino.h>
#else
#define HIGH 0x1
#define LOW 0x0
#endif

#include <string.h>

UPDI::UPDI(uint8_t txPin, uint8_t rxPin) : m_txPin(txPin), m_rxPin(rxPin), m_bufferIndex(0) {
#ifdef VORTEX_EMBEDDED
  pinMode(m_txPin, OUTPUT);
  pinMode(m_rxPin, INPUT);
#endif
  // Initialize the buffer array
  memset(m_buffer, 0, sizeof(m_buffer));
}

void UPDI::initializeUPDICommunication() {
  sendBreakFrame();
  sendSynchCharacter();
}

void UPDI::readEEPROMAndUserRow() {
  // Assuming sizes and starting addresses are known
  uint16_t userRowSize = 128; // Example size
  uint32_t userRowStartAddress = 0x1300;
  uint16_t eepromSize = 256; // Example size
  uint32_t eepromStartAddress = 0x1400;

  // Read USERROW
  for (uint16_t i = 0; i < userRowSize; i++) {
    sendLdsInstruction(userRowStartAddress + i, 1);
    // Immediately read back the data
    uint8_t dataByte = receiveByte();
    // Process or store `dataByte` as needed
  }

  // Read EEPROM
  for (uint16_t i = 0; i < eepromSize; i++) {
    sendLdsInstruction(eepromStartAddress + i, 1);
    // Immediately read back the data
    uint8_t dataByte = receiveByte();
    // Process or store `dataByte` as needed
  }
}

void UPDI::writeEEPROMAndUserRow(const uint8_t* data, uint16_t size) {
  // Split the data size for USERROW and EEPROM equally for this example
  uint32_t userRowStartAddress = 0x1300;
  uint32_t eepromStartAddress = 0x1400;
  uint16_t halfSize = size / 2;

  // Write to USERROW
  for (uint16_t i = 0; i < halfSize; i++) {
    sendStsInstruction(userRowStartAddress + i, 1, data[i]);
  }

  // Write to EEPROM
  for (uint16_t i = 0; i < halfSize; i++) {
    sendStsInstruction(eepromStartAddress + i, 1, data[halfSize + i]);
  }
}

void UPDI::writeFirmwareToProgramFlash(const uint8_t* firmware, uint32_t size) {
  // Placeholder: Detailed implementation would follow a similar pattern to EEPROM/User Row writing
  // but with considerations for flash page sizes, erase cycles, and possibly using NVM commands.
}

void UPDI::sendKey(KeyType keyType) {
  uint64_t key;
  switch (keyType) {
    case CHIP_ERASE:
      key = 0x4E564D4572617365ULL;
      break;
    case NVMPROG:
      key = 0x4E564D50726F6720ULL;
      break;
    case USERROW_WRITE:
      key = 0x4E564D5573267465ULL;
      break;
    default:
      return; // Invalid key type
  }
  sendKeyInstruction(key);
}

void UPDI::sendLdsInstruction(uint32_t address, uint8_t addressSize) {
  // Reset command buffer
  m_bufferIndex = 0;

  // Buffer the LDS instruction, address size, and the address itself
  bufferByte(0x00); // LDS opcode placeholder
  bufferByte(addressSize);
  for (int i = 0; i < addressSize; i++) {
    bufferByte((address >> (8 * i)) & 0xFF);
  }

  executeInstruction();
}

void UPDI::sendStsInstruction(uint32_t address, uint8_t addressSize, uint8_t data) {
  m_bufferIndex = 0;

  bufferByte(0x40); // STS opcode placeholder
  bufferByte(addressSize);
  for (int i = 0; i < addressSize; i++) {
    bufferByte((address >> (8 * i)) & 0xFF);
  }
  bufferByte(data);

  executeInstruction();
}

void UPDI::sendLdcsInstruction(uint8_t csAddress) {
  m_bufferIndex = 0;

  bufferByte(0x04); // LDCS opcode placeholder
  bufferByte(csAddress);

  executeInstruction();
}

void UPDI::sendStcsInstruction(uint8_t csAddress, uint8_t data) {
  m_bufferIndex = 0;

  bufferByte(0xC0); // STCS opcode placeholder
  bufferByte(csAddress);
  bufferByte(data);

  executeInstruction();
}

void UPDI::sendKeyInstruction(uint64_t key) {
  // KEY instruction is prefixed with 0xE0 followed by the 8-byte key
  m_bufferIndex = 0; // Reset the buffer before building the command

  bufferByte(0xE0); // Key command opcode (example, adjust as needed)
  for (int i = 0; i < 8; i++) {
    // Sending LSB first
    bufferByte((key >> (8 * i)) & 0xFF);
  }

  executeInstruction(); // Send the buffered command
}

void UPDI::executeInstruction() {
  for (uint16_t i = 0; i < m_bufferIndex; i++) {
#ifdef VORTEX_EMBEDDED
    digitalWrite(m_txPin, m_buffer[i] ? HIGH : LOW);
    delayMicroseconds(444); // Adjust based on the actual baud rate
#endif
  }
  m_bufferIndex = 0;
}

uint8_t UPDI::receiveByte() {
  uint8_t data = 0;
#ifdef VORTEX_EMBEDDED
  for (int i = 0; i < 8; ++i) {
    while (digitalRead(m_rxPin) == HIGH); // Wait for the start bit

    delayMicroseconds(222); // Wait for the middle of the bit
    data |= (digitalRead(m_rxPin) << i);
    delayMicroseconds(222); // Finish the bit period
  }
#endif
  // Skipping parity and stop bits for simplicity
  return data;
}

void UPDI::bufferByte(uint8_t value, bool includeParity) {
  // Start bit
  bufferBit(LOW);
  bool parity = 0;
  // Buffer each bit of the byte
  for (int i = 0; i < 8; ++i) {
    bool bit = value & (1 << i);
    bufferBit(bit);
    parity ^= bit;
  }
  // Optionally include the parity bit
  if (includeParity) {
    bufferBit(parity);
  }
  // Stop bits
  bufferBit(HIGH);
  bufferBit(HIGH);
}

void UPDI::bufferBit(bool bitValue) {
  // Example method, assuming we directly map bits to HIGH/LOW states in m_buffer for simplicity
  // In a real implementation, you'd handle timing and synchronization more precisely
  if (m_bufferIndex < sizeof(m_buffer) * 8) { // Assuming 8 bits per buffered command byte
                                                            // Simplified: Just directly add the bit as a byte for demonstration
    m_buffer[m_bufferIndex++] = bitValue ? 1 : 0;
  }
}