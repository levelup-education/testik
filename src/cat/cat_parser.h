#ifndef CAT_PARSER_H
#define CAT_PARSER_H

typedef struct {
  int b;
  int e;
  int E;
  int n;
  int s;
  int t;
  int T;
  int v;
} CatFlags;

void parse_flags(int argc, char* argv[], CatFlags* flags);

#endif