#pragma once

#include "VortexLib.h"

#include "Patterns/Patterns.h"
#include "Patterns/Pattern.h"
#include "Colors/ColorTypes.h"
#include "Colors/Colorset.h"
#include "Leds/LedTypes.h"

#include "VirtualSerial.h"

class VortexCLI
{
public:
  VortexCLI();
  ~VortexCLI();

  // initialize the test framework
  bool init(int argc, char *argv[]);

  // run the test framework
  void run();
  void cleanup();

  // render the cli vortex tool
  void show();

  // whether the button is pressed
  bool isButtonPressed();

  // whether the test framework is still running
  bool stillRunning() const;

  // setup the array of leds
  void installLeds(void *leds, uint32_t count);

  static void printlog(const char *file, const char *func, int line, const char *msg, va_list list);

  void setColoredOutput(bool output) { m_outputType = OUTPUT_TYPE_COLOR; }
  void setHexOutput(bool output) { m_outputType = OUTPUT_TYPE_HEX; }
  void setNoTimestep(bool timestep) { m_noTimestep = timestep; }
  void setInPlace(bool inplace) { m_inPlace = inplace; }

private:
  class VortexCLICallbacks : public VortexCallbacks
  {
  public:
    VortexCLICallbacks() {}
    virtual ~VortexCLICallbacks() {}
    virtual long checkPinHook(uint32_t pin) override;
    virtual void ledsInit(void *cl, int count) override;
    virtual void ledsShow() override;
    virtual bool serialCheck() override;
    virtual void serialBegin(uint32_t baud) override;
    virtual int32_t serialAvail() override;
    virtual size_t serialRead(char *buf, size_t amt) override;
    virtual uint32_t serialWrite(const uint8_t *buf, size_t amt) override;
    virtual bool serialConnectedReal() override;
  private:
    // receive a message from client
  };

  // internal helper for updating terminal size
  void get_terminal_size();

  // find a USB serial device (/dev/ttyUSB*, /dev/ttyACM*) that the browser's
  // Web Serial can enumerate, returns empty string when none is present
  std::string findUsbSerialDevice();
  // synthesize a virtual USB serial device in the kernel (dummy_hcd +
  // composite acm gadget) so the browser has a real port to open even with no
  // physical hardware, needs sudo once per boot
  bool setupVirtualGadget();
  // undo the synthesized gadget so the ttyACM* it created goes away when the
  // tool exits, needs sudo once per boot
  void teardownVirtualGadget();
  // spawn socat to bridge the virtual port to a USB serial device, returns
  // false if socat is missing or the bridge could not be started
  bool startSocatBridge();
  // called every run loop tick while waiting for a device to be plugged in
  void pollBridge();
  // samples the modem status lines of the bridged gadget device so we can
  // tell whether Web Serial actually opened the host-side port (DTR)
  void updateHostConnection();
  // whether the editor is truly reachable end-to-end: pty client attached
  // AND (for the synthesized gadget) the browser has the port open
  bool isEditorConnected();

  // these are in no particular order
  RGBColor *m_ledList;
  uint32_t m_numLeds;
  bool m_initialized;
  bool m_buttonPressed;
  bool m_keepGoing;
  volatile bool m_isPaused;
  PatternID m_curPattern;
  Colorset m_curColorset;
  enum OutputType {
    OUTPUT_TYPE_NONE,
    OUTPUT_TYPE_HEX,
    OUTPUT_TYPE_COLOR,
    OUTPUT_TYPE_SILENT,
  };
  OutputType m_outputType;
  enum JsonMode : uint32_t {
    JSON_MODE_NONE = 0,

    // technically two these are mutually exclusive... but this is easier
    JSON_MODE_READ_STDIN    = (1<<0),
    JSON_MODE_READ_FILE     = (1<<1),

    // also these too, mutually exclusive but this is easier
    JSON_MODE_WRITE_STDOUT  = (1<<2),
    JSON_MODE_WRITE_FILE    = (1<<3),
  };
  JsonMode m_jsonMode;
  bool m_noTimestep;
  bool m_lockstep;
  bool m_inPlace;
  bool m_record;
  bool m_storage;
  bool m_sleepEnabled;
  bool m_lockEnabled;
  bool m_jsonPretty;
  bool m_quickExit;
  std::string m_storageFile;
  std::string m_writeSaveFile;
  std::string m_writeModeFile;
  std::string m_loadSaveFile;
  std::string m_jsonInFile;
  std::string m_jsonOutFile;
  std::string m_patternIDStr;
  std::string m_colorsetStr;
  std::string m_argumentsStr;
  // virtual serial device support, emulates a USB-connected device so the
  // editor website can connect to the engine over a real com port
  bool m_editorMode;
  VirtualSerial *m_virtualSerial;
  // automatic socat bridge between the virtual port and a real USB serial
  // device (the only thing the browser's Web Serial can enumerate)
  bool m_bridgeStarted;
  int m_socatPid;
  uint32_t m_bridgeScanTick;
  // virtual USB gadget (dummy_hcd) synthesis state
  bool m_gadgetAttempted;
  bool m_gadgetReady;
  // set once we bridge the device side (ttyGS*) of a synthesized gadget, even
  // if the gadget itself was created by an earlier run; makes teardown remove
  // it on exit instead of leaving the ttyACM* behind
  bool m_usingVirtualGadget;
  // the device the socat bridge relays to (ex: /dev/ttyGS0)
  std::string m_bridgeDevice;
  // whether any process (i.e. Web Serial in the browser) actually has the
  // bridged device open, detected by scanning /proc/*/fd
  bool m_hostConnected;
  uint32_t m_hostCheckTick;
  // to pipe stuff into the engine
  int m_pipe_fd[2];
  int m_saved_stdin;
  std::string m_inputBuffer;
};

extern VortexCLI *g_pVortexCLI;
