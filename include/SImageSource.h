/* This file is part of Spica, released under MIT license.
 * See LICENSE for details.
 */

/* Author: Piotr Polesiuk, 2022 */

#ifndef __SPICA_IMAGE_SOURCE_H__
#define __SPICA_IMAGE_SOURCE_H__

#include "SSharedImage.h"

typedef enum SImageSourceType {
  SIst_File,
  SIst_User
} SImageSourceType_t;

typedef struct SImageSourceOps {
  SImageSourceType_t type;
  void            (*free)(void *);
  SSharedImage_t *(*get)(void *);
} SImageSourceOps_t;

typedef struct SImageSource {
  const SImageSourceOps_t *ops;
  void                    *data;
} SImageSource_t;

/* ========================================================================= */

static void SImageSource_deinit(SImageSource_t *src) __attribute__((unused));

static SSharedImage_t *SImageSource_get(SImageSource_t *src)
  __attribute__((unused));

/* ========================================================================= */

typedef enum SImageFileType {
  SFtp_PNG,
  SFtp_SIWW
} SImageFileType_t;

typedef struct SImageSource_File {
  const char      *fname;
  SImageFileType_t type;
} SImageSource_File_t;

extern const SImageSourceOps_t SImageSource_File_ops;

SImageSource_t SImageSource_File_create(
  const char      *fname,
  SImageFileType_t type);

/* ========================================================================= */

static void SImageSource_deinit(SImageSource_t *src) {
  src->ops->free(src->data);
}

static SSharedImage_t *SImageSource_get(SImageSource_t *src) {
  return src->ops->get(src->data);
}

#endif
