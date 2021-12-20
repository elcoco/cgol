#include "matrix.h"
#include "assert.h"


int count_neighbours(Node* self) {
    // count alive neighbours
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

void toggle_node(Matrix* self, int clicked_x, int clicked_y, int pan_x, int pan_y) {
    /* toggle node state after mouseclick in viewport */

    // translate curses (0,0 is topleft) to vp coordinates (0,0 is in middle)
    int vp_x = clicked_x - (self->vp->size_x/2);
    int vp_y = clicked_y - (self->vp->size_y/2);

    // translate vp coordinates to matrix coordinates
    int m_x = vp_x + pan_x;
    int m_y = vp_y + pan_y;

    int loc = get_index(MATRIX_WIDTH, MATRIX_HEIGHT, m_x, m_y, EP_STOP);

    Node* n = *(self->nodes + loc);
    if (n->state) {
        n->state = false;
        self->remove_alive_node(self, n);

    } else {
        n->state = true;
        self->insert_alive_node(self, n);
    }
}

void clear_matrix(Matrix* self) {
    *(self->head) = NULL;
    Node** n = self->nodes;

    while (*n) {
        (*n)->state = false;
        (*n)->was_alive = false;
        n++;
    }
    self->alive_nodes = 0;
}

Node** init_nodes(Matrix* self) {
    /* Function is called by init_matrix() function to initialize nodes */
    Node** nodes = (Node**)malloc(((self->size_x*self->size_y)+1)*sizeof(Node*));
    self->nodes = nodes;

    // create nodes
    for (int i=0 ; i<(self->size_x*self->size_y) ; i++) {
        Node* n = (Node*)malloc(sizeof(Node));
        n->index = i;
        n->state = false;
        n->tmp_state = false;
        n->print = &print;
        n->count_neighbours = &count_neighbours;
        n->becomes_alive = &becomes_alive;
        n->was_alive = false;
        nodes[i] = n;
    }

    // terminate array
    nodes[(self->size_x*self->size_y)+1] = NULL;

    int i = 0;

    // this will truncate in the right way
    int xmin = 0 - (self->size_x / 2);
    int ymin = 0 - (self->size_y / 2);
    int xmax = self->size_x / 2;
    int ymax = self->size_y / 2;

    for (int y=ymin ; y<=ymax ; y++) {
        for (int x=xmin ; x<=xmax ; x++, i++) {
            Node* n = *(nodes+i);

            int loc_nw = get_index(self->size_x, self->size_y, x-1, y-1, self->edge_policy);
            int loc_n  = get_index(self->size_x, self->size_y,   x, y-1, self->edge_policy);
            int loc_ne = get_index(self->size_x, self->size_y, x+1, y-1, self->edge_policy);
                                                                               
            int loc_w  = get_index(self->size_x, self->size_y, x-1,   y,  self->edge_policy);
            int loc_e  = get_index(self->size_x, self->size_y, x+1,   y,  self->edge_policy);
                                                                               
            int loc_sw = get_index(self->size_x, self->size_y, x-1, y+1,  self->edge_policy);
            int loc_s  = get_index(self->size_x, self->size_y,   x, y+1,  self->edge_policy);
            int loc_se = get_index(self->size_x, self->size_y, x+1, y+1,  self->edge_policy);

            n->nw = (loc_nw < 0) ? NULL : *(nodes + loc_nw);
            n->n  = (loc_n  < 0) ? NULL : *(nodes + loc_n );
            n->ne = (loc_ne < 0) ? NULL : *(nodes + loc_ne);

            n->w  = (loc_w < 0) ? NULL : *(nodes + loc_w);
            n->e  = (loc_e < 0) ? NULL : *(nodes + loc_e);

            n->sw = (loc_sw < 0) ? NULL : *(nodes + loc_sw);
            n->s  = (loc_s  < 0) ? NULL : *(nodes + loc_s );
            n->se = (loc_se < 0) ? NULL : *(nodes + loc_se);

            //n->print(n);
            //printf("------------------\n");
        }
    }
    printf("finished creating %d nodes\n", i);

    return nodes;
}

void print_viewport(ViewPort* self) {
    /* Print out nodes as a matrix, not used anymore.
     * This is handled by ncurses */
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

int update_viewport(ViewPort* self, Matrix* m, int offset_x, int offset_y, int size_x, int size_y) {
    /* update viewport parameters */
    if (self->nodes != NULL) {
        if (realloc(self->nodes, ((size_x*size_y)+1)*sizeof(Node*)) == NULL)
            return -1;
    }
    else
        self->nodes = (Node**)malloc(((size_x*size_y)+1)*sizeof(Node*));

    self->size_x = size_x;
    self->size_y = size_y;
    self->offset_x = offset_x;
    self->offset_y = offset_y;

    Node** vp_n = self->nodes;

    // find top left corner coordinates
    self->start_x = offset_x - (size_x/2);
    self->start_y = offset_y - (size_y/2);

    // find bottom right corner coordinates
    self->end_x = self->start_x + self->size_x; // TODO -1?
    self->end_y = self->start_y + self->size_y;

    // Add nodes from matrix to self->nodes, check for oob
    for (int y=0 ; y<size_y ; y++) {

        // translate coordinates to nodes array index
        int loc = get_index(m->size_x, m->size_y, self->start_x, self->start_y+y, m->edge_policy); 
        //int loc = get_loc(m->size_x, m->size_y, 0, self->start_x, self->start_y+y, m->edge_policy); 

        // fill viewport nodes array with nodes
        for (int i=loc ; i<(loc+size_x) ; i++, vp_n++) {
            if (i < (m->size_x * m->size_y))
                *vp_n = *(m->nodes+i);
            else
                printf("OUTOFBOUNDS do something!!!!\n");
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

    // leave traces where alive cells have been
    vp->set_shade = false;

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

    n->was_alive = true;

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
    //printf("removing node: %d nb: %d\n", n->index, n->count_neighbours(n));
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
    m->toggle_node = &toggle_node;
    m->clear_matrix = &clear_matrix;
    return m;
}
