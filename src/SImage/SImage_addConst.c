/* This file is part of Spica, released under MIT license.
 * See LICENSE for details.
 */

/* Author: Piotr Polesiuk, 2022 */

#include "SImage.h"

static void addConstGray(SVec2f_t *data, size_t n, float v) {
  for (size_t i = 0; i < n; i++)
    data[i][0] += v * data[i][1];
}

static void addConstRGB(SVec4f_t *data, size_t n, float r, float g, float b) {
  SVec4f_t v = { r, g, b, 0.0f };
  for (size_t i = 0; i < n; i++)
    data[i] += v * data[i][3];
}

void SImage_addConst(SImage_t *image, float v) {
  switch (image->format) {
  case SFmt_Invalid:
    return;
  case SFmt_Gray:
    addConstGray(image->data_gray, image->width * image->height, v);
    break;
  case SFmt_RGB:
    addConstRGB(image->data_rgb, image->width * image->height, v, v, v);
    break;
  case SFmt_SeparateRGB:
    addConstGray(SImage_dataRed(image),   image->width * image->height, v);
    addConstGray(SImage_dataGreen(image), image->width * image->height, v);
    addConstGray(SImage_dataBlue(image),  image->width * image->height, v);
    break;
  }
}

void SImage_addConstRGB(SImage_t *image, float r, float g, float b) {
  switch (image->format) {
  case SFmt_Invalid:
    return;
  case SFmt_Gray:
    addConstGray(
      image->data_gray, image->width * image->height, (r + g + b) / 3.0f);
    break;
  case SFmt_RGB:
    addConstRGB(image->data_rgb, image->width * image->height, r, g, b);
    break;
  case SFmt_SeparateRGB:
    addConstGray(SImage_dataRed(image),   image->width * image->height, r);
    addConstGray(SImage_dataGreen(image), image->width * image->height, g);
    addConstGray(SImage_dataBlue(image),  image->width * image->height, b);
    break;
  }
}
