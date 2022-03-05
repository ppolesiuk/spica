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

/** \brief Conjugate complex number */
static inline SVec2f_t SVec2f_complexConj(SVec2f_t a) __attribute__((unused));

/** \brief Inverse of a complex number */
static inline SVec2f_t SVec2f_complexInv(SVec2f_t a) __attribute__((unused));

/** \brief Multiply two complex numbers */
static inline SVec2f_t SVec2f_complexMul(SVec2f_t a, SVec2f_t b)
  __attribute__((unused));

/** \brief Divide two complex numbers */
static inline SVec2f_t SVec2f_complexDiv(SVec2f_t a, SVec2f_t b)
  __attribute__((unused));

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

static inline SVec2f_t SVec2f_complexConj(SVec2f_t a) {
  SVec2f_t result = { a[0], -a[1] };
  return result;
}

static inline SVec2f_t SVec2f_complexInv(SVec2f_t a) {
  return SVec2f_complexConj(a) / SVec2f_lengthSq(a);
}

static inline SVec2f_t SVec2f_complexMul(SVec2f_t a, SVec2f_t b) {
  SVec2f_t c = { -b[1], b[0] };
  return a[0] * b + a[1] * c;
}

static inline SVec2f_t SVec2f_complexDiv(SVec2f_t a, SVec2f_t b) {
  return SVec2f_complexMul(a, SVec2f_complexConj(b)) / SVec2f_lengthSq(b);
}

#endif /* __SPICA_VEC_H__ */
