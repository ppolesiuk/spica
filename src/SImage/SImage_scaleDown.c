/* This file is part of Spica, released under MIT license.
 * See LICENSE for details.
 */

/* Author: Piotr Polesiuk, 2022 */

#include "SImage.h"

#include <assert.h>
#include <stdlib.h>

inline static unsigned umin(unsigned x, unsigned y) {
  return x < y ? x : y;
}

static void scaleDownGray(
  SVec2f_t *dst,        unsigned dst_w, unsigned dst_h,
  const SVec2f_t *src,  unsigned src_w, unsigned src_h,
  unsigned factor)
{
  for (unsigned y = 0; y < dst_h; y++) {
    for (unsigned x = 0; x < dst_w; x++) {
      SVec2f_t v = { 0.0f, 0.0f };
      unsigned mx = umin(src_w, factor * (x + 1));
      unsigned my = umin(src_h, factor * (y + 1));
      for (unsigned sy = factor * y; sy < my; sy++) {
        for (unsigned sx = factor * x; sx < mx; sx++) {
          v += src[sy * src_w + sx];
        }
      }
      dst[y * dst_w + x] = v;
    }
  }
}

static void scaleDownRGB(
  SVec4f_t *dst,        unsigned dst_w, unsigned dst_h,
  const SVec4f_t *src,  unsigned src_w, unsigned src_h,
  unsigned factor)
{
  for (unsigned y = 0; y < dst_h; y++) {
    for (unsigned x = 0; x < dst_w; x++) {
      SVec4f_t v = { 0.0f, 0.0f };
      unsigned mx = umin(src_w, factor * (x + 1));
      unsigned my = umin(src_h, factor * (y + 1));
      for (unsigned sy = factor * y; sy < my; sy++) {
        for (unsigned sx = factor * x; sx < mx; sx++) {
          v += src[sy * src_w + sx];
        }
      }
      dst[y * dst_w + x] = v;
    }
  }
}

void SImage_scaleDown_at(
  SImage_t       *dst,
  const SImage_t *image,
  unsigned        factor)
{
  assert(factor != 0);
  unsigned width  = (image->width  + factor - 1) / factor;
  unsigned height = (image->height + factor - 1) / factor;
  SImage_init(dst, width, height, image->format);
  switch (dst->format) {
  case SFmt_Invalid:
    return;
  case SFmt_Gray:
    scaleDownGray(
      dst->data_gray,   width,        height,
      image->data_gray, image->width, image->height,
      factor);
    break;
  case SFmt_RGB:
    scaleDownRGB(
      dst->data_rgb,   width,        height,
      image->data_rgb, image->width, image->height,
      factor);
    break;
  case SFmt_SeparateRGB:
    scaleDownGray(
      SImage_dataRed(dst),   width,        height,
      SImage_dataRed(image), image->width, image->height,
      factor);
    scaleDownGray(
      SImage_dataGreen(dst),   width,        height,
      SImage_dataGreen(image), image->width, image->height,
      factor);
    scaleDownGray(
      SImage_dataBlue(dst),   width,        height,
      SImage_dataBlue(image), image->width, image->height,
      factor);
    break;
  }
}

SImage_t *SImage_scaleDown(const SImage_t *image, unsigned factor) {
  SImage_t *dst = malloc(sizeof(SImage_t));
  if (dst == NULL) return NULL;
  SImage_scaleDown_at(dst, image, factor);
  return dst;
}
