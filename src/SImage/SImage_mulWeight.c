/* This file is part of Spica, released under MIT license.
 * See LICENSE for details.
 */

/* Author: Piotr Polesiuk, 2022 */

#include "SImage.h"

static void mulWeightGray(SVec2f_t *data, size_t n, float v) {
  for (size_t i = 0; i < n; i++)
    data[i] *= v;
}

static void mulWeightRGB(SVec4f_t *data, size_t n, float v) {
  for (size_t i = 0; i < n; i++)
    data[i] *= v;
}

void SImage_mulWeight(SImage_t *image, float v) {
  switch (image->format) {
  case SFmt_Invalid:
    return;
  case SFmt_Gray:
    mulWeightGray(image->data_gray, image->width * image->height, v);
    break;
  case SFmt_RGB:
    mulWeightRGB(image->data_rgb, image->width * image->height, v);
    break;
  case SFmt_SeparateRGB:
    mulWeightGray(SImage_dataRed(image),   image->width * image->height, v);
    mulWeightGray(SImage_dataGreen(image), image->width * image->height, v);
    mulWeightGray(SImage_dataBlue(image),  image->width * image->height, v);
    break;
  }
}

void SImage_mulWeightRGB(SImage_t *image, float r, float g, float b) {
  if (image->format != SFmt_SeparateRGB) return;

  mulWeightGray(SImage_dataRed(image),   image->width * image->height, r);
  mulWeightGray(SImage_dataGreen(image), image->width * image->height, g);
  mulWeightGray(SImage_dataBlue(image),  image->width * image->height, b);
}
