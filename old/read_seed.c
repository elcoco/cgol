
int read_seed(Node** nodes, char* path, int xlim, int ylim) {
    /* Read seed from file, do some parsing of cell plaintext format.
     * Load results into nodes array.
     * TODO !!! this is a bit of a discustingly large function and needs splitting up.
     */
    FILE *fp = fopen(path, "r");
    char c;

    // offset from seed in matrix
    int x_offset;
    int y_offset;

    // keep track of location in text file
    int x = 0;
    int y = 0;

    if (fp == NULL) {
        printf("File not found!\n");
        return -1;
    }

    // count seed dimensions, used for calculating matrix offset
    char buf[5000];
    int max_xlen = 0;
    int max_ylen = 0;
    while (fgets(buf, 5000, fp) != NULL ) {

        if (buf[0] == '!') {
            continue;
        }

        // remove newline
        int xlen = strlen(buf)-2;
        if (xlen > max_xlen) {
            max_xlen = xlen;
        }

        max_ylen++;
    }
    printf("max_x/max_y: %d, %d\n", max_xlen, max_ylen);

    x_offset = (xlim-max_xlen) /2;
    y_offset = (ylim-max_ylen) /2;

    rewind(fp);

    // read file char by char
    while ((c = fgetc(fp)) != EOF ) {

        // if line is a comment, fast forward
        if (c == '!') {
            while (c != '\n') {
                c = fgetc(fp);
            }
            continue;
        }

        // replace newline with 0 terminator
        if (c == '\n') {
            y++;
            x = 0;
            printf("\n");
        }

        else {
            // if character is read, find corresponding location in matrix
            if (c == 'O') {
                int loc = get_loc(xlim, ylim, 0, x+x_offset, y+y_offset);
                Node* n = *(nodes+loc);
                n->state = 1;

                printf(">> seed max_x: %d\n", xlim);
                printf(">> seed max_y: %d\n", ylim);
                printf(">> x_offset: %d\n", x_offset);
                printf(">> y_offset: %d\n", y_offset);
                printf(">> [%c] x=%d, y=%d, loc=%d\n", c, x, y, loc);
            }

            printf("read: %c [%d,%d]\n", c, x, y);
            x++;
        }
    }
    fclose(fp);
    return 0;
}
