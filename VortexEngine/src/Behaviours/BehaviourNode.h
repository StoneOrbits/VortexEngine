#ifndef BEHAVIOURS_NODE_H
#define BEHAVIOURS_NODE_H

#include <stdint.h>
#include "../VortexConfig.h"

#include "Behaviours.h"

class VortexEngine;

class BehaviourNode
{
public:

  BehaviourNode(VortexEngine &engine, Behaviours::NodeType type);
  virtual ~BehaviourNode();

  void addInput(BehaviourNode *n);
  BehaviourNode *input(uint8_t i) const;
  uint8_t inputCount() const;
  Behaviours::NodeType type() const;
  virtual float evaluate() = 0;

  float param1;
  float param2;

protected:
  VortexEngine &m_engine;
  BehaviourNode *m_inputs[MAX_BEHAVIOUR_INPUTS];
  uint8_t m_inputCount;
  Behaviours::NodeType m_type;
  float m_cached;

  friend class Behaviours;
};

class NodeAccelMotion : public BehaviourNode
{
public:
  NodeAccelMotion(VortexEngine &engine);
  float evaluate();
};

class NodeAccelNormalized : public BehaviourNode
{
public:
  NodeAccelNormalized(VortexEngine &engine);
  float evaluate();
};

class NodeAccelCurved : public BehaviourNode
{
public:
  NodeAccelCurved(VortexEngine &engine);
  float evaluate();
};

class NodeAccelFiltered : public BehaviourNode
{
public:
  NodeAccelFiltered(VortexEngine &engine);
  float evaluate();
};

class NodeAccelDirX : public BehaviourNode
{
public:
  NodeAccelDirX(VortexEngine &engine);
  float evaluate();
};

class NodeAccelDirY : public BehaviourNode
{
public:
  NodeAccelDirY(VortexEngine &engine);
  float evaluate();
};

class NodeAccelDirZ : public BehaviourNode
{
public:
  NodeAccelDirZ(VortexEngine &engine);
  float evaluate();
};

class NodeAccelPitch : public BehaviourNode
{
public:
  NodeAccelPitch(VortexEngine &engine);
  float evaluate();
};

class NodeAccelRoll : public BehaviourNode
{
public:
  NodeAccelRoll(VortexEngine &engine);
  float evaluate();
};

class NodeAccelTilt : public BehaviourNode
{
public:
  NodeAccelTilt(VortexEngine &engine);
  float evaluate();
};

class NodeAbs : public BehaviourNode
{
public:
  NodeAbs(VortexEngine &engine);
  float evaluate();
};

class NodeAdd : public BehaviourNode
{
public:
  NodeAdd(VortexEngine &engine);
  float evaluate();
};

class NodeMultiply : public BehaviourNode
{
public:
  NodeMultiply(VortexEngine &engine);
  float evaluate();
};

class NodeClamp : public BehaviourNode
{
public:
  NodeClamp(VortexEngine &engine);
  float evaluate();
};

class NodeCurve : public BehaviourNode
{
public:
  NodeCurve(VortexEngine &engine);
  float evaluate();
};

class NodeThreshold : public BehaviourNode
{
public:
  NodeThreshold(VortexEngine &engine);
  float evaluate();
};

class NodeModeBlend : public BehaviourNode
{
public:
  NodeModeBlend(VortexEngine &engine);
  float evaluate();
};

#endif