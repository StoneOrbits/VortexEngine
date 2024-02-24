#include "UPDIFlasher.h"

#include "../Log/Log.h"
#include "../Time/TimeControl.h"

#ifdef VORTEX_EMBEDDED

#define UPDI_PIN (gpio_num_t)10

// UPDI command codes and SIB address (example address, confirm from datasheet)
#define UPDI_LDS 0x4C  // Load Direct from Space (single byte)
#define SIB_ADDRESS 0x1000  // Hypothetical SIB address, replace with the actual address
#define SIB_LENGTH 16  // Assumed length of SIB; confirm from datasheet

bool UPDIFlasher::init()
{
  FastGPIO::init(UPDI_PIN);
  FastGPIO::setOutput(); // Set UPDI pin as output initially
  return true;
}

void UPDIFlasher::readSIB()
{
  sendBreak();
  sendSynch();

  // Construct the command sequence to read the SIB
  std::vector<uint8_t> command;
  command.push_back(UPDI_LDS); // LDS command
  // Append the address of the SIB (16-bit address, LSB first)
  command.push_back(SIB_ADDRESS & 0xFF);
  command.push_back((SIB_ADDRESS >> 8) & 0xFF);
  // For LDS command, specify the length of the data to read - assuming SIB_LENGTH is correct
  for (uint8_t i = 0; i < SIB_LENGTH; ++i) {
    command.push_back(0x00);  // Placeholder for address increment, if needed
  }

  sendUPDICommand(command);

  // Assuming the UPDI device auto-increments the address after each byte read,
  // the above command may need adjustment if auto-increment is not enabled or behaves differently.

  // Switch to input to read the response
  FastGPIO::setInput();
  auto sibData = readBytes(SIB_LENGTH);  // Read the bytes of the SIB
  FastGPIO::setOutput();  // Switch back to output after reading

  // Process and display the SIB data
  for (auto byte : sibData) {
    INFO_LOGF("SIB Byte: %x", byte);
  }
}

void UPDIFlasher::sendBreak()
{
  FastGPIO::writeLow();
  // Example: double break duration
  Time::delayMicroseconds(calculateBitDuration() * 24);
  FastGPIO::writeHigh();
}

void UPDIFlasher::sendSynch()
{
  // Synchronization character (0x55) as per UPDI specification
  writeByte(0x55);
}

void UPDIFlasher::sendUPDICommand(const std::vector<uint8_t> &command)
{
  for (auto byte : command) {
    writeByte(byte);
  }
}

std::vector<uint8_t> UPDIFlasher::readBytes(size_t count)
{
  std::vector<uint8_t> data;
  for (size_t i = 0; i < count; ++i) {
    data.push_back(readByte());
  }
  return data;
}

void UPDIFlasher::writeByte(uint8_t byte)
{
  writeBit(false); // Start bit

  uint8_t parity = 0;
  for (int i = 0; i < 8; ++i) {
    bool bit = (byte >> i) & 0x01;
    writeBit(bit);
    parity ^= bit; // Even parity calculation
  }

  writeBit(parity == 0); // Parity bit
  writeBit(true); // Stop bits (S1)
  writeBit(true); // Stop bits (S2)
}

void UPDIFlasher::writeBit(bool bit)
{
  FastGPIO::writeLow(); // Ensure start bit condition for a brief period
  Time::delayMicroseconds(calculateBitDuration() / 2); // Short delay for start bit setup
  FastGPIO::write(bit); // Write the actual bit
  Time::delayMicroseconds(calculateBitDuration());
  FastGPIO::writeHigh(); // Ensure line goes back high before the next start bit
}

uint8_t UPDIFlasher::readByte()
{
  uint8_t byte = 0;
  for (int i = 0; i < 8; ++i) {
    if (FastGPIO::read()) {
      byte |= (1 << i);
    }
    Time::delayMicroseconds(calculateBitDuration());
  }
  return byte;
}

unsigned int UPDIFlasher::calculateBitDuration()
{
  // Adjust this method based on your target UPDI clock speed and baud rate
  // Example for 225 kbps (default UPDI clock of 4 MHz)
  return 1000000 / 225000; // Calculate duration in microseconds
}

#endif