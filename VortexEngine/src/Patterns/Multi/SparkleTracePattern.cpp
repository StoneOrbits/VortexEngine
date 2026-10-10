#include "SparkleTracePattern.h"

#include <string.h>
#include "../../Time/TimeControl.h"
#include "../../Leds/Leds.h"
#include "../../Log/Log.h"
#include "../../Memory/Memory.h"
#include "../Pattern.h"

const BlinkStepVtable SparkleTracePattern_bvtable = {
  .base = {
    .destroy = SparkleTracePattern_destroy,
    .bind = MultiLedPattern_bind,
    .init = BlinkStepPattern_initImpl,
    .play = BlinkStepPattern_play
  },
  .blinkOn = SparkleTracePattern_blinkOn,
  .blinkOff = BlinkStepPattern_blinkOff,
  .prestep = BlinkStepPattern_prestep,
  .poststep = SparkleTracePattern_poststep,
};

void SparkleTracePattern_init(SparkleTracePattern *self, const PatternArgs *args)
{
  BlinkStepPattern_init(&self->base, args, &SparkleTracePattern_bvtable);
  Random_make(&self->m_randCtx);
  Pattern_setPatternID((&self->base.base.base), PATTERN_SPARKLETRACE);
  Pattern_setArgs((&self->base.base.base), args);
}

void SparkleTracePattern_destroy(Pattern *pat)
{
  SparkleTracePattern *self = (SparkleTracePattern *)pat;
  (void)self;
}

void SparkleTracePattern_blinkOn(BlinkStepPattern *bsp)
{
  SparkleTracePattern *self = (SparkleTracePattern *)bsp;
  Leds_setAll(Colorset_get(&(&self->base.base.base)->m_colorset, 0));
}

void SparkleTracePattern_poststep(BlinkStepPattern *bsp)
{
  SparkleTracePattern *self = (SparkleTracePattern *)bsp;
  for (uint8_t dot = 0; dot < 4; ++dot) {
    Leds_setPair((Pair)Random_next8(&self->m_randCtx, PAIR_FIRST, PAIR_LAST), Colorset_cur(&(&self->base.base.base)->m_colorset));
  }
  Colorset_skip(&(&self->base.base.base)->m_colorset, 1);
  if (Colorset_curIndex(&(&self->base.base.base)->m_colorset) == 0) {
    Colorset_skip(&(&self->base.base.base)->m_colorset, 1);
  }
}
