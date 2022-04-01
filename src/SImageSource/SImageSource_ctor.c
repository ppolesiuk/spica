/* This file is part of Spica, released under MIT license.
 * See LICENSE for details.
 */

/* Author: Piotr Polesiuk, 2022 */

#include "SImageSource.h"

void SImageSource_deinit(SImageSource_t *src) {
  src->ops->free(src->data);
}
