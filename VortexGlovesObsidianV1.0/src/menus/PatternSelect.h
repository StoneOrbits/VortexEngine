#ifndef PATTERN_SELECT_H
#define PATTERN_SELECT_H

#include "Menu.h"

#include "../Colorset.h"

class Pattern;

class PatternSelect : public Menu
{
public:
  PatternSelect();

  bool init(Mode *curMode);

  bool run();

  // handlers for clicks
  void onShortClick();
  void onLongClick();

private:
  void showListSelection();
  void showPatternSelection();
  void nextPattern();

  // private enumeration for internal state of pattern selection
  enum PatternSelectState : uint32_t
  {
    // currently picking the list of patterns
    STATE_PICK_LIST,
    // currently picking a pattern in the list
    STATE_PICK_PATTERN
  };

  // the current state of the pattern selection menu
  PatternSelectState m_state;

  // copy of the current colorset to be used for demo display
  Colorset m_colorset;

  // a pointer to the current pattern
  Pattern *m_pNewPattern;
};

#endif
