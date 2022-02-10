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
 * (called normalized value or normalized brightness) is a quotient of channel
 * values and the weight. If weight is equal to 0, other channels alos should
 * be zero. When images are stacked, corresponding pixels are added (as
 * vectors), which results in computing weighted-mean of their values.
 *
 * The values of the pixels usually have values between zero (black) and
 * weight (white), but it is possible to have values out of this range.
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
 * Freshly initialized image contains uninitialized data. It can be
 * initialized using \ref SImage_clear function.
 *
 * \param image Pointer to already allocated SImage_t.
 * \param width  Width of an image (in pixels).
 * \param height Height of an image (in pixels).
 * \param format Format of an image. It may be ignored on error.
 *
 * \sa SImage_alloc */
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
 * \param image Pointer to SImage_t to be deinitialized
 *
 * \sa SImage_free */
void SImage_deinit(
  SImage_t *image);

/** \brief Allocate and initialize new SImage_t
 *
 * When \p width or \p height are invalid, or system is unable to allocate
 * needed memory, the \p format argument is ignored and set to
 * \ref SFmt_Invalid.
 *
 * The new image contains uninitialized data. It can be initialized using
 * \ref SImage_clear function.
 *
 * \param width  Width of an image (in pixels).
 * \param height Height of an image (in pixels).
 * \param format Format of an image. It may be ignored on error.
 *
 * \return Pointer to the newly allocated image, or NULL on malloc error.
 *   The image can be freed with \ref SImage_free function.
 *
 * \sa SImage_init */
SImage_t *SImage_alloc(
  unsigned       width,
  unsigned       height,
  SImageFormat_t format);

/** \brief Free image previously allocated with \ref SImage_alloc
 *
 * \param image Pointer to the image. It may be NULL.
 *
 * \sa SImage_deinit */
void SImage_free(
  SImage_t *image);

/** \brief Convert image format and strore result in already allocated SImage_t
 *
 * \param dst Pointer to the destination SImage_t structure. The function will
 *   initialize this memory using \ref SImage_init function. If \p dst already
 *   contains a valid image, the \ref SImage_deinit should be called first.
 * \param image Source image
 * \param format Requested format
 *
 * \sa SImage_toFormat */
void SImage_toFormat_at(
  SImage_t       *dst,
  const SImage_t *image,
  SImageFormat_t  format);

/** \brief Convert image to requested format
 *
 * \param image Source image
 * \param format Requested format
 *
 * \return pointer to the newly allocated image that contains the same data in
 *   requested format. The new image should be freed using \ref SImage_free
 *   function.
 *
 * \sa SImage_toFormat_at */
SImage_t *SImage_toFormat(
  const SImage_t *image,
  SImageFormat_t  format);

/** \brief Clear the image contents, i.e., sets weights of all pixels to zero
 *
 * \param image Image to be cleared
 *
 * \sa SImage_clearBlack, SImage_clearWhite */
void SImage_clear(
  SImage_t *image);

/** \brief Clear the image with the black color.
 *
 * This functions sets all pixel values to zero, and all pixel weights to one.
 *
 * \param image Image to be cleared
 *
 * \sa SImage_clear, SImage_clearWhite */
void SImage_clearBlack(
  SImage_t *image);

/** \brief Clear the image with the white color.
 *
 * This functions sets all pixel values and weights to one.
 *
 * \param image Image to be cleared
 *
 * \sa SImage_clear, SImage_clearBlack */
void SImage_clearWhite(
  SImage_t *image);

/** \brief Get the size of memory occupied by the image data (in bytes)
 *
 * \return The size of the array used to store image data (in bytes) */
size_t SImage_dataSize(
  const SImage_t *image);

/** \brief Pointer to data of the red channel
 *
 * \return Pointer to an array that represents the red channel. For
 *  \ref SFmt_Gray returns pointer to data. For \ref SFmt_Invalid or
 *  \ref SFmt_RGB returns NULL
 *
 * \sa SImage_rowRed, SImage_dataGreen, SImage_dataBlue */
SVec2f_t *SImage_dataRed(
  const SImage_t *image);

/** \brief Pointer to data of the green channel
 *
 * \return Pointer to an array that represents the green channel. For
 *  \ref SFmt_Gray returns pointer to data. For \ref SFmt_Invalid or
 *  \ref SFmt_RGB returns NULL
 *
 * \sa SImage_rowGreen, SImage_dataRed, SImage_dataBlue */
SVec2f_t *SImage_dataGreen(
  const SImage_t *image);

/** \brief Pointer to data of the blue channel
 *
 * \return Pointer to an array that represents the blue channel. For
 *  \ref SFmt_Gray returns pointer to data. For \ref SFmt_Invalid or
 *  \ref SFmt_RGB returns NULL
 *
 * \sa SImage_rowBlue, SImage_dataRed, SImage_dataGreen */
SVec2f_t *SImage_dataBlue(
  const SImage_t *image);

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
 *  returns NULL.
 *
 * \sa SImage_dataRed, SImage_rowGreen, SImage_rowBlue */
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
 *  returns NULL.
 *
 * \sa SImage_dataGreen, SImage_row_Red, SImage_rowBlue */
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
 *  returns NULL.
 *
 * \sa SImage_dataBlue, SImage_rowRed, SImage_rowGreen */
SVec2f_t *SImage_rowBlue(
  const SImage_t *image,
  unsigned        y);

/** \brief Stack on image on another
 *
 * Stacking adds corresponding pixels from \p src image to \p tgt image.
 * Pixels are added as vectors, which results in computing weighted-mean
 * and increasing weight. Additionally, \p src image is translated by
 * \p x_offset and \p y_offset. This function modifies pixels in \p tgt
 * image.
 *
 * Images may have different formats. If so, the \p src image is internally
 * converted to the format of \p tgt image, before stacking, while original
 * \p src remains untouched.
 *
 * \param tgt Image on which pixels are stacked
 * \param x_offset X-offset of \p src image, used while stacking
 * \param y_offset Y-offset of \p src image, used while stacking
 * \param src Soucre image
 *
 * \sa SImage_mask, SImage_add */
void SImage_stack(
  SImage_t       *tgt,
  int             x_offset,
  int             y_offset,
  const SImage_t *src);

/** \brief Apply mask on image
 *
 * Applying mask is a multiplication both pixel values and weight by a 
 * normalized value of corresponding pixel from a mask. Because it uses
 * multiplication of value-weight vector by a scalar, using color masks
 * makes sense only for \ref SFmt_SeparateRGB images. For different formats,
 * mask is converted to \ref SFmt_Gray before applying.
 *
 * \param image Image to be masked
 * \param x_offset X-offset of a mask
 * \param y_offset Y-offset of a mask
 * \param mask Mask image
 *
 * \sa SImage_mul, SImage_mulWeight, SImage_mulWeightRGB */
void SImage_mask(
  SImage_t       *image,
  int             x_offset,
  int             y_offset,
  const SImage_t *mask);

/** \brief Add one image to another
 *
 * This function arithemtically adds pixel values from \p src image to
 * corresponding pixels of \p tgt image. In contrast to \ref SImage_stack
 * values normalized with respect to pixel weight are added, so this function
 * implements arithmetical addition instead of weighted mean. Weights in
 * \p tgt image are not changed by this function.
 *
 * Images may have different formats. If so, the \p src image is internally
 * converted to the format of \p tgt image, before the operation, while
 * original \p src remains untouched.
 *
 * \param tgt Target image of an addition
 * \param x_offset X-offset of \p src image
 * \param y_offset Y-offset of \p src image
 * \param src Soucre image
 *
 * \sa SImage_stack, SImage_sub, SImage_mul, SImage_div, SImage_addConst,
 *   SImage_addConstRGB */
void SImage_add(
  SImage_t       *tgt,
  int             x_offset,
  int             y_offset,
  const SImage_t *src);

/** \brief Subtract one image to another
 *
 * This function arithemtically subtract pixel of \p src image from
 * corresponding pixels of \p tgt image. This function operates on values
 * normalized with respect to pixel weight. Weights remain unchanged.
 *
 * Images may have different formats. If so, the \p src image is internally
 * converted to the format of \p tgt image, before the operation, while
 * original \p src remains untouched.
 *
 * \param tgt Target image of a subtraction
 * \param x_offset X-offset of \p src image
 * \param y_offset Y-offset of \p src image
 * \param src Soucre image, to be subtracted from \p tgt image
 *
 * \sa SImage_add, SImage_mul, SImage_div, SImage_subConst,
 * SImage_subConstRGB */
void SImage_sub(
  SImage_t       *tgt,
  int             x_offset,
  int             y_offset,
  const SImage_t *src);

/** \brief Multiply one image by another
 *
 * This function arithemtically multiply pixel of \p dst image by
 * corresponding pixels of \p src image. This function operates on values
 * normalized with respect to pixel weight. In contrast to \ref SImage_mask
 * weights remain unchanged.
 *
 * Images may have different formats. If so, the \p src image is internally
 * converted to the format of \p tgt image, before the operation, while
 * original \p src remains untouched.
 *
 * \param tgt Target image of a multiplication and one of factor
 * \param x_offset X-offset of \p src image
 * \param y_offset Y-offset of \p src image
 * \param src Soucre image -- the another factor
 *
 * \sa SImage_mask, SImage_add, SImage_sub, SImage_div, SImage_mulConst,
 *   SImage_mulConstRGB, SImage_mulWeight, SImage_mulWeightRGB */
void SImage_mul(
  SImage_t       *tgt,
  int             x_offset,
  int             y_offset,
  const SImage_t *src);

/** \brief Divide one image by another
 *
 * This function arithemtically divide pixel of \p dst image by
 * corresponding pixels of \p src image. This function operates on values
 * normalized with respect to pixel weight. Weights remain unchanged.
 *
 * Images may have different formats. If so, the \p src image is internally
 * converted to the format of \p tgt image, before the operation, while
 * original \p src remains untouched.
 *
 * \param tgt Target image, and the divident
 * \param x_offset X-offset of \p src image
 * \param y_offset Y-offset of \p src image
 * \param src Soucre image -- the divisor
 *
 * \sa SImage_add, SImage_sub, SImage_mul, SImage_divConst,
 *   SImage_divConstRGB, SImage_invert */
void SImage_div(
  SImage_t       *tgt,
  int             x_offset,
  int             y_offset,
  const SImage_t *src);

/** \brief Add constant value to an image
 *
 * This function arithemtically adds constant value to each pixel of an image.
 * For each pixel, the value is normalized with respect to weight before
 * addition. Weights are not changed by this function.
 *
 * \param image Image to be modified
 * \param v Value added to each pixel
 *
 * \sa SImage_add, SImage_addConstRGB, SImage_subConst, SImage_mulConst,
 *   SImage_divConst */
void SImage_addConst(SImage_t *image, float v);

/** \brief Add constant value to each RGB channel of an image
 *
 * This function arithmetically adds constant value to each channel of each
 * pixel of an image (different channels may use different constant). For each
 * pixel, the value is normalized with respect to weight before addition.
 * Weights remain unchanged.
 *
 * \param image Image to be modified
 * \param r Value added to the red channel
 * \param g Value added to the green channel
 * \param b Value added to the blue channel
 *
 * \sa SImage_add, SImage_addConst, SImage_subConstRGB, SImage_mulConstRGB,
 *   SImage_divConstRGB */
void SImage_addConstRGB(SImage_t *image, float r, float g, float b);

/** \brief Subtract constant value from an image
 *
 * This function arithemtically subtracts constant value from each pixel of an
 * image. For each pixel, the value is normalized with respect to weight
 * before subtraction. Weights are not changed by this function.
 *
 * \param image Image to be modified
 * \param v Value subtracted each pixel
 *
 * \sa SImage_sub, SImage_subConstRGB, SImage_addConst, SImage_mulConst,
 *   SImage_divConst */
void SImage_subConst(SImage_t *image, float v);

/** \brief Subtract constant value from each of RGB channels of an image
 *
 * This function arithmetically subtracts constant value from each channel of
 * each pixel of an image (different channels may use different constant). For
 * each pixel, the value is normalized with respect to weight before subtaction.
 * Weights remain unchanged.
 *
 * \param image Image to be modified
 * \param r Value subtracted from the red channel
 * \param g Value subtracted from the green channel
 * \param b Value subtracted from the blue channel
 *
 * \sa SImage_sub, SImage_subConst, SImage_addConstRGB, SImage_mulConstRGB,
 *   SImage_divConstRGB */
void SImage_subConstRGB(SImage_t *image, float r, float g, float b);

/** \brief Multiply image by a constant value
 *
 * This function multiplies each pixel of an image by a constant value.
 * Weights are not changed by this function.
 *
 * \param image Image to be modified
 * \param v Value multiplied to each pixel
 *
 * \sa SImage_mul, SImage_mulConstRGB, SImage_mulWeight, SImage_addConst,
 *   SImage_subConst, SImage_divConst */
void SImage_mulConst(SImage_t *image, float v);

/** \brief Multiply each of RGB channels of an image by a constant value
 *
 * This function multiplies each of RGB channels of each pixels of an image by
 * a constant value (different channels may use different constant). Weights
 * remain unchanged.
 *
 * \param image Image to be modified
 * \param r Value multiplied to the red channel
 * \param g Value multiplied to the green channel
 * \param b Value multiplied to the blue channel
 *
 * \sa SImage_mul, SImage_mulConst, SImage_mulWeightRGB, SImage_addConstRGB,
 *   SImage_subConstRGB, SImage_divConstRGB */
void SImage_mulConstRGB(SImage_t *image, float r, float g, float b);

/** \brief Divide image by a constant value
 *
 * This function divides each pixel of an image by a constant value.
 * Weights are not changed by this function.
 *
 * \param image Image to be modified
 * \param v Divisor applied to each pixel
 *
 * \sa SImage_div, SImage_divConstRGB, SImage_addConst, SImage_subConst,
 *   SImage_mulConst */
void SImage_divConst(SImage_t *image, float v);

/** \brief Divide each of RGB channels of an image by a constant value
 *
 * This function divides each of RGB channels of each pixels of an image by
 * a constant value (different channels may use different constant). Weights
 * remain unchanged.
 *
 * \param image Image to be modified
 * \param r Divisor applied to the red channel
 * \param g Divisor applied to the green channel
 * \param b Divisor applied to the blue channel
 *
 * \sa SImage_div, SImage_divConst, SImage_addConstRGB, SImage_subConstRGB,
 *   SImage_mulConstRGB */
void SImage_divConstRGB(SImage_t *image, float r, float g, float b);

/** \brief Multiply image weight by a constant value
 *
 * This function multiplies each vector that represents single pixel of an
 * image by a constant value. This operation results in multiplying pixel
 * weights by a constant, without changing their colors.
 *
 * \param image Image to be modified
 * \param v Value multiplied to each pixel
 *
 * \sa SImage_mul, SImage_mulConst, SImage_mulConstRGB, SImage_mulWeightRGB */
void SImage_mulWeight(SImage_t *image, float v);

/** \brief Multiply weights of each RGB channel of an image by a constant value
 *
 * This function multiplies each vector that represents single pixel of each
 * channel of \ref SFmt_SeparateRGB image by a constant value (different
 * channels may use different constant). This operation results in multiplying
 * pixel weights by a constant, without changing their colors. This function
 * makes sense only for \ref SFmt_SeparateRGB images.
 *
 * \param image Image to be modified
 * \param r Value multiplied to each pixel of red channel
 * \param g Value multiplied to each pixel of green channel
 * \param b Value multiplied to each pixel of blue channel
 *
 * \sa SImage_mul, SImage_mulConst, SImage_mulConstRGB, SImage_mulWeightRGB */
void SImage_mulWeightRGB(SImage_t *image, float r, float g, float b);

/** \brief Invert (multiplicatively) each pixel of an image
 *
 * This function multiplicatively inverts each of image pixel: if a pixel has
 * normalized brightness $x$, then inverted pixel has normalized brightness
 * $1/x$. This is not the same as image negative. This function does not modify
 * pixel weights.
 *
 * Note that this operation usually produce an image, where normalized pixel
 * values are greated than one.
 *
 * \param image Image to be inverted
 *
 * \sa SImage_div */
void SImage_invert(SImage_t *image);

#endif /* __SPICA_IMAGE_H__ */
