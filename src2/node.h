#ifndef NODE_H
#define NODE_H

#include <stdint.h>         // uint types
#include <stdlib.h>
#include <stdio.h>


typedef struct Node Node;
struct Node {
    // on or off state
    int8_t state;

    // All rules need to be applied at the same time on all nodes
    // So we nee to make a backup of the state while itering over linked list
    int8_t tmp_state;

    int32_t index;

    // amount of neighbours
    uint8_t neighbours;

    // coordinates in matrix
    int32_t x;
    int32_t y;

    // pointer to prev/next in linked list
    Node* next;
    Node* prev;

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
    void(*set_state)(Node* self, int8_t state);
};

int8_t init_nodes(Node* rn, uint32_t xlim, uint32_t ylim);
void print_matrix(Node* rn, uint32_t xlim, uint32_t ylim);
int8_t create_nodes(Node* prev, uint32_t length, uint32_t index);
int8_t display_nodes(Node *n);
Node* get_node(Node* n, int32_t index);

#endif
