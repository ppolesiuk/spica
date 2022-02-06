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

#endif /* __SPICA_VEC_H__ */
