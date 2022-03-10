/* This file is part of Spica, released under MIT license.
 * See LICENSE for details.
 */

/* Author: Piotr Polesiuk, 2022 */

/** \file SCoarseAlign.h
 *  \brief Coarse image alignment
 *
 * Coarse alignment of stars from different images. Coarse alignment is
 * required to match stars by \ref SStarMatcher_t before fine alignment.
 */

#ifndef __SPICA_COARSE_ALIGN_H__
#define __SPICA_COARSE_ALIGN_H__

#include "SStar.h"
#include "STransform.h"

/** \brief Coarse alignment method based on assumption that transformations
 *    of consecutive images do not differ much.
 *
 * This method transforms each star using transformation from previous frame
 * and then, tries to find a nearby star in reference set. For such a matching
 * it computes the \ref STr_Linear transformation using complex linear
 * regression. This method is pretty fast, but in some cases it is unable to
 * find any solution, and returns \ref STr_Drop transformation. In such a case
 * you can fallback to other slower, but more reliable method, e.g.,
 * \ref SBrutAligner_t.
 */
typedef struct SSmallChangeAligner {
  /** \brief maximal distance to matched star (in sigmas) */
  float distThreshold;
  /** \brief minimal number of matched stars, to accept matching (otherwise
   *    \ref STr_Drop is returned). */
  int   minStarN;
} SSmallChangeAligner_t;

/** \brief Initialize SSmallChangeAligner_t with default values
 *
 * Field         | Default value
 * --------------| -------------
 * distThreshold | 5.0f
 * minStarN      | 4
 */
void SSmallChangeAligner_init(SSmallChangeAligner_t *aligner);

/** \brief Run SSmallChangeAligner_t
 *
 * \param aligner Settings of the aligner
 * \param ref_sset Reference set of stars, e.g., from SStarMatcher_t
 * \param prev_tr Transformation from the previous frame
 * \param sset Set of stars
 *
 * \returns Transformation that transforms positions of stars from
 *   \p sset to positions of corresponding stars from \p ref_sset,
 *   or \ref STr_Drop transformation when no matching was found. */
STransform_t SSmallChangeAligner_align(
  const SSmallChangeAligner_t *aligner,
  const SStarSet_t *ref_sset,
  const STransform_t *prev_tr,
  const SStarSet_t *sset);

/** \brief Coarse alignment that tries to match each pair of stars to each pair
 *    of reference stars and picks the best such matching.
 *
 * This method is relatively slow (O(n^6)) but works well in practice. To
 * speed-up matching it is possible to narrow search space by setting how
 * scaling and rotation is should be close to the identity transformation. */
typedef struct SBrutAligner {
  /** \brief Number of stars from input star set used to find pair that
   *    matches pair from reference set. First \ref starN stars are taken.
   *    Negative value means to take all stars. */
  int starN;
  /** \brief Number of stars from reference set used to find matching pair.
   *    First \ref refStarN stars are taken. Negative value means to take
   *    all stars. */
  int refStarN;
  /** \brief Number of stars from input star set used to measure quality
   *    of alignment. First \ref rankStarN stars are taken. Negative value
   *    means to take all stars. */
  int rankStarN;
  /** \brief Maximal distance between stars (measured in sigmas) when they
   *    are treated as same star. Used in measuring quality of alignment */
  float distTol;
  /** \brief Allowed scale tolerance. E.g. 0.1f means 10% error range. */
  float scaleTol;
  /** \brief Allowed rotation tolerance, measured in distance between unit
   *    vector and its rotated and normalized version. For small angles it
   *    is almost the same as angle in radians. Values greater than 2.0f
   *    means that every rotation is allowed */
  float rotTol;
} SBrutAligner_t;

/** \brief Initialize SBrutAligner_t with default values
 *
 * Field     | Default value
 * --------- | -------------
 * starN     | 30
 * refStarN  | -1
 * rankStarN | -1
 * distTol   | 1.5f
 * scaleTol  | 0.1f
 * rotTol    | 3.0f
 */
void SBrutAligner_init(SBrutAligner_t *aligner);

/** \brief Run SBrutAligner_t
 *
 * \param aligner Settings of the aligner
 * \param ref_sset Reference set of stars, e.g., from SStarMatcher_t
 * \param sset Set of stars
 *
 * \returns Transformation that transforms positions of stars from
 *   \p sset to positions of corresponding stars from \p ref_sset,
 *   or \ref STr_Drop transformation when no matching was found. */
STransform_t SBrutAligner_align(
  const SBrutAligner_t *aligner,
  const SStarSet_t *ref_sset,
  const SStarSet_t *sset);

#endif /* __SPICA_COARSE_ALIGN_H__ */
