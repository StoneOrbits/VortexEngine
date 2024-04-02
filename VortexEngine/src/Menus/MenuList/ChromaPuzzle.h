#ifndef CHROMA_PUZZLE_H
#define CHROMA_PUZZLE_H

#include "../Menu.h"
#include "../../Random/Random.h"

class ChromaPuzzle : public Menu
{
public:
  ChromaPuzzle(const RGBColor &col, bool advanced);
  ~ChromaPuzzle();

  bool init() override;
  MenuAction run() override;
  void show();

  void pressLeft();
  void pressRight();
  void select();
  void clearRings();
  void rotateRing(bool clockwise);
  void rotateSwapPattern(bool clockwise);
  void toggleRingSelection();
  void swapLEDs();
  void scramble();
  bool checkWinCondition();
  void win();

private:
  uint8_t m_currentLevel;
  bool m_selectedRingInner;
  RGBColor m_ledValuesInner[LED_COUNT / 2];
  RGBColor m_ledValuesOuter[LED_COUNT / 2];
  uint16_t m_swapPattern;
  Random m_rand;
};

#endif
