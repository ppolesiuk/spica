/* This file is part of Spica, released under MIT license.
 * See LICENSE for details.
 */

/* Author: Piotr Polesiuk, 2023 */

#include "SImage.h"

#include <assert.h>

static const SGradient_t dummyGradient = {
  .is_color = 0,
  .gray     = {
    .bias = 0.0f,
    .coef = { 0.0f, 0.0f }
  }
};

static SGrayGradient_t singlePixelGrayGradient(SVec2f_t pix) {
  return (SGrayGradient_t) {
    .bias = (pix[1] == 0.0f ? 0.0f : pix[0] / pix[1]),
    .coef = { 0.0f, 0.0f }
  };
}

/* Gradient of single-pixel image */
static SGradient_t singlePixelGradient(const SImage_t *image) {
  switch (image->format) {
  case SFmt_Invalid:
    return dummyGradient;
  case SFmt_Gray:
    return (SGradient_t) {
      .is_color = 0,
      .gray     = singlePixelGrayGradient(SImage_pixelGray(image, 0, 0))
    };
  case SFmt_RGB:
  case SFmt_SeparateRGB:
    return (SGradient_t) {
      .is_color = 1,
      .red      = singlePixelGrayGradient(SImage_pixelRed  (image, 0, 0)),
      .green    = singlePixelGrayGradient(SImage_pixelGreen(image, 0, 0)),
      .blue     = singlePixelGrayGradient(SImage_pixelBlue (image, 0, 0))
    };
  }
  assert(0 && "Impossible case");
}

/* ========================================================================= */

typedef SVec2f_t (*getLinPixel_t)(const SImage_t *image, unsigned x);

typedef SVec2f_t linGradient_t;

/* Find gradient of linear data. Returns vector { bias, coef } */
static linGradient_t linearGradient(
  const SImage_t *image, getLinPixel_t getPixel, unsigned size)
{
  unsigned center = size / 2;
  unsigned x1, x2, x;
  float    v1, v2;

  /* Find candiate on LHS */
  for (x = 0; 1; x++) {
    /* no candiate on LHS */
    if (x > center) return SVec2f(0.0f, 0.0f);
    SVec2f_t pix = getPixel(image, x);
    if (pix[1] > 0.0f) {
      x1 = x;
      v1 = pix[0] / pix[1];
      break;
    }
  }

  /* Find candidate on RHS */
  for (x = center + 1; 1; x++) {
    if (x >= size) return SVec2f(v1, 0.0f);
    SVec2f_t pix = getPixel(image, x);
    if (pix[1] > 0.0f) {
      x2 = x;
      v2 = pix[0] / pix[1];
      break;
    }
  }

  float coef = (v2 - v1) / (x2 - x1);
  float bias = v1 - coef * x1;
  float center_v = bias + coef * (center + 0.5f);

  /* Fixpoint iteration */
  int change;
  do {
    change = 0;

    for (x = 0; x < size; x++) {
      SVec2f_t pix = getPixel(image, x);
      /* skip empty pixels */
      if (pix[1] <= 0.0f) continue;

      float v = pix[0] / pix[1];

      /* skip pixels with value higher than gradient line */
      if (v >= bias + coef * x) continue; 
      
      float new_coef, new_bias;
      if (x <= center) {
        new_coef = (v2 - v) / (x2 - x);
        new_bias = v - new_coef * x;
      } else {
        new_coef = (v - v1) / (x - x1);
        new_bias = v1 - new_coef * x1;
      }
      float new_center_v = new_bias + new_coef * (center + 0.5f);

      if (new_center_v < center_v) {
        if (x <= center) {
          x1 = x;
          v1 = v;
        } else {
          x2 = x;
          v2 = v;
        }
        coef = new_coef;
        bias = new_bias;
        center_v = new_center_v;
        change = 1;
      }
    }
  } while (change);

  return SVec2f(bias, coef);
}

/* ------------------------------------------------------------------------- */

static SGrayGradient_t grayVerticalGradient(linGradient_t g) {
  return (SGrayGradient_t) {
    .bias = g[0],
    .coef = { 0.0f, g[1] }
  };
}

static SVec2f_t grayVerticalPixel(const SImage_t *image, unsigned x) {
  return image->data_gray[x * image->width];
}

static SVec2f_t redVerticalPixel(const SImage_t *image, unsigned x) {
  SVec4f_t pix = image->data_rgb[x * image->width];
  return SVec2f(pix[0], pix[3]);
}

static SVec2f_t greenVerticalPixel(const SImage_t *image, unsigned x) {
  SVec4f_t pix = image->data_rgb[x * image->width];
  return SVec2f(pix[1], pix[3]);
}

static SVec2f_t blueVerticalPixel(const SImage_t *image, unsigned x) {
  SVec4f_t pix = image->data_rgb[x * image->width];
  return SVec2f(pix[2], pix[3]);
}

static SVec2f_t sredVerticalPixel(const SImage_t *image, unsigned x) {
  return SImage_dataRed(image)[x * image->width];
}

static SVec2f_t sgreenVerticalPixel(const SImage_t *image, unsigned x) {
  return SImage_dataGreen(image)[x * image->width];
}

static SVec2f_t sblueVerticalPixel(const SImage_t *image, unsigned x) {
  return SImage_dataBlue(image)[x * image->width];
}

static SGradient_t verticalGradient(const SImage_t *image, unsigned height) {
  switch (image->format) {
  case SFmt_Invalid:
    return dummyGradient;
  case SFmt_Gray:
    return (SGradient_t) {
      .is_color = 0,
      .gray     = grayVerticalGradient(
        linearGradient(image, grayVerticalPixel, height))
    };
  case SFmt_RGB:
    return (SGradient_t) {
      .is_color = 1,
      .red      = grayVerticalGradient(
        linearGradient(image, redVerticalPixel, height)),
      .green    = grayVerticalGradient(
        linearGradient(image, greenVerticalPixel, height)),
      .blue     = grayVerticalGradient(
        linearGradient(image, blueVerticalPixel, height))
    };
  case SFmt_SeparateRGB:
    return (SGradient_t) {
      .is_color = 1,
      .red      = grayVerticalGradient(
        linearGradient(image, sredVerticalPixel, height)),
      .green    = grayVerticalGradient(
        linearGradient(image, sgreenVerticalPixel, height)),
      .blue     = grayVerticalGradient(
        linearGradient(image, sblueVerticalPixel, height))
    };
  }
  assert(0 && "Impossible case");
}

/* ------------------------------------------------------------------------- */

static SGrayGradient_t grayHorizontalGradient(linGradient_t g) {
  return (SGrayGradient_t) {
    .bias = g[0],
    .coef = { g[1], 0.0f }
  };
}

static SVec2f_t grayHorizontalPixel(const SImage_t *image, unsigned x) {
  return image->data_gray[x];
}

static SVec2f_t redHorizontalPixel(const SImage_t *image, unsigned x) {
  SVec4f_t pix = image->data_rgb[x];
  return SVec2f(pix[0], pix[3]);
}

static SVec2f_t greenHorizontalPixel(const SImage_t *image, unsigned x) {
  SVec4f_t pix = image->data_rgb[x];
  return SVec2f(pix[1], pix[3]);
}

static SVec2f_t blueHorizontalPixel(const SImage_t *image, unsigned x) {
  SVec4f_t pix = image->data_rgb[x];
  return SVec2f(pix[2], pix[3]);
}

static SVec2f_t sredHorizontalPixel(const SImage_t *image, unsigned x) {
  return SImage_dataRed(image)[x];
}

static SVec2f_t sgreenHorizontalPixel(const SImage_t *image, unsigned x) {
  return SImage_dataGreen(image)[x];
}

static SVec2f_t sblueHorizontalPixel(const SImage_t *image, unsigned x) {
  return SImage_dataBlue(image)[x];
}

static SGradient_t horizontalGradient(const SImage_t *image, unsigned width) {
  switch (image->format) {
  case SFmt_Invalid:
    return dummyGradient;
  case SFmt_Gray:
    return (SGradient_t) {
      .is_color = 0,
      .gray     = grayHorizontalGradient(
        linearGradient(image, grayHorizontalPixel, width))
    };
  case SFmt_RGB:
    return (SGradient_t) {
      .is_color = 1,
      .red      = grayHorizontalGradient(
        linearGradient(image, redHorizontalPixel, width)),
      .green    = grayHorizontalGradient(
        linearGradient(image, greenHorizontalPixel, width)),
      .blue     = grayHorizontalGradient(
        linearGradient(image, blueHorizontalPixel, width))
    };
  case SFmt_SeparateRGB:
    return (SGradient_t) {
      .is_color = 1,
      .red      = grayHorizontalGradient(
        linearGradient(image, sredHorizontalPixel, width)),
      .green    = grayHorizontalGradient(
        linearGradient(image, sgreenHorizontalPixel, width)),
      .blue     = grayHorizontalGradient(
        linearGradient(image, sblueHorizontalPixel, width))
    };
  }
  assert(0 && "Impossible case");
}

/* ========================================================================= */

typedef SVec2f_t (*getPixel_t)(const SImage_t *image, unsigned x, unsigned y);

typedef struct trianglePoint {
  unsigned x;
  unsigned y;
  float    v;
} trianglePoint_t;

/* Find non-empty point on image. Returns 0 on success */
static int findPoint(
  trianglePoint_t *p, const SImage_t *image, getPixel_t getPixel,
  int x_start, int x_step, int x_end,
  int y_start, int y_step, int y_end)
{
  for (int y = y_start; y != y_end; y += y_step) {
    for (int x = x_start; x != x_end; x += x_step) {
      SVec2f_t pix = getPixel(image, x, y);
      if (pix[1] > 0.0f) {
        p->x = x;
        p->y = y;
        p->v = pix[0] / pix[1];
        return 0;
      }
    }
  }
  return 1;
}

/* Check if given triangle contains given point + (0.5, 0.5).
 * Triangle on collinear points do not contain any point.
 * The last three parameters allows to change one point in a triangle */
static int containsCenter(unsigned center_x, unsigned center_y,
  const trianglePoint_t *triangle,
  int ch_index, unsigned ch_x, unsigned ch_y)
{
  long xc = center_x * 2 + 1;
  long yc = center_y * 2 + 1;
  long x[3];
  long y[3];
  long c[3];
  int i;

  for (i = 0; i < 3; i++) {
    x[i] = (ch_index == i ? ch_x : triangle[i].x) * 2;
    y[i] = (ch_index == i ? ch_y : triangle[i].y) * 2;
  }
  /* Skip collinear trianlges */
  if ((x[1] - x[0]) * (y[2] - y[0]) - (x[2] - x[0]) * (y[1] - y[0]) == 0)
    return 0;

  for (i = 0; i < 3; i++) {
    int j = (i+1) % 3;
    c[i] = (x[i] - xc) * (y[j] - yc) - (x[j] - xc) * (y[i] - xc);
  }

  return (c[0] >= 0 && c[1] >= 0 && c[2] >= 0)
      || (c[0] <= 0 && c[1] <= 0 && c[2] <= 0);
}

static SGrayGradient_t gradientOfTriangle(trianglePoint_t *triangle) {
  /* triangle base */
  long b1_x = (long)triangle[1].x - (long)triangle[0].x;
  long b1_y = (long)triangle[1].y - (long)triangle[0].y;
  long b2_x = (long)triangle[2].x - (long)triangle[0].x;
  long b2_y = (long)triangle[2].y - (long)triangle[0].y;

  SVec2f_t b1 = SVec2f(b1_x, b1_y);
  SVec2f_t b2 = SVec2f(b2_x, b2_y);

  /* gradient in trianlge base */
  SGrayGradient_t tg = {
    .bias = triangle[0].v,
    .coef = { triangle[1].v - triangle[0].v, triangle[2].v - triangle[0].v }
  };

  /* inverted b matrix */
  float det_inv = 1.0f / (b1_x * b2_y - b1_y * b2_x);

  SVec2f_t c1 = det_inv * SVec2f(b2[1], -b1[1]);
  SVec2f_t c2 = det_inv * SVec2f(-b2[0], b1[0]);

  /* coordinates of (0,0) point in a base of triangle */
  SVec2f_t origin =
    -(c1 * (float)(triangle[0].x) + c2 * (float)(triangle[0].y));

  float bias = SGrayGradient_value(tg, origin[0], origin[1]);
  SVec2f_t coef_x = c1 * tg.coef;
  SVec2f_t coef_y = c2 * tg.coef;

  return (SGrayGradient_t) {
    .bias = bias,
    .coef = { coef_x[0] + coef_x[1], coef_y[0] + coef_y[1] }
  };
}

/* ------------------------------------------------------------------------- */

static SGrayGradient_t grayGradient(
  const SImage_t *image, getPixel_t getPixel, unsigned width, unsigned height)
{
  unsigned center_x = width / 2;
  unsigned center_y = width / 2;

  /* find any triangle that contains center */
  trianglePoint_t triangle[3];
  if (findPoint(triangle + 0, image, getPixel,
        0, 1, center_x+1,
        0, 1, center_y+1))
    return dummyGradient.gray;
  if (findPoint(triangle + 1, image, getPixel,
        width-1, -1, center_x,
        0, 1, center_y+1))
    return dummyGradient.gray;
  if (findPoint(triangle + 2, image, getPixel,
        0, 1, center_x+1,
        height-1, -1, center_y))
    return dummyGradient.gray;

  if (!containsCenter(center_x, center_y, triangle, 3, 0, 0)) {
    if (findPoint(triangle + 0, image, getPixel,
        width-1,  -1, center_x,
        height-1, -1, center_y))
      return dummyGradient.gray;
  }

  /* initialize loop */
  SGrayGradient_t g = gradientOfTriangle(triangle);
  float center_v = SGrayGradient_value(g, center_x + 0.5f, center_y + 0.5f);

  /* Fixpoint iteration */
  int change;
  do {
    change = 0;

    for (unsigned y = 0; y < height; y++) {
      for (unsigned x = 0; x < width; x++) {
        SVec2f_t pix = getPixel(image, x, y);
        /* skip empty pixels */
        if (pix[1] <= 0.0f) continue;

        float v = pix[0] / pix[1];

        /* skip pixels with value higher than gradient value */
        if (v >= SGrayGradient_value(g, x, y)) continue; 

        /* which point int triangle should be changed? */
        int n = -1;
        for (int i = 0; i < 3; i++) {
          if (containsCenter(center_x, center_y, triangle, i, x, y)) {
            n = i;
            break;
          }
        }
        /* skip on nothing (collinear points) */
        if (n == -1) continue;

        trianglePoint_t old = triangle[n];
        /* update triangle and gradient */
        triangle[n].x = x;
        triangle[n].y = y;
        triangle[n].v = v;
        SGrayGradient_t new_g = gradientOfTriangle(triangle);
        float new_center_v =
          SGrayGradient_value(new_g, center_x + 0.5f, center_y + 0.5f);
        /* accept or rollback change */
        if (new_center_v < center_v) {
          change = 1;
          g = new_g;
          center_v = new_center_v;
        } else {
          triangle[n] = old;
        }
      }
    }
  } while (change);

  return g;
}

/* ------------------------------------------------------------------------- */

static SVec2f_t grayPixel(const SImage_t *image, unsigned x, unsigned y) {
  return image->data_gray[y * image->width + x];
}

static SVec2f_t redPixel(const SImage_t *image, unsigned x, unsigned y) {
  SVec4f_t pix = image->data_rgb[y * image->width + x];
  return SVec2f(pix[0], pix[3]);
}

static SVec2f_t greenPixel(const SImage_t *image, unsigned x, unsigned y) {
  SVec4f_t pix = image->data_rgb[y * image->width + x];
  return SVec2f(pix[1], pix[3]);
}

static SVec2f_t bluePixel(const SImage_t *image, unsigned x, unsigned y) {
  SVec4f_t pix = image->data_rgb[y * image->width + x];
  return SVec2f(pix[2], pix[3]);
}

static SVec2f_t sredPixel(const SImage_t *image, unsigned x, unsigned y) {
  return SImage_dataRed(image)[y * image->width + x];
}

static SVec2f_t sgreenPixel(const SImage_t *image, unsigned x, unsigned y) {
  return SImage_dataGreen(image)[y * image->width + x];
}

static SVec2f_t sbluePixel(const SImage_t *image, unsigned x, unsigned y) {
  return SImage_dataBlue(image)[y * image->width + x];
}

static SGradient_t fullGradient(
  const SImage_t *image, unsigned width, unsigned height)
{
  switch (image->format) {
  case SFmt_Invalid:
    return dummyGradient;
  case SFmt_Gray:
    return (SGradient_t) {
      .is_color = 0,
      .gray     = grayGradient(image, grayPixel, width, height)
    };
  case SFmt_RGB:
    return (SGradient_t) {
      .is_color = 1,
      .red      = grayGradient(image, redPixel,   width, height),
      .green    = grayGradient(image, greenPixel, width, height),
      .blue     = grayGradient(image, bluePixel,  width, height)
    };
  case SFmt_SeparateRGB:
    return (SGradient_t) {
      .is_color = 1,
      .red      = grayGradient(image, sredPixel,   width, height),
      .green    = grayGradient(image, sgreenPixel, width, height),
      .blue     = grayGradient(image, sbluePixel,  width, height)
    };
  }
  assert(0 && "Impossible case");
}

/* ========================================================================= */

/* Find image gradient based on rectangle of size width x height */
static SGradient_t backgroundGradient(
  const SImage_t *image, unsigned width, unsigned height)
{
  if (width <= 1) {
    if (height <= 1) return singlePixelGradient(image);
    else return verticalGradient(image, height);
  } else {
    if (height <= 1) return horizontalGradient(image, width);
    else return fullGradient(image, width, height);
  }
}

SGradient_t SImage_backgroundGradient(const SImage_t *image, unsigned binning)
{
  assert(binning > 0);

  if (image->format == SFmt_Invalid || image->width == 0 || image->height == 0)
    return dummyGradient;

  if (binning == 1)
    return backgroundGradient(image, image->width, image->height);

  SImage_t img;
  SImage_scaleDown_at(&img, image, binning);
  SGradient_t g =
    backgroundGradient(&img, image->width / binning, image->height / binning);
  SImage_deinit(&img);

  return SGradient_scale(&g, binning);
}
