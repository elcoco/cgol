#include "utils.h"

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

int get_index(int xlim, int ylim, int x, int y, EdgePolicy edge_policy) {
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

void print_usage() {
    printf("Seagull :: Game of life written in C!\n");
    //printf("\nMandatory arguments:\n");
    printf("\nOptional arguments:\n");
    printf("    -f SEED_FILE  Set seed file\n");
    printf("    -s SPEED_MS   Transition speed, Default=1000\n");
    printf("    -S            Set shading\n");
    printf("    -r            Set random seed\n");
    printf("    -w            Set wrapping edges\n");
}

bool parse_args(State* state, int argc, char** argv) {
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

int get_speed_incr(int speed, int incr) {
    /* get new speed depending on range */
    if (speed > 100000)
        return 10000;
    if (speed > 1000)
        return 1000;
    if (speed > 100)
        return 10;

    return 1;
}

/*
int test(int argc, char** argv) {
    Matrix* m = init_matrix(5, 5);
    m->edge_policy = EP_STOP;
    m->init_nodes(m);

    ViewPort* vp = m->init_viewport(m);

    State state;
    set_defaults(&state);

    if (!parse_args(&state, argc, argv))
        return 1;

    vp->update_viewport(vp, m, 0,
                               0,
                               3,
                               3);
    Seed* s = init_seed(5, 5);

    if (s->read_file(s, state.seed_path) < 0)
        return 1;

    s->to_matrix(s, m);     // write seed to matrix

    print_viewport(vp);

    init_ui();              // setup curses ui
    show_matrix(vp);
    sleep(1000);
    cleanup_ui();
}
*/
