/* This file is part of Spica, released under MIT license.
 * See LICENSE for details.
 */

/* Author: Piotr Polesiuk, 2025 */

#include "SImage.h"
#include "SImage_frame.h"

typedef SVec2f_t (*subpixelGray_t)(const SImage_t *, SVec2f_t);

void maskByWeightTrGray(
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
      tgt_data[y * f.tgt_w + x] *= spix[1];
    }
  }
}

void maskByWeightTrRGB(
  SImage_t           *tgt,
  SVec4f_t           *tgt_data,
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
      tgt_data[y * f.tgt_w + x] *= spix[1];
    }
  }
}

void maskByWeightTrMain(
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
    maskByWeightTrGray(
      tgt, tgt->data_gray, src, SImage_subpixelGray, tr, tr_inv);
    return;
  case SFmt_RGB:
    maskByWeightTrRGB(
      tgt, tgt->data_rgb, src, SImage_subpixelGray, tr, tr_inv);
    return;
  case SFmt_SeparateRGB:
    maskByWeightTrGray(
      tgt, SImage_dataRed(tgt), src, SImage_subpixelRed, tr, tr_inv);
    maskByWeightTrGray(
      tgt, SImage_dataGreen(tgt), src, SImage_subpixelGreen, tr, tr_inv);
    maskByWeightTrGray(
      tgt, SImage_dataBlue(tgt), src, SImage_subpixelBlue, tr, tr_inv);
    return;
  }
}

/* ========================================================================= */
void SImage_maskByWeightTr(
  SImage_t           *image,
  const STransform_t *tr,
  const SImage_t     *mask)
{
  if (tr->type == STr_Drop) return;

  STransform_t tr_inv = STransform_inverse(tr);
  maskByWeightTrMain(image, tr, &tr_inv, mask);
}

void SImage_maskByWeightTrInv(
  SImage_t           *image,
  const STransform_t *tr,
  const SImage_t     *mask)
{
  if (tr->type == STr_Drop) return;

  STransform_t tr_inv = STransform_inverse(tr);
  maskByWeightTrMain(image, &tr_inv, tr, mask);
}
