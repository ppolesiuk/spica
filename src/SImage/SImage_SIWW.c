/* This file is part of Spica, released under MIT license.
 * See LICENSE for details.
 */

/* Author: Piotr Polesiuk, 2022 */

#include "SImage.h"

#include "SDataRepr.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SIWW_MAGIC   "SPICAIWW"
#define SIWW_VERSION 1

#define MAX_SUPPORTED_FORMAT SFmt_SeparateRGB

typedef struct SIWW_header {
  char     magic[8];
  uint32_t version;
  uint16_t header_size;
  uint16_t format;
  uint16_t width;
  uint16_t height;
} SIWW_header_t;

int SImage_loadSIWW_at(SImage_t *image, const char *fname) {
  SImage_init(image, 0, 0, SFmt_Invalid);

  FILE *file = fopen(fname, "rb");
  if (!file) return SPICA_ERROR;

  do {
    SIWW_header_t header;
    if (fread(&header, sizeof(SIWW_header_t), 1, file) != 1) break;

    header.version     = SLittleEndian32(header.version);
    header.header_size = SLittleEndian16(header.header_size);
    header.format      = SLittleEndian16(header.format);
    header.width       = SLittleEndian16(header.width);
    header.height      = SLittleEndian16(header.height);

    if (memcmp(SIWW_MAGIC, header.magic, 8) != 0
      || header.header_size < sizeof(SIWW_header_t)
      || header.format >= MAX_SUPPORTED_FORMAT)
    {
      break;
    }

    if (fseek(file, header.header_size, SEEK_SET)) break;

    SImage_init(image, header.width, header.height, header.format);
    if (image->format == SFmt_Invalid) break;

#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
    fread(image->data, 1, SImage_dataSize(image), file);
#else
#  error unsupported endianness
#endif
  } while (0);

  fclose(file);
  return (image->format ? SPICA_OK : SPICA_ERROR);
}

SImage_t *SImage_loadSIWW(const char *fname) {
  SImage_t *image = malloc(sizeof(SImage_t));
  if (image == NULL) return NULL;

  SImage_loadSIWW_at(image, fname);
  return image;
}

int SImage_saveSIWW(const SImage_t *image, const char *fname) {
  FILE *file = fopen(fname, "wb");
  if (!file) return SPICA_ERROR;

  int status = SPICA_ERROR;

  do {
    /* Write header */
    SIWW_header_t header = { .magic = SIWW_MAGIC };
    header.version     = SLittleEndian32(SIWW_VERSION);
    header.header_size = SLittleEndian16(sizeof(SIWW_header_t));
    header.format      = SLittleEndian16(image->format);
    header.width       = SLittleEndian16(image->width);
    header.height      = SLittleEndian16(image->height);
    if (fwrite(&header, sizeof(SIWW_header_t), 1, file) != 1) break;

#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
    size_t dataSize = SImage_dataSize(image);
    if (fwrite(image->data, 1, dataSize, file) != dataSize) break;
#else
#  error unsupported endianness
#endif

    /* Done */
    status = SPICA_OK;
  } while (0);

  fclose(file);

  return status;
}
