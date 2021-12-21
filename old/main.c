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

int get_loc_bak(int xlim, int ylim, int start_index, int xmov, int ymov, EdgePolicy edge_policy) {
    /* Translate array index to physical matrix coordinates,
     * apply movement, and translate back to array index
     * Wrap edges if specified
     */
    // find xy coordinates
    int startx = start_index % xlim;
    int starty = start_index / xlim;

    // apply movement to current location
    int destx = startx + xmov;
    int desty = starty + ymov;

    if (edge_policy == EP_WRAP) {
        // wrap matrix sides
        if (destx > (xlim-1)) {
            destx = destx - xlim;
        }
        if (destx < 0) {
            destx = xlim - (0 - destx);
        }
        if (desty > (ylim-1)) {
            desty = desty-ylim;
        }
        if (desty < 0) {
            desty = ylim - (0 - desty);
        }

    } else if (edge_policy == EP_STOP) {
        // don't move beyond matrix limits
        if (destx > (xlim-1)) {
            destx = xlim-((xlim-1)-destx);
        }
        if (destx < 0) {
            destx = 0;
        }
        if (desty > (ylim-1)) {
            destx = ylim-((ylim-1)-desty);
        }
        if (desty < 0) {
            desty = 0;
        }
    }

    //printf("index=%d=>%d, %d %d -> %d %d\n", start_index, (desty*xlim) + destx, curx, cury, destx, desty);
    return (desty*xlim) + destx;
}

int get_loc(int xlim, int ylim, int start_index, int xmov, int ymov, EdgePolicy edge_policy) {
    /* Translate array index to physical matrix coordinates,
     * apply movement, and translate back to array index
     * Wrap edges if specified
     */
    // calculate array index of the 0,0 coordinate
    // xlim*ylim should always be an odd number for this to work
    int origin = (xlim * ylim) / 2;

    // find xy coordinates
    int startx = (start_index+origin) % xlim;
    int starty = (start_index+origin) / xlim;

    // apply movement to current location
    int destx = startx + xmov;
    int desty = starty + ymov;

    int matrix_maxx = xlim/2;
    int matrix_minx = 0-(xlim/2);
    int matrix_maxy = ylim/2;
    int matrix_miny = 0-(ylim/2);

    if (edge_policy == EP_WRAP) {
        // wrap matrix sides
        if (destx > (xlim-1)) {
            destx = destx - xlim;
        }
        if (destx < 0) {
            destx = xlim - (0 - destx);
        }
        if (desty > (ylim-1)) {
            desty = desty-ylim;
        }
        if (desty < 0) {
            desty = ylim - (0 - desty);
        }

    } else if (edge_policy == EP_STOP) {
        // don't move beyond matrix limits
        if (destx > matrix_minx) {
            destx = matrix_minx-(matrix_minx-destx);
        }
        if (destx < matrix_minx) {
            destx = matrix_minx;
        }
        if (desty > matrix_maxy) {
            destx = matrix_maxy-(matrix_maxy-desty);
        }
        if (desty < matrix_miny) {
            desty = matrix_miny;
        }
    }



    //printf("index=%d=>%d, %d %d -> %d %d\n", start_index, (desty*xlim) + destx, curx, cury, destx, desty);
    return (desty*xlim) + destx;
}

