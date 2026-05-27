#include <stdio.h>

#include "cat_process.h"

static void print_line_number(
    int ch,
    int *line_number,
    int new_line,
    CatFlags flags
) {
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

static int squeeze_blank(
    int ch,
    int prev,
    int *empty_count,
    CatFlags flags
) {
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

static void print_char(int ch, CatFlags flags) {
    if ((flags.t || flags.T) && ch == '\t') {
        printf("^I");

    } else if ((flags.e || flags.E) && ch == '\n') {
        printf("$\n");

    } else if (
        flags.v &&
        ch < 32 &&
        ch != '\n' &&
        ch != '\t'
    ) {
        printf("^%c", ch + 64);

    } else {
        putchar(ch);
    }
}

void process_file(const char *filename, CatFlags flags) {
    FILE *fp = fopen(filename, "r");

    if (fp != NULL) {
        int ch = 0;
        int line_number = 1;
        int new_line = 1;
        int empty_lines = 0;
        int prev = '\n';

        while ((ch = fgetc(fp)) != EOF) {

            if (!squeeze_blank(ch, prev, &empty_lines, flags)) {

                print_line_number(
                    ch,
                    &line_number,
                    new_line,
                    flags);

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
    }
}