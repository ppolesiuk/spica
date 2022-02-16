/* This file is part of Spica, released under MIT license.
 * See LICENSE for details.
 */

/* Author: Piotr Polesiuk, 2022 */

#include "SImage.h"

#include <string.h>

void SImage_clear(SImage_t *image) {
  if (image->format == SFmt_Invalid) return;

  memset(image->data, 0, SImage_dataSize(image));
}

static void clearWithVec2f(SVec2f_t *data, SVec2f_t v, size_t size) {
  for (size_t i = 0; i < size; i++) data[i] = v;
}

static void clearWithVec4f(SVec4f_t *data, SVec4f_t v, size_t size) {
  for (size_t i = 0; i < size; i++) data[i] = v;
}

void SImage_clearBlack(SImage_t *image) {
  switch (image->format) {
  case SFmt_Invalid:
    return;
  case SFmt_Gray:
    clearWithVec2f(
      image->data_gray,
      SVec2f(0.0f, 1.0f),
      image->width * image->height);
    break;
  case SFmt_RGB:
    clearWithVec4f(
      image->data_rgb,
      SVec4f(0.0f, 0.0f, 0.0f, 1.0f),
      image->width * image->height);
    break;
  case SFmt_SeparateRGB:
    clearWithVec2f(
      image->data_red,
      SVec2f(0.0f, 1.0f),
      3 * image->width * image->height);
    break;
  }
}

void SImage_clearWhite(SImage_t *image) {
  switch (image->format) {
  case SFmt_Invalid:
    return;
  case SFmt_Gray:
    clearWithVec2f(
      image->data_gray,
      SVec2f(1.0f, 1.0f),
      image->width * image->height);
    break;
  case SFmt_RGB:
    clearWithVec4f(
      image->data_rgb,
      SVec4f(1.0f, 1.0f, 1.0f, 1.0f),
      image->width * image->height);
    break;
  case SFmt_SeparateRGB:
    clearWithVec2f(
      image->data_red,
      SVec2f(1.0f, 1.0f),
      3 * image->width * image->height);
    break;
  }
}
