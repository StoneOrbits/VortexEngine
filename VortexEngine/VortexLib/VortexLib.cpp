#include "VortexLib.h"

// VortexEngine includes
#include "VortexEngine.h"
#include "Buttons/Button.h"
#include "Buttons/Buttons.h"
#include "Serial/ByteStream.h"
#include "Wireless/IRReceiver.h"
#include "Wireless/VLReceiver.h"
#include "Wireless/VLSender.h"
#include "Modes/DefaultModes.h"
#include "Memory/Memory.h"
#include "Patterns/PatternBuilder.h"
#include "Time/TimeControl.h"
#include "Patterns/Pattern.h"
#include "Colors/Colorset.h"
#include "Storage/Storage.h"
#include "Random/Random.h"
#include "Time/Timings.h"
#include "Menus/Menus.h"
#include "Modes/Modes.h"
#include "Menus/Menu.h"
#include "Modes/Mode.h"
#include "Random/Random.h"

#ifndef _WIN32
#include <sys/ioctl.h>
#include <unistd.h>
#include <stdio.h>
#else
#include <Windows.h>
#endif

#include <algorithm>
#include <iostream>
#include <sstream>
#include <fstream>

#ifdef WASM
#include <emscripten/bind.h>
#include <emscripten/val.h>

using namespace emscripten;

RGBColor *leds = nullptr;
int led_count = 0;

// Assuming VortexCallbacks and ColorInfo are correctly defined and included above this line
class VortexWASMCallbacks : public VortexCallbacks {
  public:
    void ledsInit(void *cl, int count) override {
      leds = (RGBColor *)cl;
      led_count = count;
    }
};

static void init_vortex()
{
  Vortex::initEx<VortexWASMCallbacks>();
}

static void cleanup_vortex()
{
  Vortex::cleanup();
}

// This wraps Vortex::tick but returns an array of led colors for the tick
val tick_vortex() {
  // first run a tick
  Vortex::tick();
  // then extract the color that was produced by the tick
  val ledArray = val::array();
  for (uint32_t i = 0; i < led_count; ++i) {
    val color = val::object();
    color.set("red", leds[i].red);
    color.set("green", leds[i].green);
    color.set("blue", leds[i].blue);
    ledArray.set(i, color);
  }
  return ledArray;
}

emscripten::val getDataArray(const ByteStream &byteStream)
{
  const uint8_t *dataPtr = ByteStream_data(&byteStream);
  uint32_t size = ByteStream_size(&byteStream);

  emscripten::val dataArray = emscripten::val::array();

  for (uint32_t i = 0; i < size; ++i) {
    dataArray.call<void>("push", dataPtr[i]);
  }

  return dataArray;
}

emscripten::val getRawDataArray(const ByteStream &byteStream)
{
  const uint8_t *rawDataPtr = reinterpret_cast<const uint8_t *>(ByteStream_rawData(&byteStream));
  uint32_t rawSize = ByteStream_rawSize(&byteStream);

  emscripten::val rawDataArray = emscripten::val::array();

  for (uint32_t i = 0; i < rawSize; ++i) {
    rawDataArray.call<void>("push", rawDataPtr[i]);
  }

  return rawDataArray;
}

bool createByteStreamFromData(val bytesArray, ByteStream &stream)
{
  // Convert val to std::vector<uint8_t>
  std::vector<uint8_t> data = vecFromJSArray<uint8_t>(bytesArray);

  // Assuming size is part of the data or passed separately
  size_t size = data.size();
  if (!ByteStream_rawInit(&stream, data.data(), size)) {
    return true;
  }
  return false;
}

// js is dumb and has issues doing this conversion I guess
PatternID intToPatternID(int val)
{
  return (PatternID)val;
}

// embind deletes the objects it owns, ByteStreams must release their arena
// chunk first and are heap allocated so the arena owner never moves
namespace emscripten {
namespace internal {
template<>
void raw_destructor<ByteStream>(ByteStream *ptr)
{
  ByteStream_destroy(ptr);
  delete ptr;
}
}
}

// constructor wrappers for the C style init functions
static ByteStream *wasm_ByteStream_new()
{
  ByteStream *stream = new ByteStream;
  ByteStream_make(stream);
  return stream;
}

static ByteStream *wasm_ByteStream_newBuf(uint32_t size, const uint8_t *buf)
{
  ByteStream *stream = new ByteStream;
  ByteStream_makeBuf(stream, size, buf);
  return stream;
}

static Colorset wasm_Colorset_new()
{
  Colorset set;
  Colorset_make(&set);
  return set;
}

static Colorset wasm_Colorset_newRGB(RGBColor c1, RGBColor c2, RGBColor c3, RGBColor c4,
  RGBColor c5, RGBColor c6, RGBColor c7, RGBColor c8)
{
  Colorset set;
  Colorset_makeRGB(&set, c1, c2, c3, c4, c5, c6, c7, c8);
  return set;
}

static Colorset wasm_Colorset_newCols(uint8_t numCols, const uint32_t *cols)
{
  Colorset set;
  Colorset_makeCols(&set, numCols, cols);
  return set;
}

#define WASM_ARGS_P0
#define WASM_ARGS_P1 uint8_t a1
#define WASM_ARGS_P2 WASM_ARGS_P1, uint8_t a2
#define WASM_ARGS_P3 WASM_ARGS_P2, uint8_t a3
#define WASM_ARGS_P4 WASM_ARGS_P3, uint8_t a4
#define WASM_ARGS_P5 WASM_ARGS_P4, uint8_t a5
#define WASM_ARGS_P6 WASM_ARGS_P5, uint8_t a6
#define WASM_ARGS_P7 WASM_ARGS_P6, uint8_t a7
#define WASM_ARGS_P8 WASM_ARGS_P7, uint8_t a8
#define WASM_ARGS_A0
#define WASM_ARGS_A1 , a1
#define WASM_ARGS_A2 WASM_ARGS_A1, a2
#define WASM_ARGS_A3 WASM_ARGS_A2, a3
#define WASM_ARGS_A4 WASM_ARGS_A3, a4
#define WASM_ARGS_A5 WASM_ARGS_A4, a5
#define WASM_ARGS_A6 WASM_ARGS_A5, a6
#define WASM_ARGS_A7 WASM_ARGS_A6, a7
#define WASM_ARGS_A8 WASM_ARGS_A7, a8
#define WASM_PATTERN_ARGS_NEW(n) \
  static PatternArgs wasm_PatternArgs_new##n(WASM_ARGS_P##n) \
  { \
    PatternArgs args; \
    PatternArgs_make##n(&args WASM_ARGS_A##n); \
    return args; \
  }
WASM_PATTERN_ARGS_NEW(0)
WASM_PATTERN_ARGS_NEW(1)
WASM_PATTERN_ARGS_NEW(2)
WASM_PATTERN_ARGS_NEW(3)
WASM_PATTERN_ARGS_NEW(4)
WASM_PATTERN_ARGS_NEW(5)
WASM_PATTERN_ARGS_NEW(6)
WASM_PATTERN_ARGS_NEW(7)
WASM_PATTERN_ARGS_NEW(8)

// the anonymous enums that hold the LedPos and PatternID values
typedef decltype(LED_FIRST) LedPosEnum;
typedef decltype(PATTERN_NONE) PatternIDEnum;

// placeholder types to group the static module functions in js
struct PatternBuilderBinding {};
struct MenusBinding {};
struct ModesBinding {};

EMSCRIPTEN_BINDINGS(Vortex) {
  // vector<string>
  register_vector<std::string>("VectorString");

  // basic control functions
  function("Init", &init_vortex);
  function("Cleanup", &cleanup_vortex);
  function("Tick", &tick_vortex);

  // Bind the HSVColor class
  class_<HSVColor>("HSVColor")
    .constructor(&HSVColor_make)
    .constructor(&HSVColor_makeHSV)
    .constructor(&HSVColor_fromU32)
    .function("empty", &HSVColor_empty, allow_raw_pointers())
    .function("clear", &HSVColor_clear, allow_raw_pointers())
    .function("raw", &HSVColor_raw, allow_raw_pointers())
    .property("hue", &HSVColor::hue)
    .property("sat", &HSVColor::sat)
    .property("val", &HSVColor::val);

  // Bind the RGBColor class
  class_<RGBColor>("RGBColor")
    .constructor(&RGBColor_make)
    .constructor(&RGBColor_makeRGB)
    .constructor(&RGBColor_fromU32)
    .function("empty", &RGBColor_empty, allow_raw_pointers())
    .function("clear", &RGBColor_clear, allow_raw_pointers())
    .function("adjustBrightness", &RGBColor_adjustBrightness, allow_raw_pointers())
    .function("serialize", &RGBColor_serialize, allow_raw_pointers())
    .function("unserialize", &RGBColor_unserialize, allow_raw_pointers())
    .function("raw", &RGBColor_raw, allow_raw_pointers())
    .property("red", &RGBColor::red)
    .property("green", &RGBColor::green)
    .property("blue", &RGBColor::blue);

  // Bind the utility conversion functions
  function("hsv_to_rgb_rainbow", &hsv_to_rgb_rainbow, allow_raw_pointers());
  function("hsv_to_rgb_raw_C", &hsv_to_rgb_raw_C, allow_raw_pointers());
  function("hsv_to_rgb_generic", &hsv_to_rgb_generic, allow_raw_pointers());
  function("rgb_to_hsv_approx", &rgb_to_hsv_approx, allow_raw_pointers());
  function("rgb_to_hsv_generic", &rgb_to_hsv_generic, allow_raw_pointers());

  class_<ByteStream>("ByteStream")
    .constructor(&wasm_ByteStream_new, allow_raw_pointers())
    .constructor(&wasm_ByteStream_newBuf, allow_raw_pointers())

    // member functions
    .function("rawInit", &ByteStream_rawInit, allow_raw_pointers())
    .function("init", &ByteStream_initBuf, allow_raw_pointers())
    .function("clear", &ByteStream_clear, allow_raw_pointers())
    .function("shrink", &ByteStream_shrink, allow_raw_pointers())
    .function("append", &ByteStream_append, allow_raw_pointers())
    .function("extend", &ByteStream_extend, allow_raw_pointers())
    .function("trim", &ByteStream_trim, allow_raw_pointers())
    .function("compress", &ByteStream_compress, allow_raw_pointers())
    .function("decompress", &ByteStream_decompress, allow_raw_pointers())
    .function("recalcCRC", &ByteStream_recalcCRC, allow_raw_pointers())
    .function("sanity", &ByteStream_sanity, allow_raw_pointers())
    .function("checkCRC", &ByteStream_checkCRC, allow_raw_pointers())
    .function("isCRCDirty", &ByteStream_isCRCDirty, allow_raw_pointers())
    .function("serialize8", &ByteStream_serialize8, allow_raw_pointers())
    .function("serialize16", &ByteStream_serialize16, allow_raw_pointers())
    .function("serialize32", &ByteStream_serialize32, allow_raw_pointers())
    .function("resetUnserializer", &ByteStream_resetUnserializer, allow_raw_pointers())
    .function("moveUnserializer", &ByteStream_moveUnserializer, allow_raw_pointers())
    .function("unserializerAtEnd", &ByteStream_unserializerAtEnd, allow_raw_pointers())
    // TODO: provide better apis here
    //.function("unserialize8", &ByteStream_unserialize8)
    //.function("unserialize16", &ByteStream_unserialize16)
    //.function("unserialize32", &ByteStream_unserialize32)
    .function("peek8", &ByteStream_peek8, allow_raw_pointers())
    .function("peek16", &ByteStream_peek16, allow_raw_pointers())
    .function("peek32", &ByteStream_peek32, allow_raw_pointers())
    .function("data", &ByteStream_data, allow_raw_pointers())
    .function("rawData", &ByteStream_rawData, allow_raw_pointers())
    .function("rawSize", &ByteStream_rawSize, allow_raw_pointers())
    .function("size", &ByteStream_size, allow_raw_pointers())
    .function("capacity", &ByteStream_capacity, allow_raw_pointers())
    .function("is_compressed", &ByteStream_is_compressed, allow_raw_pointers())
    .function("CRC", &ByteStream_CRC, allow_raw_pointers());

  // Binding static enum values
  // LedPos and PatternID are plain integers now so they are exposed to js as numbers
  enum_<LedPosEnum>("LedPos", enum_value_type::number)
    .value("LED_FIRST", (LedPosEnum)LED_FIRST)
    .value("LED_0", (LedPosEnum)LED_0)
    .value("LED_1", (LedPosEnum)LED_1)
    .value("LED_COUNT", (LedPosEnum)LED_COUNT)
    .value("LED_LAST", (LedPosEnum)LED_LAST)
    .value("LED_ALL", (LedPosEnum)LED_ALL)
    .value("LED_MULTI", (LedPosEnum)LED_MULTI)
    .value("LED_ALL_SINGLE", (LedPosEnum)LED_ALL_SINGLE)
    .value("LED_ANY", (LedPosEnum)LED_ANY);

  function("intToPatternID", &intToPatternID);
  function("isMultiLedPatternID", &isMultiLedPatternID);
  function("isSingleLedPatternID", &isSingleLedPatternID);

  enum_<PatternIDEnum>("PatternID", enum_value_type::number)
    // Meta Constants
    .value("PATTERN_NONE", (PatternIDEnum)PATTERN_NONE)

    // single led patterns
    .value("PATTERN_STROBE", (PatternIDEnum)PATTERN_STROBE)
    .value("PATTERN_HYPERSTROBE", (PatternIDEnum)PATTERN_HYPERSTROBE)
    .value("PATTERN_PICOSTROBE", (PatternIDEnum)PATTERN_PICOSTROBE)
    .value("PATTERN_STROBIE", (PatternIDEnum)PATTERN_STROBIE)
    .value("PATTERN_DOPS", (PatternIDEnum)PATTERN_DOPS)
    .value("PATTERN_ULTRADOPS", (PatternIDEnum)PATTERN_ULTRADOPS)
    .value("PATTERN_STROBEGAP", (PatternIDEnum)PATTERN_STROBEGAP)
    .value("PATTERN_HYPERGAP", (PatternIDEnum)PATTERN_HYPERGAP)
    .value("PATTERN_PICOGAP", (PatternIDEnum)PATTERN_PICOGAP)
    .value("PATTERN_STROBIEGAP", (PatternIDEnum)PATTERN_STROBIEGAP)
    .value("PATTERN_DOPSGAP", (PatternIDEnum)PATTERN_DOPSGAP)
    .value("PATTERN_ULTRAGAP", (PatternIDEnum)PATTERN_ULTRAGAP)
    .value("PATTERN_BLINKIE", (PatternIDEnum)PATTERN_BLINKIE)
    .value("PATTERN_GHOSTCRUSH", (PatternIDEnum)PATTERN_GHOSTCRUSH)
    .value("PATTERN_DOUBLEDOPS", (PatternIDEnum)PATTERN_DOUBLEDOPS)
    .value("PATTERN_CHOPPER", (PatternIDEnum)PATTERN_CHOPPER)
    .value("PATTERN_DASHGAP", (PatternIDEnum)PATTERN_DASHGAP)
    .value("PATTERN_DASHDOPS", (PatternIDEnum)PATTERN_DASHDOPS)
    .value("PATTERN_DASHCRUSH", (PatternIDEnum)PATTERN_DASHCRUSH)
    .value("PATTERN_ULTRADASH", (PatternIDEnum)PATTERN_ULTRADASH)
    .value("PATTERN_GAPCYCLE", (PatternIDEnum)PATTERN_GAPCYCLE)
    .value("PATTERN_DASHCYCLE", (PatternIDEnum)PATTERN_DASHCYCLE)
    .value("PATTERN_TRACER", (PatternIDEnum)PATTERN_TRACER)
    .value("PATTERN_RIBBON", (PatternIDEnum)PATTERN_RIBBON)
    .value("PATTERN_MINIRIBBON", (PatternIDEnum)PATTERN_MINIRIBBON)
    .value("PATTERN_BLEND", (PatternIDEnum)PATTERN_BLEND)
    .value("PATTERN_BLENDSTROBE", (PatternIDEnum)PATTERN_BLENDSTROBE)
    .value("PATTERN_BLENDSTROBEGAP", (PatternIDEnum)PATTERN_BLENDSTROBEGAP)
    .value("PATTERN_COMPLEMENTARY_BLEND", (PatternIDEnum)PATTERN_COMPLEMENTARY_BLEND)
    .value("PATTERN_COMPLEMENTARY_BLENDSTROBE", (PatternIDEnum)PATTERN_COMPLEMENTARY_BLENDSTROBE)
    .value("PATTERN_COMPLEMENTARY_BLENDSTROBEGAP", (PatternIDEnum)PATTERN_COMPLEMENTARY_BLENDSTROBEGAP)
    .value("PATTERN_SOLID", (PatternIDEnum)PATTERN_SOLID)
    // multi led patterns
    .value("PATTERN_HUE_SCROLL", (PatternIDEnum)PATTERN_HUE_SCROLL)
    .value("PATTERN_THEATER_CHASE", (PatternIDEnum)PATTERN_THEATER_CHASE)
    .value("PATTERN_CHASER", (PatternIDEnum)PATTERN_CHASER)
    .value("PATTERN_ZIGZAG", (PatternIDEnum)PATTERN_ZIGZAG)
    .value("PATTERN_ZIPFADE", (PatternIDEnum)PATTERN_ZIPFADE)
    .value("PATTERN_DRIP", (PatternIDEnum)PATTERN_DRIP)
    .value("PATTERN_DRIPMORPH", (PatternIDEnum)PATTERN_DRIPMORPH)
    .value("PATTERN_CROSSDOPS", (PatternIDEnum)PATTERN_CROSSDOPS)
    .value("PATTERN_DOUBLESTROBE", (PatternIDEnum)PATTERN_DOUBLESTROBE)
    .value("PATTERN_METEOR", (PatternIDEnum)PATTERN_METEOR)
    .value("PATTERN_SPARKLETRACE", (PatternIDEnum)PATTERN_SPARKLETRACE)
    .value("PATTERN_VORTEXWIPE", (PatternIDEnum)PATTERN_VORTEXWIPE)
    .value("PATTERN_WARP", (PatternIDEnum)PATTERN_WARP)
    .value("PATTERN_WARPWORM", (PatternIDEnum)PATTERN_WARPWORM)
    .value("PATTERN_SNOWBALL", (PatternIDEnum)PATTERN_SNOWBALL)
    .value("PATTERN_LIGHTHOUSE", (PatternIDEnum)PATTERN_LIGHTHOUSE)
    .value("PATTERN_PULSISH", (PatternIDEnum)PATTERN_PULSISH)
    .value("PATTERN_FILL", (PatternIDEnum)PATTERN_FILL)
    .value("PATTERN_BOUNCE", (PatternIDEnum)PATTERN_BOUNCE)
    .value("PATTERN_SPLITSTROBIE", (PatternIDEnum)PATTERN_SPLITSTROBIE)
    .value("PATTERN_BACKSTROBE", (PatternIDEnum)PATTERN_BACKSTROBE)
    .value("PATTERN_VORTEX", (PatternIDEnum)PATTERN_VORTEX)
    .value("PATTERN_COUNT", (PatternIDEnum)PATTERN_COUNT);
    // meta constants
    //.value("PATTERN_FIRST", (PatternIDEnum)PATTERN_FIRST)
    //.value("PATTERN_SINGLE_FIRST", (PatternIDEnum)PATTERN_SINGLE_FIRST)
    //.value("PATTERN_MULTI_FIRST", (PatternIDEnum)PATTERN_MULTI_FIRST)
    //.value("PATTERN_SINGLE_LAST", (PatternIDEnum)PATTERN_SINGLE_LAST)
    //.value("PATTERN_SINGLE_COUNT", (PatternIDEnum)PATTERN_SINGLE_COUNT)
    //.value("PATTERN_MULTI_LAST", (PatternIDEnum)PATTERN_MULTI_LAST)
    //.value("PATTERN_MULTI_COUNT", (PatternIDEnum)PATTERN_MULTI_COUNT)
    //.value("PATTERN_LAST", (PatternIDEnum)PATTERN_LAST)

  enum_<MenuEntryID>("MenuEntryID")
    .value("MENU_NONE", MenuEntryID::MENU_NONE)
    .value("MENU_FIRST", MenuEntryID::MENU_FIRST)
    .value("MENU_RANDOMIZER", MenuEntryID::MENU_RANDOMIZER)
    .value("MENU_MODE_SHARING", MenuEntryID::MENU_MODE_SHARING)
#if ENABLE_EDITOR_CONNECTION == 1
    .value("MENU_EDITOR_CONNECTION", MenuEntryID::MENU_EDITOR_CONNECTION)
#endif
    .value("MENU_COLOR_SELECT", MenuEntryID::MENU_COLOR_SELECT)
    .value("MENU_PATTERN_SELECT", MenuEntryID::MENU_PATTERN_SELECT)
    .value("MENU_GLOBAL_BRIGHTNESS", MenuEntryID::MENU_GLOBAL_BRIGHTNESS)
    .value("MENU_FACTORY_RESET", MenuEntryID::MENU_FACTORY_RESET);

  class_<Colorset>("Colorset")
    .constructor(&wasm_Colorset_new)
    .constructor(&wasm_Colorset_newRGB)
    .constructor(&wasm_Colorset_newCols, allow_raw_pointers())
    .function("init", &Colorset_init, allow_raw_pointers())
    .function("clear", &Colorset_clear, allow_raw_pointers())
    .function("equals", &Colorset_equals, allow_raw_pointers())
    .function("get", &Colorset_get, allow_raw_pointers())
    .function("set", &Colorset_setColor, allow_raw_pointers())
    .function("skip", &Colorset_skip, allow_raw_pointers())
    .function("cur", &Colorset_cur, allow_raw_pointers())
    .function("setCurIndex", &Colorset_setCurIndex, allow_raw_pointers())
    .function("resetIndex", &Colorset_resetIndex, allow_raw_pointers())
    .function("curIndex", &Colorset_curIndex, allow_raw_pointers())
    .function("getPrev", &Colorset_getPrev, allow_raw_pointers())
    .function("getNext", &Colorset_getNext, allow_raw_pointers())
    .function("peek", &Colorset_peek, allow_raw_pointers())
    .function("peekNext", &Colorset_peekNext, allow_raw_pointers())
    .function("numColors", &Colorset_numColors, allow_raw_pointers())
    .function("onStart", &Colorset_onStart, allow_raw_pointers())
    .function("onEnd", &Colorset_onEnd, allow_raw_pointers())
    .function("serialize", &Colorset_serialize, allow_raw_pointers())
    .function("unserialize", &Colorset_unserialize, allow_raw_pointers())
    .function("addColor", &Colorset_addColor, allow_raw_pointers())
    .function("addColorHSV", &Colorset_addColorHSV, allow_raw_pointers())
    .function("randomize", &Colorset_randomize, allow_raw_pointers())
    .function("adjustBrightness", &Colorset_adjustBrightness, allow_raw_pointers())
    .function("removeColor", &Colorset_removeColor, allow_raw_pointers())
    .function("operator[]", &Colorset_at, allow_raw_pointers())
    // If more specific or additional randomize functions are needed, they can be added here as well
    .function("randomizeSolid", &Colorset_randomizeSolid, allow_raw_pointers())
    .function("randomizeComplimentary", &Colorset_randomizeComplimentary, allow_raw_pointers())
    .function("randomizeTriadic", &Colorset_randomizeTriadic, allow_raw_pointers())
    .function("randomizeSquare", &Colorset_randomizeSquare, allow_raw_pointers())
    .function("randomizePentadic", &Colorset_randomizePentadic, allow_raw_pointers())
    .function("randomizeRainbow", &Colorset_randomizeRainbow, allow_raw_pointers());

  class_<PatternArgs>("PatternArgs")
    .constructor(&wasm_PatternArgs_new0)
    .constructor(&wasm_PatternArgs_new1)
    .constructor(&wasm_PatternArgs_new2)
    .constructor(&wasm_PatternArgs_new3)
    .constructor(&wasm_PatternArgs_new4)
    .constructor(&wasm_PatternArgs_new5)
    .constructor(&wasm_PatternArgs_new6)
    .constructor(&wasm_PatternArgs_new7)
    .constructor(&wasm_PatternArgs_new8)
    .function("init", &PatternArgs_init0, allow_raw_pointers())
    .function("init", &PatternArgs_init1, allow_raw_pointers())
    .function("init", &PatternArgs_init2, allow_raw_pointers())
    .function("init", &PatternArgs_init3, allow_raw_pointers())
    .function("init", &PatternArgs_init4, allow_raw_pointers())
    .function("init", &PatternArgs_init5, allow_raw_pointers())
    .function("init", &PatternArgs_init6, allow_raw_pointers())
    .function("init", &PatternArgs_init7, allow_raw_pointers())
    .function("init", &PatternArgs_init8, allow_raw_pointers())
    .function("addArgs", &PatternArgs_addArgs1, allow_raw_pointers())
    .function("addArgs", &PatternArgs_addArgs2, allow_raw_pointers())
    .function("addArgs", &PatternArgs_addArgs3, allow_raw_pointers())
    .function("addArgs", &PatternArgs_addArgs4, allow_raw_pointers())
    .function("addArgs", &PatternArgs_addArgs5, allow_raw_pointers())
    .function("addArgs", &PatternArgs_addArgs6, allow_raw_pointers())
    .function("addArgs", &PatternArgs_addArgs7, allow_raw_pointers())
    .function("addArgs", &PatternArgs_addArgs8, allow_raw_pointers())
    .property("arg1", &PatternArgs::arg1)
    .property("arg2", &PatternArgs::arg2)
    .property("arg3", &PatternArgs::arg3)
    .property("arg4", &PatternArgs::arg4)
    .property("arg5", &PatternArgs::arg5)
    .property("arg6", &PatternArgs::arg6)
    .property("arg7", &PatternArgs::arg7)
    .property("arg8", &PatternArgs::arg8);

  class_<Pattern>("Pattern")
    .function("bind", &Pattern_bind, allow_raw_pointers())
    .function("init", &Pattern_init, allow_raw_pointers())
    .function("serialize", &Pattern_serialize, allow_raw_pointers())
    .function("unserialize", &Pattern_unserialize, allow_raw_pointers())
    .function("setArg", &Pattern_setArg, allow_raw_pointers())
    .function("getArg", &Pattern_getArg, allow_raw_pointers())
    .function("setArgs", &Pattern_setArgs, allow_raw_pointers())
    .function("getArgs", &Pattern_getArgs, allow_raw_pointers())
    .function("getNumArgs", &Pattern_getNumArgs, allow_raw_pointers())
    .function("equals", &Pattern_equals, allow_raw_pointers())
    .function("getColorset", &Pattern_getColorset, allow_raw_pointers())
    .function("setColorset", &Pattern_setColorset, allow_raw_pointers())
    .function("clearColorset", &Pattern_clearColorset, allow_raw_pointers())
    .function("setLedPos", &Pattern_setLedPos, allow_raw_pointers())
    .function("getPatternID", &Pattern_getPatternID, allow_raw_pointers())
    .function("getLedPos", &Pattern_getLedPos, allow_raw_pointers())
    .function("getFlags", &Pattern_getFlags, allow_raw_pointers())
    .function("hasFlags", &Pattern_hasFlags, allow_raw_pointers());

  class_<PatternBuilderBinding>("PatternBuilder")
    .class_function("make", &PatternBuilder_make, allow_raw_pointers())
    .class_function("dupe", &PatternBuilder_dupe, allow_raw_pointers())
    .class_function("makeSingle", &PatternBuilder_makeSingle, allow_raw_pointers())
    .class_function("makeMulti", &PatternBuilder_makeMulti, allow_raw_pointers())
    //.function("unserialize", &PatternBuilder_unserialize)
    .class_function("getDefaultArgs", &PatternBuilder_getDefaultArgs, allow_raw_pointers())
    .class_function("numDefaultArgs", &PatternBuilder_numDefaultArgs);

  class_<Mode>("Mode")
    //.constructor<>()
    // overloading only works with param count not typing
    //.constructor<PatternID, const Colorset &>()
    //.constructor<PatternID, const PatternArgs &, const Colorset &>()
    //.constructor<PatternID, const PatternArgs *, const Colorset *>()
    //.constructor<const Mode *>()
    .function("copyFrom", &Mode_assign, allow_raw_pointers())
    .function("equals", &Mode_opEquals, allow_raw_pointers())
    .function("notEquals", &Mode_opNotEquals, allow_raw_pointers())
    .function("init", &Mode_initPatterns, allow_raw_pointers())
    .function("play", &Mode_play, allow_raw_pointers())
    .function("saveToBuffer", &Mode_saveToBuffer, allow_raw_pointers())
    .function("loadFromBuffer", &Mode_loadFromBuffer, allow_raw_pointers())
    .function("serialize", &Mode_serialize, allow_raw_pointers())
    .function("unserialize", &Mode_unserialize, allow_raw_pointers())
    .function("equalsMode", &Mode_equals, allow_raw_pointers())
    //.function("setLedCount", &Mode_setLedCount)
    .function("getLedCount", &Mode_getLedCount, allow_raw_pointers())
    .function("getColorset", &Mode_getColorset, allow_raw_pointers())
    .function("getPatternID", &Mode_getPatternID, allow_raw_pointers())
    .function("getPattern", &Mode_getPatternMutable, allow_raw_pointers())
    .function("getConstPattern", &Mode_getPattern, allow_raw_pointers())
    .function("setPattern", &Mode_setPattern, allow_raw_pointers())
    .function("setPattern2", &Mode_setPattern, allow_raw_pointers())
    .function("setColorset", &Mode_setColorset, allow_raw_pointers())
    .function("clearPattern", &Mode_clearPattern, allow_raw_pointers())
    .function("clearColorset", &Mode_clearColorset, allow_raw_pointers())
    .function("setArg", &Mode_setArg, allow_raw_pointers())
    .function("getArg", &Mode_getArg, allow_raw_pointers())
    .function("hasMultiLed", &Mode_hasMultiLed, allow_raw_pointers())
    .function("hasSingleLed", &Mode_hasSingleLed, allow_raw_pointers())
    .function("hasSameSingleLed", &Mode_hasSameSingleLed, allow_raw_pointers())
    .function("hasSparseSingleLed", &Mode_hasSparseSingleLed, allow_raw_pointers())
    .function("isEmpty", &Mode_isEmpty, allow_raw_pointers())
    //.function("getSingleLedMap", &Mode_getSingleLedMap)
    .function("isMultiLed", &Mode_isMultiLed, allow_raw_pointers());

  class_<MenusBinding>("Menus")
    .class_function("init", &Menus_init)
    .class_function("cleanup", &Menus_cleanup)
    .class_function("run", &Menus_run)
    .class_function("openMenuSelection", &Menus_openMenuSelection)
    .class_function("openMenu", &Menus_openMenu)
    .class_function("showSelection", &Menus_showSelection)
    .class_function("checkOpen", &Menus_checkOpen)
    .class_function("checkInMenu", &Menus_checkInMenu)
    .class_function("curMenu", &Menus_curMenu, allow_raw_pointers())
    .class_function("curMenuID", &Menus_curMenuID);

  class_<Menu>("Menu")
    .function("init", &Menu_init, allow_raw_pointers())
    .function("run", &Menu_run, allow_raw_pointers())
    .function("onLedSelected", &Menu_onLedSelected, allow_raw_pointers())
    .function("onShortClick", &Menu_onShortClick, allow_raw_pointers())
    .function("onLongClick", &Menu_onLongClick, allow_raw_pointers())
    .function("leaveMenu", &Menu_leaveMenu, allow_raw_pointers());
    //.function("setTargetLeds", &Menu_setTargetLeds);

  class_<ModesBinding>("Modes")
    .class_function("init", &Modes_init)
    .class_function("cleanup", &Modes_cleanup)
    .class_function("play", &Modes_play)
    .class_function("saveToBuffer", &Modes_saveToBuffer, allow_raw_pointers())
    .class_function("loadFromBuffer", &Modes_loadFromBuffer, allow_raw_pointers())
    .class_function("loadStorage", &Modes_loadStorage)
    .class_function("saveStorage", &Modes_saveStorage)
    .class_function("serialize", &Modes_serialize, allow_raw_pointers())
    .class_function("unserialize", &Modes_unserialize, allow_raw_pointers())
    .class_function("setDefaults", &Modes_setDefaults)
    .class_function("shiftCurMode", &Modes_shiftCurMode)
    .class_function("updateCurMode", &Modes_updateCurMode, allow_raw_pointers())
    .class_function("setCurMode", &Modes_setCurMode, allow_raw_pointers())
    .class_function("curMode", &Modes_curMode, allow_raw_pointers())
    .class_function("nextMode", &Modes_nextMode, allow_raw_pointers())
    .class_function("previousMode", &Modes_previousMode, allow_raw_pointers())
    .class_function("nextModeSkipEmpty", &Modes_nextModeSkipEmpty, allow_raw_pointers())
    .class_function("numModes", &Modes_numModes)
    .class_function("curModeIndex", &Modes_curModeIndex)
    .class_function("lastSwitchTime", &Modes_lastSwitchTime)
    .class_function("deleteCurMode", &Modes_deleteCurMode)
    .class_function("clearModes", &Modes_clearModes)
    .class_function("setStartupMode", &Modes_setStartupMode)
    .class_function("startupMode", &Modes_startupMode)
    //.class_function("initCurMode", &Modes_initCurMode, allow_raw_pointers())
    //.class_function("saveCurMode", &Modes_saveCurMode)
    .class_function("setFlag", &Modes_setFlag)
    .class_function("getFlag", &Modes_getFlag)
    .class_function("resetFlags", &Modes_resetFlags)
    .class_function("setOneClickMode", &Modes_setOneClickMode)
    .class_function("oneClickModeEnabled", &Modes_oneClickModeEnabled)
    .class_function("setLocked", &Modes_setLocked)
    .class_function("locked", &Modes_locked)
    .class_function("setAdvancedMenus", &Modes_setAdvancedMenus)
    .class_function("advancedMenusEnabled", &Modes_advancedMenusEnabled)
    .class_function("setKeychainMode", &Modes_setKeychainMode)
    .class_function("keychainModeEnabled", &Modes_keychainModeEnabled);

  class_<Vortex>("Vortex")
    .class_function("init", select_overload<void()>(&Vortex::init))
    .class_function("setInstantTimestep", &Vortex::setInstantTimestep)
    .class_function("shortClick", &Vortex::shortClick)
    .class_function("longClick", &Vortex::longClick)
    .class_function("menuEnterClick", &Vortex::menuEnterClick)
    .class_function("advMenuEnterClick", &Vortex::advMenuEnterClick)
    .class_function("deleteColClick", &Vortex::deleteColClick)
    .class_function("sleepClick", &Vortex::sleepClick)
    .class_function("forceSleepClick", &Vortex::forceSleepClick)
    .class_function("pressButton", &Vortex::pressButton)
    .class_function("releaseButton", &Vortex::releaseButton)
    .class_function("isButtonPressed", &Vortex::isButtonPressed)
    .class_function("sendWait", &Vortex::sendWait)
    .class_function("rapidClick", &Vortex::rapidClick)
    .class_function("getMenuDemoMode", &Vortex::getMenuDemoMode, allow_raw_pointers())
    .class_function("setMenuDemoMode", &Vortex::setMenuDemoMode, allow_raw_pointers())
    .class_function("quitClick", &Vortex::quitClick)
    .class_function("IRDeliver", &Vortex::IRDeliver)
    .class_function("VLDeliver", &Vortex::VLDeliver)
    //.function("getStorageStats", &Vortex::getStorageStats)
    .class_function("loadStorage", &Vortex::loadStorage)
    .class_function("openRandomizer", &Vortex::openRandomizer)
    .class_function("openColorSelect", &Vortex::openColorSelect)
    .class_function("openPatternSelect", &Vortex::openPatternSelect)
    .class_function("openGlobalBrightness", &Vortex::openGlobalBrightness)
    .class_function("openFactoryReset", &Vortex::openFactoryReset)
    .class_function("openModeSharing", &Vortex::openModeSharing)
    .class_function("openEditorConnection", &Vortex::openEditorConnection)
    //.class_function("clearMenuTargetLeds", &Vortex::clearMenuTargetLeds)
    //.class_function("setMenuTargetLeds", &Vortex::setMenuTargetLeds)
    //.class_function("addMenuTargetLeds", &Vortex::addMenuTargetLeds)
    .class_function("getModes", &Vortex::getModes)
    .class_function("setModes", &Vortex::setModes)
    .class_function("getCurMode", &Vortex::getCurMode)
    .class_function("matchLedCount", &Vortex::matchLedCount)
    .class_function("checkLedCount", &Vortex::checkLedCount)
    .class_function("setLedCount", &Vortex::setLedCount)
    .class_function("curModeIndex", &Vortex::curModeIndex)
    .class_function("numModes", &Vortex::numModes)
    .class_function("numLedsInMode", &Vortex::numLedsInMode)
    .class_function("addNewMode", select_overload<bool(bool)>(&Vortex::addNewMode))
    .class_function("addNewMode", select_overload<bool(ByteStream &, bool)>(&Vortex::addNewMode))
    .class_function("setCurMode", &Vortex::setCurMode)
    .class_function("nextMode", &Vortex::nextMode)
    .class_function("delCurMode", &Vortex::delCurMode)
    .class_function("shiftCurMode", &Vortex::shiftCurMode)
    //.function("setPattern", &Vortex::setPattern)
    .class_function("getPatternID", &Vortex::getPatternID)
    .class_function("getPatternName", &Vortex::getPatternName)
    .class_function("getModeName", &Vortex::getModeName)
    //.function("setPatternAt", &Vortex::setPatternAt)
    .class_function("getColorset", &Vortex::getColorset)
    .class_function("setColorset", &Vortex::setColorset)
    .class_function("getPatternArgs", &Vortex::getPatternArgs)
    .class_function("setPatternArgs", &Vortex::setPatternArgs)
    .class_function("isCurModeMulti", &Vortex::isCurModeMulti)
    .class_function("patternToString", &Vortex::patternToString)
    .class_function("ledToString", &Vortex::ledToString)
    .class_function("numCustomParams", &Vortex::numCustomParams)
    .class_function("getCustomParams", &Vortex::getCustomParams)
    .class_function("setUndoBufferLimit", &Vortex::setUndoBufferLimit)
    .class_function("addUndoBuffer", &Vortex::addUndoBuffer)
    .class_function("undo", &Vortex::undo)
    .class_function("redo", &Vortex::redo)
    .class_function("setTickrate", &Vortex::setTickrate)
    .class_function("getTickrate", &Vortex::getTickrate)
    .class_function("enableUndo", &Vortex::enableUndo)
    //.function("vcallbacks", &Vortex::vcallbacks)
    .class_function("doCommand", &Vortex::doCommand)
    .class_function("setSleepEnabled", &Vortex::setSleepEnabled)
    .class_function("sleepEnabled", &Vortex::sleepEnabled)
    .class_function("enterSleep", &Vortex::enterSleep)
    .class_function("isSleeping", &Vortex::isSleeping)
    .class_function("enableCommandLog", &Vortex::enableCommandLog)
    .class_function("getCommandLog", &Vortex::getCommandLog)
    .class_function("clearCommandLog", &Vortex::clearCommandLog)
    .class_function("enableLockstep", &Vortex::enableLockstep)
    .class_function("isLockstep", &Vortex::isLockstep)
    .class_function("enableStorage", &Vortex::enableStorage)
    .class_function("storageEnabled", &Vortex::storageEnabled)
    .class_function("setStorageFilename", &Vortex::setStorageFilename)
    .class_function("getStorageFilename", &Vortex::getStorageFilename)
    .class_function("printModeJson", &Vortex::printModeJson)
    .class_function("parseModeJson", &Vortex::parseModeJson)
    .class_function("printPatternJson", &Vortex::printPatternJson)
    .class_function("parsePatternJson", &Vortex::parsePatternJson)
    .class_function("printJson", &Vortex::printJson)
    .class_function("parseJson", &Vortex::parseJson)
    .class_function("setLockEnabled", &Vortex::setLockEnabled)
    .class_function("lockEnabled", &Vortex::lockEnabled);

  function("getDataArray", &getDataArray);
  function("getRawDataArray", &getRawDataArray);
  function("createByteStreamFromData", &createByteStreamFromData);

}
#endif

using namespace std;

// I wish there was a way to do this automatically but it would be
// quite messy and idk if it's worth it
static const char *patternNames[PATTERN_COUNT] = {
  "strobe", "hyperstrobe", "picostrobe", "strobie", "dops", "ultradops", "strobegap",
  "hypergap", "picogap", "strobiegap", "dopsgap", "ultragap", "blinkie",
  "ghostcrush", "doubledops", "chopper", "dashgap", "dashdops", "dash-crush",
  "ultradash", "gapcycle", "dashcycle", "tracer", "ribbon", "miniribbon",
  "blend", "blendstrobe", "blendstrobegap", "complementary_blend",
  "complementary_blendstrobe", "complementary_blendstrobegap", "solid",
  "hueshift", "theater_chase", "chaser", "zigzag", "zipfade", "drip",
  "dripmorph", "crossdops", "doublestrobe", "meteor", "sparkletrace",
  "vortexwipe", "warp", "warpworm", "snowball", "lighthouse", "pulsish",
  "fill", "bounce", "splitstrobie", "backstrobe", "vortex",
};

// static vortex data
char Vortex::m_lastCommand = 0;
deque<ByteStream> Vortex::m_undoBuffer;
uint32_t Vortex::m_undoLimit = 0;
uint32_t Vortex::m_undoIndex = 0;
bool Vortex::m_undoEnabled = true;
VortexCallbacks *Vortex::m_storedCallbacks;
FILE *Vortex::m_consoleHandle = nullptr;
#if LOG_TO_FILE == 1
FILE *Vortex::m_logHandle = nullptr;
#endif
deque<Vortex::VortexButtonEvent> Vortex::m_buttonEventQueue;
bool Vortex::m_initialized = false;
uint32_t Vortex::m_buttonsPressed = 0;
uint8_t Vortex::m_selectedButton = 0;
string Vortex::m_commandLog;
bool Vortex::m_commandLogEnabled = false;
bool Vortex::m_lockstepEnabled = false;
bool Vortex::m_storageEnabled = false;
bool Vortex::m_sleepEnabled = true;
bool Vortex::m_lockEnabled = true;
Random Vortex::m_randCtx;

#ifdef _WIN32
BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
  // Perform actions based on the reason for calling.
  switch (fdwReason) {
  case DLL_PROCESS_ATTACH:
    break;
  case DLL_THREAD_ATTACH:
    break;
  case DLL_THREAD_DETACH:
    break;
  case DLL_PROCESS_DETACH:
    break;
  }
  return TRUE;  // Successful DLL_PROCESS_ATTACH.
}
#endif

// called when engine reads digital pins, use this to feed button presses to the engine
long VortexCallbacks::checkPinHook(uint32_t pin)
{
  // leaving the pin checking to user-overrides rather than
  // checking the default pins here
  return 1; // HIGH
}

Vortex::Vortex()
{
  // default callbacks pointer that can be replaced with a derivative
  // of the VortexCallbacks class
  m_storedCallbacks = new VortexCallbacks;
  if (!m_storedCallbacks) {
    // error! out of memory
  }
#ifdef _WIN32
  Random_seed(&m_randCtx, (uint32_t)(time(NULL) ^ GetTickCount64()));
#else
  struct timespec ts;
  unsigned long theTick = 0U;
  clock_gettime(CLOCK_MONOTONIC, &ts);
  theTick = ts.tv_nsec / 1000000;
  theTick += ts.tv_sec * 1000;
  Random_seed(&m_randCtx, time(NULL) ^ theTick);
#endif
}

Vortex::~Vortex()
{
  cleanup();
}

bool Vortex::init(VortexCallbacks *callbacks)
{
  if (m_storedCallbacks) {
    delete m_storedCallbacks;
  }
  // store the callbacks object
  m_storedCallbacks = callbacks;

#if LOG_TO_CONSOLE == 1
  if (!m_consoleHandle) {
#ifdef _WIN32
    AllocConsole();
    freopen_s(&m_consoleHandle, "CONOUT$", "w", stdout);
#else
    m_consoleHandle = stdout;
#endif
  }
#endif
#if LOG_TO_FILE == 1
  if (!m_logHandle) {
    time_t t = time(nullptr);
    tm tm;
    localtime_s(&tm, &t);
    ostringstream oss;
    oss << put_time(&tm, "%d-%m-%Y-%H-%M-%S");
    oss << "." << GetCurrentProcessId();
    string filename = VORTEX_LOG_NAME "." + oss.str() + ".txt";
    int err = fopen_s(&m_logHandle, filename.c_str(), "w");
    if (err != 0 || !m_logHandle) {
      MessageBox(NULL, "Failed to open logfile", to_string(err).c_str(), 0);
      return false;
    }
  }
#endif

  {
    // only the engine itself counts towards the device memory
    HEAP_TRACK_SCOPE(true);
    // init the engine
    VortexEngine_init();
    // load the modes
    // TODO: don't load modes here? separate api?
    Modes_load();
  }
  // clear the modes
  //Modes_clearModes();
  // save and set undo buffer
  doSave();

  m_initialized = true;

  return true;
}

void Vortex::cleanup()
{
  {
    HEAP_TRACK_SCOPE(true);
    VortexEngine_cleanup();
  }
  if (m_storedCallbacks) {
    delete m_storedCallbacks;
    m_storedCallbacks = nullptr;
  }
  m_initialized = false;
}

void Vortex::handleNumber(char c)
{
  if (!isdigit(c) || !m_lastCommand) {
    return;
  }
  int number = c - '0';
  char newc = 0;
  // read the digits into the repeatAmount
  while (1) {
    newc = getchar();
    if (!isdigit(newc)) {
      // stop once we reach a non digit
      break;
    }
    // accumulate the digits into the repeat amount
    number = (number * 10) + (newc - '0');
  }
  // shove the last non-digit back into the stream
  ungetc(newc, stdin);
  DEBUG_LOGF("Repeating last command (%c) x%u times", m_lastCommand, number);
  m_commandLog += to_string(number);
  // check to see if we are repeating a 'rapid click' which is a special case
  // because the rapid click command itself is composed or 'r' and a repeat count
  // to designate how many rapid clicks to deliver
  switch (m_lastCommand) {
  case 'r':
    // perform a rapid click number times
    rapidClick(number);
    break;
  case 'b':
    // select button index to target for click events
    selectButton(number);
    break;
  case 'v':
    // another device sends this default mode over VL
    m_buttonEventQueue.push_back(VortexButtonEvent(number, EVENT_VL_TRANSMIT));
    break;
  default: // any other command
    if (number > 0) {
      // offset repeat amount by exactly 1 because it's already done
      number--;
    }
    // repeat the last command that many times
    while (number > 0) {
      doCommand(m_lastCommand);
      number--;
    }
    break;
  }
}

// injects a command into the engine, the engine will parse one command
// per tick so multiple commands will be queued up
void Vortex::doCommand(char c)
{
  if (!isprint(c)) {
    return;
  }

  m_commandLog += c;

  switch (c) {
  case 'c':
    if (m_lastCommand != c) {
      DEBUG_LOG("Injecting short click");
    }
    shortClick();
    break;
  case 'l':
    if (m_lastCommand != c) {
      DEBUG_LOG("Injecting long click");
    }
    longClick();
    break;
  case 'm':
    if (m_lastCommand != c) {
      DEBUG_LOG("Injecting menu enter click");
    }
    menuEnterClick();
    break;
  case 'a':
    if (m_lastCommand != c) {
      DEBUG_LOG("Injecting adv menu enter click");
    }
    advMenuEnterClick();
    break;
  case 'd':
    if (m_lastCommand != c) {
      DEBUG_LOG("Injecting adv menu enter click");
    }
    deleteColClick();
    break;
  case 's':
    if (m_lastCommand != c) {
      DEBUG_LOG("Injecting sleep click");
    }
    sleepClick();
    break;
  case 'f':
    if (m_lastCommand != c) {
      DEBUG_LOG("Injecting force sleep click");
    }
    forceSleepClick();
    break;
  case 'q':
    //case '\n':
    if (m_lastCommand != c) {
      DEBUG_LOG("Injecting quit click\n");
    }
    quitClick();
    break;
  case 'v':
    // do nothing for the initial 'v', handleNumber() will handle the default
    // mode index that follows and queue the VL transmit of that mode
    break;
#if HEAP_TRACKING == 1
  case 'H':
    m_buttonEventQueue.push_back(VortexButtonEvent(0, EVENT_HEAP_STATS));
    break;
#endif
  case 't':
    if (isButtonPressed()) {
      if (m_lastCommand != c) {
        DEBUG_LOG("Injecting release");
      }
      releaseButton();
    } else {
      if (m_lastCommand != c) {
        DEBUG_LOG("Injecting press");
      }
      pressButton();
    }
    break;
  case 'r':
    // do nothing for the initial 'r', handleNumber() will handle the numeric
    // repeat count that follows this letter and issue the rapidClick(amt) call
    break;
  case 'b':
    // button selector, same as rapid click, it's a little weird because the number
    // it not a repeat amount but actually the button index to select -- but none
    // the less it works the same as rapid click so it will be handled inside handleNumber()
    break;
  case 'w':
    if (m_lastCommand != c) {
      DEBUG_LOG("Injecting wait");
    }
    sendWait();
    break;
  default:
    handleNumber(c);
    // return instead of break because this isn't a command
    return;
  }
  m_lastCommand = c;
}

// whether the engine has sleep enabled, if disabled it will always be awake
void Vortex::setSleepEnabled(bool enable)
{
  m_sleepEnabled = enable;
}

bool Vortex::sleepEnabled()
{
  return m_sleepEnabled;
}

// whether the engine is sleeping, and/or to enter sleep
void Vortex::enterSleep(bool save)
{
  VortexEngine_enterSleep(save);
}

bool Vortex::isSleeping()
{
  return VortexEngine_isSleeping();
}

bool Vortex::tick()
{
  if (!m_initialized) {
    // do not cleanup prematurely here
    return false;
  }
  // use ioctl to determine how many characters are on stdin so that
  // we don't call getchar() too many times and accidentally block
  uint32_t numInputs = getNumInputs();
  if (m_lockstepEnabled && !numInputs) {
    // don't tick till we have input
    return true;
  }
  // iterate the number of inputs on stdin and parse each letter
  // into a command for the engine, this will inject all of the commands
  // that are available into the engine but that doesn't necessarily
  // mean that the engine will do anything with them right away
  for (uint32_t i = 0; i < numInputs; ++i) {
    doCommand(getchar());
  }
  // tick the vortex engine forward
  HEAP_TRACK_SCOPE(true);
  VortexEngine_tick();
  return true;
}


void Vortex::installCallbacks(VortexCallbacks *callbacks)
{
  m_storedCallbacks = callbacks;
}

void Vortex::setInstantTimestep(bool timestep)
{
  Time_setInstantTimestep(timestep);
}

// select the button to send clicks to (0 = first button)
void Vortex::selectButton(uint8_t buttonIndex)
{
  if (buttonIndex >= Buttons_numButtons()) {
    return;
  }
  m_selectedButton = buttonIndex;
}

// send various clicks
void Vortex::shortClick(uint8_t buttonIndex)
{
  if (!buttonIndex) {
    buttonIndex = m_selectedButton;
  }
  m_buttonEventQueue.push_back(VortexButtonEvent(buttonIndex, EVENT_SHORT_CLICK));
}

void Vortex::longClick(uint8_t buttonIndex)
{
  if (!buttonIndex) {
    buttonIndex = m_selectedButton;
  }
  m_buttonEventQueue.push_back(VortexButtonEvent(buttonIndex, EVENT_LONG_CLICK));
}

void Vortex::menuEnterClick(uint8_t buttonIndex)
{
  if (!buttonIndex) {
    buttonIndex = m_selectedButton;
  }
  m_buttonEventQueue.push_back(VortexButtonEvent(buttonIndex, EVENT_MENU_ENTER_CLICK));
}

void Vortex::advMenuEnterClick(uint8_t buttonIndex)
{
  if (!buttonIndex) {
    buttonIndex = m_selectedButton;
  }
  m_buttonEventQueue.push_back(VortexButtonEvent(buttonIndex, EVENT_ADV_MENU_ENTER_CLICK));
}

void Vortex::deleteColClick(uint8_t buttonIndex)
{
  if (!buttonIndex) {
    buttonIndex = m_selectedButton;
  }
  m_buttonEventQueue.push_back(VortexButtonEvent(buttonIndex, EVENT_DELETE_COL));
}

void Vortex::sleepClick(uint8_t buttonIndex)
{
  if (!buttonIndex) {
    buttonIndex = m_selectedButton;
  }
  m_buttonEventQueue.push_back(VortexButtonEvent(buttonIndex, EVENT_SLEEP_CLICK));
}

void Vortex::forceSleepClick(uint8_t buttonIndex)
{
  if (!buttonIndex) {
    buttonIndex = m_selectedButton;
  }
  m_buttonEventQueue.push_back(VortexButtonEvent(buttonIndex, EVENT_FORCE_SLEEP_CLICK));
}

void Vortex::pressButton(uint8_t buttonIndex)
{
  if (!buttonIndex) {
    buttonIndex = m_selectedButton;
  }
  m_buttonsPressed |= (1 << buttonIndex);
}

void Vortex::releaseButton(uint8_t buttonIndex)
{
  if (!buttonIndex) {
    buttonIndex = m_selectedButton;
  }
  m_buttonsPressed &= ~(1 << buttonIndex);
}

bool Vortex::isButtonPressed(uint8_t buttonIndex)
{
  if (!buttonIndex) {
    buttonIndex = m_selectedButton;
  }
  return (m_buttonsPressed & (1 << buttonIndex)) != 0;
}

void Vortex::sendWait(uint32_t amount)
{
  // reusing the button index as the wait amount
  m_buttonEventQueue.push_back(VortexButtonEvent(amount, EVENT_WAIT));
}

void Vortex::rapidClick(uint32_t amount)
{
  // reusing the button index as the consecutive press amount
  m_buttonEventQueue.push_back(VortexButtonEvent(amount, EVENT_RAPID_CLICK));
}

Mode *Vortex::getMenuDemoMode()
{
  Menu *pMenu = Menus_curMenu();
  if (!pMenu) {
    return nullptr;
  }
  return &pMenu->m_previewMode;
}

bool Vortex::setMenuDemoMode(const Mode *mode)
{
  if (!mode) {
     return false;
  }
  Menu *pMenu = Menus_curMenu();
  if (!pMenu) {
    return false;
  }
  if (!Mode_getLedCount(mode) || Mode_equals(&pMenu->m_previewMode, mode)) {
    return false;
  }
  Mode_assign(&pMenu->m_previewMode, mode);
  Mode_initPatterns(&pMenu->m_previewMode);
  return true;
}

void Vortex::quitClick()
{
  m_buttonEventQueue.push_back(VortexButtonEvent(0, EVENT_QUIT_CLICK));
}

void Vortex::IRDeliver(uint32_t timing)
{
#if IR_ENABLE_RECEIVER == 1
  IRReceiver_handleIRTiming(timing);
#endif
}

void Vortex::VLDeliver(uint32_t timing)
{
#if VL_ENABLE_RECEIVER == 1
  VLReceiver_handleVLTiming(timing);
#endif
}

void Vortex::VLTransmitDefaultMode(uint8_t index)
{
#if VL_ENABLE_SENDER == 1 && VL_ENABLE_RECEIVER == 1
  if (index >= MAX_MODES) {
    return;
  }
  // the mode is built and sent by an imaginary second device so none of
  // this counts towards the device memory of this device
  HEAP_TRACK_SCOPE(false);
  Mode mode;
  Mode_initDefault(&mode, &defaultModes[index]);
  // capture the timings the sender would blink
  class VLCapture : public VortexCallbacks
  {
  public:
    void infraredWrite(bool mark, uint32_t amount) override { timings.push_back(amount); }
    vector<uint32_t> timings;
  };
  VLCapture capture;
  VortexCallbacks *callbacks = m_storedCallbacks;
  m_storedCallbacks = &capture;
  VLSender_send(&mode);
  m_storedCallbacks = callbacks;
  Mode_destroy(&mode);
  // and deliver them to the receiver of this device
  for (uint32_t timing : capture.timings) {
    VLDeliver(timing);
  }
#endif
}

void Vortex::getStorageStats(uint32_t *outTotal, uint32_t *outUsed)
{
  if (outTotal) {
    *outTotal = VortexEngine_totalStorageSpace();
  }
  if (outUsed) {
    *outUsed = VortexEngine_savefileSize();
  }
}

void Vortex::loadStorage()
{
  Modes_loadStorage();
}

void Vortex::openRandomizer(bool advanced)
{
  Menus_openMenu(MENU_RANDOMIZER, advanced);
}

void Vortex::openColorSelect(bool advanced)
{
  Menus_openMenu(MENU_COLOR_SELECT, advanced);
}

void Vortex::openPatternSelect(bool advanced)
{
  Menus_openMenu(MENU_PATTERN_SELECT, advanced);
}

void Vortex::openGlobalBrightness(bool advanced)
{
  Menus_openMenu(MENU_GLOBAL_BRIGHTNESS, advanced);
}

void Vortex::openFactoryReset(bool advanced)
{
  Menus_openMenu(MENU_FACTORY_RESET, advanced);
}

void Vortex::openModeSharing(bool advanced)
{
  Menus_openMenu(MENU_MODE_SHARING, advanced);
}

void Vortex::openEditorConnection(bool advanced)
{
#if ENABLE_EDITOR_CONNECTION == 1
  Menus_openMenu(MENU_EDITOR_CONNECTION, advanced);
#endif
}

//void Vortex::clearMenuTargetLeds()
//{
//  Menu *cur = Menus_curMenu();
//  if (!cur) {
//    return;
//  }
//  cur->setTargetLeds(MAP_LED_NONE);
//}
//
//void Vortex::setMenuTargetLeds(LedMap targetLeds)
//{
//  Menu *cur = Menus_curMenu();
//  if (!cur) {
//    return;
//  }
//  cur->setTargetLeds(targetLeds);
//}
//
//void Vortex::addMenuTargetLeds(LedPos pos)
//{
//  Menu *cur = Menus_curMenu();
//  if (!cur) {
//    return;
//  }
//  LedMap curMap = cur->getTargetLeds();
//  cur->setTargetLeds((LedMap)(curMap | MAP_LED(pos)));
//}

bool Vortex::getModes(ByteStream &outStream)
{
  // now serialize all the modes
  Modes_saveToBuffer(&outStream);
  return true;
}

bool Vortex::setModes(ByteStream &stream, bool save)
{
  Modes_clearModes();
  // now unserialize the stream of data that was read
  if (!Modes_loadFromBuffer(&stream)) {
    return false;
  }
  return !save || doSave();
}

bool Vortex::matchLedCount(ByteStream &stream, bool vtxMode)
{
  if (!ByteStream_decompress(&stream)) {
    return false;
  }
  // reset the unserializer index before unserializing anything
  ByteStream_resetUnserializer(&stream);
  uint8_t major = 0;
  uint8_t minor = 0;
  // unserialize the vortex version
  ByteStream_unserialize8(&stream, &major);
  ByteStream_unserialize8(&stream, &minor);
  // unserialize the global brightness
  if (!vtxMode) {
    uint8_t flags;
    ByteStream_unserialize8(&stream, &flags);
    uint8_t brightness = 0;
    ByteStream_unserialize8(&stream, &brightness);
    uint8_t numModes = 0;
    ByteStream_unserialize8(&stream, &numModes);
  }
  uint8_t ledCount = 0;
  ByteStream_unserialize8(&stream, &ledCount);
  // put the unserializer back where it was for the next thing
  ByteStream_resetUnserializer(&stream);
  return setLedCount(ledCount);
}

bool Vortex::checkLedCount()
{
  Mode *mode = Modes_curMode();
  if (!mode) {
    return false;
  }
  uint8_t numLeds = Mode_getLedCount(mode);
  if (numLeds != LED_COUNT) {
    //Leds_setLedCount(numLeds);
    return false;
  }
  return true;
}

uint8_t Vortex::setLedCount(uint8_t count)
{
  return true;
}

uint8_t Vortex::getLedCount()
{
  return LED_COUNT;
}

bool Vortex::getCurMode(ByteStream &outStream)
{
  Mode *pMode = Modes_curMode();
  if (!pMode) {
    return false;
  }
  // save to ensure we get the correct mode, not using doSave() because it causes
  // an undo buffer entry to be added
  if (!Modes_saveStorage()) {
    return false;
  }
  return Mode_saveToBuffer(Modes_curMode(), &outStream, LED_COUNT);
}

uint32_t Vortex::curModeIndex()
{
  return Modes_curModeIndex();
}

uint32_t Vortex::numModes()
{
  return Modes_numModes();
}

uint32_t Vortex::numLedsInMode()
{
  Mode *pMode = Modes_curMode();
  if (!pMode) {
    return false;
  }
  return Mode_getLedCount(pMode);
}

bool Vortex::addMode(const Mode *mode, bool save)
{
  if (!Modes_addMode(mode)) {
    return false;
  }
  return !save || doSave();
}

bool Vortex::addNewMode(bool save)
{
  Colorset set;
  Colorset_make(&set);
  Colorset_randomize(&set, &m_randCtx, 0);
  // create a random pattern ID from all patterns
  PatternID randomPattern;
  do {
    // continuously re-randomize the pattern so we don't get solids
    randomPattern = (PatternID)Random_next16(&m_randCtx, PATTERN_FIRST, PATTERN_SINGLE_LAST);
  } while (randomPattern == PATTERN_SOLID);
  if (!Modes_addMode(randomPattern, nullptr, &set)) {
    return false;
  }
  return !save || doSave();
}

bool Vortex::addNewMode(ByteStream &stream, bool save)
{
  if (!Modes_addModeFromBuffer(&stream)) {
    return false;
  }
  return !save || doSave();
}

bool Vortex::setCurMode(uint32_t index, bool save)
{
  if (index >= Modes_numModes()) {
    return true;
  }
  if (!Modes_setCurMode(index)) {
    return false;
  }
  return !save || doSave();
}

bool Vortex::nextMode(bool save)
{
  if (!Modes_numModes()) {
    return true;
  }
  Modes_nextMode();
  return !save || doSave();
}

bool Vortex::delCurMode(bool save)
{
  if (!Modes_numModes()) {
    return true;
  }
  Modes_deleteCurMode();
  return !save || doSave();
}

bool Vortex::shiftCurMode(int8_t offset, bool save)
{
  if (!Modes_numModes()) {
    return true;
  }
  if (offset == 0) {
    return true;
  }
  Modes_shiftCurMode(offset);
  return !save || doSave();
}

bool Vortex::setPattern(PatternID id, const PatternArgs *args, const Colorset *set, bool save)
{
  Mode *pMode = Modes_curMode();
  if (!pMode) {
    return false;
  }
  if (!Mode_setPattern(pMode, id, LED_ANY, args, set)) {
    return false;
  }
  Mode_initPatterns(pMode);
  return !save || doSave();
}

PatternID Vortex::getPatternID(LedPos pos)
{
  Mode *pMode = Modes_curMode();
  if (!pMode) {
    return PATTERN_NONE;
  }
  return Mode_getPatternID(pMode, pos);
}

string Vortex::getPatternName(LedPos pos)
{
  return patternToString(getPatternID(pos));
}

string Vortex::getModeName()
{
  Mode *pMode = Modes_curMode();
  if (!pMode) {
    return patternToString(PATTERN_NONE);
  }
  if (Mode_isMultiLed(pMode)) {
    return patternToString(Mode_getPatternID(pMode, LED_MULTI));
  }
  // can't use isSampleSingleLed because that will compare the entire
  // pattern for differences in any single led pattern, we only care
  // about the pattern id being different
  bool all_same_id = true;
  PatternID first = Mode_getPatternID(pMode, LED_FIRST);
  for (uint32_t i = LED_FIRST + 1; i < numLedsInMode(); ++i) {
    // if any don't match 0 then no good
    if (Mode_getPatternID(pMode, (LedPos)i) != first) {
      all_same_id = false;
      break;
    }
  }
  // if they're all the same we can return just the first led pattern name
  if (all_same_id) {
    return patternToString(getPatternID(LED_FIRST));
  }
  // mixed single led pattern with different pattern names
  return "custom";
}

bool Vortex::setPatternAt(LedPos pos, PatternID id,
  const PatternArgs *args, const Colorset *set, bool save)
{
  Mode *pMode = Modes_curMode();
  if (!pMode) {
    return false;
  }
  if (!Mode_setPattern(pMode, id, pos, args, set)) {
    return false;
  }
  Mode_initPatterns(pMode);
  return !save || doSave();
}

bool Vortex::getColorset(LedPos pos, Colorset &set)
{
  Mode *pMode = Modes_curMode();
  if (!pMode) {
    return false;
  }
  set = Mode_getColorset(pMode, pos);
  return true;
}

bool Vortex::setColorset(LedPos pos, const Colorset &set, bool save)
{
  Mode *pMode = Modes_curMode();
  if (!pMode) {
    return false;
  }
  if (!Mode_setColorset(pMode, &set, pos)) {
    return false;
  }
  Mode_initPatterns(pMode);
  return !save || doSave();
}

bool Vortex::getPatternArgs(LedPos pos, PatternArgs &args)
{
  Mode *pMode = Modes_curMode();
  if (!pMode) {
    return false;
  }
  Pattern *pat = Mode_getPatternMutable(pMode, pos);
  if (!pat) {
    return false;
  }
  Pattern_getArgs(pat, &args);
  return true;
}

bool Vortex::setPatternArgs(LedPos pos, PatternArgs &args, bool save)
{
  Mode *pMode = Modes_curMode();
  if (!pMode) {
    return false;
  }
  Pattern *pat = nullptr;
  // Equivalent to cases LED_ANY, LED_ALL and LED_MULTI
  if (pos == LED_ANY || pos == LED_ALL || pos == LED_MULTI) {
#if VORTEX_SLIM == 0
    pat = Mode_getPatternMutable(pMode, LED_MULTI);
    if (pat) {
      Pattern_setArgs(pat, &args);
    }
    if (pos == LED_MULTI) {
      // don't fallthrough if actually multi, it's possible
      // we got here by falling through from LED_ALL
      Mode_initPatterns(pMode);
      return !save || doSave();
    }
    // fall through if LED_ALL and change the single leds
    else if (pos == LED_ANY || pos == LED_ALL) {
      for (LedPos pos = LED_FIRST; pos < LED_COUNT; ++pos) {
        pat = Mode_getPatternMutable(pMode, pos);
        if (pat) {
          Pattern_setArgs(pat, &args);
        }
      }
      Mode_initPatterns(pMode);
      // actually break here
      return !save || doSave();
    }
#endif
  }

  // equivalent to case LED_ALL_SINGLE
  else if (pos == LED_ALL_SINGLE) {
    for (LedPos pos = LED_FIRST; pos < LED_COUNT; ++pos) {
      pat = Mode_getPatternMutable(pMode, pos);
      if (pat) {
        Pattern_setArgs(pat, &args);
      }
    }
    Mode_initPatterns(pMode);
    // actually break here
    return !save || doSave();
  }

  // equivalent to default case (covers any other pos)
  else {
    if (pos >= LED_COUNT) {
      return false;
    }
    pat = Mode_getPatternMutable(pMode, pos);
    if (!pat) {
      return false;
    }
    Pattern_setArgs(pat, &args);
    Mode_initPatterns(pMode);
    return !save || doSave();
  }

  return false;
}

bool Vortex::isCurModeMulti()
{
  Mode *pMode = Modes_curMode();
  if (!pMode) {
    return false;
  }
  return Mode_isMultiLed(pMode);
}

string Vortex::patternToString(PatternID id)
{
  // this shouldn't happen but just in case somebody messes with stuff
  if (sizeof(patternNames) / sizeof(patternNames[0]) != PATTERN_COUNT) {
    // if you see this it means the list of strings above is not equal to
    // the number of patterns in the enum, so you need to update the list
    // above to match the enum.
    return "fix patternToString()";
  }
  if (id <= PATTERN_NONE || id >= PATTERN_COUNT) {
    return "none";
  }
  return patternNames[id];
}

PatternID Vortex::stringToPattern(const std::string &pattern)
{
  static map<string, PatternID> cachedNames;
  if (cachedNames.empty()) {
    for (PatternID i = PATTERN_FIRST; i < PATTERN_COUNT; ++i) {
      cachedNames[patternNames[i]] = (PatternID)i;
    }
  }
  // lowercase the name and look it up
  string lowerPat = pattern;
  transform(lowerPat.begin(), lowerPat.end(), lowerPat.begin(), [](unsigned char c) { return tolower(c); });
  if (cachedNames.find(pattern) == cachedNames.end()) {
    return PATTERN_NONE;
  }
  return cachedNames[pattern];
}

// this shouldn't change much so this is fine
string Vortex::ledToString(LedPos pos)
{
  return "led " + to_string(pos);
}

// the number of custom parameters for any given pattern id
uint32_t Vortex::numCustomParams(PatternID id)
{
  return PatternBuilder_numDefaultArgs(id);
}

vector<string> Vortex::getCustomParams(PatternID id)
{
  Pattern *pat = PatternBuilder_make(id, nullptr);
  vector<string> params;
  if (!pat) {
    return params;
  }
  for (uint32_t i = 0; i < Pattern_getNumArgs(pat); ++i) {
    params.push_back(Pattern_getArgName(pat, i));
  }
  Pattern_destroy(pat);
  PatternBuilder_freePattern(pat);
  return params;
}

void Vortex::setUndoBufferLimit(uint32_t limit)
{
  m_undoLimit = limit;
}

bool Vortex::addUndoBuffer()
{
  if (!m_undoEnabled) {
    return true;
  }
  ByteStream modes;
  ByteStream_make(&modes);
  getModes(modes);
  if (!ByteStream_size(&modes)) {
    ByteStream_destroy(&modes);
    return false;
  }
  // only save an undo if the buffer is different
  if (m_undoBuffer.size() > 0 && ByteStream_CRC(&modes) == ByteStream_CRC(&m_undoBuffer.back())) {
    //printf("Not saving duplicate undo buffer\n");
    ByteStream_destroy(&modes);
    return false;
  }
  // must rewind to that step before pushing next step
  while (m_undoIndex > 0) {
    ByteStream_destroy(&m_undoBuffer.back());
    m_undoBuffer.pop_back();
    m_undoIndex--;
  }
  // the arena tracks each buffer's owner so move the data into
  // the deque element rather than copying the struct
  m_undoBuffer.emplace_back();
  ByteStream_make(&m_undoBuffer.back());
  ByteStream_move(&modes, &m_undoBuffer.back());
  //printf("Pushing undo buffer (pos: %u)\n", m_undoIndex);
  // make sure list doesn't grow too big
  if (m_undoLimit && m_undoBuffer.size() > m_undoLimit) {
    //printf("Popping front of undo buffer\n");
    ByteStream_destroy(&m_undoBuffer.front());
    m_undoBuffer.pop_front();
  }
#if 0
  printf("Buffer:\n");
  for (uint32_t i = 0; i < m_undoBuffer.size(); ++i) {
    printf("\t%u: %x", i, ByteStream_CRC(&m_undoBuffer[i]));
    if ((m_undoBuffer.size() - 1) - m_undoIndex == i) {
      printf(" <--\n");
    } else {
      printf("\n");
    }
  }
#endif
  return true;
}

bool Vortex::applyUndo()
{
  if (!m_undoBuffer.size()) {
    return false;
  }
  uint32_t highestIndex = (uint32_t)m_undoBuffer.size() - 1;
  if (m_undoIndex > highestIndex) {
    m_undoIndex = highestIndex;
  }
  //printf("Undo position: %u / %u\n", m_undoIndex, highestIndex);
  //printf("Buffer:\n");
#if 0
  for (uint32_t i = 0; i < m_undoBuffer.size(); ++i) {
    printf("\t%u: %x", i, ByteStream_CRC(&m_undoBuffer[i]));
    if ((m_undoBuffer.size() - 1) - m_undoIndex == i) {
      printf(" <--\n");
    } else {
      printf("\n");
    }
  }
#endif
  // index from the back instead of the front
  uint32_t backIndex = highestIndex - m_undoIndex;
  ByteStream_resetUnserializer(&m_undoBuffer[backIndex]);
  Vortex::setModes(m_undoBuffer[backIndex], false);
  return true;
}

bool Vortex::undo()
{
  if (!m_undoBuffer.size()) {
    return false;
  }
  uint32_t highestIndex = (uint32_t)m_undoBuffer.size() - 1;
  // cannot undo further into history
  if (m_undoIndex > highestIndex) {
    m_undoIndex = highestIndex;
  } else {
    m_undoIndex++;
  }
  return applyUndo();
}

bool Vortex::redo()
{
  if (!m_undoBuffer.size()) {
    return false;
  }
  // cannot undo further into history
  if (m_undoIndex > 0) {
    m_undoIndex--;
  }
  return applyUndo();
}

void Vortex::setTickrate(uint32_t tickrate)
{
  Time_setTickrate(tickrate);
}

uint32_t Vortex::getTickrate()
{
  return Time_getTickrate();
}

bool Vortex::doSave()
{
  return Modes_saveStorage() && addUndoBuffer();
}

void Vortex::handleInputQueue(Button *buttons, uint32_t numButtons)
{
  // this runs inside the engine tick but the input queue isn't part of the
  // engine so it shouldn't count towards the device memory
  HEAP_TRACK_SCOPE(false);
  // if there's nothing in the queue just return
  if (!m_buttonEventQueue.size()) {
    return;
  }
  // pop the event from the front of the queue
  VortexButtonEvent buttonEvent = m_buttonEventQueue.front();
  m_buttonEventQueue.pop_front();
  // the target button for this event (no target if event is just 'wait')
  Button *pButton = buttons;
  if (buttonEvent.type != EVENT_WAIT && buttonEvent.type != EVENT_RAPID_CLICK &&
      buttonEvent.type != EVENT_VL_TRANSMIT && buttonEvent.type != EVENT_HEAP_STATS) {
    // make sure the button that is targeted is actually a valid index
    if (buttonEvent.target >= numButtons) {
      return;
    }
    // assigned the button based on the array index target
    pButton = buttons + buttonEvent.target;
  }
  // switch on the type of event and then run the operation
  switch (buttonEvent.type) {
  case EVENT_NONE:
  default:
    break;
  case EVENT_SHORT_CLICK:
    pButton->m_newRelease = true;
    pButton->m_shortClick = true;
    pButton->m_pressTime = Time_getCurtime();
    pButton->m_holdDuration = SHORT_CLICK_THRESHOLD_TICKS - 1;
    DEBUG_LOG("Injecting short click");
    break;
  case EVENT_LONG_CLICK:
    pButton->m_newRelease = true;
    pButton->m_longClick = true;
    pButton->m_pressTime = Time_getCurtime();
    pButton->m_holdDuration = SHORT_CLICK_THRESHOLD_TICKS + 1;
    DEBUG_LOG("Injecting long click");
    break;
  case EVENT_MENU_ENTER_CLICK:
    // to do this we simply press the button and set the press time
    // to something more than the menu trigger threshold that will make
    // us immediately enter the menus. But we need to unset the pressed
    // button right after so we push a reset click event to reset the button
    pButton->m_pressTime = Time_getCurtime();
#if defined(SLEEP_ENTER_THRESHOLD_TICKS) && defined(SLEEP_WINDOW_THRESHOLD_TICKS)
    // microlight must hold longer (past sleep time)
    pButton->m_holdDuration = MENU_TRIGGER_THRESHOLD_TICKS + SLEEP_ENTER_THRESHOLD_TICKS + SLEEP_WINDOW_THRESHOLD_TICKS + 1;
#else
    pButton->m_holdDuration = MENU_TRIGGER_THRESHOLD_TICKS + 1;
#endif
    pButton->m_isPressed = true;
    m_buttonEventQueue.push_front(VortexButtonEvent(0, EVENT_RESET_CLICK));
    DEBUG_LOG("Injecting menu enter click");
    break;
  case EVENT_ADV_MENU_ENTER_CLICK:
    // to do this we simply press the button and set the press time
    // to something more than the menu trigger threshold that will make
    // us immediately enter the menus. But we need to unset the pressed
    // button right after so we push a reset click event to reset the button
    pButton->m_pressTime = Time_getCurtime();
    pButton->m_holdDuration = ADV_MENU_DURATION_TICKS + 1;
    pButton->m_longClick = true;
    pButton->m_newRelease = true;
    DEBUG_LOG("Injecting adv menu enter click");
    break;
  case EVENT_DELETE_COL:
    // to do this we simply press the button and set the press time
    // to something more than the menu trigger threshold that will make
    // us immediately enter the menus. But we need to unset the pressed
    // button right after so we push a reset click event to reset the button
    pButton->m_pressTime = Time_getCurtime();
    pButton->m_holdDuration = DELETE_THRESHOLD_TICKS + DELETE_CYCLE_TICKS + 1;
    pButton->m_longClick = true;
    pButton->m_newRelease = true;
    DEBUG_LOG("Injecting delete color click");
    break;
  case EVENT_SLEEP_CLICK:
    // to do this we simply press the button and set the press time
    // to something more than the menu trigger threshold that will make
    // us immediately enter the menus. But we need to unset the pressed
    // button right after so we push a reset click event to reset the button
#ifdef SLEEP_ENTER_THRESHOLD_TICKS
    pButton->m_pressTime = Time_getCurtime();
    pButton->m_holdDuration = SLEEP_ENTER_THRESHOLD_TICKS + 1;
    pButton->m_longClick = true;
    pButton->m_newRelease = true;
    DEBUG_LOG("Injecting sleep click");
#endif
    break;
  case EVENT_FORCE_SLEEP_CLICK:
    // to do this we simply press the button and set the press time
    // to something more than the menu trigger threshold that will make
    // us immediately enter the menus. But we need to unset the pressed
    // button right after so we push a reset click event to reset the button
#ifdef FORCE_SLEEP_THRESHOLD_TICKS
    pButton->m_pressTime = Time_getCurtime();
    pButton->m_holdDuration = FORCE_SLEEP_THRESHOLD_TICKS + 1;
    pButton->m_longClick = true;
    pButton->m_newRelease = true;
    DEBUG_LOG("Injecting force sleep click");
#endif
    break;
  case EVENT_WAIT:
    if (buttonEvent.target) {
      // backup the event queue and clear it
      deque<Vortex::VortexButtonEvent> backup;
      swap(backup, m_buttonEventQueue);
      // ticks the engine forward some number of ticks, the event queue is empty
      // so the engine won't process any input events while doing this
      {
        HEAP_TRACK_SCOPE(true);
        for (uint32_t i = 0; i < buttonEvent.target; ++i) {
          VortexEngine_tick();
        }
      }
      // then restore the event queue so that events are processed like normal
      swap(backup, m_buttonEventQueue);
    }
    break;
  case EVENT_TOGGLE_CLICK:
    if (Button_isPressed(pButton)) {
      // re-calc all this stuff because there's no api in Button class to do it
      // I don't want to add the api there because it's useless besides for this
      pButton->m_buttonState = false;
      pButton->m_isPressed = false;
      pButton->m_holdDuration = (uint32_t)(Time_getCurtime() - pButton->m_pressTime);
      pButton->m_releaseTime = Time_getCurtime();
      pButton->m_newRelease = true;
      pButton->m_shortClick = (pButton->m_holdDuration <= SHORT_CLICK_THRESHOLD_TICKS);
      pButton->m_longClick = !pButton->m_shortClick;
      DEBUG_LOG("Injecting release");
    } else {
      pButton->m_buttonState = true;
      pButton->m_isPressed = true;
      pButton->m_releaseDuration = (uint32_t)(Time_getCurtime() - pButton->m_releaseTime);
      pButton->m_pressTime = Time_getCurtime();
      pButton->m_newPress = true;
      DEBUG_LOG("Injecting press");
    }
    break;
  case EVENT_RAPID_CLICK:
    pButton->m_consecutivePresses = buttonEvent.target;
    pButton->m_newRelease = true;
    pButton->m_shortClick = true;
    pButton->m_pressTime = Time_getCurtime();
    pButton->m_holdDuration = 1;
    DEBUG_LOGF("Injecting %u x rapid click", buttonEvent.target);
    break;
  case EVENT_QUIT_CLICK:
    // just uninitialize so tick returns false
    m_initialized = false;
    DEBUG_LOG("Injecting quit");
    break;
  case EVENT_RESET_CLICK:
    pButton->m_isPressed = false;
    break;
  case EVENT_VL_TRANSMIT:
    VLTransmitDefaultMode(buttonEvent.target);
    break;
#if HEAP_TRACKING == 1
  case EVENT_HEAP_STATS:
    heap_printStats(m_commandLog.c_str());
    break;
#endif
  }
}

uint32_t Vortex::getNumInputs()
{
  uint32_t numInputs = 0;
  // On linux we need to poll stdin for input to handle commands
#if !defined(_WIN32) && !defined(WASM)
  ioctl(STDIN_FILENO, FIONREAD, &numInputs);
#elif defined(_WIN32)
  HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);

  if (GetFileType(hStdin) == FILE_TYPE_CHAR) {
    // Handle console input
    if (!GetNumberOfConsoleInputEvents(hStdin, (DWORD *)&numInputs)) {
      // Handle error here
    }
    INPUT_RECORD inputRecord;
    DWORD eventsRead;
    while (PeekConsoleInput(hStdin, &inputRecord, 1, &eventsRead) && eventsRead > 0) {
      // Discard non-character events
      if (inputRecord.EventType != KEY_EVENT || !inputRecord.Event.KeyEvent.bKeyDown) {
        numInputs--;
      }
      // Remove the event from the input buffer
      ReadConsoleInput(hStdin, &inputRecord, 1, &eventsRead);
    }
  } else {
    // Handle redirected input
    DWORD availableBytes;
    if (PeekNamedPipe(hStdin, NULL, 0, NULL, &availableBytes, NULL)) {
      numInputs = availableBytes;
    } else {
      // Handle error here
    }
  }
#endif
  return numInputs;
}

void Vortex::printlog(const char *file, const char *func, int line, const char *msg, va_list list)
{
  HEAP_TRACK_SCOPE(false);
  if (!Vortex::m_consoleHandle) {
    return;
  }
  string strMsg;
  if (file) {
    strMsg = file;
    if (strMsg.find_last_of('\\') != string::npos) {
      strMsg = strMsg.substr(strMsg.find_last_of('\\') + 1);
    }
    strMsg += ":";
    strMsg += to_string(line);
  }
  if (func) {
    strMsg += " ";
    strMsg += func;
    strMsg += "(): ";
  }
  strMsg += msg;
  strMsg += "\n";
  vfprintf(Vortex::m_consoleHandle, strMsg.c_str(), list);
#if LOG_TO_FILE == 1
  vfprintf(Vortex::m_logHandle, strMsg.c_str(), list);
#endif
}

void Vortex::setStorageFilename(const string &name)
{
  Storage_setStorageFilename(name);
}

string Vortex::getStorageFilename()
{
  return Storage_getStorageFilename();
}

json Vortex::modeToJson(const Mode *mode)
{
  if (!mode) {
    return nullptr;
  }

  json modeJson;
  modeJson["num_leds"] = Mode_getLedCount(mode);
  modeJson["flags"] = (uint8_t)Mode_getFlags(mode);

  const Pattern *multiPattern = Mode_getPattern(mode, LED_MULTI);
  if (multiPattern) {
    modeJson["multi_pat"] = patternToJson(multiPattern);
  }

  json singlePatterns = json::array();
  for (LedPos l = LED_FIRST; l < Mode_getLedCount(mode); ++l) {
    const Pattern *pattern = Mode_getPattern(mode, l);
    if (pattern) {
      singlePatterns.push_back(patternToJson(pattern));
    } else {
      singlePatterns.push_back(nullptr);
    }
  }

  modeJson["single_pats"] = singlePatterns;
  return modeJson;
}

Mode *Vortex::modeFromJson(const json &modeJson)
{
  if (modeJson.is_null()) {
    return nullptr;
  }

  Mode *mode = new Mode;
  Mode_init(mode);
  if (!mode) {
    return nullptr;
  }

  if (modeJson.contains("num_leds") && modeJson["num_leds"].is_number_unsigned()) {
    if (modeJson["num_leds"].get<uint8_t>() != LED_COUNT) {
      // error?
    }
  }

  // Extract and set multiPattern
  if (modeJson.contains("multi_pat") && modeJson["multi_pat"].is_object()) {
    const json &multiPatJson = modeJson["multi_pat"];
    Pattern *multiPattern = patternFromJson(multiPatJson);
    if (multiPattern) {
      PatternArgs args;
      PatternArgs_make0(&args);
      Pattern_getArgs(multiPattern, &args);
      Colorset set = Pattern_getColorset(multiPattern);
      Mode_setPattern(mode, Pattern_getPatternID(multiPattern), LED_MULTI, &args, &set);
      Pattern_destroy(multiPattern);
      PatternBuilder_freePattern(multiPattern);
    }
  }

  // Extract and set singlePatterns
  if (modeJson.contains("single_pats") && modeJson["single_pats"].is_array()) {
    LedPos pos = LED_FIRST;
    for (const json &patJson : modeJson["single_pats"]) {
      if (patJson.is_object()) {
        Pattern *pattern = patternFromJson(patJson);
        if (pattern) {
          PatternArgs args;
          PatternArgs_make0(&args);
          Pattern_getArgs(pattern, &args);
          Colorset set = Pattern_getColorset(pattern);
          Mode_setPattern(mode, Pattern_getPatternID(pattern), pos++, &args, &set);
          Pattern_destroy(pattern);
          PatternBuilder_freePattern(pattern);
        }
      }
    }
  }

  // fallback to load 1-led pattern data as the mode if multi_pat and single_pats are missing
  if (!modeJson.contains("multi_pat") && !modeJson.contains("single_pats")) {
    Pattern *pattern = patternFromJson(modeJson);
    if (!pattern) {
      return mode;
    }
    PatternArgs args;
    PatternArgs_make0(&args);
    Pattern_getArgs(pattern, &args);
    Colorset set = Pattern_getColorset(pattern);
    Mode_setPattern(mode, Pattern_getPatternID(pattern), LED_FIRST, &args, &set);
    Pattern_destroy(pattern);
    PatternBuilder_freePattern(pattern);
  }

  return mode;
}

json Vortex::patternToJson(const Pattern *pattern)
{
  if (!pattern) {
    return nullptr;
  }

  json patternJson;
  patternJson["pattern_id"] = Pattern_getPatternID(pattern);
  patternJson["flags"] = Pattern_getFlags(pattern);

  const Colorset &colorset = Pattern_getColorset(pattern);
  patternJson["numColors"] = Colorset_numColors(&colorset);

  json colorsetArray = json::array();
  for (uint8_t c = 0; c < Colorset_numColors(&colorset); ++c) {
    const RGBColor &color = Colorset_get(&colorset, c);
    std::stringstream colorString;
    colorString << "0x"
      << std::setfill('0') << std::setw(2) << std::hex << (int)color.red
      << std::setfill('0') << std::setw(2) << std::hex << (int)color.green
      << std::setfill('0') << std::setw(2) << std::hex << (int)color.blue;
    colorsetArray.push_back(colorString.str());
  }
  patternJson["colorset"] = colorsetArray;

  json argsArray = json::array();
  for (uint8_t a = 0; a < Pattern_getNumArgs(pattern); ++a) {
    argsArray.push_back(Pattern_getArg(pattern, a));
  }
  patternJson["args"] = argsArray;

  return patternJson;
}

Pattern *Vortex::patternFromJson(const json &patternJson)
{
  if (patternJson.is_null()) {
    return nullptr;
  }

  // Get pattern ID
  PatternID id = PATTERN_NONE;
  if (patternJson.contains("pattern_id") && patternJson["pattern_id"].is_number_unsigned()) {
    id = static_cast<PatternID>(patternJson["pattern_id"].get<uint8_t>());
  }

  // Validate the pattern ID
  if (id >= PATTERN_COUNT) {
    return nullptr;
  }

  // Parse out the args
  PatternArgs args;
  PatternArgs_make0(&args);
  if (patternJson.contains("args") && patternJson["args"].is_array()) {
    for (const auto &arg : patternJson["args"]) {
      if (arg.is_number_unsigned()) {
        PatternArgs_addArgs1(&args, static_cast<uint8_t>(arg.get<uint8_t>()));
      }
    }
  }

  // Parse out the colorset
  Colorset set;
  Colorset_make(&set);
  if (patternJson.contains("colorset") && patternJson["colorset"].is_array()) {
    for (const auto &colorElement : patternJson["colorset"]) {
      if (colorElement.is_string()) {
        string strVal = colorElement.get<string>();
        if (strVal.find("0x") == 0) {
          strVal = strVal.substr(2);
        }
        uint32_t dwCol = strtoul(strVal.c_str(), NULL, 16);
        Colorset_addColor(&set, RGBColor_fromU32(dwCol));
      }
    }
  }

  // Build the pattern with ID + args
  Pattern *pattern = PatternBuilder_make(id, &args);
  if (!pattern) {
    return nullptr;
  }

  // Apply colorset and init
  Pattern_setColorset(pattern, &set);
  Pattern_init(pattern);

  return pattern;
}

json Vortex::saveModeToJson()
{
  json saveJson;
  Mode *cur = Modes_curMode();
  if (!cur) {
    return nullptr;
  }
  return modeToJson(cur);
}

bool Vortex::loadModeFromJson(const json &js)
{
  if (js.is_null()) {
    return false;
  }

  // TODO: reintroduce version to vtxmode format
  //uint8_t major = 0;
  //uint8_t minor = 0;
  //if (js.contains("version_major") && js["version_major"].is_number_unsigned()) {
  //  major = js["version_major"].get<uint8_t>();
  //}

  //if (js.contains("version_minor") && js["version_minor"].is_number_unsigned()) {
  //  minor = js["version_minor"].get<uint8_t>();
  //}

  //if (!VortexEngine_checkVersion(major, minor)) {
  //  return false;
  //}

  Mode *mode = modeFromJson(js);
  if (!mode) {
    return false;
  }
  bool rv = Modes_addMode(mode);
  Mode_destroy(mode);
  delete mode;
  return rv;
}

json Vortex::saveToJson()
{
  json saveJson;

  saveJson["version_major"] = static_cast<uint8_t>(VORTEX_VERSION_MAJOR);
  saveJson["version_minor"] = static_cast<uint8_t>(VORTEX_VERSION_MINOR);
  saveJson["global_flags"] = Modes_globalFlags();
  saveJson["brightness"] = static_cast<uint8_t>(Leds_getBrightness());

  uint8_t numModes = Modes_numModes();
  saveJson["num_modes"] = numModes;

  json modesArray = json::array();
  Modes_setCurMode(0);
  for (uint8_t i = 0; i < numModes; ++i) {
    modesArray.push_back(saveModeToJson());
    Modes_nextMode();
  }
  saveJson["modes"] = modesArray;

  return saveJson;
}

bool Vortex::loadFromJson(const json& js)
{
  if (js.is_null()) {
    return false;
  }

  uint8_t major = 0;
  uint8_t minor = 0;
  if (js.contains("version_major") && js["version_major"].is_number_unsigned()) {
    major = js["version_major"].get<uint8_t>();
  }

  if (js.contains("version_minor") && js["version_minor"].is_number_unsigned()) {
    minor = js["version_minor"].get<uint8_t>();
  }

  if (!VortexEngine_checkVersion(major, minor)) {
    return false;
  }

  if (js.contains("brightness") && js["brightness"].is_number_unsigned()) {
    uint8_t brightness = js["brightness"].get<uint8_t>();
    Leds_setBrightness(brightness);
  }

  if (js.contains("global_flags") && js["global_flags"].is_number_unsigned()) {
    uint8_t global_flags = js["global_flags"].get<uint8_t>();
    Modes_setFlag(global_flags, true, false);
  }

  uint8_t num_modes = 0;
  if (js.contains("num_modes") && js["num_modes"].is_number_unsigned()) {
    num_modes = js["num_modes"].get<uint8_t>();
  }

  if (!js.contains("modes") || !js["modes"].is_array()) {
    return false;
  }

  Modes_clearModes();
  for (const auto &modeValue : js["modes"]) {
    if (modeValue.is_null() || !modeValue.is_object()) {
      continue;
    }
    if (!loadModeFromJson(modeValue)) {
      // error?
      return false;
    }
  }

  return Modes_numModes() == num_modes;
}

// print/parse the current mode json
std::string Vortex::printModeJson(bool pretty)
{
  std::string jsonStr = "{}";
  Mode *cur = Modes_curMode();
  if (!cur) {
    return jsonStr;
  }
  json modeJs = modeToJson(cur);
  return pretty ? modeJs.dump(4) : modeJs.dump();
}

bool Vortex::parseModeJson(const std::string &jsonStr)
{
  json jsonObj;
  try {
    jsonObj = json::parse(jsonStr);
  } catch (json::parse_error &e) {
    std::cerr << "JSON Parse Error: " << e.what() << std::endl;
    return false;
  }
  Mode *tempMode = modeFromJson(jsonObj);
  if (!tempMode) {
    return false;
  }
  // update the current mode with the parsed json mode
  bool result = Modes_updateCurMode(tempMode);
  Mode_destroy(tempMode);
  delete tempMode;
  return result;
}

// print/parse a pattern of the current mode json
std::string Vortex::printPatternJson(LedPos pos, bool pretty)
{
  std::string jsonStr = "{}";
  Mode *cur = Modes_curMode();
  if (!cur) {
    return jsonStr;
  }
  Pattern *pat = Mode_getPatternMutable(cur, pos);
  if (!pat) {
    return jsonStr;
  }
  json patternJs = patternToJson(pat);
  return pretty ? patternJs.dump(4) : patternJs.dump();
}

bool Vortex::parsePatternJson(LedPos pos, const std::string &jsonStr)
{
  Mode *cur = Modes_curMode();
  if (!cur) {
    return false;
  }
  json jsonObj;
  try {
    jsonObj = json::parse(jsonStr);
  } catch (json::parse_error &e) {
    std::cerr << "JSON Parse Error: " << e.what() << std::endl;
    return false;
  }
  Pattern *tempPat = patternFromJson(jsonObj);
  if (!tempPat) {
    return false;
  }
  // update the current mode with the parsed json mode
  PatternID patID = Pattern_getPatternID(tempPat);
  PatternArgs args;
  PatternArgs_make0(&args);
  Pattern_getArgs(tempPat, &args);
  Colorset set = Pattern_getColorset(tempPat);
  Pattern_destroy(tempPat);
  PatternBuilder_freePattern(tempPat);
  return Mode_setPattern(cur, patID, pos, &args, &set);
}

// dump the json to output
std::string Vortex::printJson(bool pretty)
{
  json json = saveToJson();
  return pretty ? json.dump(4) : json.dump();
}

bool Vortex::parseJson(const std::string &jsonStr)
{
  try {
    json jsonObj = json::parse(jsonStr);
    return loadFromJson(jsonObj);
  } catch (json::parse_error &e) {
    std::cerr << "JSON Parse Error: " << e.what() << std::endl;
    return false;
  }
}

bool Vortex::printJsonToFile(const std::string &filename, bool pretty)
{
  std::ofstream file(filename);
  if (!file.is_open()) {
    return false;
  }
  std::string jsonStr = printJson(pretty);
  if (!jsonStr.length()) {
    return false;
  }
  file << jsonStr;
  file.close();
  return true;
}

bool Vortex::parseJsonFromFile(const std::string &filename)
{
  std::ifstream file(filename);
  if (!file.is_open()) {
    return false;
  }
  try {
    json jsonObj = json::parse(file);
    return loadFromJson(jsonObj);
  } catch (json::parse_error &e) {
    std::cerr << "JSON Parse Error: " << e.what() << std::endl;
    return false;
  }
}
