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

// hard cap on the rx/tx buffers so a stuck client can't grow them forever
#define VIRTUAL_SERIAL_BUFFER_MAX (1 << 20)
// how many recently transmitted bytes to keep for echo detection
#define VIRTUAL_SERIAL_ECHO_WINDOW 8192

// whether a received chunk is an echo of data we just sent: it must match the
// tail of the recent tx window exactly, real editor commands are single
// letters and can never collide with our own (much longer) messages
static bool isEcho(const std::vector<uint8_t> &recentTx, const uint8_t *data, size_t amt)
{
  if (!amt || amt > recentTx.size()) {
    return false;
  }
  return memcmp(&recentTx[recentTx.size() - amt], data, amt) == 0;
}

// echo one chunk of serial traffic to the console, printable bytes are shown
// as-is and everything else is escaped so binary payloads stay readable
static void logTraffic(const char *dir, const uint8_t *data, size_t amt)
{
  if (!amt) {
    return;
  }
  printf("[%s] ", dir);
  for (size_t i = 0; i < amt; ++i) {
    uint8_t c = data[i];
    switch (c) {
    case '\n': printf("\\n"); break;
    case '\r': printf("\\r"); break;
    case '\t': printf("\\t"); break;
    default:
      if (c >= 0x20 && c < 0x7F) {
        putchar(c);
      } else {
        printf("\\x%02X", c);
      }
      break;
    }
  }
  printf("\n");
  fflush(stdout);
}

VirtualSerial::VirtualSerial() :
  m_backend(BACKEND_PTY),
  m_fd(-1),
  m_hasClient(false),
  m_logTraffic(false),
  m_baud(115200)
{
}

VirtualSerial::~VirtualSerial()
{
  cleanup();
}

bool VirtualSerial::init(Backend backend)
{
  m_backend = backend;
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
  m_rx.clear();
  m_tx.clear();
  m_recentTx.clear();
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
  // the port starts disconnected: nothing is ever sent until a client has
  // the slave side open, pumpPty() detects that through the master's HUP
  m_hasClient = false;
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
  // a pty master reports HUP/ERR whenever nothing has the slave side open,
  // including before the very first client connects, so this is an accurate
  // live view of whether anyone is actually attached to the port
  m_hasClient = !(pfd.revents & (POLLHUP | POLLERR));
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
    // drop our own data bouncing back off the line before it can poison the
    // engine's receive buffer or be mistaken for an editor command
    if (isEcho(m_recentTx, buf, (size_t)got)) {
      if (m_logTraffic) {
        logTraffic("ECHO", buf, (size_t)got);
      }
      continue;
    }
    m_rx.insert(m_rx.end(), buf, buf + got);
    if (m_logTraffic) {
      logTraffic("RX", buf, (size_t)got);
    }
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
  if (m_logTraffic) {
    logTraffic("TX", buf, amt);
  }
  // remember what we sent so anything identical that comes back is detected
  m_recentTx.insert(m_recentTx.end(), buf, buf + amt);
  if (m_recentTx.size() > VIRTUAL_SERIAL_ECHO_WINDOW) {
    m_recentTx.erase(m_recentTx.begin(), m_recentTx.end() - VIRTUAL_SERIAL_ECHO_WINDOW);
  }
  if (m_tx.size() + amt > VIRTUAL_SERIAL_BUFFER_MAX) {
    amt = VIRTUAL_SERIAL_BUFFER_MAX - m_tx.size();
  }
  m_tx.insert(m_tx.end(), buf, buf + amt);
  return (uint32_t)amt;
}
