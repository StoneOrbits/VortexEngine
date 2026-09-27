
#include "../../c_types.h"
void SingleLedPattern_init(SingleLedPattern *self, const PatternArgs *args)
{
  Pattern_init(&self->base, args);
}
