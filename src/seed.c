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
        if (len > self->seed_x) {
            self->seed_x = len;
        }

        *(self->data+y) = strndup(buf, strlen(buf)-1);
        y++;
        (self->seed_y)++;
    }

    fclose(fp);
    return 0;
}

void print_seed(Seed* self) {
    /* print seed for debugging */
    char** ptr = self->data;

    for (int i=0 ; i<self->seed_y ; i++, ptr++) {
        printf("[%d] %s\n", i, *ptr);
    }
}

int to_matrix(Seed* self, Matrix* m) {
    /* copy data from seed file into matrix */
    char** dptr = self->data;
    Node** nodes = m->nodes;

    // this will truncate in the right way
    int xmin = 0 - (self->seed_x / 2);
    int ymin = 0 - (self->seed_y / 2);
    int xmax = xmin + self->seed_x;
    int ymax = ymin + self->seed_y-1;

    int i = 0;

    for (int y=ymin ; y<=ymax ; y++, dptr++) {
        char* c = *dptr;

        //for (int x=xmin ; x<=(xmin + strlen(*dptr)) ; x++, c++) {
        for (int x=xmin ; x<=xmax ; x++, c++) {
            // only allowed char is O!
            if (*c != 'O')
                continue;
            i++;

            // Translate coordinates to index in nodes array
            int loc = get_index(self->term_x, self->term_y, x, y, m->edge_policy);
            printf("[%c]\t%3d x %3d\tloc=%d\n", *c, x, y, loc);
            Node* n = *(nodes+loc);
            n->state = 1;

            // add node to linked list, which is used for faster iteration inbetween generations
            m->insert_alive_node(m, n);

            //printf("\n");
            //n->print(n);
            //printf("\n");
        } 
    }
    printf("Finished putting seed in matrix, enabled %d nodes.\n", i);
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

    seed->seed_x = 0;
    seed->seed_y = 0;
    seed->term_x = term_x;
    seed->term_y = term_y;
    seed->data = (char**)malloc(DATA_SIZE*sizeof(char*));
    return seed;
}
