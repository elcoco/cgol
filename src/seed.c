#include "seed.h"


int read_file(Seed* self, char* path) {
    /* Read file contents into seed struct.
     * Count seed dimensions while we're at it. */

    char buf[5000];
    int y = 0;
    FILE *fp = fopen(path, "r");

    if (fp == NULL) {
        printf("File not found!\n");
        return -1;
    }

    while (fgets(buf, 5000, fp) != NULL ) {

        // skip comments
        if (buf[0] == '!') {
            continue;
        }

        // count max x stringlen (used for calculating matrix offset) and record in struct
        int len = strlen(buf)-2; // why 2?, \n is just one char ffs!
        if (len > self->seed_x) {
            self->seed_x = len;
        }

        *(self->data+y) = strndup(buf, strlen(buf)-1);
        y++;
        (self->seed_y)++;
    }

    fclose(fp);
    return 0;
}

bool fast_forward(char** c, char* search_lst, char* expected_lst, char* ignore_lst, char* buf) {
    /* fast forward until a char from search_lst is found
     * Save all chars in buf until a char from search_lst is found
     * Only save in buf when a char is found in expected_lst
     *
     * If buf == NULL,          don't save chars
     * If expected_lst == NULL, allow all characters
     */

    // save skipped chars that are on expected_lst in buffer
    char* ptr = buf;

    // don't return these chars with buffer
    ignore_lst = (ignore_lst) ? ignore_lst : "";

    while (!strchr(search_lst, **c)) {
        //printf("fast forwarding: %c\n", **c);

        if (buf != NULL) {
            if (!strchr(ignore_lst, **c) && expected_lst == NULL)
                *ptr++ = **c;
            else if (!strchr(ignore_lst, **c) && strchr(expected_lst, **c))
                *ptr++ = **c;
            else
                return false;
        }
        (*c)++;
    }

    // terminate string
    if (ptr != NULL)
        *ptr = '\0';

    return true;
}

void RLE_to_str(int amount, char type, char* buf) {
    /* Convert RLE notation to a string representation */
    for (int i=0 ; i<amount ; i++)
        strcat(buf, (type == 'o') ? "O" : " ");
}

int read_rle(Seed* self, char* path) {
    char sbuf[5000];            // source buffer
    char lbuf[500] = {'\0'};    // line buffer
    char* spos = sbuf;          // source position pointer
    char** dpos = self->data;   // destination position pointer
    char c;

    FILE *fp = fopen(path, "r");
    if (fp == NULL) {
        printf("File not found!\n");
        return -1;
    }

    // read all chars in buffer
    while ((c = fgetc(fp)) != EOF )
        *spos++ = c;
    *spos = '\0';

    // reset position in source buffer
    spos = sbuf;

    // parse RLE char by char
    for (int i=0 ; i<strlen(sbuf) ; i++, spos++) {

        if (*spos == '#') {
            char comment[500] = "";
            fast_forward(&spos, "\n", NULL, "\n", comment);
            printf("Ignoring comment: %s\n", comment);
        }

        else if (strchr("x", *spos)) {
            char header[500] = "";
            fast_forward(&spos, "\n", NULL, "\n", header);
            printf("Ignoring header: %s\n", header);
        }

        // is multiple dead/alive cells or newlines
        else if (strchr("0123456789", *spos)) {
            char amount[10] = "";
            if (!fast_forward(&spos, "ob$", "01234567890", "\n", amount)) {
                printf("RLE syntax error, found unexpected chars: %s + %c\n", amount, *spos);
                return -1;
            }

            if (*spos == '$') {
                // if char == $: this means amount*newline
                if (strlen(lbuf) > 0) {
                    *dpos++ = strdup(lbuf);
                    self->seed_x = (strlen(lbuf) > self->seed_x) ? strlen(lbuf) : self->seed_x;
                    self->seed_y++;
                }

                for (int newline=0 ; newline<atoi(amount)-1 ; newline++, self->seed_y++)
                    *dpos++ = strdup("");
                lbuf[0] = '\0';
            } 
            else {
                // if char == o|b: this means amount*(alive|dead)
                RLE_to_str(atoi(amount), *spos, lbuf);
            }
        }

        // is 1 dead or alive cell
        else if (strchr("ob", *spos)) {
            RLE_to_str(1, *spos, lbuf);
        }

        // is EOL or EOF
        else if (strchr("$!", *spos)) {
            *dpos++ = strdup(lbuf);
            self->seed_x = (strlen(lbuf) > self->seed_x) ? strlen(lbuf) : self->seed_x;
            self->seed_y++;
            lbuf[0] = '\0';
            if (*spos == '!')
                break;
        }
    }
    self->print_seed(self);
    printf("seed x/y: %d, %d\n", self->seed_x, self->seed_y);
    return 0;
}

void print_seed(Seed* self) {
    /* print seed for debugging */
    char** ptr = self->data;

    for (int i=0 ; i<self->seed_y ; i++, ptr++) {
        printf("[%d] %s\n", i, *ptr);
    }
}

int to_matrix(Seed* self, Matrix* m) {
    /* copy data from seed file into matrix */
    char** dptr = self->data;
    Node** nodes = m->nodes;

    // this will truncate in the right way
    int xmin = 0 - (self->seed_x / 2);
    int ymin = 0 - (self->seed_y / 2);
    int xmax = xmin + self->seed_x;
    int ymax = ymin + self->seed_y;

    int i = 0;

    for (int y=ymin ; y<ymax ; y++, dptr++) {
        char* c = *dptr;

        for (int x=xmin ; x<=xmax ; x++, c++) {
            // only allowed char is O!
            if (*c != 'O')
                continue;
            i++;

            // Translate coordinates to index in nodes array
            int loc = get_index(self->term_x, self->term_y, x, y, m->edge_policy);
            printf("[%c]\t%3d x %3d\tloc=%d\n", *c, x, y, loc);
            Node* n = *(nodes+loc);
            n->state = 1;

            // add node to linked list, which is used for faster iteration inbetween generations
            m->insert_alive_node(m, n);
        } 
    }
    printf("Finished putting seed in matrix, enabled %d nodes.\n", i);
    return 0;
}

int get_rand(int lower, int upper) {
    return (rand() % (upper - lower + 1)) + lower;
}

void read_random(Seed* self) {
    /*
    char** c = self->data;
    
    for (int i=0 ; i<(self->term_x*self->term_y) ; i++, c++) {
        c = (get_rand(0, 1)) ? 'O' : '.';
        //printf("bver %c\n", *c);
    }
    */

    
}

Seed* init_seed(int term_x, int term_y) {
    /* Create seed struct and connect all the func pointers */
    Seed* seed = (Seed*)malloc(sizeof(Seed));

    seed->read_file = &read_file;
    seed->read_rle = &read_rle;
    seed->to_matrix = &to_matrix;
    seed->print_seed = &print_seed;
    seed->read_random = &read_random;

    seed->seed_x = 0;
    seed->seed_y = 0;
    seed->term_x = term_x;
    seed->term_y = term_y;
    seed->data = (char**)malloc(DATA_SIZE*sizeof(char*));
    return seed;
}
