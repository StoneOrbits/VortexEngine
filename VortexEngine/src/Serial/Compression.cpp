#include "Compression.h"

// This is a heavily stripped down version of lz4

#include "../Memory/Memory.h"
#include "../VortexConfig.h"

#include <string.h>
#include <limits.h>
#include <stdint.h>

#include <stdint.h>
#include <stddef.h>
#include <limits.h>

#if VORTEX_SLIM == 0 && ENABLE_COMPRESSION == 1

#if (defined(__GNUC__) && (__GNUC__ >= 3)) || (defined(__INTEL_COMPILER) && (__INTEL_COMPILER >= 800)) || defined(__clang__)
#  define expect(expr,value)    (__builtin_expect ((expr),(value)) )
#else
#  define expect(expr,value)    (expr)
#endif

#ifndef likely
#define likely(expr)     expect((expr) != 0, 1)
#endif
#ifndef unlikely
#define unlikely(expr)   expect((expr) != 0, 0)
#endif

/* Should the alignment test prove unreliable, for some reason,
 * it can be disabled by setting LZ4_ALIGN_TEST to 0 */
#ifndef LZ4_ALIGN_TEST  /* can be externally provided */
# define LZ4_ALIGN_TEST 0
#endif

#define MINMATCH 4

#define WILDCOPYLENGTH 8
#define LASTLITERALS   5   /* see ../doc/lz4_Block_format.md#parsing-restrictions */
#define MFLIMIT       12   /* see ../doc/lz4_Block_format.md#parsing-restrictions */
#define MATCH_SAFEGUARD_DISTANCE  ((2*WILDCOPYLENGTH) - MINMATCH)   /* ensure it's possible to write 2 x wildcopyLength without overflowing output buffer */
#define FASTLOOP_SAFE_DISTANCE 64
static const int LZ4_minLength = (MFLIMIT + 1);

#define KB *(1 <<10)
#define MB *(1 <<20)
#define GB *(1U<<30)

#define LZ4_DISTANCE_ABSOLUTE_MAX 65535
#if (LZ4_DISTANCE_MAX > LZ4_DISTANCE_ABSOLUTE_MAX)   /* max supported by LZ4 format */
#  error "LZ4_DISTANCE_MAX is too big : must be <= 65535"
#endif

#define ML_BITS  4
#define ML_MASK  ((1U<<ML_BITS)-1)
#define RUN_BITS (8-ML_BITS)
#define RUN_MASK ((1U<<RUN_BITS)-1)


/*-************************************
*  Error detection
**************************************/
#if defined(LZ4_DEBUG) && (LZ4_DEBUG>=1)
#  include <assert.h>
#else
#  ifndef assert
#    define assert(condition) ((void)0)
#  endif
#endif

#define LZ4_STATIC_ASSERT(c)   { enum { LZ4_static_assert = 1/(int)(!!(c)) }; }   /* use after variable declarations */
#define DEBUGLOG(l, ...) {}    /* disabled */

static int LZ4_isAligned(const void *ptr, size_t alignment)
{
  return ((size_t)ptr & (alignment - 1)) == 0;
}

/*-************************************
*  Types
**************************************/
typedef  uint8_t BYTE;
typedef uint16_t U16;
typedef uint32_t U32;
typedef  int32_t S32;
typedef uint64_t U64;
typedef uintptr_t uptrval;

#if defined(__x86_64__)
typedef U64    reg_t;   /* 64-bits in x32 mode */
#else
typedef size_t reg_t;   /* 32-bits in x32 mode */
#endif

typedef enum
{
  notLimited = 0,
  limitedOutput = 1,
  fillOutput = 2
} limitedOutput_directive;


/*-************************************
*  Reading and writing into memory
**************************************/

/**
 * LZ4 relies on memcpy with a constant size being inlined. In freestanding
 * environments, the compiler can't assume the implementation of memcpy() is
 * standard compliant, so it can't apply its specialized memcpy() inlining
 * logic. When possible, use __builtin_memcpy() to tell the compiler to analyze
 * memcpy() as if it were standard compliant, so it can inline it in freestanding
 * environments. This is needed when decompressing the Linux Kernel, for example.
 */
#if defined(__GNUC__) && (__GNUC__ >= 4)
#define LZ4_memcpy(dst, src, size) __builtin_memcpy(dst, src, size)
#else
#define LZ4_memcpy(dst, src, size) memcpy(dst, src, size)
#endif

static unsigned LZ4_isLittleEndian(void)
{
  const union { U32 u; BYTE c[4]; } one = { 1 };   /* don't use static : performance detrimental */
  return one.c[0];
}

static U16 LZ4_read16(const void *memPtr)
{
  U16 val; LZ4_memcpy(&val, memPtr, sizeof(val)); return val;
}

static U32 LZ4_read32(const void *memPtr)
{
  U32 val; LZ4_memcpy(&val, memPtr, sizeof(val)); return val;
}

static reg_t LZ4_read_ARCH(const void *memPtr)
{
  reg_t val; LZ4_memcpy(&val, memPtr, sizeof(val)); return val;
}

static void LZ4_write16(void *memPtr, U16 value)
{
  LZ4_memcpy(memPtr, &value, sizeof(value));
}

static void LZ4_write32(void *memPtr, U32 value)
{
  LZ4_memcpy(memPtr, &value, sizeof(value));
}

static U16 LZ4_readLE16(const void *memPtr)
{
  if (LZ4_isLittleEndian()) {
    return LZ4_read16(memPtr);
  } else {
    const BYTE *p = (const BYTE *)memPtr;
    return (U16)((U16)p[0] + (p[1] << 8));
  }
}

static void LZ4_writeLE16(void *memPtr, U16 value)
{
  if (LZ4_isLittleEndian()) {
    LZ4_write16(memPtr, value);
  } else {
    BYTE *p = (BYTE *)memPtr;
    p[0] = (BYTE)value;
    p[1] = (BYTE)(value >> 8);
  }
}

/* customized variant of memcpy, which can overwrite up to 8 bytes beyond dstEnd */
void LZ4_wildCopy8(void *dstPtr, const void *srcPtr, void *dstEnd)
{
  BYTE *d = (BYTE *)dstPtr;
  const BYTE *s = (const BYTE *)srcPtr;
  BYTE *const e = (BYTE *)dstEnd;

  do { LZ4_memcpy(d, s, 8); d += 8; s += 8; } while (d < e);
}

static const unsigned inc32table[8] = { 0, 1, 2,  1,  0,  4, 4, 4 };
static const int      dec64table[8] = { 0, 0, 0, -1, -4,  1, 2, 3 };

void LZ4_memcpy_using_offset_base(BYTE *dstPtr, const BYTE *srcPtr, BYTE *dstEnd, const size_t offset)
{
  assert(srcPtr + offset == dstPtr);
  if (offset < 8) {
    LZ4_write32(dstPtr, 0);   /* silence an msan warning when offset==0 */
    dstPtr[0] = srcPtr[0];
    dstPtr[1] = srcPtr[1];
    dstPtr[2] = srcPtr[2];
    dstPtr[3] = srcPtr[3];
    srcPtr += inc32table[offset];
    LZ4_memcpy(dstPtr + 4, srcPtr, 4);
    srcPtr -= dec64table[offset];
    dstPtr += 8;
  } else {
    LZ4_memcpy(dstPtr, srcPtr, 8);
    dstPtr += 8;
    srcPtr += 8;
  }

  LZ4_wildCopy8(dstPtr, srcPtr, dstEnd);
}

/* customized variant of memcpy, which can overwrite up to 32 bytes beyond dstEnd
 * this version copies two times 16 bytes (instead of one time 32 bytes)
 * because it must be compatible with offsets >= 16. */
void LZ4_wildCopy32(void *dstPtr, const void *srcPtr, void *dstEnd)
{
  BYTE *d = (BYTE *)dstPtr;
  const BYTE *s = (const BYTE *)srcPtr;
  BYTE *const e = (BYTE *)dstEnd;

  do { LZ4_memcpy(d, s, 16); LZ4_memcpy(d + 16, s + 16, 16); d += 32; s += 32; } while (d < e);
}

/* LZ4_memcpy_using_offset()  presumes :
 * - dstEnd >= dstPtr + MINMATCH
 * - there is at least 8 bytes available to write after dstEnd */
void LZ4_memcpy_using_offset(BYTE *dstPtr, const BYTE *srcPtr, BYTE *dstEnd, const size_t offset)
{
  BYTE v[8];

  assert(dstEnd >= dstPtr + MINMATCH);

  switch (offset) {
  case 1:
    memset(v, *srcPtr, 8);
    break;
  case 2:
    LZ4_memcpy(v, srcPtr, 2);
    LZ4_memcpy(&v[2], srcPtr, 2);
    LZ4_memcpy(&v[4], v, 4);
    break;
  case 4:
    LZ4_memcpy(v, srcPtr, 4);
    LZ4_memcpy(&v[4], srcPtr, 4);
    break;
  default:
    LZ4_memcpy_using_offset_base(dstPtr, srcPtr, dstEnd, offset);
    return;
  }

  LZ4_memcpy(dstPtr, v, 8);
  dstPtr += 8;
  while (dstPtr < dstEnd) {
    LZ4_memcpy(dstPtr, v, 8);
    dstPtr += 8;
  }
}


/*-************************************
*  Common functions
**************************************/
static unsigned LZ4_NbCommonBytes(reg_t val)
{
  assert(val != 0);
  if (LZ4_isLittleEndian()) {
    if (sizeof(val) == 8) {
      /*-*************************************************************************************************
      * ARM64EC is a Microsoft-designed ARM64 ABI compatible with AMD64 applications on ARM64 Windows 11.
      * The ARM64EC ABI does not support AVX/AVX2/AVX512 instructions, nor their relevant intrinsics
      * including _tzcnt_u64. Therefore, we need to neuter the _tzcnt_u64 code path for ARM64EC.
      ****************************************************************************************************/
      const U64 m = 0x0101010101010101ULL;
      val ^= val - 1;
      return (unsigned)(((U64)((val & (m - 1)) * m)) >> 56);
    } else /* 32 bits */ {
      const U32 m = 0x01010101;
      return (unsigned)((((val - 1) ^ val) & (m - 1)) * m) >> 24;
    }
  } else   /* Big Endian CPU */ {
    if (sizeof(val) == 8) {
      /* this method doesn't consume memory space like the previous one,
       * but it contains several branches,
       * that may end up slowing execution */
      static const U32 by32 = sizeof(val) * 4;  /* 32 on 64 bits (goal), 16 on 32 bits.
      Just to avoid some static analyzer complaining about shift by 32 on 32-bits target.
      Note that this code path is never triggered in 32-bits mode. */
      unsigned r;
      if (!(val >> by32)) { r = 4; } else { r = 0; val >>= by32; }
      if (!(val >> 16)) { r += 2; val >>= 8; } else { val >>= 24; }
      r += (!val);
      return r;
    } else /* 32 bits */ {
      val >>= 8;
      val = ((((val + 0x00FFFF00) | 0x00FFFFFF) + val) |
        (val + 0x00FF0000)) >> 24;
      return (unsigned)val ^ 3;
    }
  }
}


#define STEPSIZE sizeof(reg_t)
unsigned LZ4_count(const BYTE *pIn, const BYTE *pMatch, const BYTE *pInLimit)
{
  const BYTE *const pStart = pIn;

  if (likely(pIn < pInLimit - (STEPSIZE - 1))) {
    reg_t const diff = LZ4_read_ARCH(pMatch) ^ LZ4_read_ARCH(pIn);
    if (!diff) {
      pIn += STEPSIZE; pMatch += STEPSIZE;
    } else {
      return LZ4_NbCommonBytes(diff);
    }
  }

  while (likely(pIn < pInLimit - (STEPSIZE - 1))) {
    reg_t const diff = LZ4_read_ARCH(pMatch) ^ LZ4_read_ARCH(pIn);
    if (!diff) { pIn += STEPSIZE; pMatch += STEPSIZE; continue; }
    pIn += LZ4_NbCommonBytes(diff);
    return (unsigned)(pIn - pStart);
  }

  if ((STEPSIZE == 8) && (pIn < (pInLimit - 3)) && (LZ4_read32(pMatch) == LZ4_read32(pIn))) { pIn += 4; pMatch += 4; }
  if ((pIn < (pInLimit - 1)) && (LZ4_read16(pMatch) == LZ4_read16(pIn))) { pIn += 2; pMatch += 2; }
  if ((pIn < pInLimit) && (*pMatch == *pIn)) pIn++;
  return (unsigned)(pIn - pStart);
}


/*-************************************
*  Local Constants
**************************************/
static const int LZ4_64Klimit = ((64 KB) + (MFLIMIT - 1));
static const U32 LZ4_skipTrigger = 6;  /* Increase this value ==> compression run slower on incompressible data */


/*-************************************
*  Local Structures and types
**************************************/
typedef enum { clearedTable = 0, byPtr, byU32, byU16 } tableType_t;

/**
 * This enum distinguishes several different modes of accessing previous
 * content in the stream.
 *
 * - noDict        : There is no preceding content.
 * - withPrefix64k : Table entries up to ctx->dictSize before the current blob
 *                   blob being compressed are valid and refer to the preceding
 *                   content (of length ctx->dictSize), which is available
 *                   contiguously preceding in memory the content currently
 *                   being compressed.
 * - usingExtDict  : Like withPrefix64k, but the preceding content is somewhere
 *                   else in memory, starting at ctx->dictionary with length
 *                   ctx->dictSize.
 * - usingDictCtx  : Everything concerning the preceding content is
 *                   in a separate context, pointed to by ctx->dictCtx.
 *                   ctx->dictionary, ctx->dictSize, and table entries
 *                   in the current context that refer to positions
 *                   preceding the beginning of the current compression are
 *                   ignored. Instead, ctx->dictCtx->dictionary and ctx->dictCtx
 *                   ->dictSize describe the location and size of the preceding
 *                   content, and matches are found by looking in the ctx
 *                   ->dictCtx->hashTable.
 */
typedef enum { noDict = 0, withPrefix64k, usingExtDict, usingDictCtx } dict_directive;
typedef enum { noDictIssue = 0, dictSmall } dictIssue_directive;


/*-************************************
*  Local Utils
**************************************/
int LZ4_compressBound(int isize) { return LZ4_COMPRESSBOUND(isize); }

/*-******************************
*  Compression functions
********************************/
U32 LZ4_hash4(U32 sequence, tableType_t const tableType)
{
  if (tableType == byU16)
    return ((sequence * 2654435761U) >> ((MINMATCH * 8) - (LZ4_HASHLOG + 1)));
  else
    return ((sequence * 2654435761U) >> ((MINMATCH * 8) - LZ4_HASHLOG));
}

U32 LZ4_hash5(U64 sequence, tableType_t const tableType)
{
  const U32 hashLog = (tableType == byU16) ? LZ4_HASHLOG + 1 : LZ4_HASHLOG;
  if (LZ4_isLittleEndian()) {
    const U64 prime5bytes = 889523592379ULL;
    return (U32)(((sequence << 24) * prime5bytes) >> (64 - hashLog));
  } else {
    const U64 prime8bytes = 11400714785074694791ULL;
    return (U32)(((sequence >> 24) * prime8bytes) >> (64 - hashLog));
  }
}

U32 LZ4_hashPosition(const void *const p, tableType_t const tableType)
{
  if ((sizeof(reg_t) == 8) && (tableType != byU16)) return LZ4_hash5(LZ4_read_ARCH(p), tableType);
  return LZ4_hash4(LZ4_read32(p), tableType);
}

void LZ4_clearHash(U32 h, void *tableBase, tableType_t const tableType)
{
  switch (tableType) {
  default: /* fallthrough */
  case clearedTable: { /* illegal! */ assert(0); return; }
  case byPtr: { const BYTE **hashTable = (const BYTE **)tableBase; hashTable[h] = NULL; return; }
  case byU32: { U32 *hashTable = (U32 *)tableBase; hashTable[h] = 0; return; }
  case byU16: { U16 *hashTable = (U16 *)tableBase; hashTable[h] = 0; return; }
  }
}

void LZ4_putIndexOnHash(U32 idx, U32 h, void *tableBase, tableType_t const tableType)
{
  switch (tableType) {
  default: /* fallthrough */
  case clearedTable: /* fallthrough */
  case byPtr: { /* illegal! */ assert(0); return; }
  case byU32: { U32 *hashTable = (U32 *)tableBase; hashTable[h] = idx; return; }
  case byU16: { U16 *hashTable = (U16 *)tableBase; assert(idx < 65536); hashTable[h] = (U16)idx; return; }
  }
}

void LZ4_putPositionOnHash(const BYTE *p, U32 h,
  void *tableBase, tableType_t const tableType,
  const BYTE *srcBase)
{
  switch (tableType) {
  case clearedTable: { /* illegal! */ assert(0); return; }
  case byPtr: { const BYTE **hashTable = (const BYTE **)tableBase; hashTable[h] = p; return; }
  case byU32: { U32 *hashTable = (U32 *)tableBase; hashTable[h] = (U32)(p - srcBase); return; }
  case byU16: { U16 *hashTable = (U16 *)tableBase; hashTable[h] = (U16)(p - srcBase); return; }
  }
}

void LZ4_putPosition(const BYTE *p, void *tableBase, tableType_t tableType, const BYTE *srcBase)
{
  U32 const h = LZ4_hashPosition(p, tableType);
  LZ4_putPositionOnHash(p, h, tableBase, tableType, srcBase);
}

/* LZ4_getIndexOnHash() :
 * Index of match position registered in hash table.
 * hash position must be calculated by using base+index, or dictBase+index.
 * Assumption 1 : only valid if tableType == byU32 or byU16.
 * Assumption 2 : h is presumed valid (within limits of hash table)
 */
U32 LZ4_getIndexOnHash(U32 h, const void *tableBase, tableType_t tableType)
{
  LZ4_STATIC_ASSERT(LZ4_MEMORY_USAGE > 2);
  if (tableType == byU32) {
    const U32 *const hashTable = (const U32 *)tableBase;
    assert(h < (1U << (LZ4_MEMORY_USAGE - 2)));
    return hashTable[h];
  }
  if (tableType == byU16) {
    const U16 *const hashTable = (const U16 *)tableBase;
    assert(h < (1U << (LZ4_MEMORY_USAGE - 1)));
    return hashTable[h];
  }
  assert(0); return 0;  /* forbidden case */
}

static const BYTE *LZ4_getPositionOnHash(U32 h, const void *tableBase, tableType_t tableType, const BYTE *srcBase)
{
  if (tableType == byPtr) { const BYTE *const *hashTable = (const BYTE *const *)tableBase; return hashTable[h]; }
  if (tableType == byU32) { const U32 *const hashTable = (const U32 *)tableBase; return hashTable[h] + srcBase; }
  { const U16 *const hashTable = (const U16 *)tableBase; return hashTable[h] + srcBase; }   /* default, to ensure a return */
}

const BYTE * LZ4_getPosition(const BYTE *p,
  const void *tableBase, tableType_t tableType,
  const BYTE *srcBase)
{
  U32 const h = LZ4_hashPosition(p, tableType);
  return LZ4_getPositionOnHash(h, tableBase, tableType, srcBase);
}

/** LZ4_compress_generic() :
 *  inlined, to ensure branches are decided at compilation time.
 *  Presumed already validated at this stage:
 *  - source != NULL
 *  - inputSize > 0
 */
int LZ4_compress_generic_validated(
  LZ4_stream_t_internal *const cctx,
  const char *const source,
  char *const dest,
  const int inputSize,
  int *inputConsumed, /* only written when outputDirective == fillOutput */
  const int maxOutputSize,
  const limitedOutput_directive outputDirective,
  const tableType_t tableType,
  const dict_directive dictDirective,
  const dictIssue_directive dictIssue,
  const int acceleration)
{
  int result;
  const BYTE *ip = (const BYTE *)source;

  U32 const startIndex = cctx->currentOffset;
  const BYTE *base = (const BYTE *)source - startIndex;
  const BYTE *lowLimit;

  const LZ4_stream_t_internal *dictCtx = (const LZ4_stream_t_internal *)cctx->dictCtx;
  const BYTE *const dictionary =
    dictDirective == usingDictCtx ? dictCtx->dictionary : cctx->dictionary;
  const U32 dictSize =
    dictDirective == usingDictCtx ? dictCtx->dictSize : cctx->dictSize;
  const U32 dictDelta = (dictDirective == usingDictCtx) ? startIndex - dictCtx->currentOffset : 0;   /* make indexes in dictCtx comparable with index in current context */

  int const maybe_extMem = (dictDirective == usingExtDict) || (dictDirective == usingDictCtx);
  U32 const prefixIdxLimit = startIndex - dictSize;   /* used when dictDirective == dictSmall */
  const BYTE *const dictEnd = dictionary ? dictionary + dictSize : dictionary;
  const BYTE *anchor = (const BYTE *)source;
  const BYTE *const iend = ip + inputSize;
  const BYTE *const mflimitPlusOne = iend - MFLIMIT + 1;
  const BYTE *const matchlimit = iend - LASTLITERALS;

  /* the dictCtx currentOffset is indexed on the start of the dictionary,
   * while a dictionary in the current context precedes the currentOffset */
  const BYTE *dictBase = (dictionary == NULL) ? NULL :
    (dictDirective == usingDictCtx) ?
    dictionary + dictSize - dictCtx->currentOffset :
    dictionary + dictSize - startIndex;

  BYTE *op = (BYTE *)dest;
  BYTE *const olimit = op + maxOutputSize;

  U32 offset = 0;
  U32 forwardH;

  DEBUGLOG(5, "LZ4_compress_generic_validated: srcSize=%i, tableType=%u", inputSize, tableType);
  assert(ip != NULL);
  /* If init conditions are not met, we don't have to mark stream
   * as having dirty context, since no action was taken yet */
  if (outputDirective == fillOutput && maxOutputSize < 1) { return 0; } /* Impossible to store anything */
  if ((tableType == byU16) && (inputSize >= LZ4_64Klimit)) { return 0; }  /* Size too large (not within 64K limit) */
  if (tableType == byPtr) assert(dictDirective == noDict);      /* only supported use case with byPtr */
  assert(acceleration >= 1);

  lowLimit = (const BYTE *)source - (dictDirective == withPrefix64k ? dictSize : 0);

  /* Update context state */
  if (dictDirective == usingDictCtx) {
    /* Subsequent linked blocks can't use the dictionary. */
    /* Instead, they use the block we just compressed. */
    cctx->dictCtx = NULL;
    cctx->dictSize = (U32)inputSize;
  } else {
    cctx->dictSize += (U32)inputSize;
  }
  cctx->currentOffset += (U32)inputSize;
  cctx->tableType = (U32)tableType;

  if (inputSize < LZ4_minLength) goto _last_literals;        /* Input too small, no compression (all literals) */

  /* First Byte */
  LZ4_putPosition(ip, cctx->hashTable, tableType, base);
  ip++; forwardH = LZ4_hashPosition(ip, tableType);

  /* Main Loop */
  for (; ; ) {
    const BYTE *match;
    BYTE *token;
    const BYTE *filledIp;

    /* Find a match */
    if (tableType == byPtr) {
      const BYTE *forwardIp = ip;
      int step = 1;
      int searchMatchNb = acceleration << LZ4_skipTrigger;
      do {
        U32 const h = forwardH;
        ip = forwardIp;
        forwardIp += step;
        step = (searchMatchNb++ >> LZ4_skipTrigger);

        if (unlikely(forwardIp > mflimitPlusOne)) goto _last_literals;
        assert(ip < mflimitPlusOne);

        match = LZ4_getPositionOnHash(h, cctx->hashTable, tableType, base);
        forwardH = LZ4_hashPosition(forwardIp, tableType);
        LZ4_putPositionOnHash(ip, h, cctx->hashTable, tableType, base);

      } while ((match + LZ4_DISTANCE_MAX < ip)
        || (LZ4_read32(match) != LZ4_read32(ip)));

    } else {   /* byU32, byU16 */

      const BYTE *forwardIp = ip;
      int step = 1;
      int searchMatchNb = acceleration << LZ4_skipTrigger;
      do {
        U32 const h = forwardH;
        U32 const current = (U32)(forwardIp - base);
        U32 matchIndex = LZ4_getIndexOnHash(h, cctx->hashTable, tableType);
        assert(matchIndex <= current);
        assert(forwardIp - base < (ptrdiff_t)(2 GB - 1));
        ip = forwardIp;
        forwardIp += step;
        step = (searchMatchNb++ >> LZ4_skipTrigger);

        if (unlikely(forwardIp > mflimitPlusOne)) goto _last_literals;
        assert(ip < mflimitPlusOne);

        if (dictDirective == usingDictCtx) {
          if (matchIndex < startIndex) {
            /* there was no match, try the dictionary */
            assert(tableType == byU32);
            matchIndex = LZ4_getIndexOnHash(h, dictCtx->hashTable, byU32);
            match = dictBase + matchIndex;
            matchIndex += dictDelta;   /* make dictCtx index comparable with current context */
            lowLimit = dictionary;
          } else {
            match = base + matchIndex;
            lowLimit = (const BYTE *)source;
          }
        } else if (dictDirective == usingExtDict) {
          if (matchIndex < startIndex) {
            DEBUGLOG(7, "extDict candidate: matchIndex=%5u  <  startIndex=%5u", matchIndex, startIndex);
            assert(startIndex - matchIndex >= MINMATCH);
            assert(dictBase);
            match = dictBase + matchIndex;
            lowLimit = dictionary;
          } else {
            match = base + matchIndex;
            lowLimit = (const BYTE *)source;
          }
        } else {   /* single continuous memory segment */
          match = base + matchIndex;
        }
        forwardH = LZ4_hashPosition(forwardIp, tableType);
        LZ4_putIndexOnHash(current, h, cctx->hashTable, tableType);

        DEBUGLOG(7, "candidate at pos=%u  (offset=%u \n", matchIndex, current - matchIndex);
        if ((dictIssue == dictSmall) && (matchIndex < prefixIdxLimit)) { continue; }    /* match outside of valid area */
        assert(matchIndex < current);
        if (((tableType != byU16) || (LZ4_DISTANCE_MAX < LZ4_DISTANCE_ABSOLUTE_MAX))
          && (matchIndex + LZ4_DISTANCE_MAX < current)) {
          continue;
        } /* too far */
        assert((current - matchIndex) <= LZ4_DISTANCE_MAX);  /* match now expected within distance */

        if (LZ4_read32(match) == LZ4_read32(ip)) {
          if (maybe_extMem) offset = current - matchIndex;
          break;   /* match found */
        }

      } while (1);
    }

    /* Catch up */
    filledIp = ip;
    while (((ip > anchor) & (match > lowLimit)) && (unlikely(ip[-1] == match[-1]))) { ip--; match--; }

    /* Encode Literals */
    {
      unsigned const litLength = (unsigned)(ip - anchor);
      token = op++;
      if ((outputDirective == limitedOutput) &&  /* Check output buffer overflow */
        (unlikely(op + litLength + (2 + 1 + LASTLITERALS) + (litLength / 255) > olimit))) {
        return 0;   /* cannot compress within `dst` budget. Stored indexes in hash table are nonetheless fine */
      }
      if ((outputDirective == fillOutput) &&
        (unlikely(op + (litLength + 240) / 255 /* litlen */ + litLength /* literals */ + 2 /* offset */ + 1 /* token */ + MFLIMIT - MINMATCH /* min last literals so last match is <= end - MFLIMIT */ > olimit))) {
        op--;
        goto _last_literals;
      }
      if (litLength >= RUN_MASK) {
        int len = (int)(litLength - RUN_MASK);
        *token = (RUN_MASK << ML_BITS);
        for (; len >= 255; len -= 255) *op++ = 255;
        *op++ = (BYTE)len;
      } else *token = (BYTE)(litLength << ML_BITS);

      /* Copy Literals */
      LZ4_wildCopy8(op, anchor, op + litLength);
      op += litLength;
      DEBUGLOG(6, "seq.start:%i, literals=%u, match.start:%i",
        (int)(anchor - (const BYTE *)source), litLength, (int)(ip - (const BYTE *)source));
    }

  _next_match:
    /* at this stage, the following variables must be correctly set :
     * - ip : at start of LZ operation
     * - match : at start of previous pattern occurrence; can be within current prefix, or within extDict
     * - offset : if maybe_ext_memSegment==1 (constant)
     * - lowLimit : must be == dictionary to mean "match is within extDict"; must be == source otherwise
     * - token and *token : position to write 4-bits for match length; higher 4-bits for literal length supposed already written
     */

    if ((outputDirective == fillOutput) &&
      (op + 2 /* offset */ + 1 /* token */ + MFLIMIT - MINMATCH /* min last literals so last match is <= end - MFLIMIT */ > olimit)) {
      /* the match was too close to the end, rewind and go to last literals */
      op = token;
      goto _last_literals;
    }

    /* Encode Offset */
    if (maybe_extMem) {   /* static test */
      DEBUGLOG(6, "             with offset=%u  (ext if > %i)", offset, (int)(ip - (const BYTE *)source));
      assert(offset <= LZ4_DISTANCE_MAX && offset > 0);
      LZ4_writeLE16(op, (U16)offset); op += 2;
    } else {
      DEBUGLOG(6, "             with offset=%u  (same segment)", (U32)(ip - match));
      assert(ip - match <= LZ4_DISTANCE_MAX);
      LZ4_writeLE16(op, (U16)(ip - match)); op += 2;
    }

    /* Encode MatchLength */
    {
      unsigned matchCode;

      if ((dictDirective == usingExtDict || dictDirective == usingDictCtx)
        && (lowLimit == dictionary) /* match within extDict */) {
        const BYTE *limit = ip + (dictEnd - match);
        assert(dictEnd > match);
        if (limit > matchlimit) limit = matchlimit;
        matchCode = LZ4_count(ip + MINMATCH, match + MINMATCH, limit);
        ip += (size_t)matchCode + MINMATCH;
        if (ip == limit) {
          unsigned const more = LZ4_count(limit, (const BYTE *)source, matchlimit);
          matchCode += more;
          ip += more;
        }
        DEBUGLOG(6, "             with matchLength=%u starting in extDict", matchCode + MINMATCH);
      } else {
        matchCode = LZ4_count(ip + MINMATCH, match + MINMATCH, matchlimit);
        ip += (size_t)matchCode + MINMATCH;
        DEBUGLOG(6, "             with matchLength=%u", matchCode + MINMATCH);
      }

      if ((outputDirective) &&    /* Check output buffer overflow */
        (unlikely(op + (1 + LASTLITERALS) + (matchCode + 240) / 255 > olimit))) {
        if (outputDirective == fillOutput) {
          /* Match description too long : reduce it */
          U32 newMatchCode = 15 /* in token */ - 1 /* to avoid needing a zero byte */ + ((U32)(olimit - op) - 1 - LASTLITERALS) * 255;
          ip -= matchCode - newMatchCode;
          assert(newMatchCode < matchCode);
          matchCode = newMatchCode;
          if (unlikely(ip <= filledIp)) {
            /* We have already filled up to filledIp so if ip ends up less than filledIp
             * we have positions in the hash table beyond the current position. This is
             * a problem if we reuse the hash table. So we have to remove these positions
             * from the hash table.
             */
            const BYTE *ptr;
            DEBUGLOG(5, "Clearing %u positions", (U32)(filledIp - ip));
            for (ptr = ip; ptr <= filledIp; ++ptr) {
              U32 const h = LZ4_hashPosition(ptr, tableType);
              LZ4_clearHash(h, cctx->hashTable, tableType);
            }
          }
        } else {
          assert(outputDirective == limitedOutput);
          return 0;   /* cannot compress within `dst` budget. Stored indexes in hash table are nonetheless fine */
        }
      }
      if (matchCode >= ML_MASK) {
        *token += ML_MASK;
        matchCode -= ML_MASK;
        LZ4_write32(op, 0xFFFFFFFF);
        while (matchCode >= 4 * 255) {
          op += 4;
          LZ4_write32(op, 0xFFFFFFFF);
          matchCode -= 4 * 255;
        }
        op += matchCode / 255;
        *op++ = (BYTE)(matchCode % 255);
      } else
        *token += (BYTE)(matchCode);
    }
    /* Ensure we have enough space for the last literals. */
    assert(!(outputDirective == fillOutput && op + 1 + LASTLITERALS > olimit));

    anchor = ip;

    /* Test end of chunk */
    if (ip >= mflimitPlusOne) break;

    /* Fill table */
    LZ4_putPosition(ip - 2, cctx->hashTable, tableType, base);

    /* Test next position */
    if (tableType == byPtr) {

      match = LZ4_getPosition(ip, cctx->hashTable, tableType, base);
      LZ4_putPosition(ip, cctx->hashTable, tableType, base);
      if ((match + LZ4_DISTANCE_MAX >= ip)
        && (LZ4_read32(match) == LZ4_read32(ip))) {
        token = op++; *token = 0; goto _next_match;
      }

    } else {   /* byU32, byU16 */

      U32 const h = LZ4_hashPosition(ip, tableType);
      U32 const current = (U32)(ip - base);
      U32 matchIndex = LZ4_getIndexOnHash(h, cctx->hashTable, tableType);
      assert(matchIndex < current);
      if (dictDirective == usingDictCtx) {
        if (matchIndex < startIndex) {
          /* there was no match, try the dictionary */
          matchIndex = LZ4_getIndexOnHash(h, dictCtx->hashTable, byU32);
          match = dictBase + matchIndex;
          lowLimit = dictionary;   /* required for match length counter */
          matchIndex += dictDelta;
        } else {
          match = base + matchIndex;
          lowLimit = (const BYTE *)source;  /* required for match length counter */
        }
      } else if (dictDirective == usingExtDict) {
        if (matchIndex < startIndex) {
          assert(dictBase);
          match = dictBase + matchIndex;
          lowLimit = dictionary;   /* required for match length counter */
        } else {
          match = base + matchIndex;
          lowLimit = (const BYTE *)source;   /* required for match length counter */
        }
      } else {   /* single memory segment */
        match = base + matchIndex;
      }
      LZ4_putIndexOnHash(current, h, cctx->hashTable, tableType);
      assert(matchIndex < current);
      if (((dictIssue == dictSmall) ? (matchIndex >= prefixIdxLimit) : 1)
        && (((tableType == byU16) && (LZ4_DISTANCE_MAX == LZ4_DISTANCE_ABSOLUTE_MAX)) ? 1 : (matchIndex + LZ4_DISTANCE_MAX >= current))
        && (LZ4_read32(match) == LZ4_read32(ip))) {
        token = op++;
        *token = 0;
        if (maybe_extMem) offset = current - matchIndex;
        DEBUGLOG(6, "seq.start:%i, literals=%u, match.start:%i",
          (int)(anchor - (const BYTE *)source), 0, (int)(ip - (const BYTE *)source));
        goto _next_match;
      }
    }

    /* Prepare next loop */
    forwardH = LZ4_hashPosition(++ip, tableType);

  }

_last_literals:
  /* Encode Last Literals */
  {
    size_t lastRun = (size_t)(iend - anchor);
    if ((outputDirective) &&  /* Check output buffer overflow */
      (op + lastRun + 1 + ((lastRun + 255 - RUN_MASK) / 255) > olimit)) {
      if (outputDirective == fillOutput) {
        /* adapt lastRun to fill 'dst' */
        assert(olimit >= op);
        lastRun = (size_t)(olimit - op) - 1/*token*/;
        lastRun -= (lastRun + 256 - RUN_MASK) / 256;  /*additional length tokens*/
      } else {
        assert(outputDirective == limitedOutput);
        return 0;   /* cannot compress within `dst` budget. Stored indexes in hash table are nonetheless fine */
      }
    }
    DEBUGLOG(6, "Final literal run : %i literals", (int)lastRun);
    if (lastRun >= RUN_MASK) {
      size_t accumulator = lastRun - RUN_MASK;
      *op++ = RUN_MASK << ML_BITS;
      for (; accumulator >= 255; accumulator -= 255) *op++ = 255;
      *op++ = (BYTE)accumulator;
    } else {
      *op++ = (BYTE)(lastRun << ML_BITS);
    }
    LZ4_memcpy(op, anchor, lastRun);
    ip = anchor + lastRun;
    op += lastRun;
  }

  if (outputDirective == fillOutput) {
    *inputConsumed = (int)(((const char *)ip) - source);
  }
  result = (int)(((char *)op) - dest);
  assert(result > 0);
  DEBUGLOG(5, "LZ4_compress_generic: compressed %i bytes into %i bytes", inputSize, result);
  return result;
}

/** LZ4_compress_generic() :
 *  inlined, to ensure branches are decided at compilation time;
 *  takes care of src == (NULL, 0)
 *  and forward the rest to LZ4_compress_generic_validated */
int LZ4_compress_generic(
  LZ4_stream_t_internal *const cctx,
  const char *const src,
  char *const dst,
  const int srcSize,
  int *inputConsumed, /* only written when outputDirective == fillOutput */
  const int dstCapacity,
  const limitedOutput_directive outputDirective,
  const tableType_t tableType,
  const dict_directive dictDirective,
  const dictIssue_directive dictIssue,
  const int acceleration)
{
  DEBUGLOG(5, "LZ4_compress_generic: srcSize=%i, dstCapacity=%i",
    srcSize, dstCapacity);

  if ((U32)srcSize > (U32)LZ4_MAX_INPUT_SIZE) { return 0; }  /* Unsupported srcSize, too large (or negative) */
  if (srcSize == 0) {   /* src == NULL supported if srcSize == 0 */
    if (outputDirective != notLimited && dstCapacity <= 0) return 0;  /* no output, can't write anything */
    DEBUGLOG(5, "Generating an empty block");
    assert(outputDirective == notLimited || dstCapacity >= 1);
    assert(dst != NULL);
    dst[0] = 0;
    if (outputDirective == fillOutput) {
      assert(inputConsumed != NULL);
      *inputConsumed = 0;
    }
    return 1;
  }
  assert(src != NULL);

  return LZ4_compress_generic_validated(cctx, src, dst, srcSize,
    inputConsumed, /* only written into if outputDirective == fillOutput */
    dstCapacity, outputDirective,
    tableType, dictDirective, dictIssue, acceleration);
}

int LZ4_compress_fast_extState(void *state, const char *source, char *dest, int inputSize, int maxOutputSize, int acceleration)
{
  LZ4_stream_t_internal *const ctx = &LZ4_initStream(state, sizeof(LZ4_stream_t))->internal_donotuse;
  assert(ctx != NULL);
  if (acceleration < 1) acceleration = 1;
  if (acceleration > 65537) acceleration = 65537;
  if (maxOutputSize >= LZ4_compressBound(inputSize)) {
    if (inputSize < LZ4_64Klimit) {
      return LZ4_compress_generic(ctx, source, dest, inputSize, NULL, 0, notLimited, byU16, noDict, noDictIssue, acceleration);
    } else {
      const tableType_t tableType = ((sizeof(void *) == 4) && ((uptrval)source > LZ4_DISTANCE_MAX)) ? byPtr : byU32;
      return LZ4_compress_generic(ctx, source, dest, inputSize, NULL, 0, notLimited, tableType, noDict, noDictIssue, acceleration);
    }
  } else {
    if (inputSize < LZ4_64Klimit) {
      return LZ4_compress_generic(ctx, source, dest, inputSize, NULL, maxOutputSize, limitedOutput, byU16, noDict, noDictIssue, acceleration);
    } else {
      const tableType_t tableType = ((sizeof(void *) == 4) && ((uptrval)source > LZ4_DISTANCE_MAX)) ? byPtr : byU32;
      return LZ4_compress_generic(ctx, source, dest, inputSize, NULL, maxOutputSize, limitedOutput, tableType, noDict, noDictIssue, acceleration);
    }
  }
}

int LZ4_compress_fast(const char *source, char *dest, int inputSize, int maxOutputSize, int acceleration)
{
  int result;
  LZ4_stream_t *ctxPtr = (LZ4_stream_t *)vmalloc(sizeof(LZ4_stream_t));   /* malloc-calloc always properly aligned */
  if (ctxPtr == NULL) return 0;
  result = LZ4_compress_fast_extState(ctxPtr, source, dest, inputSize, maxOutputSize, acceleration);
  vfree(ctxPtr);
  return result;
}

int LZ4_compress_default(const char *src, char *dst, int srcSize, int maxOutputSize)
{
  return LZ4_compress_fast(src, dst, srcSize, maxOutputSize, 1);
}

/*-******************************
*  Streaming functions
********************************/

static size_t LZ4_stream_t_alignment(void)
{
#if LZ4_ALIGN_TEST
  typedef struct { char c; LZ4_stream_t t; } t_a;
  return sizeof(t_a) - sizeof(LZ4_stream_t);
#else
  return 1;  /* effectively disabled */
#endif
}

LZ4_stream_t *LZ4_initStream(void *buffer, size_t size)
{
  DEBUGLOG(5, "LZ4_initStream");
  if (buffer == NULL) { return NULL; }
  if (size < sizeof(LZ4_stream_t)) { return NULL; }
  if (!LZ4_isAligned(buffer, LZ4_stream_t_alignment())) return NULL;
  memset(buffer, 0, sizeof(LZ4_stream_t_internal));
  return (LZ4_stream_t *)buffer;
}


/*-*******************************
 *  Decompression functions
 ********************************/

typedef enum { endOnOutputSize = 0, endOnInputSize = 1 } endCondition_directive;
typedef enum { decode_full_block = 0, partial_decode = 1 } earlyEnd_directive;

#undef MIN
#define MIN(a,b)    ( (a) < (b) ? (a) : (b) )

/* Read the variable-length literal or match length.
 *
 * ip - pointer to use as input.
 * lencheck - end ip.  Return an error if ip advances >= lencheck.
 * loop_check - check ip >= lencheck in body of loop.  Returns loop_error if so.
 * initial_check - check ip >= lencheck before start of loop.  Returns initial_error if so.
 * error (output) - error code.  Should be set to 0 before call.
 */
typedef enum { loop_error = -2, initial_error = -1, ok = 0 } variable_length_error;
unsigned read_variable_length(const BYTE **ip, const BYTE *lencheck,
  int loop_check, int initial_check,
  variable_length_error *error)
{
  U32 length = 0;
  U32 s;
  if (initial_check && unlikely((*ip) >= lencheck)) {    /* overflow detection */
    *error = initial_error;
    return length;
  }
  do {
    s = **ip;
    (*ip)++;
    length += s;
    if (loop_check && unlikely((*ip) >= lencheck)) {    /* overflow detection */
      *error = loop_error;
      return length;
    }
  } while (s == 255);

  return length;
}

/*! LZ4_decompress_generic() :
 *  This generic decompression function covers all use cases.
 *  It shall be instantiated several times, using different sets of directives.
 *  Note that it is important for performance that this function really get inlined,
 *  in order to remove useless branches during compilation optimization.
 */
int LZ4_decompress_generic(
  const char *const src,
  char *const dst,
  int srcSize,
  int outputSize,         /* If endOnInput==endOnInputSize, this value is `dstCapacity` */

  endCondition_directive endOnInput,   /* endOnOutputSize, endOnInputSize */
  earlyEnd_directive partialDecoding,  /* full, partial */
  dict_directive dict,                 /* noDict, withPrefix64k, usingExtDict */
  const BYTE *const lowPrefix,  /* always <= dst, == dst when no prefix */
  const BYTE *const dictStart,  /* only if dict==usingExtDict */
  const size_t dictSize         /* note : = 0 if noDict */
)
{
  if ((src == NULL) || (outputSize < 0)) { return -1; }

  {
    const BYTE *ip = (const BYTE *)src;
    const BYTE *const iend = ip + srcSize;

    BYTE *op = (BYTE *)dst;
    BYTE *const oend = op + outputSize;
    BYTE *cpy;

    const BYTE *const dictEnd = (dictStart == NULL) ? NULL : dictStart + dictSize;

    const int safeDecode = (endOnInput == endOnInputSize);
    const int checkOffset = ((safeDecode) && (dictSize < (int)(64 KB)));


    /* Set up the "end" pointers for the shortcut. */
    const BYTE *const shortiend = iend - (endOnInput ? 14 : 8) /*maxLL*/ - 2 /*offset*/;
    const BYTE *const shortoend = oend - (endOnInput ? 14 : 8) /*maxLL*/ - 18 /*maxML*/;

    const BYTE *match;
    size_t offset;
    unsigned token;
    size_t length;


    DEBUGLOG(5, "LZ4_decompress_generic (srcSize:%i, dstSize:%i)", srcSize, outputSize);

    /* Special cases */
    assert(lowPrefix <= op);
    if ((endOnInput) && (unlikely(outputSize == 0))) {
      /* Empty output buffer */
      if (partialDecoding) return 0;
      return ((srcSize == 1) && (*ip == 0)) ? 0 : -1;
    }
    if ((!endOnInput) && (unlikely(outputSize == 0))) { return (*ip == 0 ? 1 : -1); }
    if ((endOnInput) && unlikely(srcSize == 0)) { return -1; }

    /* Currently the fast loop shows a regression on qualcomm arm chips. */
    if ((oend - op) < FASTLOOP_SAFE_DISTANCE) {
      DEBUGLOG(6, "skip fast decode loop");
      goto safe_decode;
    }

    /* Fast loop : decode sequences as long as output < iend-FASTLOOP_SAFE_DISTANCE */
    while (1) {
      /* Main fastloop assertion: We can always wildcopy FASTLOOP_SAFE_DISTANCE */
      assert(oend - op >= FASTLOOP_SAFE_DISTANCE);
      if (endOnInput) { assert(ip < iend); }
      token = *ip++;
      length = token >> ML_BITS;  /* literal length */

      assert(!endOnInput || ip <= iend); /* ip < iend before the increment */

      /* decode literal length */
      if (length == RUN_MASK) {
        variable_length_error error = ok;
        length += read_variable_length(&ip, iend - RUN_MASK, (int)endOnInput, (int)endOnInput, &error);
        if (error == initial_error) { goto _output_error; }
        if ((safeDecode) && unlikely((uptrval)(op)+length < (uptrval)(op))) { goto _output_error; } /* overflow detection */
        if ((safeDecode) && unlikely((uptrval)(ip)+length < (uptrval)(ip))) { goto _output_error; } /* overflow detection */

        /* copy literals */
        cpy = op + length;
        LZ4_STATIC_ASSERT(MFLIMIT >= WILDCOPYLENGTH);
        if (endOnInput) {  /* LZ4_decompress_safe() */
          if ((cpy > oend - 32) || (ip + length > iend - 32)) { goto safe_literal_copy; }
          LZ4_wildCopy32(op, ip, cpy);
        } else {   /* LZ4_decompress_fast() */
          if (cpy > oend - 8) { goto safe_literal_copy; }
          LZ4_wildCopy8(op, ip, cpy); /* LZ4_decompress_fast() cannot copy more than 8 bytes at a time :
                                       * it doesn't know input length, and only relies on end-of-block properties */
        }
        ip += length; op = cpy;
      } else {
        cpy = op + length;
        if (endOnInput) {  /* LZ4_decompress_safe() */
          DEBUGLOG(7, "copy %u bytes in a 16-bytes stripe", (unsigned)length);
          /* We don't need to check oend, since we check it once for each loop below */
          if (ip > iend - (16 + 1/*max lit + offset + nextToken*/)) { goto safe_literal_copy; }
          /* Literals can only be 14, but hope compilers optimize if we copy by a register size */
          LZ4_memcpy(op, ip, 16);
        } else {  /* LZ4_decompress_fast() */
            /* LZ4_decompress_fast() cannot copy more than 8 bytes at a time :
             * it doesn't know input length, and relies on end-of-block properties */
          LZ4_memcpy(op, ip, 8);
          if (length > 8) { LZ4_memcpy(op + 8, ip + 8, 8); }
        }
        ip += length; op = cpy;
      }

      /* get offset */
      offset = LZ4_readLE16(ip); ip += 2;
      match = op - offset;
      assert(match <= op);

      /* get matchlength */
      length = token & ML_MASK;

      if (length == ML_MASK) {
        variable_length_error error = ok;
        if ((checkOffset) && (unlikely(match + dictSize < lowPrefix))) { goto _output_error; } /* Error : offset outside buffers */
        length += read_variable_length(&ip, iend - LASTLITERALS + 1, (int)endOnInput, 0, &error);
        if (error != ok) { goto _output_error; }
        if ((safeDecode) && unlikely((uptrval)(op)+length < (uptrval)op)) { goto _output_error; } /* overflow detection */
        length += MINMATCH;
        if (op + length >= oend - FASTLOOP_SAFE_DISTANCE) {
          goto safe_match_copy;
        }
      } else {
        length += MINMATCH;
        if (op + length >= oend - FASTLOOP_SAFE_DISTANCE) {
          goto safe_match_copy;
        }

        /* Fastpath check: Avoids a branch in LZ4_wildCopy32 if true */
        if ((dict == withPrefix64k) || (match >= lowPrefix)) {
          if (offset >= 8) {
            assert(match >= lowPrefix);
            assert(match <= op);
            assert(op + 18 <= oend);

            LZ4_memcpy(op, match, 8);
            LZ4_memcpy(op + 8, match + 8, 8);
            LZ4_memcpy(op + 16, match + 16, 2);
            op += length;
            continue;
          }
        }
      }

      if (checkOffset && (unlikely(match + dictSize < lowPrefix))) { goto _output_error; } /* Error : offset outside buffers */
      /* match starting within external dictionary */
      if ((dict == usingExtDict) && (match < lowPrefix)) {
        if (unlikely(op + length > oend - LASTLITERALS)) {
          if (partialDecoding) {
            DEBUGLOG(7, "partialDecoding: dictionary match, close to dstEnd");
            length = MIN(length, (size_t)(oend - op));
          } else {
            goto _output_error;  /* end-of-block condition violated */
          }
        }

        if (length <= (size_t)(lowPrefix - match)) {
          /* match fits entirely within external dictionary : just copy */
          memmove(op, dictEnd - (lowPrefix - match), length);
          op += length;
        } else {
          /* match stretches into both external dictionary and current block */
          size_t const copySize = (size_t)(lowPrefix - match);
          size_t const restSize = length - copySize;
          LZ4_memcpy(op, dictEnd - copySize, copySize);
          op += copySize;
          if (restSize > (size_t)(op - lowPrefix)) {  /* overlap copy */
            BYTE *const endOfMatch = op + restSize;
            const BYTE *copyFrom = lowPrefix;
            while (op < endOfMatch) { *op++ = *copyFrom++; }
          } else {
            LZ4_memcpy(op, lowPrefix, restSize);
            op += restSize;
          }
        }
        continue;
      }

      /* copy match within block */
      cpy = op + length;

      assert((op <= oend) && (oend - op >= 32));
      if (unlikely(offset < 16)) {
        LZ4_memcpy_using_offset(op, match, cpy, offset);
      } else {
        LZ4_wildCopy32(op, match, cpy);
      }

      op = cpy;   /* wildcopy correction */
    }
  safe_decode:

    /* Main Loop : decode remaining sequences where output < FASTLOOP_SAFE_DISTANCE */
    while (1) {
      token = *ip++;
      length = token >> ML_BITS;  /* literal length */

      assert(!endOnInput || ip <= iend); /* ip < iend before the increment */

      /* A two-stage shortcut for the most common case:
       * 1) If the literal length is 0..14, and there is enough space,
       * enter the shortcut and copy 16 bytes on behalf of the literals
       * (in the fast mode, only 8 bytes can be safely copied this way).
       * 2) Further if the match length is 4..18, copy 18 bytes in a similar
       * manner; but we ensure that there's enough space in the output for
       * those 18 bytes earlier, upon entering the shortcut (in other words,
       * there is a combined check for both stages).
       */
      if ((endOnInput ? length != RUN_MASK : length <= 8)
        /* strictly "less than" on input, to re-enter the loop with at least one byte */
        && likely((endOnInput ? ip < shortiend : 1) & (op <= shortoend))) {
        /* Copy the literals */
        LZ4_memcpy(op, ip, endOnInput ? 16 : 8);
        op += length; ip += length;

        /* The second stage: prepare for match copying, decode full info.
         * If it doesn't work out, the info won't be wasted. */
        length = token & ML_MASK; /* match length */
        offset = LZ4_readLE16(ip); ip += 2;
        match = op - offset;
        assert(match <= op); /* check overflow */

        /* Do not deal with overlapping matches. */
        if ((length != ML_MASK)
          && (offset >= 8)
          && (dict == withPrefix64k || match >= lowPrefix)) {
          /* Copy the match. */
          LZ4_memcpy(op + 0, match + 0, 8);
          LZ4_memcpy(op + 8, match + 8, 8);
          LZ4_memcpy(op + 16, match + 16, 2);
          op += length + MINMATCH;
          /* Both stages worked, load the next token. */
          continue;
        }

        /* The second stage didn't work out, but the info is ready.
         * Propel it right to the point of match copying. */
        goto _copy_match;
      }

      /* decode literal length */
      if (length == RUN_MASK) {
        variable_length_error error = ok;
        length += read_variable_length(&ip, iend - RUN_MASK, (int)endOnInput, (int)endOnInput, &error);
        if (error == initial_error) { goto _output_error; }
        if ((safeDecode) && unlikely((uptrval)(op)+length < (uptrval)(op))) { goto _output_error; } /* overflow detection */
        if ((safeDecode) && unlikely((uptrval)(ip)+length < (uptrval)(ip))) { goto _output_error; } /* overflow detection */
      }

      /* copy literals */
      cpy = op + length;
      safe_literal_copy :
      LZ4_STATIC_ASSERT(MFLIMIT >= WILDCOPYLENGTH);
      if (((endOnInput) && ((cpy > oend - MFLIMIT) || (ip + length > iend - (2 + 1 + LASTLITERALS))))
        || ((!endOnInput) && (cpy > oend - WILDCOPYLENGTH))) {
        /* We've either hit the input parsing restriction or the output parsing restriction.
         * In the normal scenario, decoding a full block, it must be the last sequence,
         * otherwise it's an error (invalid input or dimensions).
         * In partialDecoding scenario, it's necessary to ensure there is no buffer overflow.
         */
        if (partialDecoding) {
          /* Since we are partial decoding we may be in this block because of the output parsing
           * restriction, which is not valid since the output buffer is allowed to be undersized.
           */
          assert(endOnInput);
          DEBUGLOG(7, "partialDecoding: copying literals, close to input or output end")
            DEBUGLOG(7, "partialDecoding: literal length = %u", (unsigned)length);
          DEBUGLOG(7, "partialDecoding: remaining space in dstBuffer : %i", (int)(oend - op));
          DEBUGLOG(7, "partialDecoding: remaining space in srcBuffer : %i", (int)(iend - ip));
          /* Finishing in the middle of a literals segment,
           * due to lack of input.
           */
          if (ip + length > iend) {
            length = (size_t)(iend - ip);
            cpy = op + length;
          }
          /* Finishing in the middle of a literals segment,
           * due to lack of output space.
           */
          if (cpy > oend) {
            cpy = oend;
            assert(op <= oend);
            length = (size_t)(oend - op);
          }
        } else {
          /* We must be on the last sequence because of the parsing limitations so check
           * that we exactly regenerate the original size (must be exact when !endOnInput).
           */
          if ((!endOnInput) && (cpy != oend)) { goto _output_error; }
          /* We must be on the last sequence (or invalid) because of the parsing limitations
           * so check that we exactly consume the input and don't overrun the output buffer.
           */
          if ((endOnInput) && ((ip + length != iend) || (cpy > oend))) {
            DEBUGLOG(6, "should have been last run of literals")
              DEBUGLOG(6, "ip(%p) + length(%i) = %p != iend (%p)", ip, (int)length, ip + length, iend);
            DEBUGLOG(6, "or cpy(%p) > oend(%p)", cpy, oend);
            goto _output_error;
          }
        }
        memmove(op, ip, length);  /* supports overlapping memory regions; only matters for in-place decompression scenarios */
        ip += length;
        op += length;
        /* Necessarily EOF when !partialDecoding.
         * When partialDecoding, it is EOF if we've either
         * filled the output buffer or
         * can't proceed with reading an offset for following match.
         */
        if (!partialDecoding || (cpy == oend) || (ip >= (iend - 2))) {
          break;
        }
      } else {
        LZ4_wildCopy8(op, ip, cpy);   /* may overwrite up to WILDCOPYLENGTH beyond cpy */
        ip += length; op = cpy;
      }

      /* get offset */
      offset = LZ4_readLE16(ip); ip += 2;
      match = op - offset;

      /* get matchlength */
      length = token & ML_MASK;

    _copy_match:
      if (length == ML_MASK) {
        variable_length_error error = ok;
        length += read_variable_length(&ip, iend - LASTLITERALS + 1, (int)endOnInput, 0, &error);
        if (error != ok) goto _output_error;
        if ((safeDecode) && unlikely((uptrval)(op)+length < (uptrval)op)) goto _output_error;   /* overflow detection */
      }
      length += MINMATCH;

      safe_match_copy :
      if ((checkOffset) && (unlikely(match + dictSize < lowPrefix))) goto _output_error;   /* Error : offset outside buffers */
      /* match starting within external dictionary */
      if ((dict == usingExtDict) && (match < lowPrefix)) {
        if (unlikely(op + length > oend - LASTLITERALS)) {
          if (partialDecoding) length = MIN(length, (size_t)(oend - op));
          else goto _output_error;   /* doesn't respect parsing restriction */
        }

        if (length <= (size_t)(lowPrefix - match)) {
          /* match fits entirely within external dictionary : just copy */
          memmove(op, dictEnd - (lowPrefix - match), length);
          op += length;
        } else {
          /* match stretches into both external dictionary and current block */
          size_t const copySize = (size_t)(lowPrefix - match);
          size_t const restSize = length - copySize;
          LZ4_memcpy(op, dictEnd - copySize, copySize);
          op += copySize;
          if (restSize > (size_t)(op - lowPrefix)) {  /* overlap copy */
            BYTE *const endOfMatch = op + restSize;
            const BYTE *copyFrom = lowPrefix;
            while (op < endOfMatch) *op++ = *copyFrom++;
          } else {
            LZ4_memcpy(op, lowPrefix, restSize);
            op += restSize;
          }
        }
        continue;
      }
      assert(match >= lowPrefix);

      /* copy match within block */
      cpy = op + length;

      /* partialDecoding : may end anywhere within the block */
      assert(op <= oend);
      if (partialDecoding && (cpy > oend - MATCH_SAFEGUARD_DISTANCE)) {
        size_t const mlen = MIN(length, (size_t)(oend - op));
        const BYTE *const matchEnd = match + mlen;
        BYTE *const copyEnd = op + mlen;
        if (matchEnd > op) {   /* overlap copy */
          while (op < copyEnd) { *op++ = *match++; }
        } else {
          LZ4_memcpy(op, match, mlen);
        }
        op = copyEnd;
        if (op == oend) { break; }
        continue;
      }

      if (unlikely(offset < 8)) {
        LZ4_write32(op, 0);   /* silence msan warning when offset==0 */
        op[0] = match[0];
        op[1] = match[1];
        op[2] = match[2];
        op[3] = match[3];
        match += inc32table[offset];
        LZ4_memcpy(op + 4, match, 4);
        match -= dec64table[offset];
      } else {
        LZ4_memcpy(op, match, 8);
        match += 8;
      }
      op += 8;

      if (unlikely(cpy > oend - MATCH_SAFEGUARD_DISTANCE)) {
        BYTE *const oCopyLimit = oend - (WILDCOPYLENGTH - 1);
        if (cpy > oend - LASTLITERALS) { goto _output_error; } /* Error : last LASTLITERALS bytes must be literals (uncompressed) */
        if (op < oCopyLimit) {
          LZ4_wildCopy8(op, match, oCopyLimit);
          match += oCopyLimit - op;
          op = oCopyLimit;
        }
        while (op < cpy) { *op++ = *match++; }
      } else {
        LZ4_memcpy(op, match, 8);
        if (length > 16) { LZ4_wildCopy8(op + 8, match + 8, cpy); }
      }
      op = cpy;   /* wildcopy correction */
    }

    /* end of decoding */
    if (endOnInput) {
      DEBUGLOG(5, "decoded %i bytes", (int)(((char *)op) - dst));
      return (int)(((char *)op) - dst);     /* Nb of output bytes decoded */
    } else {
      return (int)(((const char *)ip) - src);   /* Nb of input bytes read */
    }

    /* Overflow error detected */
  _output_error:
    return (int)(-(((const char *)ip) - src)) - 1;
  }
}

int LZ4_decompress_safe(const char *source, char *dest, int compressedSize, int maxDecompressedSize)
{
  return LZ4_decompress_generic(source, dest, compressedSize, maxDecompressedSize,
    endOnInputSize, decode_full_block, noDict,
    (BYTE *)dest, NULL, 0);
}

#endif // VORTEX_SLIM == 0
