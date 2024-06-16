#ifndef UPDI_H
#define UPDI_H

#include <inttypes.h>

class ByteStream;

class UPDI {
public:
  static bool init();
  static void cleanup();

  // read the duo save header over updi
  static bool readHeader(ByteStream &headerBuffer);
  static bool readMode(uint8_t idx, ByteStream &modeBuffer);

  static bool eraseMemory();
  static bool readMemory();
  static bool writeMemory();

private:
  enum cmnd
  {
    NVM_CMD_NOP,      // Does nothing
    NVM_CMD_WP,       // Write page buffer to memory
    NVM_CMD_ER,       // Erase page
    NVM_CMD_ERWP,     // Erase and write page
    NVM_CMD_PBC,      // Page buffer clear
    NVM_CMD_CHER,     // Chip erase: erase Flash and EEPROM
    NVM_CMD_EEER,     // EEPROM Erase
    NVM_CMD_WFU       // Write fuse
  };

  enum cs_reg : uint8_t
  {
    Status_A, Status_B, Control_A, Control_B,
    Reg_4, Reg_5, Reg_6, ASI_Key_Status,
    ASI_Reset_Request, ASI_Control_A, ASI_System_Control_A, ASI_System_Status,
    ASI_CRC_Status, Reg_13, Reg_14, Reg_15
  };

  static bool enterProgrammingMode();
  static void resetOn();
  static bool resetOff();
  static bool reset();

  static void sendByte(uint8_t byte);
  static uint8_t receiveByte();
  static void sendBreak();
  static void sendDoubleBreak();
  static void sendKey(const char *key);

  static uint8_t ldcs(cs_reg r);
  static void stcs(cs_reg r, uint8_t data);
  static void stptr_p(const uint8_t *addr_p, uint8_t n);
  static void rep(uint8_t repeats);
  static uint8_t ldinc_b();
  static uint8_t lds_b(uint16_t address);
  static uint8_t ld_b();
  static void sts_b(uint16_t address, uint8_t data);

  template <uint8_t mask = 0xFF>
  static uint8_t cpu_mode()
  {
    uint8_t mode = ldcs(ASI_System_Status);
    return mode & mask;
  }

  static void sendEraseKey() { sendKey("NVMErase"); }
  static void sendProgKey() { sendKey("NVMProg "); }
  static void sendUserrowKey() { sendKey("NVMUs&te"); }
};

#endif // UPDI_H
