/* This file is part of Spica, released under MIT license.
 * See LICENSE for details.
 */

/* Author: Piotr Polesiuk, 2022 */

/** \file STransform.h
 * \brief 2D transformations
 */

#ifndef __SPICA_TRANSFORM_H__
#define __SPICA_TRANSFORM_H__

#include "SBoundingBox.h"
#include "SVec.h"

/** \brief Type of STransform_t transformation */
typedef enum STransformType {
  /** No transformation. Just drop this image. */
  STr_Drop,
  /** Identity transformation. */
  STr_Identity,
  /** Translation-only */
  STr_Shift,
  /** Rotation and translation -- linear function on complex numbers */
  STr_Linear
} STransformType_t;

/** \brief Transformation of 2D vectors. */
typedef struct STransform {
  /** \brief  Type of the transformation */
  STransformType_t type;
  /** \brief  Rotation applied to vector.
   *
   * Rotation is a complex number multiplied by rotated vector before
   * translation. This field is ignored by \ref STr_Drop, \ref STr_Identity,
   * and \ref STr_Shift translations. */
  SVec2f_t         rot;
  /** \brief Translation vector (added to translated vector).
   *
   * Ignored by \ref STr_Identity and \ref STr_Drop transformations */
  SVec2f_t         shift;
} STransform_t;

/** \brief Create translation by given vector */
static inline STransform_t STransform_shift(SVec2f_t shift)
  __attribute__((unused));

/** \brief Create transformation that rotates, scales, and translates
 *
 * Rotation and scaling is represented by \p rot complex number: the argument
 * of a transformation is multiplied (as a complex number) by it, then it is
 * translated by \p shift vector.
 *
 * \param rot   rotation and scaling -- a complex number
 * \param shift translation applied after rotation */
static inline STransform_t STransform_linear(SVec2f_t rot, SVec2f_t shift)
  __attribute__((unused));

/** \brief Compute the inverse transformation */
STransform_t STransform_inverse(const STransform_t *tr);

/** \brief Compose two transformations.
 *
 * The order of arguments is the same as for composition of functions:
 * \p tr2 is applied first. */
STransform_t STransform_compose(
  const STransform_t *tr2,
  const STransform_t *tr1);

/** \brief Apply transformation to given vector */
SVec2f_t STransform_apply(const STransform_t *tr, SVec2f_t v);

/** \brief Apply transformation to SBoundingBox_t
 *
 * \returns SBoundingBox_t that is large enough, to contain transformed
 *   bounding box. */
SBoundingBox_t STransform_boundingBox(
  const STransform_t *tr,
  SBoundingBox_t      bb);

/* ========================================================================= */
static inline STransform_t STransform_shift(SVec2f_t shift) {
  STransform_t tr = {
    .type  = STr_Shift,
    .shift = shift,
    .rot   = { 1.0f, 0.0f }
  };
  return tr;
}

static inline STransform_t STransform_linear(SVec2f_t rot, SVec2f_t shift) {
  STransform_t tr = {
    .type  = STr_Linear,
    .rot   = rot,
    .shift = shift
  };
  return tr;
}

#endif /* __SPICA_TRANSFORM_H__ */
