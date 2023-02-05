#include "EngineWrapper.h"

// VortexEngine includes
#include "VortexEngine.h"
#include "Serial/ByteStream.h"
#include "Patterns/PatternBuilder.h"
#include "Patterns/Pattern.h"
#include "Colors/Colorset.h"
#include "Modes/Modes.h"
#include "Modes/Mode.h"

// for random()
#include "Arduino.h"

using namespace std;

// the undo buffer and data
deque<ByteStream> VEngine::m_undoBuffer;
uint32_t VEngine::m_undoLimit = 0;
uint32_t VEngine::m_undoIndex = 0;
bool VEngine::m_undoEnabled = true;

VEngine::VEngine()
{
}

void VEngine::init()
{
  // init the engine
  VortexEngine::init();
  // clear the modes
  //Modes::clearModes();
  // save and set undo buffer
  doSave();

  // check all custom params
  for (PatternID id = PATTERN_FIRST; id < PATTERN_COUNT; ++id) {
    vector<string> params = VEngine::getCustomParams(id);
    PatternArgs args = PatternBuilder::getDefaultArgs(id);
    if (params.size() != args.numArgs) {
      // TODO: Handle this more elegantly and just return false when
      //       VEngine is used a wrapper for exposing Vortex in a dll
      MessageBox(NULL, ("Update tooltips for pattern ID " + to_string(id)).c_str(), "FATAL ERROR", 0);
      PostQuitMessage(0);
    }
  }
}

void VEngine::cleanup()
{
}

void VEngine::getStorageStats(uint32_t *outTotal, uint32_t *outUsed)
{
  if (outTotal) {
    *outTotal = VortexEngine::totalStorageSpace();
  }
  if (outUsed) {
    *outUsed = VortexEngine::savefileSize();
  }
}

bool VEngine::getModes(ByteStream &outStream)
{
  // save to ensure we get the correct mode, not using doSave() because it causes
  // an undo buffer entry to be added
  Modes::saveStorage();
  // now serialize all the modes
  Modes::saveToBuffer(outStream);
  return true;
}

bool VEngine::setModes(ByteStream &stream, bool save)
{
  // now unserialize the stream of data that was read
  if (!Modes::loadFromBuffer(stream)) {
    //printf("Unserialize failed\n");
    return false;
  }
  return !save || doSave();
}

bool VEngine::getCurMode(ByteStream &outStream)
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
  return pMode->saveToBuffer(outStream);
}

uint32_t VEngine::curMode()
{
  return Modes::curModeIndex();
}

uint32_t VEngine::numModes()
{
  return Modes::numModes();
}

uint32_t VEngine::numLedsInMode()
{
  Mode *pMode = Modes::curMode();
  if (!pMode) {
    return false;
  }
  return pMode->getLedCount();
}

bool VEngine::addNewMode(bool save)
{
  Colorset set;
  set.randomize();
    // create a random pattern ID from all patterns
  PatternID randomPattern;
  do {
    // continuously re-randomize the pattern so we don't get solids
    randomPattern = (PatternID)random(PATTERN_FIRST, PATTERN_COUNT);
  } while (randomPattern == PATTERN_SOLID);
  if (!Modes::addMode(randomPattern, nullptr, &set)) {
    return false;
  }
  return !save || doSave();
}

bool VEngine::addNewMode(ByteStream &stream, bool save)
{
  if (!Modes::addModeFromBuffer(stream)) {
    return false;
  }
  return !save || doSave();
}

bool VEngine::setCurMode(uint32_t index, bool save)
{
  Modes::setCurMode(index);
  return !save || doSave();
}

bool VEngine::nextMode(bool save)
{
  Modes::nextMode();
  return !save || doSave();
}

bool VEngine::delCurMode(bool save)
{
  Modes::deleteCurMode();
  return !save || doSave();
}

bool VEngine::shiftCurMode(int8_t offset, bool save)
{
  if (offset == 0) {
    return true;
  }
  Modes::shiftCurMode(offset);
  return !save || doSave();
}

bool VEngine::setPattern(PatternID id, const PatternArgs *args, const Colorset *set, bool save)
{
  Mode *pMode = Modes::curMode();
  if (!pMode) {
    return false;
  }
  if (!pMode->setPattern(id, args, set)) {
    return false;
  }
  return !save || doSave();
}

PatternID VEngine::getPatternID(LedPos pos)
{
  Mode *pMode = Modes::curMode();
  if (!pMode) {
    return PATTERN_NONE;
  }
  return pMode->getPatternID(pos);
}

string VEngine::getPatternName(LedPos pos)
{
  return patternToString(getPatternID(pos));
}

string VEngine::getModeName()
{
  Mode *pMode = Modes::curMode();
  if (!pMode) {
    return patternToString(PATTERN_NONE);
  }
  if (pMode->isMultiLed()) {
    return patternToString(getPatternID(LED_FIRST));
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

bool VEngine::setSinglePat(LedPos pos, PatternID id,
  const PatternArgs *args, const Colorset *set, bool save)
{
  Mode *pMode = Modes::curMode();
  if (!pMode) {
    return false;
  }
  if (!pMode->setSinglePat(pos, id, args, set)) {
    return false;
  }
  return !save || doSave();
}

bool VEngine::getColorset(LedPos pos, Colorset &set)
{
  Mode *pMode = Modes::curMode();
  if (!pMode) {
    return false;
  }
  const Colorset *pSet = pMode->getColorset(pos);
  if (!pSet) {
    return false;
  }
  set = *pSet;
  return true;
}

bool VEngine::setColorset(LedPos pos, const Colorset &set, bool save)
{
  Mode *pMode = Modes::curMode();
  if (!pMode) {
    return false;
  }
  Pattern *pat = pMode->getPattern(pos);
  if (!pat) {
    return false;
  }
  pat->setColorset(&set);
  return !save || doSave();
}

bool VEngine::getPatternArgs(LedPos pos, PatternArgs &args)
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

bool VEngine::setPatternArgs(LedPos pos, PatternArgs &args, bool save)
{
  Mode *pMode = Modes::curMode();
  if (!pMode) {
    return false;
  }
  Pattern *pat = pMode->getPattern(pos);
  if (!pat) {
    return false;
  }
  pat->setArgs(args);
  // re-initialize the mode after changing pattern args
  pMode->init();
  // save the new params
  return !save || doSave();
}

string VEngine::patternToString(PatternID id)
{
  if (id == PATTERN_NONE || id >= PATTERN_COUNT) {
    return "pattern_none";
  }
  // This is awful but idk how else to do it for now
  static const char *patternNames[PATTERN_COUNT] = {
    "basic", "strobe", "hyperstrobe", "dops", "dopish", "ultradops", "strobie",
    "ribbon", "miniribbon", "blinkie", "ghostcrush", "solid", "tracer",
    "dashdops", "advanced", "blend", "complementary blend", "brackets",
    "rabbit", "hueshift", "theater chase", /*"chaser",*/ "zigzag", "zipfade",
    "tiptop", "drip", "dripmorph", "crossdops", "doublestrobe", "meteor",
    "sparkletrace", "vortexwipe", "warp", "warpworm", "snowball", "lighthouse",
    "pulsish", "fill", "bounce", "impact", "splitstrobie", "backstrobe",
    "flowers", "jest", "materia",
  };
  return patternNames[id];
}

// this shouldn't change much so this is fine
string VEngine::ledToString(LedPos pos)
{
  if (numLedsInMode() != 10 || pos >= 10) {
    return "led " + to_string(pos);
  }
  static const char *ledNames[10] = {
    // tips       tops
    "pinkie tip", "pinkie top",
    "ring tip",   "ring top",
    "middle tip", "middle top",
    "index tip",  "index top",
    "thumb tip",  "thumb top",
  };
  return ledNames[pos];
}

// the number of custom parameters for any given pattern id
uint32_t VEngine::numCustomParams(PatternID id)
{
  Pattern *pat = PatternBuilder::make(id);
  if (!pat) {
    return 0;
  }
  PatternArgs args;
  pat->getArgs(args);
  uint32_t numArgs = args.numArgs;
  delete pat;
  return numArgs;
}

vector<string> VEngine::getCustomParams(PatternID id)
{
  switch (id) {
  case PATTERN_BASIC:
  case PATTERN_STROBE:
    case PATTERN_HYPERSTROBE:
    case PATTERN_DOPS:
    case PATTERN_DOPISH:
    case PATTERN_ULTRADOPS:
    case PATTERN_STROBIE:
    case PATTERN_RIBBON:
    case PATTERN_MINIRIBBON:
    case PATTERN_BLINKIE:
    case PATTERN_GHOSTCRUSH:
      return { "On Duration", "Off Duration", "Gap Duration" };
    case PATTERN_SOLID:
      return { "On Duration", "Off Duration", "Gap Duration", "Color Index" };
    case PATTERN_TRACER:
      return { "Tracer Duration", "Dot Duration" };
    case PATTERN_DASHDOPS:
      return { "Dash Duration", "Dot Duration", "Off Duration" };
    case PATTERN_ADVANCED:
      return { "On Duration", "Off Duration", "Gap Duration", "Group Size", "Skip Colors", "Repeat Group" };
    case PATTERN_BLEND:
    case PATTERN_COMPLEMENTARY_BLEND:
      return { "On Duration", "Off Duration", "Gap Duration", "Start Offset" };
    case PATTERN_BRACKETS:
      return { "Bracket Duration", "Mid Duration", "Off Duration" };
    case PATTERN_RABBIT:
    case PATTERN_FLOWERS:
    case PATTERN_TIPTOP:
      return { "On Duration 1", "Off Duration 1", "Gap Duration 1",
        "On Duration 2", "Off Duration 2", "Gap Duration 2" };
    case PATTERN_IMPACT:
      return { "On Duration 1", "Off Duration 1", "On Duration 2",
        "Off Duration 2", "On Duration 3", "Off Duration 3" };
    case PATTERN_JEST:
      return { "On Duration" , "Off Duration", "Gap 1 Duration", "Gap 2 Duration", "Group Size" };
    case PATTERN_HUESHIFT:
      return { "Speed", "Scale" };
    case PATTERN_THEATER_CHASE:
      return { "On Duration", "Off Duration", "Step Duration" };
    case PATTERN_ZIGZAG:
    case PATTERN_ZIPFADE:
      return { "On Duration", "Off Duration", "Step Duration", "Snake Size", "Fade Amount" };
    case PATTERN_DRIP:
    case PATTERN_CROSSDOPS:
    case PATTERN_DOUBLESTROBE:
    case PATTERN_SPARKLETRACE:
    case PATTERN_VORTEXWIPE:
    case PATTERN_WARP:
    case PATTERN_WARPWORM:
    case PATTERN_SNOWBALL:
    case PATTERN_FILL:
      return { "On Duration", "Off Duration", "Step Duration" };
    case PATTERN_BOUNCE:
      return { "On Duration", "Off Duration", "Step Duration", "Fade Amount" };
    //case PATTERN_CHASER:
    case PATTERN_DRIPMORPH:
      return { "On Duration", "Off Duration", "Speed" };
    case PATTERN_METEOR:
      return { "On Duration", "Off Duration", "Step Duration", "Fade Amount" };
    case PATTERN_LIGHTHOUSE:
      return { "On Duration", "Off Duration", "Step Duration", "Fade Amount", "Fade Rate" };
    case PATTERN_PULSISH:
    case PATTERN_MATERIA:
      return { "On Duration 1", "Off Duration 1", "On Duration 2", "Off Duration 2", "Step Duration" };
    case PATTERN_SPLITSTROBIE:
      return { "On Duration", "Off Duration", "Gap Duration", "Dash Duration", 
        "Dot Duration", "Step Duration x 100ms" };
    case PATTERN_BACKSTROBE:
      return { "On Duration 1", "Off Duration 1", "Gap Duration 1",  "On Duration 2", "Off Duration 2",
        "Gap Duration 2", "Step Duration x 100ms" };
    case PATTERN_NONE:
    default:
      break;
  }
  return vector<string>();
}

void VEngine::setUndoBufferLimit(uint32_t limit)
{
  m_undoLimit = limit;
}

bool VEngine::addUndoBuffer()
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

bool VEngine::applyUndo()
{
  if (!m_undoBuffer.size()) {
    return false;
  }
  uint32_t highestIndex = m_undoBuffer.size() - 1;
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
  VEngine::setModes(m_undoBuffer[backIndex], false);
  return true;
}

bool VEngine::undo()
{
  if (!m_undoBuffer.size()) {
    return false;
  }
  uint32_t highestIndex = m_undoBuffer.size() - 1;
  // cannot undo further into history
  if (m_undoIndex > highestIndex) {
    m_undoIndex = highestIndex;
  } else {
    m_undoIndex++;
  }
  return applyUndo();
}

bool VEngine::redo()
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

bool VEngine::doSave()
{
  return Modes::saveStorage() && addUndoBuffer();
}
