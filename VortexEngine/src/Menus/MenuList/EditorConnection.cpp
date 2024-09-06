#include "EditorConnection.h"

#include "../../VortexEngine.h"

#include "../../Patterns/PatternArgs.h"
#include "../../Serial/ByteStream.h"
#include "../../Serial/Serial.h"
#include "../../Storage/Storage.h"
#include "../../Wireless/VLSender.h"
#include "../../Time/TimeControl.h"
#include "../../Colors/Colorset.h"
#include "../../Modes/Modes.h"
#include "../../Modes/Mode.h"
#include "../../Leds/Leds.h"
#include "../../Log/Log.h"

#include <string.h>

EditorConnection::EditorConnection(VortexEngine &engine, const RGBColor &col, bool advanced) :
  Menu(engine, col, advanced),
  m_state(STATE_DISCONNECTED),
  m_allowReset(true)
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
  // skip led selection
  m_ledSelected = true;
  clearDemo();
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
  for (size_t i = 0; i < len; ++i) {
    if (!m_receiveBuffer.unserialize8(&byte)) {
      return false;
    }
  }
  // if everything was read out, reset
  if (m_receiveBuffer.unserializerAtEnd()) {
    m_receiveBuffer.clear();
  }
  // we have now received at least one command, do not allow resetting
  m_allowReset = false;
  return true;
}

void EditorConnection::clearDemo()
{
  Colorset set(RGB_WHITE0);
  PatternArgs args(1, 0, 0);
  m_previewMode.setPattern(PATTERN_STROBE, LED_ALL, &args, &set);
  m_previewMode.init();
}

Menu::MenuAction EditorConnection::run()
{
  MenuAction result = Menu::run();
  if (result != MENU_CONTINUE) {
    return result;
  }
  // TODO: auto leave the editor menu when unplugged
  // show the editor
  showEditor();
  // receive any data from serial into the receive buffer
  receiveData();
  // operate on the state of the editor connection
  switch (m_state) {
  case STATE_DISCONNECTED:
    // not connected yet so check for connections
    if (!m_engine.serial().isConnected()) {
      if (!m_engine.serial().checkSerial()) {
        // no connection found just continue waiting
        break;
      }
    }
    // a connection was found, say hello
    m_state = STATE_GREETING;
    break;
  case STATE_GREETING:
    m_receiveBuffer.clear();
    // send the hello greeting with our version number and build time
    m_engine.serial().write(EDITOR_VERB_GREETING);
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
    m_receiveBuffer.clear();
    // send our acknowledgement that the modes were sent
    m_engine.serial().write(EDITOR_VERB_PULL_MODES_ACK);
    // go idle
    m_state = STATE_IDLE;
    break;
  case STATE_PUSH_MODES:
    // editor requested to push modes, clear first and reset first
    m_receiveBuffer.clear();
    // now say we are ready
    m_engine.serial().write(EDITOR_VERB_READY);
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
    m_receiveBuffer.clear();
    m_engine.serial().write(EDITOR_VERB_PUSH_MODES_ACK);
    m_state = STATE_IDLE;
    break;
  case STATE_DEMO_MODE:
    // editor requested to push modes, clear first and reset first
    m_receiveBuffer.clear();
    // now say we are ready
    m_engine.serial().write(EDITOR_VERB_READY);
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
    m_receiveBuffer.clear();
    m_engine.serial().write(EDITOR_VERB_DEMO_MODE_ACK);
    m_state = STATE_IDLE;
    break;
  case STATE_CLEAR_DEMO:
    clearDemo();
    m_receiveBuffer.clear();
    m_engine.serial().write(EDITOR_VERB_CLEAR_DEMO_ACK);
    m_state = STATE_IDLE;
    break;
  case STATE_TRANSMIT_MODE_VL:
#if VL_ENABLE_SENDER == 1
    // if still sending and the send command indicated more data
    if (m_engine.vlSender().isSending() && m_engine.vlSender().send()) {
      // then continue sending
      break;
    }
#endif
    // othewrise, done, switch to the transmit done state
    m_state = STATE_TRANSMIT_MODE_VL_DONE;
    break;
  case STATE_TRANSMIT_MODE_VL_DONE:
    // done transmitting
    m_receiveBuffer.clear();
    m_engine.serial().write(EDITOR_VERB_TRANSMIT_VL_ACK);
    m_state = STATE_IDLE;
    break;
  case STATE_PULL_EACH_MODE:
    // editor requested pull modes, send the modes
    m_receiveBuffer.clear();
    sendModeCount();
    m_state = STATE_PULL_EACH_MODE_COUNT;
    break;
  case STATE_PULL_EACH_MODE_COUNT:
    if (receiveMessage(EDITOR_VERB_PULL_EACH_MODE_ACK)) {
      if (m_engine.modes().numModes() == 0) {
        m_state = STATE_PULL_EACH_MODE_DONE;
      } else {
        m_previousModeIndex = m_engine.modes().curModeIndex();
        m_state = STATE_PULL_EACH_MODE_SEND;
      }
    }
    break;
  case STATE_PULL_EACH_MODE_SEND:
    m_receiveBuffer.clear();
    // send the current mode
    sendCurMode();
    // wait for the ack
    m_state = STATE_PULL_EACH_MODE_WAIT;
    break;
  case STATE_PULL_EACH_MODE_WAIT:
    // recive the ack from the editor to send next mode
    if (receiveMessage(EDITOR_VERB_PULL_EACH_MODE_ACK)) {
      // if there is still more modes
      if (m_engine.modes().curModeIndex() < (m_engine.modes().numModes() - 1)) {
        // then iterate to the next mode and send
        m_engine.modes().nextMode();
        m_state = STATE_PULL_EACH_MODE_SEND;
      } else {
        // otherwise done sending modes
        m_state = STATE_PULL_EACH_MODE_DONE;
      }
    }
    break;
  case STATE_PULL_EACH_MODE_DONE:
    m_receiveBuffer.clear();
    // send our acknowledgement that the modes were sent
    m_engine.serial().write(EDITOR_VERB_PULL_EACH_MODE_DONE);
    // switch back to the previous mode
    m_engine.modes().setCurMode(m_previousModeIndex);
    // go idle
    m_state = STATE_IDLE;
    break;
  case STATE_PUSH_EACH_MODE:
    // editor requested to push modes, find out how many
    m_receiveBuffer.clear();
    // ack the command and wait for the amount of modes
    m_engine.serial().write(EDITOR_VERB_PUSH_EACH_MODE_ACK);
    m_state = STATE_PUSH_EACH_MODE_COUNT;
    break;
  case STATE_PUSH_EACH_MODE_COUNT:
    if (receiveModeCount()) {
      // clear modes and start receiving
      m_engine.modes().clearModes();
      // write out an ack
      m_receiveBuffer.clear();
      m_engine.serial().write(EDITOR_VERB_PUSH_EACH_MODE_ACK);
      // ready to receive a mode
      m_state = STATE_PUSH_EACH_MODE_RECEIVE;
    }
    break;
  case STATE_PUSH_EACH_MODE_RECEIVE:
    // receive the modes into the receive buffer
    if (receiveMode()) {
      if (m_engine.modes().numModes() < m_numModesToReceive) {
        // clear the receive buffer and ack the mode, continue receiving
        m_receiveBuffer.clear();
        m_engine.serial().write(EDITOR_VERB_PUSH_EACH_MODE_ACK);
      } else {
        // success modes were received send the done
        m_state = STATE_PUSH_EACH_MODE_DONE;
      }
    }
    break;
  case STATE_PUSH_EACH_MODE_DONE:
    // say we are done
    m_receiveBuffer.clear();
    m_engine.serial().write(EDITOR_VERB_PUSH_EACH_MODE_DONE);
    m_state = STATE_IDLE;
    break;
  }
  return MENU_CONTINUE;
}

void EditorConnection::sendCurModeVL()
{
#if VL_ENABLE_SENDER == 1
  // immediately load the mode and send it now
  m_engine.vlSender().loadMode(&m_previewMode);
  m_engine.vlSender().send();
#endif
  m_state = STATE_TRANSMIT_MODE_VL;
}

// handlers for clicks
void EditorConnection::onShortClick()
{
  // if the device has received any commands do not reset!
  if (!m_allowReset) {
    return;
  }
  // reset, this won't actually disconnect the com port
  m_state = STATE_DISCONNECTED;
  // clear the demo
  clearDemo();
  // sent a reset, do not allow another
  m_allowReset = false;
}

void EditorConnection::onLongClick()
{
  leaveMenu(true);
}

void EditorConnection::leaveMenu(bool doSave)
{
  m_engine.serial().write(EDITOR_VERB_GOODBYE);
  Menu::leaveMenu(true);
}

void EditorConnection::showEditor()
{
  switch (m_state) {
  case STATE_DISCONNECTED:
    m_engine.leds().clearAll();
    m_engine.leds().blinkAll(250, 150, RGB_WHITE0);
    break;
  case STATE_IDLE:
    m_previewMode.play();
    break;
  default:
    // do nothing!
    // Note if you clear the leds while selecting color
    // it may make the color selection choppy
    break;
  }
}

void EditorConnection::receiveData()
{
  // read more data into the receive buffer
  m_engine.serial().read(m_receiveBuffer);
}

void EditorConnection::sendModes()
{
  ByteStream modesBuffer;
  m_engine.modes().saveToBuffer(modesBuffer);
  m_engine.serial().write(modesBuffer);
}

void EditorConnection::sendModeCount()
{
  ByteStream buffer;
  buffer.serialize8(m_engine.modes().numModes());
  m_engine.serial().write(buffer);
}

void EditorConnection::sendCurMode()
{
  ByteStream modeBuffer;
  Mode *cur = m_engine.modes().curMode();
  if (!cur) {
    // ??
    return;
  }
  if (!cur->saveToBuffer(modeBuffer)) {
    // ??
    return;
  }
  m_engine.serial().write(modeBuffer);
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
  if (!m_receiveBuffer.unserialize32(&size)) {
    return false;
  }
  // create a new ByteStream that will hold the full buffer of data
  ByteStream buf(m_receiveBuffer.rawSize());
  // then copy everything from the receive buffer into the rawdata
  // which is going to overwrite the crc/size/flags of the ByteStream
  memcpy(buf.rawData(), m_receiveBuffer.data() + sizeof(size),
    m_receiveBuffer.size() - sizeof(size));
  // clear the receive buffer
  m_receiveBuffer.clear();
  m_engine.modes().loadFromBuffer(buf);
  m_engine.modes().saveStorage();
  return true;
}

bool EditorConnection::receiveModeCount()
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
  if (!m_receiveBuffer.unserialize32(&size)) {
    return false;
  }
  // create a new ByteStream that will hold the full buffer of data
  ByteStream buf(m_receiveBuffer.rawSize());
  // then copy everything from the receive buffer into the rawdata
  // which is going to overwrite the crc/size/flags of the ByteStream
  memcpy(buf.rawData(), m_receiveBuffer.data() + sizeof(size),
    m_receiveBuffer.size() - sizeof(size));
  // unserialize the mode count
  if (!buf.unserialize8(&m_numModesToReceive)) {
    return false;
  }
  if (m_numModesToReceive > MAX_MODES) {
    return false;
  }
  // good mode count
  return true;
}

bool EditorConnection::receiveMode()
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
  if (!m_receiveBuffer.unserialize32(&size)) {
    return false;
  }
  // create a new ByteStream that will hold the full buffer of data
  ByteStream buf(m_receiveBuffer.rawSize());
  // then copy everything from the receive buffer into the rawdata
  // which is going to overwrite the crc/size/flags of the ByteStream
  memcpy(buf.rawData(), m_receiveBuffer.data() + sizeof(size),
    m_receiveBuffer.size() - sizeof(size));
  // clear the receive buffer
  m_receiveBuffer.clear();
  // unserialize the mode into the demo mode
  if (!m_engine.modes().addModeFromBuffer(buf)) {
    // error
  }
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
  if (!m_receiveBuffer.unserialize32(&size)) {
    return false;
  }
  // create a new ByteStream that will hold the full buffer of data
  ByteStream buf(m_receiveBuffer.rawSize());
  // then copy everything from the receive buffer into the rawdata
  // which is going to overwrite the crc/size/flags of the ByteStream
  memcpy(buf.rawData(), m_receiveBuffer.data() + sizeof(size),
    m_receiveBuffer.size() - sizeof(size));
  // clear the receive buffer
  m_receiveBuffer.clear();
  // unserialize the mode into the demo mode
  if (!m_previewMode.loadFromBuffer(buf)) {
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
  } else if (receiveMessage(EDITOR_VERB_PULL_EACH_MODE)) {
    m_state = STATE_PULL_EACH_MODE;
  } else if (receiveMessage(EDITOR_VERB_PUSH_EACH_MODE)) {
    m_state = STATE_PUSH_EACH_MODE;
  } else if (receiveMessage(EDITOR_VERB_TRANSMIT_VL)) {
    sendCurModeVL();
  }
}
