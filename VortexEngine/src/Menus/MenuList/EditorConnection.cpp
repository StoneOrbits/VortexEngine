#include "EditorConnection.h"

#include "../../Patterns/PatternArgs.h"
#include "../../Serial/ByteStream.h"
#include "../../Serial/Serial.h"
#include "../../Storage/Storage.h"
#include "../../Wireless/VLSender.h"
#include "../../Wireless/VLReceiver.h"
#include "../../Time/TimeControl.h"
#include "../../Time/Timings.h"
#include "../../Colors/Colorset.h"
#include "../../Modes/Modes.h"
#include "../../Modes/Mode.h"
#include "../../Leds/Leds.h"
#include "../../UPDI/updi.h"
#include "../../Log/Log.h"

#include <string.h>

EditorConnection::EditorConnection(const RGBColor &col, bool advanced) :
  Menu(col, advanced),
  m_state(STATE_DISCONNECTED),
  m_timeOutStartTime(0),
  m_chromaModeIdx(0),
  m_allowReset(true),
  m_previousModeIndex(0),
  m_numModesToReceive(0),
  m_curStep(0)
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
    if (!SerialComs::isConnected()) {
      if (!SerialComs::checkSerial()) {
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
    SerialComs::write(EDITOR_VERB_GREETING);
    m_state = STATE_IDLE;
    break;
  case STATE_IDLE:
    // parse the receive buffer for any commands from the editor
    handleCommand();
    // watch for disconnects
    if (!SerialComs::isConnectedReal()) {
      Leds::holdAll(RGB_GREEN);
      leaveMenu(true);
    }
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
    SerialComs::write(EDITOR_VERB_PULL_MODES_ACK);
    // go idle
    m_state = STATE_IDLE;
    break;
  case STATE_PUSH_MODES:
    // editor requested to push modes, clear first and reset first
    m_receiveBuffer.clear();
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
    m_receiveBuffer.clear();
    SerialComs::write(EDITOR_VERB_PUSH_MODES_ACK);
    m_state = STATE_IDLE;
    break;
  case STATE_DEMO_MODE:
    // editor requested to push modes, clear first and reset first
    m_receiveBuffer.clear();
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
    m_receiveBuffer.clear();
    SerialComs::write(EDITOR_VERB_DEMO_MODE_ACK);
    m_state = STATE_IDLE;
    break;
  case STATE_CLEAR_DEMO:
    clearDemo();
    m_receiveBuffer.clear();
    SerialComs::write(EDITOR_VERB_CLEAR_DEMO_ACK);
    m_state = STATE_IDLE;
    break;
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
    m_receiveBuffer.clear();
    SerialComs::write(EDITOR_VERB_TRANSMIT_VL_ACK);
    m_state = STATE_IDLE;
    break;
  case STATE_LISTEN_MODE_VL:
    showReceiveModeVL();
    receiveModeVL();
    break;
  case STATE_LISTEN_MODE_VL_DONE:
    // done transmitting
    m_receiveBuffer.clear();
    SerialComs::write(EDITOR_VERB_LISTEN_VL_ACK);
    m_state = STATE_IDLE;
    break;
  case STATE_PULL_HEADER_CHROMALINK:
    pullHeaderChromalink();
    m_state = STATE_PULL_HEADER_CHROMALINK_SEND;
    break;
  case STATE_PULL_HEADER_CHROMALINK_SEND:
    // recive the send modes ack from the editor (reusing the pull modes verb)
    if (receiveMessage(EDITOR_VERB_PULL_MODES_DONE)) {
      m_state = STATE_PULL_HEADER_CHROMALINK_DONE;
    }
    break;
  case STATE_PULL_HEADER_CHROMALINK_DONE:
    m_receiveBuffer.clear();
    // send our acknowledgement that the header was sent
    SerialComs::write(EDITOR_VERB_PULL_CHROMA_HDR_ACK);
    // go idle
    m_state = STATE_IDLE;
    break;
  case STATE_PULL_MODE_CHROMALINK:
    // now say we are ready
    SerialComs::write(EDITOR_VERB_READY);
    m_state = STATE_PULL_MODE_CHROMALINK_IDX;
    break;
  case STATE_PULL_MODE_CHROMALINK_IDX:
    // send the stuff
    if (!pullModeChromalink()) {
      break;
    }
    m_state = STATE_PULL_MODE_CHROMALINK_SEND;
    break;
  case STATE_PULL_MODE_CHROMALINK_SEND:
    // recive the send modes ack from the editor (reusing the pull modes verb)
    if (receiveMessage(EDITOR_VERB_PULL_MODES_DONE)) {
      m_state = STATE_PULL_MODE_CHROMALINK_DONE;
    }
    break;
  case STATE_PULL_MODE_CHROMALINK_DONE:
    m_receiveBuffer.clear();
    // send our acknowledgement that the header was sent
    SerialComs::write(EDITOR_VERB_PULL_CHROMA_MODE_ACK);
    // go idle
    m_state = STATE_IDLE;
    break;
  case STATE_PUSH_HEADER_CHROMALINK:
    // editor requested to push modes, clear first and reset first
    m_receiveBuffer.clear();
    // now say we are ready
    SerialComs::write(EDITOR_VERB_READY);
    // move to receiving
    m_state = STATE_PUSH_HEADER_CHROMALINK_RECEIVE;
    break;
  case STATE_PUSH_HEADER_CHROMALINK_RECEIVE:
    // receive the modes into the receive buffer
    if (receiveChromaHdr()) {
      // success modes were received send the done
      m_state = STATE_PUSH_HEADER_CHROMALINK_DONE;
    }
    break;
  case STATE_PUSH_HEADER_CHROMALINK_DONE:
    // say we are done
    m_receiveBuffer.clear();
    SerialComs::write(EDITOR_VERB_PUSH_CHROMA_HDR_ACK);
    m_state = STATE_IDLE;
    break;
  case STATE_PUSH_MODE_CHROMALINK:
    // editor requested to push modes, clear first and reset first
    m_receiveBuffer.clear();
    // now say we are ready
    SerialComs::write(EDITOR_VERB_READY);
    // move to receiving
    m_state = STATE_PUSH_MODE_CHROMALINK_RECEIVE_IDX;
    break;
  case STATE_PUSH_MODE_CHROMALINK_RECEIVE_IDX:
    if (!receiveModeIdx(m_chromaModeIdx)) {
      break;
    }
    m_receiveBuffer.clear();
    SerialComs::write(EDITOR_VERB_READY);
    m_state = STATE_PUSH_MODE_CHROMALINK_RECEIVE;
    break;
  case STATE_PUSH_MODE_CHROMALINK_RECEIVE:
    if (!receiveChromaMode()) {
      break;
    }
    m_state = STATE_PUSH_MODE_CHROMALINK_DONE;
    break;
  case STATE_PUSH_MODE_CHROMALINK_DONE:
    // say we are done
    m_receiveBuffer.clear();
    SerialComs::write(EDITOR_VERB_PUSH_CHROMA_MODE_ACK);
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
      if (Modes::numModes() == 0) {
        m_state = STATE_PULL_EACH_MODE_DONE;
      } else {
        m_previousModeIndex = Modes::curModeIndex();
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
    m_receiveBuffer.clear();
    // send our acknowledgement that the modes were sent
    SerialComs::write(EDITOR_VERB_PULL_EACH_MODE_DONE);
    // switch back to the previous mode
    Modes::setCurMode(m_previousModeIndex);
    // go idle
    m_state = STATE_IDLE;
    break;
  case STATE_PUSH_EACH_MODE:
    // editor requested to push modes, find out how many
    m_receiveBuffer.clear();
    // ack the command and wait for the amount of modes
    SerialComs::write(EDITOR_VERB_PUSH_EACH_MODE_ACK);
    m_state = STATE_PUSH_EACH_MODE_COUNT;
    break;
  case STATE_PUSH_EACH_MODE_COUNT:
    if (receiveModeCount()) {
      // clear modes and start receiving
      Modes::clearModes();
      // write out an ack
      m_receiveBuffer.clear();
      SerialComs::write(EDITOR_VERB_PUSH_EACH_MODE_ACK);
      // ready to receive a mode
      m_state = STATE_PUSH_EACH_MODE_RECEIVE;
    }
    break;
  case STATE_PUSH_EACH_MODE_RECEIVE:
    // receive the modes into the receive buffer
    if (receiveMode()) {
      m_receiveBuffer.clear();
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
  }
  return MENU_CONTINUE;
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

void EditorConnection::listenModeVL()
{
#if VL_ENABLE_SENDER == 1
  // immediately load the mode and send it now
  VLReceiver::beginReceiving();
#endif
  m_state = STATE_LISTEN_MODE_VL;
}

bool EditorConnection::pullHeaderChromalink()
{
  // first read the duo save header
  ByteStream saveHeader;
  m_curStep = 0;
  Leds::setAll(RGB_YELLOW2);
  if (!UPDI::readHeader(saveHeader)) {
    Leds::setIndex((LedPos)m_curStep++, RGB_RED3);
    Leds::update();
    return false;
  }
  Leds::setIndex((LedPos)m_curStep++, RGB_GREEN3);
  Leds::update();
  if (!saveHeader.size() || !saveHeader.checkCRC()) {
    Leds::setIndex((LedPos)m_curStep++, RGB_RED3);
  } else {
    Leds::setIndex((LedPos)m_curStep++, RGB_GREEN3);
  }
  Leds::update();
  SerialComs::write(saveHeader);
  Leds::setIndex((LedPos)m_curStep++, RGB_GREEN3);
  Leds::update();
  return true;
}

bool EditorConnection::pushHeaderChromalink()
{
  return true;
}

// pull/push through the chromalink
bool EditorConnection::pullModeChromalink()
{
  // try to receive the mode index
  uint8_t modeIdx = 0;
  // only 9 modes on duo, maybe this should be a macro or something
  if (!receiveModeIdx(modeIdx) || modeIdx >= 9) {
    Leds::setIndex((LedPos)m_curStep++, RGB_RED3);
    Leds::update();
    return false;
  }
  ByteStream modeBuffer;
  if (!UPDI::readMode(modeIdx, modeBuffer)) {
    Leds::setIndex((LedPos)m_curStep++, RGB_RED3);
    Leds::update();
    return false;
  }
  Leds::setIndex((LedPos)m_curStep++, RGB_GREEN3);
  Leds::update();
  SerialComs::write(modeBuffer);
  return true;
}

bool EditorConnection::pushModeChromalink()
{
  return true;
}

void EditorConnection::onShortClickM()
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

void EditorConnection::onLongClickM()
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
  } else if (receiveMessage(EDITOR_VERB_LISTEN_VL)) {
    listenModeVL();
  } else if (receiveMessage(EDITOR_VERB_PULL_CHROMA_HDR)) {
    m_state = STATE_PULL_HEADER_CHROMALINK;
  } else if (receiveMessage(EDITOR_VERB_PUSH_CHROMA_HDR)) {
    m_state = STATE_PUSH_HEADER_CHROMALINK;
  } else if (receiveMessage(EDITOR_VERB_PULL_CHROMA_MODE)) {
    m_state = STATE_PULL_MODE_CHROMALINK;
  } else if (receiveMessage(EDITOR_VERB_PUSH_CHROMA_MODE)) {
    m_state = STATE_PUSH_MODE_CHROMALINK;
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

bool EditorConnection::receiveBuffer(ByteStream &buffer)
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
  buffer.init(m_receiveBuffer.rawSize());
  // then copy everything from the receive buffer into the rawdata
  // which is going to overwrite the crc/size/flags of the ByteStream
  memcpy(buffer.rawData(), m_receiveBuffer.data() + sizeof(size),
    m_receiveBuffer.size() - sizeof(size));
  // clear the receive buffer
  m_receiveBuffer.clear();
  return true;
}

bool EditorConnection::receiveModes()
{
  // create a new ByteStream that will hold the full buffer of data
  ByteStream buf;
  if (!receiveBuffer(buf)) {
    return false;
  }
  Modes::loadFromBuffer(buf);
  Modes::saveStorage();
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
  if (!Modes::addModeFromBuffer(buf)) {
    // error
  }
  return true;
}

bool EditorConnection::receiveDemoMode()
{
  // create a new ByteStream that will hold the full buffer of data
  ByteStream buf;
  if (!receiveBuffer(buf)) {
    return false;
  }
  // unserialize the mode into the demo mode
  if (!m_previewMode.loadFromBuffer(buf)) {
    // failure
  }
  return true;
}

void EditorConnection::clearDemo()
{
  Colorset set(RGB_WHITE0);
  PatternArgs args(1, 0, 0);
  m_previewMode.setPattern(PATTERN_STROBE, LED_ALL, &args, &set);
  m_previewMode.init();
}

void EditorConnection::receiveModeVL()
{
  // if reveiving new data set our last data time
  if (VLReceiver::onNewData()) {
    m_timeOutStartTime = Time::getCurtime();
    // if our last data was more than time out duration reset the recveiver
  } else if (m_timeOutStartTime > 0 && (m_timeOutStartTime + MAX_TIMEOUT_DURATION) < Time::getCurtime()) {
    VLReceiver::resetVLState();
    m_timeOutStartTime = 0;
    return;
  }
  // check if the VLReceiver has a full packet available
  if (!VLReceiver::dataReady()) {
    // nothing available yet
    return;
  }
  DEBUG_LOG("Mode ready to receive! Receiving...");
  // receive the VL mode into the current mode
  if (!VLReceiver::receiveMode(&m_previewMode)) {
    ERROR_LOG("Failed to receive mode");
    return;
  }
  DEBUG_LOGF("Success receiving mode: %u", m_previewMode.getPatternID());
  Modes::updateCurMode(&m_previewMode);
  ByteStream modeBuffer;
  m_previewMode.saveToBuffer(modeBuffer);
  SerialComs::write(modeBuffer);
  m_state = STATE_LISTEN_MODE_VL_DONE;
}

void EditorConnection::showReceiveModeVL()
{
  if (VLReceiver::isReceiving()) {
    // using uint32_t to avoid overflow, the result should be within 10 to 255
    //Leds::setAll(RGBColor(0, VLReceiver::percentReceived(), 0));
    Leds::setRange(LED_0, (LedPos)(VLReceiver::percentReceived() / 10), RGB_GREEN6);
    Leds::setRange(LED_10, (LedPos)(LED_10 + (VLReceiver::percentReceived() / 10)), RGB_GREEN6);
  } else {
    Leds::setAll(RGB_WHITE0);
  }
}

bool EditorConnection::receiveModeIdx(uint8_t &idx)
{
  // need at least the buffer size first
  if (m_receiveBuffer.size() < sizeof(idx)) {
    // wait, not enough data available yet
    return false;
  }
  m_receiveBuffer.resetUnserializer();
  // okay unserialize now, first unserialize the size
  if (!m_receiveBuffer.unserialize8(&idx)) {
    return false;
  }
  return true;
}

bool EditorConnection::receiveChromaHdr()
{
  // create a new ByteStream that will hold the full buffer of data
  ByteStream buf;
  if (!receiveBuffer(buf)) {
    return false;
  }
  return UPDI::writeHeader(buf);
}

bool EditorConnection::receiveChromaMode()
{
  // create a new ByteStream that will hold the full buffer of data
  ByteStream buf;
  if (!receiveBuffer(buf)) {
    return false;
  }
  return UPDI::writeMode(m_chromaModeIdx, buf);
}
