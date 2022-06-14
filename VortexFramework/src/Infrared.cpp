#include "Infrared.h"

#include "SerialBuffer.h"
#include "TimeControl.h"
#include "BitStream.h"
#include "Buttons.h"
#include "Leds.h"
#include "Log.h"

#include <Arduino.h>

using namespace std;

#ifndef TEST_FRAMEWORK
// Timer used for PWM, is initialized in initpwm()
Tcc* IR_TCCx2;
#endif

//#define IR_TIMING 564
#define IR_TIMING 1564
#define IR_TIMING_MIN ((uint32_t)(IR_TIMING * 0.1))

#define HEADER_MARK (IR_TIMING * 16)
#define HEADER_SPACE (IR_TIMING * 8)

#define HEADER_MARK_MIN ((uint32_t)(HEADER_MARK * 0.85))
#define HEADER_SPACE_MIN ((uint32_t)(HEADER_SPACE * 0.85))

#define HEADER_MARK_MAX ((uint32_t)(HEADER_MARK * 1.15))
#define HEADER_SPACE_MAX ((uint32_t)(HEADER_SPACE * 1.15))

#define DEFAULT_MARK_EXCESS 50
#define DEFAULT_FRAME_TIMEOUT 7800 //maximum length of SPACE Sbefore we assume frame ended
//DEFAULT_TIMEOUT should be 1.25*the_largest_space_any_valid_IR_protocol_might_have.
//In IRremote library ir_Dish.cpp space they use DISH_RPT_SPACE 6200 while referenced says
//about 6000. If we take 6200*1.25= 7750 rounded up we will use 7800. Previous IRLib
//value was 10000 was probably too large. Thanks to Gabriel Staples for this note.

#define IR_SEND_PWM_PIN 0
#define RECEIVER_PIN 2

// this bool is toggled when actively sending data to prevent
// the receiver from running and receiving our own data
volatile bool receiving = false;

#define IR_BUF_SIZE 1024
uint32_t g_ir_data[IR_BUF_SIZE] = {0};
uint32_t cur_ir_pos = 0;

#define IR_TIMING_BUF_SIZE 2048
uint32_t g_ir_timings[IR_TIMING_BUF_SIZE] = {0};
uint32_t cur_ir_timing = 0;

uint32_t num_dwords = 0;
uint32_t cur_dwords = 0;

enum recv_state_t {
  WAITING_HEADER_MARK,
  WAITING_HEADER_SPACE,
  READING_DATA,
};

recv_state_t recv_state = WAITING_HEADER_MARK;
uint32_t recv_data = 0;
uint32_t recv_bit = 0;
bool on_mark = false;

void resetIRState()
{
  recv_state = WAITING_HEADER_MARK;
  recv_data = 0;
  recv_bit = 0;
  on_mark = false;
  num_dwords = 0;
  cur_dwords = 0;
  cur_ir_timing = 0;
}

void push_ir_data(uint32_t data)
{
  if (cur_ir_pos >= IR_BUF_SIZE) {
    DEBUG_LOGF("DISCARDING: %x", data);
    return;
  }
  if (!cur_ir_pos) {
    DEBUG_LOGF("Num DWORDS: %u", data);
    num_dwords = data;
    cur_dwords = 0;
  } else {
    DEBUG_LOGF("Received DWORD %u/%u: %x", 
      ++cur_dwords, num_dwords, recv_data);
  }
  g_ir_data[cur_ir_pos] = data;
  cur_ir_pos++;
  recv_data = 0;
  recv_bit = 0;
#if 0
  if (cur_dwords > 0 && (cur_dwords % 8) == 0){
    // reset to header but not fully
    recv_state = WAITING_HEADER_MARK;
  }
#endif
}

Infrared::Infrared()
{
}

bool Infrared::init()
{
  pinMode(RECEIVER_PIN, INPUT_PULLUP);
  pinMode(IR_SEND_PWM_PIN, OUTPUT);
  digitalWrite(IR_SEND_PWM_PIN, LOW); // When not sending PWM, we want it low
  receiving = false;
  return true;
}

void Infrared::cleanup()
{
}

bool Infrared::read(SerialBuffer &data)
{
#if 0
  for (uint32_t i = 0; i < cur_ir_timing; ++i) {
    uint32_t val = g_ir_timings[i];
    DEBUG_LOGF("IR time: %u", val);
  }
  cur_ir_timing = 0;
#endif
  if (!cur_ir_pos) {
    return false;
  }
  uint32_t num_dwords = g_ir_data[0];
  data.init(num_dwords * sizeof(uint32_t));
  uint32_t *dwData = (uint32_t *)data.rawData();
  DEBUG_LOG("=====================");
  for (uint32_t i = 1; i < cur_ir_pos; ++i) {
    uint32_t val = g_ir_data[i];
    DEBUG_LOGF("IR val: %x", val);
    *dwData = val;
    dwData++;
  }
  cur_ir_pos = 0;
  return true;
#if 0
  //data.clear();
  //data.init(num_dwords * 4);
  uint32_t *rawBuf = (uint32_t *)data.rawData();
  for (uint32_t i = 0; i < num_dwords; ++i) {
    rawBuf[i] = next_ir_data();
  }
  return true;
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

void log32(uint32_t data)
{
  for (int b = 31; b >= 0; b--) {
    // grab the bit of data at the index
    bool bit = ((data & (1 << b)) != 0);
    // send 3x timing size for 1s and 1x timing for 0
    DEBUG_LOGF("M/S: %u", IR_TIMING + (IR_TIMING * (2 * bit)));
    // send 1x timing size for space
    DEBUG_LOGF("Space: %u", IR_TIMING);
  }
}

bool Infrared::write(SerialBuffer &data)
{
  // stop receiving
  receiving = false;
  // initialize the thing
  initpwm();
  // access the data in dwords
  uint32_t *dwData = (uint32_t *)data.rawData();
  uint32_t num_dwords = (data.rawSize() + 3) / 4;
  if (num_dwords > 10) {
    DEBUG_LOG("TOO MANY DWORDS");
    return false;
  }
  // wakeup the other receiver with a quick mark/space
  mark(50);
  space(100);
  // now send the header
  mark(HEADER_MARK);
  space(HEADER_SPACE);
  // write the number of dwords for the IR decoder
  write32(num_dwords);
  // now iterate the bytes of data
  for (uint32_t i = 0; i < num_dwords; ++i) {
    write32(dwData[i]);
#if 0
    if (i > 0 && ((i + 1) % 8) == 0) {
      // send another mark/space for sync every 8
      //mark(HEADER_MARK);
      space(IR_TIMING * 10);
    }
#endif
  }
  DEBUG_LOGF("Wrote %x", num_dwords);
  for (uint32_t i = 0; i < num_dwords; ++i) {
    //log32(dwData[i]);
    DEBUG_LOGF("Wrote %x", dwData[i]);
#if 0
    if (i > 0 && ((i + 1) % 8) == 0) {
      DEBUG_LOG("SEPARATOR");
    }
#endif
  }
  // can receive again
  receiving = true;
  return true;
}

bool Infrared::beginReceiving()
{
  attachInterrupt(digitalPinToInterrupt(RECEIVER_PIN), Infrared::recvPCIHandler, CHANGE);
  resetIRState();
  receiving = true;
  return true;
}

bool Infrared::endReceiving()
{
  detachInterrupt(digitalPinToInterrupt(RECEIVER_PIN));
  resetIRState();
  receiving = false;
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
#if 0
  uint32_t val = g_ir_data.front();
  g_ir_data.pop();
  DEBUG_LOGF("IR val: %x", val);
  return val;
#endif
  return 0;
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
#if 0
  if (!g_ir_data.size()) {
    DEBUG_LOG("No data to match");
    return false;
  }
  uint32_t val = next_ir_data();
  DEBUG_LOGF("matching %u to expected %u", val, expected);
  return match(val, expected);
#endif
  return true;
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

void Infrared::recvPCIHandler()
{
  // used to track the changes
  static uint64_t prevTime = 0;
  static uint8_t pinState = HIGH;

  // toggle the tracked pin state
  pinState = !pinState;
  // grab current time
  uint32_t now = micros();
  uint32_t diff = (uint32_t)(now - prevTime);
  prevTime = now;
  // if we're sending right now don't receive anything
  // otherwise we will receive our own data
  if (!receiving) {
    resetIRState();
    return;
  }
  if (cur_ir_timing < (sizeof(g_ir_timings) / sizeof(g_ir_timings[0]))) {
    g_ir_timings[cur_ir_timing++] = diff;
  } else {
    DEBUG_LOG("TIMING BUFFER FULL");
  }
  // don't push the first captured data, or if prevTime is invalid
  if (!prevTime || prevTime > now) {
    DEBUG_LOGF("Bad prevtime: %u", prevTime);
    resetIRState();
    return;
  }
  //DEBUG_LOGF("IR Diff %u", diff);
  if (diff > HEADER_MARK_MAX) {
    DEBUG_LOGF("bad delay: %u", diff);
    resetIRState();
    return;
  }
  if (diff < IR_TIMING_MIN) {
    DEBUG_LOG("MIN OK");
    return;
  }
  switch (recv_state) {
  case WAITING_HEADER_MARK:
    if (diff < HEADER_MARK_MIN) {
      DEBUG_LOGF("[%u] Didn't match header mark min %u", diff, HEADER_MARK_MIN);
      resetIRState();
      return;
    }
    DEBUG_LOGF("[%u] Matched header mark min %u", diff, HEADER_MARK_MIN);
    recv_state = WAITING_HEADER_SPACE;
    return;
  case WAITING_HEADER_SPACE:
    if (diff < HEADER_SPACE_MIN) {
      // failed to read space
      DEBUG_LOGF("[%u] Didn't match header space min %u", diff, HEADER_SPACE_MIN);
      resetIRState();
      return;
    }
    // success
    DEBUG_LOGF("[%u] Matched header space min %u", diff, HEADER_SPACE_MIN);
    recv_state = READING_DATA;
    // setup mark reader to only read marks in data
    return;
  default: // ??
    DEBUG_LOGF("Bad receive state: %u", recv_state);
    return;
  case READING_DATA:
    on_mark = !on_mark;
    if (!on_mark) {
      // skip spaces
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
    //DEBUG_LOG("MARK");
  } else {
    //DEBUG_LOG("SPACE");
  }
  //DEBUG_LOGF("Received data %u decoded as %s for bit #%u (curdata: %x)", 
  //  diff, mark ? "MARK" : "SPACE", recv_bit, recv_data);
  // count each bit we receive
  recv_bit++;
  // when we get 32 bits push it into the buffer and
  // reset the data and bit counter
  if (recv_bit == 32) {
    push_ir_data(recv_data);
  }
}
