/* This file is part of Spica, released under MIT license.
 * See LICENSE for details.
 */

/* Author: Piotr Polesiuk, 2023 */

#include "SImage.h"
#include "SImage_frame.h"

typedef SVec2f_t (*subpixelGray_t)(const SImage_t *, SVec2f_t);
typedef SVec4f_t (*subpixelRGB_t)(const SImage_t *, SVec2f_t);

static void mulTrGray(
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
      SVec2f_t spix =
        subpixel(src, STransform_apply(tr_inv, SVec2f(x, y)));
      if (spix[1] == 0.0f) continue;
      float v = spix[0] / spix[1];
      tgt_data[y * f.tgt_w + x][0] *= v;
    }
  }
}

static void mulTrRGB(
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
      SVec4f_t spix =
        subpixel(src, STransform_apply(tr_inv, SVec2f(x, y)));
      if (spix[3] == 0.0f) continue;
      spix *= 1.0f / spix[3];
      tgt_data[y * f.tgt_w + x] *= spix;
    }
  }
}

static void mulTrMain(
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
    mulTrGray(tgt, tgt->data_gray, src, SImage_subpixelGray, tr, tr_inv);
    return;
  case SFmt_RGB:
    mulTrRGB(tgt, tgt->data_rgb, src, SImage_subpixelRGB, tr, tr_inv);
    return;
  case SFmt_SeparateRGB:
    mulTrGray(tgt, SImage_dataRed(tgt),
      src, SImage_subpixelRed,   tr, tr_inv);
    mulTrGray(tgt, SImage_dataGreen(tgt),
      src, SImage_subpixelGreen, tr, tr_inv);
    mulTrGray(tgt, SImage_dataBlue(tgt),
      src, SImage_subpixelBlue,  tr, tr_inv);
    return;
  }
}

/* ========================================================================= */
void SImage_mulTr(
  SImage_t           *tgt,
  const STransform_t *tr,
  const SImage_t     *src)
{
  if (tr->type == STr_Drop) return;

  STransform_t tr_inv = STransform_inverse(tr);
  mulTrMain(tgt, tr, &tr_inv, src);
}

void SImage_mulTrInv(
  SImage_t           *tgt,
  const STransform_t *tr,
  const SImage_t     *src)
{
  if (tr->type == STr_Drop) return;

  STransform_t tr_inv = STransform_inverse(tr);
  mulTrMain(tgt, &tr_inv, tr, src);
}
