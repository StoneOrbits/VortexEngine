#ifndef UPDI_H
#define UPDI_H

#include <inttypes.h>

#include "../VortexConfig.h"


class ByteStream;

class UPDI {
public:
  static bool init();
  static void cleanup();

  // read the duo save header over updi
  static bool readHeader(ByteStream &headerBuffer);
  static bool readMode(uint8_t idx, ByteStream &modeBuffer);

  static bool writeHeader(ByteStream &headerBuffer);
  static bool writeMode(uint8_t idx, ByteStream &modeBuffer);

  static bool eraseMemory();
  static bool readMemory();
  static bool writeMemory();

private:
#ifdef VORTEX_EMBEDDED
  // *** Base Addresses ***
  enum base
  {
    NVM_base = 0x1000,       /* Base address of the NVM controller */
    Sig_base = 0x1100,       /* Base address of the signature */
    Fuse_base = 0x1280,       /* Base address of the fuses */
    User_base = 0x1300,       /* Base address of the User Row EEPROM */
    EEPROM_base = 0x1400        /* Base address of the main EEPROM */
  };

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

  // *** NVM Registers (offsets from NVN_base are enum default values) ***
  enum nvm_reg
  {
    NVM_CTRLA,
    NVM_CTRLB,
    NVM_STATUS,
    NVM_INTCTRL,
    NVM_INTFLAGS,
    NVM_Reg_5,
    NVM_DATA_lo,
    NVM_DATA_hi,
    NVM_ADDR_lo,
    NVM_ADDR_hi
  };

  // *** NVM Commands (write to CTRLA to execute) ***
  enum nvm_cmd
  {
    NVM_NOP,      /* Does nothing */
    NVM_WP,       /* Write page buffer to memory */
    NVM_ER,       /* Erase page */
    NVM_ERWP,     /* Erase and write page */
    NVM_PBC,      /* Page buffer clear */
    NVM_CHER,     /* Chip erase: erase Flash and EEPROM */
    NVM_EEER,     /* EEPROM Erase */
    NVM_WFU       /* Write fuse */
  };

  static void enterProgrammingMode();

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

  static void nvmCmd(nvm_cmd cmd) { UPDI::sts_b(NVM_base + NVM_CTRLA, cmd); }
  static void nvmWait() { while (UPDI::lds_b(NVM_base + NVM_STATUS) & 0x03); }

  template <uint8_t mask = 0xFF>
  static uint8_t cpu_mode()
  {
    uint8_t mode = ldcs(ASI_System_Status);
    return mode & mask;
  }

  static void sendEraseKey() { sendKey("NVMErase"); }
  static void sendProgKey() { sendKey("NVMProg "); }
  static void sendUserrowKey() { sendKey("NVMUs&te"); }
#endif
};


#endif // UPDI_H
