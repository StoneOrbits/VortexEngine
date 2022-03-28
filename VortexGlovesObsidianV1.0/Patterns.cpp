#include "Arduino.h"
#include "Modes.h"
#include "Patterns.h"


Patterns::Patterns() {
}

void Patterns::refresh(Modes thisMode) {
  mode = thisMode;
  frame = 0;
  gap = 0;
  rep = 0;
  clearAll();
}

CRGB Patterns::getLed(int i) {
  return leds[i];
}

void Patterns::getColor(int target) {
  hue = mode.hue[target];
  sat = mode.sat[target];
  val = mode.val[target];
}

void Patterns::setLed(int target) {
  leds[target].setHSV(hue, sat, val);
}

void Patterns::setLeds(int first, int last) {
  for (int a = first; a <= last; a++) setLed(a);
}

void Patterns::nextColor(int start) {
  mode.currentColor++;
  if (mode.currentColor >= mode.numColors) mode.currentColor = start;
  mode.nextColor = mode.currentColor + 1;
  if (mode.nextColor >= mode.numColors) mode.nextColor = start;
}

void Patterns::nextColor1(int start) {
  mode.currentColor1++;
  if (mode.currentColor1 >= mode.numColors) mode.currentColor1 = start;
  mode.nextColor1 = mode.currentColor1 + 1;
  if (mode.nextColor1 >= mode.numColors) mode.nextColor1 = start;
}

void Patterns::clearAll() {
  for (int a = 0; a < 28; a++) leds[a].setHSV(0, 0, 0);
}

void Patterns::clearLight(int lightNum) {
  leds[lightNum].setHSV(0, 0, 0);
}

void Patterns::clearLights(int first, int last) {
  for (int a = first; a <= last; a++) clearLight(a);
}

void Patterns::adjustValues(unsigned long v1, unsigned long v2, unsigned long v3, unsigned long v4) {
  time1 = v1;
  time2 = v2;
  time3 = v3;
  time4 = v4;
}

void Patterns::basicPattern(unsigned long onDuration, unsigned long offDuration, unsigned long gapDuration) {
  if (onDuration == 2 && offDuration >= 5 & offDuration <= 31) onDuration = 3;
  static bool lightsOn = true;
  static unsigned long previousClockTime;
  static unsigned long timerDuration;
  if (lightsOn) {
    getColor(mode.currentColor);
    setLeds(0, NUM_LEDS);
    timerDuration = onDuration;
  }
  if (!lightsOn) {
    clearAll();
    if (gapDuration == 0) timerDuration = offDuration;
    if (gapDuration != 0) {
      if (mode.currentColor == 0) timerDuration = gapDuration;
      if (mode.currentColor != 0) {
        if (offDuration == 0) lightsOn = true;
        if (offDuration != 0) timerDuration = offDuration;
      }
    }
  }
  if (mainClock - previousClockTime > timerDuration) {
    if (lightsOn) nextColor(0);
    if (offDuration != 0) lightsOn = !lightsOn;
    if (offDuration == 0) {
      if (gapDuration != 0 && mode.currentColor == 0) lightsOn = !lightsOn;
      if (gapDuration == 0) lightsOn = true;
    }
    previousClockTime = mainClock;
  }
}

void Patterns::dashDops(unsigned long dotDuration, unsigned long offDuration, unsigned long dashDuration) {

  static bool lightsOn;
  static unsigned long previousClockTime, timerDuration;
  if (lightsOn) {
    getColor(mode.currentColor);
    setLeds(0, 27);
    if (mode.currentColor == 0) timerDuration = dashDuration;
    else timerDuration = dotDuration;
  }
  if (!lightsOn) {
    clearAll();
    timerDuration = offDuration;
  }

  if (mainClock - previousClockTime > timerDuration) {
    if (lightsOn)nextColor(0);
    if (offDuration != 0) lightsOn = !lightsOn;
    if (offDuration == 0 && !lightsOn) lightsOn = true;
    previousClockTime = mainClock;
  }
}

void Patterns::tracer(unsigned long dashDuration, unsigned long dotDuration) {
  static bool lightsOn;
  static unsigned long previousClockTime, timerDuration;

  getColor(0);                        // Get color 0
  setLeds(0, 9);                      // Set all LEDs
  if (lightsOn) {                     // when blink is on
    getColor(mode.currentColor);      // get the current color in the set
    if (mode.numColors == 1) val = 0; // (special case when set has 1 color)
    setLeds(0, 9);                    // set all LEDs
    timerDuration = dotDuration;      // set Timer to 2ms
  }
  if (!lightsOn) timerDuration = dashDuration;          // when blink is on set Timer to 10ms
  if (mainClock - previousClockTime > timerDuration) {  // check if timer duration has passed
    if (!lightsOn)nextColor(1);                         // when blink is not on, que next color (excluding color 0)
    lightsOn = !lightsOn;                               // flip blink on/off
    previousClockTime = mainClock;                      // refresh timer counter
  }
}

void Patterns::blendPattern(unsigned long onDuration, unsigned long offDuration) {
  if (onDuration == 2 && offDuration >= 5 & offDuration <= 31) onDuration = 3;

  static bool lightsOn = true ;
  static unsigned long previousClockTime, timerDuration;
  static int colorGap = 0;

  if (lightsOn) {
    getColor(mode.currentColor);
    int color1 = mode.hue[mode.currentColor];                                    // next colors and set
    int color2 = mode.hue[mode.nextColor];                                            // all leds
    if (mode.numColors < 2) color2 = mode.hue[mode.currentColor];
    if (color1 > color2 && color1 - color2 < (255 - color1) + color2)colorGap--;    //
    if (color1 > color2 && color1 - color2 > (255 - color1) + color2)colorGap++;    //
    if (color1 < color2 && color2 - color1 < (255 - color2) + color1)colorGap++;    //
    if (color1 < color2 && color2 - color1 > (255 - color2) + color1)colorGap--;    //
    if (color1 + gap >= 255) colorGap -= 255;                                       //
    if (color1 + gap < 0) gap += 255;                                               //
    int finalHue = color1 + colorGap;                                               //
    if (finalHue == color2) colorGap = 0, nextColor(0);                             //
    for (int a = 0; a < 10; a++) leds[a].setHSV(finalHue, sat, val);                //
    timerDuration = onDuration;
  }
  if (!lightsOn) {
    timerDuration = offDuration;
    clearAll();
  }

  if (mainClock - previousClockTime > timerDuration) {
    if (offDuration == 0) lightsOn = true;
    else if (offDuration != 0) lightsOn = !lightsOn;
    previousClockTime = mainClock;
  }
}

void Patterns::brackets(unsigned long onDuration, unsigned long edgeDuration, unsigned long offDuration) {
  static bool lightsOn = true ;
  static unsigned long previousClockTime, timerDuration;
  static int progress = 0;

  clearAll();
  if (!lightsOn) timerDuration = offDuration;// 20
  if (lightsOn) {
    if (progress == 0 || progress == 2) {
      getColor(mode.currentColor);
      setLeds(0, 27);
      timerDuration = edgeDuration; // 3
    }
    if (progress == 1) {
      if (mode.numColors > 1)getColor(mode.nextColor);
      else val = 0;
      setLeds(0, 27);
      timerDuration = onDuration; // 9
    }
  }
  if (mainClock - previousClockTime > timerDuration) {
    if (lightsOn) {
      progress++;
      if (progress > 2) {
        nextColor(0);
        progress = 0;
        if (offDuration != 0) {
          lightsOn = !lightsOn;
        }
      }
    }
    else if (!lightsOn) lightsOn = !lightsOn;
    previousClockTime = mainClock;
  }
}

void Patterns::theaterChase(unsigned long onDuration, unsigned long offDuration, unsigned long flipDuration, unsigned long fingerDuration) {
  if (fingerDuration <= 50) fingerDuration = 50;

  static bool lightsOn = true;
  static bool lightsOn2 = true;
  static bool lightsOn3 = true;
  static unsigned long previousClockTime, timerDuration;
  static unsigned long previousClockTime2;
  static unsigned long previousClockTime3;

  getColor(mode.currentColor);      // get the current color in the set
  clearAll();                       // clear Leds
  if (lightsOn) {                   // when blink is on
    if (lightsOn2) {                // and when blink2 is on
      if (lightsOn3) {
        setLed(0);                  // set Led 0, 4, and 8
        setLed(4);
        setLed(8);
        timerDuration = onDuration;
      }
      if (!lightsOn3) timerDuration = offDuration;
    }
    if (!lightsOn2) {                     // and when blink2 is off
      if (lightsOn3) {
        setLed(1);                        // set Led 1, 5, 9
        setLed(5);
        setLed(9);
        timerDuration = onDuration;
      }
      if (!lightsOn3) timerDuration = offDuration;
    }
  }
  if (!lightsOn) {                        // when blink is off
    if (lightsOn2) {                      // and when blink2 is on
      if (lightsOn3) {
        setLed(2);                        // set Led 2, 6
        setLed(6);
        timerDuration = onDuration;
      }
      if (!lightsOn3) timerDuration = offDuration;
    }
    if (!lightsOn2) {                     // and when blink2 is off
      if (lightsOn3) {
        setLed(3);                        // set Led 3,7
        setLed(7);
        timerDuration = onDuration;
      }
      if (!lightsOn3) timerDuration = offDuration;
    }

  }
  if (mainClock - previousClockTime > fingerDuration) {  // check if Timer2 has passed 125ms
    lightsOn = !lightsOn;                                // flip blink on/off
    previousClockTime = mainClock;                       // refresh Timer2
  }
  if (mainClock - previousClockTime2 > flipDuration) {   // check if Timer has passed 2ms
    lightsOn2 = !lightsOn2;                              // flip blink2 on/off
    nextColor(0);                                        // que next color
    previousClockTime2 = mainClock;                      // refresh timer
  }
  if (mainClock - previousClockTime3 > timerDuration) {
    lightsOn3 = !lightsOn3;
    previousClockTime3 = mainClock;
  }
}

void Patterns::zigZag(unsigned long onDuration, unsigned long offDuration, unsigned long zipDuration) {
  if (zipDuration <= 8) zipDuration = 8;

  static bool lightsOn = true;
  static unsigned long previousClockTime, timerDuration;
  static unsigned long previousClockTime2;
  static int progress = 0;

  clearAll();                              // clear all
  if (lightsOn) {
    getColor(mode.currentColor);           // get current color
    setLed(progress * 2);                  // set next top Led
    getColor(mode.nextColor);              // get next color
    setLed(9 - progress * 2);              // set next tip Led
    timerDuration = onDuration;
  }
  if (!lightsOn) timerDuration = offDuration;
  if (mainClock - previousClockTime > zipDuration) {    // check if timer has passed 50ms
    progress ++;                                        // next repetition
    if (progress > 4) {                                 // if repetition is more than 4
      progress = 0;                                     // reset repetition count
      nextColor(0);                                     // que next color
    }
    previousClockTime = mainClock;                      // refresh timer
  }
  if (mainClock - previousClockTime2 > timerDuration) {    // check if timer2 has passed 4ms
    lightsOn = !lightsOn;                                  // flip blink on/off
    if (offDuration == 0) lightsOn = true;
    previousClockTime2 = mainClock;                        // refresh timer2
  }
}

void Patterns::zipFade(unsigned long onDuration, unsigned long offDuration, unsigned long zipDuration, unsigned long fadeDuration) {
  if (zipDuration <= 15) zipDuration = 15;

  static bool lightsOn = true;
  static unsigned long previousClockTime, timerDuration;
  static unsigned long previousClockTime2;
  static unsigned long previousClockTime3;
  static int progress = 0;

  if (lightsOn) {
    getColor(mode.nextColor);                                       //
    setLed(progress * 2);                                           // Top zip
    getColor(mode.currentColor);                                    //
    setLed(9 - progress * 2);                                       // Tip zip
    if (mainClock - previousClockTime2 > fadeDuration) {            // fade timer
      for (int a = 0; a < NUM_LEDS; a++)leds[a].fadeToBlackBy(20);  //
      previousClockTime2 = mainClock;                               //
    }
    timerDuration = onDuration;
  }
  if (!lightsOn) timerDuration = offDuration;

  if (mainClock - previousClockTime > timerDuration) {    // strobe timer
    lightsOn = !lightsOn;                                 //
    if (lightsOn) {                                       //
      for (int a = 0; a < NUM_LEDS; a++) {                // special save/load
        leds[a] = copy[a];                                // for fade effect
      }                                                   // with dops
    }                                                     //
    if (!lightsOn) {                                      //
      for (int a = 0; a < NUM_LEDS; a++) {                //
        copy[a] = leds[a];                                //
      }                                                   //
      clearAll();                                         //
    }                                                     //
    previousClockTime = mainClock;                        //
  }
  if (mainClock - previousClockTime3 > zipDuration) {   // Zip timer
    progress ++;                                        //
    if (progress > 4) {                                 //
      progress = 0;                                     //
    }
    if (progress == 2) nextColor(0);                    // next color each zip step
    previousClockTime3 = mainClock;                     //
  }
}

void Patterns::tipTop(unsigned long onDuration, unsigned long offDuration, unsigned long onDuration2, unsigned long offDuration2) {

  static bool lightsOn = true;
  static bool lightsOn2 = true;
  static unsigned long previousClockTime, timerDuration;
  static unsigned long previousClockTime2, timerDuration2;
  static int progress = 0;

  clearAll();
  for (int i = 0; i < NUM_LEDS; i++) {      //
    if (i % 2 == 1) {                       // Tips (odds)
      if (lightsOn) {                             //
        getColor(0);                        // get first color
        setLed(i);                          // blink tips on
        timerDuration2 = onDuration2;
      }
      else {
        clearLight(i);                   // blink tips off
        timerDuration2 = offDuration2;
      }
    }
    if (i % 2 == 0) {                       // Tops (evens)
      if (lightsOn2) {                            //
        getColor(mode.currentColor);             // get current color
        setLed(i);                          // blink tops on
      }
      else clearLight(i);                   // blink tops off
    }
  }
  if (lightsOn2) {
    if (mode.numColors > 1) nextColor(1);                    // get next color after top blink
    timerDuration = onDuration;
  }
  if (!lightsOn2) timerDuration = offDuration;
  if (mainClock - prevTime > timerDuration2) {          // blink rate tips
    lightsOn = !lightsOn;                               //
    prevTime = mainClock;                   //
  }
  if (mainClock - prevTime2 > timerDuration) {         // blink rate tops
    lightsOn2 = !lightsOn2;                             //
    prevTime2 = mainClock;                  //
  }
}

//Patterns
//---------------------------------------------------------
// To add new patterns add a switch to this method and update totalPatterns.
// In your pattern code, getColor() will set the hue, sat, val variables for this iteration.
// Then call setLeds() to set those values on to the leds you want.
// this code will repeat forever, incrementing "mainClock" with each iteration.

void Patterns::playPattern() {
  mainClock = millis();
  int totalColors = mode.numColors;
  int currentColor = mode.currentColor;

  int currentColor1 = mode.currentColor1;
  int next = mode.nextColor;
  int pat = mode.patternNum;
  switch (pat) {
    case 1: { // Hyperstrobe
        //format basicPattern(ontime, offtime, gaptime)
        basicPattern(25, 25);
        break;
      }

    case 2: { // Dops
        basicPattern(2, 13);
        break;
      }

    case 3: { //Dopish
        basicPattern(2, 7);
        break;
      }

    case 4: { // UltraDops (dops but a lot)
        basicPattern(1, 3);
        break;
      }

    case 5: { // Strobie
        basicPattern(3, 22);
        break;
      }

    case 6: { // Blinkie
        basicPattern(5, 8, 35);
        break;
      }

    case 7: { // Ghost Crush (breifly flash all colors in set before a pause)
        basicPattern(1, 0, 50);
        break;
      }

    case 8: { // Ribbon
        basicPattern(20);
        break;
      }

    case 9: { // Mini Ribbon (chroma but tighter)
        basicPattern(3);
        break;
      }

    case 10: { // Dash dops (first color dash, rest of the colors dops)
        //dashDops(dotDuration, offDuration, dashDuration);
        dashDops(1, 5, 20);
        break;
      }

    case 11: { // All tracer (Synchronized tracer)
        tracer(10, 2);
        break;
      }

    case 12: { // Blend (gradient between selected colors with dops blink)
        blendPattern(2, 13);
        break;
      }

    case 13: { //Brackets (candie strobe?)
        brackets(12, 5, 50);
        break;
      }

    case 14: { // Theater chase       (finger chase with tip/top dops flip)
        theaterChase(5, 8, 25, 125);
        break;
      }

    case 15: { // Zig Zag (Dot zips from thumb to pinkie Tops while dot2 zips pinkie to thumb tips with dops)
        zigZag(3, 5, 50);
        break;
      }

    case 16: { // Zip fade (Zip with a constant fade with dops)
        zipFade(3, 22, 100, 2);
        break;
      }

    case 17: { // Tip Top (Tips hyperstrobe color 1, tops strobe full set)
        //tipTop(unsigned long onDuration, unsigned long offDuration, unsgined long onDuration2, unsigned long offDuration2);
        //tipTop(5,8,25,25);
        tipTop(time1,time2,time3,time4);
//        clearAll();
//        for (int i = 0; i < NUM_LEDS; i++) {      //
//          if (i % 2 == 1) {                       // Tips (odds)
//            if (on) {                             //
//              getColor(0);                        // get first color
//              setLed(i);                          // blink tips on
//            }
//            else clearLight(i);                   // blink tips off
//          }
//          if (i % 2 == 0) {                       // Tops (evens)
//            if (on2) {                            //
//              getColor(currentColor);             // get current color
//              setLed(i);                          // blink tops on
//            }
//            else clearLight(i);                   // blink tops off
//          }
//        }
//        if (on2) {
//          if (totalColors > 1) nextColor(1);                    // get next color after top blink
//          duration = 5;
//        }
//        if (!on2)duration = 8;
//        if (mainClock - prevTime > 25) {          // blink rate tips
//          on = !on;                               //
//          prevTime = mainClock;                   //
//        }
//        if (mainClock - prevTime2 > duration) {         // blink rate tops
//          on2 = !on2;                             //
//          prevTime2 = mainClock;                  //
//        }
        break;
      }

    case 18: { // Drip (Tops to tips with strobe)
        if (on) {                                 // blink on
          getColor(currentColor);                 //
          setLeds(0, NUM_LEDS);                   // set all to current color

          if (frame == 1) {                       // every 2nd frame
            for (int i = 0; i < NUM_LEDS; i++) {
              if (i % 2 == 0) {                   // find Tops (evens)
                if (totalColors > 1) getColor(next);                   // get next color
                else val = 0;
                setLed(i);                        // blink tops on
              }
            }
          }
        }
        else clearAll();                          // blink off
        if (mainClock - prevTime2 > 10) {         // blink rate
          on = !on;                               //
          prevTime2 = mainClock;                  //
        }
        if (mainClock - prevTime > 250) {         // drip speed
          frame += 1;
          if (frame > 1) {
            frame = 0;
            nextColor(0);                         // next color
          }
          prevTime = mainClock;
        }

        break;
      }

    case 19: { // Drip Morph
        if (on) {
          getColor(currentColor);                                                    // between current and
          for (int a = 0; a < 10; a++) {
            if (a % 2 == 1) leds[a].setHSV(prevHue, sat, val);
          }
          int color1 = mode.hue[currentColor];                                    // next colors and set
          int color2 = mode.hue[next];                                            // all leds
          if (totalColors < 2) color2 = mode.hue[currentColor];
          if (color1 > color2 && color1 - color2 < (255 - color1) + color2)gap--;    //
          if (color1 > color2 && color1 - color2 > (255 - color1) + color2)gap++;    //
          if (color1 < color2 && color2 - color1 < (255 - color2) + color1)gap++;    //
          if (color1 < color2 && color2 - color1 > (255 - color2) + color1)gap--;    //
          if (color1 + gap >= 255) gap -= 255;                                       //
          if (color1 + gap < 0) gap += 255;                                          //
          int finalHue = color1 + gap;                                               //
          if (finalHue == color2) {
            gap = 0, nextColor(0);                             //
            prevHue = finalHue;
          }
          for (int a = 0; a < 10; a++) {
            if (a % 2 == 0) leds[a].setHSV(finalHue, sat, val);            //
          }
        }
        else clearAll();
        if (mainClock - prevTime2 > 10) {         // blink rate
          on = !on;                               //
          prevTime2 = mainClock;                  //
        }
        break;
      }

    case 20: { // Cross dops   (dops on alternating tips and tops)
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
        if (on2) duration = 2;
        if (!on2) duration = 13;
        if (mainClock - prevTime2 > duration) {  // check if timer2 has passed 2ms
          on2 = !on2;                     // flip blink2 on/off
          prevTime2 = mainClock;          // refresh timer2
        }
        break;
      }

    case 21: { //Double Strobe (flash next 2 colors simultaneously)
        clearAll();
        if (on) {                               // Strobe 1
          for (int i = 0; i < NUM_LEDS; i++) {  //
            if (i % 2 == 0) {                   //
              getColor(currentColor);           //
              setLed(i);                        //
            }
            if (i % 2 == 1) {                   // Strobe 2
              if (totalColors > 1) getColor(next);                   //
              setLed(i);                        //
            }
            duration = 5;
          }
        }
        if (!on) duration = 8;
        if (mainClock - prevTime > 75) {       // Color timer
          nextColor(0);                         //
          prevTime = mainClock;                 //
        }

        if (mainClock - prevTime2 > duration) {       // Strobe timer
          on = !on;                             //
          prevTime2 = mainClock;                //
        }

        break;
      }

    case 22: { //Sloth Strobe (flash next 2 colors simultaneously)
        clearAll();
        if (on) {                               // Strobe 1
          for (int i = 0; i < NUM_LEDS; i++) {  //
            if (i % 2 == 0) {                   //
              getColor(currentColor);           //
              setLed(i);                        //
            }
            if (i % 2 == 1) {                   // Strobe 2
              if (totalColors > 1) getColor(next);                   //
              setLed(i);                        //
            }
            duration = 5;
          }
        }
        if (!on) duration = 8;
        if (mainClock - prevTime > 1000) {       // Color timer
          nextColor(0);                         //
          prevTime = mainClock;                 //
        }

        if (mainClock - prevTime2 > duration) {       // Strobe timer
          on = !on;                             //
          prevTime2 = mainClock;                //
        }

        break;
      }

    case 23: { // Meteor fingers (randomly flash fingers while constantly dimming)
        for (int a = 0; a < NUM_LEDS; a++)leds[a].fadeToBlackBy(75);
        if (mainClock - prevTime > 3) {
          getColor(currentColor);
          int t = random(0, 5);
          setLed(t * 2);
          setLed(t * 2 + 1);
          nextColor (0);
          prevTime = mainClock;
        }

        break;
      }

    case 24: { // SparkleTrace (Randomized tracer)
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
        if (!on) if (totalColors > 1) nextColor (1);             // when blink is not on, que next color (excluding color 0)
        if (mainClock - prevTime > 3) {
          on = !on;                           // flip blink on/off
          prevTime = mainClock;
        }
        break;
      }

    case 25: { // Vortex Wipe   (Color wipe across tops then tips with dops)
        getColor(currentColor);       //
        if (on) {                     //
          duration = 2;
          setLeds(0, 9);              //
          if (totalColors > 1) getColor(next);             //
          else val = 0;
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
        if (!on) {
          clearAll();
          duration = 7;
        }
        if (mainClock - prevTime > 125) {  // Wipe timer
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

    case 26: { // Vortex UltraWipe   (Color wipe across tops then tips with dops)
        getColor(currentColor);       //
        if (on) {                     //
          duration = 2;
          setLeds(0, 9);              //
          if (totalColors > 1) getColor(next);             //
          else val = 0;
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
        if (!on) {
          clearAll();
          duration = 7;
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

    case 27: { // Warp (Dot travels from thumb to pinkie then next color with dops)
        clearAll();                       //
        if (on) {                         //
          getColor(currentColor);         //
          setLeds(0, 9);                  //
          if (totalColors > 1) getColor(next);                 //
          else val = 0;
          setLed(dot);                    //
          duration = 2;
        }
        if (!on) duration = 7;

        if (mainClock - prevTime > 100) {  //
          dot++;                          //
          if (dot >= 10) {                //
            dot = 0;                      //
            nextColor(0);                 //
          }
          prevTime = mainClock;           //
        }
        if (mainClock - prevTime2 > duration) {  //
          on = !on;                       //
          prevTime2 = mainClock;          //
        }

        break;
      }


    case 28: { // Warp Worm (Group of color 1 lights the travel from thumb to pinky with dops)
        clearAll();                         //
        if (on) {                           //
          getColor(0);                      //
          setLeds(0, 9);                    //
          getColor(currentColor);           //
          if (currentColor == 0) getColor(1);
          if (totalColors == 1) val = 0;
          for (int i = 0; i < 6; i++) {     //
            int chunk = i + k;              //
            if (chunk > 9) chunk -= 10;     //
            setLed(chunk);                  //
          }
          duration = 2;
        }
        if (!on)duration = 7;
        if (mainClock - prevTime > 100) {    //
          k++;                              //
          prevTime = mainClock;             //
          if (k > 9) {
            k = 0;
            if (totalColors > 1) nextColor(1);   //
          }
        }
        if (mainClock - prevTime2 > duration) {    //
          on = !on;                         //
          prevTime2 = mainClock;            //
        }
        break;
      }

    case 29: { // Snowball warp (Group of all colors travel from thumb to pinkie with dops)
        clearAll();                           //
        if (on) {                             //
          getColor(0);                        // set trace
          setLeds(0, 9);                      //
          getColor(currentColor);             //
          if (totalColors == 1) val = 0;
          for (int i = 0; i < 3; i++) {       // set worm
            int chunk = i + k;                //
            if (chunk > 9) chunk -= 10;       //
            setLed(chunk);                    //
          }
          duration = 2;
        }
        if (!on) duration = 7;
        if (mainClock - prevTime > 100) {              // color/worm timer
          if (currentColor == totalColors - 1) k++;   //
          prevTime = mainClock;                       //
          if (k > 9) k = 0;                           //
          if (totalColors > 1) nextColor(1);                               //
        }
        if (mainClock - prevTime2 > duration) {              // dops timer
          on = !on;                                   //
          prevTime2 = mainClock;                      //
        }

        break;
      }

    case 30: { //Lighthouse (Warp from thumb to pinkie with constant fade with dops)
        getColor(currentColor);               // get the next color
        setLed(dot);                          // set to the next finger
        if (mainClock - prevTime > 100) {      // Finger/color timer
          dot++;                              //
          if (dot % 2 == 0) nextColor(0);     //
          if (dot > 9) {                      //
            dot = 0;                          //
          }                                   //
          prevTime = mainClock;               //
        }
        if (on) {                                                          //
          if (mainClock - prevTime2 > 10) {                                // Fade timer
            for (int a = 0; a < NUM_LEDS; a++) leds[a].fadeToBlackBy(8);  //
            prevTime2 = mainClock;                                         //
          }                                                                //
          duration = 5;
        }                                                                  //
        if (!on) duration = 8;
        if (mainClock - prevTime3 > duration) {          // dops timer
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

    case 31: { // Pulsish (Warp color 1 with strobe, full hand ghost dops)
        if (on) {                                         // blink on
          getColor(currentColor);                         //
          if (totalColors == 1) val = 0;
          setLeds(0, 27);                                 // set all to current color
          duration = 2;
        }
        if (!on) {
          clearAll();                                  // blink off
          duration = 13;
        }
        getColor(0);                                        // get first color
        if (on2) {
          setLeds(0 + (2 * frame), 1 + (2 * frame)); // blink pulse to first color
          duration2 = 1;
        }
        else {                                              //
          duration2 = 3;
          clearLight(0 + (2 * frame));                      // blink pulse off
          clearLight(1 + (2 * frame));                      //
        }
        if (mainClock - prevTime > duration) {              // blink rate
          on = !on;                                         //
          if (on) if (totalColors > 1) nextColor(1);                             // get next color
          prevTime = mainClock;                             //
        }
        if (mainClock - prevTime3 > duration2) {                   // blink rate of pulse
          on2 = !on2;                                       //
          prevTime3 = mainClock;                            //
        }
        if (mainClock - prevTime2 > 250) {                  // pulse move speed
          frame++;                                          //
          if (frame > 4) frame = 0;                         //
          prevTime2 = mainClock;                            //
        }
        break;

      }

    case 32: { //Fill (Fill thumb to pinkie with strobes)
        if (on) {
          getColor(currentColor);                 //
          setLeds(0, NUM_LEDS);                   // set all to current color

          if (totalColors > 1) getColor(next);                         // get next color
          else val = 0;
          setLeds(0, (frame * 2) - 1);            // set LEDs up to current fill
          duration = 5;

        }
        else {
          clearAll();
          duration = 24;
        }
        if (mainClock - prevTime > duration) {          // blink rate
          on = !on;                               //
          prevTime = mainClock;                   //
        }
        if (mainClock - prevTime2 > 200) {        //
          frame++;                                // fill speed
          if (frame >= 5) {                       //
            frame = 0;                            //
            nextColor(0);                         // get next color after 5 fingers filled
          }
          prevTime2 = mainClock;
        }

        break;
      }

    case 33: { // Bounce (Bounce and change colors between thumb/pinkie with strobe)
        if (on) {                                                             // blink on
          getColor(currentColor);                                             // get current color
          setLeds(0, NUM_LEDS);                                               // set all leds
          getColor(next);                                                     // get next color
          if (totalColors == 1) val = 0;
          setLed (int(triwave8(frame) / 25.4));                               // set bounce led
          duration = 5;
        }
        else {
          clearAll();                                                      // blink off
          duration = 8;
        }

        if (mainClock - prevTime > duration) {                                // bounce and blink rate
          on = !on;                                                           //
          prevTime = mainClock;                                               //
        }
        if (mainClock - prevTime2 > 10) {
          frame += 2;
          if (triwave8(frame) == 0 || triwave8(frame) == 254) nextColor(0);   // next color on thumb/pinky
          prevTime2 = mainClock;
        }
        break;
      }

    case 34: { // Impact (independend thumbs tops and tips)
        clearAll();
        getColor(0);                            // Set thumb to blink first color infrequently
        if (on2) duration2 = 25, setLeds(0, 1);  //
        if (!on2) duration2 = 250;               //

        if (on4) {
          getColor(1);
          if (totalColors == 1) val = 0;
          setLeds(4, 5);
          duration4 = 1;
        }
        if (!on4) duration4 = 8;
        getColor(2);                                                                 // Set tops to color 2
        if (totalColors <= 2) val = 0;                                                  //
        if (on3) {
          for (int fingers = 0; fingers < 4; fingers++) {
            if (fingers != 1) setLed(2 + fingers * 2);                                  //
          }
          duration = 3;
        }
        if (!on3) duration = 22;

        if (on) {
          getColor(currentColor);                                                 // Set tips to next color starting from 3rd color
          if (totalColors <= 3) val = 0;                                          //
          for (int fingers = 0; fingers < 4; fingers++) {
            if (fingers != 1) setLed(3 + fingers * 2);                            //
          }
          duration3 = 5;
        }
        if (!on) duration3 = 8;
        if (mainClock - prevTime > duration3) {
          on = !on, prevTime = mainClock;       // timer for tips/tops
          if (on) nextColor(3);                                                     //
        }
        if (mainClock - prevTime2 > duration2) on2 = !on2, prevTime2 = mainClock;   // timer for thumb
        if (mainClock - prevTime3 > duration) on3 = !on3, prevTime3 = mainClock;
        if (mainClock - prevTime4 > duration4) on4 = !on4, prevTime4 = mainClock;

        break;
      }

    case 35: { // Rabbit
        getColor(0);
        if (on) {
          for (int i = 0; i < NUM_LEDS; i++) {
            if (i % 2 == 0) setLed(i);
          }
          duration = 3;
        }
        if (!on) {
          clearAll();
          duration = 22;
        }
        getColor(currentColor);
        if (on2) {
          for (int i = 0; i < NUM_LEDS; i++) {
            if (i % 2 == 1) setLed(i);
          }
          duration2 = 5;
        }
        if (!on2) {
          for (int i = 0; i < NUM_LEDS; i++) {
            if (i % 2 == 1) clearLight(i);
          }
          duration2 = 8;
        }
        if (mainClock - prevTime > duration) {
          on = !on;
          prevTime = mainClock;
        }
        if (mainClock - prevTime2 > duration2) {
          on2 = !on2;
          if (on2) if (totalColors > 1)nextColor(1);
          prevTime2 = mainClock;
        }

        break;
      }

    case 36: { ///Split Strobie (dops/tracer tips tops swap)
        clearAll();
        if (on) {                                             // dops on
          getColor(mode.currentColor);                     // get dops color
          for (int finger = 0; finger < 5; finger++) {        //
            if (on3) setLed(2 * finger);                 // set dops
            if (!on3) setLed(2 * finger + 1);             //
          }
          duration = 5;
        }
        if (!on) {                                            // dops off
          for (int finger = 0; finger < 5; finger++) {        //
            if (on3) clearLight(2 * finger);             //
            if (!on3) clearLight(2 * finger + 1);         //
          }
          duration = 8;
        }
        getColor(mode.currentColor1);                       //
        if (totalColors < 5) val = 0;
        if (on2) {
          for (int finger = 0; finger < 5; finger++) {          //
            if (on3) setLed(2 * finger + 1);               // set trace on
            if (!on3) setLed(2 * finger);
          }
          duration2 = 3;
        }
        if (!on2) duration2 = 22;
        if (mainClock - prevTime > duration) {                  // dops rate
          on = !on;
          if (on) {
            nextColor(0);                                 // next color
            if (mode.currentColor > 3) mode.currentColor = 0;
          }
          prevTime = mainClock;                                 //
        }
        if (mainClock - prevTime2 > duration2) {              //
          on2 = !on2;                                         //
          if (on2) {
            mode.currentColor1 ++;
            if (mode.currentColor1 > mode.numColors) mode.currentColor1 = 4;

          }
          prevTime2 = mainClock;                              //
        }
        if (mainClock - prevTime3 > 1000) {                   //switch timing
          on3 = !on3;
          prevTime3 = mainClock;
        }

        break;
      }

    case 37: { // Backstrobe (Hyperstrobe/dops tips tops swap)
        if (on) {                                                                   // hyperstrobe on
          getColor(mode.currentColor);                                           // get color
          for (int finger = 0; finger < 5; finger++) setLed(2 * finger + rep);      // set half leds
        }
        if (!on) {                                                                  // hyperstrobe off
          for (int finger = 0; finger < 5; finger++) clearLight(2 * finger + rep);  // set half leds
        }
        getColor(mode.currentColor1);                                            // get dop color
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
        if (mainClock - prevTime > 50) {                                            // hyperstrobe rate
          on = !on;                                                                 //
          if (!on) nextColor(0);                                                    // next hyperstrobe color
          prevTime = mainClock;                                                     //
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

    case 38: { // Flowers/clusters
        if (on) {
          getColor(currentColor1);
          setLeds(0, 1);
          setLeds(8, 9);
          duration = 5;
        }
        if (!on) {
          clearLights(0, 1);
          clearLights(8, 9);
          duration = 25;

        }
        if (mainClock - prevTime > duration) {
          on = !on;
          if (!on) {
            if (totalColors > 1) {
              mode.currentColor1 ++;
              if (mode.currentColor1 >= 2) mode.currentColor1 = 0;
            }
          }
          prevTime = mainClock;
        }

        if (on2) {
          getColor(currentColor);
          setLeds(2, 7);
          duration2 = 3;
        }
        if (!on2) {
          clearLights(2, 7);
          duration2 = 5;

        }
        if (mainClock - prevTime2 > duration2) {
          on2 = !on2;
          if (on) if (totalColors > 2) nextColor(2);
          prevTime2 = mainClock;
        }
        break;
      }

    case 39: { // Jest mode
        static int count, count2;
        clearAll();
        if (on) {
          getColor(currentColor);
          for (int a = 0; a < NUM_LEDS; a++) {
            if (a % 2 == 1) setLed(a);
          }
          duration = 0;
        }
        if (!on) {
          clearAll();
          duration = 1;
          if (count == 0) duration = 5;
        }
        if (on2) {
          getColor(currentColor1);
          for (int a = 0; a < NUM_LEDS; a++) {
            if (a % 2 == 0) setLed(a);
          }
          duration2 = 0;
        }
        if (!on2) {
          for (int a = 0; a < NUM_LEDS; a++) {
            if (a % 2 == 0) clearLight(a);
          }
          duration2 = 1;
          if (count2 == 0) duration2 = 69;
        }
        if (mainClock - prevTime > duration) {
          on = !on;
          if (!on) {
            nextColor(0);
            count++;
            if (count >= 3) count = 0;
          }
          prevTime = mainClock;
        }
        if (mainClock - prevTime2 > duration2) {
          on2 = !on2;
          if (!on2) {
            nextColor1(0);
            count2++;
            if (count2 >= 3) count2 = 0;
          }
          prevTime2 = mainClock;
        }

        break;
      }

    default: { // Strobe - executed if pat == 0 or out-of-range
        basicPattern(5, 8);
      }
  }
}
