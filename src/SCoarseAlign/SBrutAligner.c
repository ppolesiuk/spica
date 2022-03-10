/* This file is part of Spica, released under MIT license.
 * See LICENSE for details.
 */

/* Author: Piotr Polesiuk, 2022 */

#include "SCoarseAlign.h"

#include <math.h>

void SBrutAligner_init(SBrutAligner_t *aligner) {
  aligner->starN     = 30;
  aligner->refStarN  = -1;
  aligner->rankStarN = -1;
  aligner->distTol   = 1.5f;
  aligner->scaleTol  = 0.1f;
  aligner->rotTol    = 3.0f;
}

/* ========================================================================= */
static int minIntOpt(int a, int b) {
  if (a < 0 || b < a) return b;
  else return a;
}

static float rankTransform(
  int rank_star_n,
  float dist_tol,
  const SStarSet_t *ref_sset,
  const STransform_t *tr,
  const SStarSet_t *sset)
{
  float dist_tol_sq = dist_tol * dist_tol;
  float result = 0.0f;
  for (int i = 0; i < rank_star_n; i++) {
    SVec2f_t pos = STransform_apply(tr, sset->data[i].pos);
    float sigma = sset->data[i].sigma * dist_tol_sq;

    float best_rank = 1.0f;
    for (int j = 0; j < ref_sset->length; j++) {
      float rank = SVec2f_lengthSq(pos - ref_sset->data[j].pos) /
        (sigma * ref_sset->data[j].sigma);
      if (rank < best_rank) best_rank = rank;
    }

    result += best_rank;
  }
  return result;
}

static int respectScaleRotTol(const SBrutAligner_t *aligner, SVec2f_t rot) {
  float lsq = SVec2f_lengthSq(rot);
  float tol = aligner->scaleTol + 1.0f;
  tol *= tol;
  if (lsq > tol || 1.0f / lsq > tol) return 0;

  /* Abort check, when rotation tolerance is lagre */
  if (aligner->rotTol > 2.0f) return 1;

  /* Normalize vector, and compute its distance to complex unit (no rotation)
   * */
  rot /= sqrtf(lsq);
  rot[0] = 1.0f;

  lsq = SVec2f_lengthSq(rot);
  tol = aligner->rotTol;
  return lsq <= tol * tol;
}

STransform_t SBrutAligner_align(
  const SBrutAligner_t *aligner,
  const SStarSet_t *ref_sset,
  const SStarSet_t *sset)
{
  int star_n = minIntOpt(aligner->starN, sset->length);
  int ref_star_n = minIntOpt(aligner->refStarN, ref_sset->length);
  int rank_star_n = minIntOpt(aligner->rankStarN, sset->length);
  STransform_t result = {
    .type  = STr_Drop,
    .rot   = { 1.0f, 0.0f },
    .shift = { 0.0f, 0.0f }
  };
  float rank = rank_star_n;
  for (int a1 = 0; a1 < star_n; a1++) {
    for (int b1 = a1 + 1; b1 < star_n; b1++) {
      SVec2f_t pos1 = sset->data[a1].pos;
      SVec2f_t dir1 = sset->data[b1].pos - pos1;
      if (dir1[0] == 0.0f && dir1[1] == 0.0f) continue;

      for (int a2 = 0; a2 < ref_star_n; a2++) {
        for (int b2 = 0; b2 < ref_star_n; b2++) {
          if (b2 == a2) continue;
          SVec2f_t pos2 = ref_sset->data[a2].pos;
          SVec2f_t dir2 = ref_sset->data[b2].pos - pos2;
          if (dir2[0] == 0.0f && dir2[1] == 0.0f) continue;

          SVec2f_t rot = SVec2f_complexDiv(dir2, dir1);
          if (!respectScaleRotTol(aligner, rot)) continue;

          STransform_t tr = {
            .type  = STr_Linear,
            .rot   = rot,
            .shift = pos2 - SVec2f_complexMul(pos1, rot)
          };

          float new_rank =
            rankTransform(rank_star_n, aligner->distTol, ref_sset, &tr, sset);
          if (new_rank < rank) {
            rank   = new_rank;
            result = tr;
          }
        }
      }
    }
  }
  return result;
}
