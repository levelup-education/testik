#include "cat_parser.h"

static void set_flag(char flag, CatFlags* flags) {
  if (flag == 'b') {
    flags->b = 1;

  } else if (flag == 'e') {
    flags->e = 1;
    flags->v = 1;

  } else if (flag == 'E') {
    flags->E = 1;

  } else if (flag == 'n') {
    flags->n = 1;

  } else if (flag == 's') {
    flags->s = 1;

  } else if (flag == 't') {
    flags->t = 1;
    flags->v = 1;

  } else if (flag == 'T') {
    flags->T = 1;

  } else if (flag == 'v') {  // <-- ДОБАВИТЬ!
    flags->v = 1;
  }
}

void parse_flags(int argc, char* argv[], CatFlags* flags) {
  int i = 1;

  while (i < argc) {
    if (argv[i][0] == '-') {
      int j = 1;

      while (argv[i][j] != '\0') {
        set_flag(argv[i][j], flags);
        j++;
      }
    }

    i++;
  }
}