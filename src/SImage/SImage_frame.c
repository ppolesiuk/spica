/* This file is part of Spica, released under MIT license.
 * See LICENSE for details.
 */

/* Author: Piotr Polesiuk, 2022 */

#include "SImage_frame.h"

SImage_frame_t setFrame(
  const SImage_t *tgt,
  const SImage_t *src,
  int x_offset,
  int y_offset)
{
  SImage_frame_t f;
  f.min_x = x_offset > 0 ? x_offset : 0;
  f.min_y = y_offset > 0 ? y_offset : 0;
  f.max_x = (int)src->width  + x_offset;
  f.max_y = (int)src->height + y_offset;
  if ((int)tgt->width  < f.max_x) f.max_x = tgt->width;
  if ((int)tgt->height < f.max_y) f.max_y = tgt->height;
  f.tgt_w = tgt->width;
  f.src_w = src->width;
  return f;
}
