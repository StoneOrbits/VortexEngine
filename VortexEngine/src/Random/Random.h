#pragma once

#include <stdint.h>

class Random 
{
public:
  Random();
  Random(uint32_t newseed);
  ~Random();

  void seed(uint32_t newseed);

  uint32_t next(uint32_t minValue = 0, uint32_t maxValue = 0xFFFFFFFF);
  uint8_t next8(uint8_t minValue = 0, uint8_t maxValue = 0xFF);
  uint16_t next16(uint16_t minValue = 0, uint16_t maxValue = 0xFFFF);

private:
  uint32_t m_seed;
};
