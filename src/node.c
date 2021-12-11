#include "node.h"


int8_t count_neighbours(Node* self) {
    // count active neighbours
    self->neighbours = 0;
    if (self->nw && self->nw->state)
        self->neighbours++;
    if (self->n && self->n->state)
        self->neighbours++;
    if (self->ne && self->ne->state)
        self->neighbours++;
    if (self->w && self->w->state)
        self->neighbours++;
    if (self->e && self->e->state)
        self->neighbours++;
    if (self->sw && self->sw->state)
        self->neighbours++;
    if (self->s && self->s->state)
        self->neighbours++;
    if (self->se && self->se->state)
        self->neighbours++;

    return self->neighbours;
}

void print(Node* self) {
    printf("in func: %d\n", self->index);
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

void print_matrix(Node** nodes, uint32_t xlim, uint32_t ylim) {
    /* print out linked list as a matrix */
    uint32_t c = 0;
    Node** n = nodes;

    while (*n) {
        if ((c % xlim) == 0)
            printf("\n");

        if ((*n)->state)
            printf("█");
        else
            printf(".");

        n++;
        c++;
    }
    printf("\n");
}

Node** init_nodes(int xlim, int ylim) {
    //create_nodes(rn, xlim*ylim, 0);
    Node** nodes = (Node**)malloc(((xlim*ylim)+1)*sizeof(Node*));

    // create nodes
    for (int i=0 ; i<(xlim*ylim) ; i++) {
        Node* n = (Node*)malloc(sizeof(Node));
        n->index = i;
        n->print = &print;
        n->count_neighbours = &count_neighbours;
        nodes[i] = n;
    }

    // terminate array
    nodes[(xlim*ylim)+1] = NULL;

    // define nodes orientation in physical space
    // link all neighbours, do wrapping in get_loc()
    for (int i=0 ; i<(xlim*ylim) ; i++) {
        Node* n = *(nodes+i);
        n->nw = *(nodes + (get_loc(xlim, ylim, i, -1, -1))); 
        n->n  = *(nodes + (get_loc(xlim, ylim, i,  0, -1))); 
        n->ne = *(nodes + (get_loc(xlim, ylim, i,  1, -1))); 

        n->w  = *(nodes + (get_loc(xlim, ylim, i, -1, 0))); 
        n->e  = *(nodes + (get_loc(xlim, ylim, i,  1, 0))); 

        n->sw = *(nodes + (get_loc(xlim, ylim, i, -1, 1))); 
        n->s  = *(nodes + (get_loc(xlim, ylim, i,  0, 1))); 
        n->se = *(nodes + (get_loc(xlim, ylim, i,  1, 1))); 

        //int loc = get_loc(xlim, ylim, i, 1, 1);
        //printf("search: %d, found %d (%d:%d)\n", i, n->nw->index, xlim, ylim);
    }

    return nodes;
}
