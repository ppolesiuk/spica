/* This file is part of Spica, released under MIT license.
 * See LICENSE for details.
 */

/* Author: Piotr Polesiuk, 2022 */

/** \file SStarMatcher.h
 * \brief Matching stars from different images
 */

#ifndef __SPICA_STAR_MATCHER_H__
#define __SPICA_STAR_MATCHER_H__

#include "SStar.h"
#include "STransform.h"

/** \brief The structure that aggregates matched stars
 *
 * There are four steps of adding stars from new image to this structure:
 *
 * 1. Coarse alignment: use any method, to obtain STransform_t that transforms
 *    positions on new image to corresponding positions in SStarMatcher_t.
 * 2. Matching stars with those in SStarMatcher_t: \ref SStarMatcher_matchStars
 *    function.
 * 3. Fine alignment based on matched stars: \ref SStarMatcher_getTransform
 *    function.
 * 4. Updating SStarMatcher_t by new stars and more measure points of existing
 *    stars: \ref SStarMatcher_update function (this step is optional). */
typedef struct SStarMatcher {
  /** \brief Collection of matched stars */
  SStarSet_t sset;
  /** \brief Maximal distance between two stars, to be considered as same star
   *    (measured in geometric mean of their sigmas). */
  float      distThreshold;
} SStarMatcher_t;

/** \brief Initialize already allocated SStarMatcher_t
 *
 * To deinitialize it, call \ref SStarMatcher_deinit function.
 *
 * \param sm Pointer to already allocated SStarMatcher_t.
 *
 * \sa SStarMatcher_alloc */
void SStarMatcher_init(SStarMatcher_t *sm);

/** \brief Deinitialize SStarMatcher_t initialized by \ref SStarMatcher_init
 *
 * This function frees only internal resources used by SStarMatcher_t. It does
 * not free the memory occupied by SStarMatcher_t itself.
 *
 * \param sm Pointer to SStarMatcher_t to be deinitialized
 *
 * \sa SStarMatcher_free */
void SStarMatcher_deinit(SStarMatcher_t *sm);

/** \brief Allocate and initialize new SStarMatcher_t
 *
 * \return Pointer to the newly allocated SStarMatcher_t. It can be freed with
 *   \ref SStarMatcher_free function.
 *
 * \sa SStarMatcher_init */
SStarMatcher_t *SStarMatcher_alloc(void);

/** \brief Free SStarMatcher_t previously allocated with
 *    \ref SStarMatcher_alloc
 *
 * \param sm Pointer to the SStarMatcher_t structure. It may be NULL.
 *
 * \sa SStarMatcher_deinit */
void SStarMatcher_free(SStarMatcher_t *sm);

/** \brief Create copy of SStarMatcher_t state, and store it in already
 *   allocated structure.
 *
 * \param dst Pointer to the destination SStarMatcher_t structure. The function
 *   will initialize this memory, so it can be later deinitialized using
 *   \ref SStarMatcher_deinit function. If \p dst is already initialized,
 *   the \ref SStarMatcher_deinit should be called first.
 * \param sm Source SStarMatcher_t.
 *
 * \sa SStarMatcher_clone */
void SStarMatcher_clone_at(SStarMatcher_t *dst, const SStarMatcher_t *sm);

/** \brief Create copy of SStarMatcher_t state.
 *
 * \param sm Source SStarMatcher_t.
 *
 * \return pointer to the newly allocated SStarMatcher_t. It should be freed
 *   using \ref SStarMatcher_free function.
 *
 * \sa SStarMatcher_clone_at */
SStarMatcher_t *SStarMatcher_clone(const SStarMatcher_t *sm);

/** \brief Match stars in given set to those in given SStarMatcher_t.
 *
 * This function for each star in \p sset transformed by \p tr transformation
 * tries to find a corresponding star in \p sm matcher state. If the closest
 * star is closer than distThreshold, the star index (for star in \p sset) is
 * set to point to the matched star, otherwise the index is set to -1.
 * Other fields of stars in \p sset reamain unchanged.
 *
 * \param sm SStarMatcher structure with set of reference stars and other
 *   settings.
 * \param tr Transformation applied to positions of stars in \p sset before
 *   matching.
 * \param sset Set of stars matched to reference stars. */
void SStarMatcher_matchStars(
  const SStarMatcher_t *sm, const STransform_t *tr, SStarSet_t *sset);

/** \brief Fine alignment of matched stars, based on complex linear regression
 *
 * This function computes a transformation that maps stars from \p sset to
 * reference stars in \p sm. The algorithm uses complex linear regression to
 * fit complex linear function (see \ref STr_Linear). Before calling this
 * function, star indices in \p sset should be set, e.g., using
 * \ref SStarMatcher_matchStars function.
 *
 * \param sm SStarMatcher_t structure containing reference set of stars.
 * \param sset Set of stars with indices pointing to reference set
 *
 * \returns transformation that transforms star positions from \p sset to
 * positions of corresponding reference stars. */
STransform_t SStarMatcher_getTransform(
  const SStarMatcher_t *sm, const SStarSet_t *sset);

/** \brief Update reference set of stars by new data
 *
 * This function fixes positions of reference stars by new data from
 * \p sset aligned by \p tr transformation. Stars in \p sset that were
 * not matched are added to the reference set and their index is set
 * respectively. Star set \p sset should be matched and fine aligned first.
 *
 * \param sm SStarMatcher_t with reference set of stars
 * \param tr Transformation used to align stars from \p sset to reference set
 * \param sset Set of stars with new data */
void SStarMatcher_update(
  SStarMatcher_t *sm, const STransform_t *tr, SStarSet_t *sset);

#endif /* __SPICA_STAR_MATCHER_H__ */
