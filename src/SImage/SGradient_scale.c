/* This file is part of Spica, released under MIT license.
 * See LICENSE for details.
 */

/* Author: Piotr Polesiuk, 2023 */

#include "SImage.h"

static SGrayGradient_t SGrayGradient_scale(SGrayGradient_t grad, float factor)
{
  float bias_fix = (grad.coef[0] + grad.coef[1]);
  bias_fix *= 0.5f * (factor - 1.0f) / factor;
  return (SGrayGradient_t) {
    .bias = grad.bias - bias_fix,
    .coef = grad.coef / factor
  };
}

SGradient_t SGradient_scale(const SGradient_t *grad, float factor) {
  if (grad->is_color) {
    return (SGradient_t) {
      .is_color = 1,
      .red      = SGrayGradient_scale(grad->red,   factor),
      .green    = SGrayGradient_scale(grad->green, factor),
      .blue     = SGrayGradient_scale(grad->blue,  factor)
    };
  } else {
    return (SGradient_t) {
      .is_color = 0,
      .gray     = SGrayGradient_scale(grad->gray, factor)
    };
  }
}
