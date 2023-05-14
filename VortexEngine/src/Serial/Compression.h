#ifndef COMPRESSION_H
#define COMPRESSION_H

// This is a heavily stripped down version of lz4

#include "../VortexConfig.h"

#include <inttypes.h>
#include <stddef.h>

// super basic runtime length encoding compression because LZ4 overflows the stack on attiny
#if VORTEX_SLIM == 1

#define compress_size(srcSize) (2 * (srcSize))
#define compress_buffer(src, dst, srcSize, dstCapacity) rle_compress(src, dst, srcSize, dstCapacity)
#define decompress_buffer(src, dst, srcSize, dstCapacity) rle_decompress(src, dst, srcSize, dstCapacity)

int rle_compress(const uint8_t *src, uint8_t *dst, int srcSize, int dstCapacity);
int rle_decompress(const uint8_t *src, uint8_t *dst, int srcSize, int dstCapacity);

#else

#define compress_size(srcSize) LZ4_compressBound(srcSize)
#define compress_buffer(src, dst, srcSize, dstCapacity) LZ4_compress_default((const char *)src, (char *)dst, srcSize, dstCapacity)
#define decompress_buffer(src, dst, srcSize, dstCapacity) LZ4_decompress_safe((const char *)src, (char *)dst, srcSize, dstCapacity)

// memory usage cranked down to minimum
#define LZ4_MEMORY_USAGE_MIN 10
#define LZ4_MEMORY_USAGE_DEFAULT 10
#define LZ4_MEMORY_USAGE_MAX 20

#ifndef LZ4_MEMORY_USAGE
# define LZ4_MEMORY_USAGE LZ4_MEMORY_USAGE_DEFAULT
#endif

#if (LZ4_MEMORY_USAGE < LZ4_MEMORY_USAGE_MIN)
#  error "LZ4_MEMORY_USAGE is too small !"
#endif

#if (LZ4_MEMORY_USAGE > LZ4_MEMORY_USAGE_MAX)
#  error "LZ4_MEMORY_USAGE is too large !"
#endif

/*! LZ4_compress_default() :
 *  Compresses 'srcSize' bytes from buffer 'src'
 *  into already allocated 'dst' buffer of size 'dstCapacity'.
 *  Compression is guaranteed to succeed if 'dstCapacity' >= LZ4_compressBound(srcSize).
 *  It also runs faster, so it's a recommended setting.
 *  If the function cannot compress 'src' into a more limited 'dst' budget,
 *  compression stops *immediately*, and the function result is zero.
 *  In which case, 'dst' content is undefined (invalid).
 *      srcSize : max supported value is LZ4_MAX_INPUT_SIZE.
 *      dstCapacity : size of buffer 'dst' (which must be already allocated)
 *     @return  : the number of bytes written into buffer 'dst' (necessarily <= dstCapacity)
 *                or 0 if compression fails
 * Note : This function is protected against buffer overflow scenarios (never writes outside 'dst' buffer, nor read outside 'source' buffer).
 */
int LZ4_compress_default(const char *src, char *dst, int srcSize, int dstCapacity);

/*! LZ4_decompress_safe() :
 *  compressedSize : is the exact complete size of the compressed block.
 *  dstCapacity : is the size of destination buffer (which must be already allocated), presumed an upper bound of decompressed size.
 * @return : the number of bytes decompressed into destination buffer (necessarily <= dstCapacity)
 *           If destination buffer is not large enough, decoding will stop and output an error code (negative value).
 *           If the source stream is detected malformed, the function will stop decoding and return a negative result.
 * Note 1 : This function is protected against malicious data packets :
 *          it will never writes outside 'dst' buffer, nor read outside 'source' buffer,
 *          even if the compressed block is maliciously modified to order the decoder to do these actions.
 *          In such case, the decoder stops immediately, and considers the compressed block malformed.
 * Note 2 : compressedSize and dstCapacity must be provided to the function, the compressed block does not contain them.
 *          The implementation is free to send / store / derive this information in whichever way is most beneficial.
 *          If there is a need for a different format which bundles together both compressed data and its metadata, consider looking at lz4frame.h instead.
 */
int LZ4_decompress_safe(const char *src, char *dst, int compressedSize, int dstCapacity);

/*-************************************
*  Advanced Functions
**************************************/
#define LZ4_MAX_INPUT_SIZE        0x7E000000   /* 2 113 929 216 bytes */
#define LZ4_COMPRESSBOUND(isize)  ((unsigned)(isize) > (unsigned)LZ4_MAX_INPUT_SIZE ? 0 : (isize) + ((isize)/255) + 16)

/*! LZ4_compressBound() :
    Provides the maximum size that LZ4 compression may output in a "worst case" scenario (input data not compressible)
    This function is primarily useful for memory allocation purposes (destination buffer size).
    Macro LZ4_COMPRESSBOUND() is also provided for compilation-time evaluation (stack memory allocation for example).
    Note that LZ4_compress_default() compresses faster when dstCapacity is >= LZ4_compressBound(srcSize)
        inputSize  : max supported value is LZ4_MAX_INPUT_SIZE
        return : maximum output size in a "worst case" scenario
              or 0, if input size is incorrect (too large or negative)
*/
int LZ4_compressBound(int inputSize);

/*! LZ4_compress_fast() :
    Same as LZ4_compress_default(), but allows selection of "acceleration" factor.
    The larger the acceleration value, the faster the algorithm, but also the lesser the compression.
    It's a trade-off. It can be fine tuned, with each successive value providing roughly +~3% to speed.
    An acceleration value of "1" is the same as regular LZ4_compress_default()
    Values <= 0 will be replaced by LZ4_ACCELERATION_DEFAULT (currently == 1, see lz4.c).
    Values > LZ4_ACCELERATION_MAX will be replaced by LZ4_ACCELERATION_MAX (currently == 65537, see lz4.c).
*/
int LZ4_compress_fast(const char *src, char *dst, int srcSize, int dstCapacity, int acceleration);

/*! LZ4_compress_fast_extState() :
 *  Same as LZ4_compress_fast(), using an externally allocated memory space for its state.
 *  Use LZ4_sizeofState() to know how much memory must be allocated,
 *  and allocate it on 8-bytes boundaries (using `malloc()` typically).
 *  Then, provide this buffer as `void* state` to compression function.
 */
int LZ4_compress_fast_extState(void *state, const char *src, char *dst, int srcSize, int dstCapacity, int acceleration);

/*-*********************************************
*  Streaming Compression Functions
***********************************************/
typedef union LZ4_stream_u LZ4_stream_t;  /* incomplete type (defined later) */

/*-**********************************************
*  Streaming Decompression Functions
*  Bufferless synchronous API
************************************************/
typedef union LZ4_streamDecode_u LZ4_streamDecode_t;   /* tracking context */

#define LZ4_DECODER_RING_BUFFER_SIZE(maxBlockSize) (65536 + 14 + (maxBlockSize))  /* for static allocation; maxBlockSize presumed valid */

#define LZ4_DECOMPRESS_INPLACE_MARGIN(compressedSize)          (((compressedSize) >> 8) + 32)
#define LZ4_DECOMPRESS_INPLACE_BUFFER_SIZE(decompressedSize)   ((decompressedSize) + LZ4_DECOMPRESS_INPLACE_MARGIN(decompressedSize))  /**< note: presumes that compressedSize < decompressedSize. note2: margin is overestimated a bit, since it could use compressedSize instead */

#ifndef LZ4_DISTANCE_MAX   /* history window size; can be user-defined at compile time */
#  define LZ4_DISTANCE_MAX 65535   /* set to maximum value by default */
#endif

#define LZ4_COMPRESS_INPLACE_MARGIN                           (LZ4_DISTANCE_MAX + 32)   /* LZ4_DISTANCE_MAX can be safely replaced by srcSize when it's smaller */
#define LZ4_COMPRESS_INPLACE_BUFFER_SIZE(maxCompressedSize)   ((maxCompressedSize) + LZ4_COMPRESS_INPLACE_MARGIN)  /**< maxCompressedSize is generally LZ4_COMPRESSBOUND(inputSize), but can be set to any lower value, with the risk that compression can fail (return code 0(zero)) */

 /*-************************************************************
  *  Private Definitions
  **************************************************************
  * Do not use these definitions directly.
  * They are only exposed to allow static allocation of `LZ4_stream_t` and `LZ4_streamDecode_t`.
  * Accessing members will expose user code to API and/or ABI break in future versions of the library.
  **************************************************************/
#define LZ4_HASHLOG   (LZ4_MEMORY_USAGE-2)
#define LZ4_HASHTABLESIZE (1 << LZ4_MEMORY_USAGE)
#define LZ4_HASH_SIZE_U32 (1 << LZ4_HASHLOG)       /* required as macro for static allocation */

#include <stdint.h>
typedef  int8_t  LZ4_i8;
typedef uint8_t  LZ4_byte;
typedef uint16_t LZ4_u16;
typedef uint32_t LZ4_u32;

typedef struct LZ4_stream_t_internal LZ4_stream_t_internal;
struct LZ4_stream_t_internal
{
  LZ4_u32 hashTable[LZ4_HASH_SIZE_U32];
  LZ4_u32 currentOffset;
  LZ4_u32 tableType;
  const LZ4_byte *dictionary;
  const LZ4_stream_t_internal *dictCtx;
  LZ4_u32 dictSize;
};

typedef struct
{
  const LZ4_byte *externalDict;
  size_t extDictSize;
  const LZ4_byte *prefixEnd;
  size_t prefixSize;
} LZ4_streamDecode_t_internal;


/*! LZ4_stream_t :
 *  Do not use below internal definitions directly !
 *  Declare or allocate an LZ4_stream_t instead.
 *  LZ4_stream_t can also be created using LZ4_createStream(), which is recommended.
 *  The structure definition can be convenient for static allocation
 *  (on stack, or as part of larger structure).
 *  Init this structure with LZ4_initStream() before first use.
 *  note : only use this definition in association with static linking !
 *  this definition is not API/ABI safe, and may change in future versions.
 */
#define LZ4_STREAMSIZE       ((1UL << LZ4_MEMORY_USAGE) + 32)  /* static size, for inter-version compatibility */
#define LZ4_STREAMSIZE_VOIDP (LZ4_STREAMSIZE / sizeof(void*))
union LZ4_stream_u
{
  void *table[LZ4_STREAMSIZE_VOIDP];
  LZ4_stream_t_internal internal_donotuse;
}; /* previously typedef'd to LZ4_stream_t */


/*! LZ4_initStream() : v1.9.0+
 *  An LZ4_stream_t structure must be initialized at least once.
 *  This is automatically done when invoking LZ4_createStream(),
 *  but it's not when the structure is simply declared on stack (for example).
 *
 *  Use LZ4_initStream() to properly initialize a newly declared LZ4_stream_t.
 *  It can also initialize any arbitrary buffer of sufficient size,
 *  and will @return a pointer of proper type upon initialization.
 *
 *  Note : initialization fails if size and alignment conditions are not respected.
 *         In which case, the function will @return NULL.
 *  Note2: An LZ4_stream_t structure guarantees correct alignment and size.
 *  Note3: Before v1.9.0, use LZ4_resetStream() instead
 */
LZ4_stream_t *LZ4_initStream(void *buffer, size_t size);

/*! LZ4_streamDecode_t :
 *  information structure to track an LZ4 stream during decompression.
 *  init this structure  using LZ4_setStreamDecode() before first use.
 *  note : only use in association with static linking !
 *         this definition is not API/ABI safe,
 *         and may change in a future version !
 */
#define LZ4_STREAMDECODESIZE_U64 (4 + ((sizeof(void*)==16) ? 2 : 0) /*AS-400*/ )
#define LZ4_STREAMDECODESIZE     (LZ4_STREAMDECODESIZE_U64 * sizeof(unsigned long long))
union LZ4_streamDecode_u
{
  unsigned long long table[LZ4_STREAMDECODESIZE_U64];
  LZ4_streamDecode_t_internal internal_donotuse;
};   /* previously typedef'd to LZ4_streamDecode_t */

#endif
#endif
