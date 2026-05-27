#include "grep_parser.h"

#include <errno.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void InitFlags(grep_flags *flags) {
  flags->e = 0;
  flags->i = 0;
  flags->v = 0;
  flags->c = 0;
  flags->l = 0;
  flags->n = 0;
  flags->h = 0;
  flags->s = 0;
  flags->f = 0;
  flags->o = 0;
  flags->patterns = NULL;
  flags->patterns_count = 0;
  flags->patterns_capacity = 0;
}

static int AppendPattern(grep_flags *flags, const char *pattern) {
  if (flags->patterns_count >= flags->patterns_capacity) {
    int new_capacity =
        (flags->patterns_capacity == 0) ? 4 : flags->patterns_capacity * 2;
    char **tmp = realloc(flags->patterns, new_capacity * sizeof(char *));
    if (tmp == NULL) {
      return 0;
    }
    flags->patterns = tmp;
    flags->patterns_capacity = new_capacity;
  }

  flags->patterns[flags->patterns_count] = strdup(pattern);
  if (flags->patterns[flags->patterns_count] == NULL) {
    return 0;
  }

  flags->patterns_count++;
  return 1;
}

static int ReadPatternsFromFile(const char *filename, grep_flags *flags,
                                int silent) {
  FILE *fp = fopen(filename, "r");
  if (fp == NULL) {
    if (!silent) {
      fprintf(stderr, "s21_grep: %s: %s\n", filename, strerror(errno));
    }
    return 0;
  }

  char *line = NULL;
  size_t len = 0;
  ssize_t read;
  int success = 1;

  while ((read = getline(&line, &len, fp)) != -1) {
    if (read > 0 && line[read - 1] == '\n') {
      line[read - 1] = '\0';
    }
    if (!AppendPattern(flags, line)) {
      success = 0;
      break;
    }
  }

  free(line);
  fclose(fp);
  return success;
}

void FreeGrepFlags(grep_flags *flags) {
  if (flags == NULL) {
    return;
  }

  for (int i = 0; i < flags->patterns_count; i++) {
    free(flags->patterns[i]);
  }
  free(flags->patterns);
  flags->patterns = NULL;
  flags->patterns_count = 0;
  flags->patterns_capacity = 0;
}

static void SetFlag(int option, grep_flags *flags) {
  if (option == 'i') flags->i = 1;
  if (option == 'v') flags->v = 1;
  if (option == 'c') flags->c = 1;
  if (option == 'l') flags->l = 1;
  if (option == 'n') flags->n = 1;
  if (option == 'h') flags->h = 1;
  if (option == 's') flags->s = 1;
  if (option == 'o') flags->o = 1;
}

int ParseArguments(int argc, char **argv, grep_flags *flags, int *file_index) {
  int option = 0;

  InitFlags(flags);

  while ((option = getopt(argc, argv, "e:ivclnhsf:o")) != -1) {
    switch (option) {
      case 'e':
        flags->e = 1;
        if (optarg == NULL || strlen(optarg) == 0) {
          if (!flags->s) {
            fprintf(stderr, "s21_grep: option requires an argument -- e\n");
          }
          FreeGrepFlags(flags);
          return 0;
        }
        if (!AppendPattern(flags, optarg)) {
          FreeGrepFlags(flags);
          return 0;
        }
        break;
      case 'f':
        flags->f = 1;
        if (!ReadPatternsFromFile(optarg, flags, 0)) {
          if (!flags->s) {
            FreeGrepFlags(flags);
            return 0;
          }
        }
        break;
      default:
        SetFlag(option, flags);
        break;
    }
  }

  if (flags->patterns_count == 0 && optind < argc) {
    if (!AppendPattern(flags, argv[optind])) {
      FreeGrepFlags(flags);
      return 0;
    }
    optind++;
  }

  *file_index = optind;
  return (flags->patterns_count > 0);
}