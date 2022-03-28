/* This file is part of Spica, released under MIT license.
 * See LICENSE for details.
 */

/* Author: Piotr Polesiuk, 2022 */

#include "SImage.h"

#include <assert.h>

static inline SVec2f_t rgb2gray(SVec4f_t rgb) {
  return SVec2f((rgb[0] + rgb[1] + rgb[2]) / 3.0f, rgb[3]);
}

static inline SVec2f_t separate2gray(SVec2f_t r, SVec2f_t g, SVec2f_t b) {
  return (r + g + b) / 3.0f;
}

static inline SVec4f_t gray2rgb(SVec2f_t gray) {
  return SVec4f(gray[0], gray[0], gray[0], gray[1]);
}

static inline SVec2f_t rgb2red(SVec4f_t rgb) {
  return SVec2f(rgb[0], rgb[3]);
}

static inline SVec2f_t rgb2green(SVec4f_t rgb) {
  return SVec2f(rgb[1], rgb[3]);
}

static inline SVec2f_t rgb2blue(SVec4f_t rgb) {
  return SVec2f(rgb[2], rgb[3]);
}

static inline SVec4f_t separate2rgb(SVec2f_t r, SVec2f_t g, SVec2f_t b) {
  float weight = (r[1] + g[1] + b[1]) / 3.0f;
  return SVec4f(
    (r[1] == 0.0f ? 0.0f : (r[0] * weight / r[1])),
    (g[1] == 0.0f ? 0.0f : (g[0] * weight / g[1])),
    (b[1] == 0.0f ? 0.0f : (b[0] * weight / b[1])),
    weight);
}

SVec2f_t SImage_pixelGray(const SImage_t *image, int x, int y) {
  if (x < 0 || y < 0 || x >= (int)image->width || y >= (int)image->height)
    return SVec2f(0.0f, 0.0f);

  int w = image->width;
  int h = image->height;

  switch (image->format) {
  case SFmt_Invalid:
    return SVec2f(0.0f, 0.0f);
  case SFmt_Gray:
    return image->data_gray[w * y + x];
  case SFmt_RGB:
    return rgb2gray(image->data_rgb[w * y + x]);
  case SFmt_SeparateRGB:
    return separate2gray(
      image->data_red[w * y + x],
      image->data_red[w * (y+h) + x],
      image->data_red[w * (y+2*h) + x]);
  }
  assert(0 && "Impossible case");
}

SVec4f_t SImage_pixelRGB(const SImage_t *image, int x, int y) {
  if (x < 0 || y < 0 || x >= (int)image->width || y >= (int)image->height)
    return SVec4f(0.0f, 0.0f, 0.0f, 0.0f);

  int w = image->width;
  int h = image->height;

  switch (image->format) {
  case SFmt_Invalid:
    return SVec4f(0.0f, 0.0f, 0.0f, 0.0f);
  case SFmt_Gray:
    return gray2rgb(image->data_gray[w * y + x]);
  case SFmt_RGB:
    return image->data_rgb[w * y + x];
  case SFmt_SeparateRGB:
    return separate2rgb(
      image->data_red[w * y + x],
      image->data_red[w * (y+h) + x],
      image->data_red[w * (y+2*h) + x]);
  }
  assert(0 && "Impossible case");
}

SVec2f_t SImage_pixelRed(const SImage_t *image, int x, int y) {
  if (x < 0 || y < 0 || x >= (int)image->width || y >= (int)image->height)
    return SVec2f(0.0f, 0.0f);

  int w = image->width;

  switch (image->format) {
  case SFmt_Invalid:
    return SVec2f(0.0f, 0.0f);
  case SFmt_Gray:
    return image->data_gray[w * y + x];
  case SFmt_RGB:
    return rgb2red(image->data_rgb[w * y + x]);
  case SFmt_SeparateRGB:
    return image->data_red[w * y + x];
  }
  assert(0 && "Impossible case");
}

SVec2f_t SImage_pixelGreen(const SImage_t *image, int x, int y) {
  if (x < 0 || y < 0 || x >= (int)image->width || y >= (int)image->height)
    return SVec2f(0.0f, 0.0f);

  int w = image->width;
  int h = image->height;

  switch (image->format) {
  case SFmt_Invalid:
    return SVec2f(0.0f, 0.0f);
  case SFmt_Gray:
    return image->data_gray[w * y + x];
  case SFmt_RGB:
    return rgb2green(image->data_rgb[w * y + x]);
  case SFmt_SeparateRGB:
    return image->data_red[w * (y + h) + x];
  }
  assert(0 && "Impossible case");
}

SVec2f_t SImage_pixelBlue(const SImage_t *image, int x, int y) {
  if (x < 0 || y < 0 || x >= (int)image->width || y >= (int)image->height)
    return SVec2f(0.0f, 0.0f);

  int w = image->width;
  int h = image->height;

  switch (image->format) {
  case SFmt_Invalid:
    return SVec2f(0.0f, 0.0f);
  case SFmt_Gray:
    return image->data_gray[w * y + x];
  case SFmt_RGB:
    return rgb2blue(image->data_rgb[w * y + x]);
  case SFmt_SeparateRGB:
    return image->data_red[w * (y + 2*h) + x];
  }
  assert(0 && "Impossible case");
}
