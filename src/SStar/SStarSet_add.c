/* This file is part of Spica, released under MIT license.
 * See LICENSE for details.
 */

/* Author: Piotr Polesiuk, 2022 */

#include "SStar.h"

#include <stdlib.h>

void SStarSet_add(SStarSet_t *sset, const SStar_t *star) {
  if (sset->length == sset->capacity) {
    sset->capacity += 1 + (sset->capacity >> 1);
    sset->data = realloc(sset->data, sizeof(SStar_t) * sset->capacity);
  }

  sset->data[sset->length++] = *star;
}
