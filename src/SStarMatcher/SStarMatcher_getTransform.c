/* This file is part of Spica, released under MIT license.
 * See LICENSE for details.
 */

/* Author: Piotr Polesiuk, 2022 */

#include "SStarMatcher.h"

STransform_t SStarMatcher_getTransform(
  const SStarMatcher_t *sm, const SStarSet_t *sset)
{
  SVec2f_t sx  = { 0.0f, 0.0f };
  SVec2f_t sy  = { 0.0f, 0.0f };
  SVec2f_t sxy = { 0.0f, 0.0f };
  float sx2  = 0.0f;
  float tot  = 0.0f;

  for (size_t i = 0; i < sset->length; i++) {
    int idx = sset->data[i].index;
    if (idx < 0 || idx >= sm->sset.length) continue;

    SVec2f_t x = sset->data[i].pos;
    SVec2f_t y = sm->sset.data[idx].pos;

    sx += x;
    sy += y;
    sxy += SVec2f_complexMul(SVec2f_complexConj(x), y);
    sx2 += SVec2f_lengthSq(x);
    tot += 1.0f;
  }

  STransform_t tr;

  sxy *= tot;
  sx2 *= tot;
  float s2x = SVec2f_lengthSq(sx);
  if (sx2 == s2x) {
    tr.type  = STr_Drop;
    tr.rot   = SVec2f(1.0f, 0.0f);
    tr.shift = SVec2f(0.0f, 0.0f);
  } else {
    tr.type  = STr_Linear;
    tr.rot   = (sxy - SVec2f_complexMul(SVec2f_complexConj(sx), sy)) /
               (sx2 - s2x);
    tr.shift = (sy - SVec2f_complexMul(tr.rot, sx)) / tot;
  }
  return tr;
}
