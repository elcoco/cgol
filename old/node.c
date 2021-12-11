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
    printf("%2d <- ", (self->nw) ? self->nw->index : -1);
    printf("%2d -> ", (self->n)  ? self->n->index  : -1);
    printf("%2d\n",   (self->ne) ? self->ne->index : -1);

    printf("%2d <- ", (self->w)  ? self->w->index  : -1);
    printf("%2d -> ",  self->index);
    printf("%2d\n",   (self->e)  ? self->e->index  : -1);

    printf("%2d <- ", (self->sw) ? self->sw->index : -1);
    printf("%2d -> ", (self->s)  ? self->s->index  : -1);
    printf("%2d\n",   (self->se) ? self->se->index : -1);
}

void set_state(Node* self, int8_t state) {
    //self->tmp_state = self->state;
    self->state = state;
}


int8_t display_nodes(Node *n) {
    // display full linked list
    while (n != NULL) {
        printf("%p <= %p => %p | %2d\n", n->prev, n, n->next, n->index);
        n = n->next;
    }
}

Node* get_node(Node* n, int32_t index) {
    /* Go n nodes back, return pointer */
    Node* tmp = n;

    // if index is smaller than current node index
    if (index < n->index) {
        for (int32_t i=n->index ; i>index ; i--) {
            tmp = tmp->prev;

            // if null pointer, we are at start of linked list
            if (!tmp) {
                return NULL;
            }
        }

    } else {
        for (uint32_t i=n->index ; i<index ; i++) {
            tmp = tmp->next;

            // if null pointer, we are at end of linked list
            if (!tmp) {
                return NULL;
            }
        }
    }

    //printf("start: %d, find: %d, found: %d\n", n->index, index, tmp->index);
    return tmp;
}

Node* get_next(Node* n, int32_t index) {
    /* Go n nodes forward, return pointer */
    Node* tmp = n;

    for (uint32_t i=n->index ; i<index ; i++) {

        // if null pointer, we are at end of linked list
        if (tmp == NULL) {
            printf("ERRROR\n");
            return NULL;
        }
        //printf("    %d\n", tmp->index);
        tmp = tmp->next;
    }

    printf("start: %d, find: %d, found: %d\n", n->index, index, tmp->index);
    return tmp;
}

void print_matrix(Node* rn, uint32_t xlim, uint32_t ylim) {
    /* print out linked list as a matrix */
    uint32_t c = 0;
    Node* n = rn;

    while (n) {
        if ((c % xlim) == 0)
            printf("\n");

        if (n->state)
            printf("x");
        else
            printf(".");

        n = n->next;
        c++;

    }
    printf("\n");
}

int8_t create_nodes(Node* n, uint32_t length, uint32_t index) {
    /* Recursive create all nodes in linked list */
    n->index = index;

    // first node has no prev
    if (index == 0) {
        n->prev = NULL;
    }

    // check if we're at end of list
    if (index == length-1) {
        printf("EOL @ %d\n", index);
        n->next = NULL;
        return 1;
    } 

    Node* next = (Node*)malloc(sizeof(Node));

    // connect nodes
    n->next = next;
    next->prev = n;
    create_nodes(next, length, index+1);
    return 1;
}

int32_t get_loc(uint32_t xlim, uint32_t ylim, int32_t cur_index, uint32_t xmov, uint32_t ymov) {
    // translate linked list index to physical matrix
    // apply movement, and translate back to linked list index

}

int8_t init_nodes(Node* rn, uint32_t xlim, uint32_t ylim) {
    create_nodes(rn, xlim*ylim, 0);

    Node *n = rn;

    while (n != NULL) {
        // get pointers to the neighbours in the matrix, check if out of boundary
        // if n->index ends up on the left side of matrix, all neighbours
        // at the left side are NULL because they don't exist
        n->nw = ((n->index % xlim) != 0)     ? get_node(n, n->index-xlim-1) : NULL;
        n->n  = get_node(n, n->index-xlim);
        n->ne = (((n->index+1) % xlim) != 0) ? get_node(n, n->index-xlim+1) : NULL;
        
        n->w = ((n->index % xlim) != 0)     ? get_node(n, n->index-1) : NULL;
        n->e = (((n->index+1) % xlim) != 0) ? get_node(n, n->index+1) : NULL;

        n->sw = ((n->index % xlim) != 0)     ? get_node(n, n->index+xlim-1) : NULL;
        n->s  = get_node(n, n->index+xlim);
        n->se = (((n->index+1) % xlim) != 0) ? get_node(n, n->index+xlim+1) : NULL;

        n->print = &print;
        n->count_neighbours = &count_neighbours;

        n = n->next;
    }
}
