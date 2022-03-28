/* This file is part of Spica, released under MIT license.
 * See LICENSE for details.
 */

/* Author: Piotr Polesiuk, 2022 */

#include "SBoundingBox.h"

inline static float minf(float x, float y) {
  return x < y ? x : y;
}

inline static float maxf(float x, float y) {
  return x < y ? y : x;
}

SBoundingBox_t SBoundingBox_union(SBoundingBox_t bb1, SBoundingBox_t bb2) {
  if (SBoundingBox_isEmpty(bb1)) return bb2;
  if (SBoundingBox_isEmpty(bb2)) return bb1;

  SBoundingBox_t bb = {
    .minX = minf(bb1.minX, bb2.minX),
    .minY = minf(bb1.minY, bb2.minY),
    .maxX = maxf(bb1.maxX, bb2.maxX),
    .maxY = maxf(bb1.maxY, bb2.maxY),
  };
  return bb;
}

SBoundingBox_t SBoundingBox_intersection(
  SBoundingBox_t bb1, SBoundingBox_t bb2)
{
  SBoundingBox_t bb = {
    .minX = maxf(bb1.minX, bb2.minX),
    .minY = maxf(bb1.minY, bb2.minY),
    .maxX = minf(bb1.maxX, bb2.maxX),
    .maxY = minf(bb1.maxY, bb2.maxY),
  };

  if (bb.minY > bb.maxY) {
    bb.minX = 1.0f;
    bb.maxX = 0.0f;
  }
  return bb;
}
