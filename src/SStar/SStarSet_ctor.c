/* This file is part of Spica, released under MIT license.
 * See LICENSE for details.
 */

/* Author: Piotr Polesiuk, 2022 */

#include "SStar.h"

#include <stdlib.h>

void SStarSet_init(SStarSet_t *sset) {
  sset->length   = 0;
  sset->capacity = 4;
  sset->data     = malloc(sizeof(SStar_t) * sset->capacity);
}

void SStarSet_deinit(SStarSet_t *sset) {
  free(sset->data);
}

SStarSet_t *SStarSet_alloc(void) {
  SStarSet_t *sset = malloc(sizeof(SStarSet_t));
  if (sset == NULL) return NULL;

  SStarSet_init(sset);
  return sset;
}

void SStarSet_free(SStarSet_t *sset) {
  if (sset == NULL) return;
  SStarSet_deinit(sset);
  free(sset);
}
