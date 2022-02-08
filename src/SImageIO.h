/* This file is part of Spica, released under MIT license.
 * See LICENSE for details.
 */

/* Author: Piotr Polesiuk, 2022 */

/** \file SImageIO.h
 *  \brief Input-output functions on images
 */

#ifndef __SPICA_IMAGE_IO_H__
#define __SPICA_IMAGE_IO_H__

#include "SCommon.h"
#include "SImage.h"

/** \brief On-disk pixel format */
typedef enum SPixFormat {
  /** 8-bit gray scale */
  SPF_Gray8 = 0,
  /** 16-bit gray scale */
  SPF_Gray16,
  /** 8-bit RGB */
  SPF_RGB8,
  /** 16-bit RGB */
  SPF_RGB16,
} SPixFormat_t;

/** \brief load PNG image into allocated \ref SImage_t
 *
 * \param image Pointer to the SImage_t structure. The \ref SImage_loadPNG_at
 *   will initialize this memory using \ref SImage_init function. If \p image
 *   already contains an image, the \ref SImage_deinit should be called first.
 * \param fname File name of the PNG image
 *
 * \return \ref SPICA_OK on success or \ref SPICA_ERROR on fail. On error the
 *   \p image is initialized as \ref SFmt_Invalid image.
 *
 * \sa SImage_loadPNG */
int SImage_loadPNG_at(
  SImage_t   *image,
  const char *fname);

/** \brief load PNG image from file.
 *
 * \param fname File name of the PNG image
 *
 * \return newly allocated \ref SImage_t. The image should be freed using
 *   \ref SImage_free function.
 *
 * \sa SImage_loadPNG_at */
SImage_t *SImage_loadPNG(
  const char *fname);

/** \brief save PNG image into a file.
 *
 * \param image Image to be saved
 * \param format Requested on-disc format of an image
 * \param fname Name of the output file
 *
 * \return \ref SPICA_OK on success or \ref SPICA_ERROR on fail. */
int SImage_savePNG(
  const SImage_t *image,
  SPixFormat_t    format,
  const char     *fname);

#endif /* __SPICA_IMAGE_IO_H__ */
