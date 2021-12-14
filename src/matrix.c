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

Node** init_nodes(Matrix* self) {
    /* Function is called by init_matrix() function to initialize nodes */
    Node** nodes = (Node**)malloc(((self->max_x*self->max_y)+1)*sizeof(Node*));
    self->nodes = nodes;

    // create nodes
    for (int i=0 ; i<(self->max_x*self->max_y) ; i++) {
        Node* n = (Node*)malloc(sizeof(Node));
        n->index = i;
        n->state = 0;
        n->tmp_state = 0;
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

void print_viewport(ViewPort* self) {
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

ViewPort* get_viewport(Matrix* self, int origin_x, int origin_y, int max_x, int max_y) {
    /* Viewport is the subset of data that is shown on screen.
     * This enables the possibility of panning and zooming in the future
     * origin is in middle of viewport
     */
    ViewPort* vp = (ViewPort*)malloc(sizeof(ViewPort));
    vp->max_x = max_x;
    vp->max_y = max_y;
    vp->origin_x = origin_x;
    vp->origin_y = origin_y;
    vp->nodes = (Node**)malloc(((max_x*max_y)+1)*sizeof(Node*));

    // connect func pointers
    vp->print_viewport = &print_viewport;

    Node** vp_n = vp->nodes;

    // find top left corner coordinates
    int start_x = origin_x - (max_x/2);
    int start_y = origin_y - (max_y/2);

    // Add nodes from matrix to vp->nodes, check for oob
    for (int y=0 ; y<max_y ; y++) {
        int loc = get_loc(self->max_x, self->max_y, 0, start_x, start_y+y, self->edge_policy); 

        // TODO this will probably be a problem when we enable panning since there is no solution for out of bound problem
        for (int i=loc ; i<(loc+max_x) ; i++, vp_n++) {
            if (i < (self->max_x * self->max_y))
                *vp_n = *(self->nodes+i);
            else
                printf("OUTOFBOUNDS do something!!!!\n");
        }
    }

    self->vp = vp;
    return self->vp;
}

void insert_alive_node(Matrix* self, Node* n) {
    /* Insert a node at head of linked list.
     * Head needs to be passed by double pointer to be able to replace it.
     */
    self->alive_nodes++;
    Node** head = self->head;

    if (!*head) {
        // First node becomes the head node
        *head = n;
        n->next = NULL;
        n->prev = NULL;

    } else {
        // Replace head node in linked list
        Node* next_node = *head;
        *head = n;
        next_node->prev = *head;
        (*head)->next = next_node;
    }
}

void print_linked_list(Node* n) {
    /* Print out full linked list for debugging */
    int c = 0;
    while (n) {
        printf("[%3d] %d %p <- [%p] -> %p\n", c, n->index, n->prev, n, n->next);
        c++;
        n = n->next;
    }
}

void remove_alive_node(Matrix* self, Node* n) {
    /* Remove a node from linked list and reconnect the list */
    self->alive_nodes--;

    Node* n_prev = n->prev;
    Node* n_next = n->next;
    Node** head = self->head;

    if (n_prev) {
        n_prev->next = n_next;
    } else {
        // is new head
        *head = n_next;
        (*head)->prev = NULL;
    }

    if (n_next)
        n_next->prev = n_prev;
    else
        // is tail node
        n_prev->next = NULL;

    // reset removed node links
    n->next = NULL;
    n->prev = NULL;
}

Matrix* init_matrix(int max_x, int max_y) {
    /* Setup matrix struct, this contains the nodes and data about matrix like size */
    Matrix* m = (Matrix*)malloc(sizeof(Matrix));
    m->max_x = max_x;
    m->max_y = max_y;
    m->edge_policy = EP_WRAP;  // default
    m->alive_nodes = 0;

    m->head = (Node**)malloc(sizeof(Node*));
    *(m->head) = NULL;

    // connect func pointers
    m->init_nodes = &init_nodes;
    m->get_viewport = &get_viewport;
    m->insert_alive_node = &insert_alive_node;
    m->remove_alive_node = &remove_alive_node;
    return m;
}
