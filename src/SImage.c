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
/* Clear */

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
/* Frames */

typedef struct frame {
  int min_x;
  int max_x;
  int min_y;
  int max_y;
  int tgt_w;
  int src_w;
} frame_t;

static frame_t setFrame(
  const SImage_t *tgt,
  const SImage_t *src,
  int x_offset,
  int y_offset)
{
  frame_t f;
  f.min_x = x_offset > 0 ? x_offset : 0;
  f.min_y = y_offset > 0 ? y_offset : 0;
  f.max_x = (int)src->width  + x_offset;
  f.max_y = (int)src->height + y_offset;
  if ((int)tgt->width  < f.max_x) f.max_x = tgt->width;
  if ((int)tgt->height < f.max_y) f.max_y = tgt->height;
  f.tgt_w = tgt->width;
  f.src_w = src->width;
  return f;
}

/* ========================================================================= */
/* SImage_stack */

static void stackGray(
        SImage_t *tgt,       SVec2f_t *tgt_data,
  const SImage_t *src, const SVec2f_t *src_data,
  int x_offset, int y_offset)
{
  frame_t f = setFrame(tgt, src, x_offset, y_offset);
  for (int y = f.min_y; y < f.max_y; y++) {
    for (int x = f.min_x; x < f.max_x; x++) {
      tgt_data[y * f.tgt_w + x] +=
        src_data[(y - y_offset) * f.src_w + x - x_offset];
    }
  }
}

static void stackRGB(
        SImage_t *tgt,       SVec4f_t *tgt_data,
  const SImage_t *src, const SVec4f_t *src_data,
  int x_offset, int y_offset)
{
  frame_t f = setFrame(tgt, src, x_offset, y_offset);
  for (int y = f.min_y; y < f.max_y; y++) {
    for (int x = f.min_x; x < f.max_x; x++) {
      tgt_data[y * f.tgt_w + x] +=
        src_data[(y - y_offset) * f.src_w + x - x_offset];
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
  if (tgt->format == SFmt_Invalid || src->format == SFmt_Invalid) return;

  if (src->format == tgt->format) {
    stackSameFormat(tgt, x_offset, y_offset, src);
  } else {
    SImage_t src2;
    SImage_toFormat_at(&src2, src, tgt->format);
    if (src2.format != SFmt_Invalid) {
      stackSameFormat(tgt, x_offset, y_offset, &src2);
    }
    SImage_deinit(&src2);
  }
}

/* ========================================================================= */
/* SImage_mask */

static void maskGray(
        SImage_t *tgt,       SVec2f_t *tgt_data,
  const SImage_t *src, const SVec2f_t *src_data,
  int x_offset, int y_offset)
{
  frame_t f = setFrame(tgt, src, x_offset, y_offset);
  for (int y = f.min_y; y < f.max_y; y++) {
    for (int x = f.min_x; x < f.max_x; x++) {
      SVec2f_t pix = src_data[(y - y_offset) * f.src_w + x - x_offset];
      if (pix[1] == 0.0f) continue;
      tgt_data[y * f.tgt_w + x] *= pix[0] / pix[1];
    }
  }
}

static void maskRGB(
        SImage_t *tgt,       SVec4f_t *tgt_data,
  const SImage_t *src, const SVec2f_t *src_data,
  int x_offset, int y_offset)
{
  frame_t f = setFrame(tgt, src, x_offset, y_offset);
  for (int y = f.min_y; y < f.max_y; y++) {
    for (int x = f.min_x; x < f.max_x; x++) {
      SVec2f_t pix = src_data[(y - y_offset) * f.src_w + x - x_offset];
      if (pix[1] == 0.0f) continue;
      tgt_data[y * f.tgt_w + x] *= pix[0] / pix[1];
    }
  }
}

static void maskSeparateRGB_with_RGB(
  SImage_t *image, int x_offset, int y_offset, const SImage_t *mask)
{
  SVec2f_t *rdata = SImage_dataRed(image);
  SVec2f_t *gdata = SImage_dataGreen(image);
  SVec2f_t *bdata = SImage_dataBlue(image);
  SVec4f_t *mdata = mask->data_rgb;

  frame_t f = setFrame(image, mask, x_offset, y_offset);
  for (int y = f.min_y; y < f.max_y; y++) {
    for (int x = f.min_x; x < f.max_x; x++) {
      SVec4f_t pix = mdata[(y - y_offset) * f.src_w + x - x_offset];
      if (pix[3] == 0.0f) continue;
      pix /= pix[3];
      rdata[y * f.tgt_w + x] *= pix[0];
      gdata[y * f.tgt_w + x] *= pix[1];
      bdata[y * f.tgt_w + x] *= pix[2];
    }
  }
}

static void maskWithGray(
  SImage_t *image, int x_offset, int y_offset, const SImage_t *mask)
{
  switch (image->format) {
  case SFmt_Invalid:
  case SFmt_SeparateRGB:
    assert(0 && "Impossible case");
    return;
  case SFmt_Gray:
    maskGray(
      image, image->data_gray,
      mask,  mask->data_gray,
      x_offset, y_offset);
    return;
  case SFmt_RGB:
    maskRGB(
      image, image->data_rgb,
      mask,  mask->data_gray,
      x_offset, y_offset);
    return;
  }
}

void SImage_mask(
  SImage_t *image, int x_offset, int y_offset, const SImage_t *mask)
{
  if (mask->format == SFmt_Invalid) return;

  switch (image->format) {
  case SFmt_Invalid:
    return;
  case SFmt_Gray:
  case SFmt_RGB:
    if (mask->format == SFmt_Gray) {
      maskWithGray(image, x_offset, y_offset, mask);
    } else {
      SImage_t mask2;
      SImage_toFormat_at(&mask2, mask, SFmt_Gray);
      if (mask2.format != SFmt_Invalid)
        maskWithGray(image, x_offset, y_offset, &mask2);
      SImage_deinit(&mask2);
    }
    return;
  case SFmt_SeparateRGB:
    switch (mask->format) {
    case SFmt_Invalid:
      assert(0 && "Impossible case");
      return;
    case SFmt_Gray:
    case SFmt_SeparateRGB:
      maskGray(
        image, SImage_dataRed(image),
        mask,  SImage_dataRed(mask),
        x_offset, y_offset);
      maskGray(
        image, SImage_dataGreen(image),
        mask,  SImage_dataGreen(mask),
        x_offset, y_offset);
      maskGray(
        image, SImage_dataBlue(image),
        mask,  SImage_dataBlue(mask),
        x_offset, y_offset);
      return;
    case SFmt_RGB:
      maskSeparateRGB_with_RGB(image, x_offset, y_offset, mask);
      return;
    }
    return;
  }
}

/* ========================================================================= */
/* SImage_add */

static void addGray(
        SImage_t *tgt,       SVec2f_t *tgt_data,
  const SImage_t *src, const SVec2f_t *src_data,
  int x_offset, int y_offset)
{
  frame_t f = setFrame(tgt, src, x_offset, y_offset);
  for (int y = f.min_y; y < f.max_y; y++) {
    for (int x = f.min_x; x < f.max_x; x++) {
      SVec2f_t spix = src_data[(y - y_offset) * f.src_w + x - x_offset];
      if (spix[1] == 0.0f) continue;
      SVec2f_t tpix = tgt_data[y * f.tgt_w + x];
      float v = spix[0] * tpix[1] / spix[1];
      tgt_data[y * f.tgt_w + x][0] += v;
    }
  }
}

static void addRGB(
        SImage_t *tgt,       SVec4f_t *tgt_data,
  const SImage_t *src, const SVec4f_t *src_data,
  int x_offset, int y_offset)
{
  frame_t f = setFrame(tgt, src, x_offset, y_offset);
  for (int y = f.min_y; y < f.max_y; y++) {
    for (int x = f.min_x; x < f.max_x; x++) {
      SVec4f_t spix = src_data[(y - y_offset) * f.src_w + x - x_offset];
      if (spix[3] == 0.0f) continue;
      SVec4f_t tpix = tgt_data[y * f.tgt_w + x];
      spix *= tpix[3] / spix[3];
      spix[3] = 0.0f;
      tgt_data[y * f.tgt_w + x] += spix;
    }
  }
}

static void addSameFormat(
  SImage_t *tgt, int x_offset, int y_offset, const SImage_t *src)
{
  assert(src->format == tgt->format);

  switch (tgt->format) {
  case SFmt_Invalid:
    return;
  case SFmt_Gray:
    addGray(
      tgt, tgt->data_gray,
      src, src->data_gray,
      x_offset, y_offset);
    break;
  case SFmt_RGB:
    addRGB(
      tgt, tgt->data_rgb,
      src, src->data_rgb,
      x_offset, y_offset);
    break;
  case SFmt_SeparateRGB:
    addGray(
      tgt, SImage_dataRed(tgt),
      src, SImage_dataRed(src),
      x_offset, y_offset);
    addGray(
      tgt, SImage_dataGreen(tgt),
      src, SImage_dataGreen(src),
      x_offset, y_offset);
    addGray(
      tgt, SImage_dataBlue(tgt),
      src, SImage_dataBlue(src),
      x_offset, y_offset);
    break;
  }
}

void SImage_add(
  SImage_t *tgt, int x_offset, int y_offset, const SImage_t *src)
{
  if (tgt->format == SFmt_Invalid || src->format == SFmt_Invalid) return;
  
  if (src->format == tgt->format) {
    addSameFormat(tgt, x_offset, y_offset, src);
  } else {
    SImage_t src2;
    SImage_toFormat_at(&src2, src, tgt->format);
    if (src2.format != SFmt_Invalid) {
      addSameFormat(tgt, x_offset, y_offset, &src2);
    }
    SImage_deinit(&src2);
  }
}

/* ========================================================================= */
/* SImage_sub */

static void subGray(
        SImage_t *tgt,       SVec2f_t *tgt_data,
  const SImage_t *src, const SVec2f_t *src_data,
  int x_offset, int y_offset)
{
  frame_t f = setFrame(tgt, src, x_offset, y_offset);
  for (int y = f.min_y; y < f.max_y; y++) {
    for (int x = f.min_x; x < f.max_x; x++) {
      SVec2f_t spix = src_data[(y - y_offset) * f.src_w + x - x_offset];
      if (spix[1] == 0.0f) continue;
      SVec2f_t tpix = tgt_data[y * f.tgt_w + x];
      float v = spix[0] * tpix[1] / spix[1];
      tgt_data[y * f.tgt_w + x][0] -= v;
    }
  }
}

static void subRGB(
        SImage_t *tgt,       SVec4f_t *tgt_data,
  const SImage_t *src, const SVec4f_t *src_data,
  int x_offset, int y_offset)
{
  frame_t f = setFrame(tgt, src, x_offset, y_offset);
  for (int y = f.min_y; y < f.max_y; y++) {
    for (int x = f.min_x; x < f.max_x; x++) {
      SVec4f_t spix = src_data[(y - y_offset) * f.src_w + x - x_offset];
      if (spix[3] == 0.0f) continue;
      SVec4f_t tpix = tgt_data[y * f.tgt_w + x];
      spix *= tpix[3] / spix[3];
      spix[3] = 0.0f;
      tgt_data[y * f.tgt_w + x] -= spix;
    }
  }
}

static void subSameFormat(
  SImage_t *tgt, int x_offset, int y_offset, const SImage_t *src)
{
  assert(src->format == tgt->format);

  switch (tgt->format) {
  case SFmt_Invalid:
    return;
  case SFmt_Gray:
    subGray(
      tgt, tgt->data_gray,
      src, src->data_gray,
      x_offset, y_offset);
    break;
  case SFmt_RGB:
    subRGB(
      tgt, tgt->data_rgb,
      src, src->data_rgb,
      x_offset, y_offset);
    break;
  case SFmt_SeparateRGB:
    subGray(
      tgt, SImage_dataRed(tgt),
      src, SImage_dataRed(src),
      x_offset, y_offset);
    subGray(
      tgt, SImage_dataGreen(tgt),
      src, SImage_dataGreen(src),
      x_offset, y_offset);
    subGray(
      tgt, SImage_dataBlue(tgt),
      src, SImage_dataBlue(src),
      x_offset, y_offset);
    break;
  }
}

void SImage_sub(
  SImage_t *tgt, int x_offset, int y_offset, const SImage_t *src)
{
  if (tgt->format == SFmt_Invalid || src->format == SFmt_Invalid) return;
  
  if (src->format == tgt->format) {
    subSameFormat(tgt, x_offset, y_offset, src);
  } else {
    SImage_t src2;
    SImage_toFormat_at(&src2, src, tgt->format);
    if (src2.format != SFmt_Invalid) {
      subSameFormat(tgt, x_offset, y_offset, &src2);
    }
    SImage_deinit(&src2);
  }
}

/* ========================================================================= */
/* SImage_mul */

static void mulGray(
        SImage_t *tgt,       SVec2f_t *tgt_data,
  const SImage_t *src, const SVec2f_t *src_data,
  int x_offset, int y_offset)
{
  frame_t f = setFrame(tgt, src, x_offset, y_offset);
  for (int y = f.min_y; y < f.max_y; y++) {
    for (int x = f.min_x; x < f.max_x; x++) {
      SVec2f_t spix = src_data[(y - y_offset) * f.src_w + x - x_offset];
      if (spix[1] == 0.0f) continue;
      float v = spix[0] / spix[1];
      tgt_data[y * f.tgt_w + x][0] *= v;
    }
  }
}

static void mulRGB(
        SImage_t *tgt,       SVec4f_t *tgt_data,
  const SImage_t *src, const SVec4f_t *src_data,
  int x_offset, int y_offset)
{
  frame_t f = setFrame(tgt, src, x_offset, y_offset);
  for (int y = f.min_y; y < f.max_y; y++) {
    for (int x = f.min_x; x < f.max_x; x++) {
      SVec4f_t spix = src_data[(y - y_offset) * f.src_w + x - x_offset];
      if (spix[3] == 0.0f) continue;
      spix *= 1.0f / spix[3];
      tgt_data[y * f.tgt_w + x] *= spix;
    }
  }
}

static void mulSameFormat(
  SImage_t *tgt, int x_offset, int y_offset, const SImage_t *src)
{
  assert(src->format == tgt->format);

  switch (tgt->format) {
  case SFmt_Invalid:
    return;
  case SFmt_Gray:
    mulGray(
      tgt, tgt->data_gray,
      src, src->data_gray,
      x_offset, y_offset);
    break;
  case SFmt_RGB:
    mulRGB(
      tgt, tgt->data_rgb,
      src, src->data_rgb,
      x_offset, y_offset);
    break;
  case SFmt_SeparateRGB:
    mulGray(
      tgt, SImage_dataRed(tgt),
      src, SImage_dataRed(src),
      x_offset, y_offset);
    mulGray(
      tgt, SImage_dataGreen(tgt),
      src, SImage_dataGreen(src),
      x_offset, y_offset);
    mulGray(
      tgt, SImage_dataBlue(tgt),
      src, SImage_dataBlue(src),
      x_offset, y_offset);
    break;
  }
}

void SImage_mul(
  SImage_t *tgt, int x_offset, int y_offset, const SImage_t *src)
{
  if (tgt->format == SFmt_Invalid || src->format == SFmt_Invalid) return;
  
  if (src->format == tgt->format) {
    mulSameFormat(tgt, x_offset, y_offset, src);
  } else {
    SImage_t src2;
    SImage_toFormat_at(&src2, src, tgt->format);
    if (src2.format != SFmt_Invalid) {
      mulSameFormat(tgt, x_offset, y_offset, &src2);
    }
    SImage_deinit(&src2);
  }
}

/* ========================================================================= */
/* SImage_div */

static void divGray(
        SImage_t *tgt,       SVec2f_t *tgt_data,
  const SImage_t *src, const SVec2f_t *src_data,
  int x_offset, int y_offset)
{
  frame_t f = setFrame(tgt, src, x_offset, y_offset);
  for (int y = f.min_y; y < f.max_y; y++) {
    for (int x = f.min_x; x < f.max_x; x++) {
      SVec2f_t spix = src_data[(y - y_offset) * f.src_w + x - x_offset];
      if (spix[0] == 0.0f || spix[1] == 0.0f) continue;
      float v = spix[1] / spix[0];
      tgt_data[y * f.tgt_w + x][0] *= v;
    }
  }
}

static void divRGB(
        SImage_t *tgt,       SVec4f_t *tgt_data,
  const SImage_t *src, const SVec4f_t *src_data,
  int x_offset, int y_offset)
{
  frame_t f = setFrame(tgt, src, x_offset, y_offset);
  for (int y = f.min_y; y < f.max_y; y++) {
    for (int x = f.min_x; x < f.max_x; x++) {
      SVec4f_t spix = src_data[(y - y_offset) * f.src_w + x - x_offset];
      if (spix[3] == 0.0f) continue;
      spix *= 1.0f / spix[3];
      tgt_data[y * f.tgt_w + x] /= spix;
    }
  }
}

static void divSameFormat(
  SImage_t *tgt, int x_offset, int y_offset, const SImage_t *src)
{
  assert(src->format == tgt->format);

  switch (tgt->format) {
  case SFmt_Invalid:
    return;
  case SFmt_Gray:
    divGray(
      tgt, tgt->data_gray,
      src, src->data_gray,
      x_offset, y_offset);
    break;
  case SFmt_RGB:
    divRGB(
      tgt, tgt->data_rgb,
      src, src->data_rgb,
      x_offset, y_offset);
    break;
  case SFmt_SeparateRGB:
    divGray(
      tgt, SImage_dataRed(tgt),
      src, SImage_dataRed(src),
      x_offset, y_offset);
    divGray(
      tgt, SImage_dataGreen(tgt),
      src, SImage_dataGreen(src),
      x_offset, y_offset);
    divGray(
      tgt, SImage_dataBlue(tgt),
      src, SImage_dataBlue(src),
      x_offset, y_offset);
    break;
  }
}

void SImage_div(
  SImage_t *tgt, int x_offset, int y_offset, const SImage_t *src)
{
  if (tgt->format == SFmt_Invalid || src->format == SFmt_Invalid) return;
  
  if (src->format == tgt->format) {
    divSameFormat(tgt, x_offset, y_offset, src);
  } else {
    SImage_t src2;
    SImage_toFormat_at(&src2, src, tgt->format);
    if (src2.format != SFmt_Invalid) {
      divSameFormat(tgt, x_offset, y_offset, &src2);
    }
    SImage_deinit(&src2);
  }
}

/* ========================================================================= */
/* SImage_addConst */

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

void SImage_subConst(SImage_t *image, float v) {
  SImage_addConst(image, -v);
}

void SImage_subConstRGB(SImage_t *image, float r, float g, float b) {
  SImage_addConstRGB(image, -r, -g, -b);
}

/* ========================================================================= */
/* SImage_mulConst */

static void mulConstGray(SVec2f_t *data, size_t n, float v) {
  for (size_t i = 0; i < n; i++)
    data[i][0] *= v;
}

static void mulConstRGB(SVec4f_t *data, size_t n, float r, float g, float b) {
  SVec4f_t v = { r, g, b, 1.0f };
  for (size_t i = 0; i < n; i++)
    data[i] *= v;
}

void SImage_mulConst(SImage_t *image, float v) {
  switch (image->format) {
  case SFmt_Invalid:
    return;
  case SFmt_Gray:
    mulConstGray(image->data_gray, image->width * image->height, v);
    break;
  case SFmt_RGB:
    mulConstRGB(image->data_rgb, image->width * image->height, v, v, v);
    break;
  case SFmt_SeparateRGB:
    mulConstGray(SImage_dataRed(image),   image->width * image->height, v);
    mulConstGray(SImage_dataGreen(image), image->width * image->height, v);
    mulConstGray(SImage_dataBlue(image),  image->width * image->height, v);
    break;
  }
}

void SImage_mulConstRGB(SImage_t *image, float r, float g, float b) {
  switch (image->format) {
  case SFmt_Invalid:
    return;
  case SFmt_Gray:
    mulConstGray(
      image->data_gray, image->width * image->height, (r + g + b) / 3.0f);
    break;
  case SFmt_RGB:
    mulConstRGB(image->data_rgb, image->width * image->height, r, g, b);
    break;
  case SFmt_SeparateRGB:
    mulConstGray(SImage_dataRed(image),   image->width * image->height, r);
    mulConstGray(SImage_dataGreen(image), image->width * image->height, g);
    mulConstGray(SImage_dataBlue(image),  image->width * image->height, b);
    break;
  }
}

void SImage_divConst(SImage_t *image, float v) {
  SImage_mulConst(image, 1.0f / v);
}

void SImage_divConstRGB(SImage_t *image, float r, float g, float b) {
  SImage_mulConstRGB(image, 1.0f / r, 1.0f / g, 1.0f / b);
}

/* ========================================================================= */
/* SImage_mulWeight */

static void mulWeightGray(SVec2f_t *data, size_t n, float v) {
  for (size_t i = 0; i < n; i++)
    data[i] *= v;
}

static void mulWeightRGB(SVec4f_t *data, size_t n, float v) {
  for (size_t i = 0; i < n; i++)
    data[i] *= v;
}

void SImage_mulWeight(SImage_t *image, float v) {
  switch (image->format) {
  case SFmt_Invalid:
    return;
  case SFmt_Gray:
    mulWeightGray(image->data_gray, image->width * image->height, v);
    break;
  case SFmt_RGB:
    mulWeightRGB(image->data_rgb, image->width * image->height, v);
    break;
  case SFmt_SeparateRGB:
    mulWeightGray(SImage_dataRed(image),   image->width * image->height, v);
    mulWeightGray(SImage_dataGreen(image), image->width * image->height, v);
    mulWeightGray(SImage_dataBlue(image),  image->width * image->height, v);
    break;
  }
}

void SImage_mulWeightRGB(SImage_t *image, float r, float g, float b) {
  if (image->format != SFmt_SeparateRGB) return;

  mulWeightGray(SImage_dataRed(image),   image->width * image->height, r);
  mulWeightGray(SImage_dataGreen(image), image->width * image->height, g);
  mulWeightGray(SImage_dataBlue(image),  image->width * image->height, b);
}

/* ========================================================================= */
/* SImage_mulWeight */

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
