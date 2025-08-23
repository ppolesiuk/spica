/* This file is part of Spica, released under MIT license.
 * See LICENSE for details.
 */

/* Author: Piotr Polesiuk, 2025 */

/** \file SSeqLoaderSER.h
 *  \brief Image loader for SER files.
 *
 * The implementation is based on the SER file format specification,
 * version 3, available at https://grischa-hahn.hier-im-netz.de/astro/ser/
 */

#ifndef __SPICA_SEQ_LOADER_SER_H__
#define __SPICA_SEQ_LOADER_SER_H__

#include "SCommon.h"

#include <stdio.h>

typedef struct SImage SImage_t;

/** \brief Pixel format used in SER files.
 *
 * Values are defined in the SER specification.
 */
typedef enum SSERFormat {
  SSERFormat_Mono       = 0,
  SSERFormat_Bayer_RGGB = 8,
  SSERFormat_Bayer_GRBG = 9,
  SSERFormat_Bayer_GBRG = 10,
  SSERFormat_Bayer_BGGR = 11,
  SSERFormat_Bayer_CYYM = 16,
  SSERFormat_Bayer_YCMY = 17,
  SSERFormat_Bayer_YMCY = 18,
  SSERFormat_Bayer_MYYC = 19,
  SSERFormat_RGB        = 100,
  SSERFormat_BGR        = 101,
} SSERFormat_t;

/** \brief Sequence loader for SER files.
 *
 * This structure represents an opened SER file and is used to load images
 * from it.
 */
typedef struct SSeqLoaderSER {
  /** \brief The file strucure with opened SER file. Set to NULL if not
   * opened, or invalid. */
  FILE *file;
  /** \brief Pixel format */
  SSERFormat_t format;
  /** \brief The flag indicating whether data bytes are swapped. */
  int swap;
  /** \brief The width of the images. */
  unsigned width;
  /** \brief The height of the images. */
  unsigned height;
  /** \brief The number of bits per pixel. */
  unsigned bits_per_pixel;
  /** \brief The number of bytes per pixel. */
  unsigned bytes_per_pixel;
  /** \brief Total number of images in the SER file. Initialized to 0 if the
   * file is not opened or invalid. */
  size_t length;
  /** \brief The name of the observer (NULL-terminated). */
  char observer[41];
  /** \brief The name of the instrument (NULL-terminated). */
  char instrument[41];
  /** \brief The name of the telescope (NULL-terminated). */
  char telescope[41];
} SSeqLoaderSER_t;

/* ========================================================================= */

/** \brief Open a SER file and store into \ref SSeqLoaderSER_t structure.
 *
 * To close it, call \ref SSeqLoaderSER_deinit.
 *
 * \param loader The loader structure to initialize. If the loader contains
 *   already opened file, the \ref SSeqLoaderSER_deinit should be called
 *   before.
 * \param fname The name of the SER file to open.
 *
 * \return \ref SPICA_OK on success or \ref SPICA_ERROR on fail.
 *
 * \sa SSeqLoaderSER_open */
int SSeqLoaderSER_open_at(SSeqLoaderSER_t *loader, const char *fname);

/** \brief Close the SER file and deinitialize the loader structure.
 *
 * It is safe to call this function twice on the same structure, but it
 * it should not be called on the structure that was not initialized.
 *
 * \param loader The loader structure to deinitialize.
 *
 * \sa SSeqLoaderSER_free */
void SSeqLoaderSER_deinit(SSeqLoaderSER_t *loader);

/** \brief Open a SER file and return a pointer to the loader structure.
 *
 * The loader structure should be freed with \ref SSeqLoaderSER_free.
 *
 * \param fname The name of the SER file to open.
 *
 * \return Pointer to the loader structure on success, or NULL on fail.
 *
 * \sa SSeqLoaderSER_free */
SSeqLoaderSER_t *SSeqLoaderSER_open(const char *fname);

/** \brief Free the loader structure created by \ref SSeqLoaderSER_open.
 *
 * \param loader The loader structure to free.
 *
 * \sa SSeqLoaderSER_deinit */
void SSeqLoaderSER_free(SSeqLoaderSER_t *loader);

/* ========================================================================= */

/** \brief Load image from the SER file.
 *
 * \param loader The loader structure with opened SER file.
 * \param index The index of the image to load (0-based).
 *
 * \return Pointer to the loaded image on success, or NULL on fail. The image
 *  should be freed with \ref SImage_free.
 *
 * \sa SSeqLoaderSER_loadImage_at */
SImage_t *SSeqLoaderSER_loadImage(
  const SSeqLoaderSER_t *loader,
  unsigned               index);

/** \brief Load image from the SER file into existing image structure.
 *
 * \param loader The loader structure with opened SER file.
 * \param image Pointer to the \ref SImage_t structure.
 *   The \ref SSeqLoaderSER_loadImage_at function will initialize the image
 *   using \ref SImage_init. If the \p image already contains data,
 *   the \ref SImage_deinit function should be called before.
 * \param index The index of the image to load (0-based). 
 *
 * \return \ref SPICA_OK on success, or \ref SPICA_ERROR on fail.
 *
 * \sa SSeqLoaderSER_loadImage */
int SSeqLoaderSER_loadImage_at(
  const SSeqLoaderSER_t *loader,
  SImage_t              *image,
  unsigned               index);

#endif /* __SPICA_SEQ_LOADER_SER_H__ */
