#include "PatternBuilder.h"

#include "../Serial/ByteStream.h"
#include "../Time/TimeControl.h"
#include "../Patterns/Multi/Sequencer/Sequence.h"
#include "../Time/Timings.h"
#include "../Log/Log.h"

#include "Multi/Sequencer/SequencedPattern.h"
#include "Multi/Sequencer/ChaserPattern.h"

#include "Multi/TheaterChasePattern.h"
#include "Multi/HueShiftPattern.h"
#include "Multi/RabbitPattern.h"
#include "Multi/ZigzagPattern.h"
#include "Multi/TipTopPattern.h"
#include "Multi/DripPattern.h"
#include "Multi/DripMorphPattern.h"
#include "Multi/CrossDopsPattern.h"
#include "Multi/DoubleStrobePattern.h"
#include "Multi/MeteorPattern.h"
#include "Multi/SparkleTracePattern.h"
#include "Multi/VortexWipePattern.h"
#include "Multi/WarpPattern.h"
#include "Multi/WarpWormPattern.h"
#include "Multi/FillPattern.h"
#include "Multi/SnowballPattern.h"
#include "Multi/LighthousePattern.h"
#include "Multi/PulsishPattern.h"
#include "Multi/BouncePattern.h"
#include "Multi/ImpactPattern.h"
#include "Multi/SplitStrobiePattern.h"
#include "Multi/BackStrobePattern.h"
#include "Multi/FlowersPattern.h"
#include "Multi/JestPattern.h"
#include "Multi/MateriaPattern.h"

#include "Single/ComplementaryBlendPattern.h"
#include "Single/BracketsPattern.h"
#include "Single/AdvancedPattern.h"
#include "Single/TracerPattern.h"
#include "Single/DashDopsPattern.h"
#include "Single/SolidPattern.h"
#include "Single/BasicPattern.h"
#include "Single/BlendPattern.h"

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

Pattern *PatternBuilder::unserialize(ByteStream &buffer)
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
    case PATTERN_DASHDOPS: return new DashDopsPattern();
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
    case PATTERN_CHASER: return new ChaserPattern();
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
    case PATTERN_IMPACT:return new ImpactPattern();
    case PATTERN_SPLITSTROBIE:return new SplitStrobiePattern();
    case PATTERN_BACKSTROBE:return new BackStrobePattern();
    case PATTERN_FLOWERS:return new FlowersPattern();
    case PATTERN_JEST:return new JestPattern();
    case PATTERN_MATERIA:return new MateriaPattern();
    case PATTERN_NONE: return nullptr;
    default: break;

  }
  DEBUG_LOGF("Unknown pattern id: %u", id);
  return nullptr;
}
