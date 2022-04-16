void basicPattern(span onDuration, span offDuration, span gapDuration) 
void dashDops(span dotDuration, span offDuration, span dashDuration) 
void tracer(span dashDuration, span dotDuration) 
void blendPattern(span onDuration, span offDuration) 
void brackets(span onDuration, span edgeDuration, span offDuration) 
void theaterChase(span onDuration, span offDuration, span flipDuration, span fingerDuration) 
void zigZag(span onDuration, span offDuration, span zipDuration) 
void zipFade(span onDuration, span offDuration, span zipDuration, span fadeDuration) 
void tipTop(span onDuration, span offDuration, span onDuration2, span offDuration2) 
void drip(span onDuration, span offDuration, span dripDuration) 
void dripMorph(span onDuration, span offDuration) 
void crossDops(span onDuration, span offDuration, span flipDuration) 
void doubleStrobe(span onDuration, span offDuration, span colorDuration) 
void meteor(span onDuration, span offDuration, span meteorSpawnDelay, unsigned int fadeAmount) 
void sparkleTrace(span dotFrequency) 
void vortexWipe(span onDuration, span offDuration, span warpDuration) 
void warp(span onDuration, span offDuration, span warpDuration) 
void warpWorm(span onDuration, span offDuration, span wormSpeed, unsigned int wormSize) 
void snowBall(span onDuration, span offDuration, span wormSpeed, span wormSize) 
void lighthouse(span onDuration, span offDuration, span fingerSpeed, span fadeAmount) 
void pulsish(span onDuration, span offDuration, span onDuration2, span offDuration2, span fingerSpeed) 
void fill(span onDuration, span offDuration, span fillSpeed) 
void bounce(span onDuration, span offDuration, span bounceDuration) 
void impact(span onDuration, span offDuration, span onDuration2, span offDuration2,
             span onDuration3, span offDuration3, span onDuration4, span offDuration4) 
void rabbit(span onDuration, span offDuration, span onDuration2, span offDuration2) 
void splitStrobie(span onDuration, span offDuration, span onDuration2, span offDuration2, span switchDuration) 
void backstrobe(span onDuration, span offDuration, span onDuration2, span offDuration2, span switchDuration) 
void flowers(span onDuration, span offDuration, span onDuration2, span offDuration2) 
void jest(span onDuration, span offDuration, span gapDuration, span gapDuration2, unsigned int groupSize) 

void strobe()           { basicPattern(5, 8); }
void hyperstrobe()      { basicPattern(25, 25); }
void dops()             { basicPattern(2, 13); }
void dopish()           { basicPattern(2, 7); }
void ultraDops()        { basicPattern(1, 3); } // dops but a lot
void strobie()          { basicPattern(3, 22); }
void blinkie()          { basicPattern(5, 8, 35); }
void ghostCrush()       { basicPattern(1, 0, 50); } // breifly flash all colors in set before a pause
void ribbon()           { basicPattern(20); }
void miniRibbon()       { basicPattern(3); } // chroma but tighter
void dashDops()         { dashDops(1, 5, 20); } // first color dash, rest of the colors dops
void allTracer()        { tracer(10, 20); } // synchronized tracer
void blend()            { blendPattern(2, 13); } // gradient between selected colors with dops blink
void brackets()         { brackets(12, 5, 50); } // candle strobe?
void theaterChase()     { theaterChase(5, 8, 25, 125); } // finger chase with tip/top dops flip

void zigZag()           { zigZag(3, 5, 50); } // Dot zips from thumb to pinkie Tops while dot2 zips pinkie to thumb tips with dops

void zipFade()          { zipFade(3, 22, 100, 2); } // Zip with a constant fade with dops
void tipTop()           { tipTop(5, 8, 25, 25); } // Tips hyperstrobe color 1, tops strobe full set
void drip()             { drip(10, 10, 250); } // Tops to tips with strobe
void dripMorph()        { dripMorph(10, 10); }
void crossDops()        { crossDops(2, 13, 100); } // dops on alternating tips and tops
void doubleStrobe()     { doubleStrobe(5, 8, 75); } // flash next 2 colors simultaneously
void slothStrobe()      { doubleStrobe(5, 8, 1000); } // flash next 2 colors simultaneously
void meteor()           { meteor(5, 8, 3, 75); } // randomly flash fingers while constantly dimming
void sparkleTrace()     { sparkleTrace(6); } // randomized tracer
void vortexWipe()       { vortexWipe(2, 7, 125); } // Color wipe across tops then tips with dops
void vortexUltraWipe()  { vortexWipe(2, 7, 50); } // Color wipe across tops then tips with dops
void warp()             { warp(2, 7, 100); } // Dot travels from thumb to pinkie then next color with dops
void warpWorm()         { warpWorm(2, 7, 100, 6); } // Group of color 1 lights the travel from thumb to pinky with dops
void snowball()         { snowBall(2, 7, 100, 3); } // Group of all colors travel from thumb to pinkie with dops
void lighthouse()       { lighthouse(5, 8, 100, 2); } // Warp from thumb to pinkie with constant fade with dops
void pulsish()          { pulsish(2, 13, 1, 3, 250); } // Warp color 1 with strobe, full hand ghost dops
void fill()             { fill(5, 24, 200); } // Fill thumb to pinkie with strobes
void bounce()           { bounce(5, 8, 10); } // Bounce and change colors between thumb/pinkie with strobe
void impact()           { impact(25, 250, 1, 8, 3, 22, 5, 8); } // independend thumbs tops and tips
void rabbit()           { rabbit(3, 22, 5, 8); }
void splitStrobie()     { splitStrobie(5, 8, 3, 22, 1000); } // dops/tracer tips tops swap
void backstrobe()       { backstrobe(2, 2, 50, 50, 1000); } // Hyperstrobe/dops tips tops swap
void flowers()          { flowers(5, 25, 3, 5); }
void jest()             { jest(0, 1, 5, 69, 3); }

void Pattern::basicPattern(span onDuration, span offDuration, span gapDuration) {
  if (onDuration == 2 && offDuration >= 5 && offDuration <= 31) onDuration = 3;
  static bool lightsOn = true;
  static span previousClockTime = 0;
  static span timerDuration = 0;
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

void Pattern::dashDops(span dotDuration, span offDuration, span dashDuration) {

  static bool lightsOn = false;
  static span previousClockTime = 0;
  static span timerDuration = 0;

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

void Pattern::tracer(span dashDuration, span dotDuration) {
  static bool lightsOn = false;
  static span previousClockTime = 0;
  static span timerDuration = 0;

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

void Pattern::blendPattern(span onDuration, span offDuration) {
  if (onDuration == 2 && offDuration >= 5 && offDuration <= 31) onDuration = 3;

  static bool lightsOn = true ;
  static span previousClockTime = 0;
  static span timerDuration = 0;
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
    if (color1 + colorGap >= 255) colorGap -= 255;                                       //
    if (color1 + colorGap < 0) colorGap += 255;                                               //
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

void Pattern::brackets(span onDuration, span edgeDuration, span offDuration) {
  static bool lightsOn = true ;
  static span previousClockTime, timerDuration;
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

void Pattern::theaterChase(span onDuration, span offDuration, span flipDuration, span fingerDuration) {
  if (fingerDuration <= 50) fingerDuration = 50;

  static bool lightsOn = true;
  static bool lightsOn2 = true;
  static bool lightsOn3 = true;
  static span previousClockTime, timerDuration;
  static span previousClockTime2;
  static span previousClockTime3;

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

void Pattern::zigZag(span onDuration, span offDuration, span zipDuration) {
  if (zipDuration <= 8) zipDuration = 8;

  static bool lightsOn = true;
  static span previousClockTime, timerDuration;
  static span previousClockTime2;
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

void zigzag(uint64_t tick, uint32_t fingerIndex)
{
  // colorset[4] = { R, G, B, P }
  switch(fingerIndex) {
    case PINKIE:
      static int counter = 0;
      if ((tick % 4) == 0) {
        ledOn(colorset[counter++]);
      }
      break;
    case RING:

}

void Pattern::zipFade(span onDuration, span offDuration, span zipDuration, span fadeDuration) {
  if (zipDuration <= 15) zipDuration = 15;

  static bool lightsOn = true;
  static span previousClockTime, timerDuration;
  static span previousClockTime2;
  static span previousClockTime3;
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

void Pattern::tipTop(span onDuration, span offDuration, span onDuration2, span offDuration2) {
  if (onDuration == 2 && offDuration >= 5 && offDuration <= 31) onDuration = 3;
  if (onDuration2 == 2 && offDuration2 >= 5 && offDuration2 <= 31) onDuration2 = 3;

  static bool lightsOn = true;
  static bool lightsOn2 = true;
  static span previousClockTime, timerDuration;
  static span previousClockTime2, timerDuration2;

  clearAll();
  for (int i = 0; i < NUM_LEDS; i++) {      //
    if (i % 2 == 0) {                       // Tops (evens)
      if (lightsOn) {                            //
        getColor(mode.currentColor);             // get current color
        setLed(i);
        timerDuration = onDuration;              // blink tops on
      }
      else if (!lightsOn) {
        clearLight(i);                       // blink tops off
        timerDuration = offDuration;
      }
    }
    if (i % 2 == 1) {                       // Tips (odds)
      if (lightsOn2) {                             //
        getColor(0);                        // get first color
        setLed(i);                          // blink tips on
        timerDuration2 = onDuration2;
      }
      else if (!lightsOn2) {
        clearLight(i);                   // blink tips off
        timerDuration2 = offDuration2;
      }
    }
  }
  if (mainClock - previousClockTime > timerDuration) {         // blink rate tops
    lightsOn = !lightsOn;                             //
    if (offDuration == 0) lightsOn = true;
    if (lightsOn) if (mode.numColors > 1) nextColor(1);
    previousClockTime = mainClock;                  //
  }
  if (mainClock - previousClockTime2 > timerDuration2) {          // blink rate tips
    lightsOn2 = !lightsOn2;                               //
    if (offDuration2 == 0) lightsOn2 = true;
    previousClockTime2 = mainClock;
  }
}

void Pattern::drip(span onDuration, span offDuration, span dripDuration) {
  static bool lightsOn = true;
  static bool lightsOn2 = true;
  static span previousClockTime, timerDuration;
  static span previousClockTime2;

  if (lightsOn) {                                 // blink on
    getColor(mode.currentColor);                 //
    setLeds(0, NUM_LEDS);                   // set all to current color

    if (!lightsOn2) {                       // every 2nd frame
      for (int i = 0; i < NUM_LEDS; i++) {
        if (i % 2 == 0) {                   // find Tops (evens)
          if (mode.numColors > 1) getColor(mode.nextColor);                   // get next color
          else val = 0;
          setLed(i);                        // blink tops on
        }
      }
    }
    timerDuration = onDuration;
  }
  else {
    clearAll();                          // blink off
    timerDuration = offDuration;
  }
  if (mainClock - previousClockTime > timerDuration) {         // blink rate
    lightsOn = !lightsOn;                                                  //
    if (offDuration == 0) lightsOn = true;
    previousClockTime = mainClock;                             //
  }
  if (mainClock - previousClockTime2 > dripDuration) {         // drip speed
    lightsOn2 = !lightsOn2;
    if (lightsOn2) nextColor(0);                               // next color
    previousClockTime2 = mainClock;
  }
}

void Pattern::dripMorph(span onDuration, span offDuration) {
  static bool lightsOn = true;
  static bool lightsOn2 = true;
  static span previousClockTime, timerDuration;
  static int previousHue = 0;
  static int progress = 0;

  if (lightsOn) {
    getColor(mode.currentColor);                                                    // between current and
    for (int a = 0; a < 10; a++) {
      if (a % 2 == 1) leds[a].setHSV(previousHue, sat, val);
    }
    int color1 = mode.hue[mode.currentColor];                                    // next colors and set
    int color2 = mode.hue[mode.nextColor];                                            // all leds
    if (mode.val[0] == 0) mode.val[0] = 85;
    if (mode.numColors < 2) color2 = mode.hue[mode.currentColor];
    if (color1 > color2 && color1 - color2 < (255 - color1) + color2)progress--;    //
    if (color1 > color2 && color1 - color2 > (255 - color1) + color2)progress++;    //
    if (color1 < color2 && color2 - color1 < (255 - color2) + color1)progress++;    //
    if (color1 < color2 && color2 - color1 > (255 - color2) + color1)progress--;    //
    if (color1 + progress >= 255) progress -= 255;                                       //
    if (color1 + progress < 0) progress += 255;                                          //
    int finalHue = color1 + progress;                                               //
    if (finalHue == color2) {
      progress = 0, nextColor(0);                             //
      previousHue = finalHue;
    }
    for (int a = 0; a < 10; a++) {
      if (a % 2 == 0) leds[a].setHSV(finalHue, sat, val);            //
    }
    timerDuration = onDuration;
  }
  else {
    clearAll();
    timerDuration = offDuration;
  }
  if (mainClock - previousClockTime > timerDuration) {         // blink rate
    lightsOn = !lightsOn;                                      //
    if (offDuration == 0) lightsOn = true;
    previousClockTime = mainClock;                             //
  }
}

void Pattern::crossDops(span onDuration, span offDuration, span flipDuration) {

  static bool lightsOn = true;
  static bool lightsOn2 = true;
  static span previousClockTime, timerDuration;
  static span previousClockTime2;

  clearAll();                    // clear leds
  getColor(mode.currentColor);   // get current color
  if (lightsOn) {                 // when blink is on
    if (lightsOn2) {              // and blink2 is on
      setLed(0);                  // set led 0,3,4,7,8
      setLed(3);
      setLed(4);
      setLed(7);
      setLed(8);
    }
  }
  if (!lightsOn) {                // when blink is not on
    if (lightsOn2) {              // and blink2 is on
      setLed(1);            // set led 1,2,5,6,9
      setLed(2);
      setLed(5);
      setLed(6);
      setLed(9);
    }
  }
  if (lightsOn2) timerDuration = onDuration;
  if (!lightsOn2) timerDuration = offDuration;
  if (mainClock - previousClockTime > timerDuration) {       // check if timer2 has passed 2ms
    lightsOn2 = !lightsOn2;                             // flip blink2 on/off
    if (offDuration == 0) lightsOn2 = true;
    previousClockTime = mainClock;                      // refresh timer2
  }
  if (mainClock - previousClockTime2 > flipDuration) { // check if timer has passed 100ms
    nextColor (0);                                     // que next color
    lightsOn = !lightsOn;                              // flip blink on/off
    previousClockTime2 = mainClock;                    // refresh timer
  }
}

void Pattern::doubleStrobe(span onDuration, span offDuration, span colorDuration) {
  static bool lightsOn = true;
  static span previousClockTime, timerDuration;
  static span previousClockTime2;

  clearAll();
  if (lightsOn) {                               // Strobe 1
    for (int i = 0; i < NUM_LEDS; i++) {  //
      if (i % 2 == 0) {                   //
        getColor(mode.currentColor);           //
        setLed(i);                        //
      }
      if (i % 2 == 1) {                   // Strobe 2
        if (mode.numColors > 1) getColor(mode.nextColor);                   //
        setLed(i);                        //
      }
      timerDuration = onDuration;
    }
  }
  if (!lightsOn) timerDuration = offDuration;
  if (mainClock - previousClockTime > timerDuration) {       // Strobe timer
    lightsOn = !lightsOn;                             //
    if (offDuration == 0) lightsOn = true;
    previousClockTime = mainClock;                //
  }
  if (mainClock - previousClockTime2 > colorDuration) {       // Color timer
    nextColor(0);                         //
    previousClockTime2 = mainClock;                 //
  }
}

void Pattern::meteor(span onDuration, span offDuration, span meteorSpawnDelay, unsigned int fadeAmount) {
  static bool lightsOn = true;
  static span previousClockTime, timerDuration;
  static span previousClockTime2;
  static int finger = 0;

  if (lightsOn) {
    for (int a = 0; a < NUM_LEDS; a++)leds[a].fadeToBlackBy(fadeAmount);
    setLed(finger * 2);
    setLed(finger * 2 + 1);
    timerDuration = onDuration;
  }
  if (!lightsOn) timerDuration = offDuration;
  if (mainClock - previousClockTime > meteorSpawnDelay) {
    getColor(mode.currentColor);
    finger = random(0, 5);
    nextColor(0);
    previousClockTime = mainClock;
  }
  if (mainClock - previousClockTime2 > timerDuration) {   // strobe timer
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
    previousClockTime2 = mainClock;                       //
  }
}

void Pattern::sparkleTrace(span dotFrequency) {
  static span previousClockTime;

  getColor(0);                        // Get color 0
  setLeds(0, 9);                      // Set all LEDs
  getColor(mode.currentColor);           // get the current color in the set
  if (mode.numColors == 1) val = 0;    // (special case when set has 1 color)
  if (mainClock - previousClockTime > dotFrequency) {
    for (int c = 0; c < 4; c++) {     // for 4 repititions
      int r = random(0, 5);
      setLeds(r * 2, r * 2 + 1);                // choose a random pixel
    }
    if (mode.numColors > 1) nextColor (1);
    previousClockTime = mainClock;
  }
}

void Pattern::vortexWipe(span onDuration, span offDuration, span warpDuration) {
  if (onDuration == 2 && offDuration >= 5 && offDuration <= 31) onDuration = 3;

  static bool lightsOn = true;
  static span previousClockTime, timerDuration;
  static span previousClockTime2;
  static int progress = 0;

  getColor(mode.currentColor);       //
  if (lightsOn) {                     //
    setLeds(0, 9);              //
    if (mode.numColors > 1) getColor(mode.nextColor);             //
    else val = 0;
    setLed(0);                  //
    if (progress >= 1) setLed(2);    // Wipe progress
    if (progress >= 2) setLed(4);    //
    if (progress >= 3) setLed(6);    //
    if (progress >= 4) setLed(8);    //
    if (progress >= 5) setLed(9);    //
    if (progress >= 6) setLed(7);    //
    if (progress >= 7) setLed(5);    //
    if (progress >= 8) setLed(3);    //
    if (progress >= 9) setLed(1);    //
    timerDuration = onDuration;
  }
  if (!lightsOn) {
    clearAll();
    timerDuration = offDuration;
  }
  if (mainClock - previousClockTime > timerDuration) {  // Dops timer
    lightsOn = !lightsOn;
    if (offDuration == 0) lightsOn = true;
    previousClockTime = mainClock;          //                           //
  }
  if (mainClock - previousClockTime2 > warpDuration) {  // Wipe timer
    progress ++;                         //
    if (progress > 9) {                  //
      progress = 0;                      //
      nextColor(0);                      //
    }
    previousClockTime2 = mainClock;
  }
}

void Pattern::warp(span onDuration, span offDuration, span warpDuration) {
  if (onDuration == 2 && offDuration >= 5 && offDuration <= 31) onDuration = 3;

  static bool lightsOn = true;
  static span previousClockTime, timerDuration;
  static span previousClockTime2;
  static int progress = 0;

  clearAll();                       //
  if (lightsOn) {                         //
    getColor(mode.currentColor);         //
    setLeds(0, 9);                  //
    if (mode.numColors > 1) getColor(mode.nextColor);                 //
    else val = 0;
    setLed(progress);                    //
    timerDuration = onDuration;
  }
  if (!lightsOn) timerDuration = offDuration;

  if (mainClock - previousClockTime > timerDuration) {  //
    lightsOn = !lightsOn;                               //
    if (offDuration == 0) lightsOn = true;
    previousClockTime = mainClock;                      //
  }
  if (mainClock - previousClockTime2 > warpDuration) {  //
    progress++;                          //
    if (progress >= 10) {                //
      progress = 0;                      //
      nextColor(0);                 //
    }
    previousClockTime2 = mainClock;           //
  }
}

void Pattern:: warpWorm(span onDuration, span offDuration, span wormSpeed, unsigned int wormSize) {
  if (onDuration == 2 && offDuration >= 5 && offDuration <= 31) onDuration = 3;
  if (wormSize < 1) wormSize = 1;
  if (wormSize > 9) wormSize = 9;

  static bool lightsOn = true;
  static span previousClockTime, timerDuration;
  static span previousClockTime2;
  static int progress = 0;

  clearAll();                         //
  if (lightsOn) {                           //
    getColor(0);                      //
    setLeds(0, 9);                    //
    getColor(mode.currentColor);           //
    //if (mode.currentColor == 0) getColor(1);
    if (mode.numColors == 1) val = 0;
    for (int i = 0; i < wormSize; i++) {     //
      int worm = i + progress;              //
      if (worm > 9) worm -= 10;     //
      setLed(worm);                  //
    }
    timerDuration = onDuration;
  }
  if (!lightsOn)timerDuration = offDuration;
  if (mainClock - previousClockTime > timerDuration) {    //
    lightsOn = !lightsOn;                         //
    if (offDuration == 0) lightsOn = true;
    previousClockTime = mainClock;            //
  }
  if (mainClock - previousClockTime2 > wormSpeed) {
    progress++;
    if (progress > 9) {
      progress = 0;
      if (mode.numColors > 1) nextColor(1);

    }
    previousClockTime2 = mainClock;
  }
}

void Pattern::snowBall(span onDuration, span offDuration, span wormSpeed, span wormSize) {
  if (onDuration == 2 && offDuration >= 5 && offDuration <= 31) onDuration = 3;
  if (wormSpeed == 5) wormSpeed = 4;
  if (wormSpeed == 6) wormSpeed = 7;
  if (wormSize < 1) wormSize = 1;
  if (wormSize > 9) wormSize = 9;

  static bool lightsOn = true;
  static span previousClockTime, timerDuration;
  static span previousClockTime2;
  static int progress = 0;

  clearAll();                           //
  if (lightsOn) {                             //
    getColor(0);                        // set trace
    setLeds(0, 9);                      //
    getColor(mode.currentColor);             //
    if (mode.numColors == 1) val = 0;
    for (int i = 0; i < wormSize; i++) {       // set worm
      int worm = i + progress;                //
      if (worm > 9) worm -= 10;       //
      setLed(worm);                    //
    }
    timerDuration = onDuration;
  }
  if (!lightsOn) timerDuration = offDuration;
  if (mainClock - previousClockTime > timerDuration) {              // dops timer
    lightsOn = !lightsOn;
    if (offDuration == 0) lightsOn = true;
    previousClockTime = mainClock;
  }
  if (mainClock - previousClockTime2 > wormSpeed) {              // color/worm timer
    if (mode.numColors > 1) nextColor(1);
    if (mode.currentColor == 1) progress++;                         //
    if (progress > 9) progress = 0;                           //
    previousClockTime2 = mainClock;
  }
}

void Pattern::lighthouse(span onDuration, span offDuration, span fingerSpeed, span fadeAmount) {
  if (onDuration == 2 && offDuration >= 5 && offDuration <= 31) onDuration = 3;

  static bool lightsOn = true;
  static span previousClockTime, timerDuration;
  static span previousClockTime2, previousClockTime3;
  static int progress = 0;

  getColor(mode.currentColor);               // get the next color
  setLed(progress);                               // set to the next finger
  if (lightsOn) timerDuration = onDuration;                                                                 //
  if (!lightsOn) timerDuration = offDuration;
  if (mainClock - previousClockTime > timerDuration) {          // dops timer
    lightsOn = !lightsOn;                               //
    if (lightsOn) {                               //
      for (int a = 0; a < NUM_LEDS; a++) {  // special save/load
        leds[a] = copy[a];                  // for fade effect
      }                                     // with dops
    }                                       //
    if (!lightsOn) {                              //
      for (int a = 0; a < NUM_LEDS; a++) {  //
        copy[a] = leds[a];                  //
      }                                     //
      clearAll();                           //
    }                                       //
    previousClockTime = mainClock;                  //
  }
  if (mainClock - previousClockTime2 > 1) {                                // Fade timer
    for (int a = 0; a < NUM_LEDS; a++) leds[a].fadeToBlackBy(fadeAmount);  //
    previousClockTime2 = mainClock;                                         //
  }
  if (mainClock - previousClockTime3 > fingerSpeed) {      // Finger/color timer
    progress++;                              //
    if (progress % 2 == 0) nextColor(0);     //
    if (progress > 9) {                      //
      progress = 0;                          //
    }                                   //
    previousClockTime3 = mainClock;               //
  }
}

void Pattern::pulsish(span onDuration, span offDuration, span onDuration2, span offDuration2, span fingerSpeed) {
  if (onDuration == 2 && offDuration >= 5 && offDuration <= 31) onDuration = 3;
  if (onDuration2 == 2 && offDuration >= 5 && offDuration <= 31) onDuration2 = 3;

  static bool lightsOn = true;
  static bool lightsOn2 = true;
  static span previousClockTime, timerDuration, timerDuration2;
  static span previousClockTime2, previousClockTime3;
  static int progress = 0;

  if (lightsOn) {                                         // blink on
    getColor(mode.currentColor);                         //
    if (mode.numColors == 1) val = 0;
    setLeds(0, 27);                                 // set all to current color
    timerDuration = onDuration;
  }
  if (!lightsOn) {
    clearAll();                                  // blink off
    timerDuration = offDuration;
  }
  getColor(0);                                        // get first color
  if (lightsOn2) {
    setLeds(0 + (2 * progress), 1 + (2 * progress)); // blink pulse to first color
    timerDuration2 = onDuration2;
  }
  else {                                              //
    timerDuration2 = offDuration2;
    clearLight(0 + (2 * progress));                      // blink pulse off
    clearLight(1 + (2 * progress));                      //
  }
  if (mainClock - previousClockTime > timerDuration) {              // blink rate
    lightsOn = !lightsOn;                                         //
    if (offDuration == 0) lightsOn = true;
    if (lightsOn) if (mode.numColors > 1) nextColor(1);                             // get next color
    previousClockTime = mainClock;                             //
  }
  if (mainClock - previousClockTime2 > timerDuration2) {                   // blink rate of pulse
    lightsOn2 = !lightsOn2;                                       //
    if (offDuration2 == 0) lightsOn2 = true;
    previousClockTime2 = mainClock;                            //
  }
  if (mainClock - previousClockTime3 > fingerSpeed) {                  // pulse move speed
    progress++;                                          //
    if (progress > 4) progress = 0;                         //
    previousClockTime3 = mainClock;                            //
  }
}

void Pattern::fill(span onDuration, span offDuration, span fillSpeed) {
  if (onDuration == 2 && offDuration >= 5 && offDuration <= 31) onDuration = 3;

  static bool lightsOn = true;
  static span previousClockTime, timerDuration;
  static span previousClockTime2;
  static int progress = 0;

  if (lightsOn) {
    getColor(mode.currentColor);                 //
    setLeds(0, NUM_LEDS);                   // set all to current color

    if (mode.numColors > 1) getColor(mode.nextColor);                         // get next color
    else val = 0;
    setLeds(0, (progress * 2) - 1);            // set LEDs up to current fill
    timerDuration = onDuration;

  }
  else {
    clearAll();
    timerDuration = offDuration;
  }
  if (mainClock - previousClockTime > timerDuration) {          // blink rate
    lightsOn = !lightsOn;
    if (offDuration == 0) lightsOn = true;
    previousClockTime = mainClock;
  }
  if (mainClock - previousClockTime2 > fillSpeed) {
    progress++;                                // fill speed
    if (progress >= 5) {                       //
      progress = 0;                            //
      nextColor(0);                         // get next color after 5 fingers filled
    }
    previousClockTime2 = mainClock;
  }
}

void Pattern::bounce(span onDuration, span offDuration, span bounceDuration) {
  if (onDuration == 2 && offDuration >= 5 && offDuration <= 31) onDuration = 3;

  static bool lightsOn = true;
  static span previousClockTime, timerDuration;
  static span previousClockTime2;
  static int progress = 0;

  if (lightsOn) {                                                             // blink on
    getColor(mode.currentColor);                                             // get current color
    setLeds(0, NUM_LEDS);                                               // set all leds
    getColor(mode.nextColor);                                                     // get next color
    if (mode.numColors == 1) val = 0;
    setLed (int(triwave8(progress) / 25.4));                               // set bounce led
    timerDuration = onDuration;
  }
  else {
    clearAll();                                                      // blink off
    timerDuration = offDuration;
  }

  if (mainClock - previousClockTime > timerDuration) {                                // bounce and blink rate
    lightsOn = !lightsOn;                                                           //
    if (offDuration == 0) lightsOn = true;
    previousClockTime = mainClock;                                               //
  }
  if (mainClock - previousClockTime2 > bounceDuration) {
    progress += 2;
    if (triwave8(progress) == 0 || triwave8(progress) == 254) nextColor(0);   // next color on thumb/pinky
    previousClockTime2 = mainClock;
  }
}

void Pattern::impact(span onDuration, span offDuration, span onDuration2, span offDuration2,
                      span onDuration3, span offDuration3, span onDuration4, span offDuration4) {
  if (onDuration == 2 && offDuration >= 5 && offDuration <= 31) onDuration = 3;
  if (onDuration2 == 2 && offDuration >= 5 && offDuration <= 31) onDuration2 = 3;
  if (onDuration3 == 2 && offDuration >= 5 && offDuration <= 31) onDuration3 = 3;
  if (onDuration4 == 2 && offDuration >= 5 && offDuration <= 31) onDuration4 = 3;

  static bool lightsOn = true;
  static bool lightsOn2 = true;
  static bool lightsOn3 = true;
  static bool lightsOn4 = true;
  static span timerDuration, timerDuration2, timerDuration3, timerDuration4;
  static span previousClockTime, previousClockTime2, previousClockTime3, previousClockTime4;

  clearAll();
  getColor(0);                            // Set thumb to blink first color infrequently
  if (lightsOn) timerDuration = onDuration, setLeds(0, 1);  //
  if (!lightsOn) timerDuration = offDuration;               //

  if (lightsOn2) {
    getColor(1);
    if (mode.numColors == 1) val = 0;
    setLeds(4, 5);
    timerDuration2 = onDuration2;
  }
  if (!lightsOn2) timerDuration2 = offDuration2;

  if (lightsOn3) {
    getColor(2);                                                                 // Set tops to color 2
    if (mode.numColors <= 2) val = 0;
    for (int fingers = 0; fingers < 4; fingers++) {
      if (fingers != 1) setLed(2 + fingers * 2);                                  //
    }
    timerDuration3 = onDuration3;
  }
  if (!lightsOn3) timerDuration3 = offDuration3;

  if (lightsOn4) {
    getColor(mode.currentColor);                                                 // Set tips to next color starting from 3rd color
    if (mode.numColors <= 3) val = 0;                                          //
    for (int fingers = 0; fingers < 4; fingers++) {
      if (fingers != 1) setLed(3 + fingers * 2);                            //
    }
    timerDuration4 = onDuration4;
  }
  if (!lightsOn4) timerDuration4 = offDuration4;

  if (mainClock - previousClockTime > timerDuration) {
    lightsOn = !lightsOn;
    if (offDuration == 0) lightsOn = true;
    previousClockTime = mainClock;
  }
  if (mainClock - previousClockTime2 > timerDuration2) {
    lightsOn2 = !lightsOn2;
    if (offDuration2 == 0) lightsOn2 = true;
    previousClockTime2 = mainClock;
  }
  if (mainClock - previousClockTime3 > timerDuration3) {
    lightsOn3 = !lightsOn3;
    if (offDuration3 == 0) lightsOn3 = true;
    previousClockTime3 = mainClock;
  }
  if (mainClock - previousClockTime4 > timerDuration4) {
    lightsOn4 = !lightsOn4;
    if (offDuration4 == 0) lightsOn4 = true;
    if (lightsOn4) nextColor(3);
    previousClockTime4 = mainClock;
  }
}

void Pattern::rabbit(span onDuration, span offDuration, span onDuration2, span offDuration2) {
  if (onDuration == 2 && offDuration >= 5 && offDuration <= 31) onDuration = 3;
  if (onDuration2 == 2 && offDuration >= 5 && offDuration <= 31) onDuration2 = 3;

  static bool lightsOn = true;
  static bool lightsOn2 = true;
  static span previousClockTime, timerDuration;
  static span previousClockTime2, timerDuration2;

  getColor(0);
  if (lightsOn) {
    for (int i = 0; i < NUM_LEDS; i++) {
      if (i % 2 == 0) setLed(i);
    }
    timerDuration = onDuration;
  }
  if (!lightsOn) {
    clearAll();
    timerDuration = offDuration;
  }

  getColor(mode.currentColor);
  if (lightsOn2) {
    for (int i = 0; i < NUM_LEDS; i++) {
      if (i % 2 == 1) setLed(i);
    }
    timerDuration2 = onDuration2;
  }
  if (!lightsOn2) {
    for (int i = 0; i < NUM_LEDS; i++) {
      if (i % 2 == 1) clearLight(i);
    }
    timerDuration2 = offDuration2;
  }

  if (mainClock - previousClockTime > timerDuration) {
    lightsOn = !lightsOn;
    if (offDuration == 0) lightsOn = true;
    previousClockTime = mainClock;
  }
  if (mainClock - previousClockTime2 > timerDuration2) {
    lightsOn2 = !lightsOn2;
    if (offDuration2 == 0) lightsOn2 = true;
    if (lightsOn2) if (mode.numColors > 1)nextColor(1);
    previousClockTime2 = mainClock;
  }
}

void Pattern::splitStrobie(span onDuration, span offDuration, span onDuration2, span offDuration2, span switchDuration) {
  if (onDuration == 2 && offDuration >= 5 && offDuration <= 31) onDuration = 3;
  if (onDuration2 == 2 && offDuration >= 5 && offDuration <= 31) onDuration2 = 3;

  static bool lightsOn = true;
  static bool lightsOn2 = true;
  static bool lightsOn3 = true;
  static span previousClockTime, timerDuration;
  static span previousClockTime2, timerDuration2;
  static span previousClockTime3;

  if (lightsOn) {
    getColor(mode.currentColor);
    for (int i = 0; i < NUM_LEDS; i++) {
      if (lightsOn3) if (i % 2 == 0) setLed(i);
      if (!lightsOn3) if (i % 2 == 1) setLed(i);
    }
    timerDuration = onDuration;
  }
  if (!lightsOn) {
    clearAll();
    timerDuration = offDuration;
  }

  getColor(mode.currentColor1);
  if (lightsOn2) {
    for (int i = 0; i < NUM_LEDS; i++) {
      if (lightsOn3) if (i % 2 == 1) setLed(i);
      if (!lightsOn3)if (i % 2 == 0) setLed(i);
    }
    timerDuration2 = onDuration2;
  }
  if (!lightsOn2) {
    for (int i = 0; i < NUM_LEDS; i++) {
      if (lightsOn3) if (i % 2 == 1) clearLight(i);
      if (!lightsOn3) if (i % 2 == 0) clearLight(i);
    }
    timerDuration2 = offDuration2;
  }

  if (mainClock - previousClockTime > timerDuration) {
    lightsOn = !lightsOn;
    if (offDuration == 0) lightsOn = true;
    if (lightsOn) {
      nextColor(0);                                                 // next color
      if (mode.currentColor > 3) mode.currentColor = 0;
    }
    previousClockTime = mainClock;
  }
  if (mainClock - previousClockTime2 > timerDuration2) {
    lightsOn2 = !lightsOn2;
    if (offDuration2 == 0) lightsOn2 = true;
    if (lightsOn2) {
      if (mode.numColors > 4) {
        mode.currentColor1 ++;
        if (mode.currentColor1 >= mode.numColors) mode.currentColor1 = 4;
      }
    }
    previousClockTime2 = mainClock;
  }
  if (mainClock - previousClockTime3 > switchDuration) {                   //switch timing
    lightsOn3 = !lightsOn3;
    if (switchDuration == 0) lightsOn3 = true;
    previousClockTime3 = mainClock;
  }
}

void Pattern::backstrobe(span onDuration, span offDuration, span onDuration2, span offDuration2, span switchDuration) {
  if (onDuration == 2 && offDuration >= 5 && offDuration <= 31) onDuration = 3;
  if (onDuration2 == 2 && offDuration >= 5 && offDuration <= 31) onDuration2 = 3;

  static bool lightsOn = true;
  static bool lightsOn2 = true;
  static bool lightsOn3 = true;
  static span previousClockTime, timerDuration;
  static span previousClockTime2, timerDuration2;
  static span previousClockTime3;

  if (lightsOn) {
    getColor(mode.currentColor);
    for (int i = 0; i < NUM_LEDS; i++) {
      if (lightsOn3) if (i % 2 == 0) setLed(i);
      if (!lightsOn3) if (i % 2 == 1) setLed(i);
    }
    timerDuration = onDuration;
  }
  if (!lightsOn) {
    clearAll();
    timerDuration = offDuration;
  }

  getColor(mode.currentColor1);
  if (lightsOn2) {
    for (int i = 0; i < NUM_LEDS; i++) {
      if (lightsOn3) if (i % 2 == 1) setLed(i);
      if (!lightsOn3)if (i % 2 == 0) setLed(i);
    }
    timerDuration2 = onDuration2;
  }
  if (!lightsOn2) {
    for (int i = 0; i < NUM_LEDS; i++) {
      if (lightsOn3) if (i % 2 == 1) clearLight(i);
      if (!lightsOn3) if (i % 2 == 0) clearLight(i);
    }
    timerDuration2 = offDuration2;
  }

  if (mainClock - previousClockTime > timerDuration) {
    lightsOn = !lightsOn;
    if (offDuration == 0) lightsOn = true;
    if (lightsOn) nextColor(0);                                                 // next color
    previousClockTime = mainClock;
  }
  if (mainClock - previousClockTime2 > timerDuration2) {
    lightsOn2 = !lightsOn2;
    if (offDuration2 == 0) lightsOn2 = true;
    if (lightsOn2) nextColor1(0);
    previousClockTime2 = mainClock;
  }
  if (mainClock - previousClockTime3 > switchDuration) {                   //switch timing
    lightsOn3 = !lightsOn3;
    if (switchDuration == 0) lightsOn3 = true;
    previousClockTime3 = mainClock;
  }
}

void Pattern::flowers(span onDuration, span offDuration, span onDuration2, span offDuration2) {
  if (onDuration == 2 && offDuration >= 5 && offDuration <= 31) onDuration = 3;
  if (onDuration2 == 2 && offDuration >= 5 && offDuration <= 31) onDuration2 = 3;

  static bool lightsOn = true;
  static bool lightsOn2 = true;
  static bool lightsOn3 = true;
  static span previousClockTime, timerDuration;
  static span previousClockTime2, timerDuration2;

  if (lightsOn) {
    getColor(mode.currentColor1);
    setLeds(0, 1);
    setLeds(8, 9);
    timerDuration = onDuration;
  }
  if (!lightsOn) {
    clearLights(0, 1);
    clearLights(8, 9);
    timerDuration = offDuration;

  }

  if (lightsOn2) {
    getColor(mode.currentColor);
    setLeds(2, 7);
    timerDuration2 = onDuration2;
  }
  if (!lightsOn2) {
    clearLights(2, 7);
    timerDuration2 = offDuration2;

  }

  if (mainClock - previousClockTime > timerDuration) {
    lightsOn = !lightsOn;
    if (offDuration == 0) lightsOn = true;
    if (!lightsOn) {
      if (mode.numColors > 1) {
        mode.currentColor1 ++;
        if (mode.currentColor1 >= 2) mode.currentColor1 = 0;
      }
    }
    previousClockTime = mainClock;
  }
  if (mainClock - previousClockTime2 > timerDuration2) {
    lightsOn2 = !lightsOn2;
    if (offDuration2 == 0) lightsOn = true;
    if (lightsOn2) if (mode.numColors > 2) nextColor(2);
    previousClockTime2 = mainClock;
  }
}

void Pattern::jest(span onDuration, span offDuration, span gapDuration, span gapDuration2, unsigned int groupSize) {
  if (onDuration == 2 && offDuration >= 5 && offDuration <= 31) onDuration = 3;

  static bool lightsOn = true;
  static bool lightsOn2 = true;
  static span previousClockTime, timerDuration;
  static span previousClockTime2, timerDuration2;
  static int groupCount, groupCount2;

  clearAll();
  if (lightsOn) {
    getColor(mode.currentColor);
    for (int a = 0; a < NUM_LEDS; a++) {
      if (a % 2 == 1) setLed(a);
    }
    timerDuration = onDuration;
  }
  if (!lightsOn) {
    clearAll();
    timerDuration = offDuration;
    if (groupCount == 0) if (gapDuration != 0) timerDuration = gapDuration;
  }
  if (lightsOn2) {
    getColor(mode.currentColor1);
    for (int a = 0; a < NUM_LEDS; a++) {
      if (a % 2 == 0) setLed(a);
    }
    timerDuration2 = onDuration;
  }
  if (!lightsOn2) {
    for (int a = 0; a < NUM_LEDS; a++) {
      if (a % 2 == 0) clearLight(a);
    }
    timerDuration2 = offDuration;
    if (groupCount2 == 0) if (gapDuration2 != 0) timerDuration2 = gapDuration2;
  }
  if (mainClock - previousClockTime > timerDuration) {
    lightsOn = !lightsOn;
    if (offDuration == 0 && gapDuration == 0) lightsOn = true;
    if (lightsOn) {
      nextColor(0);
      groupCount++;
      if (groupCount >= groupSize) groupCount = 0;
    }
    previousClockTime = mainClock;
  }
  if (mainClock - previousClockTime2 > timerDuration2) {
    lightsOn2 = !lightsOn2;
    if (offDuration == 0 && gapDuration2 == 0) lightsOn2 = true;
    if (lightsOn2) {
      nextColor1(0);
      groupCount2++;
      if (groupCount2 >= groupSize) groupCount2 = 0;
    }
    previousClockTime2 = mainClock;
  }
}
