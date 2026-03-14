#ifndef BEHAVIOURS_NODE_H
#define BEHAVIOURS_NODE_H

#include <stdint.h>
#include "../VortexConfig.h"

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

class NodeAccelMotion : public BehaviourNode
{
public:
  NodeAccelMotion();
  float evaluate();
};

class NodeAccelNormalized : public BehaviourNode
{
public:
  NodeAccelNormalized();
  float evaluate();
};

class NodeAccelCurved : public BehaviourNode
{
public:
  NodeAccelCurved();
  float evaluate();
};

class NodeAccelFiltered : public BehaviourNode
{
public:
  NodeAccelFiltered();
  float evaluate();
};

class NodeAccelDirX : public BehaviourNode
{
public:
  NodeAccelDirX();
  float evaluate();
};

class NodeAccelDirY : public BehaviourNode
{
public:
  NodeAccelDirY();
  float evaluate();
};

class NodeAccelDirZ : public BehaviourNode
{
public:
  NodeAccelDirZ();
  float evaluate();
};

class NodeAccelPitch : public BehaviourNode
{
public:
  NodeAccelPitch();
  float evaluate();
};

class NodeAccelRoll : public BehaviourNode
{
public:
  NodeAccelRoll();
  float evaluate();
};

class NodeAccelTilt : public BehaviourNode
{
public:
  NodeAccelTilt();
  float evaluate();
};

class NodeAbs : public BehaviourNode
{
public:
  NodeAbs();
  float evaluate();
};

class NodeAdd : public BehaviourNode
{
public:
  NodeAdd();
  float evaluate();
};

class NodeMultiply : public BehaviourNode
{
public:
  NodeMultiply();
  float evaluate();
};

class NodeClamp : public BehaviourNode
{
public:
  NodeClamp();
  float evaluate();
};

class NodeCurve : public BehaviourNode
{
public:
  NodeCurve();
  float evaluate();
};

class NodeThreshold : public BehaviourNode
{
public:
  NodeThreshold();
  float evaluate();
};

class NodeModeBlend : public BehaviourNode
{
public:
  NodeModeBlend();
  float evaluate();
};

#endif