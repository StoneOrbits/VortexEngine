#include "EditorConnection.h"

#include "../../Patterns/PatternArgs.h"
#include "../../Serial/ByteStream.h"
#include "../../Serial/Serial.h"
#include "../../Storage/Storage.h"
#include "../../Wireless/VLSender.h"
#include "../../Time/TimeControl.h"
#include "../../Time/Timings.h"
#include "../../Colors/Colorset.h"
#include "../../Modes/Modes.h"
#include "../../Modes/Mode.h"
#include "../../Leds/Leds.h"
#include "../../Log/Log.h"

#include <string.h>

EditorConnection::EditorConnection(const RGBColor &col, bool advanced) :
  Menu(col, advanced),
  m_state(STATE_DISCONNECTED),
  m_allowReset(true),
  m_previousModeIndex(0),
  m_numModesToReceive(0),
  rv(RV_OK)
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
  if (memcmp(m_receiveBuffer.data(), message, len) != 0) {
    return false;
  }
  for (size_t i = 0; i < len; ++i) {
    if (!m_receiveBuffer.consume8(&byte)) {
      return false;
    }
  }
  // we have now received at least one command, do not allow resetting
  m_allowReset = false;
  return true;
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
  // handle the current state
  handleState();
  return MENU_CONTINUE;
}

void EditorConnection::handleState()
{
  // operate on the state of the editor connection
  switch (m_state) {
  // -------------------------------
  //  Disconnected
  case STATE_DISCONNECTED:
  default:
    // not connected yet so check for connections
    if (!SerialComs::isConnected()) {
      if (!SerialComs::checkSerial()) {
        // no connection found just continue waiting
        break;
      }
    }
    // a connection was found, say hello
    m_state = STATE_GREETING;
    break;

  // -------------------------------
  //  Send Greeting
  case STATE_GREETING:
    // send the hello greeting with our version number and build time
    SerialComs::write(EDITOR_VERB_GREETING);
    m_state = STATE_IDLE;
    break;

  // -------------------------------
  //  Chillin
  case STATE_IDLE:
    if (rv == RV_FAIL) {
      // handle failure from before, reset rv
      rv = RV_OK;
      // clear the buffer I guess
      m_receiveBuffer.clear();
    }
    // parse the receive buffer for any commands from the editor
    handleCommand();
    // watch for disconnects
    if (!SerialComs::isConnectedReal()) {
      Leds::holdAll(RGB_RED);
      leaveMenu(true);
    }
    break;

  // -------------------------------
  //  Send Modes to PC
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
    // send our acknowledgement that the modes were sent
    SerialComs::write(EDITOR_VERB_PULL_MODES_ACK);
    // go idle
    m_state = STATE_IDLE;
    break;

  // -------------------------------
  //  Receive Modes from PC
  case STATE_PUSH_MODES:
    // editor requested to push modes, clear first and reset first
    // now say we are ready
    SerialComs::write(EDITOR_VERB_READY);
    // move to receiving
    m_state = STATE_PUSH_MODES_RECEIVE;
    break;
  case STATE_PUSH_MODES_RECEIVE:
    // receive the modes into the receive buffer
    if (receiveModes() == RV_WAIT) {
      // just keep waiting
      break;
    }
    // success modes were received send the done
    m_state = STATE_PUSH_MODES_DONE;
    break;
  case STATE_PUSH_MODES_DONE:
    // say we are done
    SerialComs::write(EDITOR_VERB_PUSH_MODES_ACK);
    m_state = STATE_IDLE;
    break;

  // -------------------------------
  //  Demo Mode from PC
  case STATE_DEMO_MODE:
    // editor requested to push modes, clear first and reset first
    // now say we are ready
    SerialComs::write(EDITOR_VERB_READY);
    // move to receiving
    m_state = STATE_DEMO_MODE_RECEIVE;
    break;
  case STATE_DEMO_MODE_RECEIVE:
    // receive the modes into the receive buffer
    if (receiveDemoMode() == RV_WAIT) {
      // just wait
      break;
    }
    // success modes were received send the done
    m_state = STATE_DEMO_MODE_DONE;
    break;
  case STATE_DEMO_MODE_DONE:
    // say we are done
    SerialComs::write(EDITOR_VERB_DEMO_MODE_ACK);
    m_state = STATE_IDLE;
    break;

  // -------------------------------
  //  Reset Demo to Nothing
  case STATE_CLEAR_DEMO:
    clearDemo();
    SerialComs::write(EDITOR_VERB_CLEAR_DEMO_ACK);
    m_state = STATE_IDLE;
    break;

  // -------------------------------
  //  Send Mode to Duo
  case STATE_TRANSMIT_MODE_VL:
#if VL_ENABLE_SENDER == 1
    // if still sending and the send command indicated more data
    if (VLSender::isSending() && VLSender::send()) {
      // then continue sending
      break;
    }
#endif
    // othewrise, done, switch to the transmit done state
    m_state = STATE_TRANSMIT_MODE_VL_DONE;
    break;
  case STATE_TRANSMIT_MODE_VL_DONE:
    // done transmitting
    SerialComs::write(EDITOR_VERB_TRANSMIT_VL_ACK);
    m_state = STATE_IDLE;
    break;

  // -------------------------------
  //  Send Modes to PC Safer
  case STATE_PULL_EACH_MODE:
    // editor requested pull modes, send the modes
    sendModeCount();
    m_state = STATE_PULL_EACH_MODE_COUNT;
    break;
  case STATE_PULL_EACH_MODE_COUNT:
    if (receiveMessage(EDITOR_VERB_PULL_EACH_MODE_ACK)) {
      if (Modes::numModes() == 0) {
        m_state = STATE_PULL_EACH_MODE_DONE;
      } else {
        m_previousModeIndex = Modes::curModeIndex();
        m_state = STATE_PULL_EACH_MODE_SEND;
      }
    }
    break;
  case STATE_PULL_EACH_MODE_SEND:
    // send the current mode
    sendCurMode();
    // wait for the ack
    m_state = STATE_PULL_EACH_MODE_WAIT;
    break;
  case STATE_PULL_EACH_MODE_WAIT:
    // recive the ack from the editor to send next mode
    if (receiveMessage(EDITOR_VERB_PULL_EACH_MODE_ACK)) {
      // if there is still more modes
      if (Modes::curModeIndex() < (Modes::numModes() - 1)) {
        // then iterate to the next mode and send
        Modes::nextMode();
        m_state = STATE_PULL_EACH_MODE_SEND;
      } else {
        // otherwise done sending modes
        m_state = STATE_PULL_EACH_MODE_DONE;
      }
    }
    break;
  case STATE_PULL_EACH_MODE_DONE:
    // send our acknowledgement that the modes were sent
    SerialComs::write(EDITOR_VERB_PULL_EACH_MODE_DONE);
    // switch back to the previous mode
    Modes::setCurMode(m_previousModeIndex);
    // go idle
    m_state = STATE_IDLE;
    break;

  // -------------------------------
  //  Receive Modes from PC Safer
  case STATE_PUSH_EACH_MODE:
    // editor requested to push modes, find out how many
    // ack the command and wait for the amount of modes
    SerialComs::write(EDITOR_VERB_PUSH_EACH_MODE_ACK);
    m_state = STATE_PUSH_EACH_MODE_COUNT;
    break;
  case STATE_PUSH_EACH_MODE_COUNT:
    if (receiveModeCount()) {
      // clear modes and start receiving
      Modes::clearModes();
      // write out an ack
      SerialComs::write(EDITOR_VERB_PUSH_EACH_MODE_ACK);
      // ready to receive a mode
      m_state = STATE_PUSH_EACH_MODE_RECEIVE;
    }
    break;
  case STATE_PUSH_EACH_MODE_RECEIVE:
    // receive the modes into the receive buffer
    if (receiveMode()) {
      SerialComs::write(EDITOR_VERB_PUSH_EACH_MODE_ACK);
      if (m_numModesToReceive > 0) {
        m_numModesToReceive--;
      }
      if (!m_numModesToReceive) {
        // success modes were received send the done
        m_state = STATE_PUSH_EACH_MODE_DONE;
      }
    }
    break;
  case STATE_PUSH_EACH_MODE_DONE:
    // did originally receive/send a DONE message here but it wasn't working
    // on lightshow.lol so just skip to IDLE
    m_state = STATE_IDLE;
    break;

  // -------------------------------
  //  Set Global Brightness
  case STATE_SET_GLOBAL_BRIGHTNESS:
    SerialComs::write(EDITOR_VERB_READY);
    m_state = STATE_SET_GLOBAL_BRIGHTNESS_RECEIVE;
    break;
  case STATE_SET_GLOBAL_BRIGHTNESS_RECEIVE:
    // set the brightness of the device
    rv = receiveBrightness();
    if (rv == RV_WAIT) {
      // just keep waiting
      break;
    }
    m_state = STATE_IDLE;
    break;
  }
}

void EditorConnection::sendCurModeVL()
{
#if VL_ENABLE_SENDER == 1
  // immediately load the mode and send it now
  VLSender::loadMode(&m_previewMode);
  VLSender::send();
#endif
  m_state = STATE_TRANSMIT_MODE_VL;
}

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

// handlers for clicks
void EditorConnection::leaveMenu(bool doSave)
{
  SerialComs::write(EDITOR_VERB_GOODBYE);
  Menu::leaveMenu(true);
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
  } else if (receiveMessage(EDITOR_VERB_SET_GLOBAL_BRIGHTNESS)) {
    m_state = STATE_SET_GLOBAL_BRIGHTNESS;
  }
}

void EditorConnection::showEditor()
{
  switch (m_state) {
  case STATE_DISCONNECTED:
    Leds::clearAll();
    Leds::blinkAll(250, 150, RGB_WHITE0);
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

void EditorConnection::sendModeCount()
{
  ByteStream buffer;
  buffer.serialize8(Modes::numModes());
  SerialComs::write(buffer);
}

void EditorConnection::sendCurMode()
{
  ByteStream modeBuffer;
  Mode *cur = Modes::curMode();
  if (!cur) {
    // ??
    return;
  }
  if (!cur->saveToBuffer(modeBuffer)) {
    // ??
    return;
  }
  SerialComs::write(modeBuffer);
}

ReturnCode EditorConnection::receiveBuffer(ByteStream &buffer)
{
  // need at least the buffer size first
  uint32_t size = 0;
  if (m_receiveBuffer.size() < sizeof(size)) {
    // wait, not enough data available yet
    return RV_WAIT;
  }
  // grab the size out of the start
  m_receiveBuffer.resetUnserializer();
  size = m_receiveBuffer.peek32();
  if (m_receiveBuffer.size() < (size + sizeof(size))) {
    // don't unserialize yet, not ready
    return RV_WAIT;
  }
  // okay unserialize now, first unserialize the size
  if (!m_receiveBuffer.consume32(&size)) {
    // this shouldn't fail!
    return RV_FAIL;
  }
  // create a new ByteStream that will hold the full buffer of data
  if (!buffer.init(m_receiveBuffer.rawSize())) {
    Leds::holdAll(RGB_YELLOW);
    return RV_FAIL;
  }
  // then copy everything from the receive buffer into the rawdata
  // which is going to overwrite the crc/size/flags of the ByteStream
  //memcpy(buffer.rawData(), m_receiveBuffer.data(), m_receiveBuffer.size());
  if (!m_receiveBuffer.consume(buffer.rawData(), m_receiveBuffer.size())) {
    Leds::holdAll(RGB_CYAN);
    return RV_FAIL;
  }
  // clear the receive buffer
  //m_receiveBuffer.clear();
  if (!buffer.checkCRC()) {
    // TODO: this needs a different return value or something, usually false
    // just means keep listening but in this case it listened and received bad
    // data so we need to report this somehow
    Leds::holdAll(RGB_RED);
    buffer.clear();
    // return true otherwise we'll get locked in this state forever
    return RV_FAIL;
  }
  return RV_OK;
}

ReturnCode EditorConnection::receiveModes()
{
  // create a new ByteStream that will hold the full buffer of data
  ByteStream buf;
  ReturnCode rv = receiveBuffer(buf);
  if (rv != RV_OK) {
    return rv;
  }
  if (!Modes::loadFromBuffer(buf) || !Modes::saveStorage()) {
    return RV_FAIL;
  }
  return RV_OK;
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
  if (!m_receiveBuffer.consume32(&size)) {
    return false;
  }
  // create a new ByteStream that will hold the full buffer of data
  ByteStream buf(m_receiveBuffer.rawSize());
  // then copy everything from the receive buffer into the rawdata
  // which is going to overwrite the crc/size/flags of the ByteStream
  //memcpy(buf.rawData(), m_receiveBuffer.data(), m_receiveBuffer.size());
  m_receiveBuffer.consume(buf.rawData(), m_receiveBuffer.size());
  // unserialize the mode count
  if (!buf.consume8(&m_numModesToReceive)) {
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
  if (!m_receiveBuffer.consume32(&size)) {
    return false;
  }
  // create a new ByteStream that will hold the full buffer of data
  ByteStream buf(m_receiveBuffer.rawSize());
  // then copy everything from the receive buffer into the rawdata
  // which is going to overwrite the crc/size/flags of the ByteStream
  //memcpy(buf.rawData(), m_receiveBuffer.data(), m_receiveBuffer.size());
  m_receiveBuffer.consume(buf.rawData(), m_receiveBuffer.size());
  // clear the receive buffer
  //m_receiveBuffer.clear();
  // unserialize the mode into the demo mode
  if (!Modes::addModeFromBuffer(buf)) {
    // error
  }
  return true;
}

ReturnCode EditorConnection::receiveDemoMode()
{
  // create a new ByteStream that will hold the full buffer of data
  ByteStream buf;
  ReturnCode rv = receiveBuffer(buf);
  if (rv != RV_OK) {
    return rv;
  }
  // unserialize the mode into the demo mode
  if (!m_previewMode.loadFromBuffer(buf)) {
    // failure
    return RV_FAIL;
  }
  return RV_OK;
}

void EditorConnection::clearDemo()
{
  Colorset set(RGB_WHITE0);
  PatternArgs args(1, 0, 0);
  m_previewMode.setPattern(PATTERN_STROBE, LED_ALL, &args, &set);
  m_previewMode.init();
}

ReturnCode EditorConnection::receiveBrightness()
{
  // create a new ByteStream that will hold the full buffer of data
  ByteStream buf;
  ReturnCode rv = receiveBuffer(buf);
  if (rv != RV_OK) {
    // RV_WAIT or RV_FAIL
    return rv;
  }
  if (!buf.size()) {
    // failure
    return RV_FAIL;
  }
  uint8_t brightness = buf.data()[0];
  if (brightness > 0) {
    Leds::setBrightness(brightness);
  }
  return RV_OK;
}
