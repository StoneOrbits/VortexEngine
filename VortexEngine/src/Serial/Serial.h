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
  static bool checkSerial();

  // whether serial is initialized
  static bool isConnected();

private:
  // whether serial communications are initialized
  static bool m_serialConnected;
};

#endif
