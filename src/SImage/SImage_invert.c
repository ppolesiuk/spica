/* This file is part of Spica, released under MIT license.
 * See LICENSE for details.
 */

/* Author: Piotr Polesiuk, 2022 */

#include "SImage.h"

static void invertGray(SVec2f_t *data, size_t n) {
  for (size_t i = 0; i < n; i++) {
    SVec2f_t pix = data[i];
    if (pix[0] == 0.0f) continue;
    data[i][0] = pix[1] * pix[1] / pix[0];
  }
}

static void invertRGB(SVec4f_t *data, size_t n) {
  for (size_t i = 0; i < n; i++) {
    SVec4f_t pix = data[i];
    float w = pix[3];
    if (w == 0.0f) continue;
    pix = pix[3] * pix[3] / pix;
    pix[3] = w;
    data[i] = pix;
  }
}

void SImage_invert(SImage_t *image) {
  switch (image->format) {
  case SFmt_Invalid:
    return;
  case SFmt_Gray:
    invertGray(image->data_gray, image->width * image->height);
    break;
  case SFmt_RGB:
    invertRGB(image->data_rgb, image->width * image->height);
    break;
  case SFmt_SeparateRGB:
    invertGray(SImage_dataRed(image),   image->width * image->height);
    invertGray(SImage_dataGreen(image), image->width * image->height);
    invertGray(SImage_dataBlue(image),  image->width * image->height);
    break;
  }
}
