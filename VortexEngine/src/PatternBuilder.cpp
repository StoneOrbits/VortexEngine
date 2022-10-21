#include "PatternBuilder.h"

#include "SerialBuffer.h"
#include "TimeControl.h"
#include "Sequence.h"
#include "Timings.h"

#include "patterns/multi/TheaterChasePattern.h"
#include "patterns/multi/SequencedPattern.h"
#include "patterns/multi/HueShiftPattern.h"
#include "patterns/multi/RabbitPattern.h"
#include "patterns/multi/ZigzagPattern.h"
#include "patterns/multi/TipTopPattern.h"
#include "patterns/multi/DripPattern.h"
#include "patterns/multi/DripMorphPattern.h"
#include "patterns/multi/CrossDopsPattern.h"
#include "patterns/multi/DoubleStrobePattern.h"
#include "patterns/multi/MeteorPattern.h"
#include "patterns/multi/SparkleTracePattern.h"
#include "patterns/multi/VortexWipePattern.h"
#include "patterns/multi/WarpPattern.h"
#include "patterns/multi/WarpWormPattern.h"
#include "patterns/multi/FillPattern.h"
#include "patterns/multi/SnowballPattern.h"
#include "patterns/multi/LighthousePattern.h"
#include "patterns/multi/PulsishPattern.h"
#include "patterns/multi/BouncePattern.h"
#include "patterns/multi/SplitStrobiePattern.h"
#include "Patterns/multi/BackStrobePattern.h"
#include "patterns/multi/FlowersPattern.h"
#include "patterns/multi/JestPattern.h"

#include "patterns/single/ComplementaryBlendPattern.h"
#include "patterns/single/BracketsPattern.h"
#include "patterns/single/AdvancedPattern.h"
#include "patterns/single/TracerPattern.h"
#include "patterns/single/SolidPattern.h"
#include "patterns/single/BasicPattern.h"
#include "patterns/single/BlendPattern.h"

#include "Log.h"

Pattern *PatternBuilder::make(PatternID id)
{
  if (id > PATTERN_LAST) {
    DEBUG_LOGF("Invalid pattern id: %u", id);
    return nullptr;
  }
  if (isMultiLedPatternID(id)) {
    return makeMulti(id);
  }
  return makeSingle(id);
}

// generate a single LED pattern (nullptr if patternid is not single LED)
SingleLedPattern *PatternBuilder::makeSingle(PatternID id)
{
  if (!isSingleLedPatternID(id)) {
    return nullptr;
  }
  Pattern *pat = makeInternal(id);
  if (!pat) {
    return nullptr;
  }
  // don't set any flags on single pattersn
  return (SingleLedPattern *)pat;
}

// generate a multi LED pattern (nullptr if patternid is not multi LED)
MultiLedPattern *PatternBuilder::makeMulti(PatternID id)
{
  if (!isMultiLedPatternID(id)) {
    return nullptr;
  }
  Pattern *pat = makeInternal(id);
  if (!pat) {
    return nullptr;
  }
  return (MultiLedPattern *)pat;
}

Pattern *PatternBuilder::unserialize(SerialBuffer &buffer)
{
  Pattern *pat = make((PatternID)buffer.unserialize8());
  if (!pat) {
    return nullptr;
  }
  pat->unserialize(buffer);
  return pat;
}

Pattern *PatternBuilder::makeInternal(PatternID id)
{
  if (id == PATTERN_NONE) {
    // no error
    return nullptr;
  }
  Pattern *pat = generate(id);
  if (!pat) {
    ERROR_OUT_OF_MEMORY();
    return nullptr;
  }
  // set private pattern ID via friend class relationship
  pat->m_patternID = id;
  return pat;
}

Pattern *PatternBuilder::createChaser()
{
  Sequence chaserSequence;
  // there are 8 steps in the chaser
  for (uint32_t i = 0; i < 8; ++i) {
    // each step starts all fingers are dops
    PatternMap patMap(PATTERN_DOPS);
    // and one finger that moves back and forth is solid
    patMap.setPatternAt(PATTERN_SOLID0, MAP_FINGER((Finger)((i < 5) ? i : (8 - i))));
    // the step lasts for 300ms
    chaserSequence.addStep(300, patMap);
  }
  return new SequencedPattern(chaserSequence);
}

Pattern *PatternBuilder::generate(PatternID id)
{
  // NOTE: The timings of patterns are only defined at creation time
  //       which means altering the tickrate will not change how fast
  //       a pattern displays unless you re-create it
  switch (id) {

    // =====================
    //  Single Led Patterns:
    case PATTERN_STROBE: return new BasicPattern(STROBE_ON_DURATION, STROBE_OFF_DURATION);
    case PATTERN_HYPERSTROBE: return new BasicPattern(HYPERSTROBE_ON_DURATION, HYPERSTROBE_OFF_DURATION);
    case PATTERN_DOPS: return new BasicPattern(DOPS_ON_DURATION, DOPS_OFF_DURATION);
    case PATTERN_DOPISH: return new BasicPattern(DOPISH_ON_DURATION, DOPISH_OFF_DURATION);
    case PATTERN_ULTRADOPS: return new BasicPattern(ULTRADOPS_ON_DURATION, ULTRADOPS_OFF_DURATION);
    case PATTERN_STROBIE: return new BasicPattern(STROBIE_ON_DURATION, STROBE_OFF_DURATION);
    case PATTERN_RIBBON: return new BasicPattern(RIBBON_DURATION);
    case PATTERN_MINIRIBBON: return new BasicPattern(3);
    case PATTERN_TRACER: return new TracerPattern();
    case PATTERN_BLINKIE: return new BasicPattern(STROBE_ON_DURATION, STROBE_OFF_DURATION, 35);
    case PATTERN_GHOSTCRUSH: return new BasicPattern(1, 0, 50);
    case PATTERN_ADVANCED: return new AdvancedPattern(5, 5, 10, 2, 2, 1);
    case PATTERN_BLEND: return new BlendPattern();
    case PATTERN_COMPLEMENTARY_BLEND: return new ComplementaryBlendPattern();
    case PATTERN_BRACKETS: return new BracketsPattern();

    // Solid Single led patterns
    case PATTERN_SOLID0: return new SolidPattern(0, 100);
    case PATTERN_SOLID1: return new SolidPattern(1, 100);
    case PATTERN_SOLID2: return new SolidPattern(2, 100);

    // =====================
    //  Multi Led Patterns:
    case PATTERN_RABBIT: return new RabbitPattern();
    case PATTERN_HUESHIFT: return new HueShiftPattern();
    case PATTERN_THEATER_CHASE: return new TheaterChasePattern();
    case PATTERN_CHASER: return createChaser();
    case PATTERN_ZIGZAG: return new ZigzagPattern();
    case PATTERN_ZIPFADE: return new ZigzagPattern(DOPS_ON_DURATION, DOPS_OFF_DURATION, 100, 4);
    case PATTERN_TIPTOP: return new TipTopPattern();
    case PATTERN_DRIP: return new DripPattern();
    case PATTERN_DRIPMORPH: return new DripMorphPattern();
    case PATTERN_CROSSDOPS:return new CrossDopsPattern();
    case PATTERN_DOUBLESTROBE:return new DoubleStrobePattern();
    case PATTERN_METEOR:return new MeteorPattern();
    case PATTERN_SPARKLETRACE:return new SparkleTracePattern();
    case PATTERN_VORTEXWIPE:return new VortexWipePattern();
    case PATTERN_WARP:return new WarpPattern();
    case PATTERN_WARPWORM:return new WarpWormPattern();
    case PATTERN_SNOWBALL:return new SnowballPattern();
    case PATTERN_LIGHTHOUSE:return new LighthousePattern();
    case PATTERN_PULSISH:return new PulsishPattern();
    case PATTERN_FILL:return new FillPattern();
    case PATTERN_BOUNCE:return new BouncePattern();
    case PATTERN_SPLITSTROBIE:return new SplitStrobiePattern();
    case PATTERN_BACKSTROBE:return new BackStrobePattern();
    case PATTERN_FLOWERS:return new FlowersPattern();
    case PATTERN_JEST:return new JestPattern();
    case PATTERN_NONE: return nullptr;
    default: break;

  }
  DEBUG_LOGF("Unknown pattern id: %u", id);
  return nullptr;
}
