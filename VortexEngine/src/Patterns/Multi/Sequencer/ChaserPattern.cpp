#include "ChaserPattern.h"

// This controls the ratio of chaser dots to LED_COUNT. Default 1 chaser per 7 LEDs. Range: 1-LED_COUNT.
#define CHASER_RATIO 7

const PatternVtable ChaserPattern_vtable = {
  .destroy = SequencedPattern_destroy,
  .bind = MultiLedPattern_bind,
  .init = SequencedPattern_initImpl,
  .play = SequencedPattern_play,
};

// This pattern aims to be a demonstration of the sequencer.
// There are always many ways to implement a pattern, it's best
// to choose the method that is most suitable for the pattern.
void ChaserPattern_init(ChaserPattern *self, const PatternArgs *args)
{
  SequencedPattern_init(&self->base, args, &ChaserPattern_vtable);
  Pattern_setArgs((&self->base.base.base.base), args);
  uint32_t numChasers = LED_COUNT / CHASER_RATIO;
  if (!numChasers) {
    numChasers = 1;
  }
  for (uint8_t i = 0; i < (LED_COUNT / numChasers); ++i) {
    PatternMap patMap;
    PatternMap_makeWith(&patMap, PATTERN_DOPS, MAP_LED_ALL);
    for (uint8_t chaserCount = 0; chaserCount < numChasers; ++chaserCount) {
      PatternMap_setPatternAt(&patMap, PATTERN_SOLID, MAP_LED((i + (chaserCount * CHASER_RATIO)) % LED_COUNT));
    }
    Sequence_addStepWith(&self->base.m_sequence, 150, &patMap, NULL);
  }
}
