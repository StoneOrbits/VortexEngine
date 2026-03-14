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
