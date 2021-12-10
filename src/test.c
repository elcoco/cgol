#include <stdint.h>         // uint types
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define LEN 10

typedef struct {
    int val;
} Node;

Node** get_arr(int len) {
    Node** nodes = (Node**)malloc(len*sizeof(Node*));
    Node **ptr = nodes;
    for (int i=0 ; i<len ; i++, ptr++) {
        *ptr = (Node*)malloc(sizeof(Node));
        (*ptr)->val = i;
    }
    return nodes;
}


int main() {
    FILE *fp = fopen("seeds/glider.seed", "r");

    char c;
    int x = 0;
    int y = 0;

    while ((c = fgetc(fp)) != EOF ) {
        // replace newline with 0 terminator

        if (c == '\n') {
            y++;
            x = 0;
            printf("\n");
        }
        else {
            printf("%c %d,%d\n", c, x, y);
            x++;
        }

        // reserve memory and put in array

    }





}
