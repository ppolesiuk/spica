/* This file is part of Spica, released under MIT license.
 * See LICENSE for details.
 */

/* Author: Piotr Polesiuk, 2025 */

#include "SImage.h"
#include "SImage_frame.h"

static void maskByWeightGray(
        SImage_t *tgt,       SVec2f_t *tgt_data,
  const SImage_t *src, const SVec2f_t *src_data,
  int x_offset, int y_offset)
{
  SImage_frame_t f = SImage_setFrame(tgt, src, x_offset, y_offset);
  for (int y = f.min_y; y < f.max_y; y++) {
    for (int x = f.min_x; x < f.max_x; x++) {
      SVec2f_t pix = src_data[(y - y_offset) * f.src_w + x - x_offset];
      tgt_data[y * f.tgt_w + x] *= pix[1];
    }
  }
}

static void maskByWeightGray_with_RGB(
        SImage_t *tgt,       SVec2f_t *tgt_data,
  const SImage_t *src, const SVec4f_t *src_data,
  int x_offset, int y_offset)
{
  SImage_frame_t f = SImage_setFrame(tgt, src, x_offset, y_offset);
  for (int y = f.min_y; y < f.max_y; y++) {
    for (int x = f.min_x; x < f.max_x; x++) {
      SVec4f_t pix = src_data[(y - y_offset) * f.src_w + x - x_offset];
      tgt_data[y * f.tgt_w + x] *= pix[3];
    }
  }
}

static void maskByWeightRGB_with_Gray(
        SImage_t *tgt,       SVec4f_t *tgt_data,
  const SImage_t *src, const SVec2f_t *src_data,
  int x_offset, int y_offset)
{
  SImage_frame_t f = SImage_setFrame(tgt, src, x_offset, y_offset);
  for (int y = f.min_y; y < f.max_y; y++) {
    for (int x = f.min_x; x < f.max_x; x++) {
      SVec2f_t pix = src_data[(y - y_offset) * f.src_w + x - x_offset];
      tgt_data[y * f.tgt_w + x] *= pix[1];
    }
  }
}

static void maskByWeightRGB(
        SImage_t *tgt,       SVec4f_t *tgt_data,
  const SImage_t *src, const SVec4f_t *src_data,
  int x_offset, int y_offset)
{
  SImage_frame_t f = SImage_setFrame(tgt, src, x_offset, y_offset);
  for (int y = f.min_y; y < f.max_y; y++) {
    for (int x = f.min_x; x < f.max_x; x++) {
      SVec4f_t pix = src_data[(y - y_offset) * f.src_w + x - x_offset];
      tgt_data[y * f.tgt_w + x] *= pix[3];
    }
  }
}

static void maskByWeight_with_Gray(
  SImage_t *image, int x_offset, int y_offset, const SImage_t *mask)
{
  switch (image->format) {
  case SFmt_Invalid:
    return;
  case SFmt_Gray:
    maskByWeightGray(
      image, image->data_gray,
      mask,  mask->data_gray,
      x_offset, y_offset);
    return;
  case SFmt_RGB:
    maskByWeightRGB_with_Gray(
      image, image->data_rgb,
      mask,  mask->data_gray,
      x_offset, y_offset);
    return;
  case SFmt_SeparateRGB:
    maskByWeightGray(
      image, SImage_dataRed(image),
      mask,  mask->data_gray,
      x_offset, y_offset);
    maskByWeightGray(
      image, SImage_dataGreen(image),
      mask,  mask->data_gray,
      x_offset, y_offset);
    maskByWeightGray(
      image, SImage_dataBlue(image),
      mask,  mask->data_gray,
      x_offset, y_offset);
    return;
  }
}

static void maskByWeight_with_RGB(
  SImage_t *image, int x_offset, int y_offset, const SImage_t *mask)
{
  switch (image->format) {
  case SFmt_Invalid:
    return;
  case SFmt_Gray:
    maskByWeightGray_with_RGB(
      image, image->data_gray,
      mask,  mask->data_rgb,
      x_offset, y_offset);
    return;
  case SFmt_RGB:
    maskByWeightRGB(
      image, image->data_rgb,
      mask,  mask->data_rgb,
      x_offset, y_offset);
    return;
  case SFmt_SeparateRGB:
    maskByWeightGray_with_RGB(
      image, SImage_dataRed(image),
      mask,  mask->data_rgb,
      x_offset, y_offset);
    maskByWeightGray_with_RGB(
      image, SImage_dataGreen(image),
      mask,  mask->data_rgb,
      x_offset, y_offset);
    maskByWeightGray_with_RGB(
      image, SImage_dataBlue(image),
      mask,  mask->data_rgb,
      x_offset, y_offset);
    return;
  }
}

void SImage_maskByWeight(
  SImage_t *image, int x_offset, int y_offset, const SImage_t *mask)
{
  switch (mask->format) {
  case SFmt_Invalid:
    return;
  case SFmt_Gray:
    maskByWeight_with_Gray(image, x_offset, y_offset, mask);
    return;
  case SFmt_RGB:
    maskByWeight_with_RGB(image, x_offset, y_offset, mask);
    return;
  case SFmt_SeparateRGB:
    if (image->format == SFmt_SeparateRGB) {
      maskByWeightGray(
        image, SImage_dataRed(image),
        mask,  SImage_dataRed(mask),
        x_offset, y_offset);
      maskByWeightGray(
        image, SImage_dataGreen(image),
        mask,  SImage_dataGreen(mask),
        x_offset, y_offset);
      maskByWeightGray(
        image, SImage_dataBlue(image),
        mask,  SImage_dataBlue(mask),
        x_offset, y_offset);
    } else {
      SImage_t mask2;
      SImage_toFormat_at(&mask2, mask, SFmt_Gray);
      if (mask2.format != SFmt_Invalid)
        maskByWeight_with_Gray(image, x_offset, y_offset, &mask2);
      SImage_deinit(&mask2);
    }
    return;
  }
}
