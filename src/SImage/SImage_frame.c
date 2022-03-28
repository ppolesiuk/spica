/* This file is part of Spica, released under MIT license.
 * See LICENSE for details.
 */

/* Author: Piotr Polesiuk, 2022 */

#include "SImage_frame.h"

SImage_frame_t SImage_setFrame(
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

SImage_frame_t SImage_setFrameTr(
  const SImage_t     *tgt,
  const SImage_t     *src,
  const STransform_t *tr)
{
  SBoundingBox_t bb = STransform_boundingBox(tr, SImage_boundingBox(src));
  SImage_frame_t f = {
    .min_x = (int)bb.minX,
    .min_y = (int)bb.minY,
    .max_x = (int)bb.maxX + 1,
    .max_y = (int)bb.maxY + 1,
    .tgt_w = tgt->width,
    .src_w = src->width,
  };

  if (f.min_x < 0) f.min_x = 0;
  if (f.min_y < 0) f.min_y = 0;
  if (f.max_x > (int)tgt->width)  f.max_x = tgt->width;
  if (f.max_y > (int)tgt->height) f.max_y = tgt->height;
  return f;
}
