/* This file is part of Spica, released under MIT license.
 * See LICENSE for details.
 */

/* Author: Piotr Polesiuk, 2025 */

#include "SAtmoCalibration.h"

#include <assert.h>
#include <math.h>

inline static unsigned min(unsigned a, unsigned b) {
  return (a < b) ? a : b;
}

static void swap_rows(int n, double *A, double *b, int r1, int r2) {
  if (r1 == r2) return;
  for (int i = 0; i < n; i++) {
    double tmp = A[r1 * n + i];
    A[r1 * n + i] = A[r2 * n + i];
    A[r2 * n + i] = tmp;
  }
  double tmp = b[r1];
  b[r1] = b[r2];
  b[r2] = tmp;
}

static void gaussian_diagonalize_col(int n, double *A, double *b, int col) {
  /* Find the best candidate */
  double max_v = fabs(A[col * n + col]);
  int    max_i = col;
  for (int i = 0; i < col; i++) {
    double v = fabs(A[i * n + col]);
    if (v > max_v) {
      max_v = v;
      max_i = i;
    }
  }
  swap_rows(n, A, b, col, max_i);

  /* Normalize the row */
  double diag = A[col * n + col];
  for (int i = 0; i < col; i++) {
    A[col * n + i] /= diag;
  }
  A[col * n + col] = 1.0;
  b[col]          /= diag;

  /* Eliminate the column */
  for (int i = 0; i < col; i++) {
    double v = A[i * n + col];
    for (int j = 0; j < col; j++) {
      A[i * n + j] -= A[col * n + j] * v;
    }
    A[i * n + col] = 0.0;
    b[i]          -= b[col] * v;
  }
}

static void gaussian_solve_col(int n, double *A, double *b, int col) {
  for (int i = col + 1; i < n; i++) {
    b[i] -= A[i * n + col] * b[col];
    A[i * n + col] = 0.0;
  }
}

static void gaussian_elimination(int n, double *A, double *b) {
  for (int i = n - 1; i >= 0; i--) {
    gaussian_diagonalize_col(n, A, b, i);
  }

  for (int i = 0; i < n; i++) {
    gaussian_solve_col(n, A, b, i);
  }
}

/* ========================================================================= */

typedef SVec2f_t (*GetPixel_t)(const SImage_t*, int, int);

static void SAtmoCalibration_recoverGray(
  const SImage_t        *before,
  const SImage_t        *after,
  GetPixel_t             getPixel,
  SAtmoCalibrationMode_t extinctionMode,
  SGrayGradient_t       *extinction,
  SAtmoCalibrationMode_t backgroundMode,
  SGrayGradient_t       *background)
{
  unsigned width  = min(before->width,  after->width);
  unsigned height = min(before->height, after->height);

  double sum_1  = 0.0;
  double sum_x  = 0.0;
  double sum_y  = 0.0;
  double sum_xx = 0.0;
  double sum_xy = 0.0;
  double sum_yy = 0.0;

  double sum_v   = 0.0;
  double sum_xv  = 0.0;
  double sum_yv  = 0.0;
  double sum_xxv = 0.0;
  double sum_xyv = 0.0;
  double sum_yyv = 0.0;

  double sum_vv   = 0.0;
  double sum_xvv  = 0.0;
  double sum_yvv  = 0.0;
  double sum_xxvv = 0.0;
  double sum_xyvv = 0.0;
  double sum_yyvv = 0.0;

  double sum_s   = 0.0;
  double sum_xs  = 0.0;
  double sum_ys  = 0.0;
  double sum_sv  = 0.0;
  double sum_xsv = 0.0;
  double sum_ysv = 0.0;

  for (unsigned y = 0; y < height; y++) {
    for (unsigned x = 0; x < width; x++) {
      SVec2f_t beforePix = getPixel(before, x, y);
      SVec2f_t afterPix  = getPixel(after,  x, y);

      double weight = beforePix[1] * afterPix[1];
      double v_weighted = afterPix[0] * beforePix[1];
      double v_norm =
        (afterPix[1] > 0.0f) ? (afterPix[0] / afterPix[1]) : 0.0f;

      double xx = (double)x * (double)x;
      double xy = (double)x * (double)y;
      double yy = (double)y * (double)y;

      double vv = v_weighted * v_norm;
      double s  = beforePix[0] * afterPix[1];
      double sv = beforePix[0] * afterPix[0];

      sum_1  += weight;
      sum_x  += (double)x * weight;
      sum_y  += (double)y * weight;
      sum_xx += xx * weight;
      sum_xy += xy * weight;
      sum_yy += yy * weight;

      sum_v   += v_weighted;
      sum_xv  += (double)x * v_weighted;
      sum_yv  += (double)y * v_weighted;
      sum_xxv += xx * v_weighted;
      sum_xyv += xy * v_weighted;
      sum_yyv += yy * v_weighted;

      sum_vv   += vv;
      sum_xvv  += (double)x * vv;
      sum_yvv  += (double)y * vv;
      sum_xxvv += xx * vv;
      sum_xyvv += xy * vv;
      sum_yyvv += yy * vv;

      sum_s   += s;
      sum_xs  += (double)x * s;
      sum_ys  += (double)y * s;
      sum_sv  += sv;
      sum_xsv += (double)x * sv;
      sum_ysv += (double)y * sv;
    }
  }

  /* Build the system of linear equations */
  double A[6 * 6] = {0};
  double b[6]     = {0};

  A[0 * 6 + 0] = sum_xxvv;
  A[0 * 6 + 1] = sum_xyvv;
  A[0 * 6 + 2] = sum_xvv;
  A[0 * 6 + 3] = sum_xxv;
  A[0 * 6 + 4] = sum_xyv;
  A[0 * 6 + 5] = sum_xv;
  b[0]         = sum_xsv;

  A[1 * 6 + 0] = sum_xyvv;
  A[1 * 6 + 1] = sum_yyvv;
  A[1 * 6 + 2] = sum_yvv;
  A[1 * 6 + 3] = sum_xyv;
  A[1 * 6 + 4] = sum_yyv;
  A[1 * 6 + 5] = sum_yv;
  b[1]         = sum_ysv;

  A[2 * 6 + 0] = sum_xvv;
  A[2 * 6 + 1] = sum_yvv;
  A[2 * 6 + 2] = sum_vv;
  A[2 * 6 + 3] = sum_xv;
  A[2 * 6 + 4] = sum_yv;
  A[2 * 6 + 5] = sum_v;
  b[2]         = sum_sv;

  A[3 * 6 + 0] = sum_xxv;
  A[3 * 6 + 1] = sum_xyv;
  A[3 * 6 + 2] = sum_xv;
  A[3 * 6 + 3] = sum_xx;
  A[3 * 6 + 4] = sum_xy;
  A[3 * 6 + 5] = sum_x;
  b[3]         = sum_xs;

  A[4 * 6 + 0] = sum_xyv;
  A[4 * 6 + 1] = sum_yyv;
  A[4 * 6 + 2] = sum_yv;
  A[4 * 6 + 3] = sum_xy;
  A[4 * 6 + 4] = sum_yy;
  A[4 * 6 + 5] = sum_y;
  b[4]         = sum_ys;

  A[5 * 6 + 0] = sum_xv;
  A[5 * 6 + 1] = sum_yv;
  A[5 * 6 + 2] = sum_v;
  A[5 * 6 + 3] = sum_x;
  A[5 * 6 + 4] = sum_y;
  A[5 * 6 + 5] = sum_1;
  b[5]         = sum_s;

  /* Update the system according to the modes */
  switch (extinctionMode) {
  case SAtmo_Ignore:
  case SAtmo_UseExisting:
    /* Do not solve bias for extinction (column 2), use existing value */
    for (int i = 0; i < 6; i++) {
      b[i] -= A[i * 6 + 2] * extinction->bias;
      A[i * 6 + 2] = 0.0;
      A[2 * 6 + i] = 0.0;
    }
    A[2 * 6 + 2] = 1.0;
    b[2]         = extinction->bias;
    /* fall through */
  case SAtmo_Constant:
    /* Do not solve gradients for extinction (columns 0 and 1) */
    for (int i = 0; i < 6; i++) {
      b[i] -= A[i * 6 + 0] * extinction->coef[0]
            + A[i * 6 + 1] * extinction->coef[1];
      A[i * 6 + 0] = 0.0;
      A[i * 6 + 1] = 0.0;
      A[0 * 6 + i] = 0.0;
      A[1 * 6 + i] = 0.0;
    }
    A[0 * 6 + 0] = 1.0;
    A[1 * 6 + 1] = 1.0;
    b[0]         = extinction->coef[0];
    b[1]         = extinction->coef[1];
    break;
  case SAtmo_Gradient:
    /* Solve for everything */
    break;
  }

  switch (backgroundMode) {
  case SAtmo_Ignore:
  case SAtmo_UseExisting:
    /* Do not solve bias for background (column 5), use existing value */
    for (int i = 0; i < 6; i++) {
      b[i] -= A[i * 6 + 5] * background->bias;
      A[i * 6 + 5] = 0.0;
      A[5 * 6 + i] = 0.0;
    }
    A[5 * 6 + 5] = 1.0;
    b[5]         = background->bias;
    /* fall through */
  case SAtmo_Constant:
    /* Do not solve gradients for background (columns 3 and 4) */
    for (int i = 0; i < 6; i++) {
      b[i] -= A[i * 6 + 3] * background->coef[0]
            + A[i * 6 + 4] * background->coef[1];
      A[i * 6 + 3] = 0.0;
      A[i * 6 + 4] = 0.0;
      A[3 * 6 + i] = 0.0;
      A[4 * 6 + i] = 0.0;
    }
    A[3 * 6 + 3] = 1.0;
    A[4 * 6 + 4] = 1.0;
    b[3]         = background->coef[0];
    b[4]         = background->coef[1];
    break;
  case SAtmo_Gradient:
    /* Solve for everything */
    break;
  }

  /* Solve the system using Gaussian elimination */
  gaussian_elimination(6, A, b);

  extinction->coef =
    (extinctionMode == SAtmo_Gradient) ?
    SVec2f(b[0], b[1]) :
    SVec2f(0.0f, 0.0f);
  extinction->bias = (float)b[2];

  background->coef =
    (backgroundMode == SAtmo_Gradient) ?
    SVec2f(b[3], b[4]) :
    SVec2f(0.0f, 0.0f);
  background->bias = (float)b[5];
}

/* ========================================================================= */

static void SAtmoCalibration_recoverMain(
  const SImage_t        *before,
  const SImage_t        *after,
  SAtmoCalibrationMode_t extinctionMode,
  SGradient_t           *extinction,
  SAtmoCalibrationMode_t backgroundMode,
  SGradient_t           *background)
{
  assert(extinction != NULL);
  assert(background != NULL);

  switch (before->format) {
  case SFmt_Invalid:
    break;
  case SFmt_Gray:
    SGradient_toGray(extinction);
    SGradient_toGray(background);
    SAtmoCalibration_recoverGray(
      before, after, SImage_pixelGray,
      extinctionMode, &extinction->gray,
      backgroundMode, &background->gray);
    break;
  case SFmt_RGB:
  case SFmt_SeparateRGB:
    SGradient_toColor(extinction);
    SGradient_toColor(background);
    SAtmoCalibration_recoverGray(
      before, after, SImage_pixelRed,
      extinctionMode, &extinction->red,
      backgroundMode, &background->red);
    SAtmoCalibration_recoverGray(
      before, after, SImage_pixelGreen,
      extinctionMode, &extinction->green,
      backgroundMode, &background->green);
    SAtmoCalibration_recoverGray(
      before, after, SImage_pixelBlue,
      extinctionMode, &extinction->blue,
      backgroundMode, &background->blue);
    break;
  }
}

void SAtmoCalibration_recover(
  const SImage_t        *before,
  const SImage_t        *after,
  SAtmoCalibrationMode_t extinctionMode,
  SGradient_t           *extinction,
  SAtmoCalibrationMode_t backgroundMode,
  SGradient_t           *background)
{
  SGradient_t extinctionGrad;
  SGradient_t backgroundGrad;

  if (extinction && extinctionMode == SAtmo_UseExisting) {
    extinctionGrad = *extinction;
  } else {
    extinctionGrad = SGradient_const(1.0f);
  }

  if (background && backgroundMode == SAtmo_UseExisting) {
    backgroundGrad = *background;
  } else {
    backgroundGrad = SGradient_const(0.0f);
  }

  SAtmoCalibration_recoverMain(
    before, after,
    extinctionMode, &extinctionGrad,
    backgroundMode, &backgroundGrad);

  if (extinction && extinctionMode > SAtmo_UseExisting) {
    *extinction = extinctionGrad;
  }
  if (background && backgroundMode > SAtmo_UseExisting) {
    *background = backgroundGrad;
  }
}
