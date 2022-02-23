/* This file is part of Spica, released under MIT license.
 * See LICENSE for details.
 */

/* Author: Piotr Polesiuk, 2022 */

#include "SStar.h"

void SStar_init(SStar_t *star) {
  star->pos        = SVec2f(0.0f, 0.0f);
  star->brightness = 1.0f;
  star->bias       = 0.0f;
  star->sigma      = 3.0f;
  star->index      = -1;
  star->weight     = 1;
}
