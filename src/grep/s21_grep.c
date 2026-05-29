#include <stdio.h>

#include "grep_parser.h"
#include "grep_process.h"

int main(int argc, char** argv) {
  grep_flags flags;
  int file_index = 0;

  if (!ParseArguments(argc, argv, &flags, &file_index)) {
    fprintf(stderr, "Usage: s21_grep [OPTIONS] PATTERN [FILE...]\n");
    return 2;
  }

  int status = RunGrep(&flags, argc, argv, file_index);
  FreeGrepFlags(&flags);
  return status;
}