#include "UPDI.h"

#include "../VortexConfig.h"

#ifdef VORTEX_EMBEDDED
#include <Arduino.h>
#else
#define HIGH 0x1
#define LOW 0x0
#endif

#define UPDI_ASI_RESET_REQ 0x08
#define UPDI_RESET_REQ_VALUE 0x59
#define UPDI_ASI_SYS_STATUS 0x0B
#define UPDI_ASI_SYS_STATUS_NVMPROG 3

#include <string.h>

#include "../Time/TimeControl.h"
#include "../Log/Log.h"

#define READBACK

UPDI::UPDI(uint8_t txPin, uint8_t rxPin) : m_txPin(txPin), m_rxPin(rxPin), m_bufferIndex(0), m_updiSerial(1){
#ifdef VORTEX_EMBEDDED
  //pinMode(m_txPin, OUTPUT);
  //pinMode(m_rxPin, INPUT);
#endif

  // Initialize the buffer array
  memset(m_buffer, 0, sizeof(m_buffer));
}

uint8_t UPDI::updiReadWait()
{
  uint8_t b = 255;
  uint8_t counter = 0;

  // try to wait for data
  while (++counter) {
    b = m_updiSerial.read();
    if (b >= 0) {
      break;
    }
    Time::delayMilliseconds(1);
  }
  return b;
}

bool UPDI::updiSend(const uint8_t *buf, uint16_t size)
{
  /*
    NOTE: since the TX and RX pins are tied together,
    everything we send gets echo'd and needs to be
    discarded QUICKLY.
  */
  bool good_echo = true;
  uint16_t count = 0;
  uint8_t data = 0;

  m_updiSerial.flush();

  // write all data in one shot and then
  // read back the echo
  // this method requires a serial RX buffer as large as the largest possible TX block of data
  // it is possible to check the process but the larger the block, the faster

  count = m_updiSerial.write(buf, size);
  if (count != size) {
    INFO_LOGF("UpdiSerial send count error %d != %d\n", count, size);
  }
  Time::delayMilliseconds(2);
  count = 0;
  for (uint16_t i = 0; i < size; i++) {
    data = updiReadWait();
    if (data != buf[i]) {
      good_echo = false;
      INFO_LOGF("send[%d] %02x != %02x\n", i, buf[i], data);
    } else {
    }
    count++;
  }
  if (count != size) {
    INFO_LOGF("UPDISERIAL echo count error %d != %d\n", count, size);
    return false;
  }
  return good_echo;
}

void UPDI::sendByte(uint8_t b)
{
  //m_updiSerial.write(b);
  // Ensure pin is in OUTPUT mode for sending
  //pinMode(m_txPin, OUTPUT);
  //m_updiSerial.setPins(-1, m_txPin);

  m_updiSerial.write(b);
  m_updiSerial.flush(); // Ensure data is transmitted

  // Insert guard time here (considering 9600 baud rate)
  //delayMicroseconds(2000); // Example: Adjust based on actual needs

  // Switch to INPUT to listen for echo/response
  //pinMode(m_txPin, INPUT);
}

//void UPDI::sendByte(uint8_t byte)
//{
//  // Example function to send a byte using manual bit-banging on ESP32
//  // Configure the pin as output
//  gpio_set_direction(m_txPin, GPIO_MODE_OUTPUT);
//
//  // Send start bit (low)
//  gpio_set_level(m_txPin, 0);
//  delayBitTime();
//
//  uint8_t parity = 0;
//  // Send data bits
//  for (uint8_t mask = 0x01; mask != 0; mask <<= 1) {
//    bool bit = ((byte & mask) != 0);
//    gpio_set_level(m_txPin, bit);
//    delayBitTime(); // Function to wait for one bit time
//    parity ^= bit;
//  }
//
//  // Send parity bit
//  gpio_set_level(m_txPin, parity);
//  delayBitTime();
//
//  // Send stop bit(s) (high)
//  gpio_set_level(m_txPin, 1);
//  delayBitTime(); // Might need to adjust timing for stop bits
//
//  // Switch back to input mode
//  gpio_set_direction(m_txPin, GPIO_MODE_INPUT);
//}

uint8_t UPDI::receiveByte()
{
  //while (!m_updiSerial.available()) {
  //  // Optionally include a timeout to prevent infinite waiting
  //}
  //return m_updiSerial.read();
  // Ensure the pin is in INPUT mode; might be redundant if already set after send

  //m_updiSerial.setPins(m_txPin, -1);

  //while (!m_updiSerial.available()) {
  //  // Timeout implementation to avoid infinite loop
  //}
  return m_updiSerial.read();
  //while (!Serial1.available()) {
  //  // Optionally include a timeout to prevent infinite waiting
  //}
  //return Serial1.read();
}

void UPDI::reset(bool apply_reset)
{
  if (apply_reset) {
    // Apply reset
    sendStcsInstruction(UPDI_ASI_RESET_REQ, UPDI_RESET_REQ_VALUE);
  } else {
    // Release reset
    sendStcsInstruction(UPDI_ASI_RESET_REQ, 0x00);
  }
}

bool UPDI::inProgMode(void)
{
  //Checks whether the NVM PROG flag is up
  if (sendLdcsInstruction(UPDI_ASI_SYS_STATUS) & (1 << UPDI_ASI_SYS_STATUS_NVMPROG)) {
    return true;
  }
  return false;
}

#define UPDI_KEY_NVM        "NVMProg "
#define KEY_LEN             8

// Example function added to the UPDI class to enter programming mode and perform initial operations
void UPDI::enterProgrammingMode()
{
  // 0x00 is a placeholder for the correct Control/Status register, 0x59 to enable UPDI
  // Enable programming mode
  //sendStcsInstruction(0x00, 0x59);

#ifdef VORTEX_EMBEDDED
  INFO_LOG("pinmode tx");
  pinMode(m_txPin, OUTPUT);
  INFO_LOG("pinmode rx");
  pinMode(m_rxPin, OUTPUT);
  INFO_LOG("tx high");
  digitalWrite(m_txPin, HIGH);
  INFO_LOG("rx high");
  digitalWrite(m_rxPin, HIGH);
  INFO_LOG("delay");
  Time::delayMilliseconds(1);
  //pinMode(m_rxPin, INPUT);
#endif

  INFO_LOG("Held line high");

  m_updiSerial.setRxBufferSize(512 + 16);
  m_updiSerial.setTimeout(50);
  m_updiSerial.begin(115200, SERIAL_8E2, m_rxPin, m_txPin); // Initialize SoftwareSerial with UPDI baud rate

  while (!m_updiSerial); // wait for serial attach

  INFO_LOG("Began serial");

  // idk this seems to get stuck but it's in portaprog
  // while (!m_updiSerial);

  //Serial1.setRxBufferSize(512 + 16);
  //Serial1.setTimeout(50);
  //Serial1.begin(115200, SERIAL_8E2, m_rxPin, m_txPin);

  INFO_LOG("Sending break");

  sendBreakFrame();

  INFO_LOG("Sent break");

  #define UPDI_CS_CTRLA 0x02
  #define UPDI_CS_CTRLB 0x03
  #define UPDI_CTRLB_CCDETDIS_BIT 3
  #define UPDI_CTRLB_UPDIDIS_BIT 2
  #define UPDI_CTRLA_IBDLY_BIT 7
  sendStcsInstruction(UPDI_CS_CTRLB, 1 << UPDI_CTRLB_CCDETDIS_BIT);
  //sendStcsInstruction(UPDI_CS_CTRLB, (1 << UPDI_CTRLB_UPDIDIS_BIT) | (1 << UPDI_CTRLB_CCDETDIS_BIT));
  sendStcsInstruction(UPDI_CS_CTRLA, 1 << UPDI_CTRLA_IBDLY_BIT);

  INFO_LOG("Sent stcs instruction");

  Time::delayMilliseconds(1);

  // Send the NVM Programming key
  // This example uses a fixed key for demonstration; replace with actual key for your target
  sendKey(NVMPROG);

  INFO_LOG("Sent key instruction");

  reset(true);
  Time::delayMilliseconds(5);
  reset(false);

  INFO_LOG("Reset");

  Time::delayMilliseconds(5);
}

void UPDI::initializeUPDICommunication() {

}

void UPDI::readEEPROMAndUserRow() {
  // Assuming sizes and starting addresses are known
  uint16_t userRowSize = 128; // Example size
  uint32_t userRowStartAddress = 0x1300;
  uint16_t eepromSize = 256; // Example size
  uint32_t eepromStartAddress = 0x1400;

  // Read EEPROM
  for (uint16_t i = 0; i < eepromSize; i++) {
    uint8_t byte = sendLdsInstruction(eepromStartAddress + i, 1);
    INFO_LOGF("eeprom byte [%u]: %x", i, byte);
  }
  // Read USERROW
  for (uint16_t i = 0; i < userRowSize; i++) {
    uint8_t byte = sendLdsInstruction(userRowStartAddress + i, 1);
    INFO_LOGF("userrow byte [%u]: %x", i, byte);
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

const uint8_t key_chip_erase[] = {0x65, 0x73, 0x61, 0x72, 0x45, 0x4D, 0x56, 0x4E};
const uint8_t key_nvm_prog[] = {0x20, 0x67, 0x6F, 0x72, 0x50, 0x4D, 0x56, 0x4E};
const uint8_t key_write_userrow[] = {0x65, 0x74, 0x26, 0x73, 0x55, 0x4D, 0x56, 0x4E};

void UPDI::sendKey(KeyType keyType) {
  const uint8_t *key = nullptr;
  switch (keyType) {
  case CHIP_ERASE:
    key = key_chip_erase;
    break;
  case NVMPROG:
    key = key_nvm_prog;
    break;
  case USERROW_WRITE:
    key = key_write_userrow;
    break;
  default:
    ERROR_LOGF("Unknown key: %u", (uint8_t)keyType);
    return; // Invalid key type
  }
  sendKeyInstruction(key);
}

uint8_t UPDI::sendLdsInstruction(uint32_t address, uint8_t addressSize) {
  // Reset command buffer
  m_bufferIndex = 0;

  // Buffer the LDS instruction, address size, and the address itself
  sendByte(0x55); // synch character
  sendByte(0x00); // LDS opcode placeholder
  sendByte(addressSize);
  for (int i = 0; i < addressSize; i++) {
    sendByte((address >> (8 * i)) & 0xFF);
  }

#ifdef READBACK
  Time::delayMilliseconds(2);
  receiveByte();
  receiveByte();
  receiveByte();
  for (int i = 0; i < addressSize; i++) {
    receiveByte();
  }
#endif

  // Immediately read back the data
  return receiveByte();
}

void UPDI::sendStsInstruction(uint32_t address, uint8_t addressSize, uint8_t data) {
  m_bufferIndex = 0;

  sendByte(0x55); // synch character
  sendByte(0x40); // STS opcode placeholder
  sendByte(addressSize);
  for (int i = 0; i < addressSize; i++) {
    sendByte((address >> (8 * i)) & 0xFF);
  }
  sendByte(data);

#ifdef READBACK
  Time::delayMilliseconds(2);
  receiveByte();
  receiveByte();
  receiveByte();
  for (int i = 0; i < addressSize; i++) {
    receiveByte();
  }
  receiveByte();
#endif
}

uint8_t UPDI::sendLdcsInstruction(uint8_t csAddress) {
  m_bufferIndex = 0;

  sendByte(0x55); // synch character
  sendByte(0x04); // LDCS opcode placeholder
  sendByte(csAddress);

#ifdef READBACK
  Time::delayMilliseconds(2);
  receiveByte();
  receiveByte();
  receiveByte();
#endif

  // Immediately read back the data
  return receiveByte();
}

void UPDI::sendStcsInstruction(uint8_t csAddress, uint8_t data) {
  m_bufferIndex = 0;

  sendByte(0x55); // synch character
  sendByte(0xC0); // STCS opcode placeholder
  sendByte(csAddress);
  sendByte(data);

#ifdef READBACK
  Time::delayMilliseconds(2);
  receiveByte();
  receiveByte();
  receiveByte();
  receiveByte();
#endif
}

void UPDI::sendKeyInstruction(const uint8_t *key) {
  // KEY instruction is prefixed with 0xE0 followed by the 8-byte key
  m_bufferIndex = 0; // Reset the buffer before building the command

  sendByte(0x55); // synch character
  sendByte(0xE0); // Key command opcode (example, adjust as needed)
  for (int i = 0; i < 8; i++) {
    sendByte(key[i] & 0xFF);
  }

#ifdef READBACK
  Time::delayMilliseconds(2);
  receiveByte();
  receiveByte();
  for (int i = 0; i < 8; i++) {
    receiveByte();
  }
#endif
}

void UPDI::sendBreakFrame() {
  sendByte(0x00);
#ifdef READBACK
  Time::delayMilliseconds(2);
  receiveByte();
#endif
}
