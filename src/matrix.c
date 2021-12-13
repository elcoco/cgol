#include "matrix.h"


int count_neighbours(Node* self) {
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
    /* print out nodes as a matrix */
    int c = 0;
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

Node** init_nodes(Matrix* self) {
    /* Function is called by init_matrix() function to initialize nodes */
    Node** nodes = (Node**)malloc(((self->max_x*self->max_y)+1)*sizeof(Node*));
    self->nodes = nodes;

    // create nodes
    for (int i=0 ; i<(self->max_x*self->max_y) ; i++) {
        Node* n = (Node*)malloc(sizeof(Node));
        n->index = i;
        n->print = &print;
        n->count_neighbours = &count_neighbours;
        nodes[i] = n;
    }

    // terminate array
    nodes[(self->max_x*self->max_y)+1] = NULL;

    // define nodes orientation in physical space
    // link all neighbours, do wrapping in get_loc()
    for (int i=0 ; i<(self->max_x*self->max_y) ; i++) {
        Node* n = *(nodes+i);
        n->nw = *(nodes + (get_loc(self->max_x, self->max_y, i, -1, -1, self->edge_policy))); 
        n->n  = *(nodes + (get_loc(self->max_x, self->max_y, i,  0, -1, self->edge_policy))); 
        n->ne = *(nodes + (get_loc(self->max_x, self->max_y, i,  1, -1, self->edge_policy))); 

        n->w  = *(nodes + (get_loc(self->max_x, self->max_y, i, -1, 0,  self->edge_policy))); 
        n->e  = *(nodes + (get_loc(self->max_x, self->max_y, i,  1, 0,  self->edge_policy))); 

        n->sw = *(nodes + (get_loc(self->max_x, self->max_y, i, -1, 1,  self->edge_policy))); 
        n->s  = *(nodes + (get_loc(self->max_x, self->max_y, i,  0, 1,  self->edge_policy))); 
        n->se = *(nodes + (get_loc(self->max_x, self->max_y, i,  1, 1,  self->edge_policy))); 
    }
    return nodes;
}

Matrix* init_matrix(int max_x, int max_y) {
    /* Setup matrix struct, this contains the nodes and data about matrix like size */
    Matrix* m = (Matrix*)malloc(sizeof(Matrix));
    m->max_x = max_x;
    m->max_y = max_y;
    m->edge_policy = EP_WRAP;  // default

    // connect func pointers
    m->print_matrix = &print_matrix;
    m->init_nodes = &init_nodes;
    return m;
}
