#include "Infrared.h"

#include "SerialBuffer.h"
#include "TimeControl.h"
#include "BitStream.h"
#include "Buttons.h"
#include "Leds.h"
#include "Log.h"

#include <Arduino.h>

#include <queue>

using namespace std;

#ifndef TEST_FRAMEWORK
// Timer used for PWM, is initialized in initpwm()
Tcc* IR_TCCx2;
#endif

#define IR_TIMING 564
#define HEADER_MARK (IR_TIMING * 16)
#define HEADER_SPACE (IR_TIMING * 8)

#define HEADER_MARK_MIN (HEADER_SPACE - (IR_TIMING * 4))
#define HEADER_SPACE_MIN (HEADER_SPACE - (IR_TIMING * 2))

#define DEFAULT_MARK_EXCESS 50
#define DEFAULT_FRAME_TIMEOUT 7800 //maximum length of SPACE Sbefore we assume frame ended
//DEFAULT_TIMEOUT should be 1.25*the_largest_space_any_valid_IR_protocol_might_have.
//In IRremote library ir_Dish.cpp space they use DISH_RPT_SPACE 6200 while referenced says
//about 6000. If we take 6200*1.25= 7750 rounded up we will use 7800. Previous IRLib
//value was 10000 was probably too large. Thanks to Gabriel Staples for this note.

#define IR_SEND_PWM_PIN 0
#define RECEIVER_PIN 2

#define MAX_DATA_ENTRIES 4096

// this bool is toggled when actively sending data to prevent
// the receiver from running and receiving our own data
volatile bool sending = false;

// a typedef for a unit of IR time, make this smaller to save space
// but at the cost of not handling large IR timings properly. 
// TODO: handle large IR timings properly
typedef uint16_t ir_time;

// global queue of IR data that is pushed to by ISR and
// popped by the decoder in read()
queue<ir_time> g_ir_data;

Infrared::Infrared()
{
}

bool Infrared::init()
{
  pinMode(RECEIVER_PIN, INPUT_PULLUP);
  pinMode(IR_SEND_PWM_PIN, OUTPUT);
  digitalWrite(IR_SEND_PWM_PIN, LOW); // When not sending PWM, we want it low
  attachInterrupt(digitalPinToInterrupt(RECEIVER_PIN), Infrared::recvPCIHandler, CHANGE);
  sending = false;
  return true;
}

void Infrared::cleanup()
{
}

bool Infrared::read(SerialBuffer &data)
{
  uint32_t num_dwords = next_ir_data();
  data.clear();
  data.init(num_dwords * 4);
  uint32_t *rawBuf = (uint32_t *)data.rawData();
  for (uint32_t i = 0; i < num_dwords; ++i) {
    rawBuf[i] = next_ir_data();
  }
  return true;
#if 0
  uint32_t result = 0;
  while (!match_next_ir_data(HEADER_MARK)) {
    if (g_ir_data.empty()) {
      return 0;
    }
  }
  DEBUG_LOG("Matched header mark");
  // pop next ir data and try to match header mark + space
  if (!match_next_ir_data(HEADER_SPACE)) {
    DEBUG_LOG("Didn't match header space");
    return false;
  }
  DEBUG_LOG("Matched header space");
  // decode a 32bit integer, it should be the size
  for (uint32_t i = 0; i < num_dwords; ++i) {
    rawBuf[i] = decode32();
    DEBUG_LOGF("Decoded: %x", rawBuf[i]);
  }
  DEBUG_LOGF("Read %u bytes", data.size());
  return true;
#endif
}

bool Infrared::write(SerialBuffer &data)
{
  initpwm();
  sending = true;
  // wakeup the other receiver with a quick mark/space
  mark(50);
  space(100);
  // now send the header
  mark(HEADER_MARK);
  space(HEADER_SPACE);
  // access the data in dwords
  uint32_t *dwData = (uint32_t *)data.rawData();
  uint32_t num_dwords = (data.rawSize() + 3) / 4;
  // write the number of dwords for the IR decoder
  write32(num_dwords);
  // now iterate the bytes of data
  for (uint32_t i = 0; i < num_dwords; ++i) {
    write32(dwData[i]);
  }
  sending = false;
  return true;
}

// ===================
//  sending functions

// shamelessly stolen from IRLib2, thanks
void Infrared::initpwm()
{
#ifndef TEST_FRAMEWORK
  // just in case
  pinMode(IR_SEND_PWM_PIN, OUTPUT);
  digitalWrite(IR_SEND_PWM_PIN, LOW); // When not sending PWM, we want it low
  uint8_t port = g_APinDescription[IR_SEND_PWM_PIN].ulPort; // 0
  uint8_t pin = g_APinDescription[IR_SEND_PWM_PIN].ulPin;   // 8
  uint8_t IR_mapIndex = (port * 32) + pin; // 8
  ETCChannel IR_TCC_Channel = TCC0_CH0;
  int8_t IR_PER_EorF = PORT_PMUX_PMUXE_E;
  //println();Serial.print("Port:"); Serial.print(port,DEC); Serial.print(" Pin:"); Serial.println(pin,DEC);
  // Enable the port multiplexer for the PWM channel on pin   
  PORT->Group[port].PINCFG[pin].bit.PMUXEN = 1;
  
  // Connect the TCC timer to the port outputs - port pins are paired odd PMUXO and even PMUXEII 
  // F & E peripherals specify the timers: TCC0, TCC1 and TCC2
  PORT->Group[port].PMUX[pin >> 1].reg |= IR_PER_EorF; 
  
//  pinPeripheral (IR_SEND_PWM_PIN,PIO_TIMER_ALT);
  // Feed GCLK0 to TCC0 and TCC1
  REG_GCLK_CLKCTRL = GCLK_CLKCTRL_CLKEN |       // Enable GCLK0 to TCC0 and TCC1
                     GCLK_CLKCTRL_GEN_GCLK0 |   // Select GCLK0
                     GCLK_CLKCTRL_ID_TCC0_TCC1; // Feed GCLK0 to TCC0 and TCC1
  while (GCLK->STATUS.bit.SYNCBUSY);            // Wait for synchronization

  // Normal (single slope) PWM operation: timers countinuously count up to PER 
  // register value and then is reset to 0
  IR_TCCx2 = (Tcc*) GetTC(IR_TCC_Channel);
  IR_TCCx2->WAVE.reg |= TCC_WAVE_WAVEGEN_NPWM;   // Setup single slope PWM on TCCx
  while (IR_TCCx2->SYNCBUSY.bit.WAVE);           // Wait for synchronization

  // Each timer counts up to a maximum or TOP value set by the PER register,
  // this determines the frequency of the PWM operation.
  uint32_t cc = F_CPU/(38*1000) - 1;
  IR_TCCx2->PER.reg = cc;      // Set the frequency of the PWM on IR_TCCx2
  while(IR_TCCx2->SYNCBUSY.bit.PER);

  // The CCx register value corresponds to the pulsewidth in microseconds (us) 
  // Set the duty cycle of the PWM on TCC0 to 33%
  IR_TCCx2->CC[GetTCChannelNumber(IR_TCC_Channel)].reg = cc/3;      
  while (IR_TCCx2->SYNCBUSY.reg & TCC_SYNCBUSY_MASK);
  //while(IR_TCCx2->SYNCBUSY.bit.CC3);

  // Enable IR_TCCx2 timer but do not turn on PWM yet. Will turn it on later.
  IR_TCCx2->CTRLA.reg |= TCC_CTRLA_PRESCALER_DIV1;     // Divide GCLK0 by 1
  while (IR_TCCx2->SYNCBUSY.bit.ENABLE);
  IR_TCCx2->CTRLA.reg &= ~TCC_CTRLA_ENABLE;            //initially off will turn on later
  while (IR_TCCx2->SYNCBUSY.bit.ENABLE);
#endif
}

void Infrared::delayus(uint16_t time)
{
  if (!time) { 
    return;
  }
  if (time <= 16000) {
    delayMicroseconds(time);
    return;
  }
  delayMicroseconds(time % 1000);
  delay(time / 1000);
}

void Infrared::write32(uint32_t data)
{
  for (int b = 31; b >= 0; b--) {
    // grab the bit of data at the index
    bool bit = ((data & (1 << b)) != 0);
    // send 3x timing size for 1s and 1x timing for 0
    mark(IR_TIMING + (IR_TIMING * (2 * bit)));
    // send 1x timing size for space
    space(IR_TIMING);
  }
  //DEBUG_LOGF("Wrote %x", data);
}

uint32_t Infrared::mark(uint16_t time)
{
#ifndef TEST_FRAMEWORK
  // start the PWM
  IR_TCCx2->CTRLA.reg |= TCC_CTRLA_ENABLE;
  while (IR_TCCx2->SYNCBUSY.bit.ENABLE);
#endif
  delayus(time);
  return time;
}

uint32_t Infrared::space(uint16_t time)
{
#ifndef TEST_FRAMEWORK
  // stop the PWM
  IR_TCCx2->CTRLA.reg &= ~TCC_CTRLA_ENABLE;
  while (IR_TCCx2->SYNCBUSY.bit.ENABLE);
#endif
  delayus(time);
  return time;
}

// ===================
// receiver functions

#if 0
bool Infrared::poll()
{
  static uint8_t oldState = HIGH;
  static uint32_t lastTime = 0;
  uint8_t newState = (uint8_t)digitalRead(RECEIVER_PIN);
  uint32_t now = micros();
  if (oldState != newState) {
    uint32_t timeDiff = now - lastTime;
    // oldState = HIGH means SPACE.
    // If it's a very long wait
    g_ir_data.push(timeDiff);
  }
  lastTime = now;
  return true;
}
#endif

uint32_t Infrared::next_ir_data()
{
  uint32_t val = g_ir_data.front();
  g_ir_data.pop();
  DEBUG_LOGF("IR val: %x", val);
  return val;
}

#define PERCENT_TOLERANCE 45
#define ABS_TOLERANCE 60
bool floatmatch(uint32_t val, uint32_t expected)
{
  return (val >= (uint16_t)(expected * (1.0 - PERCENT_TOLERANCE / 100.0)))
      && (val <= (uint16_t)(expected * (1.0 + PERCENT_TOLERANCE / 100.0)));
}

bool match(uint32_t val, uint32_t expected)
{
  return (val >= (expected - ABS_TOLERANCE)) && (val <= (expected + ABS_TOLERANCE));
}

bool Infrared::match_next_ir_data(uint32_t expected)
{
  if (!g_ir_data.size()) {
    DEBUG_LOG("No data to match");
    return false;
  }
  uint32_t val = next_ir_data();
  DEBUG_LOGF("matching %u to expected %u", val, expected);
  return match(val, expected);
}

// decode a 32bit integer
uint32_t Infrared::decode32()
{
  uint32_t result = 0;
  // 32 bits
  for (uint32_t i = 0; i < 32; ++i) {
    // get the next timing of data
    uint32_t val = next_ir_data();
    // classify mark/space based on the timing
    uint32_t mark = (val > (IR_TIMING * 2)) ? 1 : 0;
    // shift result and OR in mark/space
    result = (result << 1) | mark;
    // check if the space was oddly sized
    if (next_ir_data() > (IR_TIMING * 2)) {
      // this indicates an error in reading/writing
      DEBUG_LOGF("Space %u didn't match: %u", i, IR_TIMING);
      // abort?
      //return false;
    }
  }
  return result;
}

enum recv_state_t {
  WAITING_HEADER_MARK,
  WAITING_HEADER_SPACE,
  READING_DATA,
};

recv_state_t recv_state = WAITING_HEADER_MARK;
uint32_t recv_data = 0;
uint32_t recv_bit = 0;
bool on_mark = true;

void Infrared::recvPCIHandler()
{
  // used to track the changes
  static uint64_t prevTime = 0;
  static uint8_t pinState = HIGH;

  // toggle the tracked pin state
  pinState = !pinState;
  // if we're sending right now don't receive anything
  // otherwise we will receive our own data
  if (sending) {
    return;
  }
  // grab current time
  uint32_t now = micros();
  // don't push the first captured data, or if prevTime is invalid
  if (!prevTime || prevTime > now) {
    prevTime = now;
    return;
  }
  ir_time diff = (ir_time)(now - prevTime);
  DEBUG_LOGF("IR Diff %u", diff);
  prevTime = now;
  switch (recv_state) {
  case WAITING_HEADER_MARK:
    if (diff > HEADER_MARK_MIN) {
      DEBUG_LOG("Matched header mark");
      recv_state = WAITING_HEADER_SPACE;
    }
    DEBUG_LOG("Didn't match header mark");
    return;
  case WAITING_HEADER_SPACE:
    if (diff > HEADER_SPACE_MIN) {
      // success
      DEBUG_LOG("Matched header space");
      recv_state = READING_DATA;
      return;
    }
    DEBUG_LOG("Didn't match header space");
    // failed to read space
    recv_state = WAITING_HEADER_MARK;
  default: // ??
    DEBUG_LOGF("Bad receive state: %u", recv_state);
    return;
  case READING_DATA:
    on_mark = !on_mark;
    // skip spaces
    if (!on_mark) {
      DEBUG_LOG("Ignoring space");
      return;
    }
    // go read data
    break;
  }
  // classify mark/space based on the timing
  uint32_t mark = (diff > (IR_TIMING * 2)) ? 1 : 0;
  // shift result and OR in mark/space
  recv_data = (recv_data << 1) | mark;
  if (mark) {
    DEBUG_LOG("MARK");
  } else {
    DEBUG_LOG("SPACE");
  }
  // count each bit we receive
  recv_bit++;
  // when we get 32 bits push it into the buffer and
  // reset the data and bit counter
  if (recv_bit == 32) {
    DEBUG_LOGF("pushed %x", recv_data);
    g_ir_data.push(recv_data);
    recv_data = 0;
    recv_bit = 0;
  }
}
