#ifndef TIMECODE_TRANSMITTER_H
#define TIMECODE_TRANSMITTER_H

#include "../Menu.h"

class TimecodeTransmitter : public Menu
{
public:
  TimecodeTransmitter(const RGBColor &col, bool advanced);
  ~TimecodeTransmitter();

  bool init() override;
  MenuAction run() override;

  // handlers for clicks
  void onShortClickM() override;
  void onShortClickL() override;
  void onShortClickR() override;
  void onLongClickM() override;

private:
  void beginSendingVL();
  void continueSendingVL();
};

#endif

