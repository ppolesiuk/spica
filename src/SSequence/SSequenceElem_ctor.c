/* This file is part of Spica, released under MIT license.
 * See LICENSE for details.
 */

/* Author: Piotr Polesiuk, 2022 */

#include "SSequence.h"

void SSequenceElem_init(
  SSequenceElem_t         *elem,
  const SImageSourceOps_t *ops,
  void                    *data)
{
  elem->image.ops  = ops;
  elem->image.data = data;

  elem->transform.type = STr_Identity;
  elem->boundingBox    = SBoundingBox_empty();
}

void SSequenceElem_deinit(SSequenceElem_t *elem) {
  SImageSource_deinit(&elem->image);
}
