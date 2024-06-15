#ifndef UPDI_H
#define UPDI_H

#include <inttypes.h>

enum cs_reg : uint8_t
{
  Status_A, Status_B, Control_A, Control_B,
  Reg_4, Reg_5, Reg_6, ASI_Key_Status,
  ASI_Reset_Request, ASI_Control_A, ASI_System_Control_A, ASI_System_Status,
  ASI_CRC_Status, Reg_13, Reg_14, Reg_15
};

class UPDI
{
public:
  UPDI(uint8_t txPin, uint8_t rxPin);
  void initializeUPDICommunication();
  void enterProgrammingMode();
  void eraseMemory();
  void readMemory();

  uint8_t ldcs(cs_reg r);
  void stcs(cs_reg r, uint8_t data);

  void send(const uint8_t *data, uint32_t len);
  uint32_t receive(uint8_t *buffer, uint32_t len, uint32_t timeout);
  void sendDoubleBreak();
  void sendEraseKey() { sendKey(chipEraseKey); }
  void sendProgKey() { sendKey(nvmProgKey); }

  void resetOn();
  bool resetOff();
  bool reset();

private:
  void sendByte(uint8_t byte);
  uint8_t receiveByte();
  void sendBreak();
  void sendKey(const uint8_t *key);
  void stptr_p(const uint8_t *addr_p, uint8_t n);
  void rep(uint8_t repeats);
  uint8_t ldinc_b();
  uint8_t lds_b(uint16_t address);
  uint8_t ld_b();

  // keys for programming
  const uint8_t nvmProgKey[8] = { 0x20, 0x67, 0x6F, 0x72, 0x50, 0x4D, 0x56, 0x4E };
  const uint8_t chipEraseKey[8] = { 0x65, 0x73, 0x61, 0x72, 0x45, 0x4D, 0x56, 0x4E };

  enum cmnd
  {
    NVM_CMD_NOP,      /* Does nothing */
    NVM_CMD_WP,       /* Write page buffer to memory */
    NVM_CMD_ER,       /* Erase page */
    NVM_CMD_ERWP,     /* Erase and write page */
    NVM_CMD_PBC,      /* Page buffer clear */
    NVM_CMD_CHER,     /* Chip erase: erase Flash and EEPROM */
    NVM_CMD_EEER,     /* EEPROM Erase */
    NVM_CMD_WFU       /* Write fuse */
  };

  void sts_b(uint16_t address, uint8_t data);

  template <uint8_t mask = 0xFF>
  uint8_t cpu_mode()
  {
    uint8_t mode = ldcs(ASI_System_Status);
    return mode & mask;
  }

  uint8_t m_txPin;
  uint8_t m_rxPin;
};

#endif // UPDI_H
