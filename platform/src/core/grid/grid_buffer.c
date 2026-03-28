#include <stdlib.h>
#include <string.h>

#include "screensave/grid_buffer_api.h"

static int screensave_byte_grid_valid_size(const screensave_sizei *size)
{
    return size != NULL && size->width > 0 && size->height > 0;
}

int screensave_byte_grid_init(
    screensave_byte_grid *grid,
    const screensave_sizei *size
)
{
    size_t cell_count;

    if (grid == NULL || !screensave_byte_grid_valid_size(size)) {
        return 0;
    }

    memset(grid, 0, sizeof(*grid));
    grid->size = *size;
    grid->stride_cells = size->width;
    cell_count = (size_t)size->width * (size_t)size->height;
    grid->cells = (unsigned char *)malloc(cell_count);
    if (grid->cells == NULL) {
        memset(grid, 0, sizeof(*grid));
        return 0;
    }

    memset(grid->cells, 0, cell_count);
    return 1;
}

int screensave_byte_grid_resize(
    screensave_byte_grid *grid,
    const screensave_sizei *size
)
{
    size_t cell_count;
    unsigned char *cells;

    if (grid == NULL || !screensave_byte_grid_valid_size(size)) {
        return 0;
    }

    if (
        grid->cells != NULL &&
        grid->size.width == size->width &&
        grid->size.height == size->height
    ) {
        return 1;
    }

    cell_count = (size_t)size->width * (size_t)size->height;
    cells = (unsigned char *)malloc(cell_count);
    if (cells == NULL) {
        return 0;
    }

    memset(cells, 0, cell_count);
    free(grid->cells);
    grid->cells = cells;
    grid->size = *size;
    grid->stride_cells = size->width;
    return 1;
}

void screensave_byte_grid_dispose(screensave_byte_grid *grid)
{
    if (grid == NULL) {
        return;
    }

    free(grid->cells);
    memset(grid, 0, sizeof(*grid));
}

void screensave_byte_grid_clear(
    screensave_byte_grid *grid,
    unsigned char value
)
{
    size_t cell_count;

    if (grid == NULL || grid->cells == NULL) {
        return;
    }

    cell_count = (size_t)grid->stride_cells * (size_t)grid->size.height;
    memset(grid->cells, value, cell_count);
}

void screensave_byte_grid_swap(
    screensave_byte_grid *left_grid,
    screensave_byte_grid *right_grid
)
{
    screensave_byte_grid swap_grid;

    if (left_grid == NULL || right_grid == NULL) {
        return;
    }

    swap_grid = *left_grid;
    *left_grid = *right_grid;
    *right_grid = swap_grid;
}

unsigned int screensave_byte_grid_count_nonzero(
    const screensave_byte_grid *grid
)
{
    size_t index;
    size_t cell_count;
    unsigned int count;

    if (grid == NULL || grid->cells == NULL) {
        return 0U;
    }

    cell_count = (size_t)grid->stride_cells * (size_t)grid->size.height;
    count = 0U;
    for (index = 0U; index < cell_count; ++index) {
        if (grid->cells[index] != 0U) {
            count += 1U;
        }
    }

    return count;
}

unsigned long screensave_byte_grid_checksum(
    const screensave_byte_grid *grid
)
{
    size_t index;
    size_t cell_count;
    unsigned long hash;

    if (grid == NULL || grid->cells == NULL) {
        return 0UL;
    }

    cell_count = (size_t)grid->stride_cells * (size_t)grid->size.height;
    hash = 2166136261UL;
    for (index = 0U; index < cell_count; ++index) {
        hash ^= (unsigned long)grid->cells[index];
        hash *= 16777619UL;
    }

    hash ^= (unsigned long)grid->size.width;
    hash *= 16777619UL;
    hash ^= (unsigned long)grid->size.height;
    hash *= 16777619UL;
    return hash;
}
