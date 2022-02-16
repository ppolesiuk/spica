/* This file is part of Spica, released under MIT license.
 * See LICENSE for details.
 */

/* Author: Piotr Polesiuk, 2022 */

#include "SImage.h"

#include <stdint.h>
#include <stdlib.h>
#include <png.h>

typedef void (* ReadRowFilter_t)(
  void *tgt, const png_byte *src, unsigned width);

static float readByteFilter(png_byte data) {
  return (data + 0.5f) / 256.0f;
}

static float readWordFilter(png_byte data_hi, png_byte data_lo) {
  uint16_t data = data_hi;
  data <<= 8;
  data |= data_lo;
  return (data + 0.5f) / 65536.0f;
}

static void readFilter_Gray8(void *tgt, const png_byte *src, unsigned width) {
  SVec2f_t *data = tgt;
  for (unsigned x = 0; x < width; x++) {
    SVec2f_t pix = { readByteFilter(src[x]), 1.0f };
    data[x] = pix;
  }
}

static void readFilter_Gray16(void *tgt, const png_byte *src, unsigned width) {
  SVec2f_t *data = tgt;
  for (unsigned x = 0; x < width; x++) {
    SVec2f_t pix = { readWordFilter(src[x*2 + 0], src[x*2 + 1]), 1.0f };
    data[x] = pix;
  }
}

static void readFilter_RGB8(void *tgt, const png_byte *src, unsigned width) {
  SVec4f_t *data = tgt;
  for (unsigned x = 0; x < width; x++) {
    SVec4f_t pix = {
      readByteFilter(src[3*x + 0]),
      readByteFilter(src[3*x + 1]),
      readByteFilter(src[3*x + 2]),
      1.0f };
    data[x] = pix;
  }
}

static void readFilter_RGB16(void *tgt, const png_byte *src, unsigned width) {
  SVec4f_t *data = tgt;
  for (unsigned x = 0; x < width; x++) {
    SVec4f_t pix = {
      readWordFilter(src[6*x + 0], src[6*x + 1]),
      readWordFilter(src[6*x + 2], src[6*x + 3]),
      readWordFilter(src[6*x + 4], src[6*x + 5]),
      1.0f };
    data[x] = pix;
  }
}

int SImage_loadPNG_at(SImage_t *image, const char *fname) {
  png_byte header[8];
  FILE       *fp       = NULL;
  png_structp png_ptr  = NULL;
  png_infop   info_ptr = NULL;
  png_bytep   row      = NULL;

  SImage_init(image, 0, 0, SFmt_Invalid);

  do {
    /* Open file */
    fp = fopen(fname, "rb");
    if (!fp) break;

    /* Check signature */
    if (fread(header, sizeof(png_byte), 8, fp) != 8
      || png_sig_cmp(header, 0, 8))
    {
      SImage_deinit(image);
      SImage_init(image, 0, 0, SFmt_Invalid);
      break;
    }

    /* Initialize png_ptr */
    png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png_ptr) break;

    /* Initialize info_ptr */
    info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr) break;

    /* Setup error handler */
    if (setjmp(png_jmpbuf(png_ptr))) break;

    /* Read info */
    png_init_io(png_ptr, fp);
    png_set_sig_bytes(png_ptr, 8);
    png_read_info(png_ptr, info_ptr);

    unsigned width  = png_get_image_width(png_ptr, info_ptr);
    unsigned height = png_get_image_height(png_ptr, info_ptr);
    int color_type  = png_get_color_type(png_ptr, info_ptr);
    int bit_depth   = png_get_bit_depth(png_ptr, info_ptr);

    ReadRowFilter_t filter;
    SImageFormat_t  format;

    if (color_type == PNG_COLOR_TYPE_RGB && bit_depth == 8) {
      row    = malloc(3 * width * sizeof(png_byte));
      filter = readFilter_RGB8;
      format = SFmt_RGB;
    } else if (color_type == PNG_COLOR_TYPE_RGB && bit_depth == 16) {
      row    = malloc(6 * width * sizeof(png_byte));
      filter = readFilter_RGB16;
      format = SFmt_RGB;
    } else if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth == 8) {
      row    = malloc(width * sizeof(png_byte));
      filter = readFilter_Gray8;
      format = SFmt_Gray;
    } else if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth == 16) {
      row    = malloc(2 * width * sizeof(png_byte));
      filter = readFilter_Gray16;
      format = SFmt_Gray;
    } else {
      /* Unsupported format */
      break;
    }

    /* If no memory for a single row */
    if (row == NULL) break;

    /* Allocate image */
    SImage_init(image, width, height, format);
    if (image->format == SFmt_Invalid) break;

    /* Read image data */
    for (unsigned y = 0; y < height; y++) {
      png_read_row(png_ptr, row, NULL);
      filter(SImage_row(image, y), row, width);
    }
  } while (0);

  /* Free resources */
  if (row != NULL)      free(row);
  if (info_ptr != NULL) png_free_data(png_ptr, info_ptr, PNG_FREE_ALL, -1);
  if (png_ptr  != NULL) png_destroy_read_struct(&png_ptr, NULL, NULL);
  if (fp != NULL)       fclose(fp);

  return (image->format ? SPICA_OK : SPICA_ERROR);
}

SImage_t *SImage_loadPNG(const char *fname) {
  SImage_t *image = malloc(sizeof(SImage_t));
  if (image == NULL) return NULL;

  SImage_loadPNG_at(image, fname);
  return image;
}
