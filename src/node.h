#ifndef NODE_H
#define NODE_H

#include <stdint.h>         // uint types
#include <stdlib.h>
#include <stdio.h>

#include "utils.h"

#define ALIVE_CHR "█"
#define DEAD_CHR  "."

// Node struct represents a cell in the matrix
typedef struct Node Node;
struct Node {
    // on or off state
    int8_t state;

    // All rules need to be applied at the same time on all nodes
    // So we need to make a backup of the state while itering over nodes
    int8_t tmp_state;

    // only for debugging
    int32_t index;

    // pointers to nodes in all directions in graph
    Node* n;
    Node* e;
    Node* s;
    Node* w;
    Node* ne;
    Node* nw;
    Node* se;
    Node* sw;

    int8_t(*count_neighbours)(Node* self); 
    void(*print)(Node* self);
};

Node** init_nodes(int xlim, int ylim);
void print_matrix(Node** nodes, uint32_t xlim, uint32_t ylim);

#endif
