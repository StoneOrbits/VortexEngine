#ifndef BEHAVIOURS_NODE_H
#define BEHAVIOURS_NODE_H

#include <stdint.h>
#include "../VortexConfig.h"
#include "../Random/Random.h"
#include "Behaviours.h"

class BehaviourNode
{
public:
  BehaviourNode(Behaviours::NodeType type);
  virtual ~BehaviourNode();

  void addInput(BehaviourNode *n);
  BehaviourNode *input(uint8_t i) const;
  uint8_t inputCount() const;
  Behaviours::NodeType type() const;
  virtual float evaluate() = 0;

  float param1;
  float param2;

protected:
  BehaviourNode *m_inputs[MAX_BEHAVIOUR_INPUTS];
  uint8_t m_inputCount;
  Behaviours::NodeType m_type;
  float m_cached;

  friend class Behaviours;
};

/* ===== INPUT ===== */

class NodeTime : public BehaviourNode { public: NodeTime(); float evaluate(); };
class NodeDeltaTime : public BehaviourNode { public: NodeDeltaTime(); float evaluate(); };
class NodeRandom : public BehaviourNode { public: NodeRandom(); float evaluate(); Random m_rand; };
class NodeConstant : public BehaviourNode { public: NodeConstant(); float evaluate(); };

class NodeAccelMotion : public BehaviourNode { public: NodeAccelMotion(); float evaluate(); };
class NodeAccelNormalized : public BehaviourNode { public: NodeAccelNormalized(); float evaluate(); };
class NodeAccelCurved : public BehaviourNode { public: NodeAccelCurved(); float evaluate(); };
class NodeAccelFiltered : public BehaviourNode { public: NodeAccelFiltered(); float evaluate(); };
class NodeAccelDirX : public BehaviourNode { public: NodeAccelDirX(); float evaluate(); };
class NodeAccelDirY : public BehaviourNode { public: NodeAccelDirY(); float evaluate(); };
class NodeAccelDirZ : public BehaviourNode { public: NodeAccelDirZ(); float evaluate(); };
class NodeAccelPitch : public BehaviourNode { public: NodeAccelPitch(); float evaluate(); };
class NodeAccelRoll : public BehaviourNode { public: NodeAccelRoll(); float evaluate(); };
class NodeAccelTilt : public BehaviourNode { public: NodeAccelTilt(); float evaluate(); };

/* ===== MODIFIER ===== */

class NodeAbs : public BehaviourNode { public: NodeAbs(); float evaluate(); };
class NodeAdd : public BehaviourNode { public: NodeAdd(); float evaluate(); };
class NodeSubtract : public BehaviourNode { public: NodeSubtract(); float evaluate(); };
class NodeMultiply : public BehaviourNode { public: NodeMultiply(); float evaluate(); };
class NodeDivide : public BehaviourNode { public: NodeDivide(); float evaluate(); };

class NodeMin : public BehaviourNode { public: NodeMin(); float evaluate(); };
class NodeMax : public BehaviourNode { public: NodeMax(); float evaluate(); };

class NodeClamp : public BehaviourNode { public: NodeClamp(); float evaluate(); };
class NodeRemap : public BehaviourNode { public: NodeRemap(); float evaluate(); };

class NodeCurve : public BehaviourNode { public: NodeCurve(); float evaluate(); };
class NodeSmoothstep : public BehaviourNode { public: NodeSmoothstep(); float evaluate(); };

class NodeSin : public BehaviourNode { public: NodeSin(); float evaluate(); };
class NodeCos : public BehaviourNode { public: NodeCos(); float evaluate(); };

class NodeThreshold : public BehaviourNode { public: NodeThreshold(); float evaluate(); };
class NodeGreater : public BehaviourNode { public: NodeGreater(); float evaluate(); };
class NodeLess : public BehaviourNode { public: NodeLess(); float evaluate(); };

class NodeLerp : public BehaviourNode { public: NodeLerp(); float evaluate(); };
class NodeSelect : public BehaviourNode { public: NodeSelect(); float evaluate(); };

/* ===== FUNCTIONAL ===== */

class NodeModeBlend : public BehaviourNode { public: NodeModeBlend(); float evaluate(); };
class NodeModeAdd : public BehaviourNode { public: NodeModeAdd(); float evaluate(); };

class NodeBrightnessShift : public BehaviourNode { public: NodeBrightnessShift(); float evaluate(); };
class NodeColorShift : public BehaviourNode { public: NodeColorShift(); float evaluate(); };
class NodePatternShift : public BehaviourNode { public: NodePatternShift(); float evaluate(); };

#endif