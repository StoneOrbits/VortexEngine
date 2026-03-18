#include "Behaviours.h"
#include "BehaviourNode.h"

#include "../VortexEngine.h"
#include "../Modes/Modes.h"
#include "../Storage/Storage.h"

#include <string.h>

BehaviourNode *Behaviours::m_nodes[MAX_BEHAVIOUR_NODES];
uint8_t Behaviours::m_nodeCount = 0;
Mode Behaviours::m_otherMode;

bool Behaviours::init()
{
  memset(m_nodes, 0, sizeof(m_nodes));

  //// create some default behaviour
  //uint8_t blendNode = create(NODE_MODE_BLEND,
  //  0.0f,   // threshold
  //  0.25f   // ramp
  //);
  //uint8_t filteredNode = create(NODE_INPUT_ACCEL_FILTERED);
  //connect(filteredNode, blendNode);

  //ByteStream test;
  //serialize(test);
  //unserialize(test);

  return true;
}

void Behaviours::cleanup()
{
  clear();
}

void Behaviours::update()
{
  for (uint8_t i = 0; i < m_nodeCount; i++) {
    auto &node = m_nodes[i];
    if (node && isFunctional(node->type())) {
      node->evaluate();
    }
  }
}

bool Behaviours::serialize(ByteStream &stream)
{
  stream.clear();

  // serialize the engine version into the modes buffer
  VortexEngine::serializeVersion(stream);

  if (!stream.serialize8(m_nodeCount)) {
    return false;
  }

  for (uint8_t i = 0; i < m_nodeCount; i++) {
    BehaviourNode *n = m_nodes[i];
    if (!stream.serialize8((uint8_t)n->type())) {
      return false;
    }
    if (!stream.serialize32(*(uint32_t *)&n->param1)) {
      return false;
    }
    if (!stream.serialize32(*(uint32_t *)&n->param2)) {
      return false;
    }
    if (!stream.serialize8(n->m_inputCount)) {
      return false;
    }
    for (uint8_t j = 0; j < n->m_inputCount; j++) {
      uint8_t index = 255;
      for (uint8_t k = 0; k < m_nodeCount; k++) {
        if (m_nodes[k] == n->m_inputs[j]) {
          index = k;
          break;
        }
      }
      if (!stream.serialize8(index)) {
        return false;
      }
    }
  }

  return true;
}

bool Behaviours::unserialize(ByteStream &stream)
{
  clear();

  uint8_t major = 0;
  uint8_t minor = 0;
  // unserialize the vortex version
  if (!stream.unserialize8(&major)) {
    return false;
  }
  if (!stream.unserialize8(&minor)) {
    return false;
  }
  // check the version for incompatibility
  if (!VortexEngine::checkVersion(major, minor)) {
    // incompatible version
    ERROR_LOGF("Incompatible savefile version: %u.%u", major, minor);
    return false;
  }

  uint8_t count;
  if (!stream.unserialize8(&count)) {
    return false;
  }
  if (count > MAX_BEHAVIOUR_NODES) {
    return false;
  }

  BehaviourNode *tempInputs[MAX_BEHAVIOUR_NODES][MAX_BEHAVIOUR_INPUTS];
  uint8_t tempCounts[MAX_BEHAVIOUR_NODES];

  for (uint8_t i = 0; i < count; i++) {
    uint8_t typeVal;
    if (!stream.unserialize8(&typeVal)) {
      return false;
    }
    BehaviourNode *n = createNodeByType((NodeType)typeVal);
    if (!n) {
      return false;
    }
    if (!stream.unserialize32((uint32_t *)&n->param1)) {
      return false;
    }
    if (!stream.unserialize32((uint32_t *)&n->param2)) {
      return false;
    }

    uint8_t inputCount;
    if (!stream.unserialize8(&inputCount)) {
      return false;
    }
    tempCounts[i] = inputCount;
    for (uint8_t j = 0; j < inputCount; j++) {
      uint8_t idx;
      if (!stream.unserialize8(&idx)) {
        return false;
      }
      tempInputs[i][j] = (BehaviourNode *)(uintptr_t)idx;
    }
    m_nodes[i] = n;
  }
  m_nodeCount = count;
  for (uint8_t i = 0; i < count; i++) {
    BehaviourNode *n = m_nodes[i];
    for (uint8_t j = 0; j < tempCounts[i]; j++) {
      uint8_t idx = (uint8_t)(uintptr_t)tempInputs[i][j];
      if (idx >= m_nodeCount) {
        return false;
      }
      n->addInput(m_nodes[idx]);
    }
  }
  return true;
}

void Behaviours::clear()
{
  for (uint8_t i = 0; i < m_nodeCount; i++) {
    delete m_nodes[i];
  }
  m_nodeCount = 0;
}

void Behaviours::copyNextMode()
{
  Mode *nextMode = Modes::nextMode();
  if (nextMode) {
    // take copy of next mode
    m_otherMode = *nextMode;
  }
  // go back
  Modes::previousMode();
}

uint8_t Behaviours::nodeCount()
{
  return m_nodeCount;
}

BehaviourNode *Behaviours::node(uint8_t index)
{
  if (index >= m_nodeCount)
    return nullptr;

  return m_nodes[index];
}

uint8_t Behaviours::create(NodeType type, float param1, float param2)
{
  if (m_nodeCount >= MAX_BEHAVIOUR_NODES)
    return 255;

  BehaviourNode *n = createNodeByType(type);
  n->param1 = param1;
  n->param2 = param2;

  if (!n)
    return 255;

  m_nodes[m_nodeCount] = n;

  return m_nodeCount++;
}

bool Behaviours::connect(uint8_t from, uint8_t to)
{
  if (from >= m_nodeCount || to >= m_nodeCount)
    return false;

  m_nodes[to]->addInput(m_nodes[from]);

  return true;
}

BehaviourNode *Behaviours::createNodeByType(NodeType type)
{
  switch (type) {

    // ===== INPUT =====
  case NODE_INPUT_TIME: return new NodeTime();
  case NODE_INPUT_DELTA_TIME: return new NodeDeltaTime();
  case NODE_INPUT_RANDOM: return new NodeRandom();
  case NODE_INPUT_CONSTANT: return new NodeConstant();

  case NODE_INPUT_ACCEL_MOTION: return new NodeAccelMotion();
  case NODE_INPUT_ACCEL_NORMALIZED: return new NodeAccelNormalized();
  case NODE_INPUT_ACCEL_CURVED: return new NodeAccelCurved();
  case NODE_INPUT_ACCEL_FILTERED: return new NodeAccelFiltered();
  case NODE_INPUT_ACCEL_DIR_X: return new NodeAccelDirX();
  case NODE_INPUT_ACCEL_DIR_Y: return new NodeAccelDirY();
  case NODE_INPUT_ACCEL_DIR_Z: return new NodeAccelDirZ();
  case NODE_INPUT_ACCEL_PITCH: return new NodeAccelPitch();
  case NODE_INPUT_ACCEL_ROLL: return new NodeAccelRoll();
  case NODE_INPUT_ACCEL_TILT: return new NodeAccelTilt();

    // ===== MODIFIER =====
  case NODE_MODIFIER_ABS: return new NodeAbs();
  case NODE_MODIFIER_ADD: return new NodeAdd();
  case NODE_MODIFIER_SUBTRACT: return new NodeSubtract();
  case NODE_MODIFIER_MULTIPLY: return new NodeMultiply();
  case NODE_MODIFIER_DIVIDE: return new NodeDivide();

  case NODE_MODIFIER_MIN: return new NodeMin();
  case NODE_MODIFIER_MAX: return new NodeMax();

  case NODE_MODIFIER_CLAMP: return new NodeClamp();
  case NODE_MODIFIER_REMAP: return new NodeRemap();

  case NODE_MODIFIER_CURVE: return new NodeCurve();
  case NODE_MODIFIER_SMOOTHSTEP: return new NodeSmoothstep();

  case NODE_MODIFIER_SIN: return new NodeSin();
  case NODE_MODIFIER_COS: return new NodeCos();

  case NODE_MODIFIER_THRESHOLD: return new NodeThreshold();
  case NODE_MODIFIER_GREATER: return new NodeGreater();
  case NODE_MODIFIER_LESS: return new NodeLess();

  case NODE_MODIFIER_LERP: return new NodeLerp();
  case NODE_MODIFIER_SELECT: return new NodeSelect();

    // ===== FUNCTIONAL =====
  case NODE_FUNCTIONAL_MODE_BLEND: return new NodeModeBlend();
  case NODE_FUNCTIONAL_MODE_ADD: return new NodeModeAdd();

  case NODE_FUNCTIONAL_BRIGHTNESS_SHIFT: return new NodeBrightnessShift();
  case NODE_FUNCTIONAL_COLOR_SHIFT: return new NodeColorShift();
  case NODE_FUNCTIONAL_PATTERN_SHIFT: return new NodePatternShift();

  default: return nullptr;
  }
}
