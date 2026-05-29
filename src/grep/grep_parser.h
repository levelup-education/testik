#ifndef GREP_PARSER_H
#define GREP_PARSER_H

#include <stddef.h>  // для size_t

typedef struct {
  int e;
  int i;
  int v;
  int c;
  int l;
  int n;
  int h;
  int s;
  int f;
  int o;

  char** patterns;
  int patterns_count;
  int patterns_capacity;
} grep_flags;

int ParseArguments(int argc, char** argv, grep_flags* flags, int* file_index);
void FreeGrepFlags(grep_flags* flags);

#endif