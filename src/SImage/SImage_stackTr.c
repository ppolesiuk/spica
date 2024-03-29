/* This file is part of Spica, released under MIT license.
 * See LICENSE for details.
 */

/* Author: Piotr Polesiuk, 2022 */

#include "SImage.h"
#include "SImage_frame.h"

typedef SVec2f_t (*subpixelGray_t)(const SImage_t *, SVec2f_t);
typedef SVec4f_t (*subpixelRGB_t)(const SImage_t *, SVec2f_t);

static void stackTrGray(
  SImage_t           *tgt,
  SVec2f_t           *tgt_data,
  const SImage_t     *src,
  subpixelGray_t      subpixel,
  const STransform_t *tr,
  const STransform_t *tr_inv)
{
  SImage_frame_t f = SImage_setFrameTr(tgt, src, tr);
  for (int y = f.min_y; y < f.max_y; y++) {
    for (int x = f.min_x; x < f.max_x; x++) {
      tgt_data[y * f.tgt_w + x] +=
        subpixel(src, STransform_apply(tr_inv, SVec2f(x, y)));
    }
  }
}

static void stackTrRGB(
  SImage_t           *tgt,
  SVec4f_t           *tgt_data,
  const SImage_t     *src,
  subpixelRGB_t       subpixel,
  const STransform_t *tr,
  const STransform_t *tr_inv)
{
  SImage_frame_t f = SImage_setFrameTr(tgt, src, tr);
  for (int y = f.min_y; y < f.max_y; y++) {
    for (int x = f.min_x; x < f.max_x; x++) {
      tgt_data[y * f.tgt_w + x] +=
        subpixel(src, STransform_apply(tr_inv, SVec2f(x, y)));
    }
  }
}

static void stackTrMain(
  SImage_t           *tgt,
  const STransform_t *tr,
  const STransform_t *tr_inv,
  const SImage_t     *src)
{
  if (src->format == SFmt_Invalid || tr->type == STr_Drop) return;

  switch (tgt->format) {
  case SFmt_Invalid:
    return;
  case SFmt_Gray:
    stackTrGray(tgt, tgt->data_gray, src, SImage_subpixelGray, tr, tr_inv);
    return;
  case SFmt_RGB:
    stackTrRGB(tgt, tgt->data_rgb, src, SImage_subpixelRGB, tr, tr_inv);
    return;
  case SFmt_SeparateRGB:
    stackTrGray(tgt, SImage_dataRed(tgt),
      src, SImage_subpixelRed,   tr, tr_inv);
    stackTrGray(tgt, SImage_dataGreen(tgt),
      src, SImage_subpixelGreen, tr, tr_inv);
    stackTrGray(tgt, SImage_dataBlue(tgt),
      src, SImage_subpixelBlue,  tr, tr_inv);
    return;
  }
}

/* ========================================================================= */
void SImage_stackTr(
  SImage_t           *tgt,
  const STransform_t *tr,
  const SImage_t     *src)
{
  if (tr->type == STr_Drop) return;

  STransform_t tr_inv = STransform_inverse(tr);
  stackTrMain(tgt, tr, &tr_inv, src);
}

void SImage_stackTrInv(
  SImage_t           *tgt,
  const STransform_t *tr,
  const SImage_t     *src)
{
  if (tr->type == STr_Drop) return;

  STransform_t tr_inv = STransform_inverse(tr);
  stackTrMain(tgt, &tr_inv, tr, src);
}
