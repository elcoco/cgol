#ifndef UTILS_H
#define UTILS_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>


typedef struct {
    char* seed_path;
    int speed_ms;
} Args;

int get_loc(int xlim, int ylim, int cur_index, int xmov, int ymov);
Args* parse_args(int argc, char** argv);
void print_usage();

#endif
