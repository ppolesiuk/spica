/* This file is part of Spica, released under MIT license.
 * See LICENSE for details.
 */

/* Author: Piotr Polesiuk, 2022 */

/** \file SStarFinder.h
 * \brief Finding stars on image
 *
 * Implementation of simple algorithm of finding stars on images. The
 * algorithm consists of the following steps:
 *
 * 1. Scale the image down σ (rounded to integral) times.
 * 2. Find pixels that are local maximums on the scaled images,
 *      and are bright enough (see candidateThreshold field of SStarFinder_t).
 * 3. For those pixels, fit the 2D gaussian function on original image
 *      with subpixel precision (σ parameter is fix).
 * 4. Accept the star if it is bright enough (see brightnessThreshold field of
 *      SStarFinder_t).
 * 5. Sort stars that are found.
 */

#ifndef __SPICA_STAR_FINDER_H__
#define __SPICA_STAR_FINDER_H__

#include "SImage.h"
#include "SStar.h"

/** \brief Configuration of SStarFinder algorithm */
typedef struct SStarFinder {
  /** \brief expected size of stars -- σ-parameter of gaussian
   *     function */
  float sigma;
  /** \brief Threshold of star brightness used to decide to put star on a list.
   *
   * If fit star has brightness below this threshold, it is not included in
   * result list of stars. Note that star brightness is a height of fit
   * gaussian function with respect to the background brightness. */
  float brightnessThreshold;
  /** \brief Threshold of pixel brightness in step 2 of the algorithm.
   *
   * This value is multiplied by \ref brightnessThreshold, and obtained value is
   * used as a threshold of difference between pixel and background brightness
   * on scaled image. */
  float candidateThreshold;
  /** \brief Minimal distance between stars (measured in \ref sigma units) to
   *     be considered as separate stars */
  float minDist;
  /** \brief Number of steps in fitting procedure */
  int   fitSteps;
} SStarFinder_t;

/** \brief Initialize SStarFinder with default values
 *
 * Field               | Default value
 * ------------------- | -------------
 * sigma               | 3.0f
 * brightnessThreshold | 0.1f 
 * candidateThreshold  | 0.5f 
 * minDist             | 2.0f
 * fitSteps            | 30
 * */
void SStarFinder_init(SStarFinder_t *finder);

/** \brief Find stars on given image.
 *
 * \param finder Configuration of star-finder algorithm
 * \param image Image to search for stars
 *
 * \returns Newly allocated set of stars on given \p image
 *
 * \sa SStarFinder_findStars_at */
SStarSet_t *SStarFinder_findStars(
  const SStarFinder_t *finder,
  const SImage_t      *image);

/** \brief Find stars on given image, and add them to existing star set
 *
 * \param sset Set of stars that will be expended by newly found stars. In
 *   most cases should be empty.
 * \param finder Configuration of star-finder algorithm
 * \param image Image to search for stars
 *
 * \sa SStarFinder_findStars */
void SStarFinder_findStars_at(
  SStarSet_t          *sset,
  const SStarFinder_t *finder,
  const SImage_t      *image);

/** \brief Fit star on grayscale image
 *
 * During the fitting process, star position, brightness and bias (background
 * brightness) is adjusted to fit data on the image. Star sigma is not changed.
 *
 * \param star Star to be fit
 * \param image Image with given star. Should be in \ref SFmt_Gray format.
 * \param steps Number of steps of fitting process (Should be greater than 0).
 */
void SStar_fit(SStar_t *star, const SImage_t *image, int steps);

#endif /* __SPICA_STAR_FINDER_H__ */
