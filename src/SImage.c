/* This file is part of Spica, released under MIT license.
 * See LICENSE for details.
 */

/* Author: Piotr Polesiuk, 2022 */

#include "SImage.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

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

/* ========================================================================= */

static void convert_RGB_to_Gray(
  unsigned size, SVec2f_t *dst, const SVec4f_t *src, SVec4f_t weight)
{
  for (unsigned i = 0; i < size; i++) {
    SVec4f_t spix = src[i] * weight;
    SVec2f_t dpix = { spix[0] + spix[1] + spix[2], spix[3] };
    dst[i] = dpix;
  }
}

static void convert_SeparateRGB_to_Gray(
  unsigned size,
  SVec2f_t *dst,
  const SVec2f_t *red, const SVec2f_t *green, const SVec2f_t *blue)
{
  for (unsigned i = 0; i < size; i++) {
    dst[i] = (red[i] + green[i] + blue[i]) / 3.0f;
  }
}

static void convert_Gray_to_RGB(
  unsigned size, SVec4f_t *dst, const SVec2f_t *src)
{
  for (unsigned i = 0; i < size; i++) {
    SVec4f_t pix = { src[i][0], src[i][0], src[i][0], src[i][1] };
    dst[i] = pix;
  }
}

static void convert_SeparateRGB_to_RGB(
  unsigned size,
  SVec4f_t *dst,
  const SVec2f_t *red, const SVec2f_t *green, const SVec2f_t *blue)
{
  for (unsigned i = 0; i < size; i++) {
    SVec2f_t r = red[i];
    SVec2f_t g = green[i];
    SVec2f_t b = blue[i];
    float weight = (r[1] + g[1] + b[1]) / 3.0f;
    SVec4f_t pix = {
      (r[1] == 0.0f ? 0.0f : (r[0] * weight / r[1])),
      (g[1] == 0.0f ? 0.0f : (g[0] * weight / g[1])),
      (b[1] == 0.0f ? 0.0f : (b[0] * weight / b[1])),
      weight };
    dst[i] = pix;
  }
}

static void convertToGray(SImage_t *dst, const SImage_t *src) {
  switch (src->format) {
  case SFmt_Invalid:
    assert(0 && "Impossible case");
    return;
  case SFmt_Gray:
    memcpy(dst->data, src->data, SImage_dataSize(src));
    break;
  case SFmt_RGB:
    convert_RGB_to_Gray(
      src->width * src->height,
      dst->data_gray,
      src->data_rgb,
      SVec4f(1.0f/3.0f, 1.0f/3.0f, 1.0f/3.0f, 1.0f));
    break;
  case SFmt_SeparateRGB:
    convert_SeparateRGB_to_Gray(
      src->width * src->height,
      dst->data_gray,
      SImage_dataRed(src),
      SImage_dataGreen(src),
      SImage_dataBlue(src));
    break;
  }
}

static void convertToRGB(SImage_t *dst, const SImage_t *src) {
  switch (src->format) {
  case SFmt_Invalid:
    assert(0 && "Impossible case");
    return;
  case SFmt_Gray:
    convert_Gray_to_RGB(
      src->width * src->height,
      dst->data_rgb,
      src->data_gray);
    break;
  case SFmt_RGB:
    memcpy(dst->data, src->data, SImage_dataSize(src));
    break;
  case SFmt_SeparateRGB:
    convert_SeparateRGB_to_RGB(
      src->width * src->height,
      dst->data_rgb,
      SImage_dataRed(src),
      SImage_dataGreen(src),
      SImage_dataBlue(src));
    break;
  }
}

static void convertToSeparateRGB(SImage_t *dst, const SImage_t *src) {
  switch (src->format) {
  case SFmt_Invalid:
    assert(0 && "Impossible case");
    return;
  case SFmt_Gray:
    memcpy(SImage_dataRed(dst),   src->data, SImage_dataSize(src));
    memcpy(SImage_dataGreen(dst), src->data, SImage_dataSize(src));
    memcpy(SImage_dataBlue(dst),  src->data, SImage_dataSize(src));
    break;
  case SFmt_RGB:
    convert_RGB_to_Gray(
      src->width * src->height,
      SImage_dataRed(dst),
      src->data_rgb,
      SVec4f(1.0f, 0.0f, 0.0f, 1.0f));
    convert_RGB_to_Gray(
      src->width * src->height,
      SImage_dataGreen(dst),
      src->data_rgb,
      SVec4f(0.0f, 1.0f, 0.0f, 1.0f));
    convert_RGB_to_Gray(
      src->width * src->height,
      SImage_dataBlue(dst),
      src->data_rgb,
      SVec4f(0.0f, 0.0f, 1.0f, 1.0f));
    break;
  case SFmt_SeparateRGB:
    memcpy(dst->data, src->data, SImage_dataSize(src));
    break;
  }
}

void SImage_toFormat_at(
  SImage_t       *dst,
  const SImage_t *image,
  SImageFormat_t  format)
{
  if (image->format == SFmt_Invalid) format = SFmt_Invalid;
  SImage_init(dst, image->width, image->height, image->format);

  switch (dst->format) {
  case SFmt_Invalid:
    return;
  case SFmt_Gray:
    convertToGray(dst, image);
    return;
  case SFmt_RGB:
    convertToRGB(dst, image);
    return;
  case SFmt_SeparateRGB:
    convertToSeparateRGB(dst, image);
    return;
  }
}

SImage_t *SImage_toFormat(const SImage_t *image, SImageFormat_t format) {
  SImage_t *dst = malloc(sizeof(SImage_t));
  if (dst == NULL) return NULL;
  SImage_toFormat_at(dst, image, format);
  return dst;
}
/* ========================================================================= */

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

/* ========================================================================= */
/* SImage_stack */

static void stackGray(
        SImage_t *tgt,       SVec2f_t *tgt_data,
  const SImage_t *src, const SVec2f_t *src_data,
  int x_offset, int y_offset)
{
  int min_x = x_offset > 0 ? x_offset : 0;
  int min_y = y_offset > 0 ? y_offset : 0;
  int max_x = (int)src->width  + x_offset;
  int max_y = (int)src->height + y_offset;
  if ((int)tgt->width  < max_x) max_x = tgt->width;
  if ((int)tgt->height < max_y) max_y = tgt->height;
  int tgt_w = tgt->width;
  int src_w = src->width;

  for (int y = min_y; y < max_y; y++) {
    for (int x = min_x; x < max_x; x++) {
      tgt_data[y * tgt_w + x] +=
        src_data[(y - y_offset) * src_w + x - x_offset];
    }
  }
}

static void stackRGB(
        SImage_t *tgt,       SVec4f_t *tgt_data,
  const SImage_t *src, const SVec4f_t *src_data,
  int x_offset, int y_offset)
{
  int min_x = x_offset > 0 ? x_offset : 0;
  int min_y = y_offset > 0 ? y_offset : 0;
  int max_x = (int)src->width  + x_offset;
  int max_y = (int)src->height + y_offset;
  if ((int)tgt->width  < max_x) max_x = tgt->width;
  if ((int)tgt->height < max_y) max_y = tgt->height;
  int tgt_w = tgt->width;
  int src_w = src->width;

  for (int y = min_y; y < max_y; y++) {
    for (int x = min_x; x < max_x; x++) {
      tgt_data[y * tgt_w + x] +=
        src_data[(y - y_offset) * src_w + x - x_offset];
    }
  }
}

static void stackSameFormat(
  SImage_t *tgt, int x_offset, int y_offset, const SImage_t *src)
{
  assert(src->format == tgt->format);

  switch (tgt->format) {
  case SFmt_Invalid:
    return;
  case SFmt_Gray:
    stackGray(
      tgt, tgt->data_gray,
      src, src->data_gray,
      x_offset, y_offset);
    break;
  case SFmt_RGB:
    stackRGB(
      tgt, tgt->data_rgb,
      src, src->data_rgb,
      x_offset, y_offset);
    break;
  case SFmt_SeparateRGB:
    stackGray(
      tgt, SImage_dataRed(tgt),
      src, SImage_dataRed(src),
      x_offset, y_offset);
    stackGray(
      tgt, SImage_dataGreen(tgt),
      src, SImage_dataGreen(src),
      x_offset, y_offset);
    stackGray(
      tgt, SImage_dataBlue(tgt),
      src, SImage_dataBlue(src),
      x_offset, y_offset);
    break;
  }
}

void SImage_stack(
  SImage_t *tgt, int x_offset, int y_offset, const SImage_t *src)
{
  if (tgt->format == SFmt_Invalid || src->format == SFmt_Invalid) {
    return;
  }

  if (src->format == tgt->format) {
    stackSameFormat(tgt, x_offset, y_offset, src);
  } else {
    SImage_t *src2 = SImage_toFormat(src, tgt->format);
    if (src2->format != SFmt_Invalid) {
      stackSameFormat(tgt, x_offset, y_offset, src2);
    }
    SImage_free(src2);
  }
}
