/* This file is part of Spica, released under MIT license.
 * See LICENSE for details.
 */

/* Author: Piotr Polesiuk, 2022 */

#include "SImageIO.h"

#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <png.h>

/* ========================================================================= */
/* READING PNG IMAGE */

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

/* ========================================================================= */
/* WRITING PNG IMAGE */

typedef void (* WriteRowFilter_t)(
  const SImage_t *image, png_byte *tgt, unsigned y);

static int pix8(float v, float w) {
  int result = (int)((v / w) * 256.0f);
  return result < 0 ? 0 : result > 255 ? 255 : result;
}

static int pix16(float v, float w) {
  int result = (int)((v / w) * 65536.0f);
  return result < 0 ? 0 : result > 65535 ? 65535 : result;
}

static void writeFilter_Gray_to_Gray8(
  const SImage_t *image, png_byte *tgt, unsigned y)
{
  unsigned width = image->width;
  SVec2f_t *data = image->data_gray + y * width;

  for (unsigned i = 0; i < width; i++) {
    tgt[i] = pix8(data[i][0], data[i][1]);
  }
}

static void writeFilter_Gray_to_Gray16(
  const SImage_t *image, png_byte *tgt, unsigned y)
{
  unsigned width = image->width;
  SVec2f_t *data = image->data_gray + y * width;

  for (unsigned i = 0; i < width; i++) {
    int v = pix16(data[i][0], data[i][1]);
    tgt[2*i + 0] = v >> 8;
    tgt[2*i + 1] = v & 0xFF;
  }
}

static void writeFilter_RGB_to_Gray8(
  const SImage_t *image, png_byte *tgt, unsigned y)
{
  unsigned width = image->width;
  SVec4f_t *data = image->data_rgb + y * width;

  for (unsigned i = 0; i < width; i++) {
    tgt[i] = pix8(data[i][0] + data[i][1] + data[i][2], data[i][3] * 3.0f);
  }
}

static void writeFilter_RGB_to_Gray16(
  const SImage_t *image, png_byte *tgt, unsigned y)
{
  unsigned width = image->width;
  SVec4f_t *data = image->data_rgb + y * width;

  for (unsigned i = 0; i < width; i++) {
    int v = pix16(data[i][0] + data[i][1] + data[i][2], data[i][3] * 3.0f);
    tgt[2*i + 0] = v >> 8;
    tgt[2*i + 1] = v & 0xFF;
  }
}

static void writeFilter_RGB_to_RGB8(
  const SImage_t *image, png_byte *tgt, unsigned y)
{
  unsigned width = image->width;
  SVec4f_t *data = image->data_rgb + y * width;

  for (unsigned i = 0; i < width; i++) {
    tgt[3*i + 0] = pix8(data[i][0], data[i][3]);
    tgt[3*i + 1] = pix8(data[i][1], data[i][3]);
    tgt[3*i + 2] = pix8(data[i][2], data[i][3]);
  }
}

static void writeFilter_RGB_to_RGB16(
  const SImage_t *image, png_byte *tgt, unsigned y)
{
  unsigned width = image->width;
  SVec4f_t *data = image->data_rgb + y * width;

  for (unsigned i = 0; i < width; i++) {
    int r = pix16(data[i][0], data[i][3]);
    int g = pix16(data[i][1], data[i][3]);
    int b = pix16(data[i][2], data[i][3]);
    tgt[6*i + 0] = r >> 8;
    tgt[6*i + 1] = r & 0xFF;
    tgt[6*i + 2] = g >> 8;
    tgt[6*i + 3] = g & 0xFF;
    tgt[6*i + 4] = b >> 8;
    tgt[6*i + 5] = b & 0xFF;
  }
}

static void writeFilter_SeparateRGB_to_Gray8(
  const SImage_t *image, png_byte *tgt, unsigned y)
{
  unsigned width = image->width;
  SVec2f_t *rdata = SImage_rowRed(image, y);
  SVec2f_t *gdata = SImage_rowGreen(image, y);
  SVec2f_t *bdata = SImage_rowBlue(image, y);

  for (unsigned i = 0; i < width; i++) {
    tgt[i] = pix8(
      rdata[i][0] + gdata[i][0] + bdata[i][0],
      rdata[i][1] + gdata[i][1] + bdata[i][1]);
  }
}

static void writeFilter_SeparateRGB_to_Gray16(
  const SImage_t *image, png_byte *tgt, unsigned y)
{
  unsigned width = image->width;
  SVec2f_t *rdata = SImage_rowRed(image, y);
  SVec2f_t *gdata = SImage_rowGreen(image, y);
  SVec2f_t *bdata = SImage_rowBlue(image, y);

  for (unsigned i = 0; i < width; i++) {
    int v = pix16(
      rdata[i][0] + gdata[i][0] + bdata[i][0],
      rdata[i][1] + gdata[i][1] + bdata[i][1]);
    tgt[2*i + 0] = v >> 8;
    tgt[2*i + 1] = v & 0xFF;
  }
}

static void writeFilter_SeparateRGB_to_RGB8(
  const SImage_t *image, png_byte *tgt, unsigned y)
{
  unsigned width = image->width;
  SVec2f_t *rdata = SImage_rowRed(image, y);
  SVec2f_t *gdata = SImage_rowGreen(image, y);
  SVec2f_t *bdata = SImage_rowBlue(image, y);

  for (unsigned i = 0; i < width; i++) {
    tgt[3*i + 0] = pix8(rdata[i][0], rdata[i][1]);
    tgt[3*i + 1] = pix8(gdata[i][0], gdata[i][1]);
    tgt[3*i + 2] = pix8(bdata[i][0], bdata[i][1]);
  }
}

static void writeFilter_SeparateRGB_to_RGB16(
  const SImage_t *image, png_byte *tgt, unsigned y)
{
  unsigned width = image->width;
  SVec2f_t *rdata = SImage_rowRed(image, y);
  SVec2f_t *gdata = SImage_rowGreen(image, y);
  SVec2f_t *bdata = SImage_rowBlue(image, y);

  for (unsigned i = 0; i < width; i++) {
    int r = pix16(rdata[i][0], rdata[i][1]);
    int g = pix16(gdata[i][0], gdata[i][1]);
    int b = pix16(bdata[i][0], bdata[i][1]);
    tgt[6*i + 0] = r >> 8;
    tgt[6*i + 1] = r & 0xFF;
    tgt[6*i + 2] = g >> 8;
    tgt[6*i + 3] = g & 0xFF;
    tgt[6*i + 4] = b >> 8;
    tgt[6*i + 5] = b & 0xFF;
  }
}

static int savePNG_generic(const SImage_t *image, const char *fname,
  int color_type, int bit_depth, int pixel_size,
  WriteRowFilter_t write_filter)
{
  FILE       *fp       = NULL;
  png_structp png_ptr  = NULL;
  png_infop   info_ptr = NULL;
  png_bytep   row      = NULL;

  unsigned width  = image->width;
  unsigned height = image->height;
  
  volatile int result = SPICA_ERROR;

  do {
    /* Open file */
    fp = fopen(fname, "wb");
    if (!fp) break;

    /* Initialize png_ptr */
    png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png_ptr) break;

    /* Initialize info_ptr */
    info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr) break;

    /* Setup error handler */
    if (setjmp(png_jmpbuf(png_ptr))) break;

    png_init_io(png_ptr, fp);
    png_set_IHDR(png_ptr, info_ptr,
      width, height,
      bit_depth, color_type, PNG_INTERLACE_NONE,
      PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);

    png_write_info(png_ptr, info_ptr);

    /* Allocate memory for single row */
    row = malloc(pixel_size * width * sizeof(png_byte));

    /* Write data */
    for (unsigned y = 0; y < height; y++) {
      write_filter(image, row, y);
      png_write_row(png_ptr, row);
    }

    png_write_end(png_ptr, NULL);
    result = SPICA_OK;
  } while (0);

  /* Free resources */
  if (row != NULL)      free(row);
  if (info_ptr != NULL) png_free_data(png_ptr, info_ptr, PNG_FREE_ALL, -1);
  if (png_ptr != NULL)  png_destroy_write_struct(&png_ptr, NULL);
  if (fp != NULL)       fclose(fp);

  return result;
}

static int savePNG_Gray8(const SImage_t *image, const char *fname) {
  WriteRowFilter_t filter;

  switch (image->format) {
  case SFmt_Gray:
    filter = writeFilter_Gray_to_Gray8;
    break;
  case SFmt_RGB:
    filter = writeFilter_RGB_to_Gray8;
    break;
  case SFmt_SeparateRGB:
    filter = writeFilter_SeparateRGB_to_Gray8;
    break;
  case SFmt_Invalid:
    /* Image is empty */
    return SPICA_ERROR;
  default:
    assert(0 && "Impossible case");
    return SPICA_ERROR;
  }

  return savePNG_generic(image, fname, PNG_COLOR_TYPE_GRAY, 8, 1, filter);
}

static int savePNG_Gray16(const SImage_t *image, const char *fname) {
  WriteRowFilter_t filter;

  switch (image->format) {
  case SFmt_Gray:
    filter = writeFilter_Gray_to_Gray16;
    break;
  case SFmt_RGB:
    filter = writeFilter_RGB_to_Gray16;
    break;
  case SFmt_SeparateRGB:
    filter = writeFilter_SeparateRGB_to_Gray16;
    break;
  case SFmt_Invalid:
    /* Image is empty */
    return SPICA_ERROR;
  default:
    assert(0 && "Impossible case");
    return SPICA_ERROR;
  }

  return savePNG_generic(image, fname, PNG_COLOR_TYPE_GRAY, 16, 2, filter);
}

static int savePNG_RGB8(const SImage_t *image, const char *fname) {
  WriteRowFilter_t filter;

  switch (image->format) {
  case SFmt_Gray:
  case SFmt_SeparateRGB:
    filter = writeFilter_SeparateRGB_to_RGB8;
    break;
  case SFmt_RGB:
    filter = writeFilter_RGB_to_RGB8;
    break;
  case SFmt_Invalid:
    /* Image is empty */
    return SPICA_ERROR;
  default:
    assert(0 && "Impossible case");
    return SPICA_ERROR;
  }

  return savePNG_generic(image, fname, PNG_COLOR_TYPE_RGB, 8, 3, filter);
}

static int savePNG_RGB16(const SImage_t *image, const char *fname) {
  WriteRowFilter_t filter;

  switch (image->format) {
  case SFmt_Gray:
  case SFmt_SeparateRGB:
    filter = writeFilter_SeparateRGB_to_RGB16;
    break;
  case SFmt_RGB:
    filter = writeFilter_RGB_to_RGB16;
    break;
  case SFmt_Invalid:
    /* Image is empty */
    return SPICA_ERROR;
  default:
    assert(0 && "Impossible case");
    return SPICA_ERROR;
  }

  return savePNG_generic(image, fname, PNG_COLOR_TYPE_RGB, 16, 6, filter);
}

int SImage_savePNG(
  const SImage_t *image,
  SPixFormat_t    format,
  const char     *fname)
{
  switch (format) {
  case SPF_Gray8:
    return savePNG_Gray8(image, fname);
  case SPF_Gray16:
    return savePNG_Gray16(image, fname);
  case SPF_RGB8:
    return savePNG_RGB8(image, fname);
  case SPF_RGB16:
    return savePNG_RGB16(image, fname);
  default:
    assert(0 && "Impossible case");
    return SPICA_ERROR;
  }
}
