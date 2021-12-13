#ifndef MATRIX_H
#define MATRIX_H

#include <stdlib.h>
#include <stdio.h>

#include "utils.h"

#define ALIVE_CHR "█"
#define DEAD_CHR  "."

#define MATRIX_WIDTH  1000
#define MATRIX_HEIGHT 1000

// Node struct represents a cell in the matrix
typedef struct Node Node;
typedef enum   EdgePolicy EdgePolicy;
typedef struct ViewPort ViewPort;
typedef struct Matrix Matrix;

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
enum EdgePolicy {
    EP_WRAP,
    EP_STOP,
    EP_DISAPEAR
};

// Represents the nodes that are visible on screen
// Holds a subset of Matrix->nodes,
struct ViewPort {
    int max_x;
    int max_y;

    int origin_x;
    int origin_y;

    Node** nodes;
    void(*print_viewport)(ViewPort* self);
};

// Matrix wraps nodes and contains info about dimensions and stuff
struct Matrix {
    Node** nodes;
    int max_x;
    int max_y;
    EdgePolicy edge_policy;

    // data currently on screen
    ViewPort* vp;

    Node**(*init_nodes)(Matrix* m);
    ViewPort*(*get_viewport)(Matrix* m, int origin_x, int origin_y, int max_x, int max_y);
};

Matrix* init_matrix(int max_x, int max_y);

#endif
