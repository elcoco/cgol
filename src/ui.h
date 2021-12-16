#ifndef UI_H
#define UI_H

#include <curses.h>
#include <sys/time.h>   // for non blocking sleep

#include "matrix.h"

#define CHECK_INTERVAL 10000

int init_ui();
void cleanup_ui();
void show_matrix(ViewPort* vp);
void init_colors();
void non_blocking_sleep(int interval, int(*callback)(void* arg), void* arg);

#endif
