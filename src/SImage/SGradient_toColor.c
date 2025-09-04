/* This file is part of Spica, released under MIT license.
 * See LICENSE for details.
 */

/* Author: Piotr Polesiuk, 2025 */

#include "SImage.h"

void SGradient_toColor(SGradient_t *grad) {
  if (grad->is_color)
    return;
  grad->is_color = 1;
  grad->red   = grad->gray;
  grad->green = grad->gray;
  grad->blue  = grad->gray;
}
