#ifndef MATRIX_H
#define MATRIX_H

#include <stdint.h>         // uint types
#include <stdio.h>
#include <stdlib.h>
//#include <unistd.h>
//

#define BLK "\e[0;30m"
#define RED "\e[0;31m"
#define GRN "\e[0;32m"
#define YEL "\e[0;33m"
#define BLU "\e[0;34m"
#define MAG "\e[0;35m"
#define CYN "\e[0;36m"
#define WHT "\e[0;37m"
#define DEFAULT WHT
#define RESET "\e[0m"

#define OFF_CHR '.'
#define ON_CHR 'x'

typedef struct Matrix Matrix;

typedef struct DataPoint {
    char chr;
    int8_t state;
    char* fgcol;
    char* bgcol;
} DataPoint;


int8_t set_point(Matrix *self, uint16_t x, uint16_t y, char* fgcol, int8_t state);
int8_t display(Matrix *self);
int8_t create_matrix(Matrix *m, uint16_t xlen, uint16_t ylen);

struct Matrix {
    DataPoint** points;
    uint16_t xlen;
    uint16_t ylen;
    int8_t(*set_point)(Matrix *self, uint16_t x, uint16_t y, char* fgcol, int8_t state);
    int8_t(*display)(Matrix *self);
};



#endif
