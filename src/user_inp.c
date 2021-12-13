#include "user_inp.h"

// this is a bit over commented but don't do it, in about a week this is complete nonsense to me


// global teminal state is saved and restored in separate functions
struct termios orig_termios;

void restore_term_state() {
    /* TCSANOW execute command now */
    tcsetattr(0, TCSANOW, &orig_termios);
}

void set_term_raw() {
    struct termios new_termios;

    /* take two copies - one for now, one for later */
    tcgetattr(0, &orig_termios);
    tcgetattr(0, &new_termios);
    //memcpy(&new_termios, &orig_termios, sizeof(new_termios));

    /* register cleanup handler, and set the new terminal mode */
    atexit(restore_term_state);         // run this function when program exits normally
    cfmakeraw(&new_termios);             // put terminal in raw mode
    tcsetattr(0, TCSANOW, &new_termios); // effectuate the new settings
}

int has_data() {
    /* watch the read stdin file discriptor for input
     * select returns non zero if there was
     */

    //struct timeval tv = { 0L, 0L };
    /* select watches sets of file discriptors */
    fd_set fds;         // create a file discriptor set
    FD_ZERO(&fds);      // zero out/initialize fd set
    FD_SET(0, &fds);    // add fd 0 to the set
    // select(max_amount_fds, reading_fds, writing_fds, err_fds, timeout)
    return select(1, &fds, NULL, NULL, NULL);
}

char get_chr() {
    int r;
    unsigned char c;
    if ((r = read(0, &c, sizeof(c))) < 0) {
        return r;
    } else {
        printf("%c\n", c);
        return c;
    }
}

int test(int argc, char *argv[]) {
    set_term_raw();

    while (!has_data()) {
        /* do some work */
    }
    (void)get_chr(); /* consume the character */
    return 0;
}
