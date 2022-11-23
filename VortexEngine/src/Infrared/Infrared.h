#ifndef INFRARED_H
#define INFRARED_H

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
