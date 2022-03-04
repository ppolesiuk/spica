/* This file is part of Spica, released under MIT license.
 * See LICENSE for details.
 */

/* Author: Piotr Polesiuk, 2022 */

#include "SStarFinder.h"

#include <assert.h>
#include <math.h>

/* ========================================================================= */
void SStarFinder_init(SStarFinder_t *finder) {
  finder->sigma               = 3.0f;
  finder->brightnessThreshold = 0.1f;
  finder->candidateThreshold  = 0.5f;
  finder->minDist             = 2.0f;
  finder->fitSteps            = 30;
}

/* ========================================================================= */
SStarSet_t *SStarFinder_findStars(
  const SStarFinder_t *finder,
  const SImage_t      *image)
{
  SStarSet_t *result = SStarSet_alloc();
  SStarFinder_findStars_at(result, finder, image);
  return result;
}

/* ========================================================================= */
static int isCandidate(
  const SStarFinder_t *finder,
  const SImage_t      *image,
  int x, int y)
{
  SVec2f_t pix = image->data_gray[y * image->width + x];
  if (pix[1] == 0.0f) return 0;
  
  /* Compute pixel brightness */
  float v = pix[0] / pix[1];

  /* Compute background brightness */
  SVec2f_t sum = { 0.0f, 0.0f };
  for (int y1 = y - 1; y1 < y + 1; y1++) {
    for (int x1 = x - 1; x1 < x + 1; x1++) {
      pix = image->data_gray[y1 * image->width + x1];
      if (pix[1] > 0.0f && pix[0] > v * pix[1])
        return 0; /* Pixel is not a local maximum */
      sum += pix;
    }
  }
  float b = sum[0] / sum[1];

  return (v - b > finder->brightnessThreshold * finder->candidateThreshold);
}

/* ------------------------------------------------------------------------- */
static int starIsInSet(
  const SStarFinder_t *finder, const SStar_t *star, const SStarSet_t *sset)
{
  float min_dist_sq = finder->sigma * finder->minDist;
  min_dist_sq *= min_dist_sq;

  for (size_t i = 0; i < sset->length; i++) {
    if (SVec2f_lengthSq(star->pos - sset->data[i].pos) < min_dist_sq)
      return 1;
  }
  return 0;
}

/* ------------------------------------------------------------------------- */
static void processCandidate(
  const SStarFinder_t *finder,
  const SImage_t      *image,
  SStarSet_t          *sset,
  float x, float y)
{
  SStar_t star = {
    .pos        = { x, y },
    .brightness = 1.0f,
    .bias       = 0.0f,
    .sigma      = finder->sigma,
    .index      = -1,
    .weight     = 1
  };

  SStar_fit(&star, image, finder->fitSteps);

  if (star.brightness < finder->brightnessThreshold) return;
  if (starIsInSet(finder, &star, sset)) return;

  SStarSet_add(sset, &star);
}

/* ------------------------------------------------------------------------- */
void SStarFinder_findStars_at(
  SStarSet_t          *sset,
  const SStarFinder_t *finder,
  const SImage_t      *image)
{
  if (image->format == SFmt_Invalid) return;

  SImage_t *gray_image;
  if (image->format == SFmt_Gray) gray_image = (SImage_t *)image;
  else gray_image = SImage_toFormat(image, SFmt_Gray);

  int scale = finder->sigma;
  if (scale < 1) scale = 1;

  SImage_t *scaled_image;
  if (scale == 1) scaled_image = gray_image;
  else scaled_image = SImage_scaleDown(gray_image, scale);

  unsigned scaled_width  = scaled_image->width;
  unsigned scaled_height = scaled_image->height;
  
  for (unsigned y = 1; y < scaled_height-1; y++) {
    for (unsigned x = 1; x < scaled_width-1; x++) {
      if (isCandidate(finder, scaled_image, x, y)) {
        processCandidate(finder, gray_image, sset,
          x * scale + 0.5f * (scale - 1),
          y * scale + 0.5f * (scale - 1));
      }
    }
  }

  if (scaled_image != gray_image)
    SImage_free(scaled_image);
  if (gray_image != image)
    SImage_free(gray_image);

  SStarSet_sort(sset);
}

/* ========================================================================= */
static float gauss2(float a, float sigma, float x, float y) {
  return a * expf(- (x*x + y*y) / (2.0f * sigma * sigma));
}

/* ------------------------------------------------------------------------- */
static void fitStarPos(SStar_t *star, const SImage_t *image) {
  assert(image->format == SFmt_Gray);

  SVec2f_t pos = { 0.0f, 0.0f };
  float mass  = 0.0f;
  float px    = star->pos[0];
  float py    = star->pos[1];
  float sigma = star->sigma;
  float bias  = star->bias;
  int cx = (int)px;
  int cy = (int)py;
  int dist = (int)(sigma * 3.0f) + 1;

  for (int y = cy - dist; y <= cy + dist; y++) {
    if (y < 0 || y >= image->height) continue;

    for (int x = cx - dist; x <= cx + dist; x++) {
      if (x < 0 || x >= image->width) continue;

      SVec2f_t pix = image->data_gray[y * image->width + x];
      if (pix[1] == 0.0f) continue;

      float v = pix[0] / pix[1] - bias;
      SVec2f_t pm = { x, y };
      v *= gauss2(1.0f, sigma, x - px, y - py);
      pos  += pm * v;
      mass += v;
    }
  }

  star->pos = pos / mass;
}

/* ------------------------------------------------------------------------- */
static void fitStarBrightness(SStar_t *star, const SImage_t *image) {
  assert(image->format == SFmt_Gray);

  SVec2f_t bght = { 0.0f, 0.0f };
  SVec2f_t bias = { 0.0f, 0.0f };
  float px    = star->pos[0];
  float py    = star->pos[1];
  float sigma = star->sigma;
  int cx = (int)px;
  int cy = (int)py;
  int dist = (int)(sigma * 3.0f) + 1;
  float bias0 = star->bias;
  float bght0 = star->brightness;

  for (int y = cy - dist; y <= cy + dist; y++) {
    if (y < 0 || y >= image->height) continue;

    for (int x = cx - dist; x <= cx + dist; x++) {
      if (x < 0 || x >= image->width) continue;

      SVec2f_t pix = image->data_gray[y * image->width + x];
      if (pix[1] == 0.0f) continue;

      float v = pix[0] / pix[1];
      float g = gauss2(1.0f, sigma, x - px, y - py);

      /* take (v-bias0)/g with weight g*g */
      bght[0] += (v - bias0) * g;
      bght[1] += g * g;

      bias[0] += (v - g*bght0) * (1.0f - g);
      bias[1] += 1.0f - g;
    }
  }

  star->brightness = bght[0] / bght[1];
  star->bias       = bias[0] / bias[1];
}

/* ------------------------------------------------------------------------- */
void SStar_fit(SStar_t *star, const SImage_t *image, int steps) {
  assert(image->format == SFmt_Gray);
  assert(steps >= 0);

  for (int i = 0; i < steps; i++) {
    fitStarPos(star, image);
    fitStarBrightness(star, image);
  }
}
