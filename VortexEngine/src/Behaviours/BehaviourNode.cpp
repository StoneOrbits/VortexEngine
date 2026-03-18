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

NodeAccelMotion::NodeAccelMotion() : BehaviourNode(Behaviours::NODE_ACCEL_MOTION) {}
float NodeAccelMotion::evaluate() { return Accelerometer::motion(); }

NodeAccelNormalized::NodeAccelNormalized() : BehaviourNode(Behaviours::NODE_ACCEL_NORMALIZED) {}
float NodeAccelNormalized::evaluate() { return Accelerometer::normalized(); }

NodeAccelCurved::NodeAccelCurved() : BehaviourNode(Behaviours::NODE_ACCEL_CURVED) {}
float NodeAccelCurved::evaluate() { return Accelerometer::curved(); }

NodeAccelFiltered::NodeAccelFiltered() : BehaviourNode(Behaviours::NODE_ACCEL_FILTERED) {}
float NodeAccelFiltered::evaluate() { return Accelerometer::filtered(); }

NodeAccelDirX::NodeAccelDirX() : BehaviourNode(Behaviours::NODE_ACCEL_DIR_X) {}
float NodeAccelDirX::evaluate() { return Accelerometer::dirX(); }

NodeAccelDirY::NodeAccelDirY() : BehaviourNode(Behaviours::NODE_ACCEL_DIR_Y) {}
float NodeAccelDirY::evaluate() { return Accelerometer::dirY(); }

NodeAccelDirZ::NodeAccelDirZ() : BehaviourNode(Behaviours::NODE_ACCEL_DIR_Z) {}
float NodeAccelDirZ::evaluate() { return Accelerometer::dirZ(); }

NodeAccelPitch::NodeAccelPitch() : BehaviourNode(Behaviours::NODE_ACCEL_PITCH) {}
float NodeAccelPitch::evaluate() { return Accelerometer::pitch(); }

NodeAccelRoll::NodeAccelRoll() : BehaviourNode(Behaviours::NODE_ACCEL_ROLL) {}
float NodeAccelRoll::evaluate() { return Accelerometer::roll(); }

NodeAccelTilt::NodeAccelTilt() : BehaviourNode(Behaviours::NODE_ACCEL_TILT) {}
float NodeAccelTilt::evaluate() { return Accelerometer::tilt(); }

NodeAbs::NodeAbs() : BehaviourNode(Behaviours::NODE_ABS) {}
float NodeAbs::evaluate()
{
  if (!inputCount())
    return 0.0f;

  return fabsf(input(0)->evaluate());
}

NodeAdd::NodeAdd() : BehaviourNode(Behaviours::NODE_ADD) {}
float NodeAdd::evaluate()
{
  float v = 0.0f;

  for (uint8_t i = 0; i < inputCount(); i++)
    v += input(i)->evaluate();

  return v;
}

NodeMultiply::NodeMultiply() : BehaviourNode(Behaviours::NODE_MULTIPLY) {}
float NodeMultiply::evaluate()
{
  float v = 1.0f;

  for (uint8_t i = 0; i < inputCount(); i++)
    v *= input(i)->evaluate();

  return v;
}

NodeClamp::NodeClamp() : BehaviourNode(Behaviours::NODE_CLAMP) {}
float NodeClamp::evaluate()
{
  if (!inputCount())
    return 0.0f;

  float v = input(0)->evaluate();

  if (v < param1) v = param1;
  if (v > param2) v = param1;

  return v;
}

NodeCurve::NodeCurve() : BehaviourNode(Behaviours::NODE_CURVE) {}
float NodeCurve::evaluate()
{
  if (!inputCount())
    return 0.0f;

  float v = input(0)->evaluate();

  return powf(v, param1);
}

NodeThreshold::NodeThreshold() : BehaviourNode(Behaviours::NODE_THRESHOLD) {}
float NodeThreshold::evaluate()
{
  if (!inputCount())
    return 0.0f;

  float v = input(0)->evaluate();

  return v > param1 ? 1.0f : 0.0f;
}

/*
    NodeModeBlend
    ---------------------------------------------------------------------------

    Behaviour graph node that blends the currently rendered mode with the
    output of the second Mode.

    The node samples the current LED state, renders the "other mode", then
    blends the two LED buffers together according to a computed blend ratio.

    Inputs
    ------
    input(0) : float
        Primary blend control signal. Expected range is [0,1], but values are
        normalized.

    input(1) : float (optional)
        External multiplier/modulator for the final blend ratio.
        If absent, a value of 1.0 is assumed.

    Parameters
    ----------
    param1 : float
        Offset applied to the primary input signal.

    param2 : float
        Scale applied after offset. This effectively controls sensitivity
        of the blend input.

        normalized = (input0 - param1) * param2

    Processing Steps
    ----------------
    1. Evaluate the primary input signal.
    2. Apply offset and scaling using param1 and param2.
    3. Clamp the resulting value to the range [0,1].
    4. Optionally multiply by the second input if present.
    5. Capture the currently rendered LED buffer.
    6. Render the "other" mode and capture its LED buffer.
    7. Blend both buffers according to the computed ratio.
    8. Restore the blended LED result.

*/
NodeModeBlend::NodeModeBlend() :
  BehaviourNode(Behaviours::NODE_MODE_BLEND)
{
}

float NodeModeBlend::evaluate()
{
  if (inputCount() == 0) {
    return 0.0f;
  }

  /* ---------------------------------------------------------------------
     Evaluate inputs
     ------------------------------------------------------------------ */

  float blendInput = input(0)->evaluate();
  float multiplier = 1.0f;

  if (inputCount() > 1) {
    multiplier = input(1)->evaluate();
  }

  /* ---------------------------------------------------------------------
     Normalize the primary blend signal using node parameters
     ------------------------------------------------------------------ */

  float normalized = (blendInput - param1) * param2;

  if (normalized < 0.0f) normalized = 0.0f;
  if (normalized > 1.0f) normalized = 1.0f;

  float finalBlend = normalized * multiplier;

  if (finalBlend < 0.0f) finalBlend = 0.0f;
  if (finalBlend > 1.0f) finalBlend = 1.0f;

  /* ---------------------------------------------------------------------
     Capture current LED state
     ------------------------------------------------------------------ */

  LedStash currentLeds;
  Leds::stashAll(currentLeds);

  // clear before playing other mode
  Leds::clearAll();

  /* ---------------------------------------------------------------------
     Render the alternate mode and capture its LED state
     ------------------------------------------------------------------ */

  Mode &other = Behaviours::otherMode();
  other.play();

  LedStash otherLeds;
  Leds::stashAll(otherLeds);

  /* ---------------------------------------------------------------------
     Blend the two LED buffers
     ------------------------------------------------------------------ */

  LedStash blended;
  LedStash::blendStashes(blended, currentLeds, otherLeds, finalBlend);

  /* ---------------------------------------------------------------------
     Restore the blended LED result
     ------------------------------------------------------------------ */

  Leds::restoreAll(blended);

  return 1.0f;
}

/*
    NodeModeAdd
    ---------------------------------------------------------------------------

    Behaviour graph node that adds the output of the "other mode" on top of the
    currently rendered mode.

    Unlike NodeModeBlend, the original mode is never reduced. Instead, the
    second mode is faded in and combined with the existing LED result.

    Result:
        0.0  -> only the current mode
        1.0  -> current mode + full intensity of the other mode

    Inputs
    ------
    input(0) : float
        Primary fade control signal.

    input(1) : float (optional)
        External multiplier/modulator for the final intensity.

    Parameters
    ----------
    param1 : float
        Offset applied to the primary input.

    param2 : float
        Scale applied after offset.

        normalized = (input0 - param1) * param2

    Processing Steps
    ----------------
    1. Evaluate the primary input.
    2. Apply offset and scaling.
    3. Clamp to [0,1].
    4. Apply optional multiplier.
    5. Capture current LED buffer.
    6. Render the "other mode".
    7. Scale the other mode by the computed value.
    8. Add the scaled result onto the current LEDs.
*/

class NodeModeAdd : public BehaviourNode
{
public:

    NodeModeAdd();
    virtual float evaluate();
};

NodeModeAdd::NodeModeAdd() :
  BehaviourNode(Behaviours::NODE_MODE_ADD)
{
}

float NodeModeAdd::evaluate()
{
  if (inputCount() == 0) {
    return 0.0f;
  }

  float addInput = input(0)->evaluate();
  float multiplier = 1.0f;

  if (inputCount() > 1) {
    multiplier = input(1)->evaluate();
  }

  float normalized = (addInput - param1) * param2;

  if (normalized < 0.0f) normalized = 0.0f;
  if (normalized > 1.0f) normalized = 1.0f;

  float finalAmount = normalized * multiplier;

  if (finalAmount < 0.0f) finalAmount = 0.0f;
  if (finalAmount > 1.0f) finalAmount = 1.0f;

  // copy of current leds
  LedStash currentLeds;
  Leds::stashAll(currentLeds);

  // clear before playing other mode
  Leds::clearAll();

  // play the other mode
  Mode &other = Behaviours::otherMode();
  other.play();

  // copy of the other mode result
  LedStash otherLeds;
  Leds::stashAll(otherLeds);

  // fade the others down
  uint8_t fade = (uint8_t)((1.0f - finalAmount) * 255.0f);
  for (int i = 0; i < LED_COUNT; ++i) {
    otherLeds[i].adjustBrightness(fade);
  }

  // add it on top of the current
  LedStash result;
  LedStash::addStashes(result, currentLeds, otherLeds);

  // apply the behaviour
  Leds::restoreAll(result);

  return 1.0f;
}

/*
    NodeColorShift
    ---------------------------------------------------------------------------

    Behaviour graph node that shifts a specific color in the currently rendered
    LED output toward another color.

    The node scans the current LED buffer and replaces pixels that match a
    target color, blending them toward a destination color based on a computed
    shift amount.

    No additional modes are rendered. This node only modifies the already
    rendered LED result.

    Inputs
    ------
    input(0) : float
        Primary shift control signal.

    input(1) : float (optional)
        External multiplier/modulator.

    Parameters
    ----------
    param1 : float
        Threshold offset applied to the input signal.

    param2 : float
        Scale applied after offset.

        normalized = (input0 - param1) * param2

    Additional Color Parameters
    ---------------------------
    colorA : Color
        Source color to detect.

    colorB : Color
        Destination color to shift toward.

    Processing Steps
    ----------------
    1. Evaluate control input.
    2. Normalize using param1/param2.
    3. Clamp result to [0,1].
    4. Capture current LED buffer.
    5. For each LED:
           If it matches colorA
           Blend it toward colorB using the computed value.
    6. Restore modified LEDs.
*/

class NodeColorShift : public BehaviourNode
{
public:

    NodeColorShift();
    virtual float evaluate();

    RGBColor colorA;
    RGBColor colorB;
};

NodeColorShift::NodeColorShift() :
  BehaviourNode(Behaviours::NODE_COLOR_SHIFT)
{
}

float NodeColorShift::evaluate()
{
  if (inputCount() == 0) {
    return 0.0f;
  }

  float shiftInput = input(0)->evaluate();
  float multiplier = 1.0f;

  if (inputCount() > 1) {
    multiplier = input(1)->evaluate();
  }

  float normalized = (shiftInput - param1) * param2;

  if (normalized < 0.0f) normalized = 0.0f;
  if (normalized > 1.0f) normalized = 1.0f;

  float finalShift = normalized * multiplier;

  if (finalShift < 0.0f) finalShift = 0.0f;
  if (finalShift > 1.0f) finalShift = 1.0f;

  LedStash leds;
  Leds::stashAll(leds);

  for (uint32_t i = 0; i < LED_COUNT; i++) {
    RGBColor &c = leds[i];

    if (c == colorA) {
      RGBColor shifted;

      shifted.red = c.red + (colorB.red - c.red) * finalShift;
      shifted.green = c.green + (colorB.green - c.green) * finalShift;
      shifted.blue = c.blue + (colorB.blue - c.blue) * finalShift;

      c = shifted;
    }
  }

  Leds::restoreAll(leds);

  return 1.0f;
}
