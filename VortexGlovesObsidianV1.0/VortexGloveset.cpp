#include "VortexGloveset.h"

#include <FastLED.h>
#include <FlashStorage.h>
#include <Adafruit_DotStar.h>

#include <IRLibSendBase.h>
#include <IRLibDecodeBase.h>
#include <IRLib_P01_NEC.h>
#include <IRLibCombo.h>

#include <IRLibRecv.h>

#include <Arduino.h>


VortexGloveset::VortexGloveset()
{
}

bool VortexGloveset::init()
{
  if (!setupSerial()) {
    // error
    return false;
  }

  if (!setupLEDs()) {
    // error
    return false;
  }

  // initialize a random seed
  // Always generate seed before creating button on digital pin 1(shared pin with analog 0)
  randomSeed(analogRead(0));

  // initialize the button on pin 1
  if (!button.init(1)) {
    // error
    return false;
  }

  //setDefaults();
  //loadSave();
  //menu = MENU_PLAY_MODE;
  //Adafruit_DotStar strip = Adafruit_DotStar(1, 7, 8, DOTSTAR_BGR);
  //strip.begin();
  //strip.show();

  //pattern.refresh(modes[m]);
}

void VortexGloveset::tick()
{
  // tick the clock
  //tick();
  //runMenus();
  //checkButton();
  //checkLimits();
  //checkSerial();


  //FastLED.setBrightness(brightness);
  //FastLED.show();

  //Serial.println(m);

}

// ===================
//  private routines

bool VortexGlovset::setupSerial()
{
  // Setup serial communications
  Serial.begin(9600);
  // may want to add debug logic in here for attaching to a test framework
  return true;
}

bool VortexGlovset::setupLEDs()
{
  // setup leds
  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);
  FastLED.setBrightness(255);
  return true;
}
#if 0
void VortexGlovset::setDefaults() 
{
  brightness = 255;
  demoSpeed = 2;
  importMode("0, 7, 5, 0, 255, 255, 96, 255, 255, 160, 255, 255, 64, 255, 255, 192, 255, 255, 0, 0, 0, 0, 0, 0, 0, 0, 0");
  importMode("1, 25, 3, 0, 255, 255, 96, 255, 255, 160, 255, 255, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0");
  importMode("2, 5, 3, 0, 255, 255, 96, 255, 255, 160, 255, 255, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0");
  importMode("3, 3, 2, 224, 255, 170, 192, 255, 170, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0");
  importMode("4, 32, 3, 0, 255, 170, 96, 255, 170, 160, 255, 170, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0");
  importMode("5, 5, 4, 0, 255, 120, 160, 255, 170, 64, 255, 170, 96, 255, 170, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0");
  importMode("6, 7, 3, 0, 255, 120, 192, 255, 170, 128, 255, 170, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0");
  importMode("7, 1, 3, 16, 255, 170, 96, 255, 255, 192, 255, 85, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0");
  importMode("8, 17, 3, 16, 255, 255, 128, 255, 85, 160, 255, 255, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0");
  importMode("9, 20, 1, 80, 255, 85, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0");
  importMode("10, 4, 8, 0, 255, 85, 32, 255, 170, 64, 255, 255, 96, 255, 85, 128, 255, 85, 160, 255, 85, 192, 255, 170, 224, 255, 85");
  importMode("11, 19, 3, 144, 0, 0, 144, 0, 255, 96, 255, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0");
  importMode("12, 2, 4, 192, 255, 85, 240, 255, 255, 64, 255, 85, 144, 170, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0");
  importMode("13, 8, 5, 16, 255, 0, 144, 255, 0, 16, 255, 85, 144, 255, 255, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0");
}

// Comma separated list of 27 numbers (3 settings + 3 * 8 colors):
// Mode Num, Pattern Num, Num Colors, Color1 H, Color1 S, Color1 V, Color2 H..... Color8 V
void VortexGlovset::importMode(const char input[]) 
{
  char* strtokIndx;
  strcpy(tempChars, input);
  strtokIndx = strtok(tempChars, ",");
  int mNum = atoi(strtokIndx);
  strtokIndx = strtok(NULL, ",");
  modes[mNum].patternNum = atoi(strtokIndx);
  strtokIndx = strtok(NULL, ",");
  modes[mNum].numColors = atoi(strtokIndx);
  for (int col = 0; col < 8; col++) {
    strtokIndx = strtok(NULL, ",");
    modes[mNum].hue[col] = atoi(strtokIndx);
    strtokIndx = strtok(NULL, ",");
    modes[mNum].sat[col] = atoi(strtokIndx);
    strtokIndx = strtok(NULL, ",");
    modes[mNum].val[col] = atoi(strtokIndx);
  }
}

void VortexGlovset::importValues(const char input[]) 
{
  char* strtokIndx;
  strcpy (tempChars, input);
  strtokIndx = strtok (tempChars, ",");
  unsigned long time1 = atol(strtokIndx);
  strtokIndx = strtok (NULL, ",");
  unsigned long time2 = atol (strtokIndx);
  strtokIndx = strtok (NULL, ",");
  unsigned long time3 = atol (strtokIndx);
  strtokIndx = strtok (NULL, ",");
  unsigned long time4 = atol (strtokIndx);
  strtokIndx = strtok (NULL, ",");
  unsigned long time5 = atol (strtokIndx);
  pattern.adjustValues(time1, time2, time3, time4, time5);
  Serial.println((String)time1 + " " + (String)time2 + " " + (String)time3 + " " + (String)time4 + " " + (String)time5);
}
#endif
