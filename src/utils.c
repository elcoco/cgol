#include "utils.h"

int get_index(int xlim, int ylim, int x, int y, EdgePolicy edge_policy)
{
    /* return array index for the given coordinates */
    // TODO handle wrapping etc

    // calculate index of (0.0)
    int origin = (xlim * ylim) / 2;

    int m_maxx = xlim/2;
    int m_maxy = ylim/2;

    int m_minx = 0-m_maxx;
    int m_miny = 0-m_maxy;

    if (edge_policy == EP_WRAP) {
        // TODO implement wrapping again
    }
    else if (edge_policy == EP_STOP) {
        if (x > m_maxx)
            x = -1;
        if (x < m_minx)
            x = -1;
        if (y > m_maxy)
            y = -1;
        if (y < m_miny)
            y = -1;
    }

    /*
    printf("xy: %d,%d | minmaxxy: %d,%d > %d,%d | xylim: %dx%d | origin: %d -> %d\n",
        x,
        y,
        m_minx,
        m_miny,
        m_maxx,
        m_maxy,
        xlim,
        ylim,
        origin,
        (y*xlim) + x + origin);
        */

    return (y*xlim) + x + origin;
}

void print_usage()
{
    printf("CGOL :: Game of life written in C!\n");
    //printf("\nMandatory arguments:\n");
    printf("\nOptional arguments:\n");
    printf("    -f SEED_FILE  Set seed file\n");
    printf("    -s SPEED_MS   Transition speed, Default=1000\n");
    printf("    -S            Set shading\n");
    printf("    -r            Set random seed\n");
    printf("    -w            Set wrapping edges\n");
}

bool parse_args(State* state, int argc, char** argv)
{
    int option;

    while((option = getopt(argc, argv, "f:s:rwS")) != -1){ //get option from the getopt() method
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
            case 'S':
                state->set_shade = 1;
                break;
            case ':': 
                printf("option needs a value\n"); 
                return false;
            case '?': 
                print_usage();
                return false;
       }
    }
    if (argc == 1) {
        print_usage();
        return false;
    }
    return true;
}

int get_speed_incr(int speed, int incr)
{
    /* get new speed depending on range */
    if (speed > 100000)
        return 10000;
    if (speed > 1000)
        return 1000;
    if (speed > 100)
        return 10;

    return 1;
}

int get_rand(int lower, int upper)
{
    return (rand() % (upper - lower + 1)) + lower;
}
