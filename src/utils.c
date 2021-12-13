#include "utils.h"

int get_loc(int xlim, int ylim, int cur_index, int xmov, int ymov, EdgePolicy edge_policy) {
    /* Translate array index to physical matrix coordinates,
     * apply movement, and translate back to array index
     * Wrap edges if specified
     */
    int wrapping = 0;

    // find xy coordinates
    int curx = cur_index % xlim;
    int cury = cur_index / xlim;

    // apply movement to current location
    int destx = curx + xmov;
    int desty = cury + ymov;

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

    //printf("index=%d=>%d, %d %d -> %d %d\n", cur_index, (desty*xlim) + destx, curx, cury, destx, desty);
    return (desty*xlim) + destx;
}

void print_usage() {
    printf("Seagull :: Game of life written in C!\n");
    //printf("\nMandatory arguments:\n");
    printf("\nOptional arguments:\n");
    printf("    -f SEED_FILE  Set seed file\n");
    printf("    -s SPEED_MS   Transition speed, Default=1000\n");
    printf("    -r            Set random seed\n");
    printf("    -w            Set wrapping edges\n");
}

int parse_args(State* state, int argc, char** argv) {
    int option;

    while((option = getopt(argc, argv, "f:s:rw")) != -1){ //get option from the getopt() method
        switch (option) {
            case 'f':
                state->seed_path = strdup(optarg);
                break;
            case 's':
                state->speed_ms = atoi(optarg) * 1000;
                break;
            case 'r':
                state->set_random = 1;
                break;
            case 'w':
                state->set_wrapping = 1;
                break;
            case ':': 
                printf("option needs a value\n"); 
                return -1;
            case '?': 
                print_usage();
                return -1;
       }
    }
    if (argc == 1) {
        print_usage();
        return -1;
    }
    if (!(state->seed_path || state->set_random)) {
        printf("Please specify seed source!\n");
        print_usage();
        return -1;
    }
    return 0;
}
