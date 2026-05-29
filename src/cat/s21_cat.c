#include "cat_parser.h"
#include "cat_process.h"

int main(int argc, char* argv[]) {
  CatFlags flags = {0};

  parse_flags(argc, argv, &flags);

  int i = 1;

  while (i < argc) {
    if (argv[i][0] != '-') {
      process_file(argv[i], flags);
    }

    i++;
  }

  return 0;
}