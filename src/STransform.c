/* This file is part of Spica, released under MIT license.
 * See LICENSE for details.
 */

/* Author: Piotr Polesiuk, 2022 */

#include "STransform.h"

#include <assert.h>

SVec2f_t STransform_apply(const STransform_t *tr, SVec2f_t v) {
  switch (tr->type) {
  case STr_Linear:
    v = SVec2f_complexMul(v, tr->rot);
  case STr_Shift:
    v += tr->shift;
  case STr_Drop:
  case STr_Identity:
    break;
  }
  return v;
}

STransform_t STransform_inverse(const STransform_t *tr) {
  STransform_t result = {
    .type  = tr->type,
    .rot   = { 1.0f, 0.0f },
    .shift = { 0.0f, 0.0f }
  };
  switch (tr->type) {
  case STr_Drop:
  case STr_Identity:
    break;
  case STr_Shift:
    result.shift = -tr->shift;
    break;
  case STr_Linear:
    result.shift = -SVec2f_complexDiv(tr->shift, tr->rot);
    result.rot   = SVec2f_complexInv(tr->rot);
    break;
  }
  return result;
}

/* ========================================================================= */
static STransform_t composeWithShift(SVec2f_t shift, const STransform_t *tr) {
  switch (tr->type) {
  case STr_Drop:
    return *tr;
  case STr_Identity:
    return STransform_shift(shift);
  case STr_Shift:
    return STransform_shift(shift + tr->shift);
  case STr_Linear:
    return STransform_linear(tr->rot, tr->shift + shift);
  }
  assert(0 && "Impossible case");
}

static STransform_t composeWithLinear(
  SVec2f_t rot, SVec2f_t shift, const STransform_t *tr)
{
  switch (tr->type) {
  case STr_Drop:
    return *tr;
  case STr_Identity:
    return STransform_linear(rot, shift);
  case STr_Shift:
    return STransform_linear(rot, SVec2f_complexMul(rot, tr->shift) + shift);
  case STr_Linear:
    return STransform_linear(
      SVec2f_complexMul(rot, tr->rot),
      SVec2f_complexMul(rot, tr->shift) + shift);
  }
  assert(0 && "Impossible case");
}

STransform_t STransform_compose(
  const STransform_t *tr2,
  const STransform_t *tr1)
{
  switch (tr2->type) {
  case STr_Drop:
    return *tr2;
  case STr_Identity:
    return *tr1;
  case STr_Shift:
    return composeWithShift(tr2->shift, tr1);
  case STr_Linear:
    return composeWithLinear(tr2->rot, tr2->shift, tr1);
  }
  assert(0 && "Impossible case");
}

/* ========================================================================= */
inline static float minf(float x, float y) {
  return x < y ? x : y;
}

inline static float maxf(float x, float y) {
  return x < y ? y : x;
}

SBoundingBox_t STransform_boundingBox(
  const STransform_t *tr,
  SBoundingBox_t      bb)
{
  if (SBoundingBox_isEmpty(bb) || tr->type == STr_Drop) {
    return SBoundingBox_empty();
  }

  SVec2f_t p1 = STransform_apply(tr, SVec2f(bb.minX, bb.minY));
  SVec2f_t p2 = STransform_apply(tr, SVec2f(bb.minX, bb.maxY));
  SVec2f_t p3 = STransform_apply(tr, SVec2f(bb.maxX, bb.minY));
  SVec2f_t p4 = STransform_apply(tr, SVec2f(bb.maxX, bb.maxY));

  SBoundingBox_t result = {
    .minX = minf(minf(p1[0], p2[0]), minf(p3[0], p4[0])),
    .minY = minf(minf(p1[1], p2[1]), minf(p3[1], p4[1])),
    .maxX = maxf(maxf(p1[0], p2[0]), maxf(p3[0], p4[0])),
    .maxY = maxf(maxf(p1[1], p2[1]), maxf(p3[1], p4[1])),
  };
  return result;
}
