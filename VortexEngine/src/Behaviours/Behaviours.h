#ifndef BEHAVIOURS_H
#define BEHAVIOURS_H

#include <stdint.h>

#include "../Serial/ByteStream.h"
#include "../VortexConfig.h"
#include "../Modes/Mode.h"

class BehaviourNode;
class VortexEngine;

class Behaviours
{
public:
  Behaviours(VortexEngine &m_engine);

  // Node types with meta constants for subtypes
  enum NodeType : uint8_t
  {
    // =====================
    // Input Nodes
    // =====================
    NODE_INPUT_START = 1,

    NODE_ACCEL_MOTION = NODE_INPUT_START,
    NODE_ACCEL_NORMALIZED,
    NODE_ACCEL_CURVED,
    NODE_ACCEL_FILTERED,
    NODE_ACCEL_DIR_X,
    NODE_ACCEL_DIR_Y,
    NODE_ACCEL_DIR_Z,
    NODE_ACCEL_PITCH,
    NODE_ACCEL_ROLL,
    NODE_ACCEL_TILT,
    // add more here

    NODE_INPUT_END = NODE_ACCEL_TILT,  // Last input node
    NODE_INPUT_COUNT = NODE_INPUT_END - NODE_INPUT_START + 1, // Total input nodes

    // =====================
    // Modifier Nodes
    // =====================
    NODE_MODIFIER_START = NODE_INPUT_END + 1,

    NODE_ABS = NODE_MODIFIER_START,
    NODE_ADD,
    NODE_MULTIPLY,
    NODE_CLAMP,
    NODE_CURVE,
    NODE_THRESHOLD,
    // add more here

    NODE_MODIFIER_END = NODE_THRESHOLD, // Last modifier node
    NODE_MODIFIER_COUNT = NODE_MODIFIER_END - NODE_MODIFIER_START + 1, // Total modifier nodes

    // =====================
    // Functional Nodes
    // =====================
    NODE_FUNCTIONAL_START = NODE_MODIFIER_END + 1,

    NODE_MODE_BLEND = NODE_FUNCTIONAL_START,
    NODE_MODE_ADD,
    NODE_COLOR_SHIFT,
    // add more here

    NODE_FUNCTIONAL_END = NODE_MODE_BLEND, // Last functional node
    NODE_FUNCTIONAL_COUNT = NODE_FUNCTIONAL_END - NODE_FUNCTIONAL_START + 1, // Total functional nodes

    // =====================
    // Total Node Types
    // =====================
    NODE_TYPE_COUNT = NODE_FUNCTIONAL_END - NODE_INPUT_START + 1 // Total node types
  };

  // Subtype enums for clarity
  enum NodeSubtype : uint8_t
  {
    SUBTYPE_INPUT,
    SUBTYPE_MODIFIER,
    SUBTYPE_FUNCTIONAL,
    SUBTYPE_UNKNOWN
  };

public:

  bool init();
  void cleanup();

  void update();

  bool serialize(ByteStream &stream);
  bool unserialize(ByteStream &stream);

  void clear();
  uint8_t create(NodeType type, float param1 = 1.0f, float param2 = 1.0f);
  bool connect(uint8_t from, uint8_t to);

  BehaviourNode *node(uint8_t index);
  uint8_t nodeCount();

  void copyNextMode();

  Mode &otherMode() { return m_otherMode; }

  // Helper API to check subtype
  NodeSubtype getSubtype(NodeType type)
  {
    if (type >= NODE_INPUT_START && type <= NODE_INPUT_END) {
      return SUBTYPE_INPUT;
    }
    if (type >= NODE_MODIFIER_START && type <= NODE_MODIFIER_END) {
      return SUBTYPE_MODIFIER;
    }
    if (type >= NODE_FUNCTIONAL_START && type <= NODE_FUNCTIONAL_END) {
      return SUBTYPE_FUNCTIONAL;
    }
    return SUBTYPE_UNKNOWN;
  }

  bool isInput(NodeType type) { return getSubtype(type) == SUBTYPE_INPUT; }
  bool isModifier(NodeType type) { return getSubtype(type) == SUBTYPE_MODIFIER; }
  bool isFunctional(NodeType type) { return getSubtype(type) == SUBTYPE_FUNCTIONAL; }

private:
  BehaviourNode *createNodeByType(NodeType type);

  VortexEngine &m_engine;
  BehaviourNode *m_nodes[MAX_BEHAVIOUR_NODES];
  uint8_t m_nodeCount;
  Mode m_otherMode;
};

#endif
