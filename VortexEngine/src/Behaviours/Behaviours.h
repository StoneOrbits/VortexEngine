#ifndef BEHAVIOURS_H
#define BEHAVIOURS_H

#include <stdint.h>

#include "../Serial/ByteStream.h"
#include "../VortexConfig.h"
#include "../Modes/Mode.h"

class BehaviourNode;

class Behaviours
{
public:

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
    NODE_MODE_SWITCH,
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

  static bool init();
  static void cleanup();

  static void update();

  static bool serialize(ByteStream &stream);
  static bool unserialize(ByteStream &stream);

  static void clear();
  static uint8_t create(NodeType type, float param1 = 1.0f, float param2 = 1.0f);
  static bool connect(uint8_t from, uint8_t to);

  static BehaviourNode *node(uint8_t index);
  static uint8_t nodeCount();

  static void copyNextMode();

  static Mode &otherMode() { return m_otherMode; }

  // Helper API to check subtype
  static NodeSubtype getSubtype(NodeType type)
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

  static bool isInput(NodeType type) { return getSubtype(type) == SUBTYPE_INPUT; }
  static bool isModifier(NodeType type) { return getSubtype(type) == SUBTYPE_MODIFIER; }
  static bool isFunctional(NodeType type) { return getSubtype(type) == SUBTYPE_FUNCTIONAL; }

private:

  static BehaviourNode *createNodeByType(NodeType type);

  static BehaviourNode *m_nodes[MAX_BEHAVIOUR_NODES];
  static uint8_t m_nodeCount;
  static Mode m_otherMode;
};

#endif