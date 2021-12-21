#ifndef SEED_H
#define SEED_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "matrix.h"
#include "utils.h"

#define DATA_SIZE 2000

// from utils.h
//int get_loc(int xlim, int ylim, int cur_index, int xmov, int ymov);

typedef struct Seed Seed;

struct Seed {
    // max dimensions of seed
    int seed_x;
    int seed_y;

    // terminal dimensions
    int term_x;
    int term_y;

    // contains 2D array of chars in file
    char** data;
    
    int(*read_file)(Seed* self, char* path);
    int(*to_matrix)(Seed* self, Matrix* m);
    void(*print_seed)(Seed* self);
};

Seed* init_seed(int term_x, int term_y);


#endif
