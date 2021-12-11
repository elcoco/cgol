#include "utils.h"

int get_loc(int xlim, int ylim, int cur_index, int xmov, int ymov) {
    // translate array index to physical matrix coordinates
    // apply movement, and translate back to array index

    // find xy coordinates
    int curx = cur_index % xlim;
    int cury = cur_index / xlim;
    int destx = curx + xmov;
    int desty = cury + ymov;

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

    //printf("index=%d=>%d, %d %d -> %d %d\n", cur_index, (desty*xlim) + destx, curx, cury, destx, desty);
    return (desty*xlim) + destx;
}

void print_usage() {
    printf("Seagull :: Game of life written in sea!\n");
    printf("\nMandatory arguments:\n");
    printf("    -f SEED_FILE\n");
    printf("\nOptional arguments:\n");
    printf("    -s SPEED_MS, Default=1000\n");
}

Args* parse_args(int argc, char** argv) {
    Args* args = (Args*)malloc(sizeof(Args));
    int option;

    while((option = getopt(argc, argv, "f:s:")) != -1){ //get option from the getopt() method
        switch (option) {
            case 'f':
               args->seed_path = strdup(optarg);
               break;
            case 's':
               args->speed_ms = atoi(optarg) * 1000;
               break;
            case ':': 
                printf("option needs a value\n"); 
                return NULL;
            case '?': 
                print_usage();
                return NULL;
       }
    }
    if (argc == 1) {
        print_usage();
        return NULL;
    }
    if (!args->seed_path) {
        printf("Please specify seed file.\n");
        return NULL;
    }
    return args;
}
