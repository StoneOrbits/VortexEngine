#ifndef MODE_SHARING_H
#define MODE_SHARING_H

#include <stdbool.h>
#include "../Menu.h"

// these three are the main options that can be iterated through
// and if the button is held it will send, otherwise receive
typedef uint8_t ModeShareState;
enum {
  SHARE_SEND_RECEIVE,           // send/receive mode
  SHARE_SEND_RECEIVE_LEGACY,    // send/receive mode legacy
  SHARE_EXIT,                   // exit mode sharing
};

typedef struct ModeSharing
{
  Menu m_base;
  ModeShareState m_sharingMode;
  // the start time when checking for timing out
  uint32_t m_timeOutStartTime;
  // used to track when the receive percentage changes for timeout purposes
  uint32_t m_lastPercentChange;
  uint8_t m_lastPercent;
} ModeSharing;

extern const MenuVtable ModeSharing_vtable;

ModeSharing *ModeSharing_new(const RGBColor *col, bool advanced);
void ModeSharing_destroy(ModeSharing *self);

bool ModeSharing_init(ModeSharing *self);
MenuAction ModeSharing_run(ModeSharing *self);

// callback after the user selects the target led
void ModeSharing_onLedSelected(ModeSharing *self);

// handlers for clicks
void ModeSharing_onShortClick(ModeSharing *self);
void ModeSharing_onLongClick(ModeSharing *self);

void ModeSharing_receiveMode(ModeSharing *self);
void ModeSharing_showReceiveMode(ModeSharing *self);

#endif