#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>         // uint types

#include <sys/ioctl.h>
#include <unistd.h>

#include "node.h"

#define X_LEN 60
#define Y_LEN 30

#define INTERVAL 200000



void do_tick(Node* rn) {
    Node* n = rn;

    // check dead cells
    while (n) {
        n->tmp_state = 0;

        // if dead cell has 3 neighbours -> alive
        if (!n->state && n->count_neighbours(n) == 3) {
            n->tmp_state = 1;
        }

        // if alive cell has 2>cell>3 neighours, stay alive
        if (n->state && n->count_neighbours(n) >= 2 && n->count_neighbours(n) <= 3) {

            n->tmp_state = 1;
        }

        n = n->next;
    }

    n = rn;
    // propagating tmp state to current state
    while (n) {
        n->state = n->tmp_state;
        n = n->next;
    }
}

void set_seed(Node* rn, uint8_t seed[5][5]) {
}




int8_t main() {
    // get window size
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);

    Node rn;
    init_nodes(&rn, w.ws_col, Y_LEN);

    uint32_t gen_counter = 0;

    
    Node* n = get_node(&rn, 500);

    // oscilator seed
    //n->sw->state = 1;
    //n->s->state = 1;
    //n->se->state = 1;
    //
    uint8_t seed[5][5] = {
        {0,0,0,0,0},
        {0,0,1,0,0},
        {0,0,0,1,0},
        {0,0,0,1,0},
        {0,1,1,1,0}
    };

    printf("ylen: %ld\n", sizeof(seed)/sizeof(seed[0]));
    printf("xlen: %ld\n", sizeof(*seed)/sizeof(uint8_t));

    set_seed(&rn, seed);
    //return 0;

    // glider seed
    n->n->state = 1;
    n->e->state = 1;
    n->sw->state = 1;
    n->s->state = 1;
    n->se->state = 1;

    while (1) {
        print_matrix(&rn, w.ws_col, Y_LEN);
        do_tick(&rn);
        printf("\n\n");
        print_matrix(&rn, w.ws_col, Y_LEN);
        printf("Generation: %d\n", gen_counter);

        gen_counter++;
        usleep(INTERVAL);
    }
}
