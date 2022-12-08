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
  // show the editor
  showEditor();
  // receive any data from serial into the receive buffer
  receiveData();
  // operate on the state of the editor connection
  switch (m_state) {
  case STATE_DISCONNECTED:
    // not connected yet so check for connections
    if (!SerialComs::isConnected()) {
      if (!SerialComs::checkSerial()) {
        // no connection found just continue waiting
        return true;
      }
    }
    // a connection was found, say hello
    m_state = STATE_HELLO;
    break;
  case STATE_HELLO:
    // send the hello greeting with our version number and build time
    SerialComs::write(EDITOR_VERB_HELLO);
    // wait for the acknowledgement
    m_state = STATE_HELLO_ACK;
    break;
  case STATE_HELLO_ACK:
    // wait for the editor to say "Hello" back to us
    if (m_receiveBuffer.size() > 0 &&
        strcmp((char *)m_receiveBuffer.data(), EDITOR_VERB_HELLO_ACK) == 0) {
      // found the hello response, start going idle
      m_state = STATE_SEND_IDLE;
    }
    break;
  case STATE_SEND_IDLE:
    // send the hello greeting with our version number and build time
    SerialComs::write(EDITOR_VERB_IDLE);
    // sent the idle verb wait for ack
    m_state = STATE_IDLE_ACK;
    break;
  case STATE_IDLE_ACK:
    // wait for the editor to ack the idle
    if (m_receiveBuffer.size() > 0 &&
        strcmp((char *)m_receiveBuffer.data(), EDITOR_VERB_IDLE_ACK) == 0) {
      // found the idle response, go idle
      m_state = STATE_IDLE;
    }
    break;
  case STATE_IDLE:
    // parse the receive buffer for any commands from the editor
    handleCommand();
    break;
  case STATE_SEND_MODES:
    sendModes();
    m_state = STATE_SEND_MODES_ACK;
    break;
  case STATE_SEND_MODES_ACK:
    if (strcmp((char *)m_receiveBuffer.data(), EDITOR_VERB_PULL_ACK) == 0) {
      m_state = STATE_IDLE;
    }
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

void EditorConnection::receiveData()
{
  // read more data into the receive buffer
  SerialComs::read(m_receiveBuffer);
}

void EditorConnection::sendModes()
{
  ByteStream modesBuffer;
  Modes::serialize(modesBuffer);
  SerialComs::writeRaw(modesBuffer);
}

void EditorConnection::handleCommand()
{
  if (strcmp((char *)m_receiveBuffer.data(), EDITOR_VERB_PULL) == 0) {
    m_state = STATE_SEND_MODES;
  }
}
