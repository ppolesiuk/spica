/* This file is part of Spica, released under MIT license.
 * See LICENSE for details.
 */

/* Author: Piotr Polesiuk, 2022 */

#include "SStar.h"

#include <stdlib.h>
#include <string.h>

void SStarSet_clone_at(SStarSet_t *dst, const SStarSet_t *sset) {
  dst->length   = sset->length;
  dst->capacity = sset->length;
  dst->data     = malloc(sizeof(SStar_t) * dst->capacity);
  memcpy(dst->data, sset->data, sizeof(SStar_t) * dst->capacity);
}

SStarSet_t *SStarSet_clone(const SStarSet_t *sset) {
  SStarSet_t *dst = malloc(sizeof(SStarSet_t));
  if (dst == NULL) return NULL;
  SStarSet_clone_at(dst, sset);
  return dst;
}
