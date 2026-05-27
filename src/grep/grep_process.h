#ifndef GREP_PROCESS_H
#define GREP_PROCESS_H

#include "grep_parser.h"

int RunGrep(grep_flags *flags, int argc, char **argv, int file_index);

#endif