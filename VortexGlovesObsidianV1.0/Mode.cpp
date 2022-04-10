#include "Mode.h"

Mode::Mode()
{

}

void Mode::saveColor(int slot, int thisHue, int thisSat, int thisVal)
{
  if (slot == numColors)numColors++;
  hue[slot] = thisHue;
  sat[slot] = thisSat;
  val[slot] = thisVal;
}

void Mode::printMode(int num)
{
  Serial.print("Mode "), Serial.println(num);
  for (int slot = 0; slot < numColors; slot ++) {
    Serial.print(hue[slot]), Serial.print(" ");
    Serial.print(sat[slot]), Serial.print(" ");
    Serial.println(val[slot]);
  }
  Serial.println();
}
