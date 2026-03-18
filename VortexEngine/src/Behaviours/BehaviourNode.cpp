#include "BehaviourNode.h"
#include "Behaviours.h"

#include "../Sensor/Accelerometer.h"
#include "../Time/TimeControl.h"
#include "../Leds/LedStash.h"
#include "../Modes/Modes.h"
#include "../Modes/Mode.h"

#include <math.h>

BehaviourNode::BehaviourNode(Behaviours::NodeType type)
{
  m_type = type;
  m_inputCount = 0;
  m_cached = 0.0f;
  param1 = 0.0f;
  param2 = 1.0f;

  for (uint8_t i = 0; i < MAX_BEHAVIOUR_INPUTS; i++)
    m_inputs[i] = nullptr;
}

BehaviourNode::~BehaviourNode()
{
}

void BehaviourNode::addInput(BehaviourNode *n)
{
  if (m_inputCount < MAX_BEHAVIOUR_INPUTS)
    m_inputs[m_inputCount++] = n;
}

BehaviourNode *BehaviourNode::input(uint8_t i) const
{
  if (i >= m_inputCount)
    return nullptr;
  return m_inputs[i];
}

uint8_t BehaviourNode::inputCount() const
{
  return m_inputCount;
}

Behaviours::NodeType BehaviourNode::type() const
{
  return m_type;
}

/* ================= INPUT NODES ================= */

NodeAccelMotion::NodeAccelMotion() : BehaviourNode(Behaviours::NODE_INPUT_ACCEL_MOTION) {}
float NodeAccelMotion::evaluate() { return Accelerometer::motion(); }

NodeAccelNormalized::NodeAccelNormalized() : BehaviourNode(Behaviours::NODE_INPUT_ACCEL_NORMALIZED) {}
float NodeAccelNormalized::evaluate() { return Accelerometer::normalized(); }

NodeAccelCurved::NodeAccelCurved() : BehaviourNode(Behaviours::NODE_INPUT_ACCEL_CURVED) {}
float NodeAccelCurved::evaluate() { return Accelerometer::curved(); }

NodeAccelFiltered::NodeAccelFiltered() : BehaviourNode(Behaviours::NODE_INPUT_ACCEL_FILTERED) {}
float NodeAccelFiltered::evaluate() { return Accelerometer::filtered(); }

NodeAccelDirX::NodeAccelDirX() : BehaviourNode(Behaviours::NODE_INPUT_ACCEL_DIR_X) {}
float NodeAccelDirX::evaluate() { return Accelerometer::dirX(); }

NodeAccelDirY::NodeAccelDirY() : BehaviourNode(Behaviours::NODE_INPUT_ACCEL_DIR_Y) {}
float NodeAccelDirY::evaluate() { return Accelerometer::dirY(); }

NodeAccelDirZ::NodeAccelDirZ() : BehaviourNode(Behaviours::NODE_INPUT_ACCEL_DIR_Z) {}
float NodeAccelDirZ::evaluate() { return Accelerometer::dirZ(); }

NodeAccelPitch::NodeAccelPitch() : BehaviourNode(Behaviours::NODE_INPUT_ACCEL_PITCH) {}
float NodeAccelPitch::evaluate() { return Accelerometer::pitch(); }

NodeAccelRoll::NodeAccelRoll() : BehaviourNode(Behaviours::NODE_INPUT_ACCEL_ROLL) {}
float NodeAccelRoll::evaluate() { return Accelerometer::roll(); }

NodeAccelTilt::NodeAccelTilt() : BehaviourNode(Behaviours::NODE_INPUT_ACCEL_TILT) {}
float NodeAccelTilt::evaluate() { return Accelerometer::tilt(); }

/* ================= MODIFIERS ================= */

NodeAbs::NodeAbs() : BehaviourNode(Behaviours::NODE_MODIFIER_ABS) {}
float NodeAbs::evaluate()
{
  if (!inputCount()) return 0.0f;
  return fabsf(input(0)->evaluate());
}

NodeAdd::NodeAdd() : BehaviourNode(Behaviours::NODE_MODIFIER_ADD) {}
float NodeAdd::evaluate()
{
  float v = 0.0f;
  for (uint8_t i = 0; i < inputCount(); i++)
    v += input(i)->evaluate();
  return v;
}

NodeMultiply::NodeMultiply() : BehaviourNode(Behaviours::NODE_MODIFIER_MULTIPLY) {}
float NodeMultiply::evaluate()
{
  float v = 1.0f;
  for (uint8_t i = 0; i < inputCount(); i++)
    v *= input(i)->evaluate();
  return v;
}

NodeClamp::NodeClamp() : BehaviourNode(Behaviours::NODE_MODIFIER_CLAMP) {}
float NodeClamp::evaluate()
{
  if (!inputCount()) return 0.0f;

  float v = input(0)->evaluate();

  if (v < param1) v = param1;
  if (v > param2) v = param2;

  return v;
}

NodeCurve::NodeCurve() : BehaviourNode(Behaviours::NODE_MODIFIER_CURVE) {}
float NodeCurve::evaluate()
{
  if (!inputCount()) return 0.0f;

  float v = input(0)->evaluate();

  if (v < 0.0f) v = 0.0f;
  if (v > 1.0f) v = 1.0f;

  return powf(v, param1 <= 0.0f ? 1.0f : param1);
}

NodeThreshold::NodeThreshold() : BehaviourNode(Behaviours::NODE_MODIFIER_THRESHOLD) {}
float NodeThreshold::evaluate()
{
  if (!inputCount()) return 0.0f;
  float v = input(0)->evaluate();
  return v > param1 ? 1.0f : 0.0f;
}

/* ================= FUNCTIONAL ================= */

NodeModeBlend::NodeModeBlend() :
  BehaviourNode(Behaviours::NODE_FUNCTIONAL_MODE_BLEND)
{
}

float NodeModeBlend::evaluate()
{
  if (inputCount() == 0) return 0.0f;

  float blendInput = input(0)->evaluate();
  float multiplier = inputCount() > 1 ? input(1)->evaluate() : 1.0f;

  float normalized = (blendInput - param1) * param2;

  if (normalized < 0.0f) normalized = 0.0f;
  if (normalized > 1.0f) normalized = 1.0f;

  float finalBlend = normalized * multiplier;

  if (finalBlend < 0.0f) finalBlend = 0.0f;
  if (finalBlend > 1.0f) finalBlend = 1.0f;

  LedStash currentLeds;
  Leds::stashAll(currentLeds);

  Leds::clearAll();

  Mode &other = Behaviours::otherMode();
  other.play();

  LedStash otherLeds;
  Leds::stashAll(otherLeds);

  LedStash blended;
  LedStash::blendStashes(blended, currentLeds, otherLeds, finalBlend);

  Leds::restoreAll(blended);

  return 1.0f;
}

/* ===== INPUT ===== */

NodeTime::NodeTime() : BehaviourNode(Behaviours::NODE_INPUT_TIME) {}
float NodeTime::evaluate() { return Time::getCurtime(); }

NodeDeltaTime::NodeDeltaTime() : BehaviourNode(Behaviours::NODE_INPUT_DELTA_TIME) {}
float NodeDeltaTime::evaluate() { return Time::getCurtime(); }

NodeRandom::NodeRandom() : BehaviourNode(Behaviours::NODE_INPUT_RANDOM) {}
float NodeRandom::evaluate() { return (float)m_rand.next16() / (float)UINT16_MAX; }

NodeConstant::NodeConstant() : BehaviourNode(Behaviours::NODE_INPUT_CONSTANT) {}
float NodeConstant::evaluate() { return param1; }

/* ===== MODIFIER ===== */

NodeSubtract::NodeSubtract() : BehaviourNode(Behaviours::NODE_MODIFIER_SUBTRACT) {}
float NodeSubtract::evaluate()
{
  if (inputCount() < 2) return 0.0f;
  return input(0)->evaluate() - input(1)->evaluate();
}

NodeDivide::NodeDivide() : BehaviourNode(Behaviours::NODE_MODIFIER_DIVIDE) {}
float NodeDivide::evaluate()
{
  if (inputCount() < 2) return 0.0f;
  float b = input(1)->evaluate();
  if (b == 0.0f) return 0.0f;
  return input(0)->evaluate() / b;
}

NodeMin::NodeMin() : BehaviourNode(Behaviours::NODE_MODIFIER_MIN) {}
float NodeMin::evaluate()
{
  if (inputCount() < 2) return 0.0f;
  float a = input(0)->evaluate();
  float b = input(1)->evaluate();
  return a < b ? a : b;
}

NodeMax::NodeMax() : BehaviourNode(Behaviours::NODE_MODIFIER_MAX) {}
float NodeMax::evaluate()
{
  if (inputCount() < 2) return 0.0f;
  float a = input(0)->evaluate();
  float b = input(1)->evaluate();
  return a > b ? a : b;
}

NodeRemap::NodeRemap() : BehaviourNode(Behaviours::NODE_MODIFIER_REMAP) {}
float NodeRemap::evaluate()
{
  if (inputCount() < 5) return 0.0f;

  float x = input(0)->evaluate();
  float inMin = input(1)->evaluate();
  float inMax = input(2)->evaluate();
  float outMin = input(3)->evaluate();
  float outMax = input(4)->evaluate();

  if (inMax - inMin == 0.0f) return outMin;

  float t = (x - inMin) / (inMax - inMin);
  return outMin + t * (outMax - outMin);
}

NodeSmoothstep::NodeSmoothstep() : BehaviourNode(Behaviours::NODE_MODIFIER_SMOOTHSTEP) {}
float NodeSmoothstep::evaluate()
{
  if (!inputCount()) return 0.0f;

  float x = input(0)->evaluate();
  float edge0 = param1;
  float edge1 = param2;

  if (edge1 - edge0 == 0.0f) return 0.0f;

  float t = (x - edge0) / (edge1 - edge0);
  if (t < 0.0f) t = 0.0f;
  if (t > 1.0f) t = 1.0f;

  return t * t * (3.0f - 2.0f * t);
}

NodeSin::NodeSin() : BehaviourNode(Behaviours::NODE_MODIFIER_SIN) {}
float NodeSin::evaluate()
{
  if (!inputCount()) return 0.0f;
  return sinf(input(0)->evaluate());
}

NodeCos::NodeCos() : BehaviourNode(Behaviours::NODE_MODIFIER_COS) {}
float NodeCos::evaluate()
{
  if (!inputCount()) return 0.0f;
  return cosf(input(0)->evaluate());
}

NodeGreater::NodeGreater() : BehaviourNode(Behaviours::NODE_MODIFIER_GREATER) {}
float NodeGreater::evaluate()
{
  if (inputCount() < 2) return 0.0f;
  return input(0)->evaluate() > input(1)->evaluate() ? 1.0f : 0.0f;
}

NodeLess::NodeLess() : BehaviourNode(Behaviours::NODE_MODIFIER_LESS) {}
float NodeLess::evaluate()
{
  if (inputCount() < 2) return 0.0f;
  return input(0)->evaluate() < input(1)->evaluate() ? 1.0f : 0.0f;
}

NodeLerp::NodeLerp() : BehaviourNode(Behaviours::NODE_MODIFIER_LERP) {}
float NodeLerp::evaluate()
{
  if (inputCount() < 3) return 0.0f;

  float a = input(0)->evaluate();
  float b = input(1)->evaluate();
  float t = input(2)->evaluate();

  if (t < 0.0f) t = 0.0f;
  if (t > 1.0f) t = 1.0f;

  return a + (b - a) * t;
}

NodeSelect::NodeSelect() : BehaviourNode(Behaviours::NODE_MODIFIER_SELECT) {}
float NodeSelect::evaluate()
{
  if (inputCount() < 3) return 0.0f;

  float cond = input(0)->evaluate();
  return cond > 0.5f ? input(1)->evaluate() : input(2)->evaluate();
}

NodeModeAdd::NodeModeAdd() :
  BehaviourNode(Behaviours::NODE_FUNCTIONAL_MODE_ADD)
{
}

float NodeModeAdd::evaluate()
{
  if (inputCount() == 0) return 0.0f;

  float inputVal = input(0)->evaluate();
  float multiplier = inputCount() > 1 ? input(1)->evaluate() : 1.0f;

  float normalized = (inputVal - param1) * param2;

  if (normalized < 0.0f) normalized = 0.0f;
  if (normalized > 1.0f) normalized = 1.0f;

  float finalAmount = normalized * multiplier;

  if (finalAmount < 0.0f) finalAmount = 0.0f;
  if (finalAmount > 1.0f) finalAmount = 1.0f;

  LedStash currentLeds;
  Leds::stashAll(currentLeds);

  Leds::clearAll();

  Mode &other = Behaviours::otherMode();
  other.play();

  LedStash otherLeds;
  Leds::stashAll(otherLeds);

  LedStash result;

  for (int i = 0; i < LED_COUNT; ++i) {
    RGBColor &a = currentLeds[i];
    RGBColor &b = otherLeds[i];

    RGBColor r;

    float f = finalAmount;

    r.red = (uint8_t)((a.red * (b.red / 255.0f)) * f + a.red * (1.0f - f));
    r.green = (uint8_t)((a.green * (b.green / 255.0f)) * f + a.green * (1.0f - f));
    r.blue = (uint8_t)((a.blue * (b.blue / 255.0f)) * f + a.blue * (1.0f - f));

    result[i] = r;
  }

  Leds::restoreAll(result);

  return 1.0f;
}

/* ================= BRIGHTNESS SHIFT ================= */

NodeBrightnessShift::NodeBrightnessShift() :
  BehaviourNode(Behaviours::NODE_FUNCTIONAL_BRIGHTNESS_SHIFT)
{
}

float NodeBrightnessShift::evaluate()
{
  if (inputCount() == 0) return 0.0f;

  float inputVal = input(0)->evaluate();
  float multiplier = inputCount() > 1 ? input(1)->evaluate() : 1.0f;

  float normalized = (inputVal - param1) * param2;

  if (normalized < 0.0f) normalized = 0.0f;
  if (normalized > 1.0f) normalized = 1.0f;

  float finalAmount = normalized * multiplier;

  if (finalAmount < 0.0f) finalAmount = 0.0f;
  if (finalAmount > 1.0f) finalAmount = 1.0f;

  LedStash leds;
  Leds::stashAll(leds);

  uint8_t scale = (uint8_t)(finalAmount * 255.0f);

  for (int i = 0; i < LED_COUNT; ++i) {
    leds[i].red = (uint8_t)((leds[i].red * scale) >> 8);
    leds[i].green = (uint8_t)((leds[i].green * scale) >> 8);
    leds[i].blue = (uint8_t)((leds[i].blue * scale) >> 8);
  }

  Leds::restoreAll(leds);

  return 1.0f;
}

NodeColorShift::NodeColorShift() :
  BehaviourNode(Behaviours::NODE_FUNCTIONAL_COLOR_SHIFT)
{
}

float NodeColorShift::evaluate()
{
  if (inputCount() == 0) return 0.0f;

  float inputVal = input(0)->evaluate();
  float multiplier = inputCount() > 1 ? input(1)->evaluate() : 1.0f;

  float normalized = (inputVal - param1) * param2;

  if (normalized < 0.0f) normalized = 0.0f;
  if (normalized > 1.0f) normalized = 1.0f;

  float finalAmount = normalized * multiplier;

  if (finalAmount < 0.0f) finalAmount = 0.0f;
  if (finalAmount > 1.0f) finalAmount = 1.0f;

  LedStash leds;
  Leds::stashAll(leds);

  uint8_t scale = (uint8_t)(finalAmount * 255.0f);

  for (int i = 0; i < LED_COUNT; ++i) {
    leds[i].red = (uint8_t)((leds[i].red * scale) >> 8);
    leds[i].green = (uint8_t)((leds[i].green * scale) >> 8);
    leds[i].blue = (uint8_t)((leds[i].blue * scale) >> 8);
  }

  Leds::restoreAll(leds);

  return 1.0f;
}

/* ================= PATTERN SHIFT ================= */

NodePatternShift::NodePatternShift() :
  BehaviourNode(Behaviours::NODE_FUNCTIONAL_PATTERN_SHIFT)
{
}

float NodePatternShift::evaluate()
{
  if (inputCount() == 0) return 0.0f;

  float inputVal = input(0)->evaluate();
  float multiplier = inputCount() > 1 ? input(1)->evaluate() : 1.0f;

  float normalized = (inputVal - param1) * param2;

  if (normalized < 0.0f) normalized = 0.0f;
  if (normalized > 1.0f) normalized = 1.0f;

  float finalAmount = normalized * multiplier;

  if (finalAmount < 0.0f) finalAmount = 0.0f;
  if (finalAmount > 1.0f) finalAmount = 1.0f;

  LedStash leds;
  Leds::stashAll(leds);

  int shift = (int)(finalAmount * LED_COUNT);

  if (shift == 0) return 1.0f;

  LedStash shifted;

  for (int i = 0; i < LED_COUNT; ++i) {
    int src = i - shift;

    while (src < 0) src += LED_COUNT;
    while (src >= LED_COUNT) src -= LED_COUNT;

    shifted[i] = leds[src];
  }

  Leds::restoreAll(shifted);

  return 1.0f;
}