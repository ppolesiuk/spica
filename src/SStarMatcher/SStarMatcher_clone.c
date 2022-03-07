/* This file is part of Spica, released under MIT license.
 * See LICENSE for details.
 */

/* Author: Piotr Polesiuk, 2022 */

#include "SStarMatcher.h"

#include <stdlib.h>

void SStarMatcher_clone_at(SStarMatcher_t *dst, const SStarMatcher_t *sm) {
  SStarSet_clone_at(&dst->sset, &sm->sset);
  dst->distThreshold = sm->distThreshold;
}

SStarMatcher_t *SStarMatcher_clone(const SStarMatcher_t *sm) {
  SStarMatcher_t *dst = malloc(sizeof(SStarMatcher_t));
  if (dst == NULL) return NULL;
  SStarMatcher_clone_at(dst, sm);
  return dst;
}
