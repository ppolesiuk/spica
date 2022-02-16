/* This file is part of Spica, released under MIT license.
 * See LICENSE for details.
 */

/* Author: Piotr Polesiuk, 2022 */

#include "SImage.h"

void SImage_divConst(SImage_t *image, float v) {
  SImage_mulConst(image, 1.0f / v);
}

void SImage_divConstRGB(SImage_t *image, float r, float g, float b) {
  SImage_mulConstRGB(image, 1.0f / r, 1.0f / g, 1.0f / b);
}
