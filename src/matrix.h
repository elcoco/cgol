#ifndef MATRIX_H
#define MATRIX_H

#include <stdlib.h>
#include <stdio.h>

#include "utils.h"

#define ALIVE_CHR "█"
#define DEAD_CHR  " "

// Node struct represents a cell in the matrix
typedef struct Node Node;
struct Node {
    // on or off state
    int state;

    // how many generations are we in this state, can be used to fancy coloring of cells
    int age;

    // All rules need to be applied at the same time on all nodes
    // So we need to make a backup of the state while itering over nodes
    int tmp_state;

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

    int(*count_neighbours)(Node* self); 
    void(*print)(Node* self);
};

// what to do when a patern moves beyond it's borders
typedef enum EdgePolicy EdgePolicy;
enum EdgePolicy {
    EP_WRAP,
    EP_STOP,
    EP_DISAPEAR
};

// Matrix wraps nodes and contains info about dimensions and stuff
typedef struct Matrix Matrix;
struct Matrix {
    Node** nodes;
    int max_x;
    int max_y;
    EdgePolicy edge_policy;

    void(*print_matrix)(Matrix* m);
    Node**(*init_nodes)(Matrix* m);
};

Matrix* init_matrix(int max_x, int max_y);

#endif
