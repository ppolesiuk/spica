/* This file is part of Spica, released under MIT license.
 * See LICENSE for details.
 */

/* Author: Piotr Polesiuk, 2022 */

#include "SImage.h"

#include <assert.h>
#include <stdlib.h>

#define MAX_IMAGE_SIZE 65535

void SImage_init(
  SImage_t      *image,
  unsigned       width,
  unsigned       height,
  SImageFormat_t format)
{
  if (width > MAX_IMAGE_SIZE || height > MAX_IMAGE_SIZE) {
    image->width = 0;
    image->height = 0;
    image->format = SFmt_Invalid;
    image->data = NULL;
    return;
  }

  image->width  = width;
  image->height = height;
  image->format = format;
  image->data   = NULL;

  switch (format) {
  case SFmt_Invalid:
    break;
  case SFmt_Gray:
    image->data_gray = malloc(width * height * sizeof(SVec2f_t));
    break;
  case SFmt_RGB:
    image->data_rgb  = malloc(width * height * sizeof(SVec4f_t));
    break;
  case SFmt_SeparateRGB:
    image->data_red  = malloc(width * height * sizeof(SVec2f_t) * 3);
    break;
  }

  if (image->data == NULL) {
    image->width  = 0;
    image->height = 0;
    image->format = SFmt_Invalid;
  }
}

void SImage_deinit(SImage_t *image) {
  if (image->data) free(image->data);
}

SImage_t *SImage_alloc(
  unsigned       width,
  unsigned       height,
  SImageFormat_t format)
{
  SImage_t *image = malloc(sizeof(SImage_t));
  if (image == NULL) return NULL;

  SImage_init(image, width, height, format);
  return image;
}

void SImage_free(SImage_t *image) {
  if (image == NULL) return;
  SImage_deinit(image);
  free(image);
}

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
