#pragma once

#include <stdint.h>

#include <string>
#include <vector>

// VirtualSerial
//
// A virtual serial port that lets the vortex engine emulate a USB-connected
// device so that external software (such as the lightshow.lol editor over
// Web Serial) can talk to it exactly like a real device.
//
//   BACKEND_PTY     create a pty (/dev/pts/N) that any serial tool can open.
//                   This is the default and works everywhere without root.
//                   The engine treats it as connected whenever a client has
//                   the slave side open. Web Serial cannot see the pty itself,
//                   so the pty is bridged to a real serial port with socat
//                   (a USB-TTL dongle the browser can enumerate).
//
// Because the browser may connect to the virtual device at any time (long
// after the engine has already sent its one-time greeting), the port re-sends
// the greeting periodically until the client sends its first command. Once a
// command is received the replay stops so it can never corrupt an active
// editor session. The replay is also capped at a fixed window after the last
// connect event to keep stale greetings from piling up in the browser's
// buffer while the user idles.
class VirtualSerial
{
public:
  enum Backend {
    BACKEND_PTY = 0,
  };

  VirtualSerial();
  ~VirtualSerial();

  // create the port, returns false if it could not be created
  bool init(Backend backend);

  // tear down the port and any connections
  void cleanup();

  // pump data between the port and the internal rx/tx buffers, call this
  // every engine tick while running
  void poll();

  // whether a client is currently connected to the port
  bool hasClient() const { return m_hasClient; }

  // number of bytes available to be read by the engine
  int32_t avail() const { return (int32_t)m_rx.size(); }

  // read data from the port into the engine
  size_t read(char *buf, size_t amt);

  // write data from the engine to the port
  uint32_t write(const uint8_t *buf, size_t amt);

  // the display name of the port (ex: /dev/pts/3)
  const char *path() const { return m_path.c_str(); }

private:
  bool initPty();

  void pumpPty();

  // flush the tx buffer out to the port
  void flushTx();
  // read any available bytes from the port into the rx buffer
  void drainRx();
  // capture and replay the greeting until the client sends a command
  void handleGreeting();
  // write raw bytes straight out to the port (bypasses the tx buffer)
  void sendBytes(const uint8_t *data, size_t amt);

  Backend m_backend;
  int m_fd;         // master pty fd
  bool m_hasClient;
  bool m_seenClientCommand;
  bool m_sawPtyHup;
  uint32_t m_lastGreet;
  uint32_t m_replayStart;
  uint32_t m_baud;
  std::string m_path;

  std::vector<uint8_t> m_rx;
  std::vector<uint8_t> m_tx;
  std::vector<uint8_t> m_greeting;
};
