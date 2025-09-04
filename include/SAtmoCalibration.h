/* This file is part of Spica, released under MIT license.
 * See LICENSE for details.
 */

/* Author: Piotr Polesiuk, 2025 */

/** \file SAtmoCalibration.h
 *  \brief Calibration of atmospheric parameters
 *
 * This file contains functions and structures related to the calibration of
 * atmospheric parameters. There are two main parameters, that can be
 * associated to each image:
 * - atmospheric extinction
 * - background sky brightness
 *
 * Each of these parameters is represented by a \ref SGradient_t structure.
 * However, the parameter can be seen as a single constant value (or RGB
 * color) if the gradient has zero slope. For color images, each of this
 * parameters is represented by a color gradient.
 *
 * Given an image with applied global calibration (dark and flat correction),
 * the value of the pixel after calibration is \f$I_c = (I - b) / k\f$, where
 * \f$I\f$ is the pixel value in the original image, \f$k\f$ is the extinction
 * coefficient, and \f$b\f$ is the background sky brightness.
 */

#ifndef __SPICA_ATMO_CALIBRATION_H__
#define __SPICA_ATMO_CALIBRATION_H__

#include "SImage.h"

/** \brief Mode of a parameter in atmospheric calibration */
typedef enum SAtmoCalibrationMode {
  /** \brief Ignore the parameter (do not recover, use default value) */
  SAtmo_Ignore,

  /** \brief Use existing value of the parameter (do not recover) */
  SAtmo_UseExisting,

  /** \brief Assume the parameter is constant across the image */
  SAtmo_Constant,

  /** \brief Assume the parameter varies linearly across the image */
  SAtmo_Gradient
} SAtmoCalibrationMode_t;

/** \brief Recover atmospheric calibration parameters
 *
 * This function recovers the atmospheric calibration parameters (extinction
 * and background) from two images: one containing image before atmospheric
 * calibration (dark and flat correction applied), and the other containing
 * the expected result.
 *
 * The algorithm respects weights of pixels in both images. As an actual
 * weight, the product of weights from both images is used.
 *
 * \param before The image before atmospheric calibration.
 * \param after  The expected image after atmospheric calibration, e.g.,
 *   back-transormed result of stacking. The image should have the same
 *   dimensions and format as the \p before image. If the formats differ,
 *   the \p after image is converted to the format of the \p before
 *   image before processing.
 * \param extinctionMode The mode for the extinction parameter.
 * \param extinction Pointer to the structure to store the recovered
 *   extinction gradient. It can be NULL. If the mode is \ref SAtmo_Ignore,
 *   or \ref SAtmo_UseExisting, the function does not modify the structure.
 *   In the latter case, the pointer should point to an existing gradient
 *   structure to be used.
 * \param backgroundMode The mode for the background parameter.
 * \param background Pointer to the structure to store the recovered
 *   background gradient. It can be NULL. If the mode is \ref SAtmo_Ignore,
 *   or \ref SAtmo_UseExisting, the function does not modify the structure.
 *   In the latter case, the pointer should point to an existing gradient
 *   structure to be used.
 */
void SAtmoCalibration_recover(
  const SImage_t        *before,
  const SImage_t        *after,
  SAtmoCalibrationMode_t extinctionMode,
  SGradient_t           *extinction,
  SAtmoCalibrationMode_t backgroundMode,
  SGradient_t           *background);

#endif /* __SPICA_ATMO_CALIBRATION_H__ */
