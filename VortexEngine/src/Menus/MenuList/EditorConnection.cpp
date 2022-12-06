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
  m_state(STATE_DISCONNECTED)
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
  showEditor();
  switch (m_state) {
  case STATE_DISCONNECTED:
    if (!SerialComs::isConnected()) {
      if (!SerialComs::checkSerial()) {
        return true;
      }
    }
    m_state = STATE_HELLO;
    break;
  case STATE_HELLO:
    SerialComs::write("== Vortex Framework v" VORTEX_VERSION " (built " __TIMESTAMP__ ") ==");
    m_state = STATE_HELLO_RECEIVE;
    break;
  case STATE_HELLO_RECEIVE:
    receiveMessage();
    break;
  }
  return true;
}

// handlers for clicks
void EditorConnection::onShortClick()
{
  // reset, this won't actually disconnect the com port
  m_state = STATE_DISCONNECTED;
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
    if (m_state == STATE_IDLE) {
      Leds::blinkAll(Time::getCurtime(), 250, 150, RGB_BLUE);
    } else {
      Leds::blinkAll(Time::getCurtime(), 250, 150, RGB_GREEN);
    }
  }
}

// override showExit so it isn't displayed on thumb
void EditorConnection::showExit()
{
}

void EditorConnection::receiveMessage()
{
  ByteStream receiveBuffer;
  SerialComs::read(receiveBuffer);
  if (!receiveBuffer.data() || !receiveBuffer.size()) {
    return;
  }
  SerialComs::write("== Received: [%s] ==", receiveBuffer.data());
  if (strcmp((char *)receiveBuffer.data(), "Hello") == 0) {
    m_state = STATE_IDLE;
  }
}
