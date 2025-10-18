#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cat.h"

int main(int argc, char **argv) {
  Params params;
  init_params(&params);
  parsing_params_in_input(argc, argv, &params);
  output(&params);

  return (EXIT_SUCCESS);
}
