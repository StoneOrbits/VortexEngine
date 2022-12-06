#ifndef EDITOR_CONNECTION_H
#define EDITOR_CONNECTION_H


#include "../Menu.h"

class EditorConnection : public Menu
{
public:
  EditorConnection();

  bool init();
  bool run();

  // handlers for clicks
  void onShortClick();
  void onLongClick();

private:
  void showEditor();

  // override showExit so it isn't displayed on thumb
  virtual void showExit() override;

  // whether actively connected to the editor
  bool m_connected;
};

#endif
