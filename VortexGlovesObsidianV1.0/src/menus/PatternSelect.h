#ifndef PATTERN_SELECT_H
#define PATTERN_SELECT_H

#include "Menu.h"

class Colorset;
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

    // the selected list
    uint32_t m_list;

    // pointer to the colorset currently used for displaying demos
    Colorset *m_pColorset;

    // a pointer to the new pattern
    Pattern *m_pNewPattern;
};

#endif
