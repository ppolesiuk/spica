/* This file is part of Spica, released under MIT license.
 * See LICENSE for details.
 */

/* Author: Piotr Polesiuk, 2022 */

#include "SImage.h"

SVec2f_t SImage_subpixelGray(const SImage_t *image, SVec2f_t pos) {
  pos += SVec2f(1.0f, 1.0f);
  int x = (int)(pos[0]);
  int y = (int)(pos[1]);
  SVec2f_t d = pos - SVec2f(x, y);

  return SVec2f_lerp(d[1],
    SVec2f_lerp(d[0], SImage_pixelGray(image, x-1, y-1),
                      SImage_pixelGray(image, x,   y-1)),
    SVec2f_lerp(d[0], SImage_pixelGray(image, x-1, y),
                      SImage_pixelGray(image, x,   y)));
}

SVec4f_t SImage_subpixelRGB(const SImage_t *image, SVec2f_t pos) {
  pos += SVec2f(1.0f, 1.0f);
  int x = (int)(pos[0]);
  int y = (int)(pos[1]);
  SVec2f_t d = pos - SVec2f(x, y);

  return SVec4f_lerp(d[1],
    SVec4f_lerp(d[0], SImage_pixelRGB(image, x-1, y-1),
                      SImage_pixelRGB(image, x,   y-1)),
    SVec4f_lerp(d[0], SImage_pixelRGB(image, x-1, y),
                      SImage_pixelRGB(image, x,   y)));
}

SVec2f_t SImage_subpixelRed(const SImage_t *image, SVec2f_t pos) {
  pos += SVec2f(1.0f, 1.0f);
  int x = (int)(pos[0]);
  int y = (int)(pos[1]);
  SVec2f_t d = pos - SVec2f(x, y);

  return SVec2f_lerp(d[1],
    SVec2f_lerp(d[0], SImage_pixelRed(image, x-1, y-1),
                      SImage_pixelRed(image, x,   y-1)),
    SVec2f_lerp(d[0], SImage_pixelRed(image, x-1, y),
                      SImage_pixelRed(image, x,   y)));
}

SVec2f_t SImage_subpixelGreen(const SImage_t *image, SVec2f_t pos) {
  pos += SVec2f(1.0f, 1.0f);
  int x = (int)(pos[0]);
  int y = (int)(pos[1]);
  SVec2f_t d = pos - SVec2f(x, y);

  return SVec2f_lerp(d[1],
    SVec2f_lerp(d[0], SImage_pixelGreen(image, x-1, y-1),
                      SImage_pixelGreen(image, x,   y-1)),
    SVec2f_lerp(d[0], SImage_pixelGreen(image, x-1, y),
                      SImage_pixelGreen(image, x,   y)));
}

SVec2f_t SImage_subpixelBlue(const SImage_t *image, SVec2f_t pos) {
  pos += SVec2f(1.0f, 1.0f);
  int x = (int)(pos[0]);
  int y = (int)(pos[1]);
  SVec2f_t d = pos - SVec2f(x, y);

  return SVec2f_lerp(d[1],
    SVec2f_lerp(d[0], SImage_pixelBlue(image, x-1, y-1),
                      SImage_pixelBlue(image, x,   y-1)),
    SVec2f_lerp(d[0], SImage_pixelBlue(image, x-1, y),
                      SImage_pixelBlue(image, x,   y)));
}
