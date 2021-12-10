#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>         // uint types
#include <string.h>

#include <sys/ioctl.h>
#include <unistd.h>

#include "node.h"

#define X_LEN 50
#define Y_LEN 30

#define INTERVAL 100000
//#define INTERVAL 100000



void evolve(Node** nodes) {
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

int read_seed(Node** nodes, char* path, int xlim, int ylim, int x_offset, int y_offset) {
    FILE *fp = fopen(path, "r");
    //FILE *fp = fopen("seeds/glider.seed", "r");

    char c;

    // keep track of location in text file
    int x = 0;
    int y = 0;

    if (fp == NULL) {
        printf("File not found!\n");
        return -1;
    }

    while ((c = fgetc(fp)) != EOF ) {

        // replace newline with 0 terminator
        if (c == '\n') {
            y++;
            x = 0;
            printf("\n");
        }
        else {
            // if character is read, find corresponding location in matrix
            if (c != ' ') {
                int loc = get_loc(xlim, ylim, 0, x+x_offset, y+y_offset);
                Node* n = *(nodes+loc);
                n->state = 1;
            }

            printf("read: %c %d,%d\n", c, x, y);
            x++;
        }
    }
    return 0;
}

void print_usage() {
    printf("Seagull :: Game of life written in sea!\n");
    printf("\nMandatory arguments:\n");
    printf("    -f SEED_FILE\n");
    printf("\nOptional arguments:\n");
    printf("    -s SPEED_MS, Default=1000\n");
}

int8_t main(int argc, char** argv) {
    // TODO wrap nodes in matrix struct so we can save dimensions and stuff
    // TODO random seed generator
    // TODO check if patern is too big for matrix

    //if (argc < 2) {
    //    printf("Specify path\n");
    //    return 1;
    //}

    int option;
    char* seed_path;
    int speed_ms = 1000000;

    while((option = getopt(argc, argv, "f:s:")) != -1){ //get option from the getopt() method
        switch (option) {
            case 'f':
               seed_path = strdup(optarg);
               break;
            case 's':
               //printf("speed: %d\n", optarg);
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

    // get window size
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);

    uint32_t gen_counter = 0;

    int matrix_height = w.ws_row -3;
    int matrix_width = w.ws_col;

    Node** nodes = init_nodes(matrix_width, matrix_height);

    if (read_seed(nodes, seed_path, matrix_width, matrix_height, 5, 5) < 0)
        return 1;

    print_matrix(nodes, matrix_width, matrix_height);

    while (1) {
        print_matrix(nodes, matrix_width, matrix_height);
        printf("Generation: %d\n", gen_counter);

        evolve(nodes);
        gen_counter++;
        usleep(speed_ms);
    }


    return 0;
}
