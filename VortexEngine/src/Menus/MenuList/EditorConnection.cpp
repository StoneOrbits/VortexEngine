#include "EditorConnection.h"

#include "../../Serial/ByteStream.h"
#include "../../Serial/Serial.h"
#include "../../Modes/ModeBuilder.h"
#include "../../Time/TimeControl.h"
#include "../../Modes/Modes.h"
#include "../../Modes/Mode.h"
#include "../../Leds/Leds.h"
#include "../../Log/Log.h"

EditorConnection::EditorConnection() :
  Menu(),
  m_connected(false)
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

#include <string>

bool EditorConnection::run()
{
  if (!Menu::run()) {
    return false;
  }
  showEditor();
  if (!SerialComs::isConnected()) {
    SerialComs::checkSerial();
    return true;
  }
  if (!m_connected) {
    Serial.println("Hello loser");
  }
  m_connected = true;
  return true;
}

// handlers for clicks
void EditorConnection::onShortClick()
{
}

void EditorConnection::onLongClick()
{
  leaveMenu();
}

void EditorConnection::showEditor()
{
  Leds::clearAll();
  // gradually fill from thumb to pinkie
  Leds::blinkAll(Time::getCurtime(), 250, 150, RGB_BLANK);
  if (SerialComs::isConnected()) {
    Leds::blinkAll(Time::getCurtime(), 250, 150, RGB_GREEN);
  }
}

// override showExit so it isn't displayed on thumb
void EditorConnection::showExit()
{
}