#ifndef PATTERN_H
#define PATTERN_H

#include "Arduino.h"
#include <FastLED.h>

#include "Mode.h"

#define NUM_LEDS 28
#define DATA_PIN 4

typedef unsigned long span;

class Pattern
{
  public:
    Pattern();

    CRGB leds[NUM_LEDS];
    CRGB copy[NUM_LEDS];

    Mode mode;

    span time1 = 5;
    span time2 = 8;
    span time3 = 0;
    span time4 = 0;
    span time5 = 250;

    span mainClock;

    int hue, sat, val;

    void playPattern();
    void refresh(Mode thisMode);
    CRGB getLed(int i);

    void adjustValues(span v1, span v2, span v3, span v4, span v5);

  private:
    void getColor(int target);
    void setLed(int target);
    void setLeds(int first, int last);
    void nextColor(int start);
    void nextColor1(int start);
    void clearAll();
    void clearLight(int lightNum);
    void clearLights(int first, int last);

    void basicPattern(span onDuration, span offDuration = 0, span gapDuration = 0);
    void dashDops(span dashDuration, span dotDuration, span offDuration);
    void tracer(span dashDuration, span dotDuration);
    void blendPattern(span onDuration, span offDuration);
    void brackets(span onDuration, span edgeDuration = 0, span offDuration = 50);
    void theaterChase(span onDuration, span offDuration, span flipDuration = 25, span fingerDuration = 125);
    void zigZag(span onDuration, span offDuration, span zipDuration = 25);
    void zipFade(span onDuration, span offDuration, span zipDuration, span fadeDuration);
    void tipTop(span onDuration, span offDuration, span onDuration2, span offDuration2);
    void drip(span onDuration, span offDuration, span dripDuration = 250);
    void dripMorph(span onDuration, span offDuration);
    void crossDops(span onDuration, span offDuration, span flipDuration);
    void doubleStrobe(span onDuration, span offDuration, span colorDuration);
    void meteor(span onDuration, span offDuration, span meteorSpawnDelay, unsigned int fadeAmount);
    void sparkleTrace(span dotFrequency);
    void vortexWipe(span onDuration, span offDuration, span warpDuration);
    void warp(span onDuration, span offDuration, span warpDuration);
    void warpWorm(span onDuraiton, span offDuration, span wormSpeed, unsigned int wormSize);
    void snowBall(span onDuration, span offDuration, span wormSpeed, span wormSize);
    void lighthouse(span onDuration, span offDuration, span fingerSpeed, span fadeAmount);
    void pulsish(span onDuration, span offDuration, span onDuration2, span offduration2, span fingerSpeed);
    void fill(span onDuration, span offDuration, span fillSpeed);
    void bounce(span onDuration, span offDuration, span bounceDuration);
    void impact(span onDuration, span offDuration, span onDuration2, span offDuration2,
                span onDuration3, span offDuration3, span onDuration4, span offDuration4);
    void rabbit(span onDuration, span offDuration, span onDuration2, span offDuration2);
    void splitStrobie(span onDuration, span offDuration, span onDuration2, span offDuration2, span switcDuration);
    void backstrobe(span onDuration, span offDuration, span onDuration2, span offDuration2, span switchDuration);
    void flowers(span onDuration, span offDuration, span onDuration2, span offDuration2);
    void jest(span onDuration, span offDuration, span gapDuration, span gapDuration2, unsigned int groupSize);
};

#endif
