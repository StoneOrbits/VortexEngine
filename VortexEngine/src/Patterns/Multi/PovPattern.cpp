#include "PovPattern.h"
#include "PovTextures.h"

#include "../../Time/TimeControl.h"
#include "../../Leds/Leds.h"
#include "../../Log/Log.h"

#include <string.h>

static_assert(NUM_POV_TEXTURES == (PATTERN_POV_LAST - PATTERN_POV_FIRST) + 1,
  "The number of POV textures must match the PATTERN_POV_ ids in Patterns.h");

PovPattern::PovPattern(const PatternArgs &args) :
  MultiLedPattern(args),
  m_columnOn(0),
  m_columnOff(0),
  m_loopGap(0),
  m_texture(nullptr),
  m_width(0),
  m_numRoles(1),
  m_startTime(0)
{
  // the pattern id is assigned by the PatternBuilder after construction,
  // the texture is picked from it in init()
  REGISTER_ARG(m_columnOn);
  REGISTER_ARG(m_columnOff);
  REGISTER_ARG(m_loopGap);
  setArgs(args);
}

PovPattern::~PovPattern()
{
}

void PovPattern::init()
{
  MultiLedPattern::init();
  // pick the texture for this pattern id
  uint32_t index = (uint32_t)(m_patternID - PATTERN_POV_FIRST);
  m_texture = (index < NUM_POV_TEXTURES) ? &povTextures[index] : nullptr;
  m_width = m_texture ? (uint8_t)strlen(m_texture->rows[0]) : 0;
  // count the color roles, '*' is the same as '1'
  m_numRoles = 1;
  for (uint8_t row = 0; m_texture && row < POV_TEXTURE_HEIGHT; ++row) {
    for (const char *p = m_texture->rows[row]; *p; ++p) {
      if (*p >= '1' && *p <= '8' && (uint8_t)(*p - '0') > m_numRoles) {
        m_numRoles = (uint8_t)(*p - '0');
      }
    }
  }
  m_startTime = Time::getCurtime();
}

void PovPattern::play()
{
  if (!m_texture || !m_width) {
    Leds::clearAll();
    return;
  }
  // the engine shows exactly one frame per tick so these are counted in
  // ticks (frames) rather than milliseconds
  uint32_t onTicks = m_columnOn ? m_columnOn : 1;
  uint32_t period = onTicks + m_columnOff;
  uint32_t elapsed = Time::getCurtime() - m_startTime;
  uint32_t step = elapsed / period;
  // one copy of the texture plus the blank gap columns
  uint32_t copyLength = (uint32_t)m_width + m_loopGap;
  uint32_t copy = step / copyLength;
  uint32_t column = step % copyLength;
  // dark between columns and during the gap columns
  if ((elapsed % period) >= onTicks || column >= m_width) {
    Leds::clearAll();
    return;
  }
  // row 0 of the texture is the tip ring, the last row is the center ring
  for (uint8_t row = 0; row < POV_TEXTURE_HEIGHT; ++row) {
    Leds::setRing((Ring)(RING_LAST - row), pixelColor(m_texture->rows[row][column], copy, step));
  }
}

uint8_t PovPattern::defaultGap(PatternID id)
{
  uint32_t index = (uint32_t)(id - PATTERN_POV_FIRST);
  return (index < NUM_POV_TEXTURES) ? povTextures[index].gap : 0;
}

RGBColor PovPattern::pixelColor(char pixel, uint32_t copy, uint32_t step)
{
  uint8_t numColors = m_colorset.numColors();
  if (!numColors) {
    return RGB_OFF;
  }
  if (pixel == '0') {
    // trim, the steady first color that frames the texture with a fast
    // blink, timed by frames so it is even across copies of the texture
    uint32_t blink = (uint32_t)m_texture->trimOn + m_texture->trimOff;
    if (blink && m_texture->trimOn && (step % blink) >= m_texture->trimOn) {
      return RGB_OFF;
    }
    return m_colorset.get(0);
  }
  uint32_t role;
  if (pixel == '*') {
    role = 0;
  } else if (pixel >= '1' && pixel <= '8') {
    role = (uint32_t)(pixel - '1');
  } else {
    return RGB_OFF;
  }
  // each copy of the texture moves on to the next set of colors
  RGBColor col = m_colorset.get((uint8_t)((role + (copy * m_numRoles)) % numColors));
  // each role can have its own brightness so the sections stand apart
  uint32_t level = m_texture->levels[role];
  if (level) {
    col.red = (uint8_t)((col.red * level) / 255);
    col.green = (uint8_t)((col.green * level) / 255);
    col.blue = (uint8_t)((col.blue * level) / 255);
  }
  return col;
}
