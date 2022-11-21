#ifndef IR_CONTROL_H
#define IR_CONTROL_H

class Infrared
{
  // private unimplemented constructor
  Infrared();

public:
  // The Infrared controller class just wraps the IRReceiver and IRSender classes
  static bool init();
  static void cleanup();

private:
};

#endif
