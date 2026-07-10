#ifndef RANDOM_H
#define RANDOM_H

#include <stdint.h>
#include <stdbool.h>

typedef struct Random_t {
  uint32_t seed;
} Random;

void Random_init(Random *self);
void Random_initWithSeed(Random *self, uint32_t newseed);
void Random_seed(Random *self, uint32_t newseed);
uint8_t Random_next8(Random *self, uint8_t minValue, uint8_t maxValue);
uint16_t Random_next16(Random *self, uint16_t minValue, uint16_t maxValue);

#endif
