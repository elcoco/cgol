#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <assert.h>

#include <sys/ioctl.h>
#include <unistd.h>

#include "matrix.h"
#include "seed.h"
#include "user_inp.h"

#define DEFAULT_SPEED_MS 1000000
#define PAUSED_INTERVAL 1000

#define EXIT_ERROR -1


/* DONE wrap nodes in matrix struct so we can save dimensions and stuff
 * TODO random seed generator
 * TODO check if patern is too big for matrix
 * TODO use escape codes to replace lines in terminal
 * TODO use a thread to listen for user input
 * DONE make wrapping optional
 * TODO matrix should be able to be bigger than viewport
 *      add viewport dimensions to matrix struct so we can take a subset to display
 * TODO create ncuses ui.c
 * DONE keep array of alive cells. By only checking these+neighbours we greatly improve speed
 * BUG  segfault using acorn seed @gen 2213, possibly when an alive cell falls off edge, doesn't happen with wrapping on
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

        // Check all dead cells around this cell.
        // If more than 3 alive neighbours -> a new cell is born.
        if (!n->nw->state && !n->nw->tmp_state && n->nw->count_neighbours(n->nw) == 3) {
            n->nw->tmp_state = 1;
            m->insert_alive_node(m, n->nw);
        }
        if (!n->n->state && !n->n->tmp_state && n->n->count_neighbours(n->n) == 3) {
            n->n->tmp_state = 1;
            m->insert_alive_node(m, n->n);
        }
        if (!n->ne->state && !n->ne->tmp_state && n->ne->count_neighbours(n->ne) == 3) {
            n->ne->tmp_state = 1;
            m->insert_alive_node(m, n->ne);
        }
        if (!n->e->state && !n->e->tmp_state && n->e->count_neighbours(n->e) == 3) {
            n->e->tmp_state = 1;
            m->insert_alive_node(m, n->e);
        }
        if (!n->w->state && !n->w->tmp_state && n->w->count_neighbours(n->w) == 3) {
            n->w->tmp_state = 1;
            m->insert_alive_node(m, n->w);
        }
        if (!n->sw->state && !n->sw->tmp_state && n->sw->count_neighbours(n->sw) == 3) {
            n->sw->tmp_state = 1;
            m->insert_alive_node(m, n->sw);
        }
        if (!n->s->state && !n->s->tmp_state && n->s->count_neighbours(n->s) == 3) {
            n->s->tmp_state = 1;
            m->insert_alive_node(m, n->s);
        }
        if (!n->se->state && !n->se->tmp_state && n->se->count_neighbours(n->se) == 3) {
            n->se->tmp_state = 1;
            m->insert_alive_node(m, n->se);
        }

        n = n->next;
    }

    print_linked_list(*m->head);

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
}

void* input_thread(void* state) {
    State* s = (State*)state;
    char c;

    set_term_raw();
    while (1) {
        while (! has_data()) {
        }

        c = get_chr();

        switch (c) {
            case 'q':
                s->is_stopped = 1;
                printf("Exit thread\n");
                return NULL;
            case ' ':
                s->is_paused = !s->is_paused;
                break;
        }
    }
}

int main(int argc, char** argv) {
    int gen_counter = 0;

    // get window size
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    int matrix_height = w.ws_row -3;
    int matrix_width = w.ws_col;

    State state;
    set_defaults(&state);

    // parse arguments
    if (parse_args(&state, argc, argv) < 0)
        return 1;

    // create matrix
    Matrix* m = init_matrix(MATRIX_WIDTH, MATRIX_HEIGHT);
    m->edge_policy = (state.set_wrapping) ? EP_WRAP : EP_STOP;
    m->init_nodes(m);

    ViewPort* vp = m->get_viewport(m, (MATRIX_WIDTH/2), (MATRIX_HEIGHT/2), matrix_width, matrix_height);

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

    pthread_t inp_thread;
    //pthread_create(&inp_thread, NULL, input_thread, (void*)state);

    while (!state.is_stopped) {

        vp->print_viewport(vp);
        //m->print_matrix(m);
        printf("Generation: %d | Paused: %d | alive_nodes: %d\n", gen_counter, state.is_paused, m->alive_nodes);

        while (state.is_paused)
            usleep(PAUSED_INTERVAL);

        evolve(m);
        gen_counter++;
        usleep(state.speed_ms);
    }

    pthread_join(inp_thread, NULL);
    return 0;
}
