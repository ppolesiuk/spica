/* This file is part of Spica, released under MIT license.
 * See LICENSE for details.
 */

/* Author: Piotr Polesiuk, 2022 */

#ifndef __SPICA_DATA_REPR_H__
#define __SPICA_DATA_REPR_H__

#include <stdint.h>

/** Swap bytes in 16-bit number */
static uint16_t SSwapBytes16(uint16_t x) __attribute__((unused));
/** Swap bytes in 32-bit number */
static uint32_t SSwapBytes32(uint32_t x) __attribute__((unused));
/** Swap bytes in 64-bit number */
static uint64_t SSwapBytes64(uint64_t x) __attribute__((unused));

/** Convert 16-bit number to/from little-endian */
static uint16_t SLittleEndian16(uint16_t x) __attribute__((unused));
/** Convert 32-bit number to/from little-endian */
static uint32_t SLittleEndian32(uint32_t x) __attribute__((unused));
/** Convert 64-bit number to/from little-endian */
static uint64_t SLittleEndian64(uint64_t x) __attribute__((unused));
/** Convert 16-bit number to/from big-endian */
static uint16_t SBigEndian16(uint16_t x) __attribute__((unused));
/** Convert 32-bit number to/from big-endian */
static uint32_t SBigEndian32(uint32_t x) __attribute__((unused));
/** Convert 64-bit number to/from big-endian */
static uint64_t SBigEndian64(uint64_t x) __attribute__((unused));

/* ========================================================================= */

static uint16_t SSwapBytes16(uint16_t x) {
  return (x << 8) | (x >> 8);
}

static uint32_t SSwapBytes32(uint32_t x) {
  return ((x & UINT32_C(0x000000FF)) << 24) |
         ((x & UINT32_C(0x0000FF00)) << 8)  |
         ((x & UINT32_C(0x00FF0000)) >> 8)  |
         ((x & UINT32_C(0xFF000000)) >> 24);
}

static uint64_t SSwapBytes64(uint64_t x) {
  x = (x << 32) | (x >> 32);
  x = ((x & UINT64_C(0x0000FFFF0000FFFF)) << 16) |
      ((x >> 16) & UINT64_C(0x0000FFFF0000FFFF));
  x = ((x & UINT64_C(0x00FF00FF00FF00FF)) << 8) |
      ((x >> 8) & UINT64_C(0x00FF00FF00FF00FF));
  return x;
}

#ifndef __BYTE_ORDER__
# error Your compiler does not support __BYTE_ORDER__ macros
#elif __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
static uint16_t SLittleEndian16(uint16_t x) {
  return x;
}

static uint32_t SLittleEndian32(uint32_t x) {
  return x;
}

static uint64_t SLittleEndian64(uint64_t x) {
  return x;
}

static uint16_t SBigEndian16(uint16_t x) {
  return SSwapBytes16(x);
}

static uint32_t SBigEndian32(uint32_t x) {
  return SSwapBytes32(x);
}

static uint64_t SBigEndian64(uint64_t x) {
  return SSwapBytes64(x);
}
#else
#  error unsupported endianness
#endif

#endif /* __SPICA_DATA_REPR_H__ */
