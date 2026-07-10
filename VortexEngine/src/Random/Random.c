#include "Random.h"

void Random_init(Random *self) {
  self->seed = 0;
}

void Random_initWithSeed(Random *self, uint32_t newseed) {
  Random_init(self);
  Random_seed(self, newseed);
}

void Random_seed(Random *self, uint32_t newseed) {
  self->seed = newseed ? newseed : 42;
}

uint16_t Random_next16(Random *self, uint16_t minValue, uint16_t maxValue) {
  self->seed = (self->seed * 1103515245 + 12345) & 0x7FFFFFFF;
  uint32_t range = maxValue - minValue;
  if (range != 0xFFFFFFFF) {
    return ((self->seed >> 16) % (range + 1)) + minValue;
  }
  return (self->seed >> 16);
}

uint8_t Random_next8(Random *self, uint8_t minValue, uint8_t maxValue) {
  uint32_t result = Random_next16(self, minValue, maxValue);
  return (uint8_t)result;
}
