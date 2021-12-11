#include "matrix.h"


int8_t set_point(Matrix *self, uint16_t x, uint16_t y, char* fgcol, int8_t state) {
    self->points[x][y].state = state;
    //self->points[x][y].chr = chr;
    self->points[x][y].fgcol = fgcol;
    return 0;
}

int8_t display(Matrix *self) {
    for (int16_t y=0 ; y<(self->ylen) ; y++) {

        for (int16_t x=0 ; x<(self->xlen) ; x++) {
            DataPoint *dp = &(self->points[x][y]);

            if (dp->state) {
                printf("%s%c%s", dp->fgcol, ON_CHR, RESET);
            } else {
                printf("%s%c%s", dp->fgcol, OFF_CHR, RESET);
            }
        }

        printf("\n");
    }
}

int8_t create_matrix(Matrix *m, uint16_t xlen, uint16_t ylen) {
    /* init matrix struct */
    m->points = (DataPoint**)malloc((xlen)*sizeof(DataPoint*));

    for (int x=0 ; x<(xlen) ; x++) {
        m->points[x] = (DataPoint*)malloc(ylen*sizeof(DataPoint));

        for (int y=0 ; y<(ylen) ; y++) {
            //m->points[x][y].chr = 0;
            m->points[x][y].state = 0;
            m->points[x][y].fgcol = DEFAULT;
        //printf("%i -> %p = %s\n", i, m->points[i]);
        }
    }

    m->xlen = xlen-1;
    m->ylen = ylen-1;
    m->set_point = &set_point;
    m->display = &display;

    return 0;
}
