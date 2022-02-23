/* This file is part of Spica, released under MIT license.
 * See LICENSE for details.
 */

/* Author: Piotr Polesiuk, 2022 */

/** \file SStar.h
 * \brief Stars and star sets on images
 */

#ifndef __SPICA_STAR_H__
#define __SPICA_STAR_H__

#include "SVec.h"

#include <stddef.h>

/** \brief Representation of a star on an image */
typedef struct SStar {
  /** \brief (X,Y) image coordinates */
  SVec2f_t pos;
  /** \brief brightness: the height of fit gaussian function */
  float    brightness;
  /** \brief background brightness */
  float    bias;
  /** \brief sigma of fit gaussian function */
  float    sigma;
  /** \brief index of a star in associated data structure
   *
   * If stars from multiple star sets are matched together, this field
   * is set to the same value for the same star in different set.
   *
   * Default value (set by \ref SStar_init) is -1, and means that the star
   * does not appear in any other data structure. */
  int    index;
  /** \brief weight of a star
   *
   * The field is used by aggregated sets of stars. It describes how many
   * stars from different sets turned out to be this star. */
  int    weight;
} SStar_t;

/** \brief Set of stars */
typedef struct SStarSet {
  /** \brief Number of stars in set.
   *
   * This field should be used read only */
  size_t   length;
  
  /** \brief The number of elements in data array prepared to holding stars
   *
   * This field should be used read only. It may be automatically increased
   * by \ref SStarSet_add function. */
  size_t   capacity;

  /** \brief Array of stars */
  SStar_t *data;
} SStarSet_t;

/** \brief Initialize star with default values
 *
 * \param star Pointer to star */
void SStar_init(SStar_t *star);

/** \brief Initialize already allocated SStarSet_t
 *
 * To deinitialize it, call \ref SStarSet_deinit function.
 *
 * \param sset Pointer to already allocated SStarSet_t.
 *
 * \sa SStarSet_alloc */
void SStarSet_init(SStarSet_t *sset);

/** \brief Deinitialize SStarSet_t initialized by \ref SStarSet_init
 *
 * This function frees only internal resources used by SStarSet_t. It does
 * not free the memory occupied by SStarSet_t itself.
 *
 * \param sset Pointer to SStarSet_t to be deinitialized
 *
 * \sa SStarSet_free */
void SStarSet_deinit(SStarSet_t *sset);

/** \brief Allocate and initialize new SStarSet_t
 *
 * \return Pointer to the newly allocated set of stars, or NULL on malloc
 *   error. The set can be freed with \ref SStarSet_free function.
 *
 * \sa SStarSet_init */
SStarSet_t *SStarSet_alloc(void);

/** \brief Free set of stars previously allocated with \ref SStarSet_alloc
 *
 * \param sset Pointer to the set. It may be NULL.
 *
 * \sa SStarSet_deinit */
void SStarSet_free(SStarSet_t *sset);

/** \brief Create copy of a set of stars, and store it in already allocated
 *    SStarSet_t
 *
 * Created set of stars will have capacity equal to its length.
 *
 * \param dst Pointer to the destination SStarSet_t structure. The function
 *   will initialize this memory, so it can be later deinitialized using
 *   \ref SStarSet_deinit function. If \p dst already
 *   contains a valid image, the \ref SStarSet_deinit should be called first.
 * \param sset Source set of stars.
 *
 * \sa SStarSet_clone */
void SStarSet_clone_at(SStarSet_t *dst, const SStarSet_t *sset);

/** \brief Create copy of a set of stars
 *
 * \param sset Source set of stars
 *
 * \return pointer to the newly allocated set that contains the same stars
 *   as \p sset. The new set should be freed using \ref SStarSet_free
 *   function. Created set of stars will have capacity equal to its length.
 *
 * \sa SStarSet_clone_at */
SStarSet_t *SStarSet_clone(const SStarSet_t *sset);

/** \brief Add star to set
 *
 * If \p sset if full (i.e., length is equal to the capacity), the capacity
 * of the set is increased to make a room for a new star.
 *
 * \param sset Set of stars
 * \param star Pointer to a new star. The star is copied into \p sset
 */
void SStarSet_add(SStarSet_t *sset, const SStar_t *star);

/** \brief Sort set of stars (brightest stars first)
 *
 * \param sset Set to be sorted */
void SStarSet_sort(SStarSet_t *sset);

#endif /* __SPICA_STAR_H__ */
