#include "EditorConnection.h"

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
#include <stdlib.h>

#define COL32(v) ((RGBColor){ ((v) >> 16) & 0xFF, ((v) >> 8) & 0xFF, (v) & 0xFF })

static void clearDemo(EditorConnectionMenu *self);
static void handleErrors(EditorConnectionMenu *self);
static void handleCommand(EditorConnectionMenu *self);
static void handleState(EditorConnectionMenu *self);
static void showEditor(EditorConnectionMenu *self);
static void receiveData(EditorConnectionMenu *self);
static void sendModes(EditorConnectionMenu *self);
static void sendModeCount(EditorConnectionMenu *self);
static void sendCurMode(EditorConnectionMenu *self);
static void sendCurModeVL(EditorConnectionMenu *self);
static ReturnCode sendBrightness(EditorConnectionMenu *self);
static ReturnCode receiveBuffer(EditorConnectionMenu *self, ByteStream *buffer);
static ReturnCode receiveModes(EditorConnectionMenu *self);
static ReturnCode receiveModeCount(EditorConnectionMenu *self);
static ReturnCode receiveMode(EditorConnectionMenu *self);
static ReturnCode receiveDemoMode(EditorConnectionMenu *self);
static ReturnCode receiveMessage(EditorConnectionMenu *self, const char *message);
static ReturnCode receiveBrightness(EditorConnectionMenu *self);

static const EditorConnectionCommandState s_commands[] = {
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

Menu *EditorConnectionMenu_Create(RGBColor col, bool advanced)
{
  EditorConnectionMenu *self = (EditorConnectionMenu *)malloc(sizeof(EditorConnectionMenu));
  if (!self) return NULL;
  Menu_construct(&self->base, col, advanced);
  self->base.vtable = &g_editorConnectionMenuVTable;
  self->state = STATE_DISCONNECTED;
  memset(&self->receiveBuffer, 0, sizeof(self->receiveBuffer));
  self->allowReset = true;
  self->previousModeIndex = 0;
  self->numModesToReceive = 0;
  self->rv = RV_OK;
  return &self->base;
}

void EditorConnectionMenu_destroy(Menu *self)
{
  EditorConnectionMenu *ec = (EditorConnectionMenu *)self;
  clearDemo(ec);
}

bool EditorConnectionMenu_init(Menu *self)
{
  if (!Menu_init(self)) {
    return false;
  }
  self->ledSelected = true;
  EditorConnectionMenu *ec = (EditorConnectionMenu *)self;
  clearDemo(ec);
  DEBUG_LOG("Entering Editor Connection");
  return true;
}

MenuAction EditorConnectionMenu_run(Menu *self)
{
  MenuAction result = Menu_run(self);
  if (result != MENU_CONTINUE) {
    return result;
  }
  EditorConnectionMenu *ec = (EditorConnectionMenu *)self;
  showEditor(ec);
  receiveData(ec);
  handleState(ec);
  return MENU_CONTINUE;
}

void EditorConnectionMenu_onShortClick(Menu *self)
{
  EditorConnectionMenu *ec = (EditorConnectionMenu *)self;
  if (!ec->allowReset) {
    return;
  }
  ec->state = STATE_DISCONNECTED;
  clearDemo(ec);
  ec->allowReset = false;
}

void EditorConnectionMenu_onLongClick(Menu *self)
{
  Menu_leaveMenu(self, true);
}

void EditorConnectionMenu_leaveMenu(Menu *self, bool doSave)
{
  (void)doSave;
  SerialComs_write(EDITOR_VERB_GOODBYE);
  Menu_leaveMenu(self, true);
}

static void clearDemo(EditorConnectionMenu *self)
{
  Colorset set;
  Colorset_init(&set);
  Colorset_addColor(&set, COL32(RGB_WHITE0));
  PatternArgs args;
  PatternArgs_init3(&args, 1, 0, 0);
  Mode_setPattern(&self->base.previewMode, PATTERN_STROBE, LED_ALL, &args, &set);
  Mode_init(&self->base.previewMode);
}

static void handleErrors(EditorConnectionMenu *self)
{
  if (self->rv == RV_FAIL) {
    self->rv = RV_OK;
    ByteStream_clear(&self->receiveBuffer);
  }
}

static void handleCommand(EditorConnectionMenu *self)
{
  if (ByteStream_size(&self->receiveBuffer) < 1) {
    return;
  }
  for (uint32_t i = 0; i < NUM_COMMANDS; ++i) {
    const uint8_t *data = ByteStream_data(&self->receiveBuffer);
    char receiveCmd = (char)data[0];
    if (receiveCmd != s_commands[i].cmd[0]) {
      continue;
    }
    uint8_t dummy;
    ByteStream_consume8(&self->receiveBuffer, &dummy);
    self->state = s_commands[i].cmdState;
    self->allowReset = false;
    break;
  }
}

static void handleState(EditorConnectionMenu *self)
{
  switch (self->state) {
  case STATE_DISCONNECTED:
  default:
    if (!SerialComs_isConnected()) {
      if (!SerialComs_checkSerial()) {
        break;
      }
    }
    self->state = STATE_GREETING;
    break;

  case STATE_GREETING:
    SerialComs_write(EDITOR_VERB_GREETING);
    self->state = STATE_IDLE;
    break;

  case STATE_IDLE:
    handleErrors(self);
    handleCommand(self);
    if (!SerialComs_isConnected()) {
      Leds_holdAll(COL32(RGB_RED));
      Menu_leaveMenu(&self->base, true);
    }
    break;

  case STATE_PULL_MODES:
    sendModes(self);
    self->state = STATE_PULL_MODES_SEND;
    break;
  case STATE_PULL_MODES_SEND:
    if (receiveMessage(self, EDITOR_VERB_PULL_MODES_DONE) == RV_WAIT) {
      break;
    }
    self->state = STATE_PULL_MODES_DONE;
    break;
  case STATE_PULL_MODES_DONE:
    SerialComs_write(EDITOR_VERB_PULL_MODES_ACK);
    self->state = STATE_IDLE;
    break;

  case STATE_PUSH_MODES:
    SerialComs_write(EDITOR_VERB_READY);
    self->state = STATE_PUSH_MODES_RECEIVE;
    break;
  case STATE_PUSH_MODES_RECEIVE:
    if (receiveModes(self) == RV_WAIT) {
      break;
    }
    self->state = STATE_PUSH_MODES_DONE;
    break;
  case STATE_PUSH_MODES_DONE:
    SerialComs_write(EDITOR_VERB_PUSH_MODES_ACK);
    self->state = STATE_IDLE;
    break;

  case STATE_DEMO_MODE:
    SerialComs_write(EDITOR_VERB_READY);
    self->state = STATE_DEMO_MODE_RECEIVE;
    break;
  case STATE_DEMO_MODE_RECEIVE:
    if (receiveDemoMode(self) == RV_WAIT) {
      break;
    }
    self->state = STATE_DEMO_MODE_DONE;
    break;
  case STATE_DEMO_MODE_DONE:
    SerialComs_write(EDITOR_VERB_DEMO_MODE_ACK);
    self->state = STATE_IDLE;
    break;

  case STATE_CLEAR_DEMO:
    clearDemo(self);
    SerialComs_write(EDITOR_VERB_CLEAR_DEMO_ACK);
    self->state = STATE_IDLE;
    break;

  case STATE_TRANSMIT_MODE_VL:
#if VL_ENABLE_SENDER == 1
    VLSender_send(&self->base.previewMode);
#endif
    self->state = STATE_TRANSMIT_MODE_VL_DONE;
    break;
  case STATE_TRANSMIT_MODE_VL_DONE:
    SerialComs_write(EDITOR_VERB_TRANSMIT_VL_ACK);
    self->state = STATE_IDLE;
    break;

  case STATE_PULL_EACH_MODE:
    sendModeCount(self);
    self->state = STATE_PULL_EACH_MODE_COUNT;
    break;
  case STATE_PULL_EACH_MODE_COUNT:
    if (receiveMessage(self, EDITOR_VERB_PULL_EACH_MODE_ACK) == RV_WAIT) {
      break;
    }
    if (Modes_numModes() == 0) {
      self->state = STATE_PULL_EACH_MODE_DONE;
    } else {
      self->previousModeIndex = Modes_curModeIndex();
      self->state = STATE_PULL_EACH_MODE_SEND;
    }
    break;
  case STATE_PULL_EACH_MODE_SEND:
    sendCurMode(self);
    self->state = STATE_PULL_EACH_MODE_WAIT;
    break;
  case STATE_PULL_EACH_MODE_WAIT:
    if (receiveMessage(self, EDITOR_VERB_PULL_EACH_MODE_ACK) == RV_WAIT) {
      break;
    }
    if (Modes_curModeIndex() < (Modes_numModes() - 1)) {
      Modes_nextMode();
      self->state = STATE_PULL_EACH_MODE_SEND;
    } else {
      self->state = STATE_PULL_EACH_MODE_DONE;
    }
    break;
  case STATE_PULL_EACH_MODE_DONE:
    SerialComs_write(EDITOR_VERB_PULL_EACH_MODE_DONE);
    Modes_setCurMode(self->previousModeIndex);
    self->state = STATE_IDLE;
    break;

  case STATE_PUSH_EACH_MODE:
    SerialComs_write(EDITOR_VERB_PUSH_EACH_MODE_ACK);
    self->state = STATE_PUSH_EACH_MODE_COUNT;
    break;
  case STATE_PUSH_EACH_MODE_COUNT:
    if (receiveModeCount(self) == RV_WAIT) {
      break;
    }
    Modes_clearModes();
    SerialComs_write(EDITOR_VERB_PUSH_EACH_MODE_ACK);
    self->state = STATE_PUSH_EACH_MODE_RECEIVE;
    break;
  case STATE_PUSH_EACH_MODE_RECEIVE:
    if (receiveMode(self) == RV_WAIT) {
      break;
    }
    SerialComs_write(EDITOR_VERB_PUSH_EACH_MODE_ACK);
    if (self->numModesToReceive > 0) {
      self->numModesToReceive--;
    }
    if (!self->numModesToReceive) {
      self->state = STATE_PUSH_EACH_MODE_DONE;
    }
    break;
  case STATE_PUSH_EACH_MODE_DONE:
    Modes_saveStorage();
    self->state = STATE_IDLE;
    break;

  case STATE_SET_GLOBAL_BRIGHTNESS:
    SerialComs_write(EDITOR_VERB_READY);
    self->state = STATE_SET_GLOBAL_BRIGHTNESS_RECEIVE;
    break;
  case STATE_SET_GLOBAL_BRIGHTNESS_RECEIVE:
    if (receiveBrightness(self) == RV_WAIT) {
      break;
    }
    self->state = STATE_IDLE;
    break;

  case STATE_GET_GLOBAL_BRIGHTNESS:
    sendBrightness(self);
    self->state = STATE_IDLE;
    break;
  }
}

static void showEditor(EditorConnectionMenu *self)
{
  switch (self->state) {
  case STATE_DISCONNECTED:
    Leds_clearAll();
    Leds_blinkAll(250, 150, COL32(RGB_WHITE0));
    break;
  case STATE_IDLE:
    Mode_play(&self->base.previewMode);
    break;
  default:
    break;
  }
}

static void receiveData(EditorConnectionMenu *self)
{
  SerialComs_read(&self->receiveBuffer);
}

static void sendModes(EditorConnectionMenu *self)
{
  (void)self;
  ByteStream modesBuffer;
  memset(&modesBuffer, 0, sizeof(modesBuffer));
  Modes_saveToBuffer(&modesBuffer);
  SerialComs_writeStream(&modesBuffer);
}

static void sendModeCount(EditorConnectionMenu *self)
{
  (void)self;
  ByteStream buffer;
  memset(&buffer, 0, sizeof(buffer));
  ByteStream_serialize8(&buffer, Modes_numModes());
  SerialComs_writeStream(&buffer);
}

static void sendCurMode(EditorConnectionMenu *self)
{
  (void)self;
  Mode *cur = Modes_curMode();
  if (!cur) {
    return;
  }
  ByteStream modeBuffer;
  memset(&modeBuffer, 0, sizeof(modeBuffer));
  if (!Mode_saveToBuffer(cur, &modeBuffer, 0)) {
    return;
  }
  SerialComs_writeStream(&modeBuffer);
}

static void sendCurModeVL(EditorConnectionMenu *self)
{
#if VL_ENABLE_SENDER == 1
  self->state = STATE_TRANSMIT_MODE_VL;
#else
  (void)self;
#endif
}

static ReturnCode sendBrightness(EditorConnectionMenu *self)
{
  (void)self;
  ByteStream brightnessBuf;
  memset(&brightnessBuf, 0, sizeof(brightnessBuf));
  if (!ByteStream_serialize8(&brightnessBuf, Leds_getBrightness())) {
    return RV_FAIL;
  }
  SerialComs_writeStream(&brightnessBuf);
  return RV_OK;
}

static ReturnCode receiveBuffer(EditorConnectionMenu *self, ByteStream *buffer)
{
  uint32_t size = 0;
  if (ByteStream_size(&self->receiveBuffer) < sizeof(size)) {
    return RV_WAIT;
  }
  ByteStream_resetUnserializer(&self->receiveBuffer);
  size = ByteStream_peek32(&self->receiveBuffer);
  if (ByteStream_size(&self->receiveBuffer) < (size + sizeof(size))) {
    return RV_WAIT;
  }
  if (!ByteStream_consume32(&self->receiveBuffer, &size)) {
    return RV_FAIL;
  }
  if (!ByteStream_init(buffer, size, NULL)) {
    return RV_FAIL;
  }
  uint8_t *rawData = (uint8_t *)ByteStream_rawData(buffer);
  if (!ByteStream_consume(&self->receiveBuffer, size, rawData)) {
    return RV_FAIL;
  }
  ByteStream_sanity(buffer);
  if (!ByteStream_checkCRC(buffer)) {
    ByteStream_clear(buffer);
    return RV_FAIL;
  }
  return RV_OK;
}

static ReturnCode receiveModes(EditorConnectionMenu *self)
{
  ByteStream buf;
  memset(&buf, 0, sizeof(buf));
  self->rv = receiveBuffer(self, &buf);
  if (self->rv != RV_OK) {
    return self->rv;
  }
  if (!Modes_loadFromBuffer(&buf) || !Modes_saveStorage()) {
    return RV_FAIL;
  }
  return RV_OK;
}

static ReturnCode receiveModeCount(EditorConnectionMenu *self)
{
  ByteStream buf;
  memset(&buf, 0, sizeof(buf));
  self->rv = receiveBuffer(self, &buf);
  if (self->rv != RV_OK) {
    return self->rv;
  }
  uint8_t count;
  if (!ByteStream_consume8(&buf, &count)) {
    return RV_FAIL;
  }
  self->numModesToReceive = count;
  if (self->numModesToReceive > MAX_MODES) {
    return RV_FAIL;
  }
  return RV_OK;
}

static ReturnCode receiveMode(EditorConnectionMenu *self)
{
  ByteStream buf;
  memset(&buf, 0, sizeof(buf));
  self->rv = receiveBuffer(self, &buf);
  if (self->rv != RV_OK) {
    return self->rv;
  }
  if (!Modes_addModeFromBuffer(&buf)) {
    return RV_FAIL;
  }
  return RV_OK;
}

static ReturnCode receiveDemoMode(EditorConnectionMenu *self)
{
  ByteStream buf;
  memset(&buf, 0, sizeof(buf));
  self->rv = receiveBuffer(self, &buf);
  if (self->rv != RV_OK) {
    return self->rv;
  }
  if (!Mode_loadFromBuffer(&self->base.previewMode, &buf)) {
    return RV_FAIL;
  }
  return RV_OK;
}

static ReturnCode receiveMessage(EditorConnectionMenu *self, const char *message)
{
  size_t len = strlen(message);
  uint8_t byte = 0;
  if (ByteStream_size(&self->receiveBuffer) < len) {
    return RV_WAIT;
  }
  if (memcmp(ByteStream_data(&self->receiveBuffer), message, len) != 0) {
    return RV_FAIL;
  }
  for (size_t i = 0; i < len; ++i) {
    if (!ByteStream_consume8(&self->receiveBuffer, &byte)) {
      return RV_FAIL;
    }
  }
  self->allowReset = false;
  return RV_OK;
}

static ReturnCode receiveBrightness(EditorConnectionMenu *self)
{
  ByteStream buf;
  memset(&buf, 0, sizeof(buf));
  self->rv = receiveBuffer(self, &buf);
  if (self->rv != RV_OK) {
    return self->rv;
  }
  if (ByteStream_size(&buf) == 0) {
    return RV_FAIL;
  }
  uint8_t brightness = 255;
  if (!ByteStream_consume8(&buf, &brightness) || brightness == 0) {
    return RV_FAIL;
  }
  if (brightness > 0) {
    Leds_setBrightness(brightness);
    Modes_saveHeader();
  }
  return RV_OK;
}

const MenuVTable g_editorConnectionMenuVTable = {
  .destroy = EditorConnectionMenu_destroy,
  .init = EditorConnectionMenu_init,
  .run = EditorConnectionMenu_run,
  .onLedSelected = Menu_onLedSelected,
  .onShortClick = EditorConnectionMenu_onShortClick,
  .onLongClick = EditorConnectionMenu_onLongClick,
  .leaveMenu = EditorConnectionMenu_leaveMenu,
};
