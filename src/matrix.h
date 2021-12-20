#ifndef MATRIX_H
#define MATRIX_H

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>

#include "utils.h"

#define ALIVE_CHR "█"
#define DEAD_CHR  "."

// data dimensions must have a middle point
#define MATRIX_WIDTH  500
#define MATRIX_HEIGHT 500


// Node struct represents a cell in the matrix
typedef struct Node Node;
typedef enum   EdgePolicy EdgePolicy;
typedef struct ViewPort ViewPort;
typedef struct Matrix Matrix;

struct Node {
    // on or off state
    bool state;

    // All rules need to be applied at the same time on all nodes
    // So we need to make a backup of the state while itering over nodes
    bool tmp_state;

    // Link to next node in linked list that only contains alive nodes.
    Node* next;
    Node* prev;

    // if node was alive, leave a shade
    bool was_alive;

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
    int(*becomes_alive)(Node* self); 
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
    int size_x;
    int size_y;

    int offset_x;
    int offset_y;

    // top left coordinates
    int start_x;
    int start_y;

    // bottom right coordinates
    int end_x;
    int end_y;

    bool set_shade;

    Node** nodes;
    void(*print_viewport)(ViewPort* self);  // not used anymore but still here
    void(*free_viewport)(ViewPort* self);
    int(*update_viewport)(ViewPort* vp, Matrix* m, int offset_x, int offset_y, int size_x, int size_y);
};

// Matrix wraps nodes and contains info about dimensions and stuff
struct Matrix {
    Node** nodes;

    // linked list that contain only alive nodes, used for fast itering of cells between generations
    Node** head;

    int alive_nodes;

    int size_x;
    int size_y;
    EdgePolicy edge_policy;

    // data currently on screen
    ViewPort* vp;

    Node**(*init_nodes)(Matrix* m);
    ViewPort*(*init_viewport)(Matrix* m);
    void(*insert_alive_node)(Matrix* self, Node* node);
    void(*remove_alive_node)(Matrix* self, Node* node);
};

Matrix* init_matrix(int size_x, int size_y);
void print_linked_list(Node* n);
void print_viewport(ViewPort* self);

#endif
