#ifndef SERIAL_H
#define SERIAL_H

// Really wish I could name this Serial but arduino ruined that for me
class SerialComs
{
  // private constructor
  SerialComs();

public:

  // init serial
  static bool init();
  static void cleanup();

  // check for any serial connection or messages
  static void checkSerial();

  // whether serial is initialized
  static bool initialized() { return m_serial_init; }

private:
  // whether serial communications are initialized
  static bool m_serial_init;
};

#endif
