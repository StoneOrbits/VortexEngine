#ifndef MENU_H
#define MENU_H

#include <inttypes.h>

#include "../Colors/ColorTypes.h"
#include "../Leds/LedTypes.h"

class Mode;
class BasicPattern;

class Menu
{
public:
  Menu();
  virtual ~Menu();

  // optional init function can be overridden, this is called when the menu is
  // first opened to initialize the menu. It will be called each time the menu
  // is opened
  virtual bool init();

  // when the menu runs it will have access to time, the button and led control
  virtual bool run() = 0;

  // optional handlers for clicks
  virtual void onShortClick();
  virtual void onLongClick();

protected:
  // close the current menu
  virtual void leaveMenu(bool doSave = false);
  // blink the selected finger
  virtual void blinkSelection(uint32_t offMs = 250, uint32_t onMs = 500);

  virtual bool showOptions() const;

  // set a color on a finger
  virtual void addOption(RGBColor col);
  virtual void setOptions(uint32_t numOptions, const RGBColor *cols);

  // all menus have an 'option selection' which is one of the registered 
  // options in order starting from 0
  typedef uint8_t OptionSelection;

  // the current mode that was selected
  Mode *m_pCurMode;

  // the current selected option which will blink a different color
  OptionSelection m_curSelection;

private: 
  struct MenuOption {
    RGBColor color;
  };

  // list of options
  MenuOption *m_options[OptionIndex::MENU_OPTION_COUNT];

  // a basic pattern used to blink the selection
  BasicPattern *m_blinker;

  // whether to close the menu
  bool m_shouldClose;
};

#endif
