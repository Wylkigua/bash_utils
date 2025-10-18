#include "cat.h"

#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void output(Params *params) {
  if (params->flags.b) {
    if (params->f_count > 0) {
      open_files(b, params->files, params->f_count);
    } else {
      function_calls(stdin, b);
    }
  } else if (params->flags.e) {
    if (params->f_count > 0) {
      open_files(e, params->files, params->f_count);
    } else {
      function_calls(stdin, e);
    }
  } else if (params->flags.s) {
    if (params->f_count > 0) {
      open_files(s, params->files, params->f_count);
    } else {
      function_calls(stdin, s);
    }
  } else if (params->flags.n) {
    if (params->f_count > 0) {
      open_files(n, params->files, params->f_count);
    } else {
      function_calls(stdin, n);
    }
  } else if (params->flags.t) {
    if (params->f_count > 0) {
      open_files(t, params->files, params->f_count);
    } else {
      function_calls(stdin, t);
    }
  } else if (params->flags.E) {
    if (params->f_count > 0) {
      open_files(E, params->files, params->f_count);
    } else {
      function_calls(stdin, E);
    }
  } else if (params->flags.T) {
    if (params->f_count > 0) {
      open_files(T, params->files, params->f_count);
    } else {
      function_calls(stdin, T);
    }
  } else if (params->f_count > 0) {
    open_files(None, params->files, params->f_count);
  } else {
    function_calls(stdin, None);
  }
}

void open_files(t_flags flag, char files[][MAXLEN], int f_count) {
  FILE *_file;
  for (int i = 0; i < f_count; i++) {
    if ((_file = fopen(files[i], "r")) == NULL) {
      fprintf(stderr, "cat: %s: No such file or directory\n", files[i]);
      exit(EXIT_FAILURE);
    }
    function_calls(_file, flag);
    if (ferror(_file) != 0) {
      fprintf(stderr, "cat: read file %s\n", files[i]);
      exit(EXIT_FAILURE);
    }
    fclose(_file);
  }
}

void function_calls(FILE *_file, t_flags flag) {
  if (flag == None) {
    char ch;
    while ((ch = getc(_file)) != EOF) {
      putc(ch, stdout);
    }
  } else if (flag == b) {
    number_lines(_file, 0);

  } else if (flag == n) {
    number_lines(_file, 1);

  } else if (flag == e) {
    show_ends(_file, 0);

  } else if (flag == E) {
    show_ends(_file, 1);

  } else if (flag == s) {
    suppress_empty(_file);

  } else if (flag == t) {
    show_tabs(_file, 0);

  } else if (flag == T) {
    show_tabs(_file, 1);
  }
}

void show_tabs(FILE *_file, int nonprint) {
  char ch;
  while ((ch = getc(_file)) != EOF) {
    if (ch == '\t' || (ch >= 0 && ch <= 31 && ch != '\n') ||
        (ch == 127 && !nonprint)) {
      putc('^', stdout);
      ch += 64;
    }
    putc(ch, stdout);
  }
}

void suppress_empty(FILE *_file) {
  static int new_line = 0;
  static char prev = '\n';
  char ch = 0;
  while ((ch = getc(_file)) != EOF) {
    if (prev == '\n' && ch == '\n') new_line++;
    if (ch != '\n') new_line = 0;
    if (new_line < 2) putc(ch, stdout);
    prev = ch;
  }
}

void show_ends(FILE *_file, int nonprint) {
  char ch;
  while ((ch = getc(_file)) != EOF) {
    if (((ch >= 0 && ch <= 8) || (ch >= 11 && ch <= 31) || ch == 127) &&
        !nonprint) {
      putc('^', stdout);
      ch += 64;
    }
    if (ch == '\n') {
      putc('$', stdout);
    }
    putc(ch, stdout);
  }
}

void number_lines(FILE *_file, int empty) {
  int ch;
  static int cnt = 0;
  ch = getc(_file);
  while (1) {
    if (ch == EOF) break;
    if (empty) {
      printf("%6d\t", ++cnt);
    } else if (ch != '\n') {
      printf("%6d\t", ++cnt);
    }
    putc(ch, stdout);

    while (1) {
      if (ch == '\n') {
        ch = getc(_file);
        break;
      }
      ch = getc(_file);
      if (ch == EOF) break;
      putc(ch, stdout);
    }
  }
}

void parsing_params_in_input(int argc, char **argv, Params *params) {
  int c = -1;
  int option_index = 0;
  struct option long_options[] = {
      {"number-nonblank", 0, 0, 'b'}, {"number", 0, 0, 'n'},
      {"squeeze-blank", 0, 0, 's'},   {"show-tabs", 0, 0, 'T'},
      {"show-ends", 0, 0, 'E'},       {0, 0, 0, 0}};

  while ((c = getopt_long(argc, argv, PARAMS, long_options, &option_index)) !=
         -1) {
    write_parameters(c, &params->flags);
  }

  for (int i = 0; optind < argc; optind++, i++) {
    strcpy(params->files[i], argv[optind]);
    params->f_count++;
  }
}

void init_params(Params *params) {
  params->f_count = 0;
  params->flags.b = false;
  params->flags.e = false;
  params->flags.s = false;
  params->flags.n = false;
  params->flags.t = false;
  params->flags.v = false;
  params->flags.E = false;
  params->flags.T = false;
}

void write_parameters(char param, Flags *flags) {
  switch (param) {
    case 'b':
      flags->b = true;
      break;
    case 'e':
      flags->e = true;
      flags->v = true;
      break;

    case 's':
      flags->s = true;
      break;

    case 'n':
      flags->n = true;
      break;

    case 't':
      flags->t = true;
      flags->v = true;
      break;

    case 'E':
      flags->E = true;
      break;

    case 'T':
      flags->T = true;
      break;
  }
}
