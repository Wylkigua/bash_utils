#include <errno.h>
#include <getopt.h>
#include <pcre.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "grep.h"

extern char *optarg;
extern int errno;
extern int opterr, optind;

int main(int argc, char **argv) {
  Param params;
  init_params(&params);
  if (input(argc, argv, &params)) {
    output(&params);
  }
  free_params(&params);
  return (EXIT_SUCCESS);
}
