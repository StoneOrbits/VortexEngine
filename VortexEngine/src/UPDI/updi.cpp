#include "UPDI.h"

#include "../VortexConfig.h"

#include <string.h>

#include "../Time/TimeControl.h"
#include "../Log/Log.h"

#include "../Serial/ByteStream.h"

#include "driver/uart.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"
#include "soc/uart_periph.h"
#include "esp_rom_gpio.h"
#include "hal/uart_types.h"

#ifdef VORTEX_EMBEDDED
#include <Arduino.h>
#else
#define HIGH 0x1
#define LOW 0x0
#endif

#define NVM_base 0x1000   /* Base address of the NVM controller */

#define UPDI_BREAK             0x00

#define UPDI_LDS            0x00
#define UPDI_STS            0x40
#define UPDI_LD                0x20
#define UPDI_ST                0x60
#define UPDI_LDCS            0x80
#define UPDI_STCS            0xC0
#define UPDI_REPEAT         0xA0
#define UPDI_KEY            0xE0

#define UPDI_PTR             0x00
#define UPDI_PTR_INC         0x04
#define UPDI_PTR_ADDRESS     0x08

#define UPDI_ADDRESS_8        0x00
#define UPDI_ADDRESS_16     0x04
#define UPDI_ADDRESS_24     0x08

#define UPDI_DATA_8             0x00
#define UPDI_DATA_16         0x01
#define UPDI_DATA_24         0x02

#define UPDI_KEY_SIB         0x04
#define UPDI_KEY_KEY         0x00

#define UPDI_KEY_64             0x00
#define UPDI_KEY_128         0x01
#define UPDI_KEY_256         0x02

#define UPDI_SIB_8BYTES         UPDI_KEY_64
#define UPDI_SIB_16BYTES     UPDI_KEY_128
#define UPDI_SIB_32BYTES     UPDI_KEY_256

#define UPDI_REPEAT_BYTE     0x00
#define UPDI_REPEAT_WORD     0x01

#define UPDI_PHY_SYNC         0x55
#define UPDI_PHY_ACK          0x40

#define UPDI_MAX_REPEAT     0x1FF    // was 0xFF but it looks like there are now chips with a page size of 512

//CS and ASI Register Address map
#define UPDI_CS_STATUSA        0x00
#define UPDI_CS_STATUSB        0x01
#define UPDI_CS_CTRLA        0x02
#define UPDI_CS_CTRLB        0x03
#define UPDI_ASI_KEY_STATUS 0x07
#define UPDI_ASI_RESET_REQ    0x08
#define UPDI_ASI_CTRLA        0x09
#define UPDI_ASI_SYS_CTRLA    0x0A
#define UPDI_ASI_SYS_STATUS 0x0B
#define UPDI_ASI_CRC_STATUS 0x0C

#define UPDI_CTRLA_IBDLY_BIT    7
#define UPDI_CTRLA_RSD_BIT        3
#define UPDI_CTRLB_CCDETDIS_BIT 3
#define UPDI_CTRLB_UPDIDIS_BIT    2

#define UPDI_KEY_NVM           "NVMProg "
#define UPDI_KEY_CHIPERASE       "NVMErase"
#define UPDI_KEY_USERROW_WRITE "NVMUs&te"


#define UPDI_ASI_STATUSA_REVID 4
#define UPDI_ASI_STATUSB_PESIG 0

#define UPDI_ASI_KEY_STATUS_CHIPERASE    3
#define UPDI_ASI_KEY_STATUS_NVMPROG        4
#define UPDI_ASI_KEY_STATUS_UROWWRITE    5

#define UPDI_ASI_SYS_STATUS_RSTSYS        5
#define UPDI_ASI_SYS_STATUS_INSLEEP        4
#define UPDI_ASI_SYS_STATUS_NVMPROG        3
#define UPDI_ASI_SYS_STATUS_UROWPROG    2
#define UPDI_ASI_SYS_STATUS_LOCKSTATUS    0

#define UPDI_ASI_SYS_CTRLA_UROWupdi_write_FINAL 1
#define UPDI_ASI_SYS_CTRLA_CLKREQ                0

#define UPDI_RESET_REQ_VALUE 0x59

//FLASH CONTROLLER
#define UPDI_NVMCTRL_CTRLA      0x00
#define UPDI_NVMCTRL_CTRLB      0x01
#define UPDI_NVMCTRL_STATUS      0x02
#define UPDI_NVMCTRL_INTCTRL  0x03
#define UPDI_NVMCTRL_INTFLAGS 0x04
#define UPDI_NVMCTRL_DATAL      0x06
#define UPDI_NVMCTRL_DATAH      0x07
#define UPDI_NVMCTRL_ADDRL      0x08
#define UPDI_NVMCTRL_ADDRH      0x09
#define UPDI_NVMCTRL_ADDRX      0x0A

//CTRLA V1
#define UPDI_NVM0CTRL_CTRLA_NOP                    0x00
#define UPDI_NVM0CTRL_CTRLA_WRITE_PAGE            0x01
#define UPDI_NVM0CTRL_CTRLA_ERASE_PAGE            0x02
#define UPDI_NVM0CTRL_CTRLA_ERASEWRITE_PAGE        0x03    // use for EEPROM
#define UPDI_NVM0CTRL_CTRLA_PAGE_BUFFER_CLR        0x04    // sue before write
#define UPDI_NVM0CTRL_CTRLA_ERASE_CHIP            0x05
#define UPDI_NVM0CTRL_CTRLA_ERASE_EEPROM        0x06

#define UPDI_NVMCTRL_CTRLA_UPDI_WRITE_FUSE        0x07

//CTRLA V2
#define UPDI_NVM2CTRL_CTRLA_NOCMD                0x00
#define UPDI_NVM2CTRL_CTRLA_FLASH_WRITE            0x02
#define UPDI_NVM2CTRL_CTRLA_FLASH_PAGE_ERASE    0x08
#define UPDI_NVM2CTRL_CTRLA_EEPROM_ERASE_WRITE    0x13
#define UPDI_NVM2CTRL_CTRLA_CHIP_ERASE            0x20
#define UPDI_NVM2CTRL_CTRLA_EEPROM_ERASE        0x30

//CTRLA V3
#define UPDI_NVM3CTRL_CTRLA_NOCMD                        0x00
#define UPDI_NVM3CTRL_CTRLA_NOOP                        0x01
#define UPDI_NVM3CTRL_CTRLA_FLASH_PAGE_WRITE            0x04
#define UPDI_NVM3CTRL_CTRLA_FLASH_PAGE_ERASE_WRITE        0x05
#define UPDI_NVM3CTRL_CTRLA_FLASH_PAGE_ERASE            0x08
#define UPDI_NVM3CTRL_CTRLA_FLASH_PAGE_BUFFER_CLEAR        0x0F
#define UPDI_NVM3CTRL_CTRLA_EEPROM_PAGE_WRITE            0x14
#define UPDI_NVM3CTRL_CTRLA_EEPROM_PAGE_ERASE_WRITE        0x15
#define UPDI_NVM3CTRL_CTRLA_EEPROM_PAGE_ERASE            0x17
#define UPDI_NVM3CTRL_CTRLA_EEPROM_PAGE_BUFFER_CLEAR    0x1F
#define UPDI_NVM3CTRL_CTRLA_CHIP_ERASE                    0x20
#define UPDI_NVM3CTRL_CTRLA_EEPROM_ERASE                0x30


#define UPDI_NVM_STATUS_UPDI_WRITE_ERROR 2
#define UPDI_NVM_STATUS_EEPROM_BUSY         1
#define UPDI_NVM_STATUS_FLASH_BUSY         0

#define UPDI_MAX_FILENAME_LEN 256

#define AVR_SYSCFG_ADDRESS    0x0F00
#define AVR_OCD_ADDRESS        0x0F80
#define AVR_NVM_ADDRESS        0x1000
#define AVR_SIG_ADDRESS        0x1100
#define AVR_EEPROM_ADDRESS    0x1400
#define AVR_FLASH_ADDR          // varies with chip size
#define AVR_UID_LENGTH          16 // was 10 but NVM2 has a 16 byte signature

// FUSE definitions
#define AVR_FUSE_UNINITIALIZED 0xFF    // TODO need to find a value that is guaranteed to never be used
#define AVR_NUM_FUSES     11 // 3 fuses are marked 'reserved': the 4th, 5th, and 10th (0-based #3, #4, and #9)

//#define AVR_FUSE_BASE     0x1280
#define AVR_FUSE_WDTCFG     0x0 // watch dog timer
#define AVR_FUSE_BODCFG     0x1 // brown out detection
#define AVR_FUSE_OSCCFG     0x2 // oscillator frequency
#define AVR_FUSE_RESV3     0x3 // must be 0xFF
#define AVR_FUSE_RESV4     0x4
#define AVR_FUSE_SYSCFG0 0x5
#define AVR_FUSE_SYSCFG1 0x6
#define AVR_FUSE_APPEND     0x7
#define AVR_FUSE_BOOTEND 0x8
#define AVR_FUSE_RESV9     0x9
#define AVR_FUSE_LOCK     0xA

#define UPDI_UART_NUM UART_NUM_1  // Use UART0 (you can choose the appropriate UART number)

#define READBACK

#define UPDI_KEY_NVM        "NVMProg "
#define KEY_LEN             8

#define UPDI_TX_PIN GPIO_NUM_8
#define UPDI_RX_PIN GPIO_NUM_8

#define UPDI_BAUD 19200

#if 0

// Configure GPIO for output
#define GPIO_SET_OUTPUT(gpio_num) (GPIO.enable_w1ts.val = (1U << gpio_num))

// Configure GPIO for input
#define GPIO_SET_INPUT(gpio_num) (GPIO.enable_w1tc.val = (1U << gpio_num))

#define GPIO_SET_HIGH(gpio_num) (GPIO.out_w1ts.val = (1U << gpio_num)); __asm__ __volatile__("nop;nop;nop;nop;nop;nop;nop;")
#define GPIO_SET_LOW(gpio_num) (GPIO.out_w1tc.val = (1U << gpio_num)); __asm__ __volatile__("nop;nop;nop;nop;nop;nop;nop;");
#define GPIO_READ(gpio_num) ((GPIO.in.val >> gpio_num) & 0x1)



UPDI::UPDI(uint8_t txPin, uint8_t rxPin) : m_txPin(txPin), m_rxPin(rxPin), m_bufferIndex(0), m_updiSerial(1)
{
#ifdef VORTEX_EMBEDDED
  //pinMode(m_txPin, OUTPUT);
  //pinMode(m_rxPin, INPUT);
#endif

  // Initialize the buffer array
  memset(m_buffer, 0, sizeof(m_buffer));
}

int UPDI::updiReadWait()
{
  uint8_t counter = 0;
  //while (!m_updiSerial.available()) {
  //  // do nothing
  //}
  //return m_updiSerial.read();
  // try to wait for data

  // receive on tx line, set sender to the rx line
  //m_updiSerial.setPins(m_txPin, -1);

#if 0
  int b = -1;
  while (counter++ < 255) {
    b = m_updiSerial.read();
    if (b >= 0) {
      break;
    }
    Time::delayMicroseconds(300);
  }
#else
  uint8_t b = 0;
  size_t buffered_size;
  // Clear the UART buffer
  int len = uart_read_bytes(UPDI_UART_NUM, &b, 1, 50 / portTICK_PERIOD_MS); // Timeout in ticks
  if (len != 1) {
    return -1;
  }
#endif
  // swap lines back
  //m_updiSerial.setPins(-1, m_txPin);
  return b;
}

size_t UPDI::updiSendReceive(uint8_t *data, uint16_t size, uint8_t *buff, uint32_t len)
{
  if (!updiSend(data, size)) {
    INFO_LOG("something happened with serial_send");
    return 0;
  }
  uint32_t count = 0;
  bool timeout = false;
  // Clear the UART buffer
  int result = uart_read_bytes(UPDI_UART_NUM, buff, len, 50 / portTICK_PERIOD_MS); // Timeout in ticks
  if (len != result) {
    INFO_LOGF("timeout on receive received %u instead of %u", result, len);
    return 0;
  }

  //for (int32_t i = 0; i < len; i++) {
  //  b = updiReadWait();
  //  buff[count++] = b;
  //  if (b == -1) {
  //    timeout = true;
  //  }
  //}
  //if (count != len) {
  //  INFO_LOGF("Count is not length %u != %u", count, len);
  //  return 0;
  //}
  //if (timeout) {
  //  INFO_LOG("timeout on receive");
  //  return 0;
  //}
  return len;
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
  int data = 0;

  //GPIO_SET_OUTPUT(GPIO_NUM_8);

  // write all data in one shot and then
  // read back the echo
  // this method requires a serial RX buffer as large as the largest possible TX block of data
  // it is possible to check the process but the larger the block, the faster

#if 0
  m_updiSerial.flush();
  count = m_updiSerial.write(buf, size);
#else

  //gpio_set_direction(UPDI_TX_PIN, GPIO_MODE_OUTPUT_OD);
  //gpio_set_direction(UPDI_RX_PIN, GPIO_MODE_OUTPUT_OD);

  //esp_rom_gpio_connect_out_signal(UPDI_TX_PIN, UART_PERIPH_SIGNAL(UPDI_UART_NUM, SOC_UART_TX_PIN_IDX), false, false);

  // Write the character to the UART
  count = uart_write_bytes(UPDI_UART_NUM, buf, size);
  // Wait for UART transmission to complete
  uart_wait_tx_done(UPDI_UART_NUM, portMAX_DELAY);
#endif

  gpio_set_direction(UPDI_TX_PIN, GPIO_MODE_INPUT);
  //gpio_set_direction(UPDI_RX_PIN, GPIO_MODE_INPUT);
  esp_rom_gpio_connect_in_signal(UPDI_TX_PIN, UART_PERIPH_SIGNAL(UPDI_UART_NUM, SOC_UART_RX_PIN_IDX), false);

  return true;

  //updi_serial_setRX();
  // pretty sure I never send anything bigger than 32, lol
  uint8_t wasteBuf[32] = { 0 };
  // Clear the UART buffer
  int len = uart_read_bytes(UPDI_UART_NUM, wasteBuf, size, 50 / portTICK_PERIOD_MS); // Timeout in ticks
  if (len != count) {
    INFO_LOGF("UPDISERIAL echo count error %d != %d", count, len);
    return false;
  }

  //GPIO_SET_INPUT(GPIO_NUM_8);
  //return count == size;


  //if (count != size) {
  //  INFO_LOGF("UpdiSerial send count error %d != %d", count, size);
  //}
  ////Time::delayMilliseconds(10);
  //count = 0;
  //for (uint16_t i = 0; i < size; i++) {
  //  data = updiReadWait();
  //  if (data != buf[i]) {
  //    good_echo = false;
  //    INFO_LOGF("Failure send[%d] %02x != %02x", i, buf[i], data);
  //  //} else {
  //  //  INFO_LOGF("Success send[%d] %02x == %02x", i, buf[i], data);
  //  }
  //  count++;
  //}
  //if (count != size) {
  //  INFO_LOGF("UPDISERIAL echo count error %d != %d", count, size);
  //  return false;
  //}
  return good_echo;
}

void UPDI::sendByte(uint8_t b)
{
  //m_updiSerial.write(b);
  // Ensure pin is in OUTPUT mode for sending
  //pinMode(m_txPin, OUTPUT);
  //m_updiSerial.setPins(-1, m_txPin);

  m_updiSerial.write(b);
  m_updiSerial.flush();

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

  //uint8_t _updi_serial_retry_counter = 0;
  //int b = -1;

  while (!m_updiSerial.available()) {
    // Timeout implementation to avoid infinite loop
  }
  return m_updiSerial.read();

  //while (++_updi_serial_retry_counter) {
  //  if ((b = m_updiSerial.read()) >= 0)
  //    break;
  //  Time::delayMilliseconds(1);
  //}
  //return b;
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


bool UPDI::check(void)
{
  // Check the STATUSA register to see if the updi revision
  if (checkCsRegisterSet(UPDI_CS_STATUSA, 0xF0)) {
    return true;
  }
  return false;
}

bool UPDI::inProgMode(void)
{
  //Checks whether the NVM PROG flag is up
  if (checkCsRegisterSet(UPDI_ASI_SYS_STATUS, 1 << UPDI_ASI_SYS_STATUS_NVMPROG)) {
    return true;
  }
  return false;
}

void UPDI::updi_serial_init()
{
#if 0
  pinMode(m_rxPin, OUTPUT);
  pinMode(m_txPin, OUTPUT);
  digitalWrite(m_rxPin, HIGH);
  digitalWrite(m_txPin, HIGH);
  delay(1);
  //pinMode(m_rxPin, INPUT);
  //pinMode(m_txPin, OUTPUT);
  size_t s = 0;
  s = m_updiSerial.setRxBufferSize(512 + 16);	// KLUDGE this should be based off of UPDI_MAX_REPEAT
  m_updiSerial.setTimeout(50);
  m_updiSerial.begin(225000, SERIAL_8E2, m_rxPin, m_txPin);
  //m_updiSerial.begin(115200, SERIAL_8E2, m_rxPin, m_txPin);
  while (!m_updiSerial); // wait for serial attach

  // Configure the tx as open-drain
  //gpio_set_pull_mode(GPIO_NUM_8, GPIO_PULLUP_ONLY);
  gpio_set_direction(GPIO_NUM_8, GPIO_MODE_OUTPUT_OD);

  //// configure rx as input
  //gpio_set_pull_mode(GPIO_NUM_2, GPIO_PULLUP_ONLY);
  //gpio_set_direction(GPIO_NUM_2, GPIO_MODE_INPUT);
#else

  static bool init = false;
  if (init) {
    uart_driver_delete(UPDI_UART_NUM);
  }
  init = true;

  // UART configuration
  uart_config_t uart_config = {
    .baud_rate = UPDI_BAUD,
    .data_bits = UART_DATA_8_BITS,
    .parity = UART_PARITY_EVEN,
    .stop_bits = UART_STOP_BITS_2,
    .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
    .rx_flow_ctrl_thresh = 0,
    .source_clk = UART_SCLK_RTC,
  };
  // gpio configuration
  //const gpio_config_t rxpin_cfg {
  //  .pin_bit_mask = (1ul << UPDI_RX_PIN),
  //  .mode = GPIO_MODE_INPUT, // Input and Output w/ open-drain!
  //  .pull_up_en = GPIO_PULLUP_ENABLE, // Open-drain requires a pull-up.
  //  .pull_down_en = GPIO_PULLDOWN_DISABLE,
  //  .intr_type = GPIO_INTR_DISABLE
  //};
  const gpio_config_t txpin_cfg = {
    .pin_bit_mask = (1ul << UPDI_TX_PIN) | (1ul << UPDI_RX_PIN),
    .mode = GPIO_MODE_INPUT_OUTPUT, // Input and Output w/ open-drain!
    .pull_up_en = GPIO_PULLUP_ENABLE, // Open-drain requires a pull-up.
    .pull_down_en = GPIO_PULLDOWN_DISABLE,
    .intr_type = GPIO_INTR_DISABLE
  };

  // Configure UART parameters
  uart_param_config(UPDI_UART_NUM, &uart_config);
  // Set UART pins
  uart_set_pin(UPDI_UART_NUM, UPDI_TX_PIN, UPDI_RX_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
  // Install UART driver
  uart_driver_install(UPDI_UART_NUM, 1024 * 2, 0, 0, NULL, 0);
  // Set UART read timeout
  uart_set_rx_timeout(UPDI_UART_NUM, 50);

  // now configure the gpios
  //gpio_config(&rxpin_cfg);
  gpio_config(&txpin_cfg);

  // and attach the listeners for uart
  esp_rom_gpio_connect_out_signal(UPDI_TX_PIN, UART_PERIPH_SIGNAL(UPDI_UART_NUM, SOC_UART_TX_PIN_IDX), false, false);

  //gpio_set_direction(UPDI_TX_PIN, GPIO_MODE_INPUT);
  //esp_rom_gpio_connect_in_signal(UPDI_TX_PIN, UART_PERIPH_SIGNAL(UPDI_UART_NUM, SOC_UART_RX_PIN_IDX), false);    
  //gpio_set_direction(UPDI_RX_PIN, GPIO_MODE_INPUT);

  //uint8_t address = 0xB;
  //uint8_t buf[] = { UPDI_PHY_SYNC, (uint8_t)(UPDI_LDCS | address) };
  //updiSend(buf, sizeof(buf));

  //updi_serial_setTX();
#endif
}


void UPDI::updi_serial_setTX()
{

  uart_driver_delete(UPDI_UART_NUM);

  // UART configuration
  uart_config_t uart_config = {
    .baud_rate = UPDI_BAUD,
    .data_bits = UART_DATA_8_BITS,
    .parity = UART_PARITY_EVEN,
    .stop_bits = UART_STOP_BITS_2,
    .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
    .rx_flow_ctrl_thresh = 0,
    .source_clk = UART_SCLK_RTC,
  };
  // gpio configuration
  const gpio_config_t pin_cfg = {
    .pin_bit_mask = (1ul << UPDI_TX_PIN),
    .mode = GPIO_MODE_OUTPUT, // Input and Output w/ open-drain!
    .pull_up_en = GPIO_PULLUP_ENABLE, // Open-drain requires a pull-up.
    .pull_down_en = GPIO_PULLDOWN_DISABLE,
    .intr_type = GPIO_INTR_DISABLE
  };

  // Configure UART parameters
  uart_param_config(UPDI_UART_NUM, &uart_config);
  // Set UART pins
  uart_set_pin(UPDI_UART_NUM, -1, UPDI_TX_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
  // Install UART driver
  uart_driver_install(UPDI_UART_NUM, 1024 * 2, 0, 0, NULL, 0);
  // Set UART read timeout
  uart_set_rx_timeout(UPDI_UART_NUM, 50);

  // now configure the gpios
  gpio_config(&pin_cfg);

  // and attach the listeners for uart
  esp_rom_gpio_connect_out_signal(UPDI_TX_PIN, UART_PERIPH_SIGNAL(UPDI_UART_NUM, SOC_UART_TX_PIN_IDX), false, false);
}

void UPDI::updi_serial_setRX()
{
  uart_driver_delete(UPDI_UART_NUM);

  // UART configuration
  uart_config_t uart_config = {
    .baud_rate = UPDI_BAUD,
    .data_bits = UART_DATA_8_BITS,
    .parity = UART_PARITY_EVEN,
    .stop_bits = UART_STOP_BITS_2,
    .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
    .rx_flow_ctrl_thresh = 0,
    .source_clk = UART_SCLK_RTC,
  };
  // gpio configuration
  const gpio_config_t pin_cfg = {
    .pin_bit_mask = (1ul << UPDI_TX_PIN),
    .mode = GPIO_MODE_INPUT, // Input and Output w/ open-drain!
    .pull_up_en = GPIO_PULLUP_ENABLE, // Open-drain requires a pull-up.
    .pull_down_en = GPIO_PULLDOWN_DISABLE,
    .intr_type = GPIO_INTR_DISABLE
  };

  // Configure UART parameters
  uart_param_config(UPDI_UART_NUM, &uart_config);
  // Set UART pins
  uart_set_pin(UPDI_UART_NUM, UPDI_TX_PIN, -1, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
  // Install UART driver
  uart_driver_install(UPDI_UART_NUM, 1024 * 2, 0, 0, NULL, 0);
  // Set UART read timeout
  uart_set_rx_timeout(UPDI_UART_NUM, 50);

  // now configure the gpios
  gpio_config(&pin_cfg);

  // and attach the listeners for uart
  esp_rom_gpio_connect_in_signal(UPDI_TX_PIN, UART_PERIPH_SIGNAL(UPDI_UART_NUM, SOC_UART_RX_PIN_IDX), false);
}

// Example function added to the UPDI class to enter programming mode and perform initial operations
void UPDI::enterProgrammingMode()
{
  // 0x00 is a placeholder for the correct Control/Status register, 0x59 to enable UPDI
  // Enable programming mode
  //sendStcsInstruction(0x00, 0x59);

#ifdef VORTEX_EMBEDDED

  //pinMode(m_rxPin, INPUT);
  //pinMode(m_txPin, OUTPUT);
  //digitalWrite(m_txPin, HIGH);
  //int result = digitalRead(m_rxPin);
  //INFO_LOGF("Test1: %d == %d", HIGH, result);
  //result = digitalWrite(m_txPin, LOW);
  //result = digitalRead(m_rxPin);
  //INFO_LOGF("Test2: %d == %d", LOW, result);
  //digitalWrite(m_txPin, HIGH);
  //result = digitalRead(m_rxPin);
  //INFO_LOGF("Test3: %d == %d", HIGH, result);
  //digitalWrite(m_txPin, HIGH);
  //result = digitalRead(m_rxPin);
  //INFO_LOGF("Test4: %d == %d", HIGH, result);
  //digitalWrite(m_txPin, LOW);
  //result = digitalRead(m_rxPin);
  //INFO_LOGF("Test5: %d == %d", LOW, result);


  //INFO_LOG("Beginning Updi Connection");
  //pinMode(m_rxPin, OUTPUT);
  //pinMode(m_txPin, OUTPUT);
  ////digitalWrite(m_rxPin, HIGH);
  ////digitalWrite(m_txPin, HIGH);
  ////Time::delayMilliseconds(1);

  //digitalWrite(m_rxPin, HIGH);
  //digitalWrite(m_txPin, HIGH);
  //delay(12);
  //// first BREAK
  //INFO_LOG("updi_serial Digital BREAK 1\n");
  //digitalWrite(m_rxPin, LOW);
  //digitalWrite(m_txPin, LOW);
  //delay(25);
  //digitalWrite(m_rxPin, HIGH);
  //digitalWrite(m_txPin, HIGH);
  //delay(12);
  //INFO_LOG("updi_serial Digital BREAK 2\n");
  //digitalWrite(m_rxPin, LOW);
  //digitalWrite(m_txPin, LOW);
  //delay(25);
  //// establish a known high state
  //digitalWrite(m_rxPin, HIGH);
  //digitalWrite(m_txPin, HIGH);

  //pinMode(m_rxPin, OUTPUT);
  //pinMode(m_txPin, OUTPUT);
  //digitalWrite(m_rxPin, HIGH);
  //digitalWrite(m_txPin, HIGH);
  //delay(1);
#endif

  uint32_t lastms = 0;
  uint32_t lastus = 0;
  uint32_t us = 0;
  uint32_t ms = 0;

  //us = micros();
  //ms = millis();
  //int initial = digitalRead(m_rxPin);
  //lastms = ms;
  //lastus = us;
  //INFO_LOGF("%u.%u (+%u.%u): 0x%x (Initial Value)", ms, us, ms - lastms, us - lastus, initial);

  //int change = 0;
  //while (2 + 2 != 5) {
  //  int val = digitalRead(m_rxPin);
  //  if (val != initial) {
  //    change++;
  //    us = micros();
  //    ms = millis();
  //    INFO_LOGF("%u.%u (+%u.%u): 0x%x (Changed Value)", ms, us, ms - lastms, us - lastus, val);
  //    lastms = ms;
  //    lastus = us;
  //  }
  //  if (change == 4) {
  //    break;
  //  }
  //}

  us = micros();
  ms = millis();
  lastms = ms;
  lastus = us;
  INFO_LOGF("%u.%u (+%u.%u): Starting Serial", ms, us, ms - lastms, us - lastus);

  //digitalWrite(m_txPin, LOW);
  //digitalWrite(m_rxPin, LOW);
  //Time::delayMicroseconds(10);
  //digitalWrite(m_txPin, HIGH);
  //digitalWrite(m_rxPin, HIGH);
  //Time::delayMilliseconds(10);

  //m_updiSerial.begin(115200, SERIAL_8N1, 16, 17);
  //m_updiSerial.setRxBufferSize(512 + 16);
  //m_updiSerial.setTimeout(50);
  //m_updiSerial.begin(115200, SERIAL_8E2, m_rxPin, m_rxPin); // Initialize SoftwareSerial with UPDI baud rate

  //while (!m_updiSerial); // wait for serial attach


  //us = micros();
  //ms = millis();
  //INFO_LOGF("%u.%u (+%u.%u): Serial Initialized", ms, us, ms - lastms, us - lastus);
  //lastms = ms;
  //lastus = us;

  //while (2 + 2 != 5) {
  //  //while (!m_updiSerial.available());
  //  //uint8_t byte = receiveByte();
  //  uint8_t byte = updiReadWait();
  //  us = micros();
  //  ms = millis();
  //  INFO_LOGF("%u.%u (+%u.%u): 0x%x", ms, us, ms - lastms, us - lastus, byte);
  //  lastms = ms;
  //  lastus = us;
  //}


  //us = micros();
  //ms = millis();
  //INFO_LOGF("%u.%u (+%u.%u): Sending Break", ms, us, ms - lastms, us - lastus);
  //lastms = ms;
  //lastus = us;



  updi_serial_init();
  //return;

  sendDoubleBreak();

  // flush
  //updiReadWait();

  //sendBreak();
  //sendBreak();

  //us = micros();
  //ms = millis();
  //INFO_LOGF("%u.%u (+%u.%u): Sent Break", ms, us, ms - lastms, us - lastus);
  //lastms = ms;
  //lastus = us;

  // toggle reset
    //sendBreak();
    //sendBreak();
      //us = micros();
      //ms = millis();
      //INFO_LOGF("%u.%u (+%u.%u): Cpu Mode: 0x%02x", ms, us, ms - lastms, us - lastus, md);
      //lastms = ms;
      //lastus = us;

    //sendBreak();
    // send synch
    //uint8_t synch = 0x55;
    //updiSend(&synch, 1);

    //uint8_t md = cpu_mode();
    //sendStcsInstruction(UPDI_CS_CTRLA, 0x6);
    //sendStcsInstruction(ASI_Reset_Request, 0x59);
    //sendStcsInstruction(ASI_Reset_Request, 0x00);

    //us = micros();
    //ms = millis();
    //INFO_LOGF("%u.%u (+%u.%u): Toggled reset", ms, us, ms - lastms, us - lastus);
    //lastms = ms;
    //lastus = us;

    //uint8_t buf2[] = { 0x55, 0xad, 0x83, 0x10, 0xad, 0x00 };
    //uint8_t outBuf[2] = {0};
    //updiSendReceive(buf2, sizeof(buf2), outBuf, 1);

    //sendStcsInstruction(UPDI_CS_CTRLB, 1 << UPDI_CTRLB_CCDETDIS_BIT);
    //sendStcsInstruction(UPDI_CS_CTRLA, 0x6);

    //us = micros();
    //ms = millis();
    //INFO_LOGF("%u.%u (+%u.%u): Sent STCS", ms, us, ms - lastms, us - lastus);
    //lastms = ms;
    //lastus = us;

    //uint8_t mode = cpu_mode<0xEF>();
    //if (mode != 0x92) {
    //  INFO_LOGF("Bad CPU_Mode: 0x%02x", mode);
    //} else {
    //  INFO_LOGF("Good CPU_Mode: 0x%02x", mode);
    //}
      //  return;
      ////uint8_t val_ASI_CRC_Status = ldcs(ASI_CRC_Status);
      ////uint8_t val_Reg_13 = ldcs(Reg_13);
      ////uint8_t val_Reg_14 = ldcs(Reg_14);
      ////uint8_t val_Reg_15 = ldcs(Reg_15);

  //while (2 + 2 != 5) {

  uint8_t b = cpu_mode<0xFF>();

  if (b != 0) {
    us = micros();
    ms = millis();
    INFO_LOGF("%u.%u (+%u.%u): Cpu Mode: 0x%02x", ms, us, ms - lastms, us - lastus, b);
    lastms = ms;
    lastus = us;
  }
  if (b == 0x8) {
    INFO_LOG("Success cpu mode 0x8");
  }
  return;


  //// Write pin to LOW
  //pinMode(m_txPin, OUTPUT);
  //digitalWrite(m_txPin, LOW);
  //pinMode(m_txPin, INPUT);

  //// poll pin till it goes HIGH
  //while (digitalRead(m_txPin) == LOW);

  uint8_t sys_status = 0;
  sendLdcsInstruction(10, sys_status);

  INFO_LOGF("CPU_Mode: 0x%02x", sys_status);

  while (2 + 2 != 5) {
    //while (!m_updiSerial.available());
    uint8_t byte = updiReadWait();
    us = micros();
    ms = millis();
    INFO_LOGF("%u.%u (+%u.%u): 0x%x", ms, us, ms - lastms, us - lastus, byte);
    lastms = ms;
    lastus = us;
  }

  //uint8_t buf2[] = { 0x55, 0xad, 0x83, 0x10, 0xad, 0x00 };
  //updiSend(buf2, sizeof(buf2));
  uint8_t val = 0;
  if (!sendLdcsInstruction(UPDI_CS_STATUSA, val)) {
    us = micros();
    ms = millis();
    INFO_LOGF("%u.%u (+%u.%u): LDCS Failed!", ms, us, ms - lastms, us - lastus);
    lastms = ms;
    lastus = us;
    return;
  }

  us = micros();
  ms = millis();
  INFO_LOGF("%u.%u (+%u.%u): LDCS Result: %u", ms, us, ms - lastms, us - lastus, val);
  lastms = ms;
  lastus = us;

  //us = micros();
  //ms = millis();
  //INFO_LOGF("%u.%u (+%u.%u): Sent FF AD 83 10 AD 00, Listening...", ms, us, ms - lastms, us - lastus);
  //lastms = ms;
  //lastus = us;

  while (2 + 2 != 5) {
    while (!m_updiSerial.available());
    uint8_t byte = receiveByte();
    us = micros();
    ms = millis();
    INFO_LOGF("%u.%u (+%u.%u): 0x%x", ms, us, ms - lastms, us - lastus, byte);
    lastms = ms;
    lastus = us;
  }

  //sendBreakFrame();
  uint8_t buf[2] = { UPDI_BREAK, UPDI_BREAK };
  updiSend(buf, 2);

  INFO_LOG("Serial Connected, Sent break");

  // Writing a 1 to this bit disables the contention detection
  sendStcsInstruction(UPDI_CS_CTRLB, 1 << UPDI_CTRLB_CCDETDIS_BIT);
  // Writing a 1 to this bit enables a fixed-length inter-byte delay between each data byte transmitted
  // from the UPDI when doing multi-byte LD(S). The fixed length is two IDLE bits.
  sendStcsInstruction(UPDI_CS_CTRLA, 1 << UPDI_CTRLA_IBDLY_BIT);
  //sendStcsInstruction(UPDI_CS_CTRLB, (1 << UPDI_CTRLB_UPDIDIS_BIT) | (1 << UPDI_CTRLB_CCDETDIS_BIT));

  INFO_LOG("Checking...");
  int tries = 3;
  while (!check() && tries-- > 0) {
    INFO_LOG("UPDI not ready...");
    Time::delayMilliseconds(500);
  }
  if (!tries && !check()) {
    INFO_LOG("UPDI failure");
    return;
  }
  INFO_LOG("UPDI Ready!");

  INFO_LOG("Checking if progmode...");
  tries = 3;
  while (!inProgMode() && tries-- > 0) {
    INFO_LOG("Not in progmode...");
    Time::delayMilliseconds(500);
  }
  if (!tries && !inProgMode()) {
    INFO_LOG("UPDI progmode failure");
    return;
  }
  INFO_LOG("UPDI in progmode!");

  // Send the NVM Programming key
  // This example uses a fixed key for demonstration; replace with actual key for your target
  if (!sendKey(NVMPROG)) {
    INFO_LOG("Failed to send key!");
    return;
  }

  INFO_LOG("Sent key instruction");

  reset(true);
  if (checkCsRegisterUnset(UPDI_ASI_SYS_STATUS, 1 << UPDI_ASI_SYS_STATUS_RSTSYS)) {
    INFO_LOG("Error applying reset!");
    return;
  }
  Time::delayMilliseconds(5);
  reset(false);
  int retries = 0;
  while (retries++ < 10) {
    if (checkCsRegisterUnset(UPDI_ASI_SYS_STATUS, 1 << UPDI_ASI_SYS_STATUS_RSTSYS)) {
      break;
    }
  }
  if (retries >= 10) {
    INFO_LOG("Error releasing reset!");
    return;
  }
  INFO_LOG("Succes applying reset");

  retries = 0;
  bool locked = true;
  while (retries < 100) {
    if (checkCsRegisterUnset(UPDI_ASI_SYS_STATUS, 1 << UPDI_ASI_SYS_STATUS_LOCKSTATUS)) {
      locked = false;
      // success
      break;
    }
    retries++;
    Time::delayMicroseconds(100);
  }
  if (locked) {
    INFO_LOG("Succes! Device unlocked!");
  } else {
    INFO_LOG("Failure! Device locked!");
  }
}

void UPDI::readEEPROMAndUserRow()
{
  // Assuming sizes and starting addresses are known
  uint16_t userRowSize = 128; // Example size
  uint32_t userRowStartAddress = 0x1300;
  uint16_t eepromSize = 256; // Example size
  uint32_t eepromStartAddress = 0x1400;
  uint8_t byte = 0;

  // Read EEPROM
  for (uint16_t i = 0; i < eepromSize; i++) {
    sendLdsInstruction(eepromStartAddress + i, 1, byte);
    INFO_LOGF("eeprom byte [%u]: %x", i, byte);
  }
  // Read USERROW
  for (uint16_t i = 0; i < userRowSize; i++) {
    sendLdsInstruction(userRowStartAddress + i, 1, byte);
    INFO_LOGF("userrow byte [%u]: %x", i, byte);
  }
}

void UPDI::writeEEPROMAndUserRow(const uint8_t *data, uint16_t size)
{
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

void UPDI::writeFirmwareToProgramFlash(const uint8_t *firmware, uint32_t size)
{
  // Placeholder: Detailed implementation would follow a similar pattern to EEPROM/User Row writing
  // but with considerations for flash page sizes, erase cycles, and possibly using NVM commands.
}

bool UPDI::sendKey(KeyType keyType)
{
  const uint8_t *key = nullptr;
  switch (keyType) {
  case CHIP_ERASE:
    key = (const uint8_t *)UPDI_KEY_CHIPERASE;
    break;
  case NVMPROG:
    key = (const uint8_t *)UPDI_KEY_NVM;
    break;
  case USERROW_WRITE:
    key = (const uint8_t *)UPDI_KEY_USERROW_WRITE;
    break;
  default:
    ERROR_LOGF("Unknown key: %u", (uint8_t)keyType);
    return false; // Invalid key type
  }
  return sendKeyInstruction(key);
}

bool UPDI::sendLdsInstruction(uint32_t address, uint8_t addressSize, uint8_t &outVal)
{
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
  outVal = receiveByte();
  return true;
}

bool UPDI::sendStsInstruction(uint32_t address, uint8_t addressSize, uint8_t data)
{
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
  return true;
}

bool UPDI::sendLdcsInstruction(uint8_t address, uint8_t &outVal)
{
  uint8_t buf[2] = { UPDI_PHY_SYNC, (uint8_t)(UPDI_LDCS | (address & 0x0F)) };
  uint8_t recv = 0;
  //INFO_LOG("LD_CS ...");
  if (updiSendReceive(buf, 2, &recv, 1) < 1) {
    INFO_LOGF("Error LD_CS %02X %02X = %02X", buf[0], buf[1], recv);
    return false;
  }
  //INFO_LOGF("... LD_CS %02X %02X = %02X", buf[0], buf[1], recv);
  outVal = recv;
  return true;
}

bool UPDI::sendStcsInstruction(uint8_t address, uint8_t value)
{
  uint8_t buf[3] = { UPDI_PHY_SYNC, (uint8_t)(UPDI_STCS | (address & 0x0F)), value };
  if (!updiSend(buf, 3)) {
    INFO_LOGF("Error ST_CS(%02X) %02X %02X %02X", value, buf[0], buf[1], buf[2]);
    return false;
  }
  return true;
}

bool UPDI::sendKeyInstruction(const uint8_t *key)
{
  uint16_t len = 8 << UPDI_KEY_64;
  uint8_t buf[2] = { UPDI_PHY_SYNC, (uint8_t)(UPDI_KEY | UPDI_KEY_KEY | UPDI_KEY_64) };
  uint8_t key_reversed[8 << UPDI_KEY_64];
  INFO_LOGF("KEY %02X %02X  ", buf[0], buf[1]);
  for (uint16_t i = 0; i < len; i++) {
    INFO_LOGF("%c", key[i]);
    key_reversed[i] = key[len - 1 - i];
  }
  updiSend(buf, 2);
  updiSend(key_reversed, 8 << UPDI_KEY_64);
  // check status
  return checkCsRegisterSet(UPDI_ASI_KEY_STATUS, 1 << UPDI_ASI_KEY_STATUS_NVMPROG);
}

bool UPDI::checkCsRegisterSet(uint8_t csAddress, uint8_t val)
{
  uint8_t csVal = 0;
  if (!sendLdcsInstruction(csAddress, csVal)) {
    return false;
  }
  if ((csVal & val) != val) {
    return false;
  }
  // cs address contains val
  return true;
}

bool UPDI::checkCsRegisterUnset(uint8_t csAddress, uint8_t val)
{
  uint8_t csVal = 0;
  if (!sendLdcsInstruction(csAddress, csVal)) {
    return false;
  }
  if ((csVal & val) == val) {
    return false;
  }
  // cs address doesn't contain val
  return true;
}

void UPDI::sendBreakFrame()
{
  uint8_t buf = { UPDI_BREAK };
  updiSend(&buf, 1);
  //  m_updiSerial.flush(); // Ensure data is transmitted
  //  sendByte(0x00);
  //#ifdef READBACK
  //  Time::delayMilliseconds(2);
  //  if (0x00 == receiveByte()) {
  //    INFO_LOG("Received break back");
  //  }
  //#endif
}

#include "driver/gpio.h"
#include "esp32/rom/ets_sys.h"

#include "driver/rmt.h"
#include "driver/gpio.h"
#include "esp_log.h"

// Define GPIO pin for UPDI communication
#define UPDI_PIN GPIO_NUM_8

#define RMT_TX_CHANNEL RMT_CHANNEL_0
#define RMT_RX_CHANNEL RMT_CHANNEL_1

#define RMT_TX_CHANNEL RMT_CHANNEL_0
#define RMT_RX_CHANNEL RMT_CHANNEL_1

// Set GPIO to output
void gpio_set_output(uint8_t pin)
{
  gpio_set_direction((gpio_num_t)pin, GPIO_MODE_OUTPUT);
}

// Set GPIO to input
void gpio_set_input(uint8_t pin)
{
  gpio_set_direction((gpio_num_t)pin, GPIO_MODE_INPUT);
}

// Set GPIO pin high
void gpio_set_high(uint8_t pin)
{
  gpio_set_level((gpio_num_t)pin, 1);
}

// Set GPIO pin low
void gpio_set_low(uint8_t pin)
{
  gpio_set_level((gpio_num_t)pin, 0);
}

// Read GPIO pin level
uint8_t gpio_read(uint8_t pin)
{
  return gpio_get_level((gpio_num_t)pin);
}

// Delay for a bit period (adjust based on your baud rate)
void bit_delay()
{
  ets_delay_us(100);  // Example delay, adjust for your bit rate
}

// Send a break signal
void UPDI::sendBreak()
{
  //gpio_set_output(m_txPin);
  //gpio_set_low(m_txPin);
  //ets_delay_us(2400);  // Adjust based on the baud rate (example for 2400 us)
  //gpio_set_high(m_txPin);
  //ets_delay_us(2400);  // Ensure the line is idle
  uint8_t buf = UPDI_BREAK;
  updiSend(&buf, 1);
}

void UPDI::updi_serial_term()
{
  m_updiSerial.flush();
  delay(10);
  delay(10);
  m_updiSerial.end();
  // now we manually drive both pins low (timing is relative to chip clock speed (Mhz):
  //20MHz = 6ms, 8Mhz = 13, 4Mhz = 25 : and since we do not know the clock, we assume its slow
  // must force the pins back to being MUXed for I/O
  gpio_matrix_out(m_txPin, SIG_GPIO_OUT_IDX, false, false);
  gpio_matrix_out(m_rxPin, SIG_GPIO_OUT_IDX, false, false);
  //delay(1);
  // switch pins to input so we are not feeding power back over the UPDI pin
  pinMode(m_rxPin, INPUT_PULLUP);
  pinMode(m_txPin, INPUT_PULLUP);
  //return;
  pinMode(m_rxPin, OUTPUT);
  pinMode(m_txPin, OUTPUT);
  //delay(1);
  digitalWrite(m_rxPin, HIGH);
  digitalWrite(m_txPin, HIGH);
  //delay(1);
}

void UPDI::sendDoubleBreak()
{
#if 0

#if 0
  //GPIO_SET_INPUT(GPIO_NUM_8);
  while (m_updiSerial.available()) m_updiSerial.read();
  //GPIO_SET_OUTPUT(GPIO_NUM_8);
  m_updiSerial.updateBaudRate(50);
  delay(50);
  m_updiSerial.write(0);

  //GPIO_SET_INPUT(GPIO_NUM_8);
  while (m_updiSerial.available()) m_updiSerial.read();
  //GPIO_SET_OUTPUT(GPIO_NUM_8);
  delay(12);
  m_updiSerial.write(0);

  //GPIO_SET_INPUT(GPIO_NUM_8);
  while (m_updiSerial.available()) m_updiSerial.read();
  //GPIO_SET_OUTPUT(GPIO_NUM_8);
  m_updiSerial.updateBaudRate(225000);

#else 
  // Clear the UART buffer
  size_t buffered_size;
  uint8_t wasteBuf[16] = { 0 };
  uint8_t breakChar = 0x00;
  uart_read_bytes(UPDI_UART_NUM, wasteBuf, 1, 50 / portTICK_PERIOD_MS);

  // Change baud rate to 10
  uart_set_baudrate(UPDI_UART_NUM, 10);

  // send a double break
  // as per PyUpdi:
  // At 300 bauds, the break character will pull the line low for 30ms
  // Which is slightly above the recommended 24.6ms
  uint8_t breaks[] = { 0x00 };
  uart_write_bytes(UPDI_UART_NUM, breaks, sizeof(breaks));
  uart_read_bytes(UPDI_UART_NUM, wasteBuf, sizeof(breaks), 50 / portTICK_PERIOD_MS);

  GPIO_SET_HIGH(UPDI_TX_PIN);
  GPIO_SET_HIGH(UPDI_RX_PIN);
  delay(12);

  uart_write_bytes(UPDI_UART_NUM, breaks, sizeof(breaks));
  uart_read_bytes(UPDI_UART_NUM, wasteBuf, sizeof(breaks), 50 / portTICK_PERIOD_MS);

  delay(10);

  // Change baud rate back to original UPDI baud rate
  uart_set_baudrate(UPDI_UART_NUM, UPDI_BAUD);

  // Clear the UART buffer
  do {
    uart_read_bytes(UPDI_UART_NUM, wasteBuf, 1, 50 / portTICK_PERIOD_MS);
  } while (uart_get_buffered_data_len(UPDI_UART_NUM, &buffered_size) == ESP_OK && buffered_size > 0);
#endif 
#else
#if 0
  //updi_serial_term();
  GPIO_SET_OUTPUT(UPDI_TX_PIN);
  GPIO_SET_OUTPUT(UPDI_RX_PIN);

  GPIO_SET_HIGH(UPDI_TX_PIN);
  GPIO_SET_HIGH(UPDI_RX_PIN);
  delay(12);
  // first BREAK
  GPIO_SET_LOW(UPDI_TX_PIN);
  GPIO_SET_LOW(UPDI_RX_PIN);
  delay(24);
  GPIO_SET_HIGH(UPDI_TX_PIN);
  GPIO_SET_HIGH(UPDI_RX_PIN);
  delay(2);
  GPIO_SET_LOW(UPDI_TX_PIN);
  GPIO_SET_LOW(UPDI_RX_PIN);
  delay(24);
  GPIO_SET_HIGH(UPDI_TX_PIN);
  GPIO_SET_HIGH(UPDI_RX_PIN);
  // establish a known high state
  delay(38);
  //updi_serial_init();
#else

  //gpio_set_direction(UPDI_TX_PIN, GPIO_MODE_OUTPUT_OD);
  //gpio_set_direction(UPDI_RX_PIN, GPIO_MODE_OUTPUT_OD);

  //esp_rom_gpio_connect_out_signal(UPDI_TX_PIN, UART_PERIPH_SIGNAL(UPDI_UART_NUM, SOC_UART_TX_PIN_IDX), false, false);

  uart_set_line_inverse(UPDI_UART_NUM, UART_SIGNAL_TXD_INV);
  ets_delay_us(24000);
  uart_set_line_inverse(UPDI_UART_NUM, 0);
  ets_delay_us(2000);
  uart_set_line_inverse(UPDI_UART_NUM, UART_SIGNAL_TXD_INV);
  ets_delay_us(24000);
  uart_set_line_inverse(UPDI_UART_NUM, 0);

  //gpio_set_direction(UPDI_TX_PIN, GPIO_MODE_INPUT);
  //gpio_set_direction(UPDI_RX_PIN, GPIO_MODE_INPUT);

  //esp_rom_gpio_connect_in_signal(UPDI_TX_PIN, UART_PERIPH_SIGNAL(UPDI_UART_NUM, SOC_UART_RX_PIN_IDX), false);    

  ets_delay_us(28000);

  //size_t buffered_size;
  //uint8_t wasteBuf[16] = {0};
  //// Clear the UART buffer
  //do {
  //  uart_read_bytes(UPDI_UART_NUM, wasteBuf, 1, 10 / portTICK_PERIOD_MS);
  //} while (uart_get_buffered_data_len(UPDI_UART_NUM, &buffered_size) == ESP_OK && buffered_size > 0);

#endif
#endif
}

//// Send a byte using bit-banging
//void UPDI::sendByte(uint8_t byte) {
//    gpio_set_output(m_txPin);
//
//    // Send start bit (low)
//    gpio_set_low(m_txPin);
//    ets_delay_us(100);  // Adjust based on the baud rate
//
//    // Send data bits (LSB first)
//    for (uint8_t i = 0; i < 8; i++) {
//        if (byte & 0x01) {
//            gpio_set_high(m_txPin);
//        } else {
//            gpio_set_low(m_txPin);
//        }
//        ets_delay_us(100);  // Adjust based on the baud rate
//        byte >>= 1;
//    }
//
//    // Send parity bit (even parity)
//    uint8_t parity = __builtin_parity(byte);
//    if (parity) {
//        gpio_set_high(m_txPin);
//    } else {
//        gpio_set_low(m_txPin);
//    }
//    ets_delay_us(100);  // Adjust based on the baud rate
//
//    // Send stop bit (high)
//    gpio_set_high(m_txPin);
//    ets_delay_us(100);  // Adjust based on the baud rate
//}
//
//// Receive a byte using bit-banging
//uint8_t UPDI::receiveByte() {
//    uint8_t byte = 0;
//
//    gpio_set_input(m_txPin);
//
//    // Wait for start bit (low)
//    while (gpio_read(m_txPin) == 1);
//
//    ets_delay_us(50);  // Move to the middle of the start bit
//
//    // Read data bits (LSB first)
//    for (uint8_t i = 0; i < 8; i++) {
//        ets_delay_us(100);  // Adjust based on the baud rate
//        byte >>= 1;
//        if (gpio_read(m_txPin)) {
//            byte |= 0x80;
//        }
//    }
//
//    // Read and discard parity bit
//    ets_delay_us(100);  // Adjust based on the baud rate
//
//    // Read stop bit
//    ets_delay_us(100);  // Adjust based on the baud rate
//    if (gpio_read(m_txPin) == 0) {
//        // Stop bit should be high, if not, there's an error
//        // Handle error if necessary
//    }
//
//    return byte;
//}

// LDCS function
//uint8_t UPDI::ldcs(uint8_t reg) {
//  //sendBreak();
//    sendByte(0x55);  // SYNCH
//    sendByte(0x80 + reg);
//    return receiveByte();
//}


// Define constants
#define UPDI_UART_PORT UART_NUM_1
#define UPDI_BAUD_RATE 9600
#define UPDI_TX_RX_PIN GPIO_NUM_8  // Example GPIO pin, change as needed
#define UPDI_INIT_DELAY_NS 200

//UPDI::UPDI(uint8_t txPin, uint8_t rxPin) : m_txPin(txPin), m_rxPin(rxPin)
//{
//  gpio_set_direction((gpio_num_t)m_txPin, GPIO_MODE_OUTPUT);
//  gpio_set_high(m_txPin);
//}
//
//void UPDI::initializeUPDICommunication()
//{
//  //// Configure the UART parameters
//  //uart_config_t uart_config = {
//  //    .baud_rate = UPDI_BAUD_RATE,
//  //    .data_bits = UART_DATA_8_BITS,
//  //    .parity = UART_PARITY_EVEN,
//  //    .stop_bits = UART_STOP_BITS_2,
//  //    .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
//  //    .rx_flow_ctrl_thresh = 0,
//  //    .source_clk = UART_SCLK_APB,
//  //};
//
//  //// Install the UART driver
//  //uart_param_config(UPDI_UART_PORT, &uart_config);
//  //uart_set_pin(UPDI_UART_PORT, m_txPin, m_rxPin, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
//  //uart_driver_install(UPDI_UART_PORT, 512, 512, 0, NULL, 0);
//}

//void UPDI::send(const uint8_t *data, uint32_t len)
//{
//  //// Send the data
//  //uart_write_bytes(UPDI_UART_PORT, (const char *)data, len);
//
//  //// Wait for the transmission to complete
//  //uart_wait_tx_done(UPDI_UART_PORT, portMAX_DELAY);
//}
//
//uint32_t UPDI::receive(uint8_t *buffer, uint32_t len, uint32_t timeout)
//{
//  //// Receive data from the UART
//  //return uart_read_bytes(UPDI_UART_PORT, buffer, len, pdMS_TO_TICKS(timeout));
//  return 0;
//}

//void UPDI::enterProgrammingMode()
//{
//    // Drive the UPDI pin low for more than 200 ns to start the initialization sequence
//    gpio_set_low(m_txPin);
//    ets_delay_us(1000);  // 1000 microsecond delay, ensure it is more than 200 ns
//    gpio_set_high(m_txPin);
//
//    // Poll the UPDI pin until it goes high
//    gpio_set_input(m_txPin);
//    while (gpio_read(m_txPin) == 0);
//
//    // Now the UPDI interface is ready for communication
//  uint8_t response = ldcs(0x00);
//  INFO_LOGF("LDCS Response: %02X\n", response);
//
//
//
//  //// Initialize the UART for UPDI communication
//  //initializeUPDICommunication();
//
//  ////// Set the UPDI pin as GPIO output to hold it low
//  //gpio_set_direction((gpio_num_t)m_txPin, GPIO_MODE_OUTPUT);
//  ////gpio_set_direction((gpio_num_t)m_rxPin, GPIO_MODE_OUTPUT);
//  //gpio_set_level((gpio_num_t)m_txPin, 0);
//  ////gpio_set_level((gpio_num_t)m_rxPin, 0);
//
//  ////// Hold the line low for at least 200ns
//  //esp_rom_delay_us(1000);  // Minimum delay is 1ms
//
//
//  //// Send the UPDI command{0x55, 0x83, 0x10, 0xAD, 0x00};
//  //uint8_t address = 0;
//  //uint8_t updi_command[] = { UPDI_PHY_SYNC, (uint8_t)(UPDI_LDCS | (address & 0x0F)) };
//
//  //send(updi_command, sizeof(updi_command));
//
//  //// Set the TX pin to input mode to avoid interfering with RX
//  //gpio_set_direction((gpio_num_t)m_txPin, GPIO_MODE_INPUT);
//  //gpio_set_direction((gpio_num_t)m_rxPin, GPIO_MODE_INPUT);
//
//  //// Buffer to receive data
//  //uint8_t buffer[128];
//
//  //// Receive data
//  //uint32_t len = receive(buffer, sizeof(buffer), 5000);
//
//  //// Process received data
//  //if (len > 0) {
//  //  // Handle received data
//  //  for (uint32_t i = 0; i < len; ++i) {
//  //    INFO_LOGF("Received: %x", buffer[i]);
//  //  }
//  //} else {
//  //  INFO_LOG("Received nothing");
//  //}
//}

#else

#include "UPDI.h"
#include "driver/gpio.h"
#include "driver/timer.h"
#include "soc/gpio_struct.h"
#include "soc/soc.h"
#include "soc/io_mux_reg.h"
#include "esp_timer.h"

// Define GPIO pin for UPDI communication
#define UPDI_PIN GPIO_NUM_8

#define UPDI_BAUD 9600
#define BIT_TIME_US 100 // (1e6 / UPDI_BAUD) // Bit time in microseconds

// Timing macros (adjust as needed for your specific setup)
#define UPDI_BIT_DELAY_US      2000  // Bit period delay
#define UPDI_BREAK_DELAY_US    24000 // Break signal delay
#define UPDI_INIT_DELAY_US     1000 // Initialization delay
#define UPDI_START_BIT_DELAY_US 50  // Start bit delay

// Direct GPIO manipulation macros using ESP32 registers
#define GPIO_SET_HIGH(gpio_num) (GPIO.out_w1ts.val = (1U << gpio_num))
#define GPIO_SET_LOW(gpio_num) (GPIO.out_w1tc.val = (1U << gpio_num))
#define GPIO_READ(gpio_num) ((GPIO.in.val >> gpio_num) & 0x1)

// Configure GPIO for output
#define GPIO_SET_OUTPUT(gpio_num) (GPIO.enable_w1ts.val = (1U << gpio_num))

// Configure GPIO for input
#define GPIO_SET_INPUT(gpio_num) (GPIO.enable_w1tc.val = (1U << gpio_num)); gpio_set_pull_mode((gpio_num_t)gpio_num, GPIO_FLOATING);

void hw_timer_delay_us(uint32_t delay_us)
{
  // Get the current time in microseconds
  uint64_t start_time = esp_timer_get_time();

  // Busy-wait loop until the target time has elapsed
  while ((esp_timer_get_time() - start_time) < delay_us) {
    // Busy-wait
  }
}

UPDI::UPDI(uint8_t txPin, uint8_t rxPin) : m_txPin(UPDI_PIN), m_rxPin(UPDI_PIN)
{
  GPIO_SET_OUTPUT(UPDI_PIN);
  GPIO_SET_HIGH(UPDI_PIN);

  // Configure hardware timer
  timer_config_t config = {
      .alarm_en = TIMER_ALARM_DIS,
      .counter_en = TIMER_PAUSE,
      .intr_type = TIMER_INTR_LEVEL,
      .counter_dir = TIMER_COUNT_UP,
      .auto_reload = TIMER_AUTORELOAD_DIS,
      .divider = 2,   // Prescaler for 40 MHz (25 ns per tick)
      .clk_src = TIMER_SRC_CLK_APB
  };
  if (timer_init(TIMER_GROUP_0, TIMER_0, &config) != ESP_OK) {
    INFO_LOG("Timer init failed");
  }
  if (timer_set_counter_value(TIMER_GROUP_0, TIMER_0, 0) != ESP_OK) {
    INFO_LOG("Timer set counter value failed");
  }
  const gpio_config_t pin_cfg = {
    .pin_bit_mask = (1ul << UPDI_PIN),
    .mode = GPIO_MODE_INPUT_OUTPUT, // Input and Output w/ open-drain!
    .pull_up_en = GPIO_PULLUP_ENABLE, // Open-drain requires a pull-up.
    .pull_down_en = GPIO_PULLDOWN_DISABLE,
    .intr_type = GPIO_INTR_DISABLE
  };
  // now configure the gpios
  gpio_config(&pin_cfg);
}

void UPDI::sendBreak()
{
  GPIO_SET_OUTPUT(UPDI_PIN);
  GPIO_SET_LOW(UPDI_PIN);
  hw_timer_delay_us(UPDI_BREAK_DELAY_US); // Convert microseconds to ticks
  GPIO_SET_HIGH(UPDI_PIN);
  hw_timer_delay_us(UPDI_BREAK_DELAY_US); // Convert microseconds to ticks
}

void UPDI::sendDoubleBreak()
{
  GPIO_SET_OUTPUT(UPDI_PIN);

  // First break signal
  GPIO_SET_LOW(UPDI_PIN);
  hw_timer_delay_us(UPDI_BREAK_DELAY_US); // Convert microseconds to ticks
  GPIO_SET_HIGH(UPDI_PIN);
  hw_timer_delay_us(UPDI_BIT_DELAY_US); // Convert microseconds to ticks

  // Second break signal
  GPIO_SET_LOW(UPDI_PIN);
  hw_timer_delay_us(UPDI_BREAK_DELAY_US); // Convert microseconds to ticks
  GPIO_SET_HIGH(UPDI_PIN);
  hw_timer_delay_us(UPDI_BREAK_DELAY_US); // Convert microseconds to ticks
}

void UPDI::resetOn()
{
  stcs(ASI_Reset_Request, 0x59);
}

bool UPDI::resetOff()
{
  stcs(ASI_Reset_Request, 0x0);
  uint8_t timeout = 0;
  while (cpu_mode<0x0E>() == 0 && timeout < 10) {
    hw_timer_delay_us(100);
    timeout++;
  }
  return timeout < 2;
}

bool UPDI::reset()
{
  resetOn();
  return resetOff();
}

void wait_for_bit()
{

}

void UPDI::sendByte(uint8_t byte)
{
  uint8_t parity = __builtin_parity(byte);
  GPIO_SET_OUTPUT(UPDI_PIN);

  // Send start bit (low)
  GPIO_SET_LOW(UPDI_PIN);
  hw_timer_delay_us(BIT_TIME_US);

  // Send data bits (LSB first)
  for (uint8_t i = 0; i < 8; i++) {
    if (byte & 0x01) {
      GPIO_SET_HIGH(UPDI_PIN);
    } else {
      GPIO_SET_LOW(UPDI_PIN);
    }
    hw_timer_delay_us(BIT_TIME_US);
    byte >>= 1;
  }

  // Send parity bit (even parity)
  if (parity) {
    GPIO_SET_HIGH(UPDI_PIN);
  } else {
    GPIO_SET_LOW(UPDI_PIN);
  }
  hw_timer_delay_us(BIT_TIME_US);

  // Send stop bit (high)
  GPIO_SET_HIGH(UPDI_PIN);
  hw_timer_delay_us(BIT_TIME_US * 2);

  GPIO_SET_INPUT(UPDI_PIN);
}

uint8_t UPDI::receiveByte()
{
  uint8_t byte = 0;

  //GPIO_SET_INPUT(UPDI_PIN);

  // Wait for start bit (low)
  //while (GPIO_READ(UPDI_PIN) == 1);

  uint32_t counter = 0;
#define TIMEOUT_TT 40000000
  while (GPIO_READ(UPDI_PIN) == 1 && counter++ < TIMEOUT_TT);
  if (counter >= TIMEOUT_TT) {
    INFO_LOG("Timed out waiting for start bit");
    return 0;
  }

  hw_timer_delay_us(BIT_TIME_US / 2); // Half bit time for sampling

  // Read data bits (LSB first)
  for (uint8_t i = 0; i < 8; i++) {
    hw_timer_delay_us(BIT_TIME_US);
    byte >>= 1;
    if (GPIO_READ(UPDI_PIN)) {
      byte |= 0x80;
    }
  }

  // Read and discard parity bit
  hw_timer_delay_us(BIT_TIME_US);

  // Read stop bit
  hw_timer_delay_us(BIT_TIME_US);
  if (GPIO_READ(UPDI_PIN) == 0) {
    //INFO_LOG("Stop bit error");
  }

  hw_timer_delay_us(BIT_TIME_US * 2); // Convert microseconds to ticks

  return byte;
}

uint8_t UPDI::ldcs(cs_reg reg)
{
  sendByte(0x55);  // SYNCH
  sendByte(0x80 + reg);
  return receiveByte();
}

void UPDI::stcs(cs_reg r, uint8_t data)
{
  sendByte(0x55);
  sendByte(0xC0 + r);
  sendByte(data);
}

void UPDI::sendKey(const uint8_t *key)
{
  //static const uint8_t NVM_Prog[8] = { 0x4E, 0x56, 0x4D, 0x50, 0x72, 0x6F, 0x67, 0x20 };
  //static const uint8_t Chip_Erase[8] = { 0x4E, 0x56, 0x4D, 0x45, 0x72, 0x61, 0x73, 0x65 };
  sendByte(0x55);
  sendByte(0xE0);
  sendByte(key[0]);
  sendByte(key[1]);
  sendByte(key[2]);
  sendByte(key[3]);
  sendByte(key[4]);
  sendByte(key[5]);
  sendByte(key[6]);
  sendByte(key[7]);
}

void UPDI::stptr_p(const uint8_t *addr_p, uint8_t n)
{
  sendByte(0x55);
  sendByte(0x68 + --n);
  sendByte(*(addr_p++));
  if (n >= 1) {
    sendByte(*(addr_p++));
  }
  if (n >= 2) {
    sendByte(*addr_p);
  }
  receiveByte();
}

void UPDI::rep(uint8_t repeats)
{
  sendByte(0x55);
  sendByte(0xA0);
  sendByte(repeats);
}

uint8_t UPDI::ldinc_b()
{
  sendByte(0x55);
  sendByte(0x24);
  return receiveByte();
}

void UPDI::sts_b(uint16_t address, uint8_t data)
{
  sendByte(0x55);
  sendByte(0x44);
  sendByte(address & 0xFF);
  sendByte(address >> 8);
  receiveByte();
  sendByte(data);
  receiveByte();
}
#define SAMPLE_BUFFER_SIZE 4096 // Buffer size for logging in big blocks
#define POLL_INTERVAL_US 4 // Polling interval in microseconds (adjust as needed)

// Structure to hold the pin state and timestamp
static uint8_t sample_buffer[SAMPLE_BUFFER_SIZE];
static uint32_t sample_index = 0;

uint32_t detect_baud_rate()
{
  // Buffer to store timestamps of edges
  uint64_t timestamps[16];
  memset(timestamps, 0, sizeof(timestamps));
  int edge_count = 0;

  // Wait for the start bit (falling edge)
  while (GPIO_READ(UPDI_PIN) == 1);
  timestamps[edge_count++] = esp_timer_get_time();

  // Measure time for 10 edges (1 start bit, 8 data bits, 1 stop bit)
  while (edge_count < 10) {
    while (GPIO_READ(UPDI_PIN) == 0);
    timestamps[edge_count++] = esp_timer_get_time();
    while (GPIO_READ(UPDI_PIN) == 1);
    timestamps[edge_count++] = esp_timer_get_time();
  }

  // Calculate bit duration from timestamps
  uint64_t bit_durations[8];
  for (int i = 0; i < 8; ++i) {
    bit_durations[i] = timestamps[i + 2] - timestamps[i + 1];
  }

  // Average the bit durations
  uint64_t total_duration = 0;
  for (int i = 0; i < 8; ++i) {
    total_duration += bit_durations[i];
  }
  uint64_t average_bit_duration = total_duration / 8;

  // Calculate baud rate
  uint32_t baud_rate = 1000000 / average_bit_duration;
  return baud_rate;
}

void UPDI::enterProgrammingMode()
{
  INFO_LOG("Entering programming mode");
  uint8_t buf[256] = { 0 };
  memset(buf, 0, sizeof(buf));

  //// Drive the UPDI pin low for more than 200 ns to start the initialization sequence
  //GPIO_SET_HIGH(UPDI_PIN);
  //Time::delayMilliseconds(1);
  ////hw_timer_delay_ticks(UPDI_INIT_DELAY_US * 160 / 2); // Convert microseconds to ticks

  ////// Poll the UPDI pin until it goes high
  //GPIO_SET_INPUT(UPDI_PIN);
  //while (GPIO_READ(UPDI_PIN) == 0);

  //// Configure GPIO for input
  //gpio_pad_select_gpio(UPDI_PIN);
  //gpio_set_direction(UPDI_PIN, GPIO_MODE_INPUT);

  //while (1) {
  //  sample_index = 0;
  //  // Poll the pin in a loop and store the state in the buffer
  //  while (sample_index < SAMPLE_BUFFER_SIZE) {
  //      sample_buffer[sample_index] = GPIO_READ(UPDI_PIN);
  //      sample_index++;
  //      // Wait for the next polling interval
  //      ets_delay_us(POLL_INTERVAL_US); // Polling interval
  //  }

  //  // Log the results after sampling
  //  INFO_LOG("Logging results:");
  //  for (int i = 0; i < SAMPLE_BUFFER_SIZE; i++) {
  //    INFO_LOGF("%d", sample_buffer[i]);
  //  }
  //}


  //uint32_t baud = detect_baud_rate();
  //INFO_LOGF("Detected baud: %u", baud);


  //while (1) {
  //  uint8_t b = receiveByte();
  //  INFO_LOGF("Received: 0x%02x", b);
  //}

  //sendBreak();
  //stcs(Control_A, 0x00);


  //// Poll the UPDI pin until it goes high
  //gpio_set_input(UPDI_PIN);
  //uint32_t poll_wait_count = 0;
  //while (GPIO_READ(UPDI_PIN) == 0) {
  //    if (++poll_wait_count > 1000000) {
  //        INFO_LOG("Polling wait timeout");
  //        return;
  //    }
  //}

  // Now the UPDI interface is ready for communication
  //uint8_t val_Status_A = ldcs(Status_A);
  //uint8_t val_Status_B = ldcs(Status_B);
  //uint8_t val_Control_A = ldcs(Control_A);
  //uint8_t val_Control_B = ldcs(Control_B);
  //uint8_t val_Reg_4 = ldcs(Reg_4);
  //uint8_t val_Reg_5 = ldcs(Reg_5);
  //uint8_t val_Reg_6 = ldcs(Reg_6);
  //uint8_t val_ASI_Key_Status = ldcs(ASI_Key_Status);
  //uint8_t val_ASI_Reset_Request = ldcs(ASI_Reset_Request);
  // uint8_t val_ASI_Control_A = ldcs(ASI_Control_A);
  //uint8_t val_ASI_System_Control_A = ldcs(ASI_System_Control_A);
  //uint8_t sys_status = ldcs(ASI_System_Status);
    //INFO_LOGF("CPU_Mode: 0x%02x", sys_status);
  //  return;

  stcs(Control_A, 0x6);
  uint8_t mode = 0;
  //while (1) {
  mode = cpu_mode<0xEF>();
  if (mode != 0x82) {
    INFO_LOGF("Bad Cpu Mode 0x%02x...", mode);
    sendDoubleBreak();
    uint8_t val = ldcs(Status_B);
    INFO_LOGF("status B: 0x%02x", val);
    return;
  }
  //INFO_LOGF("Cpu Mode 0x%02x...", mode);
  return;
  //}
    //uint8_t mode2 = cpu_mode<0xEF>();
    //INFO_LOGF("Cpu Mode 0x%02x...", mode);
    //INFO_LOGF("Cpu Mode 0x%02x...", mode2);
    //return;
    //while (1) {
    //  mode = cpu_mode<0xEF>();
    //}
  switch (mode) {
  case 0x21:
  case 0xa2:
  case 0x82:
    //  return;
    ////uint8_t val_ASI_CRC_Status = ldcs(ASI_CRC_Status);
    ////uint8_t val_Reg_13 = ldcs(Reg_13);
    ////uint8_t val_Reg_14 = ldcs(Reg_14);
    ////uint8_t val_Reg_15 = ldcs(Reg_15);

    //if (!reset()) {
    //  INFO_LOG("Failed to reset 1");
    //  return;
    //}

    //if (cpu_mode<0x01>()) {
    //  INFO_LOG("still locked");
    //  return;
    //}

    sendProgKey();
    if (!reset()) {
      INFO_LOG("Failed to reset 1");
      return;
    }
    INFO_LOG("Sent key...");

    // fallthrough
  case 0x08:
    sts_b(NVM_base | Control_A, NVM_CMD_PBC);
    break;
  default:
    INFO_LOGF("Unknown cpu mode: 0x%02x", mode);
    return;
  }

  return;

  //sendEraseKey();
  //stcs(ASI_Reset_Request, 0x59);
  //stcs(ASI_Reset_Request, 0x00);
  //tt = 0;
  //while (cpu_mode<0x0E>() == 0 && tt++ < 2) {
  //  hw_timer_delay_us(1000);
  //}

  //mode = cpu_mode<0xEF>();
  //if (mode != 0x82) {
  //  INFO_LOGF("Bad CPU Mode: 0x%02x", mode);
  //  return;
  //}
  //INFO_LOGF("Good Cpu Mode 0x%02x...", mode);

  //int8_t key_status = ldcs(ASI_Key_Status);
  //INFO_LOGF("Key Status: 0x%02x", key_status);

  // perform a reset
  stcs(ASI_Reset_Request, 0x59);
  stcs(ASI_Reset_Request, 0x00);
  if (cpu_mode<0x01>()) {
    INFO_LOG("Chip is locked");
  } else {
    INFO_LOG("Chip unlocked");
  }

  //// Wait for the reset process to end.
  //// Either NVMPROG, UROWPROG or BOOTDONE bit will be set in the ASI_SYS_STATUS UPDI register.
  //// This indicates reset is complete.
  ////while (cpu_mode<0x0E>() == 0);

  ////uint8_t val_ASI_System_Status = ldcs(ASI_System_Status);


  ////INFO_LOGF("ASI System Status: 0x%02x", b);
  //sendKey();

  //enum reg
  //{
  //  CTRLA,
  //  CTRLB,
  //  STATUS,
  //  INTCTRL,
  //  INTFLAGS,
  //  Reg_5,
  //  DATA_lo,
  //  DATA_hi,
  //  ADDR_lo,
  //  ADDR_hi,
  //  ADDR_ext
  //};
  //enum cmnd
  //{
  //  NVM_CMD_NOP,      /* Does nothing */
  //  NVM_CMD_WP,       /* Write page buffer to memory */
  //  NVM_CMD_ER,       /* Erase page */
  //  NVM_CMD_ERWP,     /* Erase and write page */
  //  NVM_CMD_PBC,      /* Page buffer clear */
  //  NVM_CMD_CHER,     /* Chip erase: erase Flash and EEPROM */
  //  NVM_CMD_EEER,     /* EEPROM Erase */
  //  NVM_CMD_WFU       /* Write fuse */
  //};
  //sts_b(NVM_base | CTRLA, NVM_CMD_PBC);

  //mode = cpu_mode<0x0E>();
  //INFO_LOGF("std_b complete, CPU Mode: 0x%02x", mode);



  ////for (uint16_t i = 0; i < numBytes; i++) {
  ////  INFO_LOGF("Received [%u]: 0x%02x", i, buf[i]);
  ////}

  ////INFO_LOGF("val_Status_A: 0x%02x", val_Status_A);
  ////INFO_LOGF("val_Status_B: 0x%02x", val_Status_B);
  ////INFO_LOGF("val_Control_A: 0x%02x", val_Control_A);
  ////INFO_LOGF("val_Control_B: 0x%02x", val_Control_B);
  ////INFO_LOGF("val_Reg_4: 0x%02x", val_Reg_4);
  ////INFO_LOGF("val_Reg_5: 0x%02x", val_Reg_5);
  ////INFO_LOGF("val_Reg_6: 0x%02x", val_Reg_6);
  ////INFO_LOGF("val_ASI_Key_Status: 0x%02x", val_ASI_Key_Status);
  ////INFO_LOGF("val_ASI_Reset_Request: 0x%02x", val_ASI_Reset_Request);
  ////INFO_LOGF("val_ASI_Control_A: 0x%02x", val_ASI_Control_A);
  ////INFO_LOGF("val_ASI_System_Control_A: 0x%02x", val_ASI_System_Control_A);
  ////INFO_LOGF("val_ASI_System_Status: 0x%02x", val_ASI_System_Status);
  ////INFO_LOGF("val_ASI_CRC_Status: 0x%02x", val_ASI_CRC_Status);
  ////INFO_LOGF("val_Reg_13: 0x%02x", val_Reg_13);
  ////INFO_LOGF("val_Reg_14: 0x%02x", val_Reg_14);
  ////INFO_LOGF("val_Reg_15: 0x%02x", val_Reg_15);

  ////uint8_t response = ldcs(Status_B);
  ////INFO_LOGF("LDCS Response: 0x%02X", response);
  //while (1) {
  //  uint8_t b = receiveByte();
  //  if (b && b != 0xff) {
  //    INFO_LOGF("Received: 0x%02x", b);
  //  }
  //}
}

uint8_t UPDI::lds_b(uint16_t address)
{
  sendByte(0x55);
  sendByte(0x04);
  sendByte(address & 0xFF);
  sendByte(address >> 8);
  return receiveByte();
}

uint8_t UPDI::ld_b()
{
  sendByte(0x55);
  sendByte(0x20);
  return receiveByte();
}

void UPDI::eraseMemory()
{
  sendEraseKey();
  uint8_t status = ldcs(ASI_Key_Status);
  INFO_LOGF("Erasing mem, key status: 0x%02x...", status);
  if (!reset()) {
    INFO_LOG("Failed to reset, chip erase failed");
    return;
  }
  enterProgrammingMode();
}

void UPDI::readMemory()
{
  sendProgKey();
  uint8_t status = ldcs(ASI_Key_Status);
  if (status != 0x10) {
    INFO_LOGF("Bad prog key status: 0x%02x", status);
    return;
  }
  reset();
  status = cpu_mode();
  while (status != 0x8) {
    INFO_LOGF("Waiting cpu mode %02x", status);
    status = cpu_mode();
  }

  uint16_t chunksize = 4;
  ByteStream header(5);
  uint8_t *ptr = (uint8_t *)header.rawData();
  for (uint16_t i = 0; i < header.rawSize(); ++i) {
    uint16_t addr = 0x1400 + i;
    stptr_p((const uint8_t *)&addr, 2);
    ptr[i] = ld_b();
  }
  if (!header.checkCRC()) {
    INFO_LOG("ERROR Header CRC Invalid!");
    reset();
    return;
  }
  INFO_LOG("Header CRC Verified");
  struct HeaderData {
    uint8_t version_major;
    uint8_t version_minor;
    uint8_t global_flags;
    uint8_t brightness;
    uint8_t num_modes;
  };
  HeaderData *headerData = (HeaderData *)header.data();

  INFO_LOG("Header:");
  INFO_LOGF(" Version: %u.%u", headerData->version_major, headerData->version_minor);
  INFO_LOGF(" Flags: 0x%08x", headerData->global_flags);
  INFO_LOGF(" Brightness: %u", headerData->brightness);
  INFO_LOGF(" Num Modes: %u", headerData->num_modes);

  //for (uint16_t i = 0; i < sizeof(buf); i++) {
  //  INFO_LOGF("Byte [%u]: 0x%02x", i, buf[i]);
  //}
  reset();
}


#endif
