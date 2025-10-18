#include "grep.h"

#include <errno.h>
#include <getopt.h>
#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void output(Param *params) {
  regex_t *regexs = (regex_t *)malloc(sizeof(regex_t) * SIZE_ALLOCATE);
  if (compile(regexs, &(params->flags))) {
    FILE *p_file;
    char *line = (char *)calloc(sizeof(char), SIZE_ALLOCATE);
    int *offsets = (int *)malloc(sizeof(int) * SIZE_ALLOCATE);
    int result = 1;
    for (int i = 0; i < params->files.file_count && result; i++) {
      result = open_file(params->files.files[i], &p_file, params->flags.s);
      if (result) {
        find(&p_file, line, offsets, params->files.files[i], regexs,
             &(params->flags));
        result = close_file(&p_file, params->flags.s);
      }
    }
    free(line);
    free(offsets);
  }

  for (int i = 0; i < params->flags.pattern_len; i++) regfree(&regexs[i]);
  free(regexs);
}

void find(FILE **p_file, char *line, int *offsets, char *file_name,
          regex_t *regexs, const Flags *flags) {
  Info info;
  init_info(&info, file_name, line, offsets);
  char ch, *p_line;
  p_line = line;
  while (1) {
    ch = getc(*p_file);
    *p_line = ch;
    if (*p_line == '\n') {
      *(++p_line) = '\0';
      info.len_line = p_line - line;
      info.len_offset = search_regex_matches(&info, regexs, flags->pattern_len);
      if (print_control(&info, flags)) break;
      p_line = line;

    } else if (*p_line == EOF) {
      if (p_line - line) {
        *p_line++ = '\n';
        *p_line = '\0';
        info.len_line = p_line - line;
        info.len_offset =
            search_regex_matches(&info, regexs, flags->pattern_len);
        print_control(&info, flags);
      }
      break;
    } else {
      p_line++;
    }
  }
  if (flags->c && !flags->l) {
    if (flags->h) printf("%s:", info.file);
    if (flags->v) {
      printf("%d\n", info.num_line - info.match_line - 1);
    } else {
      printf("%d\n", info.match_line);
    }
  }
}

int print_control(Info *info, const Flags *flags) {
  if (info->len_offset >= 0) {
    if (!flags->v && print_line(info, flags)) return (1);
    info->match_line++;
  } else if (flags->v) {
    if (print_line(info, flags)) return (1);
  }
  info->num_line++;
  return (0);
}

int print_line(const Info *info, const Flags *flags) {
  if (flags->l) {
    printf("%s\n", info->file);
    return (1);
  }
  if (flags->o && !flags->c) {
    if (flags->h) {
      printf("%s:", info->file);
    }
    if (flags->n) {
      printf("%d:", info->num_line);
    }
    printf("%s", info->line);

  } else if (!flags->c) {
    for (int i = 0; i <= info->len_offset; i++) {
      if (info->offsets[i] != -1) {
        if (flags->h) {
          printf("%s:", info->file);
        }
        if (flags->n) {
          printf("%d:", info->num_line);
        }
        printf("%.*s\n", info->offsets[i], info->line + i);
      }
    }
  }
  return (0);
}

int search_regex_matches(Info *info, regex_t *regexs, int pattern_len) {
  for (int i = 0; i < SIZE_ALLOCATE; i++) info->offsets[i] = -1;

  int *p_offset, i, len_offset;
  p_offset = info->offsets;
  for (i = 0, len_offset = -1; i < pattern_len; i++) {
    search_substring(&regexs[i], info, &p_offset, &len_offset);
  }
  return (len_offset);
}

void search_substring(regex_t *regex, Info *info, int **p_offset,
                      int *len_offset) {
  char *p_line = info->line;
  regmatch_t pmatch[1];
  regoff_t off, len;
  for (int i = 0;; i++) {
    if (regexec(regex, p_line, sizeof(pmatch) / sizeof(pmatch[0]), pmatch, 0))
      break;
    off = pmatch[0].rm_so + (p_line - info->line);
    len = pmatch[0].rm_eo - pmatch[0].rm_so;
    *len_offset = ((int)off > *len_offset) ? (int)off : *len_offset;
    *((*p_offset) + (int)off) = (int)len;
    p_line += pmatch[0].rm_eo;
  }
}

int open_file(const char *file, FILE **p_file, int error_file) {
  int result = 1;
  if ((*p_file = fopen(file, "r")) == NULL) {
    result = 0;
    if (!error_file) {
      fprintf(stderr, "grep: %s: %s\n", file, strerror(errno));
    }
  }
  return (result);
}

int close_file(FILE **p_file, int error_file) {
  int result = 1;
  if (fclose(*p_file)) {
    result = 0;
    if (!error_file) {
      fprintf(stderr, "%s", strerror(errno));
    }
  }
  return (result);
}

int compile(regex_t *regexs, Flags *flags) {
  int result = 1;
  for (int i = 0; i < flags->pattern_len; i++) {
    result = regcomp(&regexs[i], flags->pattern[i], flags->i);
    if (result) {
      size_t length = regerror(result, &regexs[i], NULL, 0);
      print_regerror(result, length, &regexs[i]);
    }
  }
  return (!result);
}

void print_regerror(int errcode, size_t length, regex_t *compiled) {
  char buffer[length];
  (void)regerror(errcode, compiled, buffer, length);
  fprintf(stderr, "%s\n", buffer);
}

int input(int argc, char **argv, Param *params) {
  int ch = -1;
  int result = 1;
  opterr = 0;
  while ((ch = getopt(argc, argv, OPTSTR)) != EOF && result) {
    result = write_params(argc, argv, ch, params);
  }
  if (result) {
    if (!params->flags.pattern_len) {
      strncpy(params->flags.pattern[params->flags.pattern_len++],
              argv[optind++], MAXLEN);
    }
    optind += params->flags.e;
    optind += params->flags.f;
    for (int i = 0; optind < argc; optind++, i++) {
      strncpy(params->files.files[i], argv[optind], MAXLEN);
      params->files.file_count++;
    }

    if (params->files.file_count == 1) {
      params->flags.h = 0;
    }
  }
  return (result);
}

int read_regex_in_file(Flags *flags, const char *file) {
  FILE *p_file;
  int result = 0;
  if (open_file(file, &p_file, 0)) {
    int ch = EOF;
    char **p_pattern = flags->pattern;
    int *string = &flags->pattern_len;
    int symbol = 0;
    while ((ch = getc(p_file)) != EOF) {
      *(*(p_pattern + *string) + symbol) = ch;
      if (*(*(p_pattern + *string) + symbol) == '\n') {
        *(*(p_pattern + *string) + symbol) = '\0';
        (*string)++;
        symbol = 0;
      } else {
        symbol++;
      }
    }
    if (close_file(&p_file, 0)) result = 1;
  }
  return (result);
}

int write_params(int argc, char **argv, char ch, Param *params) {
  int result = 1;
  switch (ch) {
    case 'e':
      result = optind < argc;
      if (result) {
        params->flags.e++;
        strncpy(params->flags.pattern[params->flags.pattern_len++],
                argv[optind], MAXLEN);
      }
      break;
    case 'f':
      result = optind < argc;
      if (result) {
        params->flags.pattern_file = 1;
        params->flags.f++;
        result = read_regex_in_file(&(params->flags), argv[optind]);
      }
      break;
    case 'i':
      params->flags.i = REG_ICASE;
      break;

    case 'c':
      params->flags.c++;
      break;

    case 'l':
      params->flags.l++;
      break;

    case 'n':
      params->flags.n++;
      break;

    case 'h':
      params->flags.h = 0;
      break;

    case 's':
      params->flags.s++;
      break;

    case 'o':
      params->flags.o = 0;
      break;

    case 'v':
      params->flags.v++;
      break;
  }
  return (result);
}

void init_info(Info *info, char *file, char *line, int *offsets) {
  info->file = file;
  info->line = line;
  info->num_line = 1;
  info->match_line = 0;
  info->len_line = 0;
  info->offsets = offsets;
  info->len_offset = -1;
}

void init_params(Param *params) {
  int flag_count = params->flags.flags_count = FLAGS_COUNT;
  params->files.file_count = 0;
  params->flags.pattern_file = 0;
  params->flags.pattern_len = 0;
  params->flags.pattern = allocate_mem(SIZE_ALLOCATE);
  params->files.files = allocate_mem(SIZE_ALLOCATE);
  int *p_flag = (int *)&(params->flags);
  for (int i = 0; i < flag_count - 1; i++) {
    *(p_flag + i) = 0;
  }
  params->flags.o = 1;
  params->flags.h = 1;
}

char **allocate_mem(int n) {
  char **arr = NULL;
  arr = (char **)malloc(sizeof(char *) * n * sizeof(char) * n);
  for (int i = 0; i < SIZE_ALLOCATE; i++) arr[i] = (char *)(arr + n + i * n);
  return (arr);
}

void free_params(Param *params) {
  free(params->flags.pattern);
  free(params->files.files);
}
