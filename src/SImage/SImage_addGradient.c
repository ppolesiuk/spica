/* This file is part of Spica, released under MIT license.
 * See LICENSE for details.
 */

/* Author: Piotr Polesiuk, 2025 */

#include "SImage.h"

void SImage_addGradient(SImage_t *image, const SGradient_t *grad) {
  SGradient_t neg = SGradient_negate(grad);
  SImage_subGradient(image, &neg);
}
