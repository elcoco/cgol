#ifndef USER_INP_H
#define USER_INP_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/select.h>
#include <termios.h>

void restore_term_state();
void set_term_raw();
int has_data();
char get_chr();
int test(int argc, char *argv[]);

#endif
