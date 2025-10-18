#ifndef CAT_H
#define CAT_H

#define MAXLEN 1000
#define PARAMS "besntvET"
#include <stdbool.h>
#include <stdio.h>
#define PARAMS_COUNT 8

typedef struct {
  bool b;
  bool e;
  bool s;
  bool n;
  bool t;
  bool v;
  bool E;
  bool T;
} Flags;

typedef enum { b, e, s, n, t, v, E, T, None } t_flags;

typedef struct {
  Flags flags;
  int f_count;
  char files[MAXLEN][MAXLEN];
} Params;

void parsing_params_in_input(int, char **, Params *);
void output(Params *);
void init_params(Params *);
void function_calls(FILE *, t_flags);
void number_lines(FILE *, int);
void show_ends(FILE *, int);
void open_files(t_flags, char (*)[MAXLEN], int);
void suppress_empty(FILE *);
void show_tabs(FILE *, int);
void write_parameters(char, Flags *);

#endif /* CAT_H */
