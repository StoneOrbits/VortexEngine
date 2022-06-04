#include "Infrared.h"

#include "SerialBuffer.h"
#include "TimeControl.h"
#include "BitStream.h"
#include "Buttons.h"
#include "Log.h"

#include <Arduino.h>

#include "Leds.h"

//Timer used for PWM. Is initialized in initializeSAMD21PWM(...)
#ifndef TEST_FRAMEWORK
Tcc* IR_TCCx2;
#endif

#define TIMING_SIZE 564
#define HEADER_MARK_SIZE (TIMING_SIZE * 16)
#define HEADER_SPACE_SIZE (TIMING_SIZE * 8)

#define DEFAULT_MARK_EXCESS 50
#define DEFAULT_FRAME_TIMEOUT 7800 //maximum length of SPACE Sbefore we assume frame ended
//DEFAULT_TIMEOUT should be 1.25*the_largest_space_any_valid_IR_protocol_might_have.
//In IRremote library ir_Dish.cpp space they use DISH_RPT_SPACE 6200 while referenced says
//about 6000. If we take 6200*1.25= 7750 rounded up we will use 7800. Previous IRLib
//value was 10000 was probably too large. Thanks to Gabriel Staples for this note.

#define IR_SEND_PWM_PIN 0
#define RECEIVER_PIN 2

#define MAX_DATA_ENTRIES 1024

volatile bool sending = false;

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

uint32_t Infrared::read()
{
  uint32_t result = 0;
  if (!decode(result)) {
    return false;
  }
  return result;
}

bool Infrared::write(uint32_t data)
{
  uint32_t extent = 0;
  DEBUG_LOGF("Sending %08x", data);
  initpwm();
  sending = true;
  // wakeup the other receiver
  mark(100);
  space(100);
  //Some protocols do not send a header when sending repeat codes. So we pass a zero value to indicate skipping this.
  extent += mark(HEADER_MARK_SIZE);
  extent += space(HEADER_SPACE_SIZE);
  for (int i = 31; i >= 0; i--) {
    if (data & (1 << i)) {
      extent += mark(TIMING_SIZE);
    } else {
      extent += mark(TIMING_SIZE * 3);
    }
    extent += space(TIMING_SIZE);
  }
  sending = false;
  return true;
}

// ===================
//  sending functions

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

#include <queue>
using namespace std;
queue<uint32_t> ir_data;

SerialBuffer databuf;

BitStream bits;

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
    ir_data.push(timeDiff);
  }
  lastTime = now;
  return true;
}

uint32_t next_ir_data()
{
  uint32_t val = ir_data.front();
  ir_data.pop();
  DEBUG_LOGF("IRData: %u", val);
  return val;
}

#define PERCENT_TOLERANCE 45
bool match(uint32_t expected)
{
  if (!ir_data.size()) {
    DEBUG_LOG("No data to match");
    return false;
  }
  uint32_t val = next_ir_data();
  return (val >= (uint16_t)(expected * (1.0 - PERCENT_TOLERANCE / 100.0)))
      && (val <= (uint16_t)(expected * (1.0 + PERCENT_TOLERANCE / 100.0)));
}

bool absmatch(int16_t expected, int16_t tolerance)
{
  if (!ir_data.size()) {
    return false;
  }
  int16_t val = ir_data.front();
  ir_data.pop();
  return (val >= (expected-tolerance)) && (val <= (expected+tolerance));
}

bool Infrared::decode(uint32_t &data)
{
  do {
    poll();
  } while (ir_data.size() < 68);
  while (1) {
    if (match(HEADER_MARK_SIZE)) {
      // the header mark was matched
      break;
    }
    if (!ir_data.size()) {
      return false;
    }
  }
  if (!match(HEADER_SPACE_SIZE)) {
    return false;
  }
  // 32 bits
  for (uint32_t i = 0; i < 32; ++i) {
    // classify mark
    uint32_t val = next_ir_data();
    data <<= 1;
    bool mark = (val < (TIMING_SIZE * 2));
    DEBUG_LOGF("%u = %d", val, mark);
    data |= mark;
    // match space
    if (next_ir_data() > (TIMING_SIZE * 2)) {
      DEBUG_LOGF("Space %u didn't match: %u", i, TIMING_SIZE);
      //return false;
    }
  }
  DEBUG_LOGF("Read %x", data);
  return true;
}

uint64_t prevTime = 0;
uint8_t pinState = HIGH;

void Infrared::recvPCIHandler()
{
  pinState = !pinState;
  if (sending) {
     return;
  }
  volatile uint32_t now = micros();
  uint32_t diff = now - prevTime;
  // don't push the first captured data
  if (prevTime) {
    ir_data.push(diff);
  }
  prevTime = now;
  //// rotating list
  //if (ir_data.size() > MAX_DATA_ENTRIES) {
  //  ir_data.pop();
  //}
}
