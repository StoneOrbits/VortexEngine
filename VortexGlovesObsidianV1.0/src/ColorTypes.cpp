#include "ColorTypes.h"

#include <Arduino.h>

void RGBColor::serialize() const
{
  Serial.print(red);
  Serial.print(green);
  Serial.print(blue);
}
