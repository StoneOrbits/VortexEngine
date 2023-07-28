#include "Random.h"

Random::Random() :
  m_seed(0)
{
}

Random::Random(uint32_t newseed) :
  Random()
{
  seed(newseed);
}

Random::~Random()
{
}

void Random::seed(uint32_t newseed)
{
  if (!newseed) {
    m_seed = 42;
  }
  m_seed = newseed;
}

uint16_t Random::next16(uint16_t minValue, uint16_t maxValue)
{
  // walk the LCG forward to the next step
  m_seed = (m_seed * 1103515245 + 12345) & 0x7FFFFFFF;
  uint32_t range = maxValue - minValue;
  if (range != 0xFFFFFFFF) {
    // shift the seed 16 bits to the right because the lower 16 bits
    // of this LCG are apparently not uniform whatsoever, where as the
    // upper 16 bits appear to be quite uniform as per tests. We don't
    // really need 32bit random values so we offer max 16bits of entropy
    return ((m_seed >> 16) % (range + 1)) + minValue;
  }
  return (m_seed >> 16);
}

uint8_t Random::next8(uint8_t minValue, uint8_t maxValue)
{
  uint32_t result = next16(minValue, maxValue);
  return static_cast<uint8_t>(result);
}
