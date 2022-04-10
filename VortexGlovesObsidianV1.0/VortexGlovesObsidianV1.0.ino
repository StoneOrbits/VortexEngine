
#include <FastLED.h>
#include <FlashStorage.h>
#include <Adafruit_DotStar.h>

#include <IRLibSendBase.h>
#include <IRLibDecodeBase.h>
#include <IRLib_P01_NEC.h>
#include <IRLibCombo.h>

#include <IRLibRecv.h>

#include "Patterns.h"
#include "Button.h"
#include "Modes.h"

#define HSV_WHITE   { 0,   0,   110 }
#define HSV_ORANGE  { 20,  255, 110 }
#define HSV_BLUE    { 160, 255, 110 }
#define HSV_YELLOW  { 60,  255, 110 }
#define HSV_RED     { 0,   255, 110 }
#define HSV_TEAL    { 120, 255, 110 }
#define HSV_PURPLE  { 120, 255, 110 }

#define NUM_LEDS 28
#define DATA_PIN 4
#define CLOCK_PIN 3

#define totalModes 14 // How many modes the vortex cycles through
#define totalPatterns 40 // How many possible patterns there are

//Objects
//---------------------------------------------------------


CRGB leds[NUM_LEDS];
Modes mode[totalModes];

// There is one button on the vortex gloves
Button button;

Patterns pattern;

CRGB copy[NUM_LEDS];

CRGB boardlight[1];

IRdecode myDecoder;
IRrecv myReceiver(2);
IRsend mySender;

struct Orbit {
  bool dataIsStored;
  uint8_t sHue[totalModes][8];
  uint8_t sSat[totalModes][8];
  uint8_t sVal[totalModes][8];
  uint8_t sNumColors[totalModes];
  uint8_t sPatternNum[totalModes];
  uint8_t brightness;
  uint8_t demoSpeed;
};
FlashStorage(saveData, Orbit);

typedef struct HSVColor {
  uint8_t hue;
  uint8_t sat;
  uint8_t val;
} HSVColor;


//Variable
//---------------------------------------------------------

bool sharing = true, restore = false;
bool on;
int m = 0;
byte menu;
byte stage = 0;
byte frame = 0;
int patNum;
int targetSlot;
byte currentSlot;
int targetZone;
byte colorZone;
int targetHue;
byte selectedHue;
int targetSat;
byte selectedSat;
int targetVal;
byte selectedVal;
int targetList;
int data1[8];
int data2[8];
int data3[8];
bool received1, received2, received3;

unsigned long pressTime, prevPressTime, holdTime, prevHoldTime;

const byte numChars = 128;
char receivedChars[numChars];
char tempChars[numChars];

boolean newData = false;

int dataNumber = 0;

int menuSection, brightVal = 0, prevBrightness = 20;

// Demo mode keeps rolling randomized colors until user confirms or cancels
int demoSpeed = 0;
int newDemoSpeed = 0;
bool demoMode = false;
unsigned long demoTime;

int brightness;

//Main body
//---------------------------------------------------------

void setup() {
  Serial.begin(9600);
  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);
  FastLED.setBrightness(255);
  randomSeed(analogRead(0));//Always generate seed before creating button on digital pin 1(shared pin with analog 0)
  button.init(1); // initialize the button on pin 1
  setDefaults();
  loadSave();
  prevPressTime = 0;
  mode[m].menuNum = 0;
  Adafruit_DotStar strip = Adafruit_DotStar(1, 7, 8, DOTSTAR_BGR);
  strip.begin();
  strip.show();

  pattern.refresh(mode[m]);
}

// typedef of a manu function pointer
typedef void (*menu_func_t)(void);

// have to list prototypes first, this will be fixed once everything is refactored
void playMode();
void menuRingZero();
void menuRingOne();
void colorSet();
void patternSelect();
void modeSharing();
void chooseBrightness();
void chooseDemoSpeed();
void restoreDefaults();
void confirmBlink();

menu_func_t menu_routines[] = {
    playMode,           // 0
    menuRingZero,       // 1: Start Randomizer //Choose Colors //Choose Pattern //Share&Receive Mode
    menuRingOne,        // 2: Global Brightness //Demo Speed //Restore Defaults
    colorSet,           // 3
    patternSelect,      // 4
    modeSharing,        // 5
    chooseBrightness,   // 6
    chooseDemoSpeed,    // 7
    restoreDefaults,    // 8
    confirmBlink,       // 9
};

// the number of menus in above array
#define NUM_MENUS (sizeof(menu_routines) / sizeof(menu_routines[0]))

void checkButton();
void checkSerial();

void loop() {
  runMenus();
  checkButton();
  checkSerial();
  FastLED.setBrightness(brightness);
  FastLED.show();

  //Serial.println(m);
}

int hue, sat, val;

void runMenus() {
    menu = mode[m].menuNum;
    // check for invalid menu, prevent crash
    if (menu >= NUM_MENUS) {
        return;
    }
    // lookup the menu function for the chosen menu
    menu_func_t menu_func = menu_routines[menu];
    // prevent crash, if we leave a menu slot NULL it can do nothing
    if (!menu_func) {
        return;
    }
    // run the menu func
    menu_func();
}

void playMode() {
  if (demoMode) runDemo();

  //  patterns(mode[m].patternNum);

  pattern.playPattern();
  displayPatternResult();

  catchMode();

}

// Randomize colors and pattern every so often
void runDemo() {
  int demoInterval = 0;
  if (demoSpeed == 0) demoInterval = 3000;
  if (demoSpeed == 1) demoInterval = 5000;
  if (demoSpeed == 2) demoInterval = 8000;
  if (demoSpeed == 3) demoInterval = 16000;

  unsigned long mainClock = millis();
  if (mainClock - demoTime > demoInterval) {
    rollColors();
    demoTime = mainClock;
  }
}

//Led controlls for running patterns
//-----------------------------------------------------


void displayPatternResult() {
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = pattern.getLed(i);
  }
}

void getColor(int target) {
  hue = mode[m].hue[target];
  sat = mode[m].sat[target];
  val = mode[m].val[target];
}

void setLed(int target) {
  leds[target].setHSV(hue, sat, val);
}

void setLeds(int first, int last) {
  for (int a = first; a <= last; a++) setLed(a);
}

void clearAll() {
  for (int a = 0; a < 28; a++) leds[a].setHSV(0, 0, 0);
}

void blinkTarget(unsigned long blinkTime) {
  static unsigned long previousClock = 0;
  unsigned long mainClock = millis();
  if (!previousClock || (mainClock - previousClock) > blinkTime) {
    on = !on;
    previousClock = mainClock;
  }
}

// Randomizer
//---------------------------------------------------------

void rollColors() {
  rollPattern();
  int type = random(0, 10);
  //true random, monochrome, complimentary, analogous, triadic, split complimentary, tetradic
  if (type == 0) { // true random
    mode[m].numColors = random(1, 8);
    for (int r = 0; r < 8; r ++) {
      mode[m].hue[r] = random(0, 16) * 16;
      mode[m].sat[r] = random(0, 4) * 85;
      mode[m].val[r] = random(1, 4) * 85;
    }
  }
  if (type == 1) { // monochrome
    mode[m].numColors = 4;
    int tempHue = random(0, 16) * 16;
    for (int r = 0; r < 4; r++) {
      mode[m].hue[r] = tempHue;
      mode[m].sat[r] = r * 85;
      mode[m].val[r] = random(1, 4) * 85;
    }
  }
  if (type == 2) { // complimentary
    mode[m].numColors = 2;
    int tempHue = random(0, 16) * 16;
    int compHue = tempHue + 128;
    if (compHue >= 255) compHue -= 256;
    mode[m].hue[0] = tempHue;
    mode[m].hue[1] = compHue;
    for (int r = 0; r < 2; r++) {
      mode[m].sat[r] = 255;
      mode[m].val[r] = random(1, 4) * 85;
    }
  }
  if (type == 3) { // analogous
    mode[m].numColors = 3;
    int tempHue = random(0, 16) * 16;
    int analHue1 = tempHue - 16;
    if (analHue1 < 0) analHue1 += 256;
    int analHue2 = tempHue + 16;
    if (analHue2 > 255) analHue2 -= 256;
    mode[m].hue[0] = tempHue;
    mode[m].hue[1] = analHue1;
    mode[m].hue[2] = analHue2;
    for (int r = 0; r < 3; r++) {
      mode[m].sat[r] = 255;
      mode[m].val[r] = random(1, 4) * 85;
    }
  }
  if (type == 4) { // triadic
    mode[m].numColors = 3;
    int tempHue = random(0, 16) * 16;
    int triadHue1 = tempHue + 80;
    int triadHue2 = tempHue - 80;
    if (triadHue1 > 255) triadHue1 -= 256;
    if (triadHue2 < 0) triadHue2 += 256;
    mode[m].hue[0] = tempHue;
    mode[m].hue[1] = triadHue1;
    mode[m].hue[2] = triadHue2;
    for (int r = 0; r < 3; r++) {
      mode[m].sat[r] = 255;
      mode[m].val[r] = random(1, 4) * 85;
    }
  }
  if (type == 5) { // split complimentary
    mode[m].numColors = 3;
    int tempHue = random(0, 16) * 16;
    int splitCompHue1 = tempHue + 112;
    int splitCompHue2 = tempHue - 112;
    if (splitCompHue1 > 255) splitCompHue1 -= 256;
    if (splitCompHue2 < 0) splitCompHue2 += 256;
    mode[m].hue[0] = tempHue;
    mode[m].hue[1] = splitCompHue1;
    mode[m].hue[2] = splitCompHue2;
    for (int r = 0; r < 3; r++) {
      mode[m].sat[r] = 255;
      mode[m].val[r] = random(1, 4) * 85;
    }
  }
  if (type == 6) { // tetradic
    mode[m].numColors = 4;
    int tempHue = random(0, 16) * 16;
    int tetradHue1 = tempHue + 48;
    int tetradHue2 = tempHue + 128;
    int tetradHue3 = tempHue + 208;
    if (tetradHue1 > 255) tetradHue1 -= 256;
    if (tetradHue2 > 255) tetradHue2 -= 256;
    if (tetradHue3 > 255) tetradHue3 -= 256;
    mode[m].hue[0] = tempHue;
    mode[m].hue[1] = tetradHue1;
    mode[m].hue[2] = tetradHue2;
    mode[m].hue[3] = tetradHue3;
    for (int r = 0; r < 4; r++) {
      mode[m].sat[r] = 255;
      mode[m].val[r] = random(1, 4) * 85;
    }
  }
  if (type == 7) { // square
    mode[m].numColors = 4;
    int tempHue = random(0, 16) * 16;
    int tetradHue1 = tempHue + 64;
    int tetradHue2 = tempHue + 128;
    int tetradHue3 = tempHue + 192;
    if (tetradHue1 > 255) tetradHue1 -= 256;
    if (tetradHue2 > 255) tetradHue2 -= 256;
    if (tetradHue3 > 255) tetradHue2 -= 256;
    for (int r = 0; r < 4; r++) {
      mode[m].sat[r] = 255;
      mode[m].val[r] = random(1, 4) * 85;
    }
  }
  if (type == 8) { // full rainbow
    mode[m].numColors = 8;
    for (int r = 0; r < 8; r++) {
      mode[m].hue[r] = r * 32;
      mode[m].sat[r] = 255;
      mode[m].val[r] = random(1, 4) * 85;
    }
    bool reroll = random(0, 2);
    if (reroll == 1) rollColors();
  }
  if (type == 9) { // Solid
    mode[m].numColors = 1;
    mode[m].hue[0] = random(0, 16) * 16;
    mode[m].sat[0] = random(0, 4) * 85;
    mode[m].val[0] = random(1, 4) * 85;
  }
  if (mode[m].patternNum == 6 && mode[m].numColors < 3) rollColors();

  int blank = random(0, 4); // randomly chooses to add blanks to colorset
  if (blank == 0) {
    int blankType = 0;
    if (mode[m].numColors == 8) mode[m].val[0] = random(1, 2) * 85;// Dim first color
    if (mode[m].numColors >= 1 && mode[m].numColors <= 7) blankType = 1;
    if (mode[m].numColors >= 2 && mode[m].numColors <= 6) {
      if (mode[m].numColors % 2 == 0) blankType = random (1, 3);
    }
    if (mode[m].patternNum == 6) blankType = 0;

    if (blankType == 1) { // Blank at beginning
      for (int c = 0; c < mode[m].numColors; c++) {
        mode[m].hue[mode[m].numColors - c] = mode[m].hue[mode[m].numColors - (1 + c)];
        mode[m].sat[mode[m].numColors - c] = mode[m].sat[mode[m].numColors - (1 + c)];
        mode[m].val[mode[m].numColors - c] = mode[m].val[mode[m].numColors - (1 + c)];
      }
      mode[m].val[0] = 0;
      mode[m].numColors += 1;
    }
    if (blankType == 2) { // Blank at middle and beginning
      for (int c = 0; c < (mode[m].numColors / 2 + 1 ); c++) {
        mode[m].hue[mode[m].numColors - c] = mode[m].hue[mode[m].numColors - (1 + c)];
        mode[m].sat[mode[m].numColors - c] = mode[m].sat[mode[m].numColors - (1 + c)];
        mode[m].val[mode[m].numColors - c] = mode[m].val[mode[m].numColors - (1 + c)];
      }
      mode[m].val[mode[m].numColors / 2] = 0;
      mode[m].numColors += 1;
      for (int c = 0; c < mode[m].numColors; c++) {
        mode[m].hue[mode[m].numColors - c] = mode[m].hue[mode[m].numColors - (1 + c)];
        mode[m].sat[mode[m].numColors - c] = mode[m].sat[mode[m].numColors - (1 + c)];
        mode[m].val[mode[m].numColors - c] = mode[m].val[mode[m].numColors - (1 + c)];
      }
      mode[m].val[0] = 0;
      mode[m].numColors += 1;
    }
  }
}

void rollPattern() {
  mode[m].patternNum = random(0, totalPatterns);
}

//Menus and settings
//---------------------------------------------------------

void colorSet() {
  if (stage == 0) {
    int numColors = mode[m].numColors;            // get total colors
    clearAll();                                   // clear leds
    hue = 0, sat = 0, val = 40;                   // get "blank" slot color
    setLeds(2, 10);                               // set finger slots

    if (targetSlot <= 4) {                        // if target is in the first 4 colors of a set
      for (int i = 0; i < numColors; i++) {       //
        getColor(i);                              // get slot color
        setLeds(2 + i * 2, 3 + i * 2);            // set leds page 1
      }
    }

    if (numColors > 4) {                             // if there are 4 or more colors
      if (targetSlot >= 4) {                          // and if target slot is greater than 4
        hue = 0, sat = 0, val = 40;                   // get "blank" slot color
        setLeds(2, 10);                               // set finger slots
        for (int i = 4; i < numColors; i++) {         //
          getColor(i);                                // get target color
          setLeds(2 + (i - 4) * 2, 3 + (i - 4) * 2);  // set leds page 2
        }
      }
    }

    if (targetSlot < numColors) {                               // if target slot is less than total colors
      if (on) {                                                 //
        val = 0;                                                //
        if (mode[m].val[targetSlot] == 0) sat = 0, val = 40;    // special condition for blank slot
        if (targetSlot < 4) setLed(2 + targetSlot * 2);         // set first 4 colors
        if (targetSlot >= 4) setLed(2 + (targetSlot - 4) * 2);  // set next 4 colors
      }
      blinkTarget(300);
    }

    if (targetSlot == numColors) {                        // delete last color slot
      if (on) {                                           //
        hue = 0, sat = 0, val = 40;                       //
        if (numColors <= 4) setLed(2 * targetSlot);       // indicate delete slot page 1
        if (numColors > 4) setLed((targetSlot - 4) * 2);  // indicate delete slot page 2
      }
      blinkTarget(60);
    }

    if (targetSlot == numColors + 1 && numColors != 8) {                                                   // add color to set
      if (on) {                                                                     //
        val = 0;                                                                    //
        if (numColors < 4) setLed(2 * targetSlot);                                  // add color page 1
        if (numColors >= 4) setLeds(2 * (targetSlot - 4), 1 + 2 * (targetSlot - 4));// add color page 2
      }
      blinkTarget(300);
    }

    if (targetSlot == numColors + 2 || (numColors == 8 && targetSlot == numColors + 1)) {
      if (on) {
        hue = 0, sat = 0, val = 100;
        setLed(0);
        hue = 85, sat = 255, val = 100;
        setLed(1);
      }

      blinkTarget(300);
    }
  }
  if (stage == 1) colorWheel(0);
  if (stage == 2) colorWheel(1);
  if (stage == 3) colorWheel(2);
  if (stage == 4) colorWheel(3);
}

void colorWheel(int layer) {
  int hue = 0, sat = 255, val = 170;
  if (layer == 0) {
    for (int c = 0; c < 8; c++) {
      hue = c * 32;
      leds[c + 2].setHSV(hue, sat, val);
    }
  }
  if (layer == 1) {
    for (int shade = 0; shade < 4; shade++) {
      hue = (shade * 16) + (64 * colorZone);
      leds[2 + shade * 2].setHSV(hue, sat, val);
      leds[3 + shade * 2].setHSV(hue, sat, val);
    }
  }
  if (layer == 2) {
    for (int fade = 0; fade < 4; fade++) {
      sat = 255 - (85 * fade);
      leds[2 + fade * 2].setHSV(selectedHue, sat, val);
      leds[3 + fade * 2].setHSV(selectedHue, sat, val);
    }
  }
  if (layer == 3) {
    for (int bright = 0; bright < 4; bright ++) {
      val = 255 - (85 * bright);                                    //These set the brightness values
      //if (bright == 2) val = 120;
      leds[2 + bright * 2].setHSV(selectedHue, selectedSat, val);
      leds[3 + bright * 2].setHSV(selectedHue, selectedSat, val);
    }
  }
  if (on) {
    if (layer == 0) {
      leds[2 + targetZone * 2].setHSV(0, 0, 0);
      leds[3 + targetZone * 2].setHSV(0, 0, 0);
    }
    if (layer == 1) {
      leds[2 + targetHue * 2].setHSV(0, 0, 0);
      leds[3 + targetHue * 2].setHSV(0, 0, 0);
    }
    if (layer == 2) {
      leds[2 + targetSat * 2].setHSV(0, 0, 0);
      leds[3 + targetSat * 2].setHSV(0, 0, 0);
    }
    if (layer == 3) {
      leds[2 + targetVal * 2].setHSV(0, 0, 0);
      leds[3 + targetVal * 2].setHSV(0, 0, 0);
    }
    if (layer == 3 && targetVal == 3) {
      leds[2 + targetVal * 2].setHSV(0, 0, 40);
      leds[3 + targetVal * 2].setHSV(0, 0, 40);
    }
  }
  blinkTarget(300);
}

void patternSelect() {
  static bool lightsOn;
  static bool lightsOn2;
  static unsigned long previousClockTime;
  static unsigned long previousClockTime2;
  static unsigned long timerDuration;
  
  unsigned long mainClock = millis();
  if (stage == 0) {
    if (lightsOn2) {
      for (int i = 0; i < 5; i++) {
        sat = 0;
        val = 255;
        setLed(i * 2 + 1);
        hue = 51 * i;
        sat = 255;
        setLed(i * 2);
      }
      timerDuration = 3;
    }
    if (!lightsOn2) {
      clearAll();
      timerDuration = 7;
    }
    if (!lightsOn) {
      leds[targetList * 2 + 1].setHSV(0, 0, 0);
    }
    if (mainClock - previousClockTime > 25) {
      lightsOn = !lightsOn;
      previousClockTime = mainClock;
    }
    if (mainClock - previousClockTime2 > timerDuration) {
      lightsOn2 = !lightsOn2;
      previousClockTime2 = mainClock;
    }
  }
  if (stage == 1) {
    pattern.mode.patternNum = patNum;
    pattern.playPattern();
    displayPatternResult();
  }
}

void modeSharing() {
  if (sharing) shareMode();
  else if (!sharing) receiveMode();
}

void chooseBrightness() {
  clearAll();
  leds[2].setHSV(0, 0, 255);
  leds[3].setHSV(0, 0, 255);
  leds[4].setHSV(0, 0, 185);
  leds[5].setHSV(0, 0, 185);
  leds[6].setHSV(0, 0, 120);
  leds[7].setHSV(0, 0, 120);
  leds[8].setHSV(0, 0, 50);
  leds[9].setHSV(0, 0, 50);
  blinkTarget(300);
  if (brightVal == 0) {
    if (on) {
      leds[2].setHSV(0, 0, 0);
      leds[3].setHSV(0, 0, 0);
    }
  }
  if (brightVal == 1) {
    if (on) {
      leds[4].setHSV(0, 0, 0);
      leds[5].setHSV(0, 0, 0);
    }
  }
  if (brightVal == 2) {
    if (on) {
      leds[6].setHSV(0, 0, 0);
      leds[7].setHSV(0, 0, 0);
    }
  }
  if (brightVal == 3) {
    if (on) {
      leds[8].setHSV(0, 0, 0);
      leds[9].setHSV(0, 0, 0);
    }
  }
}

void chooseDemoSpeed() {
  clearAll();
  if (on) {
    for (int q = 0; q < 4; q++) {
      if (newDemoSpeed == 0) {
        leds[7 * (q) + 0].setHSV(190, 255, 255);
        leds[7 * (q) + 6].setHSV(190, 255, 255);
      }
      if (newDemoSpeed == 1) {
        leds[7 * (q) + 1].setHSV(190, 255, 255);
        leds[7 * (q) + 5].setHSV(190, 255, 255);
      }
      if (newDemoSpeed == 2) {
        leds[7 * (q) + 2].setHSV(190, 255, 255);
        leds[7 * (q) + 4].setHSV(190, 255, 255);
      }
      if (newDemoSpeed == 3) {
        leds[7 * (q) + 3].setHSV(190, 255, 255);
      }
    }
  }
  blinkTarget(100 * demoSpeed + 100);
}

void restoreDefaults() {
  if (restore) {
    if (on) {
      hue = 0, sat = 0, val = 0;
      setLeds(0, 27);
    }
    if (!on) {
      hue = 0, sat = 255, val = 175;
      setLeds(0, 27);
    }
    blinkTarget(100);
  }
  if (!restore) {
    if (on) {
      hue = 0, sat = 255, val = 60;
      setLeds(0, 27);
    }
    if (!on) {
      hue = 0, sat = 0, val = 0;
      setLeds(0, 27);
    }
    blinkTarget(500);
  }
}

void confirmBlink() {
  static unsigned long previousClockTime = 0;
  static int progress = 0;
  
  unsigned long mainClock = millis();
  if (mainClock - previousClockTime > 50) {
    if (progress == 0) clearAll();
    if (progress == 1) sat = 0, val = 175, setLeds(0, 27);
    if (progress == 2) clearAll();
    if (progress == 3) progress = 0, mode[m].menuNum = 0;
    progress++;
    previousClockTime = mainClock;
  }
}

// array of color values for menu ring zero
HSVColor ringZeroCols[] = {
  HSV_WHITE,
  HSV_ORANGE,
  HSV_BLUE,
  HSV_YELLOW,
  HSV_RED,
  HSV_TEAL,
};

// array of color values for menu ring one
HSVColor ringOneCols[] = {
  HSV_YELLOW,
  HSV_PURPLE,
  HSV_RED,
  // second half?
};

void menuRingZero() {
  menuRing(ringZeroCols);
}

void menuRingOne() {
  menuRing(ringOneCols);
}

void menuRing(const HSVColor *menuColors) {
  if (!menuColors) {
    return;
  }
  clearAll();
  // the threshold for how long to hold to activate the menu
  int threshold = 1000 + (1000 * menuSection);
  if (button.holdTime < threshold) {
    return;
  }
  // the amount of time held past the threshold
  int holdTime = (button.holdTime - threshold);
  // the leds turn on in sequence every 100ms another turns on:
  //  000ms = led 0 to 0
  //  100ms = led 0 to 1
  //  200ms = led 0 to 2
  int led = holdTime / 100;
  // only try to turn on 10 leds (0 through 9)
  if (led > 9) led = 9;
  // set the hsv based on the menu section
  hue = menuColors[menuSection].hue;
  sat = menuColors[menuSection].sat;
  val = menuColors[menuSection].val;
  // turn on leds 0 through led
  setLeds(0, led);
}

//Buttons
// button[0] is outer button
// button[1] is inner button
//---------------------------------------------------------

void checkButton() {
  button.buttonState = digitalRead(button.pinNum);
  if (button.buttonState == LOW && button.lastButtonState == HIGH && (millis() - button.pressTime > 200)) {
    button.pressTime = millis();
  }
  button.holdTime = (millis() - button.pressTime);
  if (button.holdTime > 50) {
    //---------------------------------------Button Down-----------------------------------------------------
    if (button.buttonState == LOW && button.holdTime > button.prevHoldTime) {
      if (button.holdTime > 1000 && button.holdTime <= 2000 && menu == 0 && !demoMode) mode[m].menuNum = 1, menuSection = 0;
      if (button.holdTime > 2000 && button.holdTime <= 3000 && menuSection == 0) menuSection = 1;
      if (button.holdTime > 3000 && button.holdTime <= 4000 && menuSection == 1) menuSection = 2;
      if (button.holdTime > 4000 && button.holdTime <= 5000 && menuSection == 2) menuSection = 3;
      if (button.holdTime > 5000 && button.holdTime <= 6000 && menuSection == 3) menuSection = 4;
      if (button.holdTime > 6000 && button.holdTime <= 7000 && menuSection == 4) menuSection = 5;
      if (button.holdTime > 7000 && menuSection == 5) mode[m].menuNum = 5;
    }//======================================================================================================
    // ---------------------------------------Button Up------------------------------------------------------
    if (button.buttonState == HIGH && button.lastButtonState == LOW && millis() - button.prevPressTime > 150) {
      if (menu == 0 && !demoMode) {
        if (button.holdTime <= 300) {
          m++, frame = 0, clearAll(); //, throwMode();
          if (m > totalModes - 1)m = 0;
          pattern.refresh(mode[m]);
        }
        if (button.holdTime > 300 && Serial) exportSettings();
      }
      // press in demo mode
      if (menu == 0 && demoMode) {
        if (button.holdTime <= 3000) {
          saveAll(), frame = 0, mode[m].currentColor = 0;
          pattern.refresh(mode[m]);
          mode[m].menuNum = 9;
          demoMode = false;
        }
      }
      if (menu == 1) {
        if (button.holdTime > 1000 && button.holdTime <= 2000) {
          demoMode = true, frame = 0, mode[m].menuNum = 0, demoTime = millis(), tempSave(), rollColors();
        }
        if (button.holdTime > 2000 && button.holdTime <= 3000) {
          mode[m].menuNum = 3, targetSlot = 0, stage = 0;
        }
        if (button.holdTime > 3000 && button.holdTime <= 4000) {
          mode[m].menuNum = 4, mode[m].currentColor = 0, mode[m].nextColor = 1, stage = 0;
        }
        if (button.holdTime > 4000 && button.holdTime <= 5000) {
          mode[m].menuNum = 6, mode[m].currentColor = 0, mode[m].nextColor = 1;
        }
        if (button.holdTime > 5000 && button.holdTime <= 6000) {
          mode[m].menuNum = 8, mode[m].currentColor = 0, mode[m].nextColor = 1;
        }
        if (button.holdTime > 6000) {
          mode[m].menuNum = 5;
        }
      }
      if (menu == 2) {
        if (button.holdTime > 1000 &&  button.holdTime <= 2000) {
        }
        if (button.holdTime > 2000 && button.holdTime <= 3000) {
        }
        if (button.holdTime > 3000) {
        }
      }
      if (menu == 3) {
        if (button.holdTime <= 300) {
          if (stage == 0) targetSlot++;
          if (stage == 1) targetZone++;
          if (stage == 2) targetHue++;
          if (stage == 3) targetSat++;
          if (stage == 4) targetVal++;
        }
        if (button.holdTime > 300 && button.holdTime < 3000) {
          if (stage == 0) {
            int setSize = mode[m].numColors;
            if (targetSlot < setSize) stage = 1, currentSlot = targetSlot; //choose slot
            if (targetSlot == setSize && mode[m].numColors > 1)targetSlot--, mode[m].numColors--; // delete slot
            if (targetSlot == setSize + 1 && setSize < 8)stage = 1, currentSlot = setSize;  //add slot
            if (targetSlot == setSize + 2 || (targetSlot == setSize + 1 && setSize == 8)) {
              mode[m].currentColor = 0, saveAll(), mode[m].menuNum = 0;
              pattern.refresh(mode[m]);
            }
          }
          else if (stage == 1) stage = 2, colorZone = targetZone;
          else if (stage == 2) stage = 3, selectedHue = (targetHue * 16) + (colorZone * 64);
          else if (stage == 3) stage = 4, selectedSat = 255 - (85 * targetSat);
          else if (stage == 4) {
            selectedVal = 255 - (85 * targetVal);
            if (targetVal == 2) selectedVal = 120;
            mode[m].saveColor(currentSlot, selectedHue, selectedSat, selectedVal);
            stage = 0;
          }
        }
      }
      if (menu == 4) {
        if (button.holdTime <= 300) {
          if (stage == 0) {
            targetList++;
          }
          if (stage == 1) {
            patNum++, frame = 0, mode[m].currentColor = 0, mode[m].nextColor = 1;
          }
        }
        if (button.holdTime > 300 && button.holdTime < 3000) {
          if (stage == 0) {
            stage = 1;
            if (targetList == 0) patNum = 0;
            if (targetList == 1) patNum = 14;
            if (targetList == 2) patNum = 25;
            if (targetList == 3) patNum = 34;
            if (targetList == 4) stage = 0;
          }
          else if (stage == 1) {
            mode[m].menuNum = 9;
            mode[m].patternNum = patNum, saveAll(), frame = 0;//confirm selection
            pattern.refresh(mode[m]);
            stage = 0;
          }
        }
      }
      if (menu == 5) {
        if (button.holdTime <= 500) sharing = !sharing;
        if (button.holdTime > 500 && button.holdTime < 3000) sharing = true, mode[m].menuNum = 9;
      }
      if (menu == 6) {
        if (button.holdTime <= 300) {
          brightVal++;
        }
        if (button.holdTime > 300 && button.holdTime < 3000) {
          mode[m].menuNum = 9;
          if (brightVal == 0) brightness = 255;
          if (brightVal == 1) brightness = 200;
          if (brightVal == 2) brightness = 150;
          if (brightVal == 3) brightness = 100;
          saveAll();
        }
      }
      if (menu == 7) {
        if (button.holdTime <= 300) {
          newDemoSpeed++;
        }
        if (button.holdTime > 300 && button.holdTime < 3000) {
          demoSpeed = newDemoSpeed; demoTime = millis(); saveAll();
          pattern.refresh(mode[m]);
          mode[m].menuNum = 9;
        }
      }
      if (menu == 8) {
        if (button.holdTime <= 300)restore = !restore;
        if (button.holdTime > 300 && button.holdTime < 3000) {
          mode[m].menuNum = 9;
          if (restore) {
            setDefaults();
            saveAll();
            pattern.refresh(mode[m]);
            frame = 0;
            mode[m].currentColor = 0;
          }
        }
      }
      //if (button.holdTime < 4000 && menu == 9)mode[m].menuNum = 7;
      button.prevPressTime = millis();
    }//======================================================================================================
  }

  //these are the max and minimum values for each variable.
  if (newDemoSpeed > 3) newDemoSpeed = 0;
  if (newDemoSpeed < 0) newDemoSpeed = 3;
  if (brightVal > 3) brightVal = 0;
  if (brightVal < 0) brightVal = 3;
  if (menu == 4 && stage == 1) {
    if (targetList == 0) if (patNum > 13) patNum = 0;
    if (targetList == 1) if (patNum > 24) patNum = 14;
    if (targetList == 2) if (patNum > 33) patNum = 25;
    if (targetList == 3) if (patNum > 39) patNum = 34;
  }
  if (patNum > totalPatterns - 1) patNum = 0;
  if (patNum < 0) patNum = totalPatterns - 1;
  int lastSlot = mode[m].numColors + 1;
  if (mode[m].numColors == 8) lastSlot = mode[m].numColors;
  if (targetSlot > lastSlot + 1) targetSlot = 0;
  if (targetSlot < 0) targetSlot = lastSlot + 1;
  if (targetZone > 3) targetZone = 0;
  if (targetZone < 0) targetZone = 3;
  if (targetHue > 3) targetHue = 0;
  if (targetHue < 0) targetHue = 3;
  if (targetSat > 3) targetSat = 0;
  if (targetSat < 0) targetSat = 3;
  if (targetVal > 3) targetVal = 0;
  if (targetVal < 0) targetVal = 3;
  if (targetList > 4) targetList = 0;
  button.lastButtonState = button.buttonState;
  button.prevHoldTime = button.holdTime;
}

int tempH[8], tempS[8], tempV[8], tempNumColors, tempPatternNum;

void tempSave() {
  tempPatternNum = mode[m].patternNum;
  tempNumColors = mode[m].numColors;
  for (int e = 0; e < tempNumColors; e++) {
    tempH[e] = mode[m].hue[e];
    tempS[e] = mode[m].sat[e];
    tempV[e] = mode[m].val[e];
  }
}

void tempLoad() {
  mode[m].patternNum = tempPatternNum;
  mode[m].numColors = tempNumColors;
  for (int e = 0; e < tempNumColors; e++) {
    mode[m].hue[e] = tempH[e];
    mode[m].sat[e] = tempS[e];
    mode[m].val[e] = tempV[e];
  }
}


//Saving/Loading
//---------------------------------------------------------

void loadSave() {
  Orbit myOrbit;
  myOrbit = saveData.read();
  if (myOrbit.dataIsStored == true) {
    for (int modes = 0; modes < totalModes; modes ++) {
      mode[modes].patternNum = myOrbit.sPatternNum[modes];
      mode[modes].numColors = myOrbit.sNumColors[modes];
      for (int c = 0; c < mode[modes].numColors; c++) {
        mode[modes].hue[c] = myOrbit.sHue[modes][c];
        mode[modes].sat[c] = myOrbit.sSat[modes][c];
        mode[modes].val[c] = myOrbit.sVal[modes][c];
      }
      brightness = myOrbit.brightness;
      demoSpeed = myOrbit.demoSpeed;
    }
  }
}
void saveAll() {
  Orbit myOrbit;
  for (int modes = 0; modes < totalModes; modes ++) {
    myOrbit.sPatternNum[modes] = mode[modes].patternNum;
    myOrbit.sNumColors[modes] = mode[modes].numColors;
    for (int c = 0; c < mode[modes].numColors; c++) {
      myOrbit.sHue[modes][c] = mode[modes].hue[c];
      myOrbit.sSat[modes][c] = mode[modes].sat[c];
      myOrbit.sVal[modes][c] = mode[modes].val[c];
    }
    myOrbit.brightness = brightness;
    myOrbit.demoSpeed = demoSpeed;
  }
  myOrbit.dataIsStored = true;
  saveData.write(myOrbit);
}

//IR commuinication
//---------------------------------------------------------
void shareMode() {
  //confirmBlink();
  static bool lightsOn;
  static unsigned long previousClockTime;
  static unsigned long previousClockTime2;
  
  if (lightsOn) {
    for (int d = 0; d < 28; d++) leds[d].setHSV(128, 255, 100);
  }
  if (!lightsOn) {
    for (int d = 0; d < 28; d++) leds[d].setHSV(0, 0, 0);
  }
  unsigned long mainClock = millis();
  if (mainClock - previousClockTime > 20) {
    lightsOn = !lightsOn;
    previousClockTime = mainClock;
  }
  if (mainClock - previousClockTime2 > 500) {
    unsigned long shareBit;
    for (int s = 0; s < 8; s++) {
      Serial.print(mode[m].hue[s]);
      Serial.print(" ");
    }
    Serial.println();
    shareBit = (((unsigned long)mode[m].hue[0] / 16 ) * (unsigned long)0x10000000) +
               (((unsigned long)mode[m].hue[1] / 16 ) * (unsigned long)0x1000000) +
               (((unsigned long)mode[m].hue[2] / 16 ) * (unsigned long)0x100000) +
               (((unsigned long)mode[m].hue[3] / 16 ) * (unsigned long)0x10000) +
               (((unsigned long)mode[m].hue[4] / 16 ) * (unsigned long)0x1000) +
               (((unsigned long)mode[m].hue[5] / 16 ) * (unsigned long)0x100) +
               (((unsigned long)mode[m].hue[6] / 16 ) * (unsigned long)0x10) +
               (unsigned long)1;
    /*mode[m].hue[0]*/
    //Serial.println(shareBit);
    Serial.println(shareBit, HEX);
    mySender.send(NEC, shareBit, 0);
    int sendSat[8];
    int sendVal[8];
    for (int n = 0; n < 8; n++) {
      if (mode[m].sat[n] >= 0 && mode[m].sat[n] < 85) sendSat[n] = 0;
      if (mode[m].sat[n] >= 85 && mode[m].sat[n] < 170) sendSat[n] = 1;
      if (mode[m].sat[n] >= 170 && mode[m].sat[n] < 255)sendSat[n] = 2;
      if (mode[m].sat[n] == 255)sendSat[n] = 3;

      if (mode[m].val[n] >= 0 && mode[m].val[n] < 85) sendVal[n] = 0;
      if (mode[m].val[n] >= 85 && mode[m].val[n] < 170) sendVal[n] = 1;
      if (mode[m].val[n] >= 170 && mode[m].val[n] < 255)sendVal[n] = 2;
      if (mode[m].val[n] == 255)sendVal[n] = 3;
      //Serial.print(sendVal[n]);
      //Serial.print(" ");
    }
    //Serial.println();
    shareBit = ((unsigned long)mode[m].hue[7] * (unsigned long)0x1000000) +
               ((unsigned long)((sendSat[0] * 0x4) + sendSat[1]) * (unsigned long)0x1000000) +
               ((unsigned long)((sendSat[2] * 0x4) + sendSat[3]) * (unsigned long)0x100000) +
               ((unsigned long)((sendSat[4] * 0x4) + sendSat[5]) * (unsigned long)0x10000) +
               ((unsigned long)((sendSat[6] * 0x4) + sendSat[7]) * (unsigned long)0x1000) +
               ((unsigned long)((sendVal[0] * 0x4) + sendVal[1]) * (unsigned long)0x100) +
               ((unsigned long)((sendVal[2] * 0x4) + sendVal[3]) * (unsigned long)0x10) +
               (unsigned long)2;
    Serial.println(shareBit, HEX);
    mySender.send(NEC, shareBit, 0);
    shareBit = ((unsigned long)((sendVal[4] * 0x4) + sendVal[5]) * (unsigned long)0x10000000) +
               ((unsigned long)((sendVal[6] * 0x4) + sendVal[7]) * (unsigned long)0x1000000) +
               ((unsigned long)mode[m].patternNum * (unsigned long)0x10000) +
               ((unsigned long)mode[m].numColors * (unsigned long)0x1000) +
               (unsigned long)3;
    Serial.println(shareBit, HEX);
    mySender.send(NEC, shareBit, 0);
    previousClockTime2 = mainClock;
  }
}

void receiveMode() {
  if (on) {
    for (int d = 0; d < 28; d++) leds[d].setHSV(128, 255, 100);
  }
  if (!on) {
    for (int d = 0; d < 28; d++) leds[d].setHSV(0, 0, 0);
  }
  blinkTarget(750);
  myReceiver.enableIRIn();
  if (myReceiver.getResults()) {
    myDecoder.decode();           //Decode it
    if (myDecoder.value != 0) {
      //myDecoder.dumpResults(false);  //Now print results. Use false for less detail
      unsigned long value = myDecoder.value;
      if (hexValue(0, value) == 1) {
        for (int f = 0; f < 8; f++) {
          data1[f] = hexValue(f, value);
          received1 = true;
        }
        Serial.println("data1");
      }
      if (hexValue(0, value) == 2) {
        for (int f = 0; f < 8; f++) {
          data2[f] = hexValue(f, value);
          received2 = true;
        }
        Serial.println("data2");
      }
      if (hexValue(0, value) == 3) {
        for (int f = 0; f < 8; f++) {
          data3[f] = hexValue(f, value);
          received3 = true;
        }
        Serial.println("data3");
      }
      //for (int f = 0; f < 8; f++) {
      //  Serial.print(hexValue(f, value), HEX);
      //  Serial.print(" ");
      //}
      //Serial.println();
      //Serial.println(value);
      Serial.println(value, HEX);
      myReceiver.enableIRIn();      //Restart receiver
    }
  }
  if (received1 && received2 && received3) {
    Serial.println("data received");
    mode[m].hue[0] = data1[7] * 16;
    mode[m].hue[1] = data1[6] * 16;
    mode[m].hue[2] = data1[5] * 16;
    mode[m].hue[3] = data1[4] * 16;
    mode[m].hue[4] = data1[3] * 16;
    mode[m].hue[5] = data1[2] * 16;
    mode[m].hue[6] = data1[1] * 16;
    mode[m].hue[7] = data2[7] * 16;
    mode[m].sat[0] = (data2[6] / 4) * 85;
    mode[m].sat[1] = (data2[6] % 4) * 85;
    mode[m].sat[2] = (data2[5] / 4) * 85;
    mode[m].sat[3] = (data2[5] % 4) * 85;
    mode[m].sat[4] = (data2[4] / 4) * 85;
    mode[m].sat[5] = (data2[4] % 4) * 85;
    mode[m].sat[6] = (data2[3] / 4) * 85;
    mode[m].sat[7] = (data2[3] % 4) * 85;
    mode[m].val[0] = (data2[2] / 4) * 85;
    mode[m].val[1] = (data2[2] % 4) * 85;
    mode[m].val[2] = (data2[1] / 4) * 85;
    mode[m].val[3] = (data2[1] % 4) * 85;
    mode[m].val[4] = (data3[7] / 4) * 85;
    mode[m].val[5] = (data3[7] % 4) * 85;
    mode[m].val[6] = (data3[6] / 4) * 85;
    mode[m].val[7] = (data3[6] % 4) * 85;
    mode[m].patternNum = (data3[5] * 16) + data3[4];
    mode[m].numColors = data3[3];
    received1 = false;
    received2 = false;
    received3 = false;
    mode[m].menuNum = 0;
    saveAll();
  }
}

unsigned long hexValue(int place, unsigned long number) {
  for (int p = 0; p < place; p++) number /= 0x10;
  return number % 0x10;
}

void throwMode() {
  unsigned long shareBit;
  shareBit = (m * (unsigned long)0x10000000) +
             (0 * (unsigned long)0x1000000) +
             (0 * (unsigned long)0x100000) +
             (0 * (unsigned long)0x10000) +
             (0 * (unsigned long)0x1000) +
             (0 * (unsigned long)0x100) +
             (0 * (unsigned long)0x10) +
             (unsigned long)4;
  mySender.send(NEC, shareBit, 0);
}

void catchMode() {
  myReceiver.enableIRIn();
  if (myReceiver.getResults()) {
    myDecoder.decode();           //Decode it
    if (myDecoder.value != 0) {
      //myDecoder.dumpResults(false);  //Now print results. Use false for less detail
      unsigned long value = myDecoder.value;
      if (hexValue(0, value) == 4) {
        m = hexValue(7, value);
        Serial.println("Mode Change Caught");
      }
      myReceiver.enableIRIn();      //Restart receiver
    }
  }
}

//Serial Mode Transfer (usb)
//---------------------------------------------------------

void exportSettings() {
  Serial.println("Each line below contains 1 mode, copy and paste them to the line above to upload it!");
  for (int mo = 0; mo < totalModes; mo++) {
    Serial.print("<");
    Serial.print(mo);
    Serial.print(", ");
    Serial.print(mode[mo].patternNum);
    Serial.print(", ");
    Serial.print(mode[mo].numColors);
    Serial.print(", ");
    for (int co = 0; co < 8; co++) {
      Serial.print(mode[mo].hue[co]);
      Serial.print(", ");
      Serial.print(mode[mo].sat[co]);
      Serial.print(", ");
      Serial.print(mode[mo].val[co]);
      if (co != 7) Serial.print(", ");
    }
    Serial.println(">");
  }
}

void checkSerial() {
  recvWithStartEndMarkers();
  importData();
}

void recvWithStartEndMarkers() {
  static boolean recvInProgress = false;
  static byte ndx = 0;
  char startMarker = '<';
  char endMarker = '>';
  char rc;

  static boolean altRec = false;
  char altStart = '[';
  char altEnd = ']';

  if (Serial.available() > 0 && newData == false) {
    rc = Serial.read();
    if (recvInProgress == true) {
      if (rc != endMarker) {
        receivedChars[ndx] = rc;
        ndx++;
        if (ndx >= numChars) {
          ndx = numChars - 1;
        }
      }
      else {
        receivedChars[ndx] = '\0'; // terminate the string
        recvInProgress = false;
        ndx = 0;
        newData = true;
      }
    }

    else if (rc == startMarker) {
      recvInProgress = true;
    }

    if (altRec == true) {
      if (rc != altEnd) {
        receivedChars[ndx] = rc;
        ndx ++;
        if (ndx >= numChars) {
          ndx = numChars - 1;
        }
      }
      else {
        receivedChars[ndx] = '\0';
        altRec = false;
        ndx = 0;
        importValues(receivedChars);
      }
    }
    else if (rc == altStart) {
      altRec = true;
    }
  }
}

void importData() {
  bool dataIsValid = false;
  char * strtokIndx; // this is used by strtok() as an index
  if (newData == true) {
    newData = false;
    if (!dataIsValid) {
      strcpy(tempChars, receivedChars);
      strtokIndx = strtok(tempChars, ",");
      if (atoi(strtokIndx) >= totalModes) {
        Serial.println("Invalid input. Mode number: too high");
        return;
      }
      strtokIndx = strtok(NULL, ",");
      if (atoi(strtokIndx) >= totalPatterns) {
        Serial.println("Invalid input. Pattern number: too high");
        return;
      }
      strtokIndx = strtok(NULL, ",");
      if (atoi(strtokIndx) < 1) {
        Serial.println("Invalid input. Number of colors: too low");
        return;
      }
      if (atoi(strtokIndx) > 8) {
        Serial.println("Invalid input. Number of colors: too high");
        return;
      }
      for (int col = 0; col < 8; col++) {
        strtokIndx = strtok(NULL, ",");
        if (atoi(strtokIndx) > 255) {
          Serial.println("Invalid input. Hue " + (String)col + ": too high");
          return;
        }
        strtokIndx = strtok(NULL, ",");
        if (atoi(strtokIndx) > 255) {
          Serial.println("Invalid input. Saturation " + (String)col + ": too high");
          return;
        }
        strtokIndx = strtok(NULL, ",");
        if (atoi(strtokIndx) > 255) {
          Serial.println("Invalid input. Brightness " + (String)col + ": too high");
          return;
        }
      }
      dataIsValid = true;
    }
    if (dataIsValid) {
      importMode(receivedChars);
      Serial.println("Data recieved");
      Serial.println(receivedChars);
      saveAll();
      pattern.refresh(mode[m]);
      dataIsValid = false;
    }
  }
}

// Comma separated list of 27 numbers (3 settings + 3 * 8 colors):
// Mode Num, Pattern Num, Num Colors, Color1 H, Color1 S, Color1 V, Color2 H..... Color8 V
void importMode(const char input[]) {
  char* strtokIndx;
  strcpy(tempChars, input);
  strtokIndx = strtok(tempChars, ",");
  int mNum = atoi(strtokIndx);
  strtokIndx = strtok(NULL, ",");
  mode[mNum].patternNum = atoi(strtokIndx);
  strtokIndx = strtok(NULL, ",");
  mode[mNum].numColors = atoi(strtokIndx);
  for (int col = 0; col < 8; col++) {
    strtokIndx = strtok(NULL, ",");
    mode[mNum].hue[col] = atoi(strtokIndx);
    strtokIndx = strtok(NULL, ",");
    mode[mNum].sat[col] = atoi(strtokIndx);
    strtokIndx = strtok(NULL, ",");
    mode[mNum].val[col] = atoi(strtokIndx);
  }
}

void importValues(const char input[]) {
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
//Default Modes
//---------------------------------------------------------

void setDefaults() {
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
