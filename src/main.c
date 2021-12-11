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



int read_seed(Node** nodes, char* path, int xlim, int ylim) {
    /* Read seed from file, do some parsing of cell plaintext format.
     * Load results into nodes array.
     * TODO !!! this is a bit of a discustingly large function and needs splitting up.
     */
    FILE *fp = fopen(path, "r");
    char c;

    // offset from seed in matrix
    int x_offset;
    int y_offset;

    // keep track of location in text file
    int x = 0;
    int y = 0;

    if (fp == NULL) {
        printf("File not found!\n");
        return -1;
    }

    // count seed dimensions, used for calculating matrix offset
    char buf[5000];
    int max_xlen = 0;
    int max_ylen = 0;
    while (fgets(buf, 5000, fp) != NULL ) {

        if (buf[0] == '!') {
            continue;
        }

        // remove newline
        int xlen = strlen(buf)-2;
        if (xlen > max_xlen) {
            max_xlen = xlen;
        }

        max_ylen++;
    }
    printf("max_x/max_y: %d, %d\n", max_xlen, max_ylen);

    x_offset = (xlim-max_xlen) /2;
    y_offset = (ylim-max_ylen) /2;

    rewind(fp);

    // read file char by char
    while ((c = fgetc(fp)) != EOF ) {

        // if line is a comment, fast forward
        if (c == '!') {
            while (c != '\n') {
                c = fgetc(fp);
            }
            continue;
        }

        // replace newline with 0 terminator
        if (c == '\n') {
            y++;
            x = 0;
            printf("\n");
        }

        else {
            // if character is read, find corresponding location in matrix
            if (c == 'O') {
                int loc = get_loc(xlim, ylim, 0, x+x_offset, y+y_offset);
                Node* n = *(nodes+loc);
                n->state = 1;
            }

            printf("read: %c [%d,%d]\n", c, x, y);
            x++;
        }
    }
    fclose(fp);
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



    Node** nodes = init_nodes(matrix_width, matrix_height);

    Seed* seed = init_seed();
    seed->read_file(seed, seed_path);
    return 1;

    if (read_seed(nodes, seed_path, matrix_width, matrix_height) < 0)
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
