/* This file is part of Spica, released under MIT license.
 * See LICENSE for details.
 */

/* Helper functions for operations on image frames */

/* Author: Piotr Polesiuk, 2022 */

#ifndef __SIMAGE_FRAME_H__
#define __SIMAGE_FRAME_H__

#include "SImage.h"

typedef struct SImage_frame {
  int min_x; /** Minimal x coordinate (inclusive) */
  int max_x; /** Maximal x coordinate (exclusive) */
  int min_y; /** Minimal y coordinage (inclusive) */
  int max_y; /** Maximal y coordinage (exclusive) */
  int tgt_w; /** Target image width */
  int src_w; /** Source image width */
} SImage_frame_t;

/** Generate frame for tgt image, that contains intersection with src image
 */
SImage_frame_t SImage_setFrame(
  const SImage_t *tgt,
  const SImage_t *src,
  int x_offset,
  int y_offset);

#endif /* __SIMAGE_FRAME_H__ */
