#include "Pattern.h"
#include "Arduino.h"
#include "Mode.h"

Pattern::Pattern() {
}

void Pattern::refresh(Mode thisMode) {
  mode = thisMode;
  clearAll();
}

CRGB Pattern::getLed(int i) {
  return leds[i];
}

void Pattern::getColor(int target) {
  hue = mode.hue[target];
  sat = mode.sat[target];
  val = mode.val[target];
}

void Pattern::setLed(int target) {
  leds[target].setHSV(hue, sat, val);
}

void Pattern::setLeds(int first, int last) {
  for (int a = first; a <= last; a++) setLed(a);
}

void Pattern::nextColor(int start) {
  mode.currentColor++;
  if (mode.currentColor >= mode.numColors) mode.currentColor = start;
  mode.nextColor = mode.currentColor + 1;
  if (mode.nextColor >= mode.numColors) mode.nextColor = start;
}

void Pattern::nextColor1(int start) {
  mode.currentColor1++;
  if (mode.currentColor1 >= mode.numColors) mode.currentColor1 = start;
  mode.nextColor1 = mode.currentColor1 + 1;
  if (mode.nextColor1 >= mode.numColors) mode.nextColor1 = start;
}

void Pattern::clearAll() {
  for (int a = 0; a < 28; a++) leds[a].setHSV(0, 0, 0);
}

void Pattern::clearLight(int lightNum) {
  leds[lightNum].setHSV(0, 0, 0);
}

void Pattern::clearLights(int first, int last) {
  for (int a = first; a <= last; a++) clearLight(a);
}

void Pattern::adjustValues(span v1, span v2, span v3, span v4, span v5) {
  time1 = v1;
  time2 = v2;
  time3 = v3;
  time4 = v4;
  time5 = v5;
}
