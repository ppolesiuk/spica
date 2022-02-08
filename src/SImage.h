/* This file is part of Spica, released under MIT license.
 * See LICENSE for details.
 */

/* Author: Piotr Polesiuk, 2022 */

/** \file SImage.h
 *  \brief Raw images without metadata
 *
 * Representation of Spica images is optimized for image stacking. Raw images
 * (SImage_t) are represented as arrays of pixels. Each pixel consists of its
 * value for each channel (gray or RGB) and weight. Weight is always the last
 * component of a vector that represents pixel data. The real value of a pixel
 * is a quotient of channel values and the weight. If weight is equal to 0,
 * other channels alos should be zero. When images are stacked, corresponding
 * pixels are added (as vectors), which results in computing weighted-mean
 * of their values.
 */

#ifndef __SPICA_IMAGE_H__
#define __SPICA_IMAGE_H__

#include "SVec.h"

#include <stddef.h>

/** \brief Pixel format of a SImage_t */
typedef enum SImageFormat {
  /** Invalid SImage_t -- it contains no data */
  SFmt_Invalid = 0,

  /** Gray-scale SImage_t. Each pixel is represented as a magnitude-wieght
   * \ref SVec2f_t vector */
  SFmt_Gray,

  /** Color SImage_t. Each pixel is represented as a red-green-blue-weight
   * \ref SVec4f_t vector */
  SFmt_RGB,

  /** Color SImage_t, that consists of three gray-scale images: red, green,
   * and blue */
  SFmt_SeparateRGB

} SImageFormat_t;

/** \brief Raw image without metadata. */
typedef struct SImage {
  /** \brief Image width */
  unsigned       width;
  /** \brief Image height */
  unsigned       height;
  /** \brief Image format */
  SImageFormat_t format;
  union {
    /** \brief Image data */
    void     *data;
    /** \brief Image data (for \ref SFmt_Gray images) */
    SVec2f_t *data_gray;
    /** \brief Image data (for \ref SFmt_RGB images) */
    SVec4f_t *data_rgb;
    /** \brief Image data (for \ref SFmt_SeparateRGB images)
     *
     * Data is organized as three consecutive arrays (red, green, blue)
     * that occupy a continous block in the memory. */
    SVec2f_t *data_red;
  };
} SImage_t;

/** \brief Initialize already allocated SImage_t
 *
 * To deinitialize it, call \ref SImage_deinit function.
 *
 * When \p width or \p height are invalid, or system is unable to allocate
 * needed memory, the \p format argument is ignored and set to
 * \ref SFmt_Invalid.
 *
 * \param image Pointer to already allocated SImage_t.
 * \param width  Width of an image (in pixels).
 * \param height Height of an image (in pixels).
 * \param format Format of an image. It may be ignored on error. */
void SImage_init(
  SImage_t      *image,
  unsigned       width,
  unsigned       height,
  SImageFormat_t format);

/** \brief Deinitialize SImage_t initialized by \ref SImage_init
 *
 * This function frees only internal resources used by SImage_t. It does
 * not free the memory occupied by SImage_t itself.
 *
 * \param image Pointer to SImage_t to be deinitialized */
void SImage_deinit(
  SImage_t *image);

/** \brief Allocate and initialize new SImage_t
 *
 * When \p width or \p height are invalid, or system is unable to allocate
 * needed memory, the \p format argument is ignored and set to
 * \ref SFmt_Invalid.
 *
 * \param width  Width of an image (in pixels).
 * \param height Height of an image (in pixels).
 * \param format Format of an image. It may be ignored on error.
 *
 * \return Pointer to the newly allocated image, or NULL on malloc error.
 *   The image can be freed with \ref SImage_free function. */
SImage_t *SImage_alloc(
  unsigned       width,
  unsigned       height,
  SImageFormat_t format);

/** \brief Free image previously allocated with \ref SImage_alloc
 *
 * \param image Pointer to the image. It may be NULL. */
void SImage_free(
  SImage_t *image);

/** \brief Pointer to given image row
 *
 * \param image Image 
 * \param y     The number of a row
 *
 * \return Pointer to image row data. For \ref SFmt_Invalid or
 *   \ref SFmt_SeparateRGB images returns NULL. */
void *SImage_row(
  const SImage_t *image,
  unsigned        y);

/** \brief Pointer to given image row for the red channel
 *
 * \param image Image
 * \param y     The number of a row
 *
 * \return Pointer to image row data for the red channel. For \ref SFmt_Gray
 *  returns pointer to \p y -th row. For \ref SFmt_Invalid or \ref SFmt_RGB
 *  returns NULL. */
SVec2f_t *SImage_rowRed(
  const SImage_t *image,
  unsigned        y);

/** \brief Pointer to given image row for the green channel
 *
 * \param image Image
 * \param y     The number of a row
 *
 * \return Pointer to image row data for the green channel. For \ref SFmt_Gray
 *  returns pointer to \p y -th row. For \ref SFmt_Invalid or \ref SFmt_RGB
 *  returns NULL. */
SVec2f_t *SImage_rowGreen(
  const SImage_t *image,
  unsigned        y);

/** \brief Pointer to given image row for the blue channel
 *
 * \param image Image
 * \param y     The number of a row
 *
 * \return Pointer to image row data for the blue channel. For \ref SFmt_Gray
 *  returns pointer to \p y -th row. For \ref SFmt_Invalid or \ref SFmt_RGB
 *  returns NULL. */
SVec2f_t *SImage_rowBlue(
  const SImage_t *image,
  unsigned        y);

#endif /* __SPICA_IMAGE_H__ */
