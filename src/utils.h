#ifndef UTILS_H
#define UTILS_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "matrix.h"

// forward declaring type from matrix.h
typedef enum EdgePolicy EdgePolicy;

typedef struct {
    char* seed_path;
    int speed_ms;
    int set_random;
    int set_wrapping;
    int is_paused;
    int is_stopped;
    int gen_counter;
    int pan_x;
    int pan_y;
    int is_pan_changed;
} State;

int get_loc(int xlim, int ylim, int cur_index, int xmov, int ymov, EdgePolicy edge_policy);
int parse_args(State* state, int argc, char** argv);
void print_usage();
int get_speed_incr(int speed, int incr);

#endif
