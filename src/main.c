#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <sys/time.h>

#include <sys/ioctl.h>
#include <unistd.h>

#include <locale.h> // for utf8 in curses

#include "matrix.h"
#include "seed.h"
#include "user_inp.h"
#include "ui.h"

#define DEFAULT_SPEED_MS 1000000
#define PAUSED_INTERVAL 1000
#define CHECK_INTERVAL 10000

#define EXIT_ERROR -1


/* DONE wrap nodes in matrix struct so we can save dimensions and stuff
 * TODO random seed generator
 * TODO check if patern is too big for matrix
 * TODO use escape codes to replace lines in terminal
 * TODO use a thread to listen for user input
 * DONE make wrapping optional
 * DONE matrix should be able to be bigger than viewport
 *      add viewport dimensions to matrix struct so we can take a subset to display
 * TODO create ncuses ui.c
 * DONE keep array of alive cells. By only checking these+neighbours we greatly improve speed
 * DONE BUG  segfault using acorn seed @gen 2213, possibly when an alive cell falls off edge, doesn't happen with wrapping on
 * TODO when decreasing time, scale it. when below 100ms stepsize should be less
 */

void evolve(Matrix* m) {
    /* Do one tick, check all cells for changes
     * Instead of checking all cells we only track the alive cells and it's neighbouring dead cells. */

    assert(*(m->head) != NULL);
    Node* n = *(m->head);

    while (n) {
        // record changed state in tmp_state so we keep a clean state to test against

        // if alive cell has 2>cell>3 neighours, stay alive
        if (n->count_neighbours(n) >= 2 && n->count_neighbours(n) <= 3) {
            n->tmp_state = 1;
        } else {
            n->tmp_state = 0;
        }

        //assert(n->nw);
        //assert(n->n);
        //assert(n->ne);
        //assert(n->w);
        //assert(n->e);
        //assert(n->sw);
        //assert(n->s);
        //assert(n->se);

        // Check all dead cells around current cell.
        // If more than 3 alive neighbours -> a new cell is born.
        if (n->nw && n->nw->becomes_alive(n->nw)) {
            n->nw->tmp_state = 1;
            m->insert_alive_node(m, n->nw);
        }
        if (n->n && n->n->becomes_alive(n->n)) {
            n->n->tmp_state = 1;
            m->insert_alive_node(m, n->n);
        }
        if (n->ne && n->ne->becomes_alive(n->ne)) {
            n->ne->tmp_state = 1;
            m->insert_alive_node(m, n->ne);
        }
        if (n->e && n->e->becomes_alive(n->e)) {
            n->e->tmp_state = 1;
            m->insert_alive_node(m, n->e);
        }
        if (n->w && n->w->becomes_alive(n->w)) {
            n->w->tmp_state = 1;
            m->insert_alive_node(m, n->w);
        }
        if (n->sw && n->sw->becomes_alive(n->sw)) {
            n->sw->tmp_state = 1;
            m->insert_alive_node(m, n->sw);
        }
        if (n->s && n->s->becomes_alive(n->s)) {
            n->s->tmp_state = 1;
            m->insert_alive_node(m, n->s);
        }
        if (n->se && n->se->becomes_alive(n->se)) {
            n->se->tmp_state = 1;
            m->insert_alive_node(m, n->se);
        }

        n = n->next;
    }

    n = *(m->head);

    // propagating tmp state to current state
    while (n) {
        n->state = n->tmp_state;

        // remove the dead nodes
        if (!n->state) {
            Node* n_tmp = n;
            n = n->next;
            m->remove_alive_node(m, n_tmp);

        } else {
            n = n->next;
        }
    }
    return;
}

void set_defaults(State* state) {
    state->speed_ms = DEFAULT_SPEED_MS;
    //state->seed_path = NULL;
    state->set_random = 0;
    state->is_paused = 0;
    state->is_stopped = 0;
    state->set_wrapping = 0;
    state->gen_counter = 0;
    state->pan_x = 0;
    state->pan_y = 0;
    state->is_pan_changed = 0;
}

int get_speed_incr(int speed, int incr) {
    if (speed > 100000)
        return 10000;
    if (speed > 1000)
        return 1000;
    if (speed > 100)
        return 10;

    return 1;



}

int check_user_input(State* state) {
    /* check for user input, return 1 if there was input */
    char c = getch();
    if (c != ERR) {
        switch (c) {
            case 'q':
                state->is_stopped = 1;
                break;
            case ' ':
                state->is_paused = !state->is_paused;
                break;
            case '+':
                state->speed_ms += get_speed_incr(state->speed_ms, 1);
                break;
            case '-':
                state->speed_ms -= get_speed_incr(state->speed_ms, 1);
                break;
            case 'h':
                state->pan_x--;
                state->is_pan_changed = 1;
                break;
            case 'l':
                state->pan_x++;
                state->is_pan_changed = 1;
                break;
            case 'k':
                state->pan_y--;
                state->is_pan_changed = 1;
                break;
            case 'j':
                state->pan_y++;
                state->is_pan_changed = 1;
                break;
            case '0':
                state->pan_y = 0;
                state->pan_x = 0;
                state->is_pan_changed = 1;
                break;
            default:
                return 0;
        }

        // flush chars
        while (c != ERR)
            c = getch();

        return 1;
    }

    return 0;
}

void non_blocking_sleep(int interval, State* state) {
    /* Do a non blocking sleep that checks for user input */
    struct timeval t_start, t_end;
    gettimeofday(&t_start, NULL);

    while (1) {
        gettimeofday(&t_end, NULL);
        if ((t_end.tv_sec*1000000 + t_end.tv_usec) - (t_start.tv_sec*1000000 + t_start.tv_usec) >= interval)
            break;

        if (check_user_input(state))
            return;

        usleep(CHECK_INTERVAL);
    }
}

int main(int argc, char** argv) {

    setlocale(LC_ALL, "");  // for UTF8 in curses

    // get window size
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    int matrix_height = w.ws_row -3;
    int matrix_width = w.ws_col;

    // state contains program state like is_paused, is_stopped etc...
    State state;
    set_defaults(&state);

    if (parse_args(&state, argc, argv) < 0)
        return 1;

    // matrix contains all data
    Matrix* m = init_matrix(MATRIX_WIDTH, MATRIX_HEIGHT);
    m->edge_policy = (state.set_wrapping) ? EP_WRAP : EP_STOP;
    m->init_nodes(m);

    // viewport represents only the part of the data that is shown on screen
    ViewPort* vp = m->init_viewport(m);
    vp->update_viewport(vp, m, (MATRIX_WIDTH/2), (MATRIX_HEIGHT/2), matrix_width, matrix_height);

    Seed* s = init_seed(MATRIX_WIDTH, MATRIX_HEIGHT);

    if (state.seed_path) {
        if (s->read_file(s, state.seed_path) < 0)
            return 1;

    } else if (state.set_random) {
        printf("Not implemented!\n");
        return 1;
    }

    // write seed to matrix
    s->to_matrix(s, m);
    printf("xoffset: %d, yoffset: %d\n", s->x_offset, s->y_offset);

    init_ui();

    while (!state.is_stopped) {

        if (state.is_pan_changed) {
            vp->update_viewport(vp, m, (MATRIX_WIDTH/2)+state.pan_x, (MATRIX_HEIGHT/2)+state.pan_y, matrix_width, matrix_height);
            state.is_pan_changed = 0;
        }

        printw("Generation: %d | Speed: %d | Paused: %d | alive_nodes: %d | vp x/y: %d:%d\n",
                state.gen_counter,
                state.speed_ms,
                state.is_paused,
                m->alive_nodes,
                vp->origin_x,
                vp->origin_y);

        if (!state.is_paused) {
            show_matrix(vp);
            evolve(m);
            state.gen_counter++;
        }
        non_blocking_sleep(state.speed_ms, &state);
    }

    cleanup_ui();
    return 0;
}
