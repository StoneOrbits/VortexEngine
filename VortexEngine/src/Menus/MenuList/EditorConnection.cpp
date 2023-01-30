#include "EditorConnection.h"

#include "../../Serial/ByteStream.h"
#include "../../Serial/Serial.h"
#include "../../Storage/Storage.h"
#include "../../Time/TimeControl.h"
#include "../../Modes/Modes.h"
#include "../../Modes/Mode.h"
#include "../../Leds/Leds.h"
#include "../../Log/Log.h"

EditorConnection::EditorConnection() :
  Menu(),
  m_state(STATE_DISCONNECTED),
  m_pDemoMode(nullptr)
{
}

EditorConnection::~EditorConnection()
{
  clearDemo();
}

bool EditorConnection::init()
{
  if (!Menu::init()) {
    return false;
  }

  DEBUG_LOG("Entering Editor Connection");
  return true;
}

bool EditorConnection::receiveMessage(const char *message)
{
  size_t len = strlen(message);
  uint8_t byte = 0;
  // wait for the editor to ack the idle
  if (m_receiveBuffer.size() < len) {
    return false;
  }
  if (memcmp(m_receiveBuffer.frontUnserializer(), message, len) != 0) {
    return false;
  }
  for (uint32_t i = 0; i < len; ++i) {
    m_receiveBuffer.unserialize(&byte);
  }
  // if everything was read out, reset
  if (m_receiveBuffer.unserializerAtEnd()) {
    m_receiveBuffer.clear();
  }
  return true;
}

void EditorConnection::clearDemo()
{
  if (!m_pDemoMode) {
    return;
  }
  delete m_pDemoMode;
  m_pDemoMode = nullptr;
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

  //static bool done_yet = false;
  //if (!done_yet && SerialComs::checkSerial()) {
  //  done_yet = true;
  //  // this kills the vortex
  //  uint8_t buf[] = {
  //    0x3c, 0x00, 0x00, 0x00, 0x30, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x5d, 0xf0, 0x0e, 0x2a, 0xf1,
  //    0x06, 0x01, 0x00, 0x0a, 0x00, 0x00, 0x00, 0x00, 0x03, 0x02, 0x4b, 0x00, 0x29, 0xe2, 0x1c, 0x1b, 0x03,
  //    0x04, 0x10, 0x00, 0x03, 0x00, 0x06, 0x00, 0x08, 0x12, 0x00, 0x40, 0x02, 0x22, 0x03, 0x02, 0x26, 0x00,
  //    0x00, 0x1e, 0x00, 0x0f, 0x18, 0x00, 0x2f, 0x50, 0x00, 0x03, 0x04, 0x10, 0x00
  //  };
  //  m_receiveBuffer.init(sizeof(buf), buf);
  //  m_state = STATE_DEMO_MODE_RECEIVE;
  //  DEBUG_LOG("STARTING");
  //}

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
    m_state = STATE_GREETING;
    break;
  case STATE_GREETING:
    //m_receiveBuffer.clear();
    // send the hello greeting with our version number and build time
    SerialComs::write(EDITOR_VERB_GREETING);
    m_state = STATE_IDLE;
    break;
  case STATE_IDLE:
    // parse the receive buffer for any commands from the editor
    handleCommand();
    break;
  case STATE_PULL_MODES:
    // editor requested pull modes, send the modes
    sendModes();
    m_state = STATE_PULL_MODES_SEND;
    break;
  case STATE_PULL_MODES_SEND:
    // recive the send modes ack from the editor
    if (receiveMessage(EDITOR_VERB_PULL_MODES_DONE)) {
      m_state = STATE_PULL_MODES_DONE;
    }
    break;
  case STATE_PULL_MODES_DONE:
    //m_receiveBuffer.clear();
    // send our acknowledgement that the modes were sent
    SerialComs::write(EDITOR_VERB_PULL_MODES_ACK);
    // go idle
    m_state = STATE_IDLE;
    break;
  case STATE_PUSH_MODES:
    // editor requested to push modes, clear first and reset first
    //m_receiveBuffer.clear();
    // now say we are ready
    SerialComs::write(EDITOR_VERB_READY);
    // move to receiving
    m_state = STATE_PUSH_MODES_RECEIVE;
    break;
  case STATE_PUSH_MODES_RECEIVE:
    // receive the modes into the receive buffer
    if (receiveModes()) {
      // success modes were received send the done
      m_state = STATE_PUSH_MODES_DONE;
    }
    break;
  case STATE_PUSH_MODES_DONE:
    // say we are done
    //m_receiveBuffer.clear();
    SerialComs::write(EDITOR_VERB_PUSH_MODES_ACK);
    m_state = STATE_IDLE;
    break;
  case STATE_DEMO_MODE:
    // editor requested to push modes, clear first and reset first
    //m_receiveBuffer.clear();
    // now say we are ready
    SerialComs::write(EDITOR_VERB_READY);
    // move to receiving
    m_state = STATE_DEMO_MODE_RECEIVE;
    break;
  case STATE_DEMO_MODE_RECEIVE:
    // receive the modes into the receive buffer
    if (receiveDemoMode()) {
      // success modes were received send the done
      m_state = STATE_DEMO_MODE_DONE;
    }
    break;
  case STATE_DEMO_MODE_DONE:
    // say we are done
    //m_receiveBuffer.clear();
    SerialComs::write(EDITOR_VERB_DEMO_MODE_ACK);
    m_state = STATE_IDLE;
    break;
  case STATE_CLEAR_DEMO:
    clearDemo();
    //m_receiveBuffer.clear();
    SerialComs::write(EDITOR_VERB_CLEAR_DEMO_ACK);
    m_state = STATE_IDLE;
  }
  return true;
}

// handlers for clicks
void EditorConnection::onShortClick()
{
  // reset, this won't actually disconnect the com port
  m_state = STATE_DISCONNECTED;
  // clear the demo
  clearDemo();
}

void EditorConnection::onLongClick()
{
  leaveMenu(true);
}

void EditorConnection::leaveMenu(bool doSave)
{
  SerialComs::write(EDITOR_VERB_GOODBYE);
  Menu::leaveMenu(true);
}

void EditorConnection::showEditor()
{
  switch (m_state) {
  case STATE_DISCONNECTED:
    Leds::clearAll();
    Leds::blinkAll(Time::getCurtime(), 250, 150, RGB_BLANK);
    break;
  case STATE_IDLE:
    if (m_pDemoMode) {
      m_pDemoMode->play();
    } else {
      Leds::setAll(RGB_BLANK);
    }
    break;
  default:
    // do nothing!
    // Note if you clear the leds while selecting color
    // it may make the color selection choppy
    break;
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
  Modes::saveToBuffer(modesBuffer);
  SerialComs::write(modesBuffer);
}

bool EditorConnection::receiveModes()
{
  // need at least the buffer size first
  uint32_t size = 0;
  if (m_receiveBuffer.size() < sizeof(size)) {
    // wait, not enough data available yet
    return false;
  }
  // grab the size out of the start
  m_receiveBuffer.resetUnserializer();
  size = m_receiveBuffer.peek32();
  if (m_receiveBuffer.size() < (size + sizeof(size))) {
    // don't unserialize yet, not ready
    return false;
  }
  // okay unserialize now, first unserialize the size
  m_receiveBuffer.unserialize(&size);
  // todo: this is kinda jank but w/e
  memmove(m_receiveBuffer.rawData(),
    ((uint8_t *)m_receiveBuffer.data()) + sizeof(size),
    m_receiveBuffer.size());
  Modes::loadFromBuffer(m_receiveBuffer);
  Modes::saveStorage();
  return true;
}

bool EditorConnection::receiveDemoMode()
{
  // need at least the buffer size first
  uint32_t size = 0;
  if (m_receiveBuffer.size() < sizeof(size)) {
    // wait, not enough data available yet
    return false;
  }
  // grab the size out of the start
  m_receiveBuffer.resetUnserializer();
  size = m_receiveBuffer.peek32();
  if (m_receiveBuffer.size() < (size + sizeof(size))) {
    // don't unserialize yet, not ready
    return false;
  }
  // okay unserialize now, first unserialize the size
  m_receiveBuffer.unserialize(&size);
  // create a new ByteStream that will hold the full buffer of data
  ByteStream buf(m_receiveBuffer.rawSize());
  // then copy everything from the receive buffer into the rawdata
  // which is going to overwrite the crc/size/flags of the ByteStream
  memcpy(buf.rawData(), m_receiveBuffer.data() + sizeof(size),
    m_receiveBuffer.size() - sizeof(size));
  // clear the receive buffer
  m_receiveBuffer.clear();
  // unserialize the mode into the demo mode
  if (!m_pDemoMode) {
    m_pDemoMode = new Mode();
  }
  if (!m_pDemoMode->loadFromBuffer(buf)) {
    // failure
  }
  return true;
}

void EditorConnection::handleCommand()
{
  if (receiveMessage(EDITOR_VERB_PULL_MODES)) {
    m_state = STATE_PULL_MODES;
  } else if (receiveMessage(EDITOR_VERB_PUSH_MODES)) {
    m_state = STATE_PUSH_MODES;
  } else if (receiveMessage(EDITOR_VERB_DEMO_MODE)) {
    m_state = STATE_DEMO_MODE;
  } else if (receiveMessage(EDITOR_VERB_CLEAR_DEMO)) {
    m_state = STATE_CLEAR_DEMO;
  }
}
