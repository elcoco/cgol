#ifndef UTILS_H
#define UTILS_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>

#include "matrix.h"

// forward declaring type from matrix.h
typedef enum EdgePolicy EdgePolicy;

typedef struct {
    char* seed_path;
    int speed_ms;
    bool set_random;
    bool set_wrapping;
    bool is_paused;
    bool is_stopped;
    int gen_counter;
    int pan_x;
    int pan_y;
    bool is_pan_changed;
    bool do_step;
    int term_x;
    int term_y;
    bool term_is_updated;
} State;

int get_loc(int xlim, int ylim, int cur_index, int xmov, int ymov, EdgePolicy edge_policy);
int parse_args(State* state, int argc, char** argv);
void print_usage();
int get_speed_incr(int speed, int incr);

#endif
