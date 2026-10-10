#include "Random.h"

void Random_make(Random *self)
{
  self->m_seed = 0;
}

void Random_makeSeed(Random *self, uint32_t newseed)
{
  Random_make(self);
  Random_seed(self, newseed);
}

void Random_destroy(Random *self)
{
}

void Random_seed(Random *self, uint32_t newseed)
{
  if (!newseed) {
    self->m_seed = 42;
  }
  self->m_seed = newseed;
}

uint16_t Random_next16(Random *self, uint16_t minValue, uint16_t maxValue)
{
  // walk the LCG forward to the next step
  self->m_seed = (self->m_seed * 1103515245 + 12345) & 0x7FFFFFFF;
  uint32_t range = maxValue - minValue;
  if (range != 0xFFFFFFFF) {
    // shift the seed 16 bits to the right because the lower 16 bits
    // of this LCG are apparently not uniform whatsoever, where as the
    // upper 16 bits appear to be quite uniform as per tests. We don't
    // really need 32bit random values so we offer max 16bits of entropy
    return ((self->m_seed >> 16) % (range + 1)) + minValue;
  }
  return (self->m_seed >> 16);
}

uint8_t Random_next8(Random *self, uint8_t minValue, uint8_t maxValue)
{
  uint32_t result = Random_next16(self, minValue, maxValue);
  return (uint8_t)result;
}
