#ifndef SCREENSAVE_GRID_BUFFER_API_H
#define SCREENSAVE_GRID_BUFFER_API_H

#include "screensave/types.h"

typedef struct screensave_byte_grid_tag {
    screensave_sizei size;
    int stride_cells;
    unsigned char *cells;
} screensave_byte_grid;

int screensave_byte_grid_init(
    screensave_byte_grid *grid,
    const screensave_sizei *size
);
int screensave_byte_grid_resize(
    screensave_byte_grid *grid,
    const screensave_sizei *size
);
void screensave_byte_grid_dispose(screensave_byte_grid *grid);
void screensave_byte_grid_clear(
    screensave_byte_grid *grid,
    unsigned char value
);
void screensave_byte_grid_swap(
    screensave_byte_grid *left_grid,
    screensave_byte_grid *right_grid
);
unsigned int screensave_byte_grid_count_nonzero(
    const screensave_byte_grid *grid
);
unsigned long screensave_byte_grid_checksum(
    const screensave_byte_grid *grid
);

#endif /* SCREENSAVE_GRID_BUFFER_API_H */
