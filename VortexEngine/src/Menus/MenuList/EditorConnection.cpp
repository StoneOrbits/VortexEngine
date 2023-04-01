#include "EditorConnection.h"

#include "../../Log/Log.h"

EditorConnection::EditorConnection() :
  Menu()
{
}

EditorConnection::~EditorConnection()
{
}

bool EditorConnection::init()
{
  if (!Menu::init()) {
    return false;
  }
  DEBUG_LOG("Entering Editor Connection");
  return true;
}

bool EditorConnection::run()
{
  if (!Menu::run()) {
    return false;
  }
  return true;
}
