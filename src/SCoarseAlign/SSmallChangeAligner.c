/* This file is part of Spica, released under MIT license.
 * See LICENSE for details.
 */

/* Author: Piotr Polesiuk, 2022 */

#include "SCoarseAlign.h"

void SSmallChangeAligner_init(SSmallChangeAligner_t *aligner) {
  aligner->distThreshold = 5.0f;
  aligner->minStarN = 4;
}

static int closestStarIndex(
  const SSmallChangeAligner_t *aligner,
  const SStarSet_t *ref_sset,
  SVec2f_t pos,
  float    sigma)
{
  if (ref_sset->length == 0) return -1;

  int   best_idx  = 0;
  float best_dist =
    SVec2f_lengthSq(pos - ref_sset->data[0].pos) /
      (sigma * ref_sset->data[0].sigma);

  for (size_t i = 0; i < ref_sset->length; i++) {
    float dist =
      SVec2f_lengthSq(pos - ref_sset->data[i].pos) /
        (sigma * ref_sset->data[i].sigma);

    if (dist < best_dist) {
      best_idx  = i;
      best_dist = dist;
    }
  }
  if (best_dist > aligner->distThreshold * aligner->distThreshold) return -1;
  return best_idx;
}

STransform_t SSmallChangeAligner_align(
  const SSmallChangeAligner_t *aligner,
  const SStarSet_t *ref_sset,
  const STransform_t *prev_tr,
  const SStarSet_t *sset)
{
  SVec2f_t sx  = { 0.0f, 0.0f };
  SVec2f_t sy  = { 0.0f, 0.0f };
  SVec2f_t sxy = { 0.0f, 0.0f };
  float sx2  = 0.0f;
  int   tot  = 0;

  for (size_t i = 0; i < sset->length; i++) {
    SVec2f_t x = sset->data[i].pos;
    int idx = closestStarIndex(aligner, ref_sset,
      STransform_apply(prev_tr, x), sset->data[i].sigma);

    if (idx == -1) continue;

    SVec2f_t y = ref_sset->data[idx].pos;

    sx += x;
    sy += y;
    sxy += SVec2f_complexMul(SVec2f_complexConj(x), y);
    sx2 += SVec2f_lengthSq(x);
    tot++;
  }

  STransform_t tr;

  sxy *= (float)tot;
  sx2 *= (float)tot;
  float s2x = SVec2f_lengthSq(sx);
  if (tot < aligner->minStarN || sx2 == s2x) {
    tr.type  = STr_Drop;
    tr.rot   = SVec2f(1.0f, 0.0f);
    tr.shift = SVec2f(0.0f, 0.0f);
  } else {
    tr.type  = STr_Linear;
    tr.rot   = (sxy - SVec2f_complexMul(SVec2f_complexConj(sx), sy)) /
               (sx2 - s2x);
    tr.shift = (sy - SVec2f_complexMul(tr.rot, sx)) / (float)tot;
  }
  return tr;
}
