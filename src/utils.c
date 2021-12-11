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

