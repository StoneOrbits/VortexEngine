#include <algorithm>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <string>
#include <ctime>
#include <map>

#ifdef _WIN32
#include <windows.h>
#include <conio.h>
#else
#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>
#endif

#include <getopt.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <signal.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <dirent.h>

#include "VortexCLI.h"

#include "Log/Log.h"

#include "VortexLib.h"

#include "Patterns/PatternBuilder.h"
#include "Time/TimeControl.h"
#include "Colors/ColorTypes.h"
#include "Colors/Colorset.h"
#include "Buttons/Button.h"
#include "Time/Timings.h"
#include "Menus/Menus.h"
#include "Modes/Modes.h"
#include "Modes/Mode.h"

#include "Patterns/Pattern.h"
#include "Patterns/Single/SingleLedPattern.h"

#define RECORD_FILE "recorded_input.txt"

VortexCLI *g_pVortexCLI = nullptr;

// set when SIGINT/SIGTERM is caught so the main loop can wind down cleanly and
// tear down the virtual USB gadget before the process exits
static volatile sig_atomic_t s_signalExit = 0;

static void handle_signal(int sig)
{
  if (s_signalExit) {
    // the graceful wind-down is already under way; a second signal means the
    // user is impatient, so restore the default action and let the OS kill us
    signal(sig, SIG_DFL);
    raise(sig);
    return;
  }
  s_signalExit = 1;
}

using namespace std;

// the usage for the input strings
const char *input_usage[] = {
  "\n   c         standard short click",
  "\n   l         standard long click",
  "\n   m         open menus length click",
  "\n   a         enter adv menu length click (enter adv menu from menus)",
  "\n   d         delete length click (delete color in col select)",
  "\n   s         enter sleep length click (enter sleep at main modes)",
  "\n   f         force sleep length click (force sleep anywhere)",
  "\n   t         toggle button pressed (only way to wake after sleep)",
  "\n   r         rapid button click (ex: r15)",
  "\n   w         wait 1 tick",
  "\n   <digits>  repeat command n times (only single digits in -i mode)",
  "\n   b<index>  select button index (ex: b0 for first button or b1 for second button)",
  "\n   q         quit",
};

#define NUM_USAGE (sizeof(input_usage) / sizeof(input_usage[0]))

// the usage for the input strings but in a brief format
const char *input_usage_brief[NUM_USAGE] = {
  "\n   c         short",
  "\n   l         long",
  "\n   m         menus",
  "\n   a         adv menu",
  "\n   d         delete col",
  "\n   s         sleep",
  "\n   f         force sleep",
  "\n   t         toggle",
  "\n   r         rapid",
  "\n   w         wait",
  "\n   <digits>  repeat",
  "\n   b<index>  button idx",
  "\n   q         quit",
};


// 1000 spaces, used for efficiently printing any number of spaces up to 1000 by
// offsetting from the start of this string with the SPACES() macro below
#define SPACESTR "                                                                                                                                                     " \
                 "                                                                                                                                                     " \
                 "                                                                                                                                                     " \
                 "                                                                                                                                                     " \
                 "                                                                                                                                                     " \
                 "                                                                                                                                                     " \
                 "                                                                                                                                                     " \
                 "                                                                                                                                                     " \
                 "                                                                                                                                                     " \
                 "                                                                                                                                                     "

// 1000 hyphens for a long line
#define LINESTR  "-----------------------------------------------------------------------------------------------------------------------------------------------------" \
                 "-----------------------------------------------------------------------------------------------------------------------------------------------------" \
                 "-----------------------------------------------------------------------------------------------------------------------------------------------------" \
                 "-----------------------------------------------------------------------------------------------------------------------------------------------------" \
                 "-----------------------------------------------------------------------------------------------------------------------------------------------------" \
                 "-----------------------------------------------------------------------------------------------------------------------------------------------------" \
                 "-----------------------------------------------------------------------------------------------------------------------------------------------------" \
                 "-----------------------------------------------------------------------------------------------------------------------------------------------------" \
                 "-----------------------------------------------------------------------------------------------------------------------------------------------------"

// some amount of spaces, if you pass in an amount greater than 1000 you will die
#define SPACES(amt) SPACESTR + (sizeof(SPACESTR) - (amt))
// some length of line, if you pass in an amount greater than 1000 you will die
#define LINE(amt) LINESTR + (sizeof(LINESTR) - (amt))

#ifdef WASM // Web assembly glue
#include <emscripten/html5.h>
#include <emscripten.h>

static EM_BOOL key_callback(int eventType, const EmscriptenKeyboardEvent *e, void *userData)
{
  if (e->key[0] == ' ') {
    if (eventType == EMSCRIPTEN_EVENT_KEYDOWN) {
      Vortex::pressButton();
    } else if (eventType == EMSCRIPTEN_EVENT_KEYUP) {
      Vortex::releaseButton();
    }
  }
  return 0;
}

static void do_run()
{
  g_pVortexCLI->run();
}

static void wasm_init()
{
  emscripten_set_keydown_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, 0, 1, key_callback);
  emscripten_set_keyup_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, 0, 1, key_callback);
  emscripten_set_main_loop(do_run, 0, true);
  // turn colored output off in the wasm version
  //g_pVortexCLI->setColoredOuptut(false);
}
#endif // ifdef WASM

VortexCLI::VortexCLI() :
  m_numLeds(0),
  m_initialized(false),
  m_buttonPressed(false),
  m_keepGoing(true),
  m_isPaused(true),
  m_curPattern(PATTERN_FIRST),
  m_curColorset(),
  m_outputType(OUTPUT_TYPE_NONE),
  m_jsonMode(JSON_MODE_NONE),
  m_noTimestep(false),
  m_lockstep(false),
  m_inPlace(false),
  m_record(false),
  m_storage(false),
  m_sleepEnabled(true),
  m_lockEnabled(true),
  m_jsonPretty(false),
  m_quickExit(false),
  m_storageFile("FlashStorage.flash"),
  m_patternIDStr(),
  m_colorsetStr(),
  m_argumentsStr(),
  m_editorMode(false),
  m_virtualSerial(nullptr),
  m_bridgeStarted(false),
  m_socatPid(-1),
  m_bridgeScanTick(0),
  m_gadgetAttempted(false),
  m_gadgetReady(false),
  m_pipe_fd{-1, -1},
  m_saved_stdin(),
  m_inputBuffer()
{
}

VortexCLI::~VortexCLI()
{
}

static struct option long_options[] = {
  {"hex", no_argument, nullptr, 'x'},
  {"color", no_argument, nullptr, 'c'},
  {"silent", no_argument, nullptr, 's'},
  {"no-timestep", no_argument, nullptr, 't'},
  {"lockstep", no_argument, nullptr, 'l'},
  {"in-place", no_argument, nullptr, 'i'},
  {"record", no_argument, nullptr, 'r'},
  {"autowake", no_argument, nullptr, 'a'},
  {"nolock", no_argument, nullptr, 'n'},
  {"quick", no_argument, nullptr, 'q'},
  {"storage", optional_argument, nullptr, 'S'},
  {"write-save", required_argument, nullptr, 'W'},
  {"write-mode", required_argument, nullptr, 'M'},
  {"load-save", required_argument, nullptr, 'L'},
  {"json-in", optional_argument, nullptr, 'I'},
  {"json-out", optional_argument, nullptr, 'O'},
  {"json-human-read", no_argument, nullptr, 'H'},
  {"pattern", required_argument, nullptr, 'P'},
  {"colorset", required_argument, nullptr, 'C'},
  {"arguments", required_argument, nullptr, 'A'},
  {"editor", no_argument, nullptr, 'e'},
  {"help", no_argument, nullptr, 'h'},
  {nullptr, 0, nullptr, 0}
};

// all the available colors that can be used to make colorsets
std::map<std::string, int> color_map = {
    {"black",   0x000000},
    {"white",   0xFFFFFF},
    {"red",     0xFF0000},
    {"lime",    0x00FF00},
    {"blue",    0x0000FF},
    {"yellow",  0xFFFF00},
    {"cyan",    0x00FFFF},
    {"magenta", 0xFF00FF},
    {"silver",  0xC0C0C0},
    {"gray",    0x808080},
    {"maroon",  0x800000},
    {"olive",   0x808000},
    {"green",   0x008000},
    {"purple",  0x800080},
    {"teal",    0x008080},
    {"navy",    0x000080},
    {"orange",  0xFFA500},
    {"pink",    0xFFC0CB},
    {"brown",   0xA52A2A}
    //...add more as needed
};

static void print_usage(const char* program_name) 
{
  fprintf(stderr, "Usage: %s [options] < input commands\n", program_name);
  fprintf(stderr, "Output Selection (at least one required):\n");
  fprintf(stderr, "  -x, --hex                Use hex values to represent led colors\n");
  fprintf(stderr, "  -c, --color              Use console color codes to represent led colors\n");
  fprintf(stderr, "  -s, --silent             Do not print any output while running\n");
  fprintf(stderr, "\n");
  fprintf(stderr, "Engine Control Flags (optional):\n");
  fprintf(stderr, "  -t, --no-timestep        Bypass the timestep and run as fast as possible\n");
  fprintf(stderr, "  -l, --lockstep           Only step once each time an input is received\n");
  fprintf(stderr, "  -i, --in-place           Print the output in-place (interactive mode)\n");
  fprintf(stderr, "  -r, --record             Record the inputs and dump to a file after (" RECORD_FILE ")\n");
  fprintf(stderr, "  -a, --autowake           Automatically and instantly wake on sleep (disable sleep)\n");
  fprintf(stderr, "  -n, --nolock             Automatically unlock upon locking the chip (disable lock)\n");
  fprintf(stderr, "  -q, --quick              Exit immediately after initialization (useful to convert data)\n");
  fprintf(stderr, "\n");
  fprintf(stderr, "Storage and Data Conversion (optional):\n");
  fprintf(stderr, "  -S, --storage [file]     Persistent storage to file (default file: FlashStorage.flash)\n");
  fprintf(stderr, "  -W, --write-save <file>  Write a full .vortex savefile at exit\n");
  fprintf(stderr, "  -M, --write-mode <file>  At exit write the current mode to a .vtxmode file\n");
  fprintf(stderr, "  -L, --load-save <file>   Load a vortex savefile (either .vortex or .vtxmode)\n");
  fprintf(stderr, "  -I, --json-in [file]     Load json from file at init (if no file: read from stdin)\n");
  fprintf(stderr, "  -O, --json-out [file]    Dump json to file at exit (if no file: write to stdout)\n");
  fprintf(stderr, "  -H, --json-human-read    Dump human readable json, instead of condensed json\n");
  fprintf(stderr, "\n");
  fprintf(stderr, "Initial Pattern Options (optional):\n");
  fprintf(stderr, "  -P, --pattern <id>       Preset the pattern ID on the first mode (numeric ID or pattern name ex: blend)\n");
  fprintf(stderr, "  -C, --colorset c1,c2...  Preset the colorset on the first mode (csv list of hex codes or color names)\n");
  fprintf(stderr, "  -A, --arguments a1,a2... Preset the arguments on the first mode (csv list of arguments)\n");
  fprintf(stderr, "\n");
  fprintf(stderr, "Virtual Device (optional):\n");
  fprintf(stderr, "  -e, --editor             Expose the engine as a virtual serial device so the lightshow.lol\n");
  fprintf(stderr, "                           editor can connect to it like a real USB device. Creates a pty and\n");
  fprintf(stderr, "                           auto-bridges it to a USB serial port with socat so the browser's\n");
  fprintf(stderr, "                           Web Serial can enumerate it as a real COM port. With no hardware\n");
  fprintf(stderr, "                           present it synthesizes one in the kernel (needs sudo once per boot).\n");
  fprintf(stderr, "                           When no output option is given this implies --silent.\n");
  fprintf(stderr, "\n");
  fprintf(stderr, "Other Options:\n");
  fprintf(stderr, "  -h, --help               Display this help message\n");
  fprintf(stderr, "\n");
  fprintf(stderr, "Input Commands (pass to stdin):");
  for (uint32_t i = 0; i < NUM_USAGE; ++i) {
    fprintf(stderr, "%s", input_usage[i]);
  }
  fprintf(stderr, "\n\n");
  fprintf(stderr, "Example Usage:\n");
  fprintf(stderr, "   ./vortex --color\n");
  fprintf(stderr, "   ./vortex -ci -P42 -Ccyan,purple\n");
  fprintf(stderr, "   ./vortex -ct -Pblend -Ccyan,yellow,magenta <<< w100q\n");
  fprintf(stderr, "   ./vortex -sq -L mysave.vortex -O output_data.json\n");
  fprintf(stderr, "   ./vortex -sq -L mymode.vtxmode -O output_data.json\n");
}

#ifdef _WIN32
static HANDLE hStdin;
static DWORD mode;

static void restore_terminal()
{
  SetConsoleMode(hStdin, mode);
}
void set_terminal_nonblocking()
{
  hStdin = GetStdHandle(STD_INPUT_HANDLE);
  GetConsoleMode(hStdin, &mode);
  SetConsoleMode(hStdin, mode & ~(ENABLE_LINE_INPUT | ENABLE_ECHO_INPUT));
  atexit(restore_terminal);
}
#else
static struct termios orig_term_attr = {0};
static bool s_terminalNonblock = false;
static void restore_terminal()
{
  // only restore if we actually put the terminal into non-blocking mode;
  // otherwise orig_term_attr is all zeros and would clobber a sane terminal
  if (!s_terminalNonblock) {
    return;
  }
  tcsetattr(STDIN_FILENO, TCSANOW, &orig_term_attr);
  s_terminalNonblock = false;
}

void set_terminal_nonblocking()
{
  struct termios term_attr = {0};

  // Get the current terminal attributes and store them
  tcgetattr(STDIN_FILENO, &term_attr);
  orig_term_attr = term_attr;

  // Set the terminal in non-canonical mode (raw mode)
  term_attr.c_lflag &= ~(ICANON | ECHO);

  // Set the minimum number of input bytes read at a time to 1
  term_attr.c_cc[VMIN] = 1;

  // Set the timeout for read to 0 (no waiting)
  term_attr.c_cc[VTIME] = 0;

  // Apply the new terminal attributes
  tcsetattr(STDIN_FILENO, TCSANOW, &term_attr);

  // Set the terminal to non-blocking mode
  int flags = fcntl(STDIN_FILENO, F_GETFL, 0);
  fcntl(STDIN_FILENO, F_SETFL, flags | O_NONBLOCK);

  s_terminalNonblock = true;

  // Register the restore_terminal function to be called at exit
  atexit(restore_terminal);
}
#endif

uint32_t term_columns = 0;
void VortexCLI::get_terminal_size()
{
#ifdef _WIN32
  // Windows-specific code
  CONSOLE_SCREEN_BUFFER_INFO csbi;
  GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
  term_columns = csbi.srWindow.Right - csbi.srWindow.Left + 1;
#else
  // Linux-specific code
  struct winsize new_terminal_size = { 0 };
  ioctl(STDOUT_FILENO, TIOCGWINSZ, &new_terminal_size);
  if (term_columns != new_terminal_size.ws_col) {
    term_columns = new_terminal_size.ws_col;
    if (m_inPlace && !system("clear")) {
      printf("Failed to clear\n");
    }
  }
#endif
}

bool VortexCLI::init(int argc, char *argv[])
{
  if (g_pVortexCLI) {
    return false;
  }
  g_pVortexCLI = this;

  // catch ctrl-c / SIGTERM so cleanup() runs (and the virtual USB gadget is
  // torn down) instead of the process just being killed
#ifndef WASM
  signal(SIGINT, handle_signal);
  signal(SIGTERM, handle_signal);
#endif

  if (argc == 1) {
    print_usage(argv[0]);
    exit(1);
  }

  int opt = -1;
  int option_index = 0;
  while ((opt = getopt_long(argc, argv, "xcstliranqS::W:M:L:I::O::HP:C:A:eh", long_options, &option_index)) != -1) {
    switch (opt) {
    case 'x':
      // if the user wants pretty colors or hex codes
      m_outputType = OUTPUT_TYPE_HEX;
      break;
    case 'c':
      // if the user wants pretty colors
      m_outputType = OUTPUT_TYPE_COLOR;
      break;
    case 's':
      // turn off output
      m_outputType = OUTPUT_TYPE_SILENT;
      break;
    case 't':
      // if the user wants to bypass timestep
      m_noTimestep = true;
      break;
    case 'l':
      // if the user wants to step in lockstep with the engine
      m_lockstep = true;
      break;
    case 'i':
      // if the user wants to print in-place (on one line)
      m_inPlace = true;
      break;
    case 'r':
      // record the inputs and dump them to a file after
      m_record = true;
      break;
    case 'a':
      // autuowake prevents sleep
      m_sleepEnabled = false;
      break;
    case 'n':
      // disable the lock
      m_lockEnabled = false;
      break;
    case 'q':
      // quick exit
      m_quickExit = true;
      break;
    case 'S':
      // enable persistent storage to file
      m_storage = true;
      if (optarg == NULL && optind < argc && argv[optind][0] != '-') {
        optarg = argv[optind++];
      }
      if (optarg) {
        m_storageFile = optarg;
      }
      break;
    case 'W':
      m_writeSaveFile = optarg;
      break;
    case 'M':
      m_writeModeFile = optarg;
      break;
    case 'L':
      m_loadSaveFile = optarg;
      break;
    case 'I':
      // read json from stdin or file
      if (optarg == NULL && optind < argc && argv[optind][0] != '-') {
        optarg = argv[optind++];
      }
      if (optarg) {
        m_jsonMode = (JsonMode)(m_jsonMode | JSON_MODE_READ_FILE);
        m_jsonInFile = optarg;
      } else {
        m_jsonMode = (JsonMode)(m_jsonMode | JSON_MODE_READ_STDIN);
      }
      break;
    case 'O':
      // write json to output or file
      if (optarg == NULL && optind < argc && argv[optind][0] != '-') {
        optarg = argv[optind++];
      }
      if (optarg) {
        m_jsonMode = (JsonMode)(m_jsonMode | JSON_MODE_WRITE_FILE);
        m_jsonOutFile = optarg;
      } else {
        m_jsonMode = (JsonMode)(m_jsonMode | JSON_MODE_WRITE_STDOUT);
      }
      break;
    case 'H':
      // json human readable
      m_jsonPretty = true;
      break;
    case 'P':
      // preset the pattern ID on the first mode
      m_patternIDStr = optarg;
      break;
    case 'C':
      // preset the colorset on the first mode
      m_colorsetStr = optarg;
      break;
    case 'A':
      // preset the arguments on the first mode
      m_argumentsStr = optarg;
      break;
    case 'e':
      // expose the engine as a virtual serial device for the editor website,
      // it emulates a plugged-in device (pty + socat bridge to a USB serial
      // port) so no backend argument is needed
      m_editorMode = true;
      break;    case 'h':
      // print usage and exit
      print_usage(argv[0]);
      exit(EXIT_SUCCESS);
    default: // '?' for unrecognized options
      printf("Unknown arg: -%c\n", opt);
      exit(EXIT_FAILURE);
    }
  }

  switch (m_outputType) {
  case OUTPUT_TYPE_NONE:
    if (m_editorMode) {
      // the virtual device needs no terminal output, default to silent
      m_outputType = OUTPUT_TYPE_SILENT;
      break;
    }
    print_usage(argv[0]);
    exit(EXIT_SUCCESS);
    break;
  case OUTPUT_TYPE_COLOR:
    setColoredOutput(true);
    break;
  case OUTPUT_TYPE_HEX:
    setHexOutput(true);
    break;
  case OUTPUT_TYPE_SILENT:
    break;
  }

  // the virtual device protocol depends on the engine running in real time,
  // lockstep/in-place/no-timestep would break the serial handshake timing
  if (m_editorMode) {
    m_lockstep = false;
    m_inPlace = false;
    m_noTimestep = false;
  }

  // do the vortex init/setup
  Vortex::initEx<VortexCLICallbacks>();

  // configure the vortex engine as the parameters dictate
  Vortex::setInstantTimestep(m_noTimestep);
  Vortex::enableCommandLog(m_record);
  Vortex::enableLockstep(m_lockstep);
  Vortex::enableStorage(m_storage);
  if (m_storage) {
    Vortex::setStorageFilename(m_storageFile);
    if (access(m_storageFile.c_str(), F_OK) == 0) {
      // load storage if the file exists
      Vortex::loadStorage();
    }
  }
  Vortex::setSleepEnabled(m_sleepEnabled);
  Vortex::setLockEnabled(m_lockEnabled);

  if (m_jsonMode & JSON_MODE_READ_STDIN) {
    // todo: read js from stdin
    printf("Reading json from stdin is not implemented yet, sorry\n");
    exit(2);
  }

  if (m_jsonMode & JSON_MODE_READ_FILE) {
    printf("Reading json from %s\n", m_jsonInFile.c_str());
    // read from m_jsonInFile;
    Vortex::parseJsonFromFile(m_jsonInFile);
  }

  // load a .vortex or .vtxmode savefile
  if (m_loadSaveFile.size() > 0) {
    ByteStream stream(4096);
    // Open the file in read mode
    FILE *inputFile = fopen(m_loadSaveFile.c_str(), "r");
    if (!inputFile) {
      printf("Failed to open: [%s] (%s)\n", m_loadSaveFile.c_str(), strerror(errno));
      exit(2);
    }
    if (!fread((void *)stream.rawData(), 1, stream.capacity(), inputFile)) {
      // error
      printf("Failed to read [%s]\n", m_loadSaveFile.c_str());
      exit(2);
    }
    fclose(inputFile);
    // clear existing modes
    Modes::clearModes();
    // check if the load from savefile was provided, this is kinda ugly but whatever
    if (m_loadSaveFile.size() >= 8 && m_loadSaveFile.rfind(".vtxmode") == m_loadSaveFile.size() - 8) {
      // ends with .vtxmode, load just a single mode
      Vortex::addNewMode(stream);
    } else if (m_loadSaveFile.size() >= 7 && m_loadSaveFile.rfind(".vortex") == m_loadSaveFile.size() - 7) {
      // ends with .vortex, load the entire save
      Vortex::setModes(stream);
    } else {
      // wasn't a valid savefile name
      printf("Savefile name must end in .vortex or .vtxmode: [%s]\n", m_loadSaveFile.c_str());
      exit(2);
    }
  }

  if (m_patternIDStr.length() > 0) {
    // convert both numeric and string to see which one seems more correct
    PatternID id = (PatternID)strtoul(m_patternIDStr.c_str(), nullptr, 10);
    PatternID strID = Vortex::stringToPattern(m_patternIDStr);
    if (id == PATTERN_FIRST && strID != PATTERN_NONE) {
      // use the str ID if the numeric ID didn't convert and the string did
      id = strID;
    }
    // TODO: add arg for the led position
    Vortex::setPatternAt(LED_ALL, id);
  }
  if (m_colorsetStr.length() > 0) {
    stringstream ss(m_colorsetStr);
    string color;
    Colorset set;
    while (getline(ss, color, ',')) {
      // iterate letters and lowercase them
      transform(color.begin(), color.end(), color.begin(), [](unsigned char c){ return tolower(c); });
      if (color_map.count(color) > 0) {
        set.addColor(color_map[color]);
      } else {
        set.addColor(strtoul(color.c_str(), nullptr, 16));
      }
    }
    // TODO: add arg for the led position
    Vortex::setColorset(LED_ALL, set);
  }
  if (m_argumentsStr.length() > 0) {
    stringstream ss(m_argumentsStr);
    string arg;
    PatternArgs args;
    while (getline(ss, arg, ',')) {
      args.args[args.numArgs++] = strtoul(arg.c_str(), nullptr, 10);
    }
    // TODO: add arg for the led position
    Vortex::setPatternArgs(LED_ALL, args);
  }
  if (m_editorMode) {
#ifndef WASM
    // the tool fully emulates a plugged-in device: create a pty and bridge it
    // to a real USB serial device with socat so the browser's Web Serial can
    // enumerate it. the pty itself is not visible to Web Serial, so the
    // bridged device is what the browser opens.
    bool socatFound = false;
    const char *pathEnv = getenv("PATH");
    if (pathEnv) {
      std::string paths(pathEnv);
      size_t start = 0;
      while (start <= paths.size()) {
        size_t end = paths.find(':', start);
        std::string dir = (end == std::string::npos) ? paths.substr(start) : paths.substr(start, end - start);
        if (dir.size() > 0 && access((dir + "/socat").c_str(), X_OK) == 0) {
          socatFound = true;
          break;
        }
        if (end == std::string::npos) break;
        start = end + 1;
      }
    }
    if (!socatFound) {
      printf("The auto bridge requires 'socat' which is not installed.\n");
      printf("  sudo pacman -S socat    (arch)\n");
      printf("  sudo apt install socat  (debian/ubuntu)\n");
      exit(2);
    }
    m_virtualSerial = new VirtualSerial();
    if (!m_virtualSerial->init(VirtualSerial::BACKEND_PTY)) {
      printf("Failed to create virtual serial port\n");
      exit(2);
    }
    printf("Virtual serial device: %s\n", m_virtualSerial->path());
    startSocatBridge();
  }
#endif

  if (m_inPlace && !system("clear")) {
    printf("Failed to clear\n");
  }

  set_terminal_nonblocking();
  get_terminal_size();

  m_initialized = true;

#ifndef WASM
#else
  // NOTE: This call does not return and will instead automatically 
  // call the VortexCLI::run() in a loop
  wasm_init();
#endif

  return true;
}

void VortexCLI::run()
{
  if (!stillRunning()) {
    return;
  }
  // a SIGINT/SIGTERM handler asked us to stop, wind down on this tick
  if (s_signalExit) {
    m_quickExit = true;
  }
  // pump any data between the virtual serial port and the engine
  if (m_virtualSerial) {
    m_virtualSerial->poll();
  }
  if (m_editorMode) {
    // keep the socat bridge alive: retry if no device was present at startup
    // or restart it if the device was unplugged
    pollBridge();
  }
  if (!Vortex::tick() || m_quickExit) {
    cleanup();
  }
}

void VortexCLI::cleanup()
{
  DEBUG_LOG("Quitting...");
  if (m_inPlace) {
    printf("\n");
  }
  if (m_record) {
    // Open the file in write mode
    FILE *outputFile = fopen(RECORD_FILE, "w");
    if (!outputFile) {
      printf("Failed to open: [%s] (%s)", RECORD_FILE, strerror(errno));
      exit(2);
    }
    // Print the recorded input to the file
    fprintf(outputFile, "%s", Vortex::getCommandLog().c_str());
    // Close the output file
    fclose(outputFile);
    printf("Wrote recorded input to " RECORD_FILE "\n");
  }
  if (m_jsonMode & JSON_MODE_WRITE_STDOUT) {
    // dump the current save in json format
    Vortex::printJson(m_jsonPretty);
  }
  if (m_jsonMode & JSON_MODE_WRITE_FILE) {
    Vortex::printJsonToFile(m_jsonOutFile.c_str(), m_jsonPretty);
    printf("Wrote JSON to file [%s]\n", m_jsonOutFile.c_str());
  }
  if (m_writeSaveFile.length() > 0) {
    ByteStream stream;
    Vortex::getModes(stream);
    FILE *outputFile = fopen(m_writeSaveFile.c_str(), "w");
    if (!outputFile) {
      printf("Failed to open: [%s] (%s)", m_writeSaveFile.c_str(), strerror(errno));
      exit(2);
    }
    // Print the recorded input to the file
    fwrite(stream.rawData(), 1, stream.rawSize(), outputFile);
    // Close the output file
    fclose(outputFile);
    printf("Wrote vortex save to [%s]\n", m_writeSaveFile.c_str());
  }
  if (m_writeModeFile.length() > 0) {
    ByteStream stream;
    Vortex::getCurMode(stream);
    FILE *outputFile = fopen(m_writeModeFile.c_str(), "w");
    if (!outputFile) {
      printf("Failed to open: [%s] (%s)", m_writeModeFile.c_str(), strerror(errno));
      exit(2);
    }
    // Print the recorded input to the file
    fwrite(stream.rawData(), 1, stream.rawSize(), outputFile);
    // Close the output file
    fclose(outputFile);
    printf("Wrote vtxmode save to [%s]\n", m_writeModeFile.c_str());
  }
  m_keepGoing = false;
  m_isPaused = false;
  if (m_storage) {
    Vortex::doSave();
  }
  if (m_socatPid > 0) {
#ifndef WASM
    kill(m_socatPid, SIGTERM);
#endif
    m_socatPid = -1;
  }
  if (m_virtualSerial) {
    delete m_virtualSerial;
    m_virtualSerial = nullptr;
  }
  // tear down the virtual USB gadget (ttyACM*) that was synthesized for the
  // editor bridge, so it doesn't linger after the tool exits
  teardownVirtualGadget();
  Vortex::cleanup();
#ifdef WASM
  emscripten_force_exit(0);
#endif
}

// when the glove framework calls 'FastLED.show'
void VortexCLI::show()
{
  if (!m_initialized) {
    return;
  }
  if (m_outputType == OUTPUT_TYPE_SILENT) {
    return;
  }
  string out;
  get_terminal_size();
  uint32_t wid = term_columns;
  uint32_t odd = (wid) % 2;
  uint32_t halfwid = wid / 2;
  uint32_t midWid = (halfwid - ((2 + (m_outputType == OUTPUT_TYPE_HEX)) * LED_COUNT)) - 1;
  if (m_inPlace) {
    // this resets the cursor back to the beginning of the line and moves it up 12 lines
    out += "\33[2K\033[17A\r";
    // this is the top border line
    out += "+";
    out += LINE((wid + odd) - 2);
    out += "+\n|";
    // this is the left inner line
    out += LINE(midWid + odd);
    out += "=";
  }
  if (m_outputType == OUTPUT_TYPE_COLOR) {
    // the color strip itself
    for (uint32_t i = 0; i < m_numLeds; ++i) {
      out += "\x1B[0m["; // opening |
      out += "\x1B[48;2;"; // colorcode start
      out += to_string(m_ledList[i].red) + ";"; // col red
      out += to_string(m_ledList[i].green) + ";"; // col green
      out += to_string(m_ledList[i].blue) + "m"; // col blue
      out += "  "; // colored space
      out += "\x1B[0m]"; // ending |
    }
  } else if (m_outputType == OUTPUT_TYPE_HEX) {
    // otherwise this just prints out the raw hex code if not in color mode
    for (uint32_t i = 0; i < m_numLeds; ++i) {
      char buf[128] = { 0 };
      snprintf(buf, sizeof(buf), "%06X", m_ledList[i].raw());
      out += buf;
    }
  }
  if (!m_inPlace) {
    out += "\n";
  } else {
    // the right inner line
    out += "=";
    out += LINE((midWid - 1) + odd);
    // the end of middle line, fold and start of 3rd line
    out += "|\n+";
    out += LINE((wid + odd) - 2);
    // space line after the box and before usage
    out += "+\n";
    out += SPACES(wid + odd);
    // the usage message
    for (uint32_t i = 0; i < NUM_USAGE; ++i) {
      const char *brief = (wid < 70) ? input_usage_brief[i] : input_usage[i];
      out += brief;
      out += SPACES((wid + odd + 1) - strlen(brief));
    }
  }
  printf("%s", out.c_str());
  fflush(stdout);
}

bool VortexCLI::isButtonPressed()
{
  return Vortex::isButtonPressed();
}

bool VortexCLI::stillRunning() const
{
  return (m_initialized && m_keepGoing);
}

void VortexCLI::installLeds(void *leds, uint32_t count)
{
  m_ledList = (RGBColor *)leds;
  m_numLeds = count;
}

long VortexCLI::VortexCLICallbacks::checkPinHook(uint32_t pin)
{
  if (pin == 20) {
    // orbit button 2
    return Vortex::isButtonPressed(1) ? 0 : 1;
  }
  return Vortex::isButtonPressed(0) ? 0 : 1;
}

void VortexCLI::VortexCLICallbacks::ledsInit(void *cl, int count)
{
  g_pVortexCLI->installLeds(cl, count);
}

void VortexCLI::VortexCLICallbacks::ledsShow()
{
  g_pVortexCLI->show();
}

bool VortexCLI::VortexCLICallbacks::serialCheck()
{
  return g_pVortexCLI->m_virtualSerial && g_pVortexCLI->m_virtualSerial->hasClient();
}

void VortexCLI::VortexCLICallbacks::serialBegin(uint32_t baud)
{
}

int32_t VortexCLI::VortexCLICallbacks::serialAvail()
{
  return g_pVortexCLI->m_virtualSerial ? g_pVortexCLI->m_virtualSerial->avail() : 0;
}

size_t VortexCLI::VortexCLICallbacks::serialRead(char *buf, size_t amt)
{
  return g_pVortexCLI->m_virtualSerial ? g_pVortexCLI->m_virtualSerial->read(buf, amt) : 0;
}

uint32_t VortexCLI::VortexCLICallbacks::serialWrite(const uint8_t *buf, size_t amt)
{
  return g_pVortexCLI->m_virtualSerial ? g_pVortexCLI->m_virtualSerial->write(buf, amt) : 0;
}

bool VortexCLI::VortexCLICallbacks::serialConnectedReal()
{
  return g_pVortexCLI->m_virtualSerial && g_pVortexCLI->m_virtualSerial->hasClient();
}

// ---------------------------------------------------------------------------
//  socat bridge
// ---------------------------------------------------------------------------

#ifndef WASM

// resolve the USB vendor id of a tty device by walking its sysfs entry up to
// the parent USB device (ex: returns "28de" for the Steam Controller Puck)
static std::string ttyUsbVendor(const char *ttyName)
{
  std::string sysLink = std::string("/sys/class/tty/") + ttyName;
  char resolved[4096];
  ssize_t len = readlink(sysLink.c_str(), resolved, sizeof(resolved) - 1);
  if (len <= 0) {
    return "";
  }
  resolved[len] = '\0';
  std::string dir = std::string("/sys/class/tty/") + resolved;
  while (true) {
    std::string vendor = dir + "/idVendor";
    FILE *f = fopen(vendor.c_str(), "r");
    if (f) {
      char buf[16] = {0};
      if (fgets(buf, sizeof(buf), f)) {
        fclose(f);
        std::string v = buf;
        if (v.size() > 0 && v[v.size() - 1] == '\n') {
          v.pop_back();
        }
        return v;
      }
      fclose(f);
    }
    size_t slash = dir.rfind('/');
    if (slash == std::string::npos || slash == 0) {
      break;
    }
    dir = dir.substr(0, slash);
  }
  return "";
}

// udev usually creates serial ports as root-owned mode 660 (group
// dialout/uucp), which a normal user's browser cannot open. widen it to 666
// so Web Serial can see it, this needs root or dialout/uucp membership. the
// gadget setup script already does this, so only warn when it's still locked.
static void chmodIfLocked(const char *path)
{
  struct stat st;
  if (stat(path, &st) == 0 && (st.st_mode & 0666) == 0666) {
    return; // already world-accessible (gadget script or udev rule)
  }
  if (chmod(path, 0666) != 0) {
    printf("WARNING: could not chmod 666 %s (%s)\n", path, strerror(errno));
    printf("  the browser can only open it if you are in the dialout/uucp group\n");
    printf("  or a udev rule grants access, for example (get ids from lsusb):\n");
    printf("  SUBSYSTEM==\"tty\", ATTRS{idVendor}==\"1a86\", ATTRS{idProduct}==\"7523\", MODE=\"0666\"\n");
  }
}

std::string VortexCLI::findUsbSerialDevice()
{
  // the browser's Web Serial can only enumerate real serial devices, look for
  // a USB-TTL adapter (or gadget endpoint) to bridge the pty to. ttyGS* is the
  // device side of a synthesized USB gadget (see setupVirtualGadget): the
  // engine talks to it and the browser opens the host-side /dev/ttyACMx.
  for (const char *pattern : {"/dev/ttyGS", "/dev/ttyUSB", "/dev/ttyACM"}) {
    for (int i = 0; i < 32; ++i) {
      char path[128];
      snprintf(path, sizeof(path), "%s%d", pattern, i);
      struct stat st;
      if (stat(path, &st) != 0 || !S_ISCHR(st.st_mode)) {
        continue;
      }
      // skip known-unsuitable devices, like the Valve Steam Controller Puck
      // (a bluetooth receiver whose ACM port is not a usable UART)
      std::string vendor = ttyUsbVendor(path + 5);
      if (vendor == "28de") {
        continue;
      }
      return path;
    }
  }
  return "";
}

#endif // !WASM

// one-time root setup that synthesizes a USB serial device in the kernel with
// no physical hardware: dummy_hcd provides a virtual USB host+gadget pair and
// a composite ACM gadget becomes the port. the host side (/dev/ttyACMx) is what
// the browser's Web Serial opens, the gadget side (/dev/ttyGS0) is what the
// engine talks to. these run inside the tool itself (elevated via sudo) rather
// than via an external shell script.
#ifndef WASM
static bool gadgetWrite(const std::string &path, const std::string &value)
{
  int fd = open(path.c_str(), O_WRONLY | O_CLOEXEC);
  if (fd < 0) {
    return false;
  }
  ssize_t n = write(fd, value.c_str(), value.size());
  close(fd);
  return n == (ssize_t)value.size();
}

static bool gadgetDirExists(const std::string &path)
{
  struct stat st;
  return stat(path.c_str(), &st) == 0 && S_ISDIR(st.st_mode);
}

static std::string findUdc()
{
  DIR *d = opendir("/sys/class/udc");
  if (!d) {
    return "";
  }
  std::string name;
  struct dirent *e = nullptr;
  while ((e = readdir(d)) != nullptr) {
    if (e->d_name[0] == '.') {
      continue;
    }
    name = e->d_name;
    break;
  }
  closedir(d);
  return name;
}

// build the composite ACM gadget tree in configfs. idempotent: if the tree is
// already present it only makes sure the ports are world-writable.
static const char *kGadgetUdevRule = "/etc/udev/rules.d/99-vortex-gadget.rules";

// install a udev rule granting world access to our gadget's tty nodes. without
// it the default tty rule puts them in the uucp group at 0660 and udev re-applies
// that on later events, silently undoing any chmod we do.
static void installGadgetUdevRule()
{
  FILE *f = fopen(kGadgetUdevRule, "w");
  if (!f) {
    return;
  }
  fputs("# Vortex Engine virtual USB gadget: make the tty nodes world-writable\n", f);
  // ttyGS* is the gadget/device side: it does not expose the usb idVendor/Product
  // attributes (those live in the configfs gadget dir, not on the udev parent
  // chain), so match it by name alone. ttyGS* only ever exists while a gadget
  // function is bound, so this is safe.
  fputs("SUBSYSTEM==\"tty\", KERNEL==\"ttyGS*\", MODE=\"0666\"\n", f);
  fputs("SUBSYSTEM==\"tty\", KERNEL==\"ttyACM*\", ATTRS{idVendor}==\"1d6b\", "
        "ATTRS{idProduct}==\"0104\", MODE=\"0666\"\n", f);
  fclose(f);
  // make udev pick it up and re-apply permissions to any matching nodes
  system("udevadm control --reload-rules 2>/dev/null");
  system("udevadm trigger --subsystem-match=tty 2>/dev/null");
  system("udevadm settle 2>/dev/null");
}

static bool doGadgetSetup()
{
  const std::string G = "/sys/kernel/config/usb_gadget/vortex";

  // load the kernel pieces (best-effort; they may already be built in)
  system("modprobe dummy_hcd 2>/dev/null");
  system("modprobe libcomposite 2>/dev/null");
  system("modprobe usb_f_acm 2>/dev/null");

  // get our udev rule in place before the nodes appear so 0666 sticks
  installGadgetUdevRule();

  if (!gadgetDirExists(G)) {
    mkdir(G.c_str(), 0755);
    mkdir((G + "/strings/0x409").c_str(), 0755);
    mkdir((G + "/configs/c.1").c_str(), 0755);
    mkdir((G + "/configs/c.1/strings/0x409").c_str(), 0755);
    mkdir((G + "/functions/acm.usb0").c_str(), 0755);
    gadgetWrite(G + "/idVendor", "0x1d6b");
    gadgetWrite(G + "/idProduct", "0x0104");
    gadgetWrite(G + "/strings/0x409/manufacturer", "lightshow.lol");
    gadgetWrite(G + "/strings/0x409/product", "Vortex Engine");
    gadgetWrite(G + "/configs/c.1/strings/0x409/configuration", "Virtual Serial");
    symlink((G + "/functions/acm.usb0").c_str(), (G + "/configs/c.1/acm.usb0").c_str());
  }

  // bind the first available UDC if not already bound
  bool bound = false;
  {
    FILE *udcFile = fopen((G + "/UDC").c_str(), "r");
    if (udcFile) {
      char buf[128] = {0};
      if (fgets(buf, sizeof(buf), udcFile) && buf[0] != '\0' && buf[0] != '\n') {
        bound = true;
      }
      fclose(udcFile);
    }
  }
  if (!bound) {
    std::string udc = findUdc();
    if (udc.empty()) {
      fprintf(stderr, "no UDC available, dummy_hcd did not load\n");
      return false;
    }
    gadgetWrite(G + "/UDC", udc);
  }

  // wait for the ports to appear (host enumeration can lag) and make them
  // world-writable as they come up, so the browser can open them. host-side
  // enumeration on dummy_hcd can take several seconds, so give it a generous
  // window rather than reporting right away.
  for (int i = 0; i < 150; ++i) {
    if (access("/dev/ttyGS0", F_OK) == 0) {
      chmod("/dev/ttyGS0", 0666);
    }
    bool anyAcm = false;
    for (int a = 0; a < 32; ++a) {
      std::string acm = "/dev/ttyACM" + std::to_string(a);
      if (access(acm.c_str(), F_OK) != 0) {
        continue;
      }
      chmod(acm.c_str(), 0666);
      anyAcm = true;
    }
    if (access("/dev/ttyGS0", F_OK) == 0 && anyAcm) {
      break;
    }
    usleep(100000);
  }

  // final pass: anything that appeared right as the loop ended still needs the
  // world-writable bit so the unprivileged socat/browser can open it (we are
  // root here, so ignore failures on ports that do not exist yet)
  chmod("/dev/ttyGS0", 0666);
  for (int a = 0; a < 32; ++a) {
    chmod(("/dev/ttyACM" + std::to_string(a)).c_str(), 0666);
  }
  return true;
}

// remove the composite ACM gadget so the ttyACM* it created goes away.
static bool doGadgetTeardown()
{
  const std::string G = "/sys/kernel/config/usb_gadget/vortex";
  if (gadgetDirExists(G)) {
    // unbind the UDC first, otherwise the configs/functions below are busy
    gadgetWrite(G + "/UDC", "\n");
    usleep(500000); // give the driver a moment to release the endpoints
    unlink((G + "/configs/c.1/acm.usb0").c_str());
    rmdir((G + "/configs/c.1/strings/0x409").c_str());
    rmdir((G + "/configs/c.1").c_str());
    rmdir((G + "/functions/acm.usb0").c_str());
    rmdir((G + "/strings/0x409").c_str());
    rmdir(G.c_str());
  }
  // the utility gadgets are gone now, so drop the udev rule we installed and
  // reload so it does not linger on the system after the tool stops
  if (unlink(kGadgetUdevRule) == 0 || errno != ENOENT) {
    system("udevadm control --reload-rules 2>/dev/null");
    system("udevadm trigger --subsystem-match=tty 2>/dev/null");
    system("udevadm settle 2>/dev/null");
  }
  return true;
}

// spawn `sudo <this binary> <modeArg>` and wait for it to finish, running the
// configfs operations above as root.
//
// the binary path must be resolved to a real absolute path BEFORE handing it
// to sudo: `/proc/self/exe` is a magic symlink resolved in the context of the
// process that reads it, and after fork/exec that process is sudo itself, so
// sudo would end up running itself with our flag as one of its own options.
static int runSudoSelf(const char *modeArg)
{
  char self[4096];
  ssize_t n = readlink("/proc/self/exe", self, sizeof(self) - 1);
  if (n <= 0) {
    return -1;
  }
  self[n] = '\0';

  pid_t pid = fork();
  if (pid < 0) {
    return -1;
  }
  if (pid == 0) {
    execlp("sudo", "sudo", self, modeArg, (char *)nullptr);
    _exit(127);
  }
  int status = 0;
  if (waitpid(pid, &status, 0) < 0) {
    return -1;
  }
  return WIFEXITED(status) ? WEXITSTATUS(status) : -1;
}
#endif // !WASM

bool VortexCLI::setupVirtualGadget()
{
  if (m_gadgetAttempted) {
    return m_gadgetReady;
  }
  m_gadgetAttempted = true;
#ifndef WASM
  // perform the gadget setup inside the tool itself, elevated via sudo
  int rc = runSudoSelf("--gadget-setup");
  if (rc != 0) {
    printf("Could not create a virtual USB serial device (the setup needs sudo).\n");
    printf("  run the tool with sudo once to synthesize it, or plug in a USB-TTL dongle instead\n");
    m_gadgetReady = false;
    return false;
  }
  m_gadgetReady = true;
  // host-side enumeration can lag a moment after the gadget comes up, so retry
  // before reporting instead of alarming the user that the port is missing
  // when it is about to show up
  std::string hostStr;
  for (int attempt = 0; attempt < 25 && hostStr.empty(); ++attempt) {
    for (int i = 0; i < 32; ++i) {
      char acm[64];
      snprintf(acm, sizeof(acm), "/dev/ttyACM%d", i);
      if (access(acm, F_OK) == 0 && ttyUsbVendor(acm + 5) != "28de") {
        hostStr = acm;
        break;
      }
    }
    if (hostStr.empty()) {
      usleep(200000);
    }
  }
  if (hostStr.empty() && access("/dev/ttyGS0", F_OK) != 0) {
    printf("WARNING: gadget created but /dev/ttyGS0 is missing\n");
  } else {
    // report the state so the user knows which port the browser should list
    printf("Virtual USB serial device up: engine->/dev/ttyGS0, browser->%s\n",
           hostStr.empty() ? "/dev/ttyACM? (none yet)" : hostStr.c_str());
    if (hostStr.empty()) {
      printf("  WARNING: no host-side ttyACM appeared; Web Serial will not show it\n");
    }
  }
  return true;
#else
  return false;
#endif
}

void VortexCLI::teardownVirtualGadget()
{
#ifndef WASM
  if (!m_usingVirtualGadget && !m_gadgetReady) {
    // we never set the gadget up or bridged one, so there is nothing of ours
    // to remove
    return;
  }
  // restore the terminal first: after ctrl-c stdin is still in raw mode and the
  // sudo password prompt won't work, so tear the gadget down on a sane terminal
  restore_terminal();
  int rc = runSudoSelf("--gadget-teardown");
  m_gadgetAttempted = false;
  m_gadgetReady = false;
  m_usingVirtualGadget = false;
  if (rc != 0) {
    printf("WARNING: could not tear down the virtual USB serial device\n");
  } else {
    printf("Virtual USB serial device torn down\n");
  }
#endif
}

bool VortexCLI::startSocatBridge()
{
  if (m_bridgeStarted) {
    return true;
  }
  if (!m_virtualSerial || m_virtualSerial->path() == nullptr) {
    return false;
  }
#ifdef WASM
  return false;
#else
  // the pty is what the engine talks to, socat relays it to the real device.
  // VORTEX_BRIDGE_DEVICE overrides auto-detection (useful for testing and for
  // picking a specific port when several adapters are plugged in)
  std::string device;
  const char *bridgeOverride = getenv("VORTEX_BRIDGE_DEVICE");
  if (bridgeOverride && bridgeOverride[0] != '\0') {
    device = bridgeOverride;
  } else {
    device = findUsbSerialDevice();
    if (device.compare(0, 10, "/dev/ttyGS") == 0) {
      // this is the device side of our synthesized gadget (whether created just
      // now or left over by an earlier run): always (re-)run the idempotent root
      // setup so both sides are world-writable, otherwise socat/the browser get
      // a root:uucp 660 port it cannot open
      setupVirtualGadget();
    } else if (device.empty() && setupVirtualGadget()) {
      // no physical dongle: the kernel gadget now provides a real serial port
      device = findUsbSerialDevice();
    }
  }
  if (device.empty()) {
    return false;
  }
  // bridging the device side of a synthesized gadget means we own that gadget,
  // even if a previous run created it, so it must be torn down on exit
  if (device.compare(0, 10, "/dev/ttyGS") == 0) {
    m_usingVirtualGadget = true;
  }
  // widen permissions on the bridge target. when bridging the device side of
  // the synthesized gadget (ttyGS0) the browser opens the host side (ttyACMx),
  // so that needs opening up too.
  chmodIfLocked(device.c_str());
  if (device.compare(0, 10, "/dev/ttyGS") == 0) {
    for (int i = 0; i < 32; ++i) {
      char acm[64];
      snprintf(acm, sizeof(acm), "/dev/ttyACM%d", i);
      struct stat st;
      if (stat(acm, &st) == 0 && S_ISCHR(st.st_mode) && ttyUsbVendor(acm + 5) != "28de") {
        chmodIfLocked(acm);
      }
    }
  }
  std::string optStr = device + ",b115200,raw,echo=0,clocal=1";
  pid_t pid = fork();
  if (pid < 0) {
    printf("Failed to fork socat bridge\n");
    return false;
  }
  if (pid == 0) {
    // child: exec socat, replace our image with it
    execlp("socat", "socat", m_virtualSerial->path(), optStr.c_str(), nullptr);
    _exit(127);
  }
  m_socatPid = pid;
  m_bridgeStarted = true;
  printf("Bridging %s to %s via socat\n", m_virtualSerial->path(), device.c_str());
  return true;
#endif
}

void VortexCLI::pollBridge()
{
#ifndef WASM
  // if socat died (device unplugged) or never started, keep looking for a
  // USB serial device silently so the user can plug one in after startup
  if (m_bridgeStarted) {
    int status = 0;
    if (m_socatPid > 0 && waitpid(m_socatPid, &status, WNOHANG) == m_socatPid) {
      m_socatPid = -1;
      m_bridgeStarted = false;
    }
  }
  if (m_bridgeStarted) {
    return;
  }
  // don't hammer the filesystem on every tick, scan once a second
  uint32_t now = Time::getCurtime();
  if (m_bridgeScanTick && (now - m_bridgeScanTick) < 1000) {
    return;
  }
  m_bridgeScanTick = now;
  startSocatBridge();
#endif
}

// main function to run the CLI
int main(int argc, char *argv[])
{
  // hidden elevated modes that perform the virtual USB gadget setup/teardown
  // in-process: the running (user) instance re-execs itself as root for these
  for (int i = 1; i < argc; ++i) {
    if (strcmp(argv[i], "--gadget-setup") == 0) {
#ifdef WASM
      return 1;
#else
      return doGadgetSetup() ? 0 : 1;
#endif
    }
    if (strcmp(argv[i], "--gadget-teardown") == 0) {
#ifdef WASM
      return 1;
#else
      return doGadgetTeardown() ? 0 : 1;
#endif
    }
  }
  VortexCLI cli;
  cli.init(argc, argv);
#ifndef WASM
  while (cli.stillRunning()) {
    cli.run();
  }
#endif
  return 0;
}
