/* This code is in the public domain.
 * You may freely copy parts of it in your project
 */

/* This program is a larger example that uses Spica library. This program
 * allows to align (register) sequence of PNG images and stack them into
 * a single PNG image. The program uses argp library to implement command-line
 * arguments parsing. All configuration parameters of used Spica components
 * (SStarFinder, SSmallChangeAligner, SBrutAligner, SStarMatcher) may be set.
 * However, the default settings work quite well. */

#include <SImage.h>
#include <SCoarseAlign.h>
#include <SStarFinder.h>
#include <SStarMatcher.h>

#include <argp.h>
#include <stdarg.h>
#include <stdlib.h>

/* ========================================================================= */
/* Description of command-line parameters */

#define OPT_DARK_FRAME        'd'
#define OPT_OUTPUT            'o'
#define OPT_VERBOSE           'v'
#define OPT_BR_THRESHOLD      'b'
#define OPT_CAN_THRESHOLD     'c'
#define OPT_FIT_STEPS         'F'
#define OPT_MIN_DIST          'm'
#define OPT_SIGMA             's'
#define OPT_SC_DIST_THRESHOLD 't'
#define OPT_SC_STARS          'M'
#define OPT_B_DIST_TOL        'T'
#define OPT_B_RANK_STARS      'R'
#define OPT_B_REF_STARS       'r'
#define OPT_B_ROT_TOL         'O'
#define OPT_B_SCALE_TOL       'S'
#define OPT_B_STAR_NUM        'n'
#define OPT_M_DIST_THRESHOLD  'D'

const char *argp_program_version = "align v0.1";
static const char doc[] =
  "Example program that aligns and stacks images";

static struct argp_option options[] = {
  { "dark-frame", OPT_DARK_FRAME, "FILE", 0,
    "Subtract dark frame read from SIWW FILE." },
  { "output", OPT_OUTPUT, "FILE", 0,
    "Set name of the output file." },
  { "verbose", OPT_VERBOSE, 0, 0,
    "Increase verbosity level. May be used several times." },
  { "brightness-threshold", OPT_BR_THRESHOLD, "NUM", 0,
    "Brightness threshold of star finder." },
  { "candidate-threshold", OPT_CAN_THRESHOLD, "NUM", 0,
    "Candidate star brightness threshold of star finder." },
  { "fit-steps", OPT_FIT_STEPS, "N", 0,
    "Number of steps of fitting algorithm." },
  { "min-dist", OPT_MIN_DIST, "NUM", 0,
    "Minimal distance between stars to be considered as separate stars, "
    "during star-finding." },
  { "sigma", OPT_SIGMA, "NUM", 0,
    "Expected size of stars on images "
    "(sigma parameter of a gaussian function)." },
  { "sc-dist-threshold", OPT_SC_DIST_THRESHOLD, "NUM", 0,
    "Distance threshold used by small-change coarse aligner" },
  { "sc-stars", OPT_SC_STARS, "N", 0,
    "Minimal number of matched stars required by small-change aligner, "
    "required to accept the alignment." },
  { "b-dist-tol", OPT_B_DIST_TOL, "NUM", 0,
    "Maximal distance between stars when they are treated as the same star, "
    "used by slower coarse-align algorithm." },
  { "b-rank-stars", OPT_B_RANK_STARS, "N", 0,
    "Number of stars used for ranking of coarse alignment (slower "
    "algorithm)." },
  { "b-ref-stars", OPT_B_REF_STARS, "N", 0,
    "Number of reference stars used by slower coarse alignment." },
  { "b-rot-tol", OPT_B_ROT_TOL, "NUM", 0,
    "Allowed rotation tolerance of coarse aligner." },
  { "b-scale-tol", OPT_B_SCALE_TOL, "NUM", 0,
    "Allowed scale tolerance of coarse aligner." },
  { "b-star-num", OPT_B_STAR_NUM, "N", 0,
    "Number of input stars used by slower coarse alignment." },
  { "m-dist-threshold", OPT_M_DIST_THRESHOLD, "NUM", 0,
    "Maximal distance between two stars, to be considered as same star "
    "by a star matcher." },
  { 0 }
};

/* ========================================================================= */
/* Input sequence of images.
 *
 * Images are processed in three passes. During the first pass (command line
 * parsing) only file name is set, and image is not loaded into a memory.
 * The second pass is the most complex. For each image algorithm searches for
 * stars on an image, and align them with stars found on previous images.
 * Transformation that aligns stars is stored in the `images` array. 
 * Additionally, during the second pass determines the format and the size of
 * the result image. During the third pass, images are stacked together.
 * */
typedef struct image {
  const char  *fname;
  STransform_t transform;
} image_t;
static image_t *images;    /* Array of images */
static size_t image_n = 0; /* Length of `images` array */

/* Settings of Spica algorithms used by this program */
static SStarFinder_t         finder;
static SSmallChangeAligner_t scAligner;
static SBrutAligner_t        brutAligner;
static SStarMatcher_t        matcher;

/* Optional dark frame. May be set by --dark-frame command line option.
 * Dark frame should be stored in SIWW file, obtained by e.g. stack-dark
 * example program */
static SImage_t *dark_frame = NULL;

/* Output file name. May be changed by --output command line option */
static const char *output_fname = "output.png";

/* ========================================================================= */
/* Logging */

static int loglevel = 0;

static void s_log(int level, const char *fmt, ...) {
  if (level > loglevel) return;

  va_list args;
  va_start(args, fmt);

  vfprintf(stderr, fmt, args);
  fprintf(stderr, "\n");

  va_end(args);
}

/* ========================================================================= */
/* Parsing command-line options */

static float parse_float(struct argp_state *state, const char *arg) {
  char *endp;
  float result = strtof(arg, &endp);
  if (*endp) argp_error(state, "Invalid floating-point value: %s", arg);
  return result;
}

static int parse_int(struct argp_state *state, const char *arg) {
  char *endp;
  int result = strtol(arg, &endp, 10);
  if (*endp) argp_error(state, "Invalid number: %s", arg);
  return result;
}

static error_t parse_opt(int key, char *arg, struct argp_state *state) {
  switch (key) {
  case OPT_DARK_FRAME:
    /* free dark_frame if it is already set */
    if (dark_frame) SImage_free(dark_frame);
    /* and load a new one */
    dark_frame = SImage_loadSIWW(arg);
    if (dark_frame && dark_frame->format == SFmt_Invalid) {
      SImage_free(dark_frame);
      dark_frame = NULL;
    }
    break;
  case OPT_OUTPUT:
    output_fname = arg;
    break;
  case OPT_VERBOSE:
    loglevel++;
    break;
  case OPT_BR_THRESHOLD:
    finder.brightnessThreshold = parse_float(state, arg);
    break;
  case OPT_CAN_THRESHOLD:
    finder.candidateThreshold = parse_float(state, arg);
    break;
  case OPT_FIT_STEPS:
    finder.fitSteps = parse_int(state, arg);
    break;
  case OPT_MIN_DIST:
    finder.minDist = parse_float(state, arg);
    break;
  case OPT_SIGMA:
    finder.sigma = parse_float(state, arg);
    break;
  case OPT_SC_DIST_THRESHOLD:
    scAligner.distThreshold = parse_float(state, arg);
    break;
  case OPT_SC_STARS:
    scAligner.minStarN = parse_int(state, arg);
    break;
  case OPT_B_DIST_TOL:
    brutAligner.distTol = parse_float(state, arg);
    break;
  case OPT_B_RANK_STARS:
    brutAligner.rankStarN = parse_int(state, arg);
    break;
  case OPT_B_REF_STARS:
    brutAligner.refStarN = parse_int(state, arg);
    break;
  case OPT_B_ROT_TOL:
    brutAligner.rotTol = parse_float(state, arg);
    break;
  case OPT_B_SCALE_TOL:
    brutAligner.scaleTol = parse_float(state, arg);
    break;
  case OPT_B_STAR_NUM:
    brutAligner.starN = parse_int(state, arg);
    break;
  case OPT_M_DIST_THRESHOLD:
    matcher.distThreshold = parse_float(state, arg);
    break;
  case ARGP_KEY_ARG:
    images[image_n++].fname = arg;
    break;
  default:
    return ARGP_ERR_UNKNOWN;
  }
  return 0;
}

static struct argp argp = { options, parse_opt, 0, doc, 0, 0, 0 };

/* ========================================================================= */
/* main function */

int main(int argc, char **argv) {
  /* Alloc array of images. Number of images cannot be larger than number of
   * command line options. */
  images = malloc(sizeof(image_t) * (argc - 1));

  /* Initialize Spica components with default values */
  SStarFinder_init(&finder);
  SSmallChangeAligner_init(&scAligner);
  SBrutAligner_init(&brutAligner);
  SStarMatcher_init(&matcher);

  /* ----------------------------------------------------------------------- */
  /* First pass -- parsing command line options */
  argp_parse(&argp, argc, argv, 0, 0, 0);

  /* ----------------------------------------------------------------------- */
  /* Second pass -- alignment */

  size_t i;
  /* Format and size of the result image */
  SImageFormat_t fmt = SFmt_Invalid;
  SBoundingBox_t bb = SBoundingBox_empty();
  /* Transformation of previous frame, used by SSmallChangeAligner_t */
  STransform_t prev_tr = { .type = STr_Drop };
  
  for (i = 0; i < image_n; i++) {
    /* Default transformation -- drop the image -- in case of error during
     * loading an image or insufficient number of stars on it */
    images[i].transform.type = STr_Drop;

    /* Load an image */
    s_log(1, "%s", images[i].fname);
    SImage_t img;
    if (SImage_loadPNG_at(&img, images[i].fname)) {
      SImage_deinit(&img);
      continue;
    }

    /* Subtract dark frame, if any */
    if (dark_frame)
      SImage_sub(&img, 0, 0, dark_frame);
    
    /* Find stars */
    SStarSet_t sset;
    SStarSet_init(&sset);
    SStarFinder_findStars_at(&sset, &finder, &img);
    s_log(2, "\t%d stars found", (int)sset.length);

    /* skip this image, if there are too few stars on it */
    if (sset.length <= 2) {
      SStarSet_deinit(&sset);
      SImage_deinit(&img);
      continue;
    }

    if (matcher.sset.length == 0) {
      /* If it is the first valid image in the sequence, then just use the
       * identity transformation */
      images[i].transform.type = STr_Identity;
    } else {
      /* Otherwise, try to coarsely align to previously found stars using fast
       * SSmallChangeAligner algorithm */
      s_log(4, "\tRunning SSmallChangeAligner");
      STransform_t tr =
        SSmallChangeAligner_align(&scAligner, &matcher.sset, &prev_tr, &sset);
      /* On failure, fallback to slower SBrutAligner algorithm */
      if (tr.type == STr_Drop) {
        s_log(3, "\tFallback to SBrutAligner");
        tr = SBrutAligner_align(&brutAligner, &matcher.sset, &sset);
      }

      if (tr.type != STr_Drop) {
        s_log(4, "\tMatching stars");
        /* Then, perform fine matching and alignment using SStarMatcher */
        SStarMatcher_matchStars(&matcher, &tr, &sset);
        images[i].transform = SStarMatcher_getTransform(&matcher, &sset);
      }
    }

    if (images[i].transform.type != STr_Drop) {
      /* Set `prev_tr` to help to align next image */
      prev_tr = images[i].transform;
      /* On matching success, update set of stars in SStarMatcher */
      SStarMatcher_update(&matcher, &images[i].transform, &sset);
      /* and update format and size of the result image */
      if (img.format > fmt) {
        fmt = img.format;
      }
      bb = SBoundingBox_union(bb,
        STransform_boundingBox(&images[i].transform,
          SImage_boundingBox(&img)));
    }

    /* Cleanup temporary data used during processing of this image */
    SStarSet_deinit(&sset);
    SImage_deinit(&img);
  }

  /* If the result image is empty, abort the program */
  if (SBoundingBox_isEmpty(bb) || fmt == SFmt_Invalid) {
    return 1;
  }

  /* ----------------------------------------------------------------------- */
  /* Third pass -- stacking */

  /* Compute additional shift transformation that puts most top-left pixel to
   * (0,0) coordinates */
  STransform_t tr_shift = STransform_shift(-SVec2f(bb.minX, bb.minY));
  /* Determine the size of the result image */
  int width  = (int)(bb.maxX - bb.minX) + 1;
  int height = (int)(bb.maxY - bb.minY) + 1;
  /* Create result image, end initialize it with "no data". */
  SImage_t result;
  s_log(1, "Creating image of size %d x %d", width, height);
  SImage_init(&result, width, height, fmt);
  SImage_clear(&result);

  for (i = 0; i < image_n; i++) {
    /* Skip images marked with STr_Drop transformation. They are invalid, or
     * the program was unable to align them */
    if (images[i].transform.type == STr_Drop) continue;
    /* Compose transformation with `tr_shift`, in order to fix image
     * coordinates */
    images[i].transform = STransform_compose(&tr_shift, &images[i].transform);

    /* Load the image again. Images are loaded twice in order to save memory.
     * In such approach the entire sequence may be enormously large, because is
     * only only image is stored in memory at time */
    s_log(1, "%s", images[i].fname);
    SImage_t img;
    if (SImage_loadPNG_at(&img, images[i].fname)) {
      SImage_deinit(&img);
      continue;
    }

    /* Subtract dark frame, again */
    if (dark_frame)
      SImage_sub(&img, 0, 0, dark_frame);
    
    /* Stack image on the result */
    SImage_stackTr(&result, &images[i].transform, &img);

    SImage_deinit(&img);
  }

  /* Save the result image */
  SImage_savePNG(&result, SPF_RGB16, output_fname);

  return 0;
}
