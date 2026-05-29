#include "grep_process.h"

#include <errno.h>
#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int CompilePatterns(const grep_flags* flags, regex_t** compiled) {
  int count = flags->patterns_count;
  regex_t* regexes = malloc(sizeof(regex_t) * count);
  if (regexes == NULL) {
    return 0;
  }

  int regex_flags = REG_EXTENDED | REG_NEWLINE;
  if (flags->i) {
    regex_flags |= REG_ICASE;
  }

  for (int i = 0; i < count; i++) {
    int ret = regcomp(&regexes[i], flags->patterns[i], regex_flags);
    if (ret != 0) {
      char errbuf[256];
      regerror(ret, &regexes[i], errbuf, sizeof(errbuf));
      fprintf(stderr, "s21_grep: regex error: %s\n", errbuf);
      for (int j = 0; j < i; j++) {
        regfree(&regexes[j]);
      }
      free(regexes);
      return 0;
    }
  }

  *compiled = regexes;
  return 1;
}

static void FreeCompiledPatterns(regex_t* compiled, int count) {
  if (compiled == NULL) {
    return;
  }

  for (int i = 0; i < count; i++) {
    regfree(&compiled[i]);
  }
  free(compiled);
}

static void PrintLinePrefix(const char* filename, int line_number,
                            const grep_flags* flags, int show_filename) {
  if (show_filename) {
    printf("%s:", filename);
  }
  if (flags->n) {
    printf("%d:", line_number);
  }
}

static void PrintLine(const char* filename, int line_number,
                      const grep_flags* flags, int show_filename,
                      const char* line) {
  PrintLinePrefix(filename, line_number, flags, show_filename);
  fputs(line, stdout);
}

static void PrintMatchesForPattern(const char* filename, int line_number,
                                   const grep_flags* flags, int show_filename,
                                   const regex_t* regex, const char* line) {
  const char* current = line;
  regmatch_t match;

  while (regexec(regex, current, 1, &match,
                 (current == line) ? 0 : REG_NOTBOL) == 0) {
    if (show_filename || flags->n) {
      PrintLinePrefix(filename, line_number, flags, show_filename);
    }
    if (match.rm_so >= 0 && match.rm_eo >= match.rm_so) {
      fwrite(current + match.rm_so, 1, (size_t)(match.rm_eo - match.rm_so),
             stdout);
    }
    putchar('\n');
    if (match.rm_eo == match.rm_so) {
      current++;
      if (*current == '\0') {
        break;
      }
    } else {
      current += match.rm_eo;
    }
  }
}

static void PrintMatches(const char* filename, int line_number,
                         const grep_flags* flags, int show_filename,
                         const regex_t* compiled, int compiled_count,
                         const char* line) {
  for (int i = 0; i < compiled_count; i++) {
    PrintMatchesForPattern(filename, line_number, flags, show_filename,
                           &compiled[i], line);
  }
}

static int MatchAnyPattern(const regex_t* compiled, int compiled_count,
                           const char* line) {
  for (int i = 0; i < compiled_count; i++) {
    if (regexec(&compiled[i], line, 0, NULL, 0) == 0) {
      return 1;
    }
  }
  return 0;
}

static int ProcessStream(FILE* stream, const char* filename,
                         const grep_flags* flags, const regex_t* compiled,
                         int compiled_count, int show_filename,
                         int* matched_lines) {
  char* line = NULL;
  size_t length = 0;
  ssize_t read = 0;
  int line_number = 0;
  int has_match = 0;

  if (flags->v && flags->o) {
    return 0;
  }

  while ((read = getline(&line, &length, stream)) != -1) {
    line_number++;
    int matched = MatchAnyPattern(compiled, compiled_count, line);
    if (flags->v) {
      matched = !matched;
    }

    if (!matched) {
      continue;
    }

    has_match = 1;
    if (flags->l || flags->c) {
      (*matched_lines)++;
      continue;
    }

    if (flags->o && !flags->v) {
      PrintMatches(filename, line_number, flags, show_filename, compiled,
                   compiled_count, line);
    } else {
      PrintLine(filename, line_number, flags, show_filename, line);
    }
  }

  free(line);
  return has_match;
}

static int ProcessFile(const char* filename, const grep_flags* flags,
                       const regex_t* compiled, int compiled_count,
                       int show_filename, int* any_match) {
  FILE* fp = NULL;
  int matched_lines = 0;

  if (filename == NULL) {
    fp = stdin;
  } else {
    fp = fopen(filename, "r");
    if (fp == NULL) {
      if (!flags->s) {
        fprintf(stderr, "s21_grep: %s: %s\n", filename, strerror(errno));
      }
      return 0;
    }
  }

  int has_match =
      ProcessStream(fp, filename == NULL ? "" : filename, flags, compiled,
                    compiled_count, show_filename, &matched_lines);

  if (fp != NULL && filename != NULL) {
    fclose(fp);
  }

  if (flags->l) {
    if (has_match) {
      printf("%s\n", filename);
      *any_match = 1;
      return 1;
    }
    return 0;
  }

  if (flags->c) {
    if (show_filename) {
      printf("%s:%d\n", filename, matched_lines);
    } else {
      printf("%d\n", matched_lines);
    }
    if (matched_lines > 0) {
      *any_match = 1;
    }
    return (matched_lines > 0);
  }

  if (has_match) {
    *any_match = 1;
  }

  return has_match;
}

int RunGrep(grep_flags* flags, int argc, char** argv, int file_index) {
  regex_t* compiled = NULL;
  if (!CompilePatterns(flags, &compiled)) {
    return 2;
  }

  int file_count = argc - file_index;
  int show_filename = (file_count > 1 && !flags->h) ? 1 : 0;
  int any_match = 0;
  int status = 1;

  if (file_count == 0) {
    if (ProcessFile(NULL, flags, compiled, flags->patterns_count,
                    /* show_filename */ 0, &any_match)) {
      status = 0;
    }
  } else {
    for (int i = file_index; i < argc; i++) {
      if (ProcessFile(argv[i], flags, compiled, flags->patterns_count,
                      show_filename, &any_match)) {
        status = 0;
      }
    }
  }

  FreeCompiledPatterns(compiled, flags->patterns_count);
  return status;
}
