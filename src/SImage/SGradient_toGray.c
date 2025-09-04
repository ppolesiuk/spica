/* This file is part of Spica, released under MIT license.
 * See LICENSE for details.
 */

/* Author: Piotr Polesiuk, 2025 */

#include "SImage.h"

SGrayGradient_t SGradient_grayGradient(const SGradient_t *grad) {
  if (grad->is_color) {
    return (SGrayGradient_t) {
      .bias = (grad->red.bias + grad->green.bias + grad->blue.bias) / 3.0f,
      .coef = (grad->red.coef + grad->green.coef + grad->blue.coef) / 3.0f
    };
  } else {
    return grad->gray;
  }
}

void SGradient_toGray(SGradient_t *grad) {
  if (!grad->is_color) return;

  grad->gray = SGradient_grayGradient(grad);
  grad->is_color  = 0;
}
