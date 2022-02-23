/* This file is part of Spica, released under MIT license.
 * See LICENSE for details.
 */

/* Author: Piotr Polesiuk, 2022 */

#include "SStar.h"

#include <stdlib.h>

static int starCompareInv(const void *st1, const void *st2) {
  const SStar_t *s1 = st1;
  const SStar_t *s2 = st2;
  if (s1->brightness == s2->brightness) return 0;
  if (s1->brightness < s2->brightness)  return 1;
  return -1;
}

void SStarSet_sort(SStarSet_t *sset) {
  qsort(sset->data, sset->length, sizeof(SStar_t), starCompareInv);
}
