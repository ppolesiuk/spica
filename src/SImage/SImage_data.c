/* This file is part of Spica, released under MIT license.
 * See LICENSE for details.
 */

/* Author: Piotr Polesiuk, 2022 */

#include "SImage.h"

#include <assert.h>

size_t SImage_dataSize(const SImage_t *image) {
  switch (image->format) {
  case SFmt_Invalid:
    return 0;
  case SFmt_Gray:
    return image->width * image->height * sizeof(SVec2f_t);
  case SFmt_RGB:
    return image->width * image->height * sizeof(SVec4f_t);
  case SFmt_SeparateRGB:
    return image->width * image->height * sizeof(SVec2f_t) * 3;
  }
  assert(0 && "Impossible case");
}

SVec2f_t *SImage_dataRed(const SImage_t *image) {
  return SImage_rowRed(image, 0);
}

SVec2f_t *SImage_dataGreen(const SImage_t *image) {
  return SImage_rowGreen(image, 0);
}

SVec2f_t *SImage_dataBlue(const SImage_t *image) {
  return SImage_rowBlue(image, 0);
}
