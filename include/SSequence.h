/* This file is part of Spica, released under MIT license.
 * See LICENSE for details.
 */

/* Author: Piotr Polesiuk, 2022 */

#ifndef __SPICA_SEQUENCE_H__
#define __SPICA_SEQUENCE_H__

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

typedef struct SSequenceElem {
  SImageSource_t image;
} SSequenceElem_t;

typedef struct SSequence {
  size_t length;
  size_t capacity;
  SSequenceElem_t *data;
} SSequence_t;

/* ========================================================================= */

void SSequenceElem_deinit(SSequenceElem_t *elem);

void SSequence_init(SSequence_t *seq);

void SSequence_deinit(SSequence_t *seq);

SSequence_t *SSequence_alloc(void);

void SSequence_free(SSequence_t *seq);

#endif /* __SPICA_SEQUENCE_H__ */
