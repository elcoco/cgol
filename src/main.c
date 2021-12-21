#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <signal.h>     // catch SIGTERM

#include <unistd.h>

#include <locale.h>     // for utf8 in curses

#include "matrix.h"
#include "seed.h"
#include "ui.h"

#define DEFAULT_SPEED_MS 1000000
#define PAUSED_INTERVAL 1000
#define DEFAULT_PAN_STEPS 5
#define DEFAULT_PAN_BIG_STEPS 20
#define STATUS_LINES 2


/* DONE wrap nodes in matrix struct so we can save dimensions and stuff
 * TODO random seed generator
 * TODO error when pattern exceeds matrix boundaries, get_index returns -1 if so
 *      should not be difficult
 * DONE make wrapping optional
 * DONE matrix should be able to be bigger than viewport
 *      add viewport dimensions to matrix struct so we can take a subset to display
 * DONE create ncuses ui.c
 * DONE keep array of alive cells. By only checking these+neighbours we greatly improve speed
 * DONE BUG  segfault using acorn seed @gen 2213, possibly when an alive cell falls off edge, doesn't happen with wrapping on
 * DONE when decreasing time, scale it. when below 100ms stepsize should be less
 * DONE when pressing button, do execute command but dont skip the delay
 * TODO color based on age
 * DONE update viewport dimensions on terminal resize
 * TODO indicate if matrix is in a stable state
 * DONE viewport xy offset should not go beyond limits
 * TODO  Segfault when making terminal bigger, doesn't happen when making term smaller
 * DONE draw patterns with mouse
 * TODO write a RLE pattern file decoder
 * TODO make curses help window
 * DONE make shading optional
 * DONE make origin in center
 * DONE when no seed is specified, start in paused mode
 */

int sigint_caught = 0;

void evolve(Matrix* m) {
    /* Do one tick, check all cells for changes
     * Instead of checking all cells we only track the alive cells and it's neighbouring dead cells. */

    assert(*(m->head) != NULL);
    Node* n = *(m->head);

    while (n) {
        // record changed state in tmp_state so we keep a clean state to test against
        // if alive cell has 2<cell<3 neighours, stay alive
        n->tmp_state = (n->count_neighbours(n) >= 2 && n->count_neighbours(n) <= 3);

        // Check all dead cells around current cell.
        // If more than 3 alive neighbours -> a new cell is born.
        if (n->nw && n->nw->becomes_alive(n->nw)) {
            n->nw->tmp_state = true;
            m->insert_alive_node(m, n->nw);
        }
        if (n->n && n->n->becomes_alive(n->n)) {
            n->n->tmp_state = true;
            m->insert_alive_node(m, n->n);
        }
        if (n->ne && n->ne->becomes_alive(n->ne)) {
            n->ne->tmp_state = true;
            m->insert_alive_node(m, n->ne);
        }
        if (n->e && n->e->becomes_alive(n->e)) {
            n->e->tmp_state = true;
            m->insert_alive_node(m, n->e);
        }
        if (n->w && n->w->becomes_alive(n->w)) {
            n->w->tmp_state = true;
            m->insert_alive_node(m, n->w);
        }
        if (n->sw && n->sw->becomes_alive(n->sw)) {
            n->sw->tmp_state = true;
            m->insert_alive_node(m, n->sw);
        }
        if (n->s && n->s->becomes_alive(n->s)) {
            n->s->tmp_state = true;
            m->insert_alive_node(m, n->s);
        }
        if (n->se && n->se->becomes_alive(n->se)) {
            n->se->tmp_state = true;
            m->insert_alive_node(m, n->se);
        }

        n = n->next;
    }

    n = *(m->head);

    // propagate tmp state to current state
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
    /* Set some default values for the state struct so we don't end up with garbage */
    state->speed_ms = DEFAULT_SPEED_MS;
    state->set_random     = false;
    state->is_paused      = false;
    state->is_stopped     = false;
    state->set_wrapping   = false;
    state->set_shade      = false;
    state->gen_counter    = 0;
    state->pan_x          = 0;
    state->pan_y          = 0;
    state->is_pan_changed = false;
    state->term_is_updated = false;
    state->term_x          = 0;
    state->term_y          = 0;
    state->clicked_x       = -1;
    state->clicked_y       = -1;
    state->do_clear       = false;
}

bool check_user_input(void* arg) {
    // state struct is passed as an argument to a callback, cast it to the proper type
    State* state = arg;
    MEVENT event;       // curses mouse event

    /* check for user input, return 1 if there was input */
    int c = getch();
    if (c != ERR) {
        switch (c) {
            case 'q':
                state->is_stopped = true;
                break;
            case ' ':
                state->is_paused = !state->is_paused;
                break;
            case '+':
                state->speed_ms -= get_speed_incr(state->speed_ms, 1);
                break;
            case '-':
                state->speed_ms += get_speed_incr(state->speed_ms, 1);
                break;
            case 'h':
                state->pan_x-=DEFAULT_PAN_STEPS;
                state->is_pan_changed = true;
                break;
            case 'l':
                state->pan_x+=DEFAULT_PAN_STEPS;
                state->is_pan_changed = true;
                break;
            case 'k':
                state->pan_y-=DEFAULT_PAN_STEPS;
                state->is_pan_changed = true;
                break;
            case 'j':
                state->pan_y+=DEFAULT_PAN_STEPS;
                state->is_pan_changed = true;
                break;
            case 'H':
                state->pan_x-=DEFAULT_PAN_BIG_STEPS;
                state->is_pan_changed = true;
                break;
            case 'L':
                state->pan_x+=DEFAULT_PAN_BIG_STEPS;
                state->is_pan_changed = true;
                break;
            case 'K':
                state->pan_y-=DEFAULT_PAN_BIG_STEPS;
                state->is_pan_changed = true;
                break;
            case 'J':
                state->pan_y+=DEFAULT_PAN_BIG_STEPS;
                state->is_pan_changed = true;
                break;
            case '0':
                state->pan_y = 0;
                state->pan_x = 0;
                state->is_pan_changed = true;
                break;
            case 's':
                state->is_paused = true;
                state->do_step = true;
                break;
            case 'S':
                state->set_shade = !state->set_shade;
                break;
            case 'c':
                state->do_clear = true;
                break;
            case KEY_MOUSE:
                if (getmouse(&event) == OK) {
                    if(event.bstate & BUTTON1_CLICKED) {
                        // This works for left-click
                        state->clicked_x = event.x;
                        state->clicked_y = event.y;
                    }
                }
                break;
            default:
                return false;
        }

        // flush chars
        while (c != ERR)
            c = getch();

        return true;
    }
    return false;
}

void show_status(State* state, Matrix* m, ViewPort* vp) {
    mvprintw(vp->size_y, 0,
            "Generation: %d | Speed: %d | Paused: %d | alive_nodes: %d | vp x/y: %d:%d | panxy %d:%d | shade: %d\n",
            state->gen_counter,
            state->speed_ms,
            state->is_paused,
            m->alive_nodes,
            vp->offset_x,
            vp->offset_y,
            state->pan_x,
            state->pan_y,
            state->set_shade);
}

void loop(State* state, Matrix* m, ViewPort* vp) {
    while (!state->is_stopped && !sigint_caught) {
        vp->set_shade = state->set_shade;

        if (state->do_clear) {
            m->clear_matrix(m);
            state->do_clear = false;
        }

        if (m->alive_nodes <= 0)
            state->is_paused = true;


        // handle mouse clicks
        if (state->clicked_x > 0 || state->clicked_y > 0) {
            m->toggle_node(m, state->clicked_x, state->clicked_y, state->pan_x, state->pan_y);
            state->clicked_x = -1;
            state->clicked_y = -1;
        }

        // handle terminal resize and panning
        if (state->term_x != COLS || state->term_y != LINES || state->is_pan_changed) {
            state->is_pan_changed = false;
            state->term_x = COLS;
            state->term_y = LINES;

            vp->update_viewport(vp, m, state->pan_x,
                                       state->pan_y,
                                       state->term_x,
                                       state->term_y-STATUS_LINES);
        }

        show_status(state, m, vp);
        show_matrix(vp);

        // listen for user input while sleeping
        if (!non_blocking_sleep(state->speed_ms, &check_user_input, state)) {

            if (!state->is_paused) {
                evolve(m);
                state->gen_counter++;
            }
        }

        // handle single steps while paused
        if (state->do_step) {
            state->do_step = false;
            evolve(m);
            state->gen_counter++;
        }

        // limit panning to data dimensions
        if (state->pan_x > (MATRIX_WIDTH/2) - (vp->size_x/2))
            state->pan_x = (MATRIX_WIDTH/2) - (vp->size_x/2);
        if (state->pan_y > (MATRIX_HEIGHT/2) - (vp->size_y/2))
            state->pan_y = (MATRIX_HEIGHT/2) - (vp->size_y/2);

        if (state->pan_x < (0-(MATRIX_WIDTH/2)) + (vp->size_x/2))
            state->pan_x = (0-(MATRIX_WIDTH/2)) + (vp->size_x/2);
        if (state->pan_y < (0-(MATRIX_HEIGHT/2)) + (vp->size_y/2))
            state->pan_y = (0-(MATRIX_HEIGHT/2)) + (vp->size_y/2);
    }
}

void on_sigint(int signum) {
    sigint_caught = 1;
}

int main(int argc, char** argv) {
    setlocale(LC_ALL, "");  // for UTF8 in curses

    // state contains program state like is_paused, is_stopped etc...
    State state;
    set_defaults(&state);

    // catch sigint (CTRL-C)
    struct sigaction action;
    memset(&action, 0, sizeof(struct sigaction));
    action.sa_handler = on_sigint;
    sigaction(SIGINT, &action, NULL);

    if (!parse_args(&state, argc, argv))
        return 1;

    // matrix contains all data
    Matrix* m = init_matrix(MATRIX_WIDTH, MATRIX_HEIGHT);
    m->edge_policy = (state.set_wrapping) ? EP_WRAP : EP_STOP;
    m->init_nodes(m);

    // viewport represents only the part of the data that is shown on screen
    ViewPort* vp = m->init_viewport(m);

    // seed is the initial state of the matrix, can be from file or random
    Seed* s = init_seed(MATRIX_WIDTH, MATRIX_HEIGHT);

    if (s->read_rle(s, state.seed_path) < 0)
        return 1;

    //if (state.seed_path) {
    //    if (s->read_file(s, state.seed_path) < 0)
    //        return 1;
    //} else if (state.set_random) {
    //    printf("Not implemented!\n");
    //    return 1;
    //}

    s->to_matrix(s, m);     // write seed to matrix
    init_ui();              // setup curses ui

    loop(&state, m, vp);

    cleanup_ui();
    return 0;
}
