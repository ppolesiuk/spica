/* This file is part of Spica, released under MIT license.
 * See LICENSE for details.
 */

/* Author: Piotr Polesiuk, 2022 */

#include "SImage.h"

void SImage_subConst(SImage_t *image, float v) {
  SImage_addConst(image, -v);
}

void SImage_subConstRGB(SImage_t *image, float r, float g, float b) {
  SImage_addConstRGB(image, -r, -g, -b);
}
