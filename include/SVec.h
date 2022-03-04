/* This file is part of Spica, released under MIT license.
 * See LICENSE for details.
 */

/* Author: Piotr Polesiuk, 2022 */

/** \file  SVec.h
 *  \brief Vectors and vector operations
 */

#ifndef __SPICA_VEC_H__
#define __SPICA_VEC_H__

/** \brief 2D single precision floating-point vector */
typedef float SVec2f_t __attribute__((vector_size(sizeof(float)*2)));

/** \brief 4D single precision floating-point vector */
typedef float SVec4f_t __attribute__((vector_size(sizeof(float)*4)));

/** \brief Create a new 2D vector of given coordinates */
static inline SVec2f_t SVec2f(float x, float y) __attribute__((unused));

/** \brief Create a new 4D vector of given coordinates */
static inline SVec4f_t SVec4f(float x, float y, float z, float w)
  __attribute__((unused));

/** \brief Square of vector length */
static inline float SVec2f_lengthSq(SVec2f_t v) __attribute__((unused));

/** \brief Square of vector length */
static inline float SVec4f_lengthSq(SVec4f_t v) __attribute__((unused));

/* ========================================================================= */

static inline SVec2f_t SVec2f(float x, float y) {
  SVec2f_t result = { x, y };
  return result;
}

static inline SVec4f_t SVec4f(float x, float y, float z, float w) {
  SVec4f_t result = { x, y, z, w };
  return result;
}

static inline float SVec2f_lengthSq(SVec2f_t v) {
  return v[0]*v[0] + v[1]*v[1];
}

static inline float SVec4f_lengthSq(SVec4f_t v) {
  return v[0]*v[0] + v[1]*v[1] + v[2]*v[2] + v[3]*v[3];
}

#endif /* __SPICA_VEC_H__ */
