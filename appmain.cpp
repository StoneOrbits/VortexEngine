#include <inttypes.h>
#include <Arduino.h>
#include <avr/io.h>

#define STORAGE_SIZE PROGMEM_PAGE_SIZE

#define LED_DATA_PIN 7
#define LED_COUNT 1

#define storage_data ((uint8_t *)0x8400)

struct RGBColor
{
  // public members
  uint8_t red;
  uint8_t green;
  uint8_t blue;
};

// Output PORT register
volatile uint8_t *m_port = nullptr;
// Output PORT bitmask
uint8_t m_pinMask = 0;
RGBColor m_ledColors[LED_COUNT] = { 0 };

// store a serial buffer to storage
bool storage_write(uint8_t *buf)
{
  // copy in page
  memcpy(storage_data, buf, STORAGE_SIZE);
  // Erase + write the flash page
  _PROTECTED_WRITE_SPM(NVMCTRL.CTRLA, 0x3);
  while (NVMCTRL.STATUS & 0x3);
  return (NVMCTRL.STATUS != 4);
}

// read a serial buffer from storage
bool storage_read(uint8_t *buf)
{
  // Read the data from EEPROM into the buffer
  memcpy(buf, storage_data, STORAGE_SIZE);
  return true;
}

bool leds_init()
{
  // clear the onboard led so it displays nothing
  // tiny neo pixels
  pinMode(LED_DATA_PIN, OUTPUT);
  // register ouput port
  m_port = portOutputRegister(digitalPinToPort(LED_DATA_PIN));
  // create a pin mask to use later
  m_pinMask = digitalPinToBitMask(LED_DATA_PIN);
  return true;
}

void leds_update()
{
  noInterrupts();
  volatile uint16_t
    i = LED_COUNT * sizeof(RGBColor); // Loop counter
  volatile uint8_t
    *ptr = (volatile uint8_t *)m_ledColors,   // Pointer to next byte
    b = *ptr++,   // Current byte value
    hi,             // PORT w/output bit set high
    lo;             // PORT w/output bit set low

    volatile uint8_t next, bit;

    hi   = *m_port |  m_pinMask;
    lo   = *m_port & ~m_pinMask;
    next = lo;
    bit  = 8;

    asm volatile(
     "head20:"                   "\n\t" // Clk  Pseudocode    (T =  0)
      "st   %a[port],  %[hi]"    "\n\t" // 1    PORT = hi     (T =  1)
      "sbrc %[byte],  7"         "\n\t" // 1-2  if (b & 128)
       "mov  %[next], %[hi]"     "\n\t" // 0-1   next = hi    (T =  3)
      "dec  %[bit]"              "\n\t" // 1    bit--         (T =  4)
      "nop"                      "\n\t" // 1    nop           (T =  5)
      "rjmp .+0"                 "\n\t" // 2    nop nop       (T =  7)
      "st   %a[port],  %[next]"  "\n\t" // 1    PORT = next   (T =  8)
      "mov  %[next] ,  %[lo]"    "\n\t" // 1    next = lo     (T =  9)
      "breq nextbyte20"          "\n\t" // 1-2  if (bit == 0) (from dec above)
      "rol  %[byte]"             "\n\t" // 1    b <<= 1       (T = 11)
      "rjmp .+0"                 "\n\t" // 2    nop nop       (T = 13)
      "rjmp .+0"                 "\n\t" // 2    nop nop       (T = 15)
      "st   %a[port],  %[lo]"    "\n\t" // 1    PORT = lo     (T = 16)
      "nop"                      "\n\t" // 1    nop           (T = 17)
      "rjmp .+0"                 "\n\t" // 2    nop nop       (T = 19)
      "rjmp .+0"                 "\n\t" // 2    nop nop       (T = 21)
      "rjmp .+0"                 "\n\t" // 2    nop nop       (T = 23)
      "rjmp head20"              "\n\t" // 2    -> head20 (next bit out)
     "nextbyte20:"               "\n\t" //                    (T = 11)
      "ldi  %[bit]  ,  8"        "\n\t" // 1    bit = 8       (T = 12)
      "ld   %[byte] ,  %a[ptr]+" "\n\t" // 2    b = *ptr++    (T = 14)
      "nop"                      "\n\t" // 1    nop           (T = 15)
      "st   %a[port], %[lo]"     "\n\t" // 1    PORT = lo     (T = 16)
      "nop"                      "\n\t" // 1    nop           (T = 17)
      "rjmp .+0"                 "\n\t" // 2    nop nop       (T = 19)
      "rjmp .+0"                 "\n\t" // 2    nop nop       (T = 21)
      "sbiw %[count], 1"         "\n\t" // 2    i--           (T = 23)
       "brne head20"             "\n"   // 2    if (i != 0) -> (next byte)  ()
    : [ptr]   "+e" (ptr),
      [byte]  "+r" (b),
      [bit]   "+d" (bit),
      [next]  "+r" (next),
      [count] "+w" (i)
    : [port]   "e" (m_port),
      [hi]     "r" (hi),
      [lo]     "r" (lo));

  interrupts();
}

bool do_work()
{
#define BUFSIZE PROGMEM_PAGE_SIZE
  uint8_t inbuf[BUFSIZE] = {0};
  uint8_t outbuf[BUFSIZE] = {0};
  for (uint8_t i = 0; i < BUFSIZE; ++i) {
    inbuf[i] = i;
  }
  if (!storage_write(inbuf)) {
    // yellow
    m_ledColors[0].red = 255;
    m_ledColors[0].green = 255;
    return false;
  }
  if (!storage_read(outbuf)) {
    // blue
    m_ledColors[0].blue = 255;
    return false;
  }
  for (uint8_t i = 0; i < BUFSIZE; ++i) {
    if (inbuf[i] != outbuf[i]) {
      // purple
      m_ledColors[0].blue = 255;
      m_ledColors[0].red = 255;
      return false;
    }
  }
  return true;
}

int main()
{
  // init megatinycore
  init();

  // == DONE MEGATINYCORE INIT ==

  leds_init();

  if (do_work()) {
    m_ledColors[0].green = 255;
  }
  leds_update();
  while(1);

  return 0;
}
