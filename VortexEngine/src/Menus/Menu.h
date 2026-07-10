#ifndef MENU_H
#define MENU_H

#include <inttypes.h>
#include <stdbool.h>

#include "../Colors/ColorTypes.h"
#include "../Leds/LedTypes.h"
#include "../Modes/Mode.h"

typedef uint8_t MenuAction;
enum {
  MENU_QUIT,
  MENU_CONTINUE,
  MENU_SKIP
};

typedef struct Menu Menu;
typedef struct MenuVTable {
  void (*destroy)(Menu *self);
  bool (*init)(Menu *self);
  MenuAction (*run)(Menu *self);
  void (*onLedSelected)(Menu *self);
  void (*onShortClick)(Menu *self);
  void (*onLongClick)(Menu *self);
  void (*leaveMenu)(Menu *self, bool doSave);
} MenuVTable;

struct Menu {
  const MenuVTable *vtable;
  Mode previewMode;
  RGBColor menuColor;
  LedMap targetLeds;
  uint8_t curSelection;
  bool ledSelected;
  bool advanced;
  bool shouldClose;
};

void Menu_construct(Menu *self, RGBColor col, bool advanced);

void Menu_destroy(Menu *self);
bool Menu_init(Menu *self);
MenuAction Menu_run(Menu *self);
void Menu_onLedSelected(Menu *self);
void Menu_onShortClick(Menu *self);
void Menu_onLongClick(Menu *self);
void Menu_leaveMenu(Menu *self, bool doSave);

void Menu_showBulbSelection(Menu *self);
void Menu_showExit(Menu *self);
void Menu_nextBulbSelection(Menu *self);
void Menu_bypassLedSelection(Menu *self, LedMap map);

extern const MenuVTable g_menuVTable;

#endif
