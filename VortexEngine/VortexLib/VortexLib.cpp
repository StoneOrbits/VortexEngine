#include "VortexLib.h"

// VortexEngine includes
#include "VortexEngine.h"
#include "Buttons/Button.h"
#include "Serial/ByteStream.h"
#include "Wireless/IRReceiver.h"
#include "Wireless/VLReceiver.h"
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

static void init_wasm()
{
  Vortex::init<VortexWASMCallbacks>();
}

static void cleanup_wasm()
{
  Vortex::cleanup();
}

// This wraps Vortex::tick but returns an array of led colors for the tick
val tick_wasm() {
  Vortex::tick();

  val ledArray = val::array();
  for (int i = 0; i < led_count; ++i) {
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
  const uint8_t *dataPtr = byteStream.data();
  uint32_t size = byteStream.size();

  emscripten::val dataArray = emscripten::val::array();

  for (uint32_t i = 0; i < size; ++i) {
    dataArray.call<void>("push", dataPtr[i]);
  }

  return dataArray;
}

emscripten::val getRawDataArray(const ByteStream &byteStream)
{
  const uint8_t *rawDataPtr = reinterpret_cast<const uint8_t *>(byteStream.rawData());
  uint32_t rawSize = byteStream.rawSize();

  emscripten::val rawDataArray = emscripten::val::array();

  for (uint32_t i = 0; i < rawSize; ++i) {
    rawDataArray.call<void>("push", rawDataPtr[i]);
  }

  return rawDataArray;
}

EMSCRIPTEN_BINDINGS(Vortex) {
  // vector<string>
  register_vector<std::string>("VectorString");

  // basic control functions
  function("Init", &init_wasm);
  function("Cleanup", &cleanup_wasm);
  function("Tick", &tick_wasm);

  // Bind the HSVColor class
  class_<HSVColor>("HSVColor")
    .constructor<>()
    .constructor<uint8_t, uint8_t, uint8_t>()
    .constructor<uint32_t>()
    .function("empty", &HSVColor::empty)
    .function("clear", &HSVColor::clear)
    .function("raw", &HSVColor::raw)
    .property("hue", &HSVColor::hue)
    .property("sat", &HSVColor::sat)
    .property("val", &HSVColor::val);

  // Bind the RGBColor class
  class_<RGBColor>("RGBColor")
    .constructor<>()
    .constructor<uint8_t, uint8_t, uint8_t>()
    .constructor<uint32_t>()
    .function("empty", &RGBColor::empty)
    .function("clear", &RGBColor::clear)
    .function("adjustBrightness", &RGBColor::adjustBrightness)
    .function("serialize", &RGBColor::serialize)
    .function("unserialize", &RGBColor::unserialize)
    .function("raw", &RGBColor::raw)
    .property("red", &RGBColor::red)
    .property("green", &RGBColor::green)
    .property("blue", &RGBColor::blue);

  // Bind the utility conversion functions
  function("hsv_to_rgb_rainbow", &hsv_to_rgb_rainbow);
  function("hsv_to_rgb_raw_C", &hsv_to_rgb_raw_C);
  function("hsv_to_rgb_generic", &hsv_to_rgb_generic);
  function("rgb_to_hsv_approx", &rgb_to_hsv_approx);
  function("rgb_to_hsv_generic", &rgb_to_hsv_generic);

  class_<ByteStream>("ByteStream")
    .constructor<>()
    .constructor<uint32_t, const uint8_t *>()

    // member functions
    .function("init", &ByteStream::init, allow_raw_pointer<const unsigned char *>())
    .function("clear", &ByteStream::clear)
    .function("shrink", &ByteStream::shrink)
    .function("append", &ByteStream::append)
    .function("extend", &ByteStream::extend)
    .function("trim", &ByteStream::trim)
    .function("compress", &ByteStream::compress)
    .function("decompress", &ByteStream::decompress)
    .function("recalcCRC", &ByteStream::recalcCRC)
    .function("sanity", &ByteStream::sanity)
    .function("checkCRC", &ByteStream::checkCRC)
    .function("isCRCDirty", &ByteStream::isCRCDirty)
    .function("serialize", select_overload<bool(uint8_t)>(&ByteStream::serialize))
    .function("serialize16", select_overload<bool(uint16_t)>(&ByteStream::serialize))
    .function("serialize32", select_overload<bool(uint32_t)>(&ByteStream::serialize))
    .function("resetUnserializer", &ByteStream::resetUnserializer)
    .function("moveUnserializer", &ByteStream::moveUnserializer)
    .function("unserializerAtEnd", &ByteStream::unserializerAtEnd)
    .function("unserialize8", &ByteStream::unserialize8)
    .function("unserialize16", &ByteStream::unserialize16)
    .function("unserialize32", &ByteStream::unserialize32)
    .function("peek8", &ByteStream::peek8)
    .function("peek16", &ByteStream::peek16)
    .function("peek32", &ByteStream::peek32)
    .function("data", &ByteStream::data, allow_raw_pointer<uint8_t>())
    .function("rawData", &ByteStream::rawData, allow_raw_pointer<void>())
    .function("rawSize", &ByteStream::rawSize)
    .function("size", &ByteStream::size)
    .function("capacity", &ByteStream::capacity)
    .function("is_compressed", &ByteStream::is_compressed)
    .function("CRC", &ByteStream::CRC);

  // Binding static enum values
  enum_<LedPos>("LedPos")
    .value("LED_FIRST", LedPos::LED_FIRST)
    .value("LED_0", LedPos::LED_0)
    .value("LED_1", LedPos::LED_1)
    .value("LED_2", LedPos::LED_2)
    .value("LED_3", LedPos::LED_3)
    .value("LED_4", LedPos::LED_4)
    .value("LED_5", LedPos::LED_5)
    .value("LED_6", LedPos::LED_6)
    .value("LED_7", LedPos::LED_7)
    .value("LED_8", LedPos::LED_8)
    .value("LED_9", LedPos::LED_9)
#if FIXED_LED_COUNT == 1
    .value("LED_COUNT", LedPos::LED_COUNT)
    .value("LED_LAST", LedPos::LED_LAST)
    .value("LED_ALL", LedPos::LED_ALL)
    .value("LED_MULTI", LedPos::LED_MULTI)
    .value("LED_ALL_SINGLE", LedPos::LED_ALL_SINGLE)
    .value("LED_ANY", LedPos::LED_ANY);
  // If you decide to uncomment and use LED_EVENS and LED_ODDS in the future
  // .value("LED_EVENS", LedPos::LED_EVENS)
  // .value("LED_ODDS", LedPos::LED_ODDS)
#else
    ; // terminate the previous one

  // Binding dynamic values from Leds class
  class_<Leds>("Leds")
    .class_function("ledCount", &Leds::ledCount)
    .class_function("ledLast", &Leds::ledLast)
    .class_function("ledMulti", &Leds::ledMulti)
    .class_function("ledAllSingle", &Leds::ledAllSingle)
    .class_function("ledAny", &Leds::ledAny);
#endif

  enum_<PatternID>("PatternID")
    // Meta Constants
    .value("PATTERN_NONE", PatternID::PATTERN_NONE)
    // single led patterns
    .value("PATTERN_STROBE", PatternID::PATTERN_STROBE)
    .value("PATTERN_HYPERSTROBE", PatternID::PATTERN_HYPERSTROBE)
    .value("PATTERN_PICOSTROBE", PatternID::PATTERN_PICOSTROBE)
    .value("PATTERN_STROBIE", PatternID::PATTERN_STROBIE)
    .value("PATTERN_DOPS", PatternID::PATTERN_DOPS)
    .value("PATTERN_ULTRADOPS", PatternID::PATTERN_ULTRADOPS)
    .value("PATTERN_STROBEGAP", PatternID::PATTERN_STROBEGAP)
    .value("PATTERN_HYPERGAP", PatternID::PATTERN_HYPERGAP)
    .value("PATTERN_PICOGAP", PatternID::PATTERN_PICOGAP)
    .value("PATTERN_STROBIEGAP", PatternID::PATTERN_STROBIEGAP)
    .value("PATTERN_DOPSGAP", PatternID::PATTERN_DOPSGAP)
    .value("PATTERN_ULTRAGAP", PatternID::PATTERN_ULTRAGAP)
    .value("PATTERN_BLINKIE", PatternID::PATTERN_BLINKIE)
    .value("PATTERN_GHOSTCRUSH", PatternID::PATTERN_GHOSTCRUSH)
    .value("PATTERN_DOUBLEDOPS", PatternID::PATTERN_DOUBLEDOPS)
    .value("PATTERN_CHOPPER", PatternID::PATTERN_CHOPPER)
    .value("PATTERN_DASHGAP", PatternID::PATTERN_DASHGAP)
    .value("PATTERN_DASHDOPS", PatternID::PATTERN_DASHDOPS)
    .value("PATTERN_DASHCRUSH", PatternID::PATTERN_DASHCRUSH)
    .value("PATTERN_ULTRADASH", PatternID::PATTERN_ULTRADASH)
    .value("PATTERN_GAPCYCLE", PatternID::PATTERN_GAPCYCLE)
    .value("PATTERN_DASHCYCLE", PatternID::PATTERN_DASHCYCLE)
    .value("PATTERN_TRACER", PatternID::PATTERN_TRACER)
    .value("PATTERN_RIBBON", PatternID::PATTERN_RIBBON)
    .value("PATTERN_MINIRIBBON", PatternID::PATTERN_MINIRIBBON)
    .value("PATTERN_BLEND", PatternID::PATTERN_BLEND)
    .value("PATTERN_BLENDSTROBE", PatternID::PATTERN_BLENDSTROBE)
    .value("PATTERN_BLENDSTROBEGAP", PatternID::PATTERN_BLENDSTROBEGAP)
    .value("PATTERN_COMPLEMENTARY_BLEND", PatternID::PATTERN_COMPLEMENTARY_BLEND)
    .value("PATTERN_COMPLEMENTARY_BLENDSTROBE", PatternID::PATTERN_COMPLEMENTARY_BLENDSTROBE)
    .value("PATTERN_COMPLEMENTARY_BLENDSTROBEGAP", PatternID::PATTERN_COMPLEMENTARY_BLENDSTROBEGAP)
    .value("PATTERN_SOLID", PatternID::PATTERN_SOLID)
    // multi led patterns
    .value("PATTERN_HUE_SCROLL", PatternID::PATTERN_HUE_SCROLL)
    .value("PATTERN_THEATER_CHASE", PatternID::PATTERN_THEATER_CHASE)
    .value("PATTERN_CHASER", PatternID::PATTERN_CHASER)
    .value("PATTERN_ZIGZAG", PatternID::PATTERN_ZIGZAG)
    .value("PATTERN_ZIPFADE", PatternID::PATTERN_ZIPFADE)
    .value("PATTERN_DRIP", PatternID::PATTERN_DRIP)
    .value("PATTERN_DRIPMORPH", PatternID::PATTERN_DRIPMORPH)
    .value("PATTERN_CROSSDOPS", PatternID::PATTERN_CROSSDOPS)
    .value("PATTERN_DOUBLESTROBE", PatternID::PATTERN_DOUBLESTROBE)
    .value("PATTERN_METEOR", PatternID::PATTERN_METEOR)
    .value("PATTERN_SPARKLETRACE", PatternID::PATTERN_SPARKLETRACE)
    .value("PATTERN_VORTEXWIPE", PatternID::PATTERN_VORTEXWIPE)
    .value("PATTERN_WARP", PatternID::PATTERN_WARP)
    .value("PATTERN_WARPWORM", PatternID::PATTERN_WARPWORM)
    .value("PATTERN_SNOWBALL", PatternID::PATTERN_SNOWBALL)
    .value("PATTERN_LIGHTHOUSE", PatternID::PATTERN_LIGHTHOUSE)
    .value("PATTERN_PULSISH", PatternID::PATTERN_PULSISH)
    .value("PATTERN_FILL", PatternID::PATTERN_FILL)
    .value("PATTERN_BOUNCE", PatternID::PATTERN_BOUNCE)
    .value("PATTERN_SPLITSTROBIE", PatternID::PATTERN_SPLITSTROBIE)
    .value("PATTERN_BACKSTROBE", PatternID::PATTERN_BACKSTROBE)
    .value("PATTERN_MATERIA", PatternID::PATTERN_MATERIA);

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
    .constructor<>()
    .constructor<RGBColor, RGBColor, RGBColor, RGBColor, RGBColor, RGBColor, RGBColor, RGBColor>()
    .constructor<uint8_t, const uint32_t *>()
    .function("init", &Colorset::init)
    .function("clear", &Colorset::clear)
    .function("equals", select_overload<bool(const Colorset & set) const>(&Colorset::equals))
    .function("get", &Colorset::get)
    .function("set", &Colorset::set)
    .function("skip", &Colorset::skip)
    .function("cur", &Colorset::cur)
    .function("setCurIndex", &Colorset::setCurIndex)
    .function("resetIndex", &Colorset::resetIndex)
    .function("curIndex", &Colorset::curIndex)
    .function("getPrev", &Colorset::getPrev)
    .function("getNext", &Colorset::getNext)
    .function("peek", &Colorset::peek)
    .function("peekNext", &Colorset::peekNext)
    .function("numColors", &Colorset::numColors)
    .function("onStart", &Colorset::onStart)
    .function("onEnd", &Colorset::onEnd)
    .function("serialize", &Colorset::serialize)
    .function("unserialize", &Colorset::unserialize)
    .function("addColor", select_overload<bool(RGBColor)>(&Colorset::addColor))
    .function("addColorHSV", &Colorset::addColorHSV)
    .function("randomize", &Colorset::randomize)
    .function("adjustBrightness", &Colorset::adjustBrightness)
    .function("removeColor", &Colorset::removeColor)
    .function("operator[]", &Colorset::operator[])
    // If more specific or additional randomize functions are needed, they can be added here as well
    .function("randomizeSolid", &Colorset::randomizeSolid)
    .function("randomizeComplimentary", &Colorset::randomizeComplimentary)
    .function("randomizeTriadic", &Colorset::randomizeTriadic)
    .function("randomizeSquare", &Colorset::randomizeSquare)
    .function("randomizePentadic", &Colorset::randomizePentadic)
    .function("randomizeRainbow", &Colorset::randomizeRainbow);

  class_<PatternArgs>("PatternArgs")
    .constructor<>()
    .constructor<uint8_t>()
    .constructor<uint8_t, uint8_t>()
    .constructor<uint8_t, uint8_t, uint8_t>()
    .constructor<uint8_t, uint8_t, uint8_t, uint8_t>()
    .constructor<uint8_t, uint8_t, uint8_t, uint8_t, uint8_t>()
    .constructor<uint8_t, uint8_t, uint8_t, uint8_t, uint8_t, uint8_t>()
    .constructor<uint8_t, uint8_t, uint8_t, uint8_t, uint8_t, uint8_t, uint8_t>()
    .constructor<uint8_t, uint8_t, uint8_t, uint8_t, uint8_t, uint8_t, uint8_t, uint8_t>()
    .function("init", select_overload<void()>(&PatternArgs::init))
    .function("init", select_overload<void(uint8_t)>(&PatternArgs::init))
    .function("init", select_overload<void(uint8_t, uint8_t)>(&PatternArgs::init))
    .function("init", select_overload<void(uint8_t, uint8_t, uint8_t)>(&PatternArgs::init))
    .function("init", select_overload<void(uint8_t, uint8_t, uint8_t, uint8_t)>(&PatternArgs::init))
    .function("init", select_overload<void(uint8_t, uint8_t, uint8_t, uint8_t, uint8_t)>(&PatternArgs::init))
    .function("init", select_overload<void(uint8_t, uint8_t, uint8_t, uint8_t, uint8_t, uint8_t)>(&PatternArgs::init))
    .function("init", select_overload<void(uint8_t, uint8_t, uint8_t, uint8_t, uint8_t, uint8_t, uint8_t)>(&PatternArgs::init))
    .function("init", select_overload<void(uint8_t, uint8_t, uint8_t, uint8_t, uint8_t, uint8_t, uint8_t, uint8_t)>(&PatternArgs::init))
    .property("arg1", &PatternArgs::arg1)
    .property("arg2", &PatternArgs::arg2)
    .property("arg3", &PatternArgs::arg3)
    .property("arg4", &PatternArgs::arg4)
    .property("arg5", &PatternArgs::arg5)
    .property("arg6", &PatternArgs::arg6)
    .property("arg7", &PatternArgs::arg7)
    .property("arg8", &PatternArgs::arg8);

  class_<Pattern>("Pattern")
    .function("bind", &Pattern::bind)
    .function("init", &Pattern::init)
    .function("serialize", &Pattern::serialize)
    .function("unserialize", &Pattern::unserialize)
    .function("setArg", &Pattern::setArg)
    .function("getArg", &Pattern::getArg)
    .function("setArgs", &Pattern::setArgs)
    .function("getArgs", &Pattern::getArgs)
    .function("getNumArgs", &Pattern::getNumArgs)
    .function("equals", &Pattern::equals, allow_raw_pointer<const Pattern *>())
    .function("getColorset", select_overload<const Colorset() const>(&Pattern::getColorset))
    .function("setColorset", &Pattern::setColorset)
    .function("clearColorset", &Pattern::clearColorset)
    .function("setLedPos", &Pattern::setLedPos)
    .function("getPatternID", &Pattern::getPatternID)
    .function("getLedPos", &Pattern::getLedPos)
    .function("getFlags", &Pattern::getFlags)
    .function("hasFlags", &Pattern::hasFlags);

  class_<PatternBuilder>("PatternBuilder")
    .class_function("make", &PatternBuilder::make, allow_raw_pointers())
    .class_function("dupe", &PatternBuilder::dupe, allow_raw_pointers())
    .class_function("makeSingle", &PatternBuilder::makeSingle, allow_raw_pointers())
    .class_function("makeMulti", &PatternBuilder::makeMulti, allow_raw_pointers())
    //.class_function("unserialize", &PatternBuilder::unserialize)
    .class_function("getDefaultArgs", &PatternBuilder::getDefaultArgs)
    .class_function("numDefaultArgs", &PatternBuilder::numDefaultArgs);

  class_<Mode>("Mode")
    .constructor<>()
    // overloading only works with param count not typing
    //.constructor<PatternID, const Colorset &>()
    //.constructor<PatternID, const PatternArgs &, const Colorset &>()
    //.constructor<PatternID, const PatternArgs *, const Colorset *>()
    .constructor<const Mode *>()
    .function("copyFrom", select_overload<void(const Mode &)>(&Mode::operator=))
    .function("equals", &Mode::operator==)
    .function("notEquals", &Mode::operator!=)
    .function("init", &Mode::init)
    .function("play", &Mode::play)
    .function("saveToBuffer", &Mode::saveToBuffer)
    .function("loadFromBuffer", &Mode::loadFromBuffer)
    .function("serialize", &Mode::serialize)
    .function("unserialize", &Mode::unserialize)
    .function("equalsMode", &Mode::equals, allow_raw_pointer<const Mode *>())
    .function("getLedCount", &Mode::getLedCount)
    .function("getColorset", select_overload<const Colorset(LedPos) const>(&Mode::getColorset))
    .function("getPatternID", &Mode::getPatternID)
    .function("getPattern", select_overload<Pattern * (LedPos)>(&Mode::getPattern), emscripten::allow_raw_pointers())
    .function("getConstPattern", select_overload<const Pattern * (LedPos) const>(&Mode::getPattern), emscripten::allow_raw_pointers())
    .function("setPattern", static_cast<bool(Mode:: *)(PatternID, LedPos, const PatternArgs *, const Colorset *)>(&Mode::setPattern), emscripten::allow_raw_pointers())
    .function("setPattern2", &Mode::setPattern, allow_raw_pointers())
    .function("setColorset", select_overload<bool(const Colorset &, LedPos)>(&Mode::setColorset))
    .function("clearPattern", &Mode::clearPattern)
    .function("clearColorset", &Mode::clearColorset)
    .function("setArg", &Mode::setArg)
    .function("getArg", &Mode::getArg);

  class_<Modes>("Modes")
    .class_function("init", &Modes::init)
    .class_function("cleanup", &Modes::cleanup)
    .class_function("play", &Modes::play)
    .class_function("saveToBuffer", select_overload<bool(ByteStream&)>(&Modes::saveToBuffer))
    .class_function("loadFromBuffer", select_overload<bool(ByteStream&)>(&Modes::loadFromBuffer))
    .class_function("loadStorage", &Modes::loadStorage)
    .class_function("saveStorage", &Modes::saveStorage)
    .class_function("serialize", &Modes::serialize)
    .class_function("unserialize", &Modes::unserialize)
    .class_function("setDefaults", &Modes::setDefaults)
    .class_function("shiftCurMode", &Modes::shiftCurMode)
    .class_function("updateCurMode", &Modes::updateCurMode, allow_raw_pointers())
    .class_function("setCurMode", &Modes::setCurMode, allow_raw_pointers())
    .class_function("curMode", &Modes::curMode, allow_raw_pointers())
    .class_function("nextMode", &Modes::nextMode, allow_raw_pointers())
    .class_function("previousMode", &Modes::previousMode, allow_raw_pointers())
    .class_function("nextModeSkipEmpty", &Modes::nextModeSkipEmpty, allow_raw_pointers())
    .class_function("numModes", &Modes::numModes)
    .class_function("curModeIndex", &Modes::curModeIndex)
    .class_function("lastSwitchTime", &Modes::lastSwitchTime)
    .class_function("deleteCurMode", &Modes::deleteCurMode)
    .class_function("clearModes", &Modes::clearModes)
    .class_function("setStartupMode", &Modes::setStartupMode)
    .class_function("startupMode", &Modes::startupMode)
    .class_function("setFlag", &Modes::setFlag)
    .class_function("getFlag", &Modes::getFlag)
    .class_function("resetFlags", &Modes::resetFlags)
    .class_function("setOneClickMode", &Modes::setOneClickMode)
    .class_function("oneClickModeEnabled", &Modes::oneClickModeEnabled)
    .class_function("setLocked", &Modes::setLocked)
    .class_function("locked", &Modes::locked)
    .class_function("setAdvancedMenus", &Modes::setAdvancedMenus)
    .class_function("advancedMenusEnabled", &Modes::advancedMenusEnabled)
    .class_function("setKeychainMode", &Modes::setKeychainMode)
    .class_function("keychainModeEnabled", &Modes::keychainModeEnabled);

  class_<Vortex>("Vortex")
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
    .class_function("getMenuDemoMode", &Vortex::getMenuDemoMode, allow_raw_pointer<arg<1>>())
    .class_function("setMenuDemoMode", &Vortex::setMenuDemoMode, allow_raw_pointer<arg<1>>())
    .class_function("quitClick", &Vortex::quitClick)
    .class_function("IRDeliver", &Vortex::IRDeliver)
    .class_function("VLDeliver", &Vortex::VLDeliver)
    //.class_function("getStorageStats", &Vortex::getStorageStats)
    .class_function("loadStorage", &Vortex::loadStorage)
    .class_function("openRandomizer", &Vortex::openRandomizer)
    .class_function("openColorSelect", &Vortex::openColorSelect)
    .class_function("openPatternSelect", &Vortex::openPatternSelect)
    .class_function("openGlobalBrightness", &Vortex::openGlobalBrightness)
    .class_function("openFactoryReset", &Vortex::openFactoryReset)
    .class_function("openModeSharing", &Vortex::openModeSharing)
    .class_function("openEditorConnection", &Vortex::openEditorConnection)
    .class_function("getModes", &Vortex::getModes)
    .class_function("setModes", &Vortex::setModes)
    .class_function("getCurMode", &Vortex::getCurMode)
    .class_function("matchLedCount", &Vortex::matchLedCount)
    .class_function("checkLedCount", &Vortex::checkLedCount)
    .class_function("setLedCount", &Vortex::setLedCount)
    .class_function("curModeIndex", &Vortex::curModeIndex)
    .class_function("numModes", &Vortex::numModes)
    .class_function("numLedsInMode", &Vortex::numLedsInMode)
    //.class_function("addNewMode", select_overload<bool(Random*, bool)>(&Vortex::addNewMode))
    .class_function("addNewMode", select_overload<bool(ByteStream &, bool)>(&Vortex::addNewMode))
    .class_function("setCurMode", &Vortex::setCurMode)
    .class_function("nextMode", &Vortex::nextMode)
    .class_function("delCurMode", &Vortex::delCurMode)
    .class_function("shiftCurMode", &Vortex::shiftCurMode)
    //.class_function("setPattern", &Vortex::setPattern)
    .class_function("getPatternID", &Vortex::getPatternID)
    .class_function("getPatternName", &Vortex::getPatternName)
    .class_function("getModeName", &Vortex::getModeName)
    //.class_function("setPatternAt", &Vortex::setPatternAt)
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
    //.class_function("vcallbacks", &Vortex::vcallbacks)
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
    .class_function("setLockEnabled", &Vortex::setLockEnabled)
    .class_function("lockEnabled", &Vortex::lockEnabled);

  function("getDataArray", &getDataArray);
  function("getRawDataArray", &getRawDataArray);


}
#endif

using namespace std;

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
string Vortex::m_commandLog;
bool Vortex::m_commandLogEnabled = false;
bool Vortex::m_lockstepEnabled = false;
bool Vortex::m_storageEnabled = false;
bool Vortex::m_sleepEnabled = true;
bool Vortex::m_lockEnabled = true;

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

  // init the engine
  VortexEngine::init();
  // clear the modes
  //Modes::clearModes();
  // save and set undo buffer
  doSave();

  m_initialized = true;

  return true;
}

void Vortex::cleanup()
{
  VortexEngine::cleanup();
  if (m_storedCallbacks) {
    delete m_storedCallbacks;
    m_storedCallbacks = nullptr;
  }
  m_initialized = false;
}

void Vortex::handleRepeat(char c)
{
  if (!isdigit(c) || !m_lastCommand) {
    return;
  }
  int repeatAmount = c - '0';
  char newc = 0;
  // read the digits into the repeatAmount
  while (1) {
    newc = getchar();
    if (!isdigit(newc)) {
      // stop once we reach a non digit
      break;
    }
    // accumulate the digits into the repeat amount
    repeatAmount = (repeatAmount * 10) + (newc - '0');
  }
  if (repeatAmount > 0) {
    // offset repeat amount by exactly 1 because it's already done
    repeatAmount--;
  }
  // shove the last non-digit back into the stream
  ungetc(newc, stdin);
  DEBUG_LOGF("Repeating last command (%c) x%u times", m_lastCommand, repeatAmount);
  m_commandLog += to_string(repeatAmount);
  // check to see if we are repeating a 'rapid click' which is a special case
  // because the rapid click command itself is composed or 'r' and a repeat count
  // to designate how many rapid clicks to deliver
  if (m_lastCommand == 'r') {
    // the repeat amount is normally decremented to account for the first command
    // so we add 1 to counter-act that logic above because the initial 'r' does nothing
    // unlike most other commands that are repeated
    Vortex::rapidClick(repeatAmount + 1);
    // don't actually repeat the command just return
    return;
  }
  // repeat the last command that many times
  while (repeatAmount > 0) {
    doCommand(m_lastCommand);
    repeatAmount--;
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
    Vortex::shortClick();
    break;
  case 'l':
    if (m_lastCommand != c) {
      DEBUG_LOG("Injecting long click");
    }
    Vortex::longClick();
    break;
  case 'm':
    if (m_lastCommand != c) {
      DEBUG_LOG("Injecting menu enter click");
    }
    Vortex::menuEnterClick();
    break;
  case 'a':
    if (m_lastCommand != c) {
      DEBUG_LOG("Injecting adv menu enter click");
    }
    Vortex::advMenuEnterClick();
    break;
  case 'd':
    if (m_lastCommand != c) {
      DEBUG_LOG("Injecting adv menu enter click");
    }
    Vortex::deleteColClick();
    break;
  case 's':
    if (m_lastCommand != c) {
      DEBUG_LOG("Injecting sleep click");
    }
    Vortex::sleepClick();
    break;
  case 'f':
    if (m_lastCommand != c) {
      DEBUG_LOG("Injecting force sleep click");
    }
    Vortex::forceSleepClick();
    break;
  case 'q':
    //case '\n':
    if (m_lastCommand != c) {
      DEBUG_LOG("Injecting quit click\n");
    }
    Vortex::quitClick();
    break;
  case 't':
    if (Vortex::isButtonPressed()) {
      if (m_lastCommand != c) {
        DEBUG_LOG("Injecting release");
      }
      Vortex::releaseButton();
    } else {
      if (m_lastCommand != c) {
        DEBUG_LOG("Injecting press");
      }
      Vortex::pressButton();
    }
    break;
  case 'r':
    // do nothing for the initial 'r', handleRepeat() will handle the numeric
    // repeat count that follows this letter and issue the rapidClick(amt) call
    break;
  case 'w':
    if (m_lastCommand != c) {
      DEBUG_LOG("Injecting wait");
    }
    Vortex::sendWait();
    break;
  default:
    handleRepeat(c);
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
  VortexEngine::enterSleep(save);
}

bool Vortex::isSleeping()
{
  return VortexEngine::isSleeping();
}

bool Vortex::tick()
{
  if (!m_initialized) {
    cleanup();
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
  VortexEngine::tick();
  return true;
}


void Vortex::installCallbacks(VortexCallbacks *callbacks)
{
  m_storedCallbacks = callbacks;
}

void Vortex::setInstantTimestep(bool timestep)
{
  Time::setInstantTimestep(timestep);
}

// send various clicks
void Vortex::shortClick(uint32_t buttonIndex)
{
  m_buttonEventQueue.push_back(VortexButtonEvent(buttonIndex, EVENT_SHORT_CLICK));
}

void Vortex::longClick(uint32_t buttonIndex)
{
  m_buttonEventQueue.push_back(VortexButtonEvent(buttonIndex, EVENT_LONG_CLICK));
}

void Vortex::menuEnterClick(uint32_t buttonIndex)
{
  m_buttonEventQueue.push_back(VortexButtonEvent(buttonIndex, EVENT_MENU_ENTER_CLICK));
}

void Vortex::advMenuEnterClick(uint32_t buttonIndex)
{
  m_buttonEventQueue.push_back(VortexButtonEvent(buttonIndex, EVENT_ADV_MENU_ENTER_CLICK));
}

void Vortex::deleteColClick(uint32_t buttonIndex)
{
  m_buttonEventQueue.push_back(VortexButtonEvent(buttonIndex, EVENT_DELETE_COL));
}

void Vortex::sleepClick(uint32_t buttonIndex)
{
  m_buttonEventQueue.push_back(VortexButtonEvent(buttonIndex, EVENT_SLEEP_CLICK));
}

void Vortex::forceSleepClick(uint32_t buttonIndex)
{
  m_buttonEventQueue.push_back(VortexButtonEvent(buttonIndex, EVENT_FORCE_SLEEP_CLICK));
}

void Vortex::pressButton(uint32_t buttonIndex)
{
  m_buttonsPressed |= (1 << buttonIndex);
}

void Vortex::releaseButton(uint32_t buttonIndex)
{
  m_buttonsPressed &= ~(1 << buttonIndex);
}

bool Vortex::isButtonPressed(uint32_t buttonIndex)
{
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
  Menu *pMenu = Menus::curMenu();
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
  Menu *pMenu = Menus::curMenu();
  if (!pMenu) {
    return false;
  }
  if (!mode->getLedCount() || pMenu->m_previewMode.equals(mode)) {
    return false;
  }
  pMenu->m_previewMode = *mode;
  pMenu->m_previewMode.init();
  return true;
}

void Vortex::quitClick()
{
  m_buttonEventQueue.push_back(VortexButtonEvent(0, EVENT_QUIT_CLICK));
}

void Vortex::IRDeliver(uint32_t timing)
{
#if IR_ENABLE_RECEIVER == 1
  IRReceiver::handleIRTiming(timing);
#endif
}

void Vortex::VLDeliver(uint32_t timing)
{
#if VL_ENABLE_RECEIVER == 1
  VLReceiver::handleVLTiming(timing);
#endif
}

void Vortex::getStorageStats(uint32_t *outTotal, uint32_t *outUsed)
{
  if (outTotal) {
    *outTotal = VortexEngine::totalStorageSpace();
  }
  if (outUsed) {
    *outUsed = VortexEngine::savefileSize();
  }
}

void Vortex::loadStorage()
{
  Modes::loadStorage();
}

void Vortex::openRandomizer()
{
  Menus::openMenu(MENU_RANDOMIZER);
}

void Vortex::openColorSelect()
{
  Menus::openMenu(MENU_COLOR_SELECT);
}

void Vortex::openPatternSelect()
{
  Menus::openMenu(MENU_PATTERN_SELECT);
}

void Vortex::openGlobalBrightness()
{
  Menus::openMenu(MENU_GLOBAL_BRIGHTNESS);
}

void Vortex::openFactoryReset()
{
  Menus::openMenu(MENU_FACTORY_RESET);
}

void Vortex::openModeSharing()
{
  Menus::openMenu(MENU_MODE_SHARING);
}

void Vortex::openEditorConnection()
{
#if ENABLE_EDITOR_CONNECTION == 1
  Menus::openMenu(MENU_EDITOR_CONNECTION);
#endif
}

bool Vortex::getModes(ByteStream &outStream)
{
  // now serialize all the modes
  Modes::saveToBuffer(outStream);
  return true;
}

bool Vortex::setModes(ByteStream &stream, bool save)
{
  Modes::clearModes();
  // now unserialize the stream of data that was read
  if (!Modes::loadFromBuffer(stream)) {
    //printf("Unserialize failed\n");
    return false;
  }
  return !save || doSave();
}

bool Vortex::matchLedCount(ByteStream &stream)
{
#if FIXED_LED_COUNT == 0
  if (!stream.decompress()) {
    return false;
  }
  // reset the unserializer index before unserializing anything
  stream.resetUnserializer();
  uint8_t major = 0;
  uint8_t minor = 0;
  // unserialize the vortex version
  stream.unserialize(&major);
  stream.unserialize(&minor);
  uint8_t flags;
  stream.unserialize(&flags);
  // unserialize the global brightness
  uint8_t brightness = 0;
  stream.unserialize(&brightness);
  uint8_t numModes = 0;
  stream.unserialize(&numModes);
  uint8_t ledCount = 0;
  stream.unserialize(&ledCount);
  // put the unserializer back where it was for the next thing
  stream.resetUnserializer();
  return setLedCount(ledCount);
#else
  return false;
#endif
}

bool Vortex::checkLedCount()
{
#if FIXED_LED_COUNT == 0
  Mode *mode = Modes::curMode();
  if (!mode) {
    return false;
  }
  uint8_t numLeds = mode->getLedCount();
  if (numLeds != LED_COUNT) {
    Leds::setLedCount(numLeds);
  }
#endif
  return true;
}

bool Vortex::setLedCount(uint8_t count)
{
#if FIXED_LED_COUNT == 0
  Mode *cur = Modes::curMode();
  if (cur && !cur->setLedCount(count)) {
    return false;
  }
  Leds::setLedCount(count);
#endif
  return true;
}

bool Vortex::getCurMode(ByteStream &outStream)
{
  Mode *pMode = Modes::curMode();
  if (!pMode) {
    return false;
  }
  // save to ensure we get the correct mode, not using doSave() because it causes
  // an undo buffer entry to be added
  if (!Modes::saveStorage()) {
    return false;
  }
  return Modes::curMode()->saveToBuffer(outStream);
}

uint32_t Vortex::curModeIndex()
{
  return Modes::curModeIndex();
}

uint32_t Vortex::numModes()
{
  return Modes::numModes();
}

uint32_t Vortex::numLedsInMode()
{
  Mode *pMode = Modes::curMode();
  if (!pMode) {
    return false;
  }
  return pMode->getLedCount();
}

bool Vortex::addNewMode(Random *pRandCtx, bool save)
{
  Colorset set;
  Random ctx;
  if (!pRandCtx) {
    pRandCtx = &ctx;
  }
  set.randomize(*pRandCtx);
  // create a random pattern ID from all patterns
  PatternID randomPattern;
  do {
    // continuously re-randomize the pattern so we don't get solids
    randomPattern = (PatternID)pRandCtx->next16(PATTERN_FIRST, PATTERN_COUNT);
  } while (randomPattern == PATTERN_SOLID);
  if (!Modes::addMode(randomPattern, nullptr, &set)) {
    return false;
  }
  return !save || doSave();
}

bool Vortex::addNewMode(ByteStream &stream, bool save)
{
  if (!Modes::addModeFromBuffer(stream)) {
    return false;
  }
  return !save || doSave();
}

bool Vortex::setCurMode(uint32_t index, bool save)
{
  if (index >= Modes::numModes()) {
    return true;
  }
  if (!Modes::setCurMode(index)) {
    return false;
  }
  return !save || doSave();
}

bool Vortex::nextMode(bool save)
{
  if (!Modes::numModes()) {
    return true;
  }
  Modes::nextMode();
  return !save || doSave();
}

bool Vortex::delCurMode(bool save)
{
  if (!Modes::numModes()) {
    return true;
  }
  Modes::deleteCurMode();
  return !save || doSave();
}

bool Vortex::shiftCurMode(int8_t offset, bool save)
{
  if (!Modes::numModes()) {
    return true;
  }
  if (offset == 0) {
    return true;
  }
  Modes::shiftCurMode(offset);
  return !save || doSave();
}

bool Vortex::setPattern(PatternID id, const PatternArgs *args, const Colorset *set, bool save)
{
  Mode *pMode = Modes::curMode();
  if (!pMode) {
    return false;
  }
  if (!pMode->setPattern(id, LED_ANY, args, set)) {
    return false;
  }
  pMode->init();
  return !save || doSave();
}

PatternID Vortex::getPatternID(LedPos pos)
{
  Mode *pMode = Modes::curMode();
  if (!pMode) {
    return PATTERN_NONE;
  }
  return pMode->getPatternID(pos);
}

string Vortex::getPatternName(LedPos pos)
{
  return patternToString(getPatternID(pos));
}

string Vortex::getModeName()
{
  Mode *pMode = Modes::curMode();
  if (!pMode) {
    return patternToString(PATTERN_NONE);
  }
  if (pMode->isMultiLed()) {
    return patternToString(pMode->getPatternID(LED_MULTI));
  }
  // can't use isSampleSingleLed because that will compare the entire
  // pattern for differences in any single led pattern, we only care
  // about the pattern id being different
  bool all_same_id = true;
  PatternID first = pMode->getPatternID(LED_FIRST);
  for (uint32_t i = LED_FIRST + 1; i < numLedsInMode(); ++i) {
    // if any don't match 0 then no good
    if (pMode->getPatternID((LedPos)i) != first) {
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
  Mode *pMode = Modes::curMode();
  if (!pMode) {
    return false;
  }
  if (!pMode->setPattern(id, pos, args, set)) {
    return false;
  }
  pMode->init();
  return !save || doSave();
}

bool Vortex::getColorset(LedPos pos, Colorset &set)
{
  Mode *pMode = Modes::curMode();
  if (!pMode) {
    return false;
  }
  set = pMode->getColorset(pos);
  return true;
}

bool Vortex::setColorset(LedPos pos, const Colorset &set, bool save)
{
  Mode *pMode = Modes::curMode();
  if (!pMode) {
    return false;
  }
  if (!pMode->setColorset(set, pos)) {
    return false;
  }
  pMode->init();
  return !save || doSave();
}

bool Vortex::getPatternArgs(LedPos pos, PatternArgs &args)
{
  Mode *pMode = Modes::curMode();
  if (!pMode) {
    return false;
  }
  Pattern *pat = pMode->getPattern(pos);
  if (!pat) {
    return false;
  }
  pat->getArgs(args);
  return true;
}

bool Vortex::setPatternArgs(LedPos pos, PatternArgs &args, bool save)
{
  Mode *pMode = Modes::curMode();
  if (!pMode) {
    return false;
  }
  Pattern *pat = nullptr;

  // Equivalent to cases LED_ANY, LED_ALL and LED_MULTI
  if (pos == LED_ANY || pos == LED_ALL || pos == LED_MULTI) {
#if VORTEX_SLIM == 0
    pat = pMode->getPattern(LED_MULTI);
    if (pat) {
      pat->setArgs(args);
    }
    if (pos == LED_MULTI) {
      // don't fallthrough if actually multi, it's possible
      // we got here by falling through from LED_ALL
      pMode->init();
      return !save || doSave();
    }
    // fall through if LED_ALL and change the single leds
    else if (pos == LED_ANY || pos == LED_ALL) {
      for (LedPos pos = LED_FIRST; pos < LED_COUNT; ++pos) {
        pat = pMode->getPattern(pos);
        if (pat) {
          pat->setArgs(args);
        }
      }
      pMode->init();
      // actually break here
      return !save || doSave();
    }
#endif
  }

  // equivalent to case LED_ALL_SINGLE
  else if (pos == LED_ALL_SINGLE) {
    for (LedPos pos = LED_FIRST; pos < LED_COUNT; ++pos) {
      pat = pMode->getPattern(pos);
      if (pat) {
        pat->setArgs(args);
      }
    }
    pMode->init();
    // actually break here
    return !save || doSave();
  }

  // equivalent to default case (covers any other pos)
  else {
    if (pos >= LED_COUNT) {
      return false;
    }
    pat = pMode->getPattern(pos);
    if (!pat) {
      return false;
    }
    pat->setArgs(args);
    pMode->init();
    return !save || doSave();
  }

  return false;
}

bool Vortex::isCurModeMulti()
{
  Mode *pMode = Modes::curMode();
  if (!pMode) {
    return false;
  }
  return pMode->isMultiLed();
}

string Vortex::patternToString(PatternID id)
{
  // I wish there was a way to do this automatically but it would be
  // quite messy and idk if it's worth it
  static const char *patternNames[] = {
    "strobe", "hyperstrobe", "picostrobe", "strobie", "dops", "ultradops", "strobegap",
    "hypergap", "picogap", "strobiegap", "dopsgap", "ultragap", "blinkie",
    "ghostcrush", "doubledops", "chopper", "dashgap", "dashdops", "dash-crush",
    "ultradash", "gapcycle", "dashcycle", "tracer", "ribbon", "miniribbon",
    "blend", "blendstrobe", "blendstrobegap", "complementary_blend",
    "complementary_blendstrobe", "complementary_blendstrobegap", "solid",
    "hueshift", "theater_chase", "chaser", "zigzag", "zipfade", "drip",
    "dripmorph", "crossdops", "doublestrobe", "meteor", "sparkletrace",
    "vortexwipe", "warp", "warpworm", "snowball", "lighthouse", "pulsish",
    "fill", "bounce", "splitstrobie", "backstrobe", "materia",
  };
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

// this shouldn't change much so this is fine
string Vortex::ledToString(LedPos pos)
{
  return "led " + to_string(pos);
}

// the number of custom parameters for any given pattern id
uint32_t Vortex::numCustomParams(PatternID id)
{
  return PatternBuilder::numDefaultArgs(id);
}

vector<string> Vortex::getCustomParams(PatternID id)
{
  Pattern *pat = PatternBuilder::make(id);
  vector<string> params;
  if (!pat) {
    return params;
  }
  for (uint32_t i = 0; i < pat->getNumArgs(); ++i) {
    params.push_back(pat->getArgName(i));
  }
  delete pat;
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
  getModes(modes);
  if (!modes.size()) {
    return false;
  }
  // only save an undo if the buffer is different
  if (m_undoBuffer.size() > 0 && modes.CRC() == m_undoBuffer.back().CRC()) {
    //printf("Not saving duplicate undo buffer\n");
    return false;
  }
  // must rewind to that step before pushing next step
  while (m_undoIndex > 0) {
    m_undoBuffer.pop_back();
    m_undoIndex--;
  }
  m_undoBuffer.push_back(modes);
  //printf("Pushing undo buffer (pos: %u)\n", m_undoIndex);
  // make sure list doesn't grow too big
  if (m_undoLimit && m_undoBuffer.size() > m_undoLimit) {
    //printf("Popping front of undo buffer\n");
    m_undoBuffer.pop_front();
  }
#if 0
  printf("Buffer:\n");
  for (uint32_t i = 0; i < m_undoBuffer.size(); ++i) {
    printf("\t%u: %x", i, m_undoBuffer[i].CRC());
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
    printf("\t%u: %x", i, m_undoBuffer[i].CRC());
    if ((m_undoBuffer.size() - 1) - m_undoIndex == i) {
      printf(" <--\n");
    } else {
      printf("\n");
    }
  }
#endif
  // index from the back instead of the front
  uint32_t backIndex = highestIndex - m_undoIndex;
  m_undoBuffer[backIndex].resetUnserializer();
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
  Time::setTickrate(tickrate);
}

uint32_t Vortex::getTickrate()
{
  return Time::getTickrate();
}

bool Vortex::doSave()
{
  return Modes::saveStorage() && addUndoBuffer();
}

void Vortex::handleInputQueue(Button *buttons, uint32_t numButtons)
{
  // if there's nothing in the queue just return
  if (!m_buttonEventQueue.size()) {
    return;
  }
  // pop the event from the front of the queue
  VortexButtonEvent buttonEvent = m_buttonEventQueue.front();
  m_buttonEventQueue.pop_front();
  // the target button for this event (no target if event is just 'wait')
  Button *pButton = buttons;
  if (buttonEvent.type != EVENT_WAIT && buttonEvent.type != EVENT_RAPID_CLICK) {
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
    pButton->m_pressTime = Time::getCurtime();
    pButton->m_holdDuration = SHORT_CLICK_THRESHOLD_TICKS - 1;
    DEBUG_LOG("Injecting short click");
    break;
  case EVENT_LONG_CLICK:
    pButton->m_newRelease = true;
    pButton->m_longClick = true;
    pButton->m_pressTime = Time::getCurtime();
    pButton->m_holdDuration = SHORT_CLICK_THRESHOLD_TICKS + 1;
    DEBUG_LOG("Injecting long click");
    break;
  case EVENT_MENU_ENTER_CLICK:
    // to do this we simply press the button and set the press time
    // to something more than the menu trigger threshold that will make
    // us immediately enter the menus. But we need to unset the pressed
    // button right after so we push a reset click event to reset the button
    pButton->m_pressTime = Time::getCurtime();
#ifdef SLEEP_ENTER_THRESHOLD_TICKS
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
    pButton->m_pressTime = Time::getCurtime();
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
    pButton->m_pressTime = Time::getCurtime();
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
    pButton->m_pressTime = Time::getCurtime();
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
    pButton->m_pressTime = Time::getCurtime();
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
      for (uint32_t i = 0; i < buttonEvent.target; ++i) {
        VortexEngine::tick();
      }
      // then restore the event queue so that events are processed like normal
      swap(backup, m_buttonEventQueue);
    }
    break;
  case EVENT_TOGGLE_CLICK:
    if (pButton->isPressed()) {
      // re-calc all this stuff because there's no api in Button class to do it
      // I don't want to add the api there because it's useless besides for this
      pButton->m_buttonState = false;
      pButton->m_isPressed = false;
      pButton->m_holdDuration = (uint32_t)(Time::getCurtime() - pButton->m_pressTime);
      pButton->m_releaseTime = Time::getCurtime();
      pButton->m_newRelease = true;
      pButton->m_shortClick = (pButton->m_holdDuration <= SHORT_CLICK_THRESHOLD_TICKS);
      pButton->m_longClick = !pButton->m_shortClick;
      DEBUG_LOG("Injecting release");
    } else {
      pButton->m_buttonState = true;
      pButton->m_isPressed = true;
      pButton->m_releaseDuration = (uint32_t)(Time::getCurtime() - pButton->m_releaseTime);
      pButton->m_pressTime = Time::getCurtime();
      pButton->m_newPress = true;
      DEBUG_LOG("Injecting press");
    }
    break;
  case EVENT_RAPID_CLICK:
    pButton->m_consecutivePresses = buttonEvent.target;
    pButton->m_newRelease = true;
    pButton->m_shortClick = true;
    pButton->m_pressTime = Time::getCurtime();
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
  Storage::setStorageFilename(name);
}

string Vortex::getStorageFilename()
{
  return Storage::getStorageFilename();
}
