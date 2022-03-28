/* This code is in the public domain.
 * You may freely copy parts of it in your project
 */

/* This program shows how images can be aligned and stacked together. In this
 * example, three monochrome PNG images are are aligned and stacked into
 * single RGB image. */

#include <SImage.h>
#include <SCoarseAlign.h>
#include <SStarFinder.h>
#include <SStarMatcher.h>

#include <stdio.h>

/* Three images, star sets, and transformations */
static SImage_t     imgs[3];
static SStarSet_t  *stars[3];
static STransform_t trans[3];

/* Main Spica components used by this program:
 *
 * SStarFinder_t find stars on each image,
 * then SBrutAligner_t perform coarse alignment,
 * allowing SStarMatcher_t to match corresponding stars and compute
 * transformation that transform coordinates from one image to another. */
static SStarFinder_t  finder;
static SBrutAligner_t brutAligner;
static SStarMatcher_t matcher;

int main(int argc, char **argv) {
  int i;
  /* Initialize Spica components with default values */
  SStarFinder_init(&finder);
  SBrutAligner_init(&brutAligner);
  SStarMatcher_init(&matcher);

  if (argc <= 3) {
    fprintf(stderr, "Usage: %s R_IMAGE G_IMAGE B_IMAGE\n", argv[0]);
    return 1;
  }
  
  /* Load images and find stars on them */
  for (i = 0; i < 3; i++) {
    if (SImage_loadPNG_at(&imgs[i], argv[i+1])) {
      fprintf(stderr, "Could not load %s image\n.", argv[i+1]);
      return 1;
    }

    stars[i] = SStarFinder_findStars(&finder, &imgs[i]);
    if (stars[i]->length <= 2) {
      fprintf(stderr, "Unable to align %s image: only %d stars was found.\n",
        argv[i+1], (int)stars[i]->length);
      return 1;
    }
  }

  /* Transformation of the first image is an identity. */
  trans[0].type = STr_Identity;
  /* Add stars from the first image to SStarMatcher */
  SStarMatcher_update(&matcher, &trans[0], stars[0]);
  /* Compute bounding box that contains all three images -- start with bounding
   * box of the first image. */
  SBoundingBox_t bb = SImage_boundingBox(&imgs[0]);

  for (i = 1; i < 3; i++) {
    /* Perform coarse alignment */
    STransform_t tr = SBrutAligner_align(&brutAligner, &matcher.sset, stars[i]);
    /* Match stars with those, previously found on other images */
    SStarMatcher_matchStars(&matcher, &tr, stars[i]);
    /* Perform fine alignment */
    trans[i] = SStarMatcher_getTransform(&matcher, stars[i]);
    /* Update SStarMatch with new stars */
    SStarMatcher_update(&matcher, &trans[i], stars[i]);
    /* Update bounding box */
    bb = SBoundingBox_union(bb,
      STransform_boundingBox(&trans[i], SImage_boundingBox(&imgs[i])));
  }

  /* Compute shift transformation, that places top-left corner to (0,0). */
  STransform_t tr_shift = STransform_shift(-SVec2f(bb.minX, bb.minY));
  /* Compute size of the output image */
  int width  = (int)(bb.maxX - bb.minX) + 1;
  int height = (int)(bb.maxY - bb.minY) + 1;
  /* Create output image and initialize it with "no-data" */
  SImage_t result;
  SImage_init(&result, width, height, SFmt_SeparateRGB);
  SImage_clear(&result);

  for (i = 0; i < 3; i++) {
    SImage_t img;
    /* Convert each image to SeparateRGB format */
    SImage_toFormat_at(&img, &imgs[i], SFmt_SeparateRGB);

    /* And pick only one channel on it (R,G,B). */
    SImage_mulWeightRGB(&img,
      (i == 0 ? 1.0f : 0.0f),
      (i == 1 ? 1.0f : 0.0f),
      (i == 2 ? 1.0f : 0.0f));
    
    /* Stack images together */
    trans[i] = STransform_compose(&tr_shift, &trans[i]);
    SImage_stackTr(&result, &trans[i], &img);

    SImage_deinit(&img);
  }

  /* Save result image */
  SImage_savePNG(&result, SPF_RGB16, "output.png");

  return 0;
}
