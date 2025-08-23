/* This file is part of Spica, released under MIT license.
 * See LICENSE for details.
 */

/* Author: Piotr Polesiuk, 2025 */

#include "SSeqLoaderSER.h"
#include "SSeqLoaderSER_common.h"

#include "SImage.h"
#include "SDataRepr.h"

#include <stdlib.h>
#include <string.h>

static int parse_header(
  SSeqLoaderSER_t *loader,
  SER_header_t    *header)
{
  if (memcmp(header->magic, SER_HEADER_MAGIC, SER_HEADER_MAGIC_SIZE) != 0) {
    return SPICA_ERROR;
  }

  header->color_id = SLittleEndian32(header->color_id);
  switch (header->color_id) {
  case SSERFormat_Mono:
  case SSERFormat_Bayer_RGGB:
  case SSERFormat_Bayer_GRBG:
  case SSERFormat_Bayer_GBRG:
  case SSERFormat_Bayer_BGGR:
  case SSERFormat_Bayer_CYYM:
  case SSERFormat_Bayer_YCMY:
  case SSERFormat_Bayer_YMCY:
  case SSERFormat_Bayer_MYYC:
    loader->format = header->color_id;
    loader->bytes_per_pixel = 1;
    break;
  case SSERFormat_RGB:
  case SSERFormat_BGR:
    loader->format = header->color_id;
    loader->bytes_per_pixel = 3;
    break;
  default:
    return SPICA_ERROR;
  }

  header->endiannes = SLittleEndian32(header->endiannes);
  switch (header->endiannes) {
  case SER_LITTLE_ENDIAN:
    loader->swap = SLittleEndian16(1) != 1;
    break;
  case SER_BIG_ENDIAN:
    loader->swap = SBigEndian16(1) != 1;
    break;
  default:
    return SPICA_ERROR;
  }

  header->width = SLittleEndian32(header->width);
  loader->width = header->width;

  header->height = SLittleEndian32(header->height);
  loader->height = header->height;

  header->pixel_depth_per_plane =
    SLittleEndian32(header->pixel_depth_per_plane);
  loader->bits_per_pixel = header->pixel_depth_per_plane;
  if (loader->bits_per_pixel > 16 || loader->bits_per_pixel < 1) {
    return SPICA_ERROR; /* unsupported pixel depth */
  } else if (loader->bits_per_pixel > 8) {
    loader->bytes_per_pixel *= 2; /* 16 bits per pixel */
  } else {
    loader->swap = 0; /* no need to swap for 8 bits */
  }

  header->frame_count = SLittleEndian32(header->frame_count);
  loader->length = header->frame_count;

  memset(loader->observer, 0, sizeof(loader->observer));
  memcpy(loader->observer, header->observer, sizeof(header->observer));

  memset(loader->instrument, 0, sizeof(loader->instrument));
  memcpy(loader->instrument, header->instrument, sizeof(header->instrument));

  memset(loader->telescope, 0, sizeof(loader->telescope));
  memcpy(loader->telescope, header->telescope, sizeof(header->telescope));

  header->date_time     = SLittleEndian64(header->date_time);
  header->date_time_utc = SLittleEndian64(header->date_time_utc);

  return SPICA_OK;
}

int SSeqLoaderSER_open_at(SSeqLoaderSER_t *loader, const char *fname) {
  loader->file = fopen(fname, "rb");
  if (!loader->file) return SPICA_ERROR;

  SER_header_t header;
  if (fread(&header.magic[0], SER_HEADER_SIZE, 1, loader->file) != 1) {
    fclose(loader->file);
    loader->file = NULL;
    loader->length = 0;
    return SPICA_ERROR;
  }

  int status = parse_header(loader, &header);
  if (status != SPICA_OK) {
    fclose(loader->file);
    loader->file = NULL;
    loader->length = 0;
    return status;
  }

  return SPICA_OK;
}

void SSeqLoaderSER_deinit(SSeqLoaderSER_t *loader) {
  if (loader->file) {
    fclose(loader->file);
    loader->file = NULL;
  }
  loader->length = 0;
}

SSeqLoaderSER_t *SSeqLoaderSER_open(const char *fname) {
  SSeqLoaderSER_t *loader = malloc(sizeof(SSeqLoaderSER_t));
  if (loader == NULL) return NULL;

  int status = SSeqLoaderSER_open_at(loader, fname);
  if (status != SPICA_OK) {
    free(loader);
    return NULL;
  }

  return loader;
}

void SSeqLoaderSER_free(SSeqLoaderSER_t *loader) {
  if (loader) {
    SSeqLoaderSER_deinit(loader);
    free(loader);
  }
}
