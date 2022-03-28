/* This code is in the public domain.
 * You may freely copy parts of it in your project
 */

/* This program shows simple usage of Spica library to stack several PNG
 * calibration images (e.g. dark frames) into one SIWW file.
 *
 * This program recognizes only one command line option: -o FILE which allows
 * to specify the name of the output file. Default is dark.siww */

#include <SImage.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void print_help(const char *program) {
  printf("Usage: %s [-o FILE] [FILE]...\n"
    "Stack calibration images into one image. Command line option -o FILE\n"
    "allows to set the name of the output file (default is dark.siww)\n",
    program);
  exit(0);
}

int main(int argc, char **argv) {
  /* name of the output file */
  const char *out_fname = "dark.siww";
  /* Output image, initialized to SFmt_Invalid image */
  SImage_t dark;
  SImage_init(&dark, 0, 0, SFmt_Invalid);

  if (argc <= 1) print_help(argv[0]);

  for (int i = 1; i < argc; i++) {
    /* Check for known command-line options */
    if (strcmp(argv[i], "-o") == 0 && i+1 < argc) {
      out_fname = argv[++i];
      continue;
    } else if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0)
      print_help(argv[0]);
    
    /* Load another image */
    SImage_t img;
    if (SImage_loadPNG_at(&img, argv[i])) continue;
    if (img.format == SFmt_Invalid) continue;

    /* Stack loaded image to result file ... */
    if (dark.format == SFmt_Invalid) {
      /* or just clone it, if it is the first image */
      SImage_clone_at(&dark, &img);
    } else {
      SImage_stack(&dark, 0, 0, &img);
    }

    /* Deinitialize image and free memory */
    SImage_deinit(&img);
  }

  /* If result contains no images */
  if (dark.format == SFmt_Invalid) return 1;

  int status = SImage_saveSIWW(&dark, out_fname);
  SImage_deinit(&dark);

  return status ? 1 : 0;
}
