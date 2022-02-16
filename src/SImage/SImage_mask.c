/* This file is part of Spica, released under MIT license.
 * See LICENSE for details.
 */

/* Author: Piotr Polesiuk, 2022 */

#include "SImage.h"
#include "SImage_frame.h"

#include <assert.h>

static void maskGray(
        SImage_t *tgt,       SVec2f_t *tgt_data,
  const SImage_t *src, const SVec2f_t *src_data,
  int x_offset, int y_offset)
{
  SImage_frame_t f = SImage_setFrame(tgt, src, x_offset, y_offset);
  for (int y = f.min_y; y < f.max_y; y++) {
    for (int x = f.min_x; x < f.max_x; x++) {
      SVec2f_t pix = src_data[(y - y_offset) * f.src_w + x - x_offset];
      if (pix[1] == 0.0f) continue;
      tgt_data[y * f.tgt_w + x] *= pix[0] / pix[1];
    }
  }
}

static void maskRGB(
        SImage_t *tgt,       SVec4f_t *tgt_data,
  const SImage_t *src, const SVec2f_t *src_data,
  int x_offset, int y_offset)
{
  SImage_frame_t f = SImage_setFrame(tgt, src, x_offset, y_offset);
  for (int y = f.min_y; y < f.max_y; y++) {
    for (int x = f.min_x; x < f.max_x; x++) {
      SVec2f_t pix = src_data[(y - y_offset) * f.src_w + x - x_offset];
      if (pix[1] == 0.0f) continue;
      tgt_data[y * f.tgt_w + x] *= pix[0] / pix[1];
    }
  }
}

static void maskSeparateRGB_with_RGB(
  SImage_t *image, int x_offset, int y_offset, const SImage_t *mask)
{
  SVec2f_t *rdata = SImage_dataRed(image);
  SVec2f_t *gdata = SImage_dataGreen(image);
  SVec2f_t *bdata = SImage_dataBlue(image);
  SVec4f_t *mdata = mask->data_rgb;

  SImage_frame_t f = SImage_setFrame(image, mask, x_offset, y_offset);
  for (int y = f.min_y; y < f.max_y; y++) {
    for (int x = f.min_x; x < f.max_x; x++) {
      SVec4f_t pix = mdata[(y - y_offset) * f.src_w + x - x_offset];
      if (pix[3] == 0.0f) continue;
      pix /= pix[3];
      rdata[y * f.tgt_w + x] *= pix[0];
      gdata[y * f.tgt_w + x] *= pix[1];
      bdata[y * f.tgt_w + x] *= pix[2];
    }
  }
}

static void maskWithGray(
  SImage_t *image, int x_offset, int y_offset, const SImage_t *mask)
{
  switch (image->format) {
  case SFmt_Invalid:
  case SFmt_SeparateRGB:
    assert(0 && "Impossible case");
    return;
  case SFmt_Gray:
    maskGray(
      image, image->data_gray,
      mask,  mask->data_gray,
      x_offset, y_offset);
    return;
  case SFmt_RGB:
    maskRGB(
      image, image->data_rgb,
      mask,  mask->data_gray,
      x_offset, y_offset);
    return;
  }
}

void SImage_mask(
  SImage_t *image, int x_offset, int y_offset, const SImage_t *mask)
{
  if (mask->format == SFmt_Invalid) return;

  switch (image->format) {
  case SFmt_Invalid:
    return;
  case SFmt_Gray:
  case SFmt_RGB:
    if (mask->format == SFmt_Gray) {
      maskWithGray(image, x_offset, y_offset, mask);
    } else {
      SImage_t mask2;
      SImage_toFormat_at(&mask2, mask, SFmt_Gray);
      if (mask2.format != SFmt_Invalid)
        maskWithGray(image, x_offset, y_offset, &mask2);
      SImage_deinit(&mask2);
    }
    return;
  case SFmt_SeparateRGB:
    switch (mask->format) {
    case SFmt_Invalid:
      assert(0 && "Impossible case");
      return;
    case SFmt_Gray:
    case SFmt_SeparateRGB:
      maskGray(
        image, SImage_dataRed(image),
        mask,  SImage_dataRed(mask),
        x_offset, y_offset);
      maskGray(
        image, SImage_dataGreen(image),
        mask,  SImage_dataGreen(mask),
        x_offset, y_offset);
      maskGray(
        image, SImage_dataBlue(image),
        mask,  SImage_dataBlue(mask),
        x_offset, y_offset);
      return;
    case SFmt_RGB:
      maskSeparateRGB_with_RGB(image, x_offset, y_offset, mask);
      return;
    }
    return;
  }
}
