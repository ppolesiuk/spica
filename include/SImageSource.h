/* This file is part of Spica, released under MIT license.
 * See LICENSE for details.
 */

/* Author: Piotr Polesiuk, 2022 */

#ifndef __SPICA_IMAGE_SOURCE_H__
#define __SPICA_IMAGE_SOURCE_H__

#include "SImage.h"

typedef enum SImageSourceType {
  SIst_File,
  SIst_User
} SImageSourceType_t;

typedef struct SImageSourceOps {
  SImageSourceType_t type;
  void (*free)(void *);
} SImageSourceOps_t;

typedef enum SImageFileType {
  SFtp_PNG,
  SFtp_SIWW
} SImageFileType_t;

typedef struct SImageSource {
  SImageSourceOps_t *ops;
  void              *data;
} SImageSource_t;

/* ========================================================================= */

void SImageSource_deinit(SImageSource_t *src);

#endif
