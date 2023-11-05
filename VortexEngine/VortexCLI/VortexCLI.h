#pragma once

#include "VortexLib.h"

#include "Patterns/Patterns.h"
#include "Patterns/Pattern.h"
#include "Colors/ColorTypes.h"
#include "Colors/Colorset.h"
#include "Leds/LedTypes.h"

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
  bool isButtonPressed() const;

  // whether the test framework is still running
  bool stillRunning() const;

  // setup the array of leds
  void installLeds(void *leds, uint32_t count);

  // dump out the json format of the current engine state
  void dumpJSON();
  void modeToJSON(const Mode &mode);
  void patternToJSON(const Pattern &pat);

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
  private:
    // receive a message from client
  };

  // internal helper for updating terminal size
  void get_terminal_size();

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
  };
  OutputType m_outputType;
  bool m_noTimestep;
  bool m_lockstep;
  bool m_inPlace;
  bool m_record;
  bool m_storage;
  bool m_json;
  bool m_sleepEnabled;
  bool m_lockEnabled;
  std::string m_storageFile;
  std::string m_patternIDStr;
  std::string m_colorsetStr;
  std::string m_argumentsStr;
  // to pipe stuff into the engine
  int m_pipe_fd[2];
  int m_saved_stdin;
  std::string m_inputBuffer;
};

extern VortexCLI *g_pVortexCLI;
