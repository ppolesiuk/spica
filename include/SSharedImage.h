/* This file is part of Spica, released under MIT license.
 * See LICENSE for details.
 */

/* Author: Piotr Polesiuk, 2022 */

#ifndef __SPICA_SHARED_IMAGE_H__
#define __SPICA_SHARED_IMAGE_H__

#include "SImage.h"

typedef struct SSharedImage {
  int      refcnt;
  SImage_t image;
} SSharedImage_t;

#endif /* __SPICA_SHARED_IMAGE_H__ */
