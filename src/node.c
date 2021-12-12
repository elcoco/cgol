#include "node.h"


int8_t count_neighbours(Node* self) {
    // count active neighbours
    int neighbours = 0;
    if (self->nw && self->nw->state)
        neighbours++;
    if (self->n && self->n->state)
        neighbours++;
    if (self->ne && self->ne->state)
        neighbours++;
    if (self->w && self->w->state)
        neighbours++;
    if (self->e && self->e->state)
        neighbours++;
    if (self->sw && self->sw->state)
        neighbours++;
    if (self->s && self->s->state)
        neighbours++;
    if (self->se && self->se->state)
        neighbours++;

    return neighbours;
}

void print(Node* self) {
    /* for debugging */
    printf("%2d <- ", self->nw->index);
    printf("%2d -> ", self->n->index);
    printf("%2d\n",   self->ne->index);

    printf("%2d <- ", self->w->index);
    printf("%2d -> ", self->index);
    printf("%2d\n",   self->e->index);

    printf("%2d <- ", self->sw->index);
    printf("%2d -> ", self->s->index);
    printf("%2d\n",   self->se->index);
}

void print_matrix(Matrix* self) {
    /* print out linked list as a matrix */
    uint32_t c = 0;
    Node** n = self->nodes;

    while (*n) {
        if ((c % self->max_x) == 0)
            printf("\n");

        if ((*n)->state)
            printf("%s", ALIVE_CHR);
        else
            printf("%s", DEAD_CHR);

        n++;
        c++;
    }
    printf("\n");
}

Node** init_nodes(int max_x, int max_y) {
    //create_nodes(rn, max_x*max_y, 0);
    Node** nodes = (Node**)malloc(((max_x*max_y)+1)*sizeof(Node*));

    // create nodes
    for (int i=0 ; i<(max_x*max_y) ; i++) {
        Node* n = (Node*)malloc(sizeof(Node));
        n->index = i;
        n->print = &print;
        n->count_neighbours = &count_neighbours;
        nodes[i] = n;
    }

    // terminate array
    nodes[(max_x*max_y)+1] = NULL;

    // define nodes orientation in physical space
    // link all neighbours, do wrapping in get_loc()
    for (int i=0 ; i<(max_x*max_y) ; i++) {
        Node* n = *(nodes+i);
        n->nw = *(nodes + (get_loc(max_x, max_y, i, -1, -1))); 
        n->n  = *(nodes + (get_loc(max_x, max_y, i,  0, -1))); 
        n->ne = *(nodes + (get_loc(max_x, max_y, i,  1, -1))); 

        n->w  = *(nodes + (get_loc(max_x, max_y, i, -1, 0))); 
        n->e  = *(nodes + (get_loc(max_x, max_y, i,  1, 0))); 

        n->sw = *(nodes + (get_loc(max_x, max_y, i, -1, 1))); 
        n->s  = *(nodes + (get_loc(max_x, max_y, i,  0, 1))); 
        n->se = *(nodes + (get_loc(max_x, max_y, i,  1, 1))); 

        //int loc = get_loc(max_x, max_y, i, 1, 1);
        //printf("search: %d, found %d (%d:%d)\n", i, n->nw->index, max_x, max_y);
    }

    return nodes;
}

Matrix* init_matrix(int max_x, int max_y) {
    Matrix* m = (Matrix*)malloc(sizeof(Matrix));
    m->nodes = init_nodes(max_x, max_y);
    m->max_x = max_x;
    m->max_y = max_y;
    m->print_matrix = print_matrix;
    return m;
}
