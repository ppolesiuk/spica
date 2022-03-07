/* This file is part of Spica, released under MIT license.
 * See LICENSE for details.
 */

/* Author: Piotr Polesiuk, 2022 */

#include "SStarMatcher.h"

static void matchStar(
  const SStarMatcher_t *sm, const STransform_t *tr, SStar_t *star)
{
  star->index = -1;

  if (sm->sset.length == 0) {
    return;
  }

  SVec2f_t pos = STransform_apply(tr, star->pos);
  float sigma = star->sigma;

  size_t best_index = 0;
  float best_dist = SVec2f_lengthSq(pos - sm->sset.data[0].pos);
  best_dist /= sigma * sm->sset.data[0].sigma;

  for (size_t i = 1; i < sm->sset.length; i++) {
    float dist = SVec2f_lengthSq(pos - sm->sset.data[i].pos);
    dist /= sigma * sm->sset.data[i].sigma;
    if (dist < best_dist) {
      best_index = i;
      best_dist  = dist;
    }
  }

  if (best_dist < sm->distThreshold * sm->distThreshold) {
    star->index = best_index;
  }
}

void SStarMatcher_matchStars(
  const SStarMatcher_t *sm, const STransform_t *tr, SStarSet_t *sset)
{
  for (size_t i = 0; i < sset->length; i++) {
    matchStar(sm, tr, &sset->data[i]);
  }
}
