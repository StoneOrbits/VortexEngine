#include "EditorConnection.h"
#include "../Menus.h"

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
#include "../../Memory/Memory.h"

#include <string.h>

typedef struct CommandState
{
  const char *cmd;
  EditorConnectionState cmdState;
} CommandState;

static const CommandState s_commands[] = {
  { EDITOR_VERB_PULL_MODES, STATE_PULL_MODES },
  { EDITOR_VERB_PUSH_MODES, STATE_PUSH_MODES },
  { EDITOR_VERB_DEMO_MODE, STATE_DEMO_MODE },
  { EDITOR_VERB_CLEAR_DEMO, STATE_CLEAR_DEMO },
  { EDITOR_VERB_PULL_EACH_MODE, STATE_PULL_EACH_MODE },
  { EDITOR_VERB_PUSH_EACH_MODE, STATE_PUSH_EACH_MODE },
  { EDITOR_VERB_TRANSMIT_VL, STATE_TRANSMIT_MODE_VL },
  { EDITOR_VERB_SET_GLOBAL_BRIGHTNESS, STATE_SET_GLOBAL_BRIGHTNESS },
  { EDITOR_VERB_GET_GLOBAL_BRIGHTNESS, STATE_GET_GLOBAL_BRIGHTNESS },
};
#define NUM_COMMANDS (sizeof(s_commands) / sizeof(s_commands[0]))

static void EditorConnection_destroyHook(void *self);
static bool EditorConnection_initHook(void *self);
static MenuAction EditorConnection_runHook(void *self);
static void EditorConnection_onShortClickHook(void *self);
static void EditorConnection_onLongClickHook(void *self);
static void EditorConnection_leaveMenuHook(void *self, bool doSave);

const MenuVtable EditorConnection_vtable = {
  EditorConnection_initHook,
  EditorConnection_runHook,
  NULL,
  EditorConnection_onShortClickHook,
  EditorConnection_onLongClickHook,
  EditorConnection_leaveMenuHook,
  EditorConnection_destroyHook,
};

EditorConnection *EditorConnection_new(const RGBColor *col, bool advanced)
{
  EditorConnection *self = (EditorConnection *)Menus_allocMenu();
  if (!self) {
    return NULL;
  }
  Menu_initBase(&self->m_base, &EditorConnection_vtable);
  RGBColor_setRGB(&self->m_base.m_menuColor, col);
  self->m_base.m_advanced = advanced;
  self->m_state = STATE_DISCONNECTED;
  self->m_allowReset = true;
  self->m_previousModeIndex = 0;
  self->m_numModesToReceive = 0;
  self->m_rv = RV_OK;
  ByteStream_make(&self->m_receiveBuffer);
  return self;
}

void EditorConnection_destroy(EditorConnection *self)
{
  EditorConnection_clearDemo(self);
  ByteStream_destroy(&self->m_receiveBuffer);
  Menu_baseDestroy(&self->m_base);
}

bool EditorConnection_init(EditorConnection *self)
{
  Menu *menu = &self->m_base;
  if (!Menu_baseInit(menu)) {
    return false;
  }
  // skip led selection
  menu->m_ledSelected = true;
  EditorConnection_clearDemo(self);
  DEBUG_LOG("Entering Editor Connection");
  return true;
}

MenuAction EditorConnection_run(EditorConnection *self)
{
  Menu *menu = &self->m_base;
  MenuAction result = Menu_runBase(menu);
  if (result != MENU_CONTINUE) {
    return result;
  }
  // TODO: auto leave the editor menu when unplugged
  // show the editor
  EditorConnection_showEditor(self);
  // receive any data from serial into the receive buffer
  EditorConnection_receiveData(self);
  // handle the current state
  EditorConnection_handleState(self);
  return MENU_CONTINUE;
}

void EditorConnection_onShortClick(EditorConnection *self)
{
  // if the device has received any commands do not reset!
  if (!self->m_allowReset) {
    return;
  }
  // reset, this won't actually disconnect the com port
  self->m_state = STATE_DISCONNECTED;
  // clear the demo
  EditorConnection_clearDemo(self);
  // sent a reset, do not allow another
  self->m_allowReset = false;
}

void EditorConnection_onLongClick(EditorConnection *self)
{
  EditorConnection_leaveMenu(self, true);
}

void EditorConnection_leaveMenu(EditorConnection *self, bool doSave)
{
  (void)doSave;
  SerialComs_write(EDITOR_VERB_GOODBYE);
  Menu_leaveMenuBase(&self->m_base, true);
}

void EditorConnection_clearDemo(EditorConnection *self)
{
  Menu *menu = &self->m_base;
  Colorset set;
  Colorset_make1(&set, RGB_WHITE0);
  PatternArgs args;
  PatternArgs_init3(&args, 1, 0, 0);
  Mode_setPattern(&menu->m_previewMode, PATTERN_STROBE, LED_ALL, &args, &set);
  Mode_initPatterns(&menu->m_previewMode);
}

void EditorConnection_handleErrors(EditorConnection *self)
{
  if (self->m_rv == RV_FAIL) {
    // handle failure from before, reset rv
    self->m_rv = RV_OK;
    // clear the buffer I guess
    ByteStream_clear(&self->m_receiveBuffer);
  }
  // TODO: Custom error codes?
}

void EditorConnection_handleCommand(EditorConnection *self)
{
  if (ByteStream_size(&self->m_receiveBuffer) < 1) {
    return;
  }
  for (uint32_t i = 0; i < NUM_COMMANDS; ++i) {
    // commands are just one letter, don't consume it yet so we can compare
    // each cmd in the list, this probably should be a lookup table
    char receiveCmd = (char)ByteStream_data(&self->m_receiveBuffer)[0];
    // if the command matches
    if (receiveCmd != s_commands[i].cmd[0]) {
      continue;
    }
    // then consume the data from the receive buffer and update the state
    ByteStream_consume8(&self->m_receiveBuffer, NULL);
    self->m_state = s_commands[i].cmdState;
    // we have now received at least one command, do not allow resetting
    self->m_allowReset = false;
    // don't need to check any more commands
    break;
  }
}

void EditorConnection_handleState(EditorConnection *self)
{
  Menu *menu = &self->m_base;
  // operate on the state of the editor connection
  switch (self->m_state) {
  // -------------------------------
  //  Disconnected
  case STATE_DISCONNECTED:
  default:
    // not connected yet so check for connections
    if (!SerialComs_isConnected()) {
      if (!SerialComs_checkSerial()) {
        // no connection found just continue waiting
        break;
      }
    }
    // a connection was found, say hello
    self->m_state = STATE_GREETING;
    break;

  // -------------------------------
  //  Send Greeting
  case STATE_GREETING:
    // send the hello greeting with our version number and build time
    SerialComs_write(EDITOR_VERB_GREETING);
    self->m_state = STATE_IDLE;
    break;

  // -------------------------------
  //  Chillin
  case STATE_IDLE:
    // handle any errors that may have occurred last run
    EditorConnection_handleErrors(self);
    // parse the receive buffer for any commands from the editor
    EditorConnection_handleCommand(self);
    // watch for disconnects
    if (!SerialComs_isConnected()) {
      Leds_holdAll(RGB_RED);
      EditorConnection_leaveMenu(self, true);
    }
    break;

  // -------------------------------
  //  Send Modes to PC
  case STATE_PULL_MODES:
    // editor requested pull modes, send the modes
    EditorConnection_sendModes(self);
    self->m_state = STATE_PULL_MODES_SEND;
    break;
  case STATE_PULL_MODES_SEND:
    // recive the send modes ack from the editor
    if (EditorConnection_receiveMessage(self, EDITOR_VERB_PULL_MODES_DONE) == RV_WAIT) {
      // just wait
      break;
    }
    self->m_state = STATE_PULL_MODES_DONE;
    break;
  case STATE_PULL_MODES_DONE:
    // send our acknowledgement that the modes were sent
    SerialComs_write(EDITOR_VERB_PULL_MODES_ACK);
    // go idle
    self->m_state = STATE_IDLE;
    break;

  // -------------------------------
  //  Receive Modes from PC
  case STATE_PUSH_MODES:
    // now say we are ready
    SerialComs_write(EDITOR_VERB_READY);
    // move to receiving
    self->m_state = STATE_PUSH_MODES_RECEIVE;
    break;
  case STATE_PUSH_MODES_RECEIVE:
    // receive the modes into the receive buffer
    if (EditorConnection_receiveModes(self) == RV_WAIT) {
      // just keep waiting
      break;
    }
    // success modes were received send the done
    self->m_state = STATE_PUSH_MODES_DONE;
    break;
  case STATE_PUSH_MODES_DONE:
    SerialComs_write(EDITOR_VERB_PUSH_MODES_ACK);
    self->m_state = STATE_IDLE;
    break;

  // -------------------------------
  //  Demo Mode from PC
  case STATE_DEMO_MODE:
    // now say we are ready
    SerialComs_write(EDITOR_VERB_READY);
    // move to receiving
    self->m_state = STATE_DEMO_MODE_RECEIVE;
    break;
  case STATE_DEMO_MODE_RECEIVE:
    // receive the modes into the receive buffer
    if (EditorConnection_receiveDemoMode(self) == RV_WAIT) {
      // just wait
      break;
    }
    // success modes were received send the done
    self->m_state = STATE_DEMO_MODE_DONE;
    break;
  case STATE_DEMO_MODE_DONE:
    // say we are done
    SerialComs_write(EDITOR_VERB_DEMO_MODE_ACK);
    self->m_state = STATE_IDLE;
    break;

  // -------------------------------
  //  Reset Demo to Nothing
  case STATE_CLEAR_DEMO:
    EditorConnection_clearDemo(self);
    SerialComs_write(EDITOR_VERB_CLEAR_DEMO_ACK);
    self->m_state = STATE_IDLE;
    break;

  // -------------------------------
  //  Send Mode to Duo
  case STATE_TRANSMIT_MODE_VL:
#if VL_ENABLE_SENDER == 1
    // immediately load the mode and send it now
    VLSender_send(&menu->m_previewMode);
#endif
    self->m_state = STATE_TRANSMIT_MODE_VL_DONE;
    break;
  case STATE_TRANSMIT_MODE_VL_DONE:
    // done transmitting
    SerialComs_write(EDITOR_VERB_TRANSMIT_VL_ACK);
    self->m_state = STATE_IDLE;
    break;

  // -------------------------------
  //  Send Modes to PC Safer
  case STATE_PULL_EACH_MODE:
    // editor requested pull modes, send the modes
    EditorConnection_sendModeCount(self);
    self->m_state = STATE_PULL_EACH_MODE_COUNT;
    break;
  case STATE_PULL_EACH_MODE_COUNT:
    if (EditorConnection_receiveMessage(self, EDITOR_VERB_PULL_EACH_MODE_ACK) == RV_WAIT) {
      // just wait
      break;
    }
    if (Modes_numModes() == 0) {
      self->m_state = STATE_PULL_EACH_MODE_DONE;
    } else {
      self->m_previousModeIndex = Modes_curModeIndex();
      self->m_state = STATE_PULL_EACH_MODE_SEND;
    }
    break;
  case STATE_PULL_EACH_MODE_SEND:
    // send the current mode
    EditorConnection_sendCurMode(self);
    // wait for the ack
    self->m_state = STATE_PULL_EACH_MODE_WAIT;
    break;
  case STATE_PULL_EACH_MODE_WAIT:
    // recive the ack from the editor to send next mode
    if (EditorConnection_receiveMessage(self, EDITOR_VERB_PULL_EACH_MODE_ACK) == RV_WAIT) {
      // just wait
      break;
    }
    // if there is still more modes
    if (Modes_curModeIndex() < (Modes_numModes() - 1)) {
      // then iterate to the next mode and send
      Modes_nextMode();
      self->m_state = STATE_PULL_EACH_MODE_SEND;
    } else {
      // otherwise done sending modes
      self->m_state = STATE_PULL_EACH_MODE_DONE;
    }
    break;
  case STATE_PULL_EACH_MODE_DONE:
    // send our acknowledgement that the modes were sent
    SerialComs_write(EDITOR_VERB_PULL_EACH_MODE_DONE);
    // switch back to the previous mode
    Modes_setCurMode(self->m_previousModeIndex);
    // go idle
    self->m_state = STATE_IDLE;
    break;

  // -------------------------------
  //  Receive Modes from PC Safer
  case STATE_PUSH_EACH_MODE:
    // editor requested to push modes, find out how many
    // ack the command and wait for the amount of modes
    SerialComs_write(EDITOR_VERB_PUSH_EACH_MODE_ACK);
    self->m_state = STATE_PUSH_EACH_MODE_COUNT;
    break;
  case STATE_PUSH_EACH_MODE_COUNT:
    if (EditorConnection_receiveModeCount(self) == RV_WAIT) {
      // just wait
      break;
    }
    // clear modes and start receiving
    Modes_clearModes();
    // write out an ack
    SerialComs_write(EDITOR_VERB_PUSH_EACH_MODE_ACK);
    // ready to receive a mode
    self->m_state = STATE_PUSH_EACH_MODE_RECEIVE;
    break;
  case STATE_PUSH_EACH_MODE_RECEIVE:
    // receive the modes into the receive buffer
    if (EditorConnection_receiveMode(self) == RV_WAIT) {
      // just wait
      break;
    }
    SerialComs_write(EDITOR_VERB_PUSH_EACH_MODE_ACK);
    if (self->m_numModesToReceive > 0) {
      self->m_numModesToReceive--;
    }
    if (!self->m_numModesToReceive) {
      // success modes were received send the done
      self->m_state = STATE_PUSH_EACH_MODE_DONE;
    }
    break;
  case STATE_PUSH_EACH_MODE_DONE:
    // save the new modes
    Modes_saveStorage();
    // did originally receive/send a DONE message here but it wasn't working
    // on lightshow.lol so just skip to IDLE
    self->m_state = STATE_IDLE;
    break;

  // -------------------------------
  //  Set Global Brightness
  case STATE_SET_GLOBAL_BRIGHTNESS:
    SerialComs_write(EDITOR_VERB_READY);
    self->m_state = STATE_SET_GLOBAL_BRIGHTNESS_RECEIVE;
    break;
  case STATE_SET_GLOBAL_BRIGHTNESS_RECEIVE:
    // set the brightness of the device
    if (EditorConnection_receiveBrightness(self) == RV_WAIT) {
      // just keep waiting
      break;
    }
    self->m_state = STATE_IDLE;
    break;

  // -------------------------------
  //  Get Global Brightness
  case STATE_GET_GLOBAL_BRIGHTNESS:
    EditorConnection_sendBrightness(self);
    self->m_state = STATE_IDLE;
    break;
  }
}

void EditorConnection_showEditor(EditorConnection *self)
{
  Menu *menu = &self->m_base;
  switch (self->m_state) {
  case STATE_DISCONNECTED:
    Leds_clearAll();
    Leds_blinkAll(250, 150, RGB_WHITE0);
    break;
  case STATE_IDLE:
    Mode_play(&menu->m_previewMode);
    break;
  default:
    // do nothing!
    // Note if you clear the leds while selecting color
    // it may make the color selection choppy
    break;
  }
}

void EditorConnection_receiveData(EditorConnection *self)
{
  // read more data into the receive buffer
  SerialComs_read(&self->m_receiveBuffer);
}

void EditorConnection_sendModes(EditorConnection *self)
{
  ByteStream modesBuffer;
  ByteStream_make(&modesBuffer);
  Modes_saveToBuffer(&modesBuffer);
  SerialComs_writeStream(&modesBuffer);
  ByteStream_destroy(&modesBuffer);
}

void EditorConnection_sendModeCount(EditorConnection *self)
{
  ByteStream buffer;
  ByteStream_make(&buffer);
  ByteStream_serialize8(&buffer, Modes_numModes());
  SerialComs_writeStream(&buffer);
  ByteStream_destroy(&buffer);
}

void EditorConnection_sendCurMode(EditorConnection *self)
{
  ByteStream modeBuffer;
  ByteStream_make(&modeBuffer);
  Mode *cur = Modes_curMode();
  if (!cur) {
    // ??
    ByteStream_destroy(&modeBuffer);
    return;
  }
  if (!Mode_saveToBuffer(cur, &modeBuffer, 0)) {
    // ??
    ByteStream_destroy(&modeBuffer);
    return;
  }
  SerialComs_writeStream(&modeBuffer);
  ByteStream_destroy(&modeBuffer);
}

void EditorConnection_sendCurModeVL(EditorConnection *self)
{
  (void)self;
#if VL_ENABLE_SENDER == 1
  self->m_state = STATE_TRANSMIT_MODE_VL;
#endif
}

ReturnCode EditorConnection_sendBrightness(EditorConnection *self)
{
  ByteStream brightnessBuf;
  ByteStream_make(&brightnessBuf);
  if (!ByteStream_serialize8(&brightnessBuf, Leds_getBrightness())) {
    ByteStream_destroy(&brightnessBuf);
    return RV_FAIL;
  }
  SerialComs_writeStream(&brightnessBuf);
  ByteStream_destroy(&brightnessBuf);
  return RV_OK;
}

ReturnCode EditorConnection_receiveBuffer(EditorConnection *self, ByteStream *buffer)
{
  // need at least the buffer size first
  uint32_t size = 0;
  if (ByteStream_size(&self->m_receiveBuffer) < sizeof(size)) {
    // wait, not enough data available yet
    return RV_WAIT;
  }
  // grab the size out of the start
  ByteStream_resetUnserializer(&self->m_receiveBuffer);
  size = ByteStream_peek32(&self->m_receiveBuffer);
  if (ByteStream_size(&self->m_receiveBuffer) < (size + sizeof(size))) {
    // don't unserialize yet, not ready
    return RV_WAIT;
  }
  // okay unserialize now, first unserialize the size
  if (!ByteStream_consume32(&self->m_receiveBuffer, &size)) {
    // this shouldn't fail!
    return RV_FAIL;
  }
  // create a new ByteStream that will hold the full buffer of data
  if (!ByteStream_init(buffer, size)) {
    return RV_FAIL;
  }
  // then copy everything from the receive buffer into the rawdata
  // which is going to overwrite the crc/size/flags of the ByteStream
  if (!ByteStream_consume(&self->m_receiveBuffer, size, ByteStream_rawData(buffer))) {
    return RV_FAIL;
  }
  ByteStream_sanity(buffer);
  if (!ByteStream_checkCRC(buffer)) {
    ByteStream_clear(buffer);
    // return true otherwise we'll get locked in this state forever
    return RV_FAIL;
  }
  return RV_OK;
}

ReturnCode EditorConnection_receiveModes(EditorConnection *self)
{
  // create a new ByteStream that will hold the full buffer of data
  ByteStream buf;
  ByteStream_make(&buf);
  self->m_rv = EditorConnection_receiveBuffer(self, &buf);
  if (self->m_rv != RV_OK) {
    ByteStream_destroy(&buf);
    return self->m_rv;
  }
  if (!Modes_loadFromBuffer(&buf) || !Modes_saveStorage()) {
    ByteStream_destroy(&buf);
    return RV_FAIL;
  }
  ByteStream_destroy(&buf);
  return RV_OK;
}

ReturnCode EditorConnection_receiveModeCount(EditorConnection *self)
{
  ByteStream buf;
  ByteStream_make(&buf);
  self->m_rv = EditorConnection_receiveBuffer(self, &buf);
  if (self->m_rv != RV_OK) {
    ByteStream_destroy(&buf);
    return self->m_rv;
  }
  // unserialize the mode count
  if (!ByteStream_consume8(&buf, &self->m_numModesToReceive)) {
    ByteStream_destroy(&buf);
    return RV_FAIL;
  }
  if (self->m_numModesToReceive > MAX_MODES) {
    ByteStream_destroy(&buf);
    return RV_FAIL;
  }
  // good mode count
  ByteStream_destroy(&buf);
  return RV_OK;
}

ReturnCode EditorConnection_receiveMode(EditorConnection *self)
{
  ByteStream buf;
  ByteStream_make(&buf);
  self->m_rv = EditorConnection_receiveBuffer(self, &buf);
  if (self->m_rv != RV_OK) {
    ByteStream_destroy(&buf);
    return self->m_rv;
  }
  // unserialize the mode into the demo mode
  if (!Modes_addModeFromBuffer(&buf)) {
    ByteStream_destroy(&buf);
    return RV_FAIL;
  }
  ByteStream_destroy(&buf);
  return RV_OK;
}

ReturnCode EditorConnection_receiveDemoMode(EditorConnection *self)
{
  Menu *menu = &self->m_base;
  // create a new ByteStream that will hold the full buffer of data
  ByteStream buf;
  ByteStream_make(&buf);
  self->m_rv = EditorConnection_receiveBuffer(self, &buf);
  if (self->m_rv != RV_OK) {
    ByteStream_destroy(&buf);
    return self->m_rv;
  }
  // unserialize the mode into the demo mode
  if (!Mode_loadFromBuffer(&menu->m_previewMode, &buf)) {
    // failure
    ByteStream_destroy(&buf);
    return RV_FAIL;
  }
  ByteStream_destroy(&buf);
  return RV_OK;
}

ReturnCode EditorConnection_receiveMessage(EditorConnection *self, const char *message)
{
  size_t len = strlen(message);
  uint8_t byte = 0;
  // wait for the editor to ack the idle
  if (ByteStream_size(&self->m_receiveBuffer) < len) {
    return RV_WAIT;
  }
  if (memcmp(ByteStream_data(&self->m_receiveBuffer), message, len) != 0) {
    return RV_FAIL;
  }
  for (size_t i = 0; i < len; ++i) {
    if (!ByteStream_consume8(&self->m_receiveBuffer, &byte)) {
      return RV_FAIL;
    }
  }
  // we have now received at least one command, do not allow resetting
  self->m_allowReset = false;
  return RV_OK;
}

ReturnCode EditorConnection_receiveBrightness(EditorConnection *self)
{
  // create a new ByteStream that will hold the full buffer of data
  ByteStream buf;
  ByteStream_make(&buf);
  self->m_rv = EditorConnection_receiveBuffer(self, &buf);
  if (self->m_rv != RV_OK) {
    // RV_WAIT or RV_FAIL
    ByteStream_destroy(&buf);
    return self->m_rv;
  }
  if (!ByteStream_size(&buf)) {
    // failure
    ByteStream_destroy(&buf);
    return RV_FAIL;
  }
  uint8_t brightness = 255;
  if (!ByteStream_consume8(&buf, &brightness) || brightness == 0) {
    // they should never send 0 brightness
    ByteStream_destroy(&buf);
    return RV_FAIL;
  }
  if (brightness > 0) {
    Leds_setBrightness(brightness);
    Modes_saveHeader();
  }
  ByteStream_destroy(&buf);
  return RV_OK;
}

static void EditorConnection_onShortClickHook(void *self)
{
  EditorConnection_onShortClick((EditorConnection *)self);
}

static void EditorConnection_onLongClickHook(void *self)
{
  EditorConnection_onLongClick((EditorConnection *)self);
}

static void EditorConnection_leaveMenuHook(void *self, bool doSave)
{
  EditorConnection_leaveMenu((EditorConnection *)self, doSave);
}

static bool EditorConnection_initHook(void *self)
{
  return EditorConnection_init((EditorConnection *)self);
}

static MenuAction EditorConnection_runHook(void *self)
{
  return EditorConnection_run((EditorConnection *)self);
}

static void EditorConnection_destroyHook(void *self)
{
  EditorConnection_destroy((EditorConnection *)self);
}
