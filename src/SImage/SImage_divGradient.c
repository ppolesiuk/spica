/* This file is part of Spica, released under MIT license.
 * See LICENSE for details.
 */

/* Author: Piotr Polesiuk, 2025 */

#include "SImage.h"

static void divGrayGradient(
  SVec2f_t *data, size_t width, size_t height, SGrayGradient_t grad)
{
  for (size_t y = 0; y < height; y++) {
    for (size_t x = 0; x < width; x++) {
      size_t i = y*width + x;
      data[i][0] /= SGrayGradient_value(grad, x, y);
    }
  }
}

static void divRGBGradient(
  SVec4f_t *data, size_t width, size_t height,
  SGrayGradient_t rg, SGrayGradient_t gg, SGrayGradient_t bg)
{
  for (size_t y = 0; y < height; y++) {
    for (size_t x = 0; x < width; x++) {
      size_t i = y*width + x;
      data[i][0] /= SGrayGradient_value(rg, x, y);
      data[i][1] /= SGrayGradient_value(gg, x, y);
      data[i][2] /= SGrayGradient_value(bg, x, y);
    }
  }
}

void SImage_divGradient(SImage_t *image, const SGradient_t *grad) {
  switch (image->format) {
  case SFmt_Invalid:
    return;
  case SFmt_Gray:
    divGrayGradient(
      (SVec2f_t*)image->data, image->width, image->height,
      SGradient_grayGradient(grad));
    break;
  case SFmt_RGB:
    divRGBGradient(
      (SVec4f_t*)image->data, image->width, image->height,
      SGradient_redGradient(grad),
      SGradient_greenGradient(grad),
      SGradient_blueGradient(grad));
    break;
  case SFmt_SeparateRGB:
    divGrayGradient(
      SImage_dataRed(image),   image->width, image->height,
      SGradient_redGradient(grad));
    divGrayGradient(
      SImage_dataGreen(image), image->width, image->height,
      SGradient_greenGradient(grad));
    divGrayGradient(
      SImage_dataBlue(image),  image->width, image->height,
      SGradient_blueGradient(grad));
    break;
  }
}
