#include "seed.h"


int get_dimensions(Seed* self) {
}

int read_file(Seed* self, char* path) {
    /* Read file contents into seed struct */

    FILE *fp = fopen(path, "r");
    char c;

    // count seed dimensions, used for calculating matrix offset
    char buf[5000];
    int y = 0;

    while (fgets(buf, 5000, fp) != NULL ) {

        // skip comments
        if (buf[0] == '!') {
            continue;
        }

        // count max x stringlen and record in struct
        int len = strlen(buf)-2;
        if (len > self->max_x) {
            self->max_x = len;
        }

        *(self->data+y) = strndup(buf, strlen(buf)-1);
        y++;
        (self->max_y)++;
    }
    printf("maxy: %d\n", self->max_y);
    printf("maxx: %d\n", self->max_x);
}

int to_matrix(Seed* self) {
}

Seed* init_seed() {
    /* Create seed struct and connect all the func pointers */
    Seed* seed;
    seed->get_dimensions = &get_dimensions;
    seed->read_file = &read_file;
    seed->to_matrix = &to_matrix;

    seed->max_x = 0;
    seed->max_y = 0;
    seed->data = (char**)malloc(DATA_SIZE*sizeof(char*));
}
