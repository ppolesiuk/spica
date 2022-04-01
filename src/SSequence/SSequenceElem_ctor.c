/* This file is part of Spica, released under MIT license.
 * See LICENSE for details.
 */

/* Author: Piotr Polesiuk, 2022 */

#include "SSequence.h"

void SSequenceElem_deinit(SSequenceElem_t *elem) {
  SImageSource_deinit(&elem->image);
}
