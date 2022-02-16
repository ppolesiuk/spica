/* This file is part of Spica, released under MIT license.
 * See LICENSE for details.
 */

/* Author: Piotr Polesiuk, 2022 */

#include "SImage.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

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
