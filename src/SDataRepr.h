/* This file is part of Spica, released under MIT license.
 * See LICENSE for details.
 */

/* Author: Piotr Polesiuk, 2022 */

#ifndef __SPICA_DATA_REPR_H__
#define __SPICA_DATA_REPR_H__

#include <stdint.h>

/** Convert 16-bit number to/from little-endian */
static uint16_t SLittleEndian16(uint16_t x) __attribute__((unused));
/** Convert 32-bit number to/from little-endian */
static uint32_t SLittleEndian32(uint32_t x) __attribute__((unused));

/* ========================================================================= */

#ifndef __BYTE_ORDER__
# error Your compiler does not support __BYTE_ORDER__ macros
#elif __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
static uint16_t SLittleEndian16(uint16_t x) {
  return x;
}

static uint32_t SLittleEndian32(uint32_t x) {
  return x;
}
#else
#  error unsupported endianness
#endif

#endif /* __SPICA_DATA_REPR_H__ */
