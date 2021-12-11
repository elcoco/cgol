#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>         // uint types
#include <string.h>

#include <sys/ioctl.h>
#include <unistd.h>

#include "node.h"
#include "seed.h"


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

void print_usage() {
    printf("Seagull :: Game of life written in sea!\n");
    printf("\nMandatory arguments:\n");
    printf("    -f SEED_FILE\n");
    printf("\nOptional arguments:\n");
    printf("    -s SPEED_MS, Default=1000\n");
}

int8_t main(int argc, char** argv) {
    int option;
    char* seed_path;
    int speed_ms = 1000000;
    uint32_t gen_counter = 0;

    // get window size
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    int matrix_height = w.ws_row -3;
    int matrix_width = w.ws_col;

    while((option = getopt(argc, argv, "f:s:")) != -1){ //get option from the getopt() method
        switch (option) {
            case 'f':
               seed_path = strdup(optarg);
               break;
            case 's':
               speed_ms = atoi(optarg) * 1000;
               break;
            case ':': 
                printf("option needs a value\n"); 
                return 1;
            case '?': 
                print_usage();
                return 1;
       }
    }
    if (argc == 1) {
        print_usage();
        return 1;
    }
    if (!seed_path) {
        printf("Please specify seed file.\n");
        return 1;
    }

    Node** nodes = init_nodes(matrix_width, matrix_height);

    // read seed and write to matrix
    Seed* seed = init_seed(matrix_width, matrix_height);
    if (seed->read_file(seed, seed_path) < 0)
        return 1;

    seed->print_seed(seed);
    seed->to_matrix(seed, nodes);

    while (1) {
        print_matrix(nodes, matrix_width, matrix_height);
        printf("Generation: %d\n", gen_counter);

        evolve(nodes);
        gen_counter++;
        usleep(speed_ms);
    }
    return 0;
}
