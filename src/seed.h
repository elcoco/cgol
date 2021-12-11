#ifndef SEED_H
#define SEED_H

#include <stdint.h>         // uint types
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "node.h"

#define DATA_SIZE 2000

typedef struct Seed Seed;

struct Seed {
    // max dimensions of seed
    int max_x;
    int max_y;

    // offset of seed in matrix
    int x_offset;
    int y_offset;

    // terminal dimensions
    int term_x;
    int term_y;

    // contains 2D array of chars in file
    char** data;
    
    int(*read_file)(Seed* self, char* path);
    int(*to_matrix)(Seed* self, Node** nodes);
    void(*print_seed)(Seed* self);
};

Seed* init_seed(int term_x, int term_y);


#endif
