/* This file is part of Spica, released under MIT license.
 * See LICENSE for details.
 */

/* Author: Piotr Polesiuk, 2025 */

#include "SImage.h"

static void negateGray(SVec2f_t *data, size_t n) {
  for (size_t i = 0; i < n; i++) {
    data[i][0] = -data[i][0];
  }
}

static void negateRGB(SVec4f_t *data, size_t n) {
  for (size_t i = 0; i < n; i++) {
    SVec4f_t pix  = data[i];
    SVec4f_t npix = -pix;
    npix[3] = pix[3];
    data[i] = npix;
  }
}

void SImage_negate(SImage_t *image) {
  switch (image->format) {
  case SFmt_Invalid:
    return;
  case SFmt_Gray:
    negateGray(image->data_gray, image->width * image->height);
    break;
  case SFmt_RGB:
    negateRGB(image->data_rgb, image->width * image->height);
    break;
  case SFmt_SeparateRGB:
    negateGray(SImage_dataRed(image),   image->width * image->height);
    negateGray(SImage_dataGreen(image), image->width * image->height);
    negateGray(SImage_dataBlue(image),  image->width * image->height);
    break;
  }
}
