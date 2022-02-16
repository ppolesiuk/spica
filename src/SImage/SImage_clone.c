/* This file is part of Spica, released under MIT license.
 * See LICENSE for details.
 */

/* Author: Piotr Polesiuk, 2022 */

#include "SImage.h"

#include <stdlib.h>
#include <string.h>

void SImage_clone_at(SImage_t *dst, const SImage_t *image) {
  SImage_init(dst, image->width, image->height, image->format);
  if (dst->format == SFmt_Invalid) return;

  memcpy(dst->data, image->data, SImage_dataSize(image));
}

SImage_t *SImage_clone(const SImage_t *image) {
  SImage_t *dst = malloc(sizeof(SImage_t));
  if (dst == NULL) return NULL;
  SImage_clone_at(dst, image);
  return dst;
}
