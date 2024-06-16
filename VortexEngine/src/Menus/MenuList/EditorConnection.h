#ifndef EDITOR_CONNECTION_H
#define EDITOR_CONNECTION_H

#include "../Menu.h"

#include "../../Serial/ByteStream.h"
#include "../../Modes/Mode.h"

class EditorConnection : public Menu
{
public:
  EditorConnection(const RGBColor &col, bool advanced);
  ~EditorConnection();

  bool init() override;
  MenuAction run() override;

  // broadcast the current preview mode over VL
  void sendCurModeVL();
  void listenModeVL();

  // pull/push through the chromalink
  bool pullHeaderChromalink();
  bool pushHeaderChromalink();
  bool pullModeChromalink();
  bool pushModeChromalink();

  // handlers for clicks
  void onShortClick() override;
  void onLongClick() override;

  // menu conn
  void leaveMenu(bool doSave = false) override;

private:
  void showEditor();
  void receiveData();
  void sendModes();
  bool receiveModes();
  bool receiveDemoMode();
  void handleCommand();
  bool receiveMessage(const char *message);
  void clearDemo();
  void receiveModeVL();
  void showReceiveModeVL();
  bool receiveModeIdx(uint8_t &idx);

  enum EditorConnectionState {
    // the editor is not connec
    STATE_DISCONNECTED,

    // Sending the greeting message
    STATE_GREETING,

    // entirely idle, waiting for commands
    STATE_IDLE,

    // engine pulls the modes from gloves, then wait for the sent modes ack
    STATE_PULL_MODES,
    STATE_PULL_MODES_SEND,
    STATE_PULL_MODES_DONE,

    // engine pushes modes to gloves, then waits for done
    STATE_PUSH_MODES,
    STATE_PUSH_MODES_RECEIVE,
    STATE_PUSH_MODES_DONE,

    // engine pushes mode to gloves for demo while idle
    STATE_DEMO_MODE,
    STATE_DEMO_MODE_RECEIVE,
    STATE_DEMO_MODE_DONE,

    // engine tells gloves to clear the demo preview, gloves acknowledge
    STATE_CLEAR_DEMO,

    // transmit the mode over visible light
    STATE_TRANSMIT_MODE_VL,
    STATE_TRANSMIT_MODE_VL_DONE,

    // receive a mode over VL
    STATE_LISTEN_MODE_VL,
    STATE_LISTEN_MODE_VL_DONE,

    // pull the header from the chromalinked duo
    STATE_PULL_HEADER_CHROMALINK,
    STATE_PULL_HEADER_CHROMALINK_SEND,
    STATE_PULL_HEADER_CHROMALINK_DONE,

    // push the header to the chromalinked duo
    STATE_PUSH_HEADER_CHROMALINK,
    STATE_PUSH_HEADER_CHROMALINK_DONE,

    // pull a mode from the chromalinked duo
    STATE_PULL_MODE_CHROMALINK,
    STATE_PULL_MODE_CHROMALINK_SEND,
    STATE_PULL_MODE_CHROMALINK_DONE,
    
    // push a mode to the chromalinked duo
    STATE_PUSH_MODE_CHROMALINK,
    STATE_PUSH_MODE_CHROMALINK_DONE,
  };

  // state of the editor
  EditorConnectionState m_state;
  // the data that is received
  ByteStream m_receiveBuffer;
  // receiver timeout
  uint32_t m_timeOutStartTime;
};

#endif
