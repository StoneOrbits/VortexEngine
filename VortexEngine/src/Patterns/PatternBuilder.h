#ifndef PATTERN_BUILDER_H
#define PATTERN_BUILDER_H

#include <inttypes.h>
#include <stdbool.h>

#include "Patterns.h"
#include "PatternArgs.h"

typedef struct ByteStream ByteStream;
typedef struct Pattern Pattern;

Pattern *PatternBuilder_make(PatternID id, const PatternArgs *args);
Pattern *PatternBuilder_dupe(const Pattern *pat);
Pattern *PatternBuilder_makeSingle(PatternID id, const PatternArgs *args);
Pattern *PatternBuilder_makeMulti(PatternID id, const PatternArgs *args);
Pattern *PatternBuilder_unserialize(ByteStream *buffer);
PatternArgs PatternBuilder_getDefaultArgs(PatternID id);
uint8_t PatternBuilder_numDefaultArgs(PatternID id);

#endif
