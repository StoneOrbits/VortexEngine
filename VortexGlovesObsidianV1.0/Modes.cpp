#include "Modes.h"

Modes::Modes() {
  bool endOfSet;
  int hue[8];
  int sat[8];
  int val[8];
  uint8_t currentColor/*[4]*/, currentColor1;
  uint8_t nextColor = 1;
  uint8_t nextColor1 = 1;
  int numColors;
  int patternNum;
  int menuNum;

}

void Modes::saveColor(int slot, int thisHue, int thisSat, int thisVal) {
  if (slot == numColors)numColors++;
  hue[slot] = thisHue;
  sat[slot] = thisSat;
  val[slot] = thisVal;
}

void Modes::printMode(int num) {
  Serial.print("Mode "), Serial.println(num);
  for (int slot = 0; slot < numColors; slot ++) {
    Serial.print(hue[slot]), Serial.print(" ");
    Serial.print(sat[slot]), Serial.print(" ");
    Serial.println(val[slot]);
  }
  Serial.println();
}
