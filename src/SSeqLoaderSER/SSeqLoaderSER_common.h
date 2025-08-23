/* This file is part of Spica, released under MIT license.
 * See LICENSE for details.
 */

/* Author: Piotr Polesiuk, 2025 */

#ifndef __SLOADER_SER_COMMON_H__
#define __SLOADER_SER_COMMON_H__

#include <stdint.h>

#define SER_HEADER_PADDING    6
#define SER_HEADER_SIZE       178
#define SER_HEADER_MAGIC      "LUCAM-RECORDER"
#define SER_HEADER_MAGIC_SIZE 14

#define SER_LITTLE_ENDIAN 0
#define SER_BIG_ENDIAN    1

typedef struct SER_header {
  /** padding, in order to align the structure to 8 bytes */
  uint8_t padding[SER_HEADER_PADDING];

  /** magic string */
  char magic[SER_HEADER_MAGIC_SIZE];

  /** unused, usually set to 0 */
  uint32_t lu_id;

  /** colorId */
  uint32_t color_id;

  /** The meaning of this field is opposite to the one in the SER
   * specification. probably a bug in the specification. */
  uint32_t endiannes;

  /** The width of the images in pixels. */
  uint32_t width;

  /** The height of the images in pixels. */
  uint32_t height;

  /** The number of bits per pixel. */
  uint32_t pixel_depth_per_plane;

  /** The number of images in the sequence. */
  uint32_t frame_count;

  /** Observer */
  uint8_t observer[40];

  /** Instrument */
  uint8_t instrument[40];

  /** Telescope */
  uint8_t telescope[40];

  /** The date and time of the first image in the stream (local time). */
  int64_t date_time;

  /** The date and time of the first image in the stream (UTC). */
  int64_t date_time_utc;
} SER_header_t;

#endif /* __SLOADER_SER_COMMON_H__ */
