/* This file is part of Spica, released under MIT license.
 * See LICENSE for details.
 */

/* Author: Piotr Polesiuk, 2025 */

#include "SImage.h"

static SGrayGradient_t SGrayGradient_negate(SGrayGradient_t grad) {
  return (SGrayGradient_t) {
    .bias = -grad.bias,
    .coef = -grad.coef
  };
}

SGradient_t SGradient_negate(const SGradient_t *grad) {
  if (grad->is_color) {
    return (SGradient_t) {
      .is_color = 1,
      .red      = SGrayGradient_negate(grad->red),
      .green    = SGrayGradient_negate(grad->green),
      .blue     = SGrayGradient_negate(grad->blue)
    };
  } else {
    return (SGradient_t) {
      .is_color = 0,
      .gray     = SGrayGradient_negate(grad->gray)
    };
  }
}
