/* This file is part of Spica, released under MIT license.
 * See LICENSE for details.
 */

/* Author: Piotr Polesiuk, 2023 */

#include "SImage.h"

static SGrayGradient_t grayGradient(const SGradient_t *grad) {
  if (grad->is_color) {
    return (SGrayGradient_t) {
      .bias = (grad->red.bias + grad->green.bias + grad->blue.bias) / 3.0f,
      .coef = (grad->red.coef + grad->green.coef + grad->blue.coef) / 3.0f
    };
  } else {
    return grad->gray;
  }
}

static SGrayGradient_t redGradient(const SGradient_t *grad) {
  return grad->is_color ? grad->red : grad->gray;
}

static SGrayGradient_t greenGradient(const SGradient_t *grad) {
  return grad->is_color ? grad->green : grad->gray;
}

static SGrayGradient_t blueGradient(const SGradient_t *grad) {
  return grad->is_color ? grad->blue : grad->gray;
}

static void subGrayGradient(
  SVec2f_t *data, size_t width, size_t height, SGrayGradient_t grad)
{
  for (size_t y = 0; y < height; y++) {
    for (size_t x = 0; x < width; x++) {
      size_t i = y*width + x;
      data[i][0] -= data[i][1] * SGrayGradient_value(grad, x, y);
    }
  }
}

static void subRGBGradient(
  SVec4f_t *data, size_t width, size_t height,
  SGrayGradient_t rg, SGrayGradient_t gg, SGrayGradient_t bg)
{
  for (size_t y = 0; y < height; y++) {
    for (size_t x = 0; x < width; x++) {
      size_t i = y*width + x;
      float w = data[i][3];
      data[i][0] -= w * SGrayGradient_value(rg, x, y);
      data[i][1] -= w * SGrayGradient_value(gg, x, y);
      data[i][2] -= w * SGrayGradient_value(bg, x, y);
    }
  }
}

void SImage_subGradient(SImage_t *image, const SGradient_t *grad) {
  switch (image->format) {
  case SFmt_Invalid:
    return;
  case SFmt_Gray:
    subGrayGradient(image->data_gray, image->width, image->height,
      grayGradient(grad));
    break;
  case SFmt_RGB:
    subRGBGradient(image->data_rgb, image->width, image->height,
      redGradient(grad), greenGradient(grad), blueGradient(grad));
    break;
  case SFmt_SeparateRGB:
    subGrayGradient(SImage_dataRed(image),   image->width, image->height,
      redGradient(grad));
    subGrayGradient(SImage_dataGreen(image), image->width, image->height,
      greenGradient(grad));
    subGrayGradient(SImage_dataBlue(image),  image->width, image->height,
      blueGradient(grad));
    break;
  }
}
