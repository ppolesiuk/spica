/* This file is part of Spica, released under MIT license.
 * See LICENSE for details.
 */

/* Author: Piotr Polesiuk, 2022 */

#include "SSequence.h"

#include <stdlib.h>

void SSequence_addImage(
  SSequence_t             *seq,
  const SImageSourceOps_t *ops,
  void                    *data)
{
  if (seq->length >= seq->capacity) {
    seq->capacity += 1 + (seq->capacity >> 1);
    seq->data = realloc(seq->data, seq->capacity * sizeof(SSequenceElem_t));
  }

  SSequenceElem_init(&seq->data[seq->length++], ops, data);
}

void SSequence_addImagePNG(SSequence_t *seq, const char *fname) {
  SImageSource_t src = SImageSource_File_create(fname, SFtp_PNG);
  SSequence_addImage(seq, src.ops, src.data);
}

void SSequence_addImageSIWW(SSequence_t *seq, const char *fname) {
  SImageSource_t src = SImageSource_File_create(fname, SFtp_SIWW);
  SSequence_addImage(seq, src.ops, src.data);
}
