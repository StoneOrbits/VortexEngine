#ifndef PATTERN_BUILDER_H
#define PATTERN_BUILDER_H

#include "Patterns.h"
#include "Pattern.h"
#include "PatternArgs.h"
#include "Multi/MultiLedPattern.h"
#include "Single/SingleLedPattern.h"

typedef struct ByteStream ByteStream;

// allocate the pattern pool, all patterns are allocated from this pool
bool PatternBuilder_init(void);
// grab a block from the pattern pool to construct a pattern in
void *PatternBuilder_allocPattern(void);
// return a pattern's block to the pool, call Pattern_destroy first
void PatternBuilder_freePattern(Pattern *pat);

// generic make any pattern
Pattern *PatternBuilder_make(PatternID id, const PatternArgs *args);

// make a copy of an existing pattern
Pattern *PatternBuilder_dupe(const Pattern *pat);

// generate a single LED pattern (NULL if patternid is not single LED)
SingleLedPattern *PatternBuilder_makeSingle(PatternID id, const PatternArgs *args);

// generate a multi LED pattern (NULL if patternid is not multi LED)
MultiLedPattern *PatternBuilder_makeMulti(PatternID id, const PatternArgs *args);

// unserialize a buffer into a pattern
Pattern *PatternBuilder_unserialize(ByteStream *buffer);

// This is just the default arguments for any given pattern id
// it will *not* indicate the true amount of arguments a pattern has
void PatternBuilder_getDefaultArgs(PatternID id, PatternArgs *out);
// this will give you actual amount of default args
uint8_t PatternBuilder_numDefaultArgs(PatternID id);

// helper routines
Pattern *PatternBuilder_makeInternal(PatternID id, const PatternArgs *args);
Pattern *PatternBuilder_generate(PatternID id, const PatternArgs *args);

#endif
