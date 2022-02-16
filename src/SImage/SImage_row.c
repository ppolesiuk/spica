/* This file is part of Spica, released under MIT license.
 * See LICENSE for details.
 */

/* Author: Piotr Polesiuk, 2022 */

#include "SImage.h"

#include <assert.h>

void *SImage_row(const SImage_t *image, unsigned y) {
  switch (image->format) {
  case SFmt_Gray:
    return image->data_gray + y * image->width;
  case SFmt_RGB:
    return image->data_rgb + y * image->width;
  case SFmt_SeparateRGB:
  case SFmt_Invalid:
    return NULL;
  }
  assert(0 && "Impossible case");
}

SVec2f_t *SImage_rowRed(const SImage_t *image, unsigned y) {
  switch (image->format) {
  case SFmt_Gray:
    return image->data_gray + y * image->width;
  case SFmt_SeparateRGB:
    return image->data_red + y * image->width;
  case SFmt_RGB:
  case SFmt_Invalid:
    return NULL;
  }
  assert(0 && "Impossible case");
}

SVec2f_t *SImage_rowGreen(const SImage_t *image, unsigned y) {
  switch (image->format) {
  case SFmt_Gray:
    return image->data_gray + y * image->width;
  case SFmt_SeparateRGB:
    return image->data_red + (image->height + y) * image->width;
  case SFmt_RGB:
  case SFmt_Invalid:
    return NULL;
  }
  assert(0 && "Impossible case");
}

SVec2f_t *SImage_rowBlue(const SImage_t *image, unsigned y) {
  switch (image->format) {
  case SFmt_Gray:
    return image->data_gray + y * image->width;
  case SFmt_SeparateRGB:
    return image->data_red + (2*image->height + y) * image->width;
  case SFmt_RGB:
  case SFmt_Invalid:
    return NULL;
  }
  assert(0 && "Impossible case");
}
