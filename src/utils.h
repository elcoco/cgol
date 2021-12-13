#ifndef UTILS_H
#define UTILS_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "node.h"

// forward declaring type from node.h
typedef enum EdgePolicy EdgePolicy;

typedef struct {
    char* seed_path;
    int speed_ms;
    int set_random;
    int set_wrapping;
    int is_paused;
    int is_stopped;
} State;

int get_loc(int xlim, int ylim, int cur_index, int xmov, int ymov, EdgePolicy edge_policy);
int parse_args(State* state, int argc, char** argv);
void print_usage();

#endif
