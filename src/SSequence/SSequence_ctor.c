/* This file is part of Spica, released under MIT license.
 * See LICENSE for details.
 */

/* Author: Piotr Polesiuk, 2022 */

#include "SSequence.h"

#include <stdlib.h>

void SSequence_init(SSequence_t *seq) {
  seq->length   = 0;
  seq->capacity = 1;
  seq->data = malloc(sizeof(SSequenceElem_t) * seq->capacity);
}

void SSequence_deinit(SSequence_t *seq) {
  for (size_t i = 0; i < seq->length; i++) {
    SSequenceElem_deinit(&seq->data[i]);
  }
  free(seq->data);
}

SSequence_t *SSequence_alloc(void) {
  SSequence_t *seq = malloc(sizeof(SSequence_t));
  if (seq == NULL) return NULL;

  SSequence_init(seq);
  return seq;
}

void SSequence_free(SSequence_t *seq) {
  if (seq == NULL) return;
  SSequence_deinit(seq);
  free(seq);
}
