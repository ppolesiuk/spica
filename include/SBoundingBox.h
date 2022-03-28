/* This file is part of Spica, released under MIT license.
 * See LICENSE for details.
 */

/* Author: Piotr Polesiuk, 2022 */

/** \file  SBoundingBox.h
 *  \brief Bounding boxes
 */

#ifndef __SPICA_BOUNDING_BOX_H__
#define __SPICA_BOUNDING_BOX_H__

/** \brief Bounding box, i.e. rectangle that contains given element e.g.,
 *     transformed image. 
 *
 * If \ref minX is larger than \ref maxX, then bounding box is treated as
 * empty. Emptiness checking does not look at Y coordinates */
typedef struct SBoundingBox {
  /** \brief Minimal X coordinate */
  float minX;
  /** \brief Minimal Y coordinate */
  float minY;
  /** \brief Maximal X coordinate */
  float maxX;
  /** \brief Maximal Y coordinate */
  float maxY;
} SBoundingBox_t;

/** \brief Create empty BoundingBox_t */
inline static SBoundingBox_t SBoundingBox_empty(void) __attribute__((unused)); 

/** \brief Check if given BoundingBox_t is empty.
 *
 * Empty bounding box has minX is larger than maxX. Emptiness checking does
 * not look at Y coordinates */
inline static int SBoundingBox_isEmpty(SBoundingBox_t bb)
  __attribute__((unused));

/** \brief Union of two bounding boxes
 *
 * This function computes bounding box that is large enough, to contain
 * both bounding boxes given as arguments. If one of arguments is empty,
 * the other bounding box is returned. */
SBoundingBox_t SBoundingBox_union(SBoundingBox_t bb1, SBoundingBox_t bb2);

/** \brief Intersection of two bounding boxes
 *
 * This function computes bounding box that contains all points that belongs
 * to bounding boxes given as arguments. If one of arguments is empty, the
 * empty bounding box is returned. */
SBoundingBox_t SBoundingBox_intersection(
  SBoundingBox_t bb1, SBoundingBox_t bb2);

/* ========================================================================= */
inline static SBoundingBox_t SBoundingBox_empty(void) {
  SBoundingBox_t result = {
    .minX = 1.0f,
    .maxX = 0.0f,
  };
  return result;
}

inline static int SBoundingBox_isEmpty(SBoundingBox_t bb) {
  return bb.minX > bb.maxX;
}

#endif
