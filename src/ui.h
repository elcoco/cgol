#ifndef UI_H
#define UI_H

#include <curses.h>

#include "matrix.h"

int init_ui();
void cleanup_ui();
void show_matrix(ViewPort* vp);
void init_colors();
void* input_thread(void* state);

#endif
