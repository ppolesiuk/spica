/* This file is part of Spica, released under MIT license.
 * See LICENSE for details.
 */

/* Author: Piotr Polesiuk, 2025 */

#include "SSeqLoaderSER.h"
#include "SSeqLoaderSER_common.h"

#include "SImage.h"
#include "SDataRepr.h"

#include <assert.h>
#include <stdlib.h>

typedef void (* ReadRowFilter_t)(
  SImage_t *image, unsigned row, float data, const void *src);

static float readByteFilter(uint8_t data, float filter_data) {
  return (data + filter_data) / 256.0f;
}

static float readWordFilter(uint16_t data, float filter_data) {
  return (data + 0.5f) * filter_data;
}

static void readFilter_Mono8(
  SImage_t *image, unsigned row, float data, const void *src)
{
  uint8_t  *src_ptr = (uint8_t *)src;
  SVec2f_t *tgt_ptr = (SVec2f_t *)SImage_row(image, row);
  unsigned width = image->width;
  for (unsigned x = 0; x < width; x++) {
    tgt_ptr[x] = (SVec2f_t){ readByteFilter(src_ptr[x], data), 1.0f };
  }
}

static void readFilter_Mono16(
  SImage_t *image, unsigned row, float data, const void *src)
{
  uint16_t *src_ptr = (uint16_t *)src;
  SVec2f_t *tgt_ptr = (SVec2f_t *)SImage_row(image, row);
  unsigned width = image->width;
  for (unsigned x = 0; x < width; x++) {
    tgt_ptr[x] = (SVec2f_t){ readWordFilter(src_ptr[x], data), 1.0f };
  }
}

static void readFilter_Bayer8(
  SVec2f_t *tgt_even, SVec2f_t *tgt_odd,
  const uint8_t *src, unsigned width, float data)
{
  for (unsigned x = 0; x < width; x++) {
    tgt_even[x] = (SVec2f_t){ readByteFilter(src[x], data), 1.0f };
    if (++x >= width) break;
    tgt_odd[x]  = (SVec2f_t){ readByteFilter(src[x], data), 1.0f };
  }
}

static void readFilter_Bayer16(
  SVec2f_t *tgt_even, SVec2f_t *tgt_odd,
  const uint16_t *src, unsigned width, float data)
{
  for (unsigned x = 0; x < width; x++) {
    tgt_even[x] = (SVec2f_t){ readWordFilter(src[x], data), 1.0f };
    if (++x >= width) break;
    tgt_odd[x]  = (SVec2f_t){ readWordFilter(src[x], data), 1.0f };
  }
}

static void readFilter_BayerRGGB8(
  SImage_t *image, unsigned row, float data, const void *src)
{
  uint8_t  *src_ptr  = (uint8_t *)src;
  SVec2f_t *tgt_even;
  SVec2f_t *tgt_odd;
  if ((row & 1) == 0) {
    tgt_even = SImage_rowRed(image, row);
    tgt_odd  = SImage_rowGreen(image, row);
  } else {
    tgt_even = SImage_rowGreen(image, row);
    tgt_odd  = SImage_rowBlue(image, row);
  }
  readFilter_Bayer8(tgt_even, tgt_odd, src_ptr, image->width, data);
}

static void readFilter_BayerRGGB16(
  SImage_t *image, unsigned row, float data, const void *src)
{
  uint16_t *src_ptr  = (uint16_t *)src;
  SVec2f_t *tgt_even;
  SVec2f_t *tgt_odd;
  if ((row & 1) == 0) {
    tgt_even = SImage_rowRed(image, row);
    tgt_odd  = SImage_rowGreen(image, row);
  } else {
    tgt_even = SImage_rowGreen(image, row);
    tgt_odd  = SImage_rowBlue(image, row);
  }
  readFilter_Bayer16(tgt_even, tgt_odd, src_ptr, image->width, data);
}

static void readFilter_BayerGRBG8(
  SImage_t *image, unsigned row, float data, const void *src)
{
  uint8_t  *src_ptr  = (uint8_t *)src;
  SVec2f_t *tgt_even;
  SVec2f_t *tgt_odd;
  if ((row & 1) == 0) {
    tgt_even = SImage_rowGreen(image, row);
    tgt_odd  = SImage_rowRed(image, row);
  } else {
    tgt_even = SImage_rowBlue(image, row);
    tgt_odd  = SImage_rowGreen(image, row);
  }
  readFilter_Bayer8(tgt_even, tgt_odd, src_ptr, image->width, data);
}

static void readFilter_BayerGRBG16(
  SImage_t *image, unsigned row, float data, const void *src)
{
  uint16_t *src_ptr  = (uint16_t *)src;
  SVec2f_t *tgt_even;
  SVec2f_t *tgt_odd;
  if ((row & 1) == 0) {
    tgt_even = SImage_rowGreen(image, row);
    tgt_odd  = SImage_rowRed(image, row);
  } else {
    tgt_even = SImage_rowBlue(image, row);
    tgt_odd  = SImage_rowGreen(image, row);
  }
  readFilter_Bayer16(tgt_even, tgt_odd, src_ptr, image->width, data);
}

static void readFilter_BayerGBRG8(
  SImage_t *image, unsigned row, float data, const void *src)
{
  uint8_t  *src_ptr  = (uint8_t *)src;
  SVec2f_t *tgt_even;
  SVec2f_t *tgt_odd;
  if ((row & 1) == 0) {
    tgt_even = SImage_rowGreen(image, row);
    tgt_odd  = SImage_rowBlue(image, row);
  } else {
    tgt_even = SImage_rowRed(image, row);
    tgt_odd  = SImage_rowGreen(image, row);
  }
  readFilter_Bayer8(tgt_even, tgt_odd, src_ptr, image->width, data);
}

static void readFilter_BayerGBRG16(
  SImage_t *image, unsigned row, float data, const void *src)
{
  uint16_t *src_ptr  = (uint16_t *)src;
  SVec2f_t *tgt_even;
  SVec2f_t *tgt_odd;
  if ((row & 1) == 0) {
    tgt_even = SImage_rowGreen(image, row);
    tgt_odd  = SImage_rowBlue(image, row);
  } else {
    tgt_even = SImage_rowRed(image, row);
    tgt_odd  = SImage_rowGreen(image, row);
  }
  readFilter_Bayer16(tgt_even, tgt_odd, src_ptr, image->width, data);
}

static void readFilter_BayerBGGR8(
  SImage_t *image, unsigned row, float data, const void *src)
{
  uint8_t  *src_ptr  = (uint8_t *)src;
  SVec2f_t *tgt_even;
  SVec2f_t *tgt_odd;
  if ((row & 1) == 0) {
    tgt_even = SImage_rowBlue(image, row);
    tgt_odd  = SImage_rowGreen(image, row);
  } else {
    tgt_even = SImage_rowGreen(image, row);
    tgt_odd  = SImage_rowRed(image, row);
  }
  readFilter_Bayer8(tgt_even, tgt_odd, src_ptr, image->width, data);
}

static void readFilter_BayerBGGR16(
  SImage_t *image, unsigned row, float data, const void *src)
{
  uint16_t *src_ptr  = (uint16_t *)src;
  SVec2f_t *tgt_even;
  SVec2f_t *tgt_odd;
  if ((row & 1) == 0) {
    tgt_even = SImage_rowBlue(image, row);
    tgt_odd  = SImage_rowGreen(image, row);
  } else {
    tgt_even = SImage_rowGreen(image, row);
    tgt_odd  = SImage_rowRed(image, row);
  }
  readFilter_Bayer16(tgt_even, tgt_odd, src_ptr, image->width, data);
}

static void readFilter_RGB8(
  SImage_t *image, unsigned row, float data, const void *src)
{
  uint8_t  *src_ptr = (uint8_t *)src;
  SVec4f_t *tgt_ptr = (SVec4f_t *)SImage_row(image, row);
  unsigned width = image->width;
  for (unsigned x = 0; x < width; x++) {
    tgt_ptr[x] = (SVec4f_t){
      readByteFilter(src_ptr[3*x + 0], data),
      readByteFilter(src_ptr[3*x + 1], data),
      readByteFilter(src_ptr[3*x + 2], data),
      1.0f
    };
  }
}

static void readFilter_RGB16(
  SImage_t *image, unsigned row, float data, const void *src)
{
  uint16_t *src_ptr = (uint16_t *)src;
  SVec4f_t *tgt_ptr = (SVec4f_t *)SImage_row(image, row);
  unsigned width = image->width;
  for (unsigned x = 0; x < width; x++) {
    tgt_ptr[x] = (SVec4f_t){
      readWordFilter(src_ptr[3*x + 0], data),
      readWordFilter(src_ptr[3*x + 1], data),
      readWordFilter(src_ptr[3*x + 2], data),
      1.0f
    };
  }
}

static void readFilter_BGR8(
  SImage_t *image, unsigned row, float data, const void *src)
{
  uint8_t  *src_ptr = (uint8_t *)src;
  SVec4f_t *tgt_ptr = (SVec4f_t *)SImage_row(image, row);
  unsigned width = image->width;
  for (unsigned x = 0; x < width; x++) {
    tgt_ptr[x] = (SVec4f_t){
      readByteFilter(src_ptr[3*x + 2], data),
      readByteFilter(src_ptr[3*x + 1], data),
      readByteFilter(src_ptr[3*x + 0], data),
      1.0f
    };
  }
}

static void readFilter_BGR16(
  SImage_t *image, unsigned row, float data, const void *src)
{
  uint16_t *src_ptr = (uint16_t *)src;
  SVec4f_t *tgt_ptr = (SVec4f_t *)SImage_row(image, row);
  unsigned width = image->width;
  for (unsigned x = 0; x < width; x++) {
    tgt_ptr[x] = (SVec4f_t){
      readWordFilter(src_ptr[3*x + 2], data),
      readWordFilter(src_ptr[3*x + 1], data),
      readWordFilter(src_ptr[3*x + 0], data),
      1.0f
    };
  }
}

/* ========================================================================= */

static void swap_bytes(void *data, unsigned width) {
  assert((width % 2) == 0);
  width /= 2;
  uint16_t *ptr = (uint16_t *)data;
  for (unsigned i = 0; i < width; i++) {
    ptr[i] = SSwapBytes16(ptr[i]);
  }
}

/* ========================================================================= */

int SSeqLoaderSER_loadImage_at(
  const SSeqLoaderSER_t *loader,
  SImage_t              *image,
  unsigned               index)
{
  if (!loader->file || index >= loader->length) {
    return SPICA_ERROR;
  }

  size_t row_size = (size_t)loader->width * loader->bytes_per_pixel;
  size_t offset   = SER_HEADER_SIZE + index * row_size * loader->height;

  if (fseek(loader->file, offset, SEEK_SET) != 0) {
    return SPICA_ERROR;
  }

  SImageFormat_t  format;
  ReadRowFilter_t read_filter;
  switch (loader->format) {
  case SSERFormat_Mono:
    format      = SFmt_Gray;
    read_filter = (loader->bits_per_pixel <= 8) ?
      readFilter_Mono8 : readFilter_Mono16;
    break;
  case SSERFormat_Bayer_RGGB:
    format = SFmt_SeparateRGB;
    read_filter = (loader->bits_per_pixel <= 8) ?
      readFilter_BayerRGGB8 : readFilter_BayerRGGB16;
    break;
  case SSERFormat_Bayer_GRBG:
    format = SFmt_SeparateRGB;
    read_filter = (loader->bits_per_pixel <= 8) ?
      readFilter_BayerGRBG8 : readFilter_BayerGRBG16;
    break;
  case SSERFormat_Bayer_GBRG:
    format = SFmt_SeparateRGB;
    read_filter = (loader->bits_per_pixel <= 8) ?
      readFilter_BayerGBRG8 : readFilter_BayerGBRG16;
    break;
  case SSERFormat_Bayer_BGGR:
    format = SFmt_SeparateRGB;
    read_filter = (loader->bits_per_pixel <= 8) ?
      readFilter_BayerBGGR8 : readFilter_BayerBGGR16;
    break;
  case SSERFormat_RGB:
    format = SFmt_RGB;
    read_filter = (loader->bits_per_pixel <= 8) ?
      readFilter_RGB8 : readFilter_RGB16;
    break;
  case SSERFormat_BGR:
    format = SFmt_RGB;
    read_filter = (loader->bits_per_pixel <= 8) ?
      readFilter_BGR8 : readFilter_BGR16;
    break;
  default:
    return SPICA_ERROR; // unsupported format
  }

  /* Since one and two byte values have different alignment in SER, the
   * meaning of filter_data is different for 8-bit and 16-bit images.
   *
   * For 8-bit images (left aligned), filter_data is the value that
   * should be added to each pixel, in order to obtain the value exactly
   * in the middle of the range.
   *
   * For 16-bit images (right aligned), filter_data is the value that should
   * be multiplied by each pixel, in order to obtain correct value. */
  float filter_data;
  if (loader->bits_per_pixel <= 8) {
    filter_data = 128.0f / (1 << loader->bits_per_pixel);
  } else {
    filter_data = (1 << (16 - loader->bits_per_pixel)) / 65536.0f;
  }

  SImage_init(image, loader->width, loader->height, format);
  if (image->format == SFmt_Invalid) {
    return SPICA_ERROR; /* failed to initialize image */
  }

  /* Bayer read filters sets data selecively, so we need to clear the image
   * before reading the data. */
  switch (loader->format) {
  case SSERFormat_Bayer_RGGB:
  case SSERFormat_Bayer_GRBG:
  case SSERFormat_Bayer_GBRG:
  case SSERFormat_Bayer_BGGR:
    SImage_clear(image);
    break;
  default:
    break; /* no need to clear for other formats */
  }

  void *row = malloc((size_t)loader->width * loader->bytes_per_pixel);
  if (!row) {
    SImage_deinit(image);
    return SPICA_ERROR; /* memory allocation failed */
  }

  for (unsigned y = 0; y < loader->height; y++) {
    if (fread(row, row_size, 1, loader->file) != 1) {
      free(row);
      SImage_deinit(image);
      return SPICA_ERROR; /* failed to read image row */
    }
    if (loader->swap) {
      swap_bytes(row, row_size);
    }

    read_filter(image, y, filter_data, row);
  }

  free(row);
  return SPICA_OK;
}

SImage_t *SSeqLoaderSER_loadImage(
  const SSeqLoaderSER_t *loader,
  unsigned               index)
{
  SImage_t *image = malloc(sizeof(SImage_t));
  if (image == NULL) return NULL;

  if (SSeqLoaderSER_loadImage_at(loader, image, index) != SPICA_OK) {
    free(image);
    return NULL; /* failed to load image */
  }
  return image;
}
