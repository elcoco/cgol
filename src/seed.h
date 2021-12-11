#ifndef SEED_H
#define SEED_H

#include <stdint.h>         // uint types
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define DATA_SIZE 2000

typedef struct Seed Seed;

struct Seed {
    // max dimensions of seed
    int max_x;
    int max_y;

    // offset of seed in matrix
    int x_offset;
    int y_offset;

    // contains 2D array of chars in file
    char** data;
    
    int(*read_file)(Seed* self, char* path);
    int(*get_dimensions)(Seed* self);
    int(*to_matrix)(Seed* self);
};

Seed* init_seed();


#endif
