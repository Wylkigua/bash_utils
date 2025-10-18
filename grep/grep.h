#ifndef GREP_H
#define GREP_H

#include <getopt.h>
#include <regex.h>
#include <stdio.h>

#define MAXLEN 1000
#define SIZE_ALLOCATE 1000
#define OPTSTR "e::f::ivclnhso"
#define FLAGS_COUNT 10
#define OVECCOUNT 900

typedef struct {
  int e;
  int f;
  int i;
  int v;
  int c;
  int l;
  int n;
  int h;
  int s;
  int o;
  int flags_count;
  char **pattern;
  int pattern_len;
  int pattern_file;
} Flags;

typedef struct {
  int file_count;
  char **files;
} Files;

typedef struct {
  Flags flags;
  Files files;
} Param;

typedef struct {
  char *file;
  int num_line;
  char *line;
  int match_line;
  int len_line;
  int *offsets;
  int len_offset;
} Info;

char **allocate_mem(int);
void free_params(Param *);
void init_params(Param *);
int input(int, char **, Param *);
void output(Param *);
int write_params(int, char **, char, Param *);
void find(FILE **, char *, int *, char *, regex_t *, const Flags *);
int compile(regex_t *, Flags *);
int open_file(const char *, FILE **, int);
int close_file(FILE **, int);
int search_regex_matches(Info *, regex_t *, int);
void search_substring(regex_t *, Info *, int **, int *);
int print_control(Info *, const Flags *);
void init_info(Info *, char *, char *, int *);
int print_line(const Info *, const Flags *);
int read_regex_in_file(Flags *, const char *);
void print_regerror(int, size_t, regex_t *);

#endif /* GREP_H */
