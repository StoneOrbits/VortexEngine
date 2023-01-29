#ifndef PATTERN_SELECT_H
#define PATTERN_SELECT_H

#include "../Menu.h"

#include "../../Colors/Colorset.h"
#include "../../Patterns/Patterns.h"

class Mode;

class PatternSelect : public Menu
{
public:
  PatternSelect();
  ~PatternSelect();

  bool init();

  bool run();

  // handlers for clicks
  void onShortClick();
  void onShortClick2();
  void onLongClick();
  void onLongClick2();

private:
  void showListSelection();
  void showPatternSelection();
  void nextPattern();
  void previousPattern();

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

  // an internal mode used for demoing patterns
  Mode *m_pDemoMode;

  // the patternid of the current demo
  PatternID m_newPatternID;
};

#endif
