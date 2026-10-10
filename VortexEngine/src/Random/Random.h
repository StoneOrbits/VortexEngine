#pragma once

#include <stdint.h>

typedef struct Random {
  uint32_t m_seed;
} Random;

void Random_make(Random *self);
void Random_makeSeed(Random *self, uint32_t newseed);
void Random_destroy(Random *self);

void Random_seed(Random *self, uint32_t newseed);

uint8_t Random_next8(Random *self, uint8_t minValue, uint8_t maxValue);
uint16_t Random_next16(Random *self, uint16_t minValue, uint16_t maxValue);

// default-argument shorthands for the common cases
static inline uint8_t Random_next8Default(Random *self)
{
  return Random_next8(self, 0, 0xFF);
}

static inline uint16_t Random_next16Default(Random *self)
{
  return Random_next16(self, 0, 0xFFFF);
}
