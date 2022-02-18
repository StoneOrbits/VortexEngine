
#include <FastLED.h>
#include <FlashStorage.h>
#include "Modes.h"
#include "Buttons.h"

#include <Adafruit_DotStar.h>

#include <IRLibSendBase.h>
#include <IRLibDecodeBase.h>
#include <IRLib_P01_NEC.h>
#include <IRLibCombo.h>

#include <IRLibRecv.h>


#define NUM_LEDS 28
#define DATA_PIN 4
#define CLOCK_PIN 3

#define totalModes 14 // How many modes the vortex cycles through
#define totalPatterns 33 // How many possible patterns there are

//Objects
//---------------------------------------------------------

CRGB leds[NUM_LEDS];
Modes mode[totalModes];
Buttons button[3];

CRGB copy[NUM_LEDS];

CRGB boardlight[1];

IRdecode myDecoder;
IRrecv myReceiver(2);
IRsend mySender;

typedef struct Orbit {
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

//Variable
//---------------------------------------------------------

bool sharing = true, restore = false;
bool dataStored = 0;
bool on, on2, on3;
int m = 0;
byte menu;
byte stage = 0;
byte frame = 0;
byte qBand;
int gap;
int patNum;
byte dot = 0;
byte k = 0;
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
bool buttonState, lastButtonState;
unsigned long mainClock, prevTime, duration, prevTime2, duration2;
int data1[8];
int data2[8];
int data3[8];
bool received1, received2, received3;
int rep = 0;
int finger = 0;

unsigned long prevTime3, prevTime4;
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
  button[0].createButton(1);
  button[1].createButton(20);
  setDefaults();
  loadSave();
  prevPressTime = 0;
  prevTime = 0;
  duration = 0;
  mode[m].menuNum = 0;

  Adafruit_DotStar strip = Adafruit_DotStar(1, 7, 8, DOTSTAR_BGR);
  strip.begin();
  strip.show();
}

void loop() {
  menu = mode[m].menuNum;
  if (menu == 0) playMode();
  if (menu == 1) menuRing(0); //Button 1: Start Randomizer //Choose Colors //Choose Pattern //Share&Receive Mode
  if (menu == 2) menuRing(1); //Button 2: Global Brightness //Demo Speed //Restore Defaults
  if (menu == 3) colorSet();
  if (menu == 4) patternSelect();
  if (menu == 5) modeSharing();
  if (menu == 6) chooseBrightness();
  if (menu == 7) chooseDemoSpeed();
  if (menu == 8) restoreDefaults();
  if (menu == 9) confirmBlink();

  checkButton();
  checkSerial();
  FastLED.setBrightness(brightness);
  FastLED.show();
  //Serial.println(m);
}

int hue, sat, val;

void playMode() {
  mainClock = millis();

  if (demoMode) runDemo();

  patterns(mode[m].patternNum);
  catchMode();
}

//Patterns
//---------------------------------------------------------
// To add new patterns add a switch to this method and update totalPatterns.
// In your pattern code, getColor() will set the hue, sat, val variables for this iteration.
// Then call setLeds() to set those values on to the leds you want.
// The LEDs are arranged from the center out and around the other side then back to center.
// So 0,1,2 is one arm, 3 is the edge, then 4,5,6 [o] 7,8,9 on the other side then 10 on the edge. Etc...
// this code will repeat forever, incrementing "mainClock" with each iteration.

void patterns(int pat) {

  int totalColors = mode[m].numColors;
  int currentColor = mode[m].currentColor;
  int currentColor1 = mode[m].currentColor1;
  int next = mode[m].nextColor;

  switch (pat) {
    case 1: { // All tracer (Synchronized tracer)
        getColor(0);                        // Get color 0
        setLeds(0, 9);                      // Set all LEDs
        if (on) {                           // when blink is on
          getColor(currentColor);           // get the current color in the set
          if (totalColors == 1) val = 0;    // (special case when set has 1 color)
          setLeds(0, 9);                    // set all LEDs
          duration = 2;                     // set Timer to 2ms
        }
        if (!on) duration = 10;                 // when blink is on set Timer to 10ms
        if (mainClock - prevTime > duration) {  // check if timer duration has passed
          if (!on)nextColor(1);                 // when blink is not on, que next color (excluding color 0)
          on = !on;                             // flip blink on/off
          prevTime = mainClock;                 // refresh timer counter
        }
        break;
      }

    case 2: { // SparkleTrace (Randomized tracer)
        getColor(0);                        // Get color 0
        setLeds(0, 9);                      // Set all LEDs
        if (on) {                           // when blink is on
          getColor(currentColor);           // get the current color in the set
          if (totalColors == 1) val = 0;    // (special case when set has 1 color)
          for (int c = 0; c < 4; c++) {     // for 4 repititions
            int r = random(0, 5);
            setLeds(r * 2, r * 2 + 1);                // choose a random pixel
          }
        }
        if (!on) nextColor (1);             // when blink is not on, que next color (excluding color 0)
        on = !on;                           // flip blink on/off
        break;
      }

    case 3: { // Vortex                   (finger chase with tip/top dops flip)
        getColor(currentColor);           // get the current color in the set
        clearAll();                       // clear Leds
        if (on) {                         // when blink is on
          if (on2) {                      // and when blink2 is on
            setLed(0);                    // set Led 0, 4, and 8
            setLed(4);
            setLed(8);
          }
          if (!on2) {                     // and when blink2 is off
            setLed(1);                    // set Led 1, 5, 9
            setLed(5);
            setLed(9);
          }
        }
        if (!on) {                        // when blink is off
          if (on2) {                      // and when blink2 is on
            setLed(2);                    // set Led 2, 6
            setLed(6);
          }
          if (!on2) {                     // and when blink2 is off
            setLed(3);                    // set Led 3,7
            setLed(7);
          }

        }
        if (mainClock - prevTime > 2) {   // check if Timer has passed 2ms
          on2 = !on2;                     // flip blink2 on/off
          nextColor(0);                   // que next color
          prevTime = mainClock;           // refresh timer
        }
        if (mainClock - prevTime2 > 125) { // check if Timer2 has passed 125ms
          on = !on;                        // flip blink on/off
          prevTime2 = mainClock;           // refresh Timer2
        }

        break;
      }

    case 4: { // Dot Zip (Dot zips from thumb to pinkie Tops while dot2 zips pinkie to thumb tips with dops)
        clearAll();                         // clear all
        if (on) {
          getColor(currentColor);           // get current color
          setLed(rep * 2);                  // set next top Led
          getColor(next);                   // get next color
          setLed(9 - rep * 2);              // set next tip Led
        }
        if (mainClock - prevTime > 50) {    // check if timer has passed 50ms
          rep ++;                           // next repetition
          if (rep > 4) {                    // if repetition is more than 4
            rep = 0;                        // reset repetition count
            nextColor(0);                   // que next color
          }
          prevTime = mainClock;             // refresh timer
        }
        if (mainClock - prevTime2 > 3) {    // check if timer2 has passed 4ms
          on = !on;                         // flip blink on/off
          prevTime2 = mainClock;            // refresh timer2
        }
        break;
      }

    case 5: { // Cross strobe   (dops on alternating tips and tops)
        clearAll();               // clear leds
        getColor(currentColor);   // get current color
        if (on) {                 // when blink is on
          if (on2) {              // and blink2 is on
            setLed(0);            // set led 0,3,4,7,8
            setLed(3);
            setLed(4);
            setLed(7);
            setLed(8);
          }
        }
        if (!on) {                // when blink is not on
          if (on2) {              // and blink2 is on
            setLed(1);            // set led 1,2,5,6,9
            setLed(2);
            setLed(5);
            setLed(6);
            setLed(9);
          }
        }
        if (mainClock - prevTime > 100) { // check if timer has passed 100ms
          nextColor (0);                  // que next color
          on = !on;                       // flip blink on/off
          prevTime = mainClock;           // refresh timer
        }
        if (mainClock - prevTime2 > 2) {  // check if timer2 has passed 2ms
          on2 = !on2;                     // flip blink2 on/off
          prevTime2 = mainClock;          // refresh timer2
        }
        break;
      }

    case 6: { // Impact (Rabbit/SOS) (independend thumbs tops and tips)
        clearAll();
        getColor(0);                            // Set thumb to blink first color infrequently
        if (on2) duration = 5, setLeds(0, 1);   //
        if (!on2) duration = 300;               //

        getColor(1);                                                                    // Set tops to color 2
        if (totalColors == 1) val = 0;                                                  //
        if (on3) for (int fingers = 0; fingers < 4; fingers++) setLed(2 + fingers * 2); //

        if (on) {
          getColor(currentColor);                                                 // Set tips to next color starting from 3rd color
          if (totalColors <= 2) val = 0;                                          //
          for (int fingers = 0; fingers < 4; fingers++) setLed(3 + fingers * 2);  //
          nextColor(2);                                                           //
        }
        if (mainClock - prevTime > 5) on = !on, prevTime = mainClock;              // timer for tips/tops
        if (mainClock - prevTime2 > duration) on2 = !on2, prevTime2 = mainClock;   // timer for thumb
        if (mainClock - prevTime3 > 5) on3 = !on3, prevTime3 = mainClock;
        break;
      }

    case 7: { // Blend (gradient between selected colors with dops blink)
        clearAll();
        if (on) {                                                                    // calculate next step
          getColor(currentColor);                                                    // between current and
          int color1 = mode[m].hue[currentColor];                                    // next colors and set
          int color2 = mode[m].hue[next];                                            // all leds
          if (color1 > color2 && color1 - color2 < (255 - color1) + color2)gap--;    //
          if (color1 > color2 && color1 - color2 > (255 - color1) + color2)gap++;    //
          if (color1 < color2 && color2 - color1 < (255 - color2) + color1)gap++;    //
          if (color1 < color2 && color2 - color1 > (255 - color2) + color1)gap--;    //
          if (color1 + gap >= 255) gap -= 255;                                       //
          if (color1 + gap < 0) gap += 255;                                          //
          int finalHue = color1 + gap;                                               //
          if (finalHue == color2) gap = 0, nextColor(0);                             //
          for (int a = 0; a < 9; a++) leds[a].setHSV(finalHue, sat, val);            //
        }
        if (mainClock - prevTime > 3) { // timer for dops
          on = !on;                     //
          prevTime = mainClock;         //
        }
        break;
      }

    case 8: { // Mini Ribbon (chroma but tighter)
        if (mainClock - prevTime > 3) {  // set the next color
          getColor(currentColor);        // at high frequency
          setLeds(0, 9);                 // with no gaps
          nextColor(0);                  //
          prevTime = mainClock;          //
        }

        break;
      }

    case 9: { // Dops Crush (breifly flash all colors in set before a pause)
        if (on) {                                               //
          clearAll();                                           //
          if (mainClock - prevTime > 50) {                      //
            on = !on;                                           //
            prevTime = mainClock;                               //
          }
        }
        if (!on) {                                              //
          if (mainClock - prevTime2 > 1) {
            getColor(currentColor);                               //
            setLeds(0, 9);                                        //
            if (currentColor == totalColors - 1) on = !on;        //
            nextColor(0);
            prevTime2 = mainClock;
          }                                                       //
        }

        break;
      }

    case 10: { // Meteor (randomly flash Leds while constantly dimming)
        for (int a = 0; a < NUM_LEDS; a++)leds[a].fadeToBlackBy(75);
        if (mainClock - prevTime > 1) {
          getColor(currentColor);
          setLed(random(0, 10));
          nextColor (0);
          prevTime = mainClock;
        }

        break;
      }

    case 11: { //Carnival/Double Strobe (flash next 2 colors simultaneously)
        clearAll();
        if (on) {                               // Strobe 1
          for (int i = 0; i < NUM_LEDS; i++) {  //
            if (i % 2 == 0) {                   //
              getColor(currentColor);           //
              setLed(i);                        //
            }
            if (i % 2 == 1) {                   // Strobe 2
              getColor(next);                   //
              setLed(i);                        //
            }
          }
        }
        if (mainClock - prevTime > 100) {       // Color timer
          nextColor(0);                         //
          prevTime = mainClock;                 //
        }

        if (mainClock - prevTime2 > 7) {       // Strobe timer
          on = !on;                             //
          prevTime2 = mainClock;                //
        }

        break;
      }

    case 12: { // Vortex Wipe   (Color wipe across tops then tips with dops)
        getColor(currentColor);       //
        if (on) {                     //
          duration = 2;
          setLeds(0, 9);              //
          getColor(next);             //
          setLed(0);                  //
          if (rep >= 1) setLed(2);    // Wipe progress
          if (rep >= 2) setLed(4);    //
          if (rep >= 3) setLed(6);    //
          if (rep >= 4) setLed(8);    //
          if (rep >= 5) setLed(9);    //
          if (rep >= 6) setLed(7);    //
          if (rep >= 7) setLed(5);    //
          if (rep >= 8) setLed(3);    //
          if (rep >= 9) setLed(1);    //
        }
        if (!on){
          clearAll();
          duration = 4;
        }
        if (mainClock - prevTime > 50) {  // Wipe timer
          prevTime = mainClock;           //
          rep ++;                         //
          if (rep > 9) {                  //
            rep = 0;                      //
            nextColor(0);                 //
          }
        }
        if (mainClock - prevTime2 > duration) {  // Dops timer
          prevTime2 = mainClock;          //
          on = !on;                       //
        }
        break;
      }

    case 13: { // Warp Worm (Group of color 1 lights the travel from thumb to pinky with dops)
        clearAll();                         //
        if (on) {                           //
          getColor(0);                      //
          setLeds(0, 9);                    //
          getColor(currentColor);           //
          for (int i = 0; i < 6; i++) {     //
            int chunk = i + k;              //
            if (chunk > 9) chunk -= 10;     //
            setLed(chunk);                  //
          }
        }
        if (mainClock - prevTime > 50) {    //
          k++;                              //
          prevTime = mainClock;             //
          if (k > 9) k = 0, nextColor(1);   //
        }
        if (mainClock - prevTime2 > 3) {    //
          on = !on;                         //
          prevTime2 = mainClock;            //
        }
        break;
      }

    case 14: { // MegaDops (dops but a lot)
        if (on) {                               //
          getColor(currentColor);               //
          setLeds(0, 9);                        //
          duration = 1;                         //
        }
        if (!on) {                              //
          clearAll();                           //
          duration = 3;                         //
        }
        if (mainClock - prevTime > duration) {  //
          if (!on)nextColor(0);                 //
          on = !on;                             //
          prevTime = mainClock;                 //
        }

        break;
      }

    case 15: { // Warp (Dot travels from thumb to pinkie then next color with dops)
        clearAll();                       //
        if (on) {                         //
          getColor(currentColor);         //
          setLeds(0, 9);                  //
          getColor(next);                 //
          setLed(dot);                    //
        }

        if (mainClock - prevTime > 50) {  //
          dot++;                          //
          if (dot >= 10) {                //
            dot = 0;                      //
            nextColor(0);                 //
          }
          prevTime = mainClock;           //
        }
        if (mainClock - prevTime2 > 3) {  //
          on = !on;                       //
          prevTime2 = mainClock;          //
        }

        break;
      }

    case 16: { // Full Warp Worm  (Group of all colors travel from thumb to pinkie with dops)
        clearAll();                           //
        if (on) {                             //
          getColor(0);                        // set trace
          setLeds(0, 9);                      //
          getColor(currentColor);             //
          for (int i = 0; i < 3; i++) {       // set worm
            int chunk = i + k;                //
            if (chunk > 9) chunk -= 10;       //
            setLed(chunk);                    //
          }
        }
        if (mainClock - prevTime > 15) {              // color/worm timer
          if (currentColor == totalColors - 1) k++;   //
          prevTime = mainClock;                       //
          if (k > 9) k = 0;                           //
          nextColor(1);                               //
        }
        if (mainClock - prevTime2 > 3) {              // dops timer
          on = !on;                                   //
          prevTime2 = mainClock;                      //
        }

        break;
      }

    case 17: { //Warp Fade (Warp from thumb to pinkie with constant fade with dops)
        getColor(currentColor);               // get the next color
        setLed(dot);                          // set to the next finger
        if (mainClock - prevTime > 50) {      // Finger/color timer
          dot++;                              //
          if (dot % 2 == 0) nextColor(0);     //
          if (dot > 9) {                      //
            dot = 0;                          //
          }                                   //
          prevTime = mainClock;               //
        }
        if (on) {                                                          //
          if (mainClock - prevTime2 > 50) {                                // Fade timer
            for (int a = 0; a < NUM_LEDS; a++) leds[a].fadeToBlackBy(50);  //
            prevTime2 = mainClock;                                         //
          }                                                                //
        }                                                                  //
        if (mainClock - prevTime3 > 3) {          // dops timer
          on = !on;                               //
          if (on) {                               //
            for (int a = 0; a < NUM_LEDS; a++) {  // special save/load
              leds[a] = copy[a];                  // for fade effect
            }                                     // with dops
          }                                       //
          if (!on) {                              //
            for (int a = 0; a < NUM_LEDS; a++) {  //
              copy[a] = leds[a];                  //
            }                                     //
            clearAll();                           //
          }                                       //
          prevTime3 = mainClock;                  //
        }

        break;
      }

    case 18: { // Zip fade (Zip with a constant fade with dops)
        getColor(next);                     //
        setLed(rep * 2);                    // Top zip
        getColor(currentColor);             //
        setLed(9 - rep * 2);                // Tip zip
        if (mainClock - prevTime > 100) {   // Zip timer
          rep ++;                           //
          nextColor(0);                     // next color each zip step
          if (rep > 4) {                    //
            rep = 0;                        //
          }
          prevTime = mainClock;             //
        }
        if (on) {
          if (mainClock - prevTime2 > 30) {                               // fade timer
            for (int a = 0; a < NUM_LEDS; a++)leds[a].fadeToBlackBy(50);  //
            prevTime2 = mainClock;                                        //
          }
        }
        if (mainClock - prevTime3 > 3) {          // dops timer
          on = !on;                               //
          if (on) {                               //
            for (int a = 0; a < NUM_LEDS; a++) {  // special save/load
              leds[a] = copy[a];                  // for fade effect
            }                                     // with dops
          }                                       //
          if (!on) {                              //
            for (int a = 0; a < NUM_LEDS; a++) {  //
              copy[a] = leds[a];                  //
            }                                     //
            clearAll();                           //
          }                                       //
          prevTime3 = mainClock;                  //
        }

        break;
      }

    case 19: { ///Chroma rezz (dops/tracer tips tops swap)
        if (on) {                                             // dops on
          getColor(mode[m].currentColor);                     // get dops color
          for (int finger = 0; finger < 5; finger++) {        //
            if (rep == 0) setLed(2 * finger);                 // set dops
            if (rep == 1) setLed(2 * finger + 1);             //
          }
          nextColor(0);                                       // next color
        }
        if (!on) {                                            // dops off
          for (int finger = 0; finger < 5; finger++) {        //
            if (rep == 0) clearLight(2 * finger);             //
            if (rep == 1) clearLight(2 * finger + 1);         //
          }
        }
        if (mainClock - prevTime > 5) {                       // dops rate
          on = !on;                                             //
          prevTime = mainClock;                                 //
        }

        getColor(0);                                          // get first color
        for (int finger = 0; finger < 5; finger++) {          //
          if (rep == 0) setLed(2 * finger + 1);               // set trace on
          if (rep == 1) setLed(2 * finger);
        }
        if (on2) {                                            // trace dop on
          getColor(mode[m].currentColor1);
          if (mode[m].numColors == 1) val = 0;                //
          duration = 5;                                       // dop length
          for (int finger = 0; finger < 5; finger++) {        //
            if (rep == 0) setLed(2 * finger + 1);             // set dop
            if (rep == 1) setLed(2 * finger);
          }
        }
        if (!on2) duration = 100;                             // trace length
        if (mainClock - prevTime2 > duration) {               // trace timing
          if (!on2)nextColor1(1);                             //
          on2 = !on2;                                         //
          prevTime2 = mainClock;                              //
        }
        if (mainClock - prevTime3 > 1000) {                   //switch timing
          rep++;
          if (rep > 1) rep = 0;
          prevTime3 = mainClock;
        }

        break;
      }

    case 20: { // Backstrobe (Hyperstrobe/dops tips tops swap)
        if (on) {                                                                   // hyperstrobe on
          getColor(mode[m].currentColor);                                           // get color
          for (int finger = 0; finger < 5; finger++) setLed(2 * finger + rep);      // set half leds
        }
        if (!on) {                                                                  // hyperstrobe off
          for (int finger = 0; finger < 5; finger++) clearLight(2 * finger + rep);  // set half leds
        }
        if (mainClock - prevTime > 50) {                                            // hyperstrobe rate
          on = !on;                                                                 //
          if (!on) nextColor(0);                                                    // next hyperstrobe color
          prevTime = mainClock;                                                     //
        }

        getColor(mode[m].currentColor1);                                            // get dop color
        for (int finger = 0; finger < 5; finger++) {                                //
          if (on2) {                                                                // dops on
            if (rep == 0) setLed(2 * finger + 1);                                   //
            if (rep == 1) setLed(2 * finger);                                       //
          }                                                                         //
          else {                                                                    // dops off
            if (rep == 0) clearLight(2 * finger + 1);                               //
            if (rep == 1) clearLight(2 * finger);                                   //
          }
        }

        if (mainClock - prevTime2 > 2) {                                            // dops rate
          on2 = !on2;                                                               //
          if (!on2) nextColor1(0);                                                  // next dops color
          prevTime2 = mainClock;                                                    //
        }

        if (mainClock - prevTime3 > 1000) {                                         // switch rate
          rep++;                                                                    //
          if (rep > 1) rep = 0;                                                     //
          prevTime3 = mainClock;                                                    //
        }

        break;
      }

    case 21: { // Dash dops (first color dash, rest of the colors dops)
        if (on) {
          getColor(currentColor);
          setLeds(0, 27);
          if (currentColor == 0) duration = 20;
          else duration = 1;
        }
        if (!on) {
          clearAll();
          duration = 5;
        }
        if (mainClock - prevTime > duration) {
          if (!on)nextColor(0);
          on = !on;
          prevTime = mainClock;
        }
        break;
      }

    case 22: { // Tip Top (Tips hyperstrobe color 1, tops strobe full set)
        for (int i = 0; i < NUM_LEDS; i++) {      //
          if (i % 2 == 1) {                       // Tips (odds)
            if (on) {                             //
              getColor(0);                        // get first color
              setLed(i);                          // blink tips on
            }
            else clearLight(i);                   // blink tips off
          }
          if (i % 2 == 0) {                       // Tops (evens)
            if (on2) {                            //
              getColor(currentColor);             // get current color
              setLed(i);                          // blink tops on
            }
            else clearLight(i);                   // blink tops off
          }
        }
        if (on2) nextColor(1);                    // get next color after top blink
        if (mainClock - prevTime > 35) {          // blink rate tips
          on = !on;                               //
          prevTime = mainClock;                   //
        }
        if (mainClock - prevTime2 > 10) {         // blink rate tops
          on2 = !on2;                             //
          prevTime2 = mainClock;                  //
        }
        break;
      }

    case 23: { // Pulse (Warp color 1 with strobe, full hand ghost dops)
        if (mainClock - prevTime > 1) {                     // blink rate
          on = !on;                                         //
          if (on) {                                         // blink on
            getColor(currentColor);                         //
            for (int a = 0; a < NUM_LEDS; a++) setLed(a);   // set all to current color
            nextColor(1);                                   // get next color
          }
          else clearAll();                                  // blink off
          prevTime = mainClock;                             //
        }
        getColor(0);                                        // get first color
        if (mainClock - prevTime3 > 15) {                   // blink rate of pulse
          on2 = !on2;                                       //
          prevTime3 = mainClock;                            //
        }
        if (on2) setLeds(0 + (2 * frame), 1 + (2 * frame)); // blink pulse to first color
        else {                                              //
          clearLight(0 + (2 * frame));                      // blink pulse off
          clearLight(1 + (2 * frame));                      //
        }
        if (mainClock - prevTime2 > 150) {                  // pulse move speed
          frame++;                                          //
          if (frame > 5) frame = 0;                         //
          prevTime2 = mainClock;                            //
        }
        break;
      }

    case 24: { //Fill (Fill thumb to pinkie with strobes)
        if (mainClock - prevTime2 > 100) {        //
          frame++;                                // fill speed
          if (frame >= 5) {                       //
            frame = 0;                            //
            nextColor(0);                         // get next color after 5 fingers filled
          }
          prevTime2 = mainClock;
        }
        if (mainClock - prevTime > 10) {          // blink rate
          on = !on;                               //
          prevTime = mainClock;                   //
        }
        if (on) {
          getColor(currentColor);                 //
          setLeds(0, NUM_LEDS);                   // set all to current color

          getColor(next);                         // get next color
          setLeds(0, (frame * 2) - 1);            // set LEDs up to current fill
        }
        else clearAll();
        break;
      }

    case 25: { // Drip (Tops to tips with strobe)
        if (on) {                                 // blink on
          getColor(currentColor);                 //
          setLeds(0, NUM_LEDS);                   // set all to current color

          if (frame == 1) {                       // every 2nd frame
            for (int i = 0; i < NUM_LEDS; i++) {
              if (i % 2 == 0) {                   // find Tops (evens)
                getColor(next);                   // get next color
                setLed(i);                        // blink tops on
              }
            }
          }
        }
        else clearAll();                          // blink off
        if (mainClock - prevTime > 150) {         // drip speed
          frame += 1;
          if (frame > 1) {
            frame = 0;
            nextColor(0);                         // next color
          }
          prevTime = mainClock;
        }
        if (mainClock - prevTime2 > 10) {         // blink rate
          on = !on;                               //
          prevTime2 = mainClock;                  //
        }
        break;
      }

    case 26: { // Bounce (Bounce and change colors between thumb/pinkie with strobe)
        if (on) {                                                             // blink on
          getColor(currentColor);                                             // get current color
          setLeds(0, NUM_LEDS);                                               // set all leds
          getColor(next);                                                     // get next color
          setLed (int(triwave8(frame) / 25.4));                               // set bounce led
        }
        else clearAll();                                                      // blink off

        if (mainClock - prevTime > 5) {                                       // bounce and blink rate
          on = !on;                                                           //
          frame += 2;                                                         //
          if (triwave8(frame) == 0 || triwave8(frame) == 254) nextColor(0);   // next color on thumb/pinky
          prevTime = mainClock;                                               //
        }
        break;
      }

    case 27: { // Strobe
        if (on) {
          getColor(currentColor);
          setLeds(0, NUM_LEDS);
        }
        if (!on) {
          clearAll();

        }
        if (mainClock - prevTime > 15) {
          on = !on;
          if (on) nextColor(0);
          prevTime = mainClock;
        }
        break;
      }

    case 28: { // Hyperstrobe
        if (on) {
          getColor(currentColor);
          setLeds(0, NUM_LEDS);
        }
        if (!on) {
          clearAll();

        }
        if (mainClock - prevTime > 35) {
          on = !on;
          if (on) nextColor(0);
          prevTime = mainClock;
        }
        break;
      }

    case 29: { // Dops ( closer to strobie with 15ms gap)
        if (on) {
          getColor(currentColor);
          setLeds(0, NUM_LEDS);
          duration = 3;
        }
        if (!on) {
          clearAll();
          duration = 20;

        }
        if (mainClock - prevTime > duration) {
          on = !on;
          if (on) nextColor(0);
          prevTime = mainClock;
        }
        break;
      }

    case 30: { // Blinkie (hyperblink?) (bundle colors to be proper blinkie)
        if (on) {
          getColor(currentColor);
          setLeds(0, NUM_LEDS);
          duration = 3;
        }
        if (!on) {
          clearAll();
          duration = 65;

        }
        if (mainClock - prevTime > duration) {
          on = !on;
          if (on) nextColor(0);
          prevTime = mainClock;
        }
        break;
      }
    case 31: { // Strobie (delete this?)
        if (on) {
          getColor(currentColor);
          setLeds(0, NUM_LEDS);
          duration = 15;
        }
        if (!on) {
          clearAll();
          duration = 45;

        }
        if (mainClock - prevTime > duration) {
          on = !on;
          if (on) nextColor(0);
          prevTime = mainClock;
        }
        break;
      }
    case 32: { //Brackets (candie strobe?)
        clearAll();
        if (!on) duration = 50;// 20
        if (on) {
          if (frame == 0 || frame == 2) {
            getColor(currentColor);
            setLeds(0, 27);
            duration = 5; // 3
          }
          if (frame == 1) {
            getColor(next);
            setLeds(0, 27);
            duration = 12; // 9
          }
        }
        if (mainClock - prevTime > duration) {
          if (on) {
            frame++;
            if (frame > 2) {
              nextColor(0);
              frame = 0;
              on = !on;
            }
          }
          else if (!on) on = !on;
          prevTime = mainClock;
        }
        break;
      }

    default: { // All Ribbon - executed if pat == 0 or out-of-range
        if (mainClock - prevTime > 20) {
          getColor(currentColor);
          setLeds(0, 27);
          nextColor(0);
          prevTime = mainClock;
        }
      }
  }
}

// Randomize colors and pattern every so often
void runDemo() {
  int demoInterval = 0;
  if (demoSpeed == 0) demoInterval = 3000;
  if (demoSpeed == 1) demoInterval = 5000;
  if (demoSpeed == 2) demoInterval = 8000;
  if (demoSpeed == 3) demoInterval = 16000;

  if (mainClock - demoTime > demoInterval) {
    rollColors();
    demoTime = mainClock;
  }
}

//Led controlls for running patterns
//-----------------------------------------------------

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

void nextColor(int start) {
  mode[m].currentColor++;
  if (mode[m].currentColor >= mode[m].numColors) mode[m].currentColor = start;
  mode[m].nextColor = mode[m].currentColor + 1;
  if (mode[m].nextColor >= mode[m].numColors) mode[m].nextColor = start;
}

void nextColor1(int start) {
  mode[m].currentColor1++;
  if (mode[m].currentColor1 >= mode[m].numColors) mode[m].currentColor1 = start;
  mode[m].nextColor1 = mode[m].currentColor1 + 1;
  if (mode[m].nextColor1 >= mode[m].numColors) mode[m].nextColor1 = start;
}

void clearAll() {
  for (int a = 0; a < 28; a++) leds[a].setHSV(0, 0, 0);
}

void clearLight(int lightNum) {
  leds[lightNum].setHSV(0, 0, 0);
}

void blinkTarget(unsigned long blinkTime) {
  mainClock = millis();
  if (mainClock - prevTime > blinkTime) {
    on = !on;
    prevTime = mainClock;
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

//void openColors() {
//  mainClock = millis();
//  if (mainClock - prevTime > 75) {
//    clearAll();
//    for (int side = 0; side < 4; side++) {
//      if (frame >= 0 && frame <= 3) {
//        leds[3 + (7 * side) + frame].setHSV(0, 0, 170);
//        leds[3 + (7 * side) - frame].setHSV(0, 0, 170);
//      }
//      if (frame >= 4 && frame <= 6) {
//        leds[3 + (7 * side) + (6 - frame)].setHSV(0, 0, 170);
//        leds[3 + (7 * side) - (6 - frame)].setHSV(0, 0, 170);
//      }
//    }
//    frame++;
//    if (frame > 6) frame = 0;
//    prevTime = mainClock;
//  }
//}

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
      val = 255 - (85 * bright);
      if (bright == 2) val = 120;
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

//void openPatterns() {
//  mainClock = millis();
//  if (mainClock - prevTime > 100) {
//    for (int a = 0; a < 28; a++) {
//      leds[a].setHSV(0, 0, 110);
//    }
//    if (on) {
//      clearAll();
//    }
//    on = !on;
//    prevTime = mainClock;
//  }
//}

void patternSelect() {
  mainClock = millis();
  patterns(patNum);
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
  mainClock = millis();
  if (mainClock - prevTime > 50) {
    if (frame == 0) clearAll();
    if (frame == 1) sat = 0, val = 175, setLeds(0, 27);
    if (frame == 2) clearAll();
    if (frame == 3) frame = 0, mode[m].menuNum = 0;
    frame++;
    prevTime = mainClock;
  }
}

void menuRing(int buttonNum) {
  clearAll();
  int menuHue, menuSat;
  if (menuSection == 0) {
    if (buttonNum == 0) menuHue = 0, menuSat = 0;
    if (buttonNum == 1) menuHue = 60, menuSat = 255;
  }
  if (menuSection == 1) {
    if (buttonNum == 0) menuHue = 20, menuSat = 255;
    if (buttonNum == 1) menuHue = 190, menuSat = 255;
  }
  if (menuSection == 2) {
    if (buttonNum == 0) menuHue = 160, menuSat = 255;
    if (buttonNum == 1) menuHue = 0, menuSat = 255;
  }
  if (menuSection == 3) {
    if (buttonNum == 0) menuHue = 60, menuSat = 255;
  }
  if (menuSection == 4) {
    if (buttonNum == 0) menuHue = 0, menuSat = 255;
  }
  if (menuSection == 5) {
    if (buttonNum == 0) menuHue = 120, menuSat = 255;
  }
  hue = menuHue;
  sat = menuSat;
  val = 110;
  for (int finger = 0; finger < 5; finger++) {
    if (button[buttonNum].holdTime > 1000 + 1000 * menuSection) setLeds(0, 1);
    if (button[buttonNum].holdTime > 1200 + 1000 * menuSection) setLeds(2, 3);
    if (button[buttonNum].holdTime > 1400 + 1000 * menuSection) setLeds(4, 5);
    if (button[buttonNum].holdTime > 1600 + 1000 * menuSection) setLeds(6, 7);
    if (button[buttonNum].holdTime > 1800 + 1000 * menuSection) setLeds(8, 9);

  }
}

//Buttons
// button[0] is outer button
// button[1] is inner button
//---------------------------------------------------------

void checkButton() {
  for (int b = 0; b < 2; b++) {
    button[b].buttonState = digitalRead(button[b].pinNum);
    if (button[b].buttonState == LOW && button[b].lastButtonState == HIGH && (millis() - button[b].pressTime > 200)) {
      button[b].pressTime = millis();
    }
    button[b].holdTime = (millis() - button[b].pressTime);
    if (button[b].holdTime > 50) {
      //---------------------------------------Button Down-----------------------------------------------------
      if (button[b].buttonState == LOW && button[b].holdTime > button[b].prevHoldTime) {
        if (b == 0) {
          if (button[b].holdTime > 1000 && button[b].holdTime <= 2000 && menu == 0 && !demoMode) mode[m].menuNum = 1, menuSection = 0;
          if (button[b].holdTime > 2000 && button[b].holdTime <= 3000 && menuSection == 0) menuSection = 1;
          if (button[b].holdTime > 3000 && button[b].holdTime <= 4000 && menuSection == 1) menuSection = 2;
          if (button[b].holdTime > 4000 && button[b].holdTime <= 5000 && menuSection == 2) menuSection = 3;
          if (button[b].holdTime > 5000 && button[b].holdTime <= 6000 && menuSection == 3) menuSection = 4;
          if (button[b].holdTime > 6000 && button[b].holdTime <= 7000 && menuSection == 4) menuSection = 5;
          if (button[b].holdTime > 7000 && menuSection == 5) mode[m].menuNum = 5;
        }
        if (b == 1) {
          if (button[b].holdTime > 1000 && button[b].holdTime <= 2000 && menu == 0) mode[m].menuNum = 2, menuSection = 0;
          if (button[b].holdTime > 2000 && button[b].holdTime <= 3000 && menuSection == 0) menuSection = 1;
          if (button[b].holdTime > 3000 && button[b].holdTime <= 4000 && menuSection == 1) menuSection = 2;
          if (button[b].holdTime > 4000 && menuSection == 2) mode[m].menuNum = 8;
        }
      }//======================================================================================================
      // ---------------------------------------Button Up------------------------------------------------------
      if (button[b].buttonState == HIGH && button[b].lastButtonState == LOW && millis() - button[b].prevPressTime > 150) {
        if (menu == 0 && !demoMode) {
          if (button[b].holdTime <= 300) {
            if (b == 0) m++, frame = 0, gap = 0; //, throwMode();
            if (b == 1) m--, frame = 0, gap = 0;
          }
          if (button[b].holdTime > 300 && Serial) exportSettings();
        }
        // press in demo mode
        if (menu == 0 && demoMode) {
          if (button[b].holdTime <= 3000) {
            if (b == 0) saveAll(), frame = 0, mode[m].currentColor = 0;
            if (b == 1) tempLoad(), frame = 0, mode[m].currentColor = 0;
            mode[m].menuNum = 9;
            demoMode = false;
          }
        }
        if (menu == 1) {
          if (button[0].holdTime > 1000 && button[b].holdTime <= 2000) {
            if (b == 0) demoMode = true, frame = 0, mode[m].menuNum = 0, demoTime = mainClock, tempSave(), rollColors();
          }
          if (button[0].holdTime > 2000 && button[b].holdTime <= 3000) {
            if (b == 0) mode[m].menuNum = 3, targetSlot = 0;
          }
          if (button[0].holdTime > 3000 && button[b].holdTime <= 4000) {
            if (b == 0) mode[m].menuNum = 4, mode[m].currentColor = 0;
          }
          if (button[0].holdTime > 4000 && button[b].holdTime <= 5000) {
            if (b == 0) mode[m].menuNum = 6, mode[m].currentColor = 0;
          }
          if (button[0].holdTime > 5000 && button[b].holdTime <= 6000) {
            if (b == 0) mode[m].menuNum = 8, mode[m].currentColor = 0;
          }
          if (button[0].holdTime > 6000) {
            if (b == 0) mode[m].menuNum = 5;
          }
        }
        if (menu == 2) {
          if (button[b].holdTime > 1000 &&  button[b].holdTime <= 2000) {
            if (b == 1) {
              mode[m].menuNum = 6;
            }
          }
          if (button[b].holdTime > 2000 && button[b].holdTime <= 3000) {
            if (b == 1) mode[m].menuNum = 7; newDemoSpeed = demoSpeed;
          }
          if (button[b].holdTime > 3000) {
            if (b == 1) mode[m].menuNum = 8;
          }
        }
        if (menu == 3) {
          if (button[b].holdTime <= 300) {
            if (b == 0) {
              if (stage == 0) targetSlot++;
              if (stage == 1) targetZone++;
              if (stage == 2) targetHue++;
              if (stage == 3) targetSat++;
              if (stage == 4) targetVal++;
            }
            if (b == 1) {
              if (stage == 0) targetSlot--;
              if (stage == 1) targetZone--;
              if (stage == 2) targetHue--;
              if (stage == 3) targetSat--;
              if (stage == 4) targetVal--;
            }
          }
          if (button[b].holdTime > 300 && button[b].holdTime < 3000) {
            if (b == 0) {
              if (stage == 0) {
                int setSize = mode[m].numColors;
                if (targetSlot < setSize) stage = 1, currentSlot = targetSlot; //choose slot
                if (targetSlot == setSize && mode[m].numColors > 1)targetSlot--, mode[m].numColors--; // delete slot
                if (targetSlot == setSize + 1 && setSize < 8)stage = 1, currentSlot = setSize;  //add slot
                if (targetSlot == setSize + 2 || (targetSlot == setSize + 1 && setSize == 8)) mode[m].currentColor = 0, saveAll(), mode[m].menuNum = 0;
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
            if (b == 1) {
              if (stage == 0)mode[m].currentColor = 0, saveAll(), mode[m].menuNum = 0;//cancle exit
              if (stage == 1)stage = 0;
              if (stage == 2)stage = 1;
              if (stage == 3)stage = 2;
              if (stage == 4)stage = 3;
            }
          }
        }
        if (menu == 4) {
          if (button[b].holdTime <= 300) {
            if (b == 0)patNum++, frame = 0, mode[m].currentColor = 0;
            if (b == 1)patNum--, frame = 0, mode[m].currentColor = 0;
          }
          if (button[b].holdTime > 300 && button[b].holdTime < 3000) {
            mode[m].menuNum = 9;
            if (b == 0) mode[m].patternNum = patNum, saveAll(), frame = 0;//confirm selection
            if (b == 1) if (menu == 4) frame = 0;//cancle exit
          }
        }
        if (menu == 5) {
          if (button[b].holdTime <= 500) sharing = !sharing;
          if (button[b].holdTime > 500 && button[b].holdTime < 3000) sharing = true, mode[m].menuNum = 9;
        }
        if (menu == 6) {
          if (button[b].holdTime <= 300) {
            if (b == 0) brightVal++;
            if (b == 1) brightVal--;
          }
          if (button[b].holdTime > 300 && button[b].holdTime < 3000) {
            mode[m].menuNum = 9;
            if (b == 0) {
              if (brightVal == 0) brightness = 255;
              if (brightVal == 1) brightness = 200;
              if (brightVal == 2) brightness = 150;
              if (brightVal == 3) brightness = 100;
              saveAll();
            }
            if (b == 1) brightness = prevBrightness;
          }
        }
        if (menu == 7) {
          if (button[b].holdTime <= 300) {
            if (b == 0) newDemoSpeed++;
            if (b == 1) newDemoSpeed--;
          }
          if (button[b].holdTime > 300 && button[b].holdTime < 3000) {
            if (b == 0) demoSpeed = newDemoSpeed; demoTime = mainClock; saveAll();
            mode[m].menuNum = 9;
          }
        }
        if (menu == 8) {
          if (button[b].holdTime <= 300)restore = !restore;
          if (button[b].holdTime > 300 && button[b].holdTime < 3000) {
            mode[m].menuNum = 9;
            if (b == 0) {
              if (restore) {
                setDefaults();
                saveAll();
                frame = 0;
                mode[m].currentColor = 0;
              }
            }
          }
        }
        //if (button[b].holdTime < 4000 && menu == 9)mode[m].menuNum = 7;
        button[b].prevPressTime = millis();
      }//======================================================================================================
    }

    //these are the max and minimum values for each variable.
    if (newDemoSpeed > 3) newDemoSpeed = 0;
    if (newDemoSpeed < 0) newDemoSpeed = 3;
    if (brightVal > 3) brightVal = 0;
    if (brightVal < 0) brightVal = 3;
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
    if (m < 0)m = totalModes - 1;
    if (m > totalModes - 1)m = 0;
    button[b].lastButtonState = button[b].buttonState;
    button[b].prevHoldTime = button[b].holdTime;
  }
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
  if (on) {
    for (int d = 0; d < 28; d++) leds[d].setHSV(128, 255, 100);
  }
  if (!on) {
    for (int d = 0; d < 28; d++) leds[d].setHSV(0, 0, 0);
  }
  mainClock = millis();
  if (mainClock - prevTime > 20) {
    on = !on;
    prevTime = mainClock;
  }
  if (mainClock - prevTime2 > 500) {
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
    prevTime2 = mainClock;
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
      dataIsValid = false;
    }
  }
}

// Comma separated list of 27 numbers (3 settings + 3 * 8 colors):
// Mode Num, Pattern Num, Num Colors, Color1 H, Color1 S, Color1 V, Color2 H..... Color8 V
void importMode(char input[]) {
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

//Default Modes
//---------------------------------------------------------

void setDefaults() {
  brightness = 255;
  demoSpeed = 2;
  importMode("0, 26, 8, 0, 255, 170, 32, 255, 170, 64, 255, 170, 96, 255, 170, 128, 255, 170, 160, 255, 170, 192, 255, 170, 224, 255, 170");
  importMode("1, 6, 3, 0, 255, 170, 160, 255, 170, 224, 255, 120, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0");
  importMode("2, 2, 5, 0, 0, 0, 0, 0, 120, 64, 255, 120, 160, 255, 120, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0");
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
