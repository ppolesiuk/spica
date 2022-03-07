/* This file is part of Spica, released under MIT license.
 * See LICENSE for details.
 */

/* Author: Piotr Polesiuk, 2022 */

#include "SStarMatcher.h"

#include <stdlib.h>

void SStarMatcher_init(SStarMatcher_t *sm) {
  SStarSet_init(&sm->sset);
  sm->distThreshold = 1.4;
}

void SStarMatcher_deinit(SStarMatcher_t *sm) {
  SStarSet_deinit(&sm->sset);
}

SStarMatcher_t *SStarMatcher_alloc(void) {
  SStarMatcher_t *sm = malloc(sizeof(SStarMatcher_t));
  if (sm == NULL) return NULL;

  SStarMatcher_init(sm);
  return sm;
}

void SStarMatcher_free(SStarMatcher_t *sm) {
  if (sm == NULL) return;
  SStarMatcher_deinit(sm);
  free(sm);
}
