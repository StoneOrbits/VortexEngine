#include "VirtualSerial.h"

#ifndef WASM
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/poll.h>
#include <termios.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#endif
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdio.h>

// how often to replay the greeting while waiting for the client's first command
#define GREETING_REPLAY_INTERVAL_MS 500
// hard cap on the rx/tx buffers so a stuck client can't grow them forever
#define VIRTUAL_SERIAL_BUFFER_MAX (1 << 20)

VirtualSerial::VirtualSerial() :
  m_backend(BACKEND_PTY),
  m_fd(-1),
  m_hasClient(false),
  m_seenClientCommand(false),
  m_sawPtyHup(false),
  m_lastGreet(0),
  m_replayStart(0),
  m_baud(115200)
{
}

VirtualSerial::~VirtualSerial()
{
  cleanup();
}

static uint32_t virtualSerialTicks()
{
  struct timespec ts;
  clock_gettime(CLOCK_MONOTONIC, &ts);
  return (uint32_t)((ts.tv_sec * 1000) + (ts.tv_nsec / 1000000));
}

bool VirtualSerial::init(Backend backend)
{
  m_backend = backend;
  m_lastGreet = virtualSerialTicks();
  m_replayStart = m_lastGreet;
  switch (m_backend) {
  case BACKEND_PTY:
    return initPty();
  }
  return false;
}

void VirtualSerial::cleanup()
{
  if (m_fd >= 0) {
    close(m_fd);
  }
  m_fd = -1;
  m_hasClient = false;
  m_seenClientCommand = false;
  m_sawPtyHup = false;
  m_replayStart = 0;
  m_rx.clear();
  m_tx.clear();
  m_greeting.clear();
}

// ---------------------------------------------------------------------------
//  backends
// ---------------------------------------------------------------------------

bool VirtualSerial::initPty()
{
#if defined(_WIN32) || defined(WASM)
  return false;
#else
  int master = posix_openpt(O_RDWR | O_NOCTTY);
  if (master < 0) {
    perror("posix_openpt");
    return false;
  }
  if (grantpt(master) != 0 || unlockpt(master) != 0) {
    perror("grantpt/unlockpt");
    close(master);
    return false;
  }
  char name[128] = {0};
  if (ptsname_r(master, name, sizeof(name)) != 0) {
    perror("ptsname_r");
    close(master);
    return false;
  }
  fcntl(master, F_SETFL, O_NONBLOCK);
  // force the pty to raw mode with no echo. a pty's termios is sticky across
  // master reincarnations, so if a previous client left echo/canonical mode
  // enabled our own outgoing data would bounce straight back into the master
  // and get mistaken for client input. raw/no-echo also matches the real
  // firmware device, which is a bare cdc serial port.
  struct termios tio;
  memset(&tio, 0, sizeof(tio));
  if (tcgetattr(master, &tio) == 0) {
    cfmakeraw(&tio);
    cfsetspeed(&tio, (speed_t)m_baud);
    tcsetattr(master, TCSANOW, &tio);
  }
  m_fd = master;
  m_path = name;
  // a pty master has no way to know if a slave is open before the first
  // slave ever opens, so treat it as connected from the start
  m_hasClient = true;
  return true;
#endif
}

// ---------------------------------------------------------------------------
//  polling
// ---------------------------------------------------------------------------

void VirtualSerial::poll()
{
  pumpPty();
  flushTx();
  drainRx();
  handleGreeting();
}

void VirtualSerial::pumpPty()
{
#if defined(_WIN32) || defined(WASM)
  return;
#else
  if (m_fd < 0) {
    return;
  }
  struct pollfd pfd = { m_fd, POLLIN | POLLHUP, 0 };
  if (::poll(&pfd, 1, 0) < 0) {
    return;
  }
  if (pfd.revents & POLLHUP) {
    // every slave fd has closed, the client is gone
    m_hasClient = false;
    m_sawPtyHup = true;
    m_seenClientCommand = false;
  } else if (m_sawPtyHup) {
    // a new slave opened up, treat it as a fresh connection
    m_hasClient = true;
    m_seenClientCommand = false;
    // restart the greeting replay window for the new client
    m_replayStart = virtualSerialTicks();
  }
#endif
}

void VirtualSerial::flushTx()
{
#ifndef WASM
  while (!m_tx.empty() && m_hasClient) {
    ssize_t written = 0;
#ifdef _WIN32
    written = send(m_fd, &m_tx[0], m_tx.size(), 0);
#else
    written = ::write(m_fd, &m_tx[0], m_tx.size());
#endif
    if (written < 0) {
      if (errno == EAGAIN || errno == EWOULDBLOCK) {
        // try again on the next poll
        return;
      }
      if (errno == EIO || errno == ENXIO) {
        // no slave side open on a pty, wait for the client to come back
        return;
      }
      return;
    }
    m_tx.erase(m_tx.begin(), m_tx.begin() + (size_t)written);
  }
#endif
}

void VirtualSerial::drainRx()
{
#ifndef WASM
  uint8_t buf[1024];
  while (m_hasClient) {
#ifdef _WIN32
    ssize_t got = recv(m_fd, buf, sizeof(buf), 0);
    if (got <= 0) {
      if (got == 0) {
        // client closed the connection
        close(m_fd);
        m_fd = -1;
        m_hasClient = false;
        m_seenClientCommand = false;
      }
      return;
    }
#else
    ssize_t got = ::read(m_fd, buf, sizeof(buf));
    if (got < 0) {
      if (errno == EAGAIN || errno == EWOULDBLOCK) {
        return;
      }
      return;
    }
    if (got == 0) {
      return;
    }
#endif
    if (m_rx.size() + (size_t)got > VIRTUAL_SERIAL_BUFFER_MAX) {
      // drop the oldest data to keep the buffer bounded
      size_t over = (m_rx.size() + (size_t)got) - VIRTUAL_SERIAL_BUFFER_MAX;
      m_rx.erase(m_rx.begin(), m_rx.begin() + over);
    }
    m_rx.insert(m_rx.end(), buf, buf + got);
    m_seenClientCommand = true;
  }
#endif
}

// ---------------------------------------------------------------------------
//  engine <-> port io
// ---------------------------------------------------------------------------

size_t VirtualSerial::read(char *buf, size_t amt)
{
  size_t count = (amt < m_rx.size()) ? amt : m_rx.size();
  if (!count) {
    return 0;
  }
  memcpy(buf, &m_rx[0], count);
  m_rx.erase(m_rx.begin(), m_rx.begin() + count);
  return count;
}

uint32_t VirtualSerial::write(const uint8_t *buf, size_t amt)
{
  // capture the one-time greeting so we can replay it for late connecting
  // clients, only grab the first one that looks like the device greeting
  if (m_greeting.empty() && amt >= 3 && buf[0] == '=' && buf[1] == '=' && buf[2] == ' ') {
    m_greeting.assign(buf, buf + amt);
    // re-send it immediately so a fresh client gets it right away
    sendBytes(m_greeting.data(), m_greeting.size());
    return (uint32_t)amt;
  }
  if (m_tx.size() + amt > VIRTUAL_SERIAL_BUFFER_MAX) {
    amt = VIRTUAL_SERIAL_BUFFER_MAX - m_tx.size();
  }
  m_tx.insert(m_tx.end(), buf, buf + amt);
  return (uint32_t)amt;
}

void VirtualSerial::sendBytes(const uint8_t *data, size_t amt)
{
#ifndef WASM
  if (!m_hasClient) {
    return;
  }
  while (amt > 0) {
#ifdef _WIN32
    ssize_t written = send(m_fd, data, amt, 0);
#else
    ssize_t written = ::write(m_fd, data, amt);
#endif
    if (written < 0) {
      if (errno == EAGAIN || errno == EWOULDBLOCK || errno == EIO) {
        return;
      }
      return;
    }
    data += written;
    amt -= (size_t)written;
  }
#endif
}

void VirtualSerial::handleGreeting()
{
  // until the client sends its first command the engine has no way to know
  // when the editor actually connected, so re-send the greeting on an interval
  // so that the editor sees the device no matter when it opens the port. there
  // is no time cap: the browser can connect at any time after the tool starts,
  // and the replay only stops once the client sends a real command.
  if (m_hasClient && !m_seenClientCommand) {
    uint32_t now = virtualSerialTicks();
    if (!m_greeting.empty() && (now - m_lastGreet) >= GREETING_REPLAY_INTERVAL_MS) {
      m_lastGreet = now;
      sendBytes(m_greeting.data(), m_greeting.size());
    }
  }
}
