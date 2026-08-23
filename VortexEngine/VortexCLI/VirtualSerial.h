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
//                   The port is only 'connected' while a client actually has
//                   the slave side open (the master reports POLLHUP whenever
//                   nothing has it open). Web Serial cannot see the pty
//                   itself, so the pty is bridged to a real serial port with
//                   socat (a USB-TTL dongle or a synthesized USB gadget).
//
// Nothing is ever sent until a client is attached: the engine detects the
// connection through the normal isConnected()/checkSerial() flow and sends
// its greeting exactly once per connect, just like real firmware.
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

  // echo all tx/rx serial traffic to the console for debugging
  void setLogTraffic(bool log) { m_logTraffic = log; }

private:
  bool initPty();

  void pumpPty();

  // flush the tx buffer out to the port
  void flushTx();
  // read any available bytes from the port into the rx buffer
  void drainRx();

  Backend m_backend;
  int m_fd;         // master pty fd
  bool m_hasClient;
  bool m_logTraffic;
  uint32_t m_baud;
  std::string m_path;

  std::vector<uint8_t> m_rx;
  std::vector<uint8_t> m_tx;
  // rolling window of recently transmitted bytes so anything that comes back
  // byte-for-byte identical (pty/tty line discipline echo, gadget quirks)
  // can be recognized and dropped instead of being fed to the engine as
  // fake editor input
  std::vector<uint8_t> m_recentTx;
};
