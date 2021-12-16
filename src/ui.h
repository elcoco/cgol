#ifndef UI_H
#define UI_H

#include <curses.h>
#include <sys/time.h>   // for non blocking sleep
#include <stdbool.h>

#include "matrix.h"

#define CHECK_INTERVAL 1000

int init_ui();
void cleanup_ui();
void show_matrix(ViewPort* vp);
void init_colors();
bool non_blocking_sleep(int interval, bool(*callback)(void* arg), void* arg);

#endif
