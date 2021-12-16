#include "matrix.h"
#include "assert.h"


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

int becomes_alive(Node *self) {
    /* Returns true if cell should become alive
     * Check all dead cells around this cell.
     * If more than 3 alive neighbours -> a new cell is born.
     */
    if (self->state || self->tmp_state)
        return 0;

    return self->count_neighbours(self) == 3;
}

Node** init_nodes(Matrix* self) {
    /* Function is called by init_matrix() function to initialize nodes */
    Node** nodes = (Node**)malloc(((self->size_x*self->size_y)+1)*sizeof(Node*));
    self->nodes = nodes;

    // create nodes
    for (int i=0 ; i<(self->size_x*self->size_y) ; i++) {
        Node* n = (Node*)malloc(sizeof(Node));
        n->index = i;
        n->state = 0;
        n->tmp_state = 0;
        n->print = &print;
        n->count_neighbours = &count_neighbours;
        n->becomes_alive = &becomes_alive;
        n->age = 0;
        n->was_alive = false;
        nodes[i] = n;
    }

    // terminate array
    nodes[(self->size_x*self->size_y)+1] = NULL;

    // define nodes orientation in physical space
    // link all neighbours, do wrapping in get_loc()
    for (int i=0 ; i<(self->size_x*self->size_y) ; i++) {
        Node* n = *(nodes+i);
        n->nw = *(nodes + (get_loc(self->size_x, self->size_y, i, -1, -1, self->edge_policy))); 
        n->n  = *(nodes + (get_loc(self->size_x, self->size_y, i,  0, -1, self->edge_policy))); 
        n->ne = *(nodes + (get_loc(self->size_x, self->size_y, i,  1, -1, self->edge_policy))); 

        n->w  = *(nodes + (get_loc(self->size_x, self->size_y, i, -1, 0,  self->edge_policy))); 
        n->e  = *(nodes + (get_loc(self->size_x, self->size_y, i,  1, 0,  self->edge_policy))); 

        n->sw = *(nodes + (get_loc(self->size_x, self->size_y, i, -1, 1,  self->edge_policy))); 
        n->s  = *(nodes + (get_loc(self->size_x, self->size_y, i,  0, 1,  self->edge_policy))); 
        n->se = *(nodes + (get_loc(self->size_x, self->size_y, i,  1, 1,  self->edge_policy))); 
    }
    return nodes;
}

void print_viewport(ViewPort* self) {
    /* print out nodes as a matrix */
    int c = 0;
    Node** n = self->nodes;

    while (*n) {
        if ((c % self->size_x) == 0)
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

int update_viewport(ViewPort* self, Matrix* m, int origin_x, int origin_y, int size_x, int size_y) {
    /* update viewport parameters */
    if (self->nodes != NULL) {
        if (realloc(self->nodes, ((size_x*size_y)+1)*sizeof(Node*)) == NULL)
            return -1;
    }
    else
        self->nodes = (Node**)malloc(((size_x*size_y)+1)*sizeof(Node*));

    self->size_x = size_x;
    self->size_y = size_y;
    self->origin_x = origin_x;
    self->origin_y = origin_y;

    Node** vp_n = self->nodes;

    // find top left corner coordinates
    int start_x = origin_x - (size_x/2);
    int start_y = origin_y - (size_y/2);

    // Add nodes from matrix to self->nodes, check for oob
    for (int y=0 ; y<size_y ; y++) {
        int loc = get_loc(m->size_x, m->size_y, 0, start_x, start_y+y, m->edge_policy); 

        // TODO this will probably be a problem when we enable panning since there is no solution for out of bound problem
        for (int i=loc ; i<(loc+size_x) ; i++, vp_n++) {
            if (i < (m->size_x * m->size_y))
                *vp_n = *(m->nodes+i);
            //else
            //    printw("OUTOFBOUNDS do something!!!!\n");
        }
    }
    return 1;
}

ViewPort* init_viewport(Matrix* self) {
    /* Viewport is the subset of data that is shown on screen.
     * This enables the possibility of panning and zooming in the future
     * origin is in middle of viewport
     */
    ViewPort* vp = (ViewPort*)malloc(sizeof(ViewPort));

    // connect func pointers
    vp->print_viewport = &print_viewport;
    vp->update_viewport = &update_viewport;

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

    if (self->alive_nodes <= 0)
        return;

    //assert(self->alive_nodes != 0);
    //assert(n);

    Node* n_prev = n->prev;
    Node* n_next = n->next;
    Node** head = self->head;

    if (n_prev != NULL) {
        n_prev->next = n_next;
    } else {
        *head = n_next;     // is new head
        (*head)->prev = NULL;
    }

    if (n_next != NULL)
        n_next->prev = n_prev;
    else
        n_prev->next = NULL; // is tail node

    // reset removed node links
    n->next = NULL;
    n->prev = NULL;
}

Matrix* init_matrix(int size_x, int size_y) {
    /* Setup matrix struct, this contains the nodes and data about matrix like size */
    Matrix* m = (Matrix*)malloc(sizeof(Matrix));
    m->size_x = size_x;
    m->size_y = size_y;
    m->edge_policy = EP_WRAP;  // default
    m->alive_nodes = 0;

    m->head = (Node**)malloc(sizeof(Node*));
    *(m->head) = NULL;

    // connect func pointers
    m->init_nodes = &init_nodes;
    m->init_viewport = &init_viewport;
    m->insert_alive_node = &insert_alive_node;
    m->remove_alive_node = &remove_alive_node;
    return m;
}
