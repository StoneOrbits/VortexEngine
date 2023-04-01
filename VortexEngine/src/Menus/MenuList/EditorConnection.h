#ifndef EDITOR_CONNECTION_H
#define EDITOR_CONNECTION_H

#include "../Menu.h"

#include "../../Serial/ByteStream.h"
#include "../../Modes/Mode.h"

class EditorConnection : public Menu
{
public:
  EditorConnection();
  ~EditorConnection();

  bool init() override;
  bool run() override;

  // handlers for clicks
  //void onShortClick() override;
  //void onLongClick() override;

private:
};

#endif
