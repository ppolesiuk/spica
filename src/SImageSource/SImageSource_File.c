/* This file is part of Spica, released under MIT license.
 * See LICENSE for details.
 */

/* Author: Piotr Polesiuk, 2022 */

#include "SImageSource.h"

#include <stdlib.h>
#include <string.h>

static char *strdup(const char *str) {
  char *result = malloc(strlen(str) + 1);
  strcpy(result, str);
  return result;
}

SImageSource_t SImageSource_File_create(
  const char      *fname,
  SImageFileType_t type)
{
  SImageSource_File_t *data = malloc(sizeof(SImageSource_File_t));
  data->fname = strdup(fname);
  data->type  = type;

  SImageSource_t result = {
    .ops  = &SImageSource_File_ops,
    .data = data
  };
  return result;
}

static void SImageSource_File_free(void *data_ptr) {
  SImageSource_File_t *data = data_ptr;
  free((char *)data->fname);
  free(data);
}

static SSharedImage_t *SImageSource_File_get(void *data_ptr) {
  SImageSource_File_t *data = data_ptr;
  SSharedImage_t *result = malloc(sizeof(SSharedImage_t));
  result->refcnt = 1;
  result->image.format = SFmt_Invalid;
  switch (data->type) {
  case SFtp_PNG:
    SImage_loadPNG_at(&result->image, data->fname);
    break;
  case SFtp_SIWW:
    SImage_loadSIWW_at(&result->image, data->fname);
    break;
  }
  return result;
}

const SImageSourceOps_t SImageSource_File_ops = {
  .type = SIst_File,
  .free = SImageSource_File_free,
  .get  = SImageSource_File_get,
};
