/* This file is part of Spica, released under MIT license.
 * See LICENSE for details.
 */

/* Author: Piotr Polesiuk, 2022 */

#include "SStarMatcher.h"

void SStarMatcher_update(
  SStarMatcher_t *sm, const STransform_t *tr, SStarSet_t *sset)
{
  for (size_t i = 0; i < sset->length; i++) {
    SVec2f_t pos = STransform_apply(tr, sset->data[i].pos);
    int idx = sset->data[i].index;
    if (idx < 0 || idx >= sm->sset.length) {
      idx = sm->sset.length;
      SStarSet_add(&sm->sset, &sset->data[i]);
      sm->sset.data[idx].pos    = pos;
      sm->sset.data[idx].index  = -1;
      sm->sset.data[idx].weight = 1;
      sset->data[i].index = idx;
    } else {
      float w = sm->sset.data[idx].weight;
      pos += sm->sset.data[idx].pos * w;
      sm->sset.data[idx].weight += 1;
      sm->sset.data[idx].pos = pos / (w+1);
      sm->sset.data[idx].brightness =
        (sm->sset.data[idx].brightness * w + sset->data[i].brightness) / (w+1);
      sm->sset.data[idx].bias =
        (sm->sset.data[idx].bias * w + sset->data[i].bias) / (w+1);
      sm->sset.data[idx].sigma =
        (sm->sset.data[idx].sigma * w + sset->data[i].sigma) / (w+1);
    }
  }
}
