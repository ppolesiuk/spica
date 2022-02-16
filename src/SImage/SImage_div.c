/* This file is part of Spica, released under MIT license.
 * See LICENSE for details.
 */

/* Author: Piotr Polesiuk, 2022 */

#include "SImage.h"
#include "SImage_frame.h"

#include <assert.h>

static void divGray(
        SImage_t *tgt,       SVec2f_t *tgt_data,
  const SImage_t *src, const SVec2f_t *src_data,
  int x_offset, int y_offset)
{
  SImage_frame_t f = SImage_setFrame(tgt, src, x_offset, y_offset);
  for (int y = f.min_y; y < f.max_y; y++) {
    for (int x = f.min_x; x < f.max_x; x++) {
      SVec2f_t spix = src_data[(y - y_offset) * f.src_w + x - x_offset];
      if (spix[0] == 0.0f || spix[1] == 0.0f) continue;
      float v = spix[1] / spix[0];
      tgt_data[y * f.tgt_w + x][0] *= v;
    }
  }
}

static void divRGB(
        SImage_t *tgt,       SVec4f_t *tgt_data,
  const SImage_t *src, const SVec4f_t *src_data,
  int x_offset, int y_offset)
{
  SImage_frame_t f = SImage_setFrame(tgt, src, x_offset, y_offset);
  for (int y = f.min_y; y < f.max_y; y++) {
    for (int x = f.min_x; x < f.max_x; x++) {
      SVec4f_t spix = src_data[(y - y_offset) * f.src_w + x - x_offset];
      if (spix[3] == 0.0f) continue;
      spix *= 1.0f / spix[3];
      tgt_data[y * f.tgt_w + x] /= spix;
    }
  }
}

static void divSameFormat(
  SImage_t *tgt, int x_offset, int y_offset, const SImage_t *src)
{
  assert(src->format == tgt->format);

  switch (tgt->format) {
  case SFmt_Invalid:
    return;
  case SFmt_Gray:
    divGray(
      tgt, tgt->data_gray,
      src, src->data_gray,
      x_offset, y_offset);
    break;
  case SFmt_RGB:
    divRGB(
      tgt, tgt->data_rgb,
      src, src->data_rgb,
      x_offset, y_offset);
    break;
  case SFmt_SeparateRGB:
    divGray(
      tgt, SImage_dataRed(tgt),
      src, SImage_dataRed(src),
      x_offset, y_offset);
    divGray(
      tgt, SImage_dataGreen(tgt),
      src, SImage_dataGreen(src),
      x_offset, y_offset);
    divGray(
      tgt, SImage_dataBlue(tgt),
      src, SImage_dataBlue(src),
      x_offset, y_offset);
    break;
  }
}

void SImage_div(
  SImage_t *tgt, int x_offset, int y_offset, const SImage_t *src)
{
  if (tgt->format == SFmt_Invalid || src->format == SFmt_Invalid) return;
  
  if (src->format == tgt->format) {
    divSameFormat(tgt, x_offset, y_offset, src);
  } else {
    SImage_t src2;
    SImage_toFormat_at(&src2, src, tgt->format);
    if (src2.format != SFmt_Invalid) {
      divSameFormat(tgt, x_offset, y_offset, &src2);
    }
    SImage_deinit(&src2);
  }
}
