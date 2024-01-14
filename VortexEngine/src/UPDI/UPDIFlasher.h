#pragma once

#ifdef VORTEX_EMBEDDED

#include <Arduino.h>

class UPDIFlasher
{
public:
  UPDIFlasher(int updiPin) : m_updiPin(updiPin)
  {
    pinMode(m_updiPin, OUTPUT);
  }

  void init()
  {
    // Initialize UPDI communication, if needed
  }

  void flashFirmware(const uint8_t *firmware, size_t size)
  {
    // Example function to flash firmware to the target device
    // Iterate through the firmware bytes and write them using UPDI commands
    for (size_t i = 0; i < size; ++i) {
      writeByte(firmware[i]);
    }
  }

private:
  int m_updiPin;

  void writeByte(uint8_t byte)
  {
    // Implement the UPDI write byte command
    // This involves bit-banging the UPDI protocol on the m_updiPin
    for (int i = 0; i < 8; ++i) {
      writeBit((byte >> i) & 0x01);
    }
    // Add parity and stop bits if required by UPDI protocol
  }

  void writeBit(bool bit)
  {
    digitalWrite(m_updiPin, bit ? HIGH : LOW);
    delayMicroseconds(calculateBitDuration()); // Match this to the desired baud rate
  }

  uint8_t readByte()
  {
    // Implement the UPDI read byte command
    uint8_t byte = 0;
    for (int i = 0; i < 8; ++i) {
      byte |= (readBit() << i);
    }
    return byte;
  }

  bool readBit()
  {
    pinMode(m_updiPin, INPUT);
    delayMicroseconds(calculateBitDuration() / 2); // Sample in the middle of the bit duration
    bool bit = digitalRead(m_updiPin);
    pinMode(m_updiPin, OUTPUT);
    delayMicroseconds(calculateBitDuration() / 2);
    return bit;
  }

  unsigned int calculateBitDuration()
  {
    // Calculate and return the duration of one bit based on the UPDI baud rate
    // For example, for 9600 baud rate, bit duration is 1 / 9600 seconds
    return 1000000 / 9600; // Microseconds for a baud rate of 9600
  }
};

#endif
