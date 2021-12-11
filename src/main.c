#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>         // uint types
#include <string.h>

#include <sys/ioctl.h>
#include <unistd.h>

#include "node.h"
#include "seed.h"

#define DEFAULT_SPEED_MS 1000000


/* TODO wrap nodes in matrix struct so we can save dimensions and stuff
 * TODO random seed generator
 * TODO check if patern is too big for matrix
 */

void evolve(Node** nodes) {
    /* Do one tick, check all cells for changes */
    Node** ptr = nodes;
    Node* n = *ptr;

    while (n) {
        // record changed state in tmp_state so we keep a clean state to test against
        n->tmp_state = 0;

        // if dead cell has 3 neighbours -> alive
        if (!n->state && n->count_neighbours(n) == 3)
            n->tmp_state = 1;

        // if alive cell has 2>cell>3 neighours, stay alive
        if (n->state && n->count_neighbours(n) >= 2 && n->count_neighbours(n) <= 3)
            n->tmp_state = 1;

        ptr++;
        n = *ptr;
    }

    // propagating tmp state to current state
    ptr = nodes;
    n = *ptr;
    while (n) {
        n->state = n->tmp_state;
        ptr++;
        n = *ptr;
    }
    return;
}

int8_t main(int argc, char** argv) {
    uint32_t gen_counter = 0;

    // get window size
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    int matrix_height = w.ws_row -3;
    int matrix_width = w.ws_col;

    // parse arguments
    Args* args = parse_args(argc, argv);
    if (!args)
        return 1;

    // set some defaults
    if (! args->speed_ms)
        args->speed_ms = DEFAULT_SPEED_MS;

    Node** nodes = init_nodes(matrix_width, matrix_height);

    // read seed and write to matrix
    Seed* seed = init_seed(matrix_width, matrix_height);
    if (seed->read_file(seed, args->seed_path) < 0)
        return 1;
    else
        seed->to_matrix(seed, nodes);

    while (1) {
        print_matrix(nodes, matrix_width, matrix_height);
        printf("Generation: %d\n", gen_counter);

        evolve(nodes);
        gen_counter++;
        usleep(args->speed_ms);
    }
    return 0;
}
