/* This file is part of Spica, released under MIT license.
 * See LICENSE for details.
 */

/* Author: Piotr Polesiuk, 2022 */

#ifndef __SPICA_SEQUENCE_H__
#define __SPICA_SEQUENCE_H__

#include "SImageSource.h"

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

void SSequence_addImage(SImageSourceOps_t ops, void *data);

void SSequence_addImagePNG(SSequence_t *seq, const char *fname);

void SSequence_addImageSIWW(SSequence_t *seq, const char *fname);

#endif /* __SPICA_SEQUENCE_H__ */
