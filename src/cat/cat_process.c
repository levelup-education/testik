#include "cat_process.h"

#include <errno.h>
#include <stdio.h>
#include <string.h>

static void print_line_number(int ch, int* line_number, int new_line,
                              CatFlags flags) {
  if (flags.b) {
    if (new_line && ch != '\n') {
      printf("%6d\t", *line_number);
      (*line_number)++;
    } else if (new_line && ch == '\n' && (flags.e || flags.E)) {
      printf("%6s\t", "");
    }
  } else if (flags.n) {
    if (new_line) {
      printf("%6d\t", *line_number);
      (*line_number)++;
    }
  }
}

static int squeeze_blank(int ch, int prev, int* empty_count, CatFlags flags) {
  int skip = 0;

  if (flags.s) {
    if (ch == '\n' && prev == '\n') {
      (*empty_count)++;

      if (*empty_count > 1) {
        skip = 1;
      }
    } else {
      *empty_count = 0;
    }
  }

  return skip;
}

void print_char(int ch, CatFlags flags) {
  int show_nonprinting = flags.v || flags.t || flags.e;

  if (show_nonprinting) {
    if (ch == '\t') {
      if (flags.t || flags.T) {
        printf("^I");
      } else {
        putchar(ch);
      }
    } else if (ch == '\n') {
      if (flags.e || flags.E) {
        putchar('$');
      }
      putchar('\n');
    } else if (ch >= 32 && ch < 127) {
      putchar(ch);
    } else if (ch == 127) {
      putchar('^');
      putchar('?');
    } else if (ch < 32) {
      putchar('^');
      putchar(ch + 64);
    } else if (ch >= 128) {
      printf("M-");
      if (ch >= 128 + 32 && ch < 128 + 127) {
        putchar(ch - 128);
      } else if (ch == 128 + 127) {
        putchar('^');
        putchar('?');
      } else {
        putchar('^');
        putchar(ch - 128 + 64);
      }
    }
  } else {
    if (ch == '\t' && flags.T) {
      printf("^I");
    } else if (ch == '\n' && flags.E) {
      putchar('$');
      putchar('\n');
    } else {
      putchar(ch);
    }
  }
}

void process_file(const char* filename, CatFlags flags) {
  FILE* fp = fopen(filename, "r");

  if (fp != NULL) {
    int ch = 0;
    int line_number = 1;
    int new_line = 1;
    int empty_lines = 0;
    int prev = '\n';

    while ((ch = fgetc(fp)) != EOF) {
      if (!squeeze_blank(ch, prev, &empty_lines, flags)) {
        print_line_number(ch, &line_number, new_line, flags);

        print_char(ch, flags);

        if (ch == '\n') {
          new_line = 1;
        } else {
          new_line = 0;
        }
      }

      prev = ch;
    }

    fclose(fp);
  } else {
    fprintf(stderr, "s21_cat: %s: %s\n", filename, strerror(errno));
  }
}