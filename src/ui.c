#include "ui.h"

WINDOW *ui_window = NULL;

int init_ui() {
    ui_window = initscr();

    if (ui_window == NULL)
        return 0;

    init_colors();
    curs_set(0);    // don't show cursor
    cbreak();       // don't wait for enter
    noecho();       // don't echo input to screen
    nodelay(stdscr, TRUE);  // don't block
}

void cleanup_ui() {
    delwin(ui_window);
    endwin();
    refresh();
}

void show_matrix(ViewPort* self) {
    Node** nodes = self->nodes;

    attrset(COLOR_PAIR(1));

    for (int y=0 ; y<self->size_y ; y++) {
        for (int x=0 ; x<self->size_x ; x++) {
            Node* n = *(self->nodes + (x+(y*self->size_x)));
            if (n->state)
                mvaddstr(y, x, ALIVE_CHR);
            else
                mvaddstr(y, x, DEAD_CHR);
        }
    }
    refresh();
    attroff(COLOR_PAIR(1));
    return;
}

void init_colors() {
    use_default_colors();

    if(has_colors()) {
        if(start_color() == OK) {
            init_pair(1, COLOR_RED,     -1);
            init_pair(2, COLOR_GREEN,   -1);
            init_pair(3, COLOR_YELLOW,  -1);
            init_pair(4, COLOR_BLUE,    -1);
            init_pair(5, COLOR_MAGENTA, -1);
            init_pair(6, COLOR_CYAN,    -1);
            init_pair(7, COLOR_WHITE,   -1);
            init_pair(8, COLOR_BLACK,   -1);

        } else {
            addstr("Cannot start colours\n");
            refresh();
        }
    } else {
        addstr("Not colour capable\n");
        refresh();
    }
} 

void* input_thread(void* state) {
    /* Listen for user input */
    State* s = (State*)state;
    char c;

    while (1) {
        c = getch();

        switch (c) {
            case 'q':
                s->is_stopped = 1;
                //printf("Exit thread\n");
                return NULL;
            case ' ':
                s->is_paused = !s->is_paused;
                break;
        }
    }
    addstr("Exit thread");
}
