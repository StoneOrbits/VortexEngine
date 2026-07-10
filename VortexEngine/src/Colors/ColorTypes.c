#include "ColorTypes.h"

#include "../Serial/ByteStream.h"

hsv_to_rgb_algorithm g_hsv_rgb_alg = HSV_TO_RGB_GENERIC;

void HSVColor_init(HSVColor *self) {
  self->hue = 0; self->sat = 0; self->val = 0;
}

void HSVColor_initHSV(HSVColor *self, uint8_t hue, uint8_t sat, uint8_t val) {
  self->hue = hue; self->sat = sat; self->val = val;
}

void HSVColor_initFromU32(HSVColor *self, uint32_t dwVal) {
  HSVColor_init(self);
  HSVColor_assignU32(self, &dwVal);
}

void HSVColor_assignU32(HSVColor *self, const uint32_t *rhs) {
  if (!(*rhs & HSV_BIT)) {
    RGBColor tmp;
    RGBColor_initFromU32(&tmp, *rhs);
    HSVColor_assignRGB(self, &tmp);
    return;
  }
  self->hue = ((*rhs >> 16) & 0xFF);
  self->sat = ((*rhs >> 8) & 0xFF);
  self->val = (*rhs & 0xFF);
}

void HSVColor_copy(HSVColor *self, const HSVColor *rhs) {
  self->hue = rhs->hue;
  self->sat = rhs->sat;
  self->val = rhs->val;
}

void HSVColor_initFromRGB(HSVColor *self, const RGBColor *rhs) {
  HSVColor_assignRGB(self, rhs);
}

void HSVColor_assignRGB(HSVColor *self, const RGBColor *rhs) {
#if HSV_TO_RGB_ALGORITHM == 1
  HSVColor result = rgb_to_hsv_approx(rhs);
  HSVColor_copy(self, &result);
#else
  HSVColor result = rgb_to_hsv_generic(rhs);
  HSVColor_copy(self, &result);
#endif
}

bool HSVColor_equals(const HSVColor *self, const HSVColor *other) {
  return (HSVColor_raw(other) == HSVColor_raw(self));
}

bool HSVColor_notEquals(const HSVColor *self, const HSVColor *other) {
  return (HSVColor_raw(other) != HSVColor_raw(self));
}

bool HSVColor_empty(const HSVColor *self) {
  return !self->hue && !self->sat && !self->val;
}

void HSVColor_clear(HSVColor *self) {
  self->hue = 0; self->sat = 0; self->val = 0;
}

uint32_t HSVColor_raw(const HSVColor *self) {
  return HSV_BIT | ((uint32_t)self->hue << 16) | ((uint32_t)self->sat << 8) | (uint32_t)self->val;
}

void RGBColor_init(RGBColor *self) {
  self->red = 0; self->green = 0; self->blue = 0;
}

void RGBColor_initRGB(RGBColor *self, uint8_t red, uint8_t green, uint8_t blue) {
  self->red = red; self->green = green; self->blue = blue;
}

void RGBColor_initFromU32(RGBColor *self, uint32_t dwVal) {
  RGBColor_init(self);
  RGBColor_assignU32(self, &dwVal);
}

void RGBColor_assignU32(RGBColor *self, const uint32_t *rhs) {
  if ((*rhs & HSV_BIT) != 0) {
    HSVColor hsv;
    HSVColor_initFromU32(&hsv, *rhs);
    RGBColor_assignHSV(self, &hsv);
    return;
  }
  self->red = ((*rhs >> 16) & 0xFF);
  self->green = ((*rhs >> 8) & 0xFF);
  self->blue = (*rhs & 0xFF);
}

void RGBColor_copy(RGBColor *self, const RGBColor *rhs) {
  self->red = rhs->red;
  self->green = rhs->green;
  self->blue = rhs->blue;
}

void RGBColor_initFromHSV(RGBColor *self, const HSVColor *rhs) {
  RGBColor_assignHSV(self, rhs);
}

void RGBColor_assignHSV(RGBColor *self, const HSVColor *rhs) {
  switch (g_hsv_rgb_alg) {
  case HSV_TO_RGB_RAINBOW: {
    RGBColor result = hsv_to_rgb_rainbow(rhs);
    RGBColor_copy(self, &result);
    break;
  }
  case HSV_TO_RGB_RAW: {
    RGBColor result = hsv_to_rgb_raw_C(rhs);
    RGBColor_copy(self, &result);
    break;
  }
  case HSV_TO_RGB_GENERIC: {
    RGBColor result = hsv_to_rgb_generic(rhs);
    RGBColor_copy(self, &result);
    break;
  }
  }
}

bool RGBColor_equals(const RGBColor *self, const RGBColor *other) {
  return (RGBColor_raw(other) == RGBColor_raw(self));
}

bool RGBColor_notEquals(const RGBColor *self, const RGBColor *other) {
  return (RGBColor_raw(other) != RGBColor_raw(self));
}

bool RGBColor_empty(const RGBColor *self) {
  return !self->red && !self->green && !self->blue;
}

void RGBColor_clear(RGBColor *self) {
  self->red = 0; self->green = 0; self->blue = 0;
}

void RGBColor_adjustBrightness(RGBColor *self, uint8_t fadeBy) {
  self->red = (((int)self->red) * (int)(256 - fadeBy)) >> 8;
  self->green = (((int)self->green) * (int)(256 - fadeBy)) >> 8;
  self->blue = (((int)self->blue) * (int)(256 - fadeBy)) >> 8;
}

uint32_t RGBColor_raw(const RGBColor *self) {
  return ((uint32_t)self->red << 16) | ((uint32_t)self->green << 8) | (uint32_t)self->blue;
}

bool RGBColor_serialize(const RGBColor *self, ByteStream *buffer) {
  if (!ByteStream_serialize8(buffer, self->red)) return false;
  if (!ByteStream_serialize8(buffer, self->green)) return false;
  if (!ByteStream_serialize8(buffer, self->blue)) return false;
  return true;
}

bool RGBColor_unserialize(RGBColor *self, ByteStream *buffer) {
  if (!ByteStream_unserialize8(buffer, &self->red)) return false;
  if (!ByteStream_unserialize8(buffer, &self->green)) return false;
  if (!ByteStream_unserialize8(buffer, &self->blue)) return false;
  return true;
}

#define SCALE8(i, scale)  (((uint16_t)i * (uint16_t)(scale)) >> 8)
#define FIXFRAC8(N,D) (((N)*256)/(D))

RGBColor hsv_to_rgb_rainbow(const HSVColor *rhs)
{
  RGBColor col;
  const uint8_t Y1 = 1;
  const uint8_t Y2 = 0;
  const uint8_t G2 = 0;
  const uint8_t Gscale = 185;

  uint8_t hue = rhs->hue;
  uint8_t sat = rhs->sat;
  uint8_t val = rhs->val;

  uint8_t offset = hue & 0x1F;

  uint8_t offset8 = offset;
  offset8 <<= 3;

  uint8_t third = SCALE8(offset8, (256 / 3));
  uint8_t r, g, b;
  if (!(hue & 0x80)) {
    if (!(hue & 0x40)) {
      if (!(hue & 0x20)) {
        r = 255 - third;
        g = third;
        b = 0;
      } else {
        if (Y1) {
          r = 171;
          g = 85 + third;
          b = 0;
        }
        if (Y2) {
          r = 170 + third;
          uint8_t twothirds = SCALE8(offset8, ((256 * 2) / 3));
          g = 85 + twothirds;
          b = 0;
        }
      }
    } else {
      if (!(hue & 0x20)) {
        if (Y1) {
          uint8_t twothirds = SCALE8(offset8, ((256 * 2) / 3));
          r = 171 - twothirds;
          g = 170 + third;
          b = 0;
        }
        if (Y2) {
          r = 255 - offset8;
          g = 255;
          b = 0;
        }
      } else {
        r = 0;
        g = 255 - third;
        b = third;
      }
    }
  } else {
    if (!(hue & 0x40)) {
      if (!(hue & 0x20)) {
        r = 0;
        uint8_t twothirds = SCALE8(offset8, ((256 * 2) / 3));
        g = 171 - twothirds;
        b = 85 + twothirds;
      } else {
        r = third;
        g = 0;
        b = 255 - third;
      }
    } else {
      if (!(hue & 0x20)) {
        r = 85 + third;
        g = 0;
        b = 171 - third;
      } else {
        r = 170 + third;
        g = 0;
        b = 85 - third;
      }
    }
  }

  if (G2) g = g >> 1;
  if (Gscale) g = SCALE8(g, Gscale);

  if (sat != 255) {
    if (sat == 0) {
      r = 255; b = 255; g = 255;
    } else {
      if (r) r = SCALE8(r, sat) + 1;
      if (g) g = SCALE8(g, sat) + 1;
      if (b) b = SCALE8(b, sat) + 1;

      uint8_t desat = 255 - sat;
      desat = SCALE8(desat, desat);

      uint8_t brightness_floor = desat;
      r += brightness_floor;
      g += brightness_floor;
      b += brightness_floor;
    }
  }

  if (val != 255) {
    val = SCALE8(val, val);
    if (val == 0) {
      r = 0; g = 0; b = 0;
    } else {
      if (r) r = SCALE8(r, val) + 1;
      if (g) g = SCALE8(g, val) + 1;
      if (b) b = SCALE8(b, val) + 1;
    }
  }

  col.red = r;
  col.green = g;
  col.blue = b;
  return col;
}

RGBColor hsv_to_rgb_raw_C(const HSVColor *rhs)
{
  RGBColor col;
  uint8_t value = rhs->val;
  uint8_t saturation = rhs->sat;

  uint8_t invsat = 255 - saturation;
  uint8_t brightness_floor = (value * invsat) / 256;

  uint8_t color_amplitude = value - brightness_floor;

  uint8_t section = rhs->hue / 0x40;
  uint8_t offset = rhs->hue % 0x40;

  uint8_t rampup = offset;
  uint8_t rampdown = (0x40 - 1) - offset;

  uint8_t rampup_amp_adj = (rampup * color_amplitude) / (256 / 4);
  uint8_t rampdown_amp_adj = (rampdown * color_amplitude) / (256 / 4);

  uint8_t rampup_adj_with_floor = rampup_amp_adj + brightness_floor;
  uint8_t rampdown_adj_with_floor = rampdown_amp_adj + brightness_floor;

  if (section) {
    if (section == 1) {
      col.red = brightness_floor;
      col.green = rampdown_adj_with_floor;
      col.blue = rampup_adj_with_floor;
    } else {
      col.red = rampup_adj_with_floor;
      col.green = brightness_floor;
      col.blue = rampdown_adj_with_floor;
    }
  } else {
    col.red = rampdown_adj_with_floor;
    col.green = rampup_adj_with_floor;
    col.blue = brightness_floor;
  }
  return col;
}

RGBColor hsv_to_rgb_generic(const HSVColor *rhs)
{
  unsigned char region, remainder, p, q, t;
  RGBColor col;

  if (rhs->sat == 0) {
    col.red = rhs->val;
    col.green = rhs->val;
    col.blue = rhs->val;
    return col;
  }

  region = rhs->hue / 43;
  remainder = ((rhs->hue - (region * 43)) * 6);

  p = (uint8_t)(((uint16_t)(rhs->val) * (255 - rhs->sat)) >> 8);
  q = (uint8_t)(((uint16_t)(rhs->val) * (255 - (((uint16_t)(rhs->sat) * remainder) >> 8))) >> 8);
  t = (uint8_t)(((uint16_t)(rhs->val) * (255 - (((uint16_t)(rhs->sat) * (255 - remainder)) >> 8))) >> 8);

  switch (region) {
  case 0:
    col.red = rhs->val; col.green = t; col.blue = p;
    break;
  case 1:
    col.red = q; col.green = rhs->val; col.blue = p;
    break;
  case 2:
    col.red = p; col.green = rhs->val; col.blue = t;
    break;
  case 3:
    col.red = p; col.green = q; col.blue = rhs->val;
    break;
  case 4:
    col.red = t; col.green = p; col.blue = rhs->val;
    break;
  default:
    col.red = rhs->val; col.green = p; col.blue = q;
    break;
  }
  return col;
}

static uint8_t sqrt16(uint16_t x)
{
  if (x <= 1) {
    return (uint8_t)x;
  }
  uint8_t low = 1;
  uint8_t mid = 0;
  uint8_t hi = (x > 7904) ? 255 : ((x >> 5) + 8);
  do {
    mid = (low + hi) >> 1;
    if ((uint16_t)(mid * mid) > x) {
      hi = mid - 1;
    } else {
      if (mid == 255) {
        return 255;
      }
      low = mid + 1;
    }
  } while (hi >= low);
  return low - 1;
}

static uint8_t qsub8(uint8_t i, uint8_t j)
{
  int t = i - j;
  if (t < 0) {
    return 0;
  }
  return t;
}

enum
{
  HUE_RED = 0,
  HUE_ORANGE = 32,
  HUE_YELLOW = 64,
  HUE_GREEN = 96,
  HUE_AQUA = 128,
  HUE_BLUE = 160,
  HUE_PURPLE = 192,
  HUE_PINK = 224
};

HSVColor rgb_to_hsv_approx(const RGBColor *rhs)
{
  uint8_t r = rhs->red;
  uint8_t g = rhs->green;
  uint8_t b = rhs->blue;
  uint8_t h, s, v;

  uint8_t desat = 255;
  if (r < desat) desat = r;
  if (g < desat) desat = g;
  if (b < desat) desat = b;

  r -= desat;
  g -= desat;
  b -= desat;

  s = 255 - desat;
  if (s != 255) {
    s = 255 - sqrt16((255 - s) * 256);
  }

  if ((r + g + b) == 0) {
    HSVColor result;
    HSVColor_initHSV(&result, 0, 0, 255 - s);
    return result;
  }

  if (s < 255) {
    if (s == 0) s = 1;
    uint32_t scaleup = 65535 / (s);
    r = ((uint32_t)(r)*scaleup) / 256;
    g = ((uint32_t)(g)*scaleup) / 256;
    b = ((uint32_t)(b)*scaleup) / 256;
  }

  uint16_t total = r + g + b;

  if (total < 255) {
    if (total == 0) total = 1;
    uint32_t scaleup = 65535 / (total);
    r = ((uint32_t)(r)*scaleup) / 256;
    g = ((uint32_t)(g)*scaleup) / 256;
    b = ((uint32_t)(b)*scaleup) / 256;
  }

  if (total > 255) {
    v = 255;
  } else {
    if (((uint32_t)desat + (uint32_t)total) <= 255) {
      v = desat + total;
    } else {
      v = 255;
    }
    if (v != 255) v = sqrt16(v * 256);
  }

  uint8_t highest = r;
  if (g > highest) highest = g;
  if (b > highest) highest = b;

  if (highest == r) {
    if (g == 0) {
      h = (HUE_PURPLE + HUE_PINK) / 2;
      h += SCALE8(qsub8(r, 128), FIXFRAC8(48, 128));
    } else if ((r - g) > g) {
      h = HUE_RED;
      h += SCALE8(g, FIXFRAC8(32, 85));
    } else {
      h = HUE_ORANGE;
      h += SCALE8(qsub8((g - 85) + (171 - r), 4), FIXFRAC8(32, 85));
    }
  } else if (highest == g) {
    if (b == 0) {
      h = HUE_YELLOW;
      uint8_t radj = SCALE8(qsub8(171, r), 47);
      uint8_t gadj = SCALE8(qsub8(g, 171), 96);
      uint8_t rgadj = radj + gadj;
      uint8_t hueadv = rgadj / 2;
      h += hueadv;
    } else {
      if ((g - b) > b) {
        h = HUE_GREEN;
        h += SCALE8(b, FIXFRAC8(32, 85));
      } else {
        h = HUE_AQUA;
        h += SCALE8(qsub8(b, 85), FIXFRAC8(8, 42));
      }
    }
  } else {
    if (r == 0) {
      h = HUE_AQUA + ((HUE_BLUE - HUE_AQUA) / 4);
      h += SCALE8(qsub8(b, 128), FIXFRAC8(24, 128));
    } else if ((b - r) > r) {
      h = HUE_BLUE;
      h += SCALE8(r, FIXFRAC8(32, 85));
    } else {
      h = HUE_PURPLE;
      h += SCALE8(qsub8(r, 85), FIXFRAC8(32, 85));
    }
  }
  h += 1;

  HSVColor result;
  HSVColor_initHSV(&result, h, s, v);
  return result;
}

HSVColor rgb_to_hsv_generic(const RGBColor *rhs)
{
  unsigned char rgbMin, rgbMax;
  rgbMin = rhs->red < rhs->green ? (rhs->red < rhs->blue ? rhs->red : rhs->blue) : (rhs->green < rhs->blue ? rhs->green : rhs->blue);
  rgbMax = rhs->red > rhs->green ? (rhs->red > rhs->blue ? rhs->red : rhs->blue) : (rhs->green > rhs->blue ? rhs->green : rhs->blue);

  HSVColor hsv;
  HSVColor_init(&hsv);

  hsv.val = rgbMax;
  if (hsv.val == 0) {
    hsv.hue = 0;
    hsv.sat = 0;
    return hsv;
  }

  hsv.sat = 255 * (long)(rgbMax - rgbMin) / hsv.val;
  if (hsv.sat == 0) {
    hsv.hue = 0;
    return hsv;
  }

  if (rgbMax == rhs->red) {
    hsv.hue = 0 + 43 * (rhs->green - rhs->blue) / (rgbMax - rgbMin);
  } else if (rgbMax == rhs->green) {
    hsv.hue = 85 + 43 * (rhs->blue - rhs->red) / (rgbMax - rgbMin);
  } else {
    hsv.hue = 171 + 43 * (rhs->red - rhs->green) / (rgbMax - rgbMin);
  }
  return hsv;
}
