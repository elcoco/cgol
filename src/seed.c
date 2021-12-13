#include "seed.h"


int read_file(Seed* self, char* path) {
    /* Read file contents into seed struct.
     * Count seed dimensions while we're at it. */

    char buf[5000];
    int y = 0;
    FILE *fp = fopen(path, "r");

    if (fp == NULL) {
        printf("File not found!\n");
        return -1;
    }

    while (fgets(buf, 5000, fp) != NULL ) {

        // skip comments
        if (buf[0] == '!') {
            continue;
        }

        // count max x stringlen (used for calculating matrix offset) and record in struct
        int len = strlen(buf)-2; // why 2?, \n is just one char ffs!
        if (len > self->max_x) {
            self->max_x = len;
        }

        *(self->data+y) = strndup(buf, strlen(buf)-1);
        y++;
        (self->max_y)++;
    }

    self->x_offset = (self->term_x-self->max_x) /2;
    self->y_offset = (self->term_y-self->max_y) /2;

    fclose(fp);
    return 0;
}

void print_seed(Seed* self) {
    /* print seed for debugging */
    char** ptr = self->data;

    for (int i=0 ; i<self->max_y ; i++, ptr++) {
        printf("[%d] %s\n", i, *ptr);
    }
}

int to_matrix(Seed* self, Matrix* m) {
    /* copy data from seed file into matrix */
    char** dptr = self->data;
    Node** nodes = m->nodes;

    for (int y=0 ; y<self->max_y ; y++, dptr++) {
        char* c = *dptr;

        for (int x=0 ; x<strlen(*dptr) ; x++, c++) {
            // only allowed char is O!
            if (*c != 'O')
                continue;

            // Translate terminal location to index in nodes array
            int loc = get_loc(self->term_x, self->term_y, 0, x+self->x_offset, y+self->y_offset, m->edge_policy);
            printf("[%c] x=%d, y=%d, loc=%d\n", *c, x, y, loc);
            Node* n = *(nodes+loc);
            n->state = 1;
        } 
    }
    return 0;
}

int get_rand(int lower, int upper) {
    return (rand() % (upper - lower + 1)) + lower;
}

void read_random(Seed* self) {
    /*
    char** c = self->data;
    
    for (int i=0 ; i<(self->term_x*self->term_y) ; i++, c++) {
        c = (get_rand(0, 1)) ? 'O' : '.';
        //printf("bver %c\n", *c);
    }
    */

    
}

Seed* init_seed(int term_x, int term_y) {
    /* Create seed struct and connect all the func pointers */
    Seed* seed = (Seed*)malloc(sizeof(Seed));

    seed->read_file = &read_file;
    seed->to_matrix = &to_matrix;
    seed->print_seed = &print_seed;
    seed->read_random = &read_random;

    seed->max_x = 0;
    seed->max_y = 0;
    seed->x_offset = 0;
    seed->y_offset = 0;
    seed->term_x = term_x;
    seed->term_y = term_y;
    seed->data = (char**)malloc(DATA_SIZE*sizeof(char*));
    return seed;
}
