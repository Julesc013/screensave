#include <stddef.h>

#include "rng_internal.h"

#define SCREENSAVE_RNG_DEFAULT_SEED 0x13579BDFUL

void screensave_rng_seed(screensave_rng_state *state, unsigned long seed)
{
    if (state == NULL) {
        return;
    }

    state->state = seed != 0UL ? seed : SCREENSAVE_RNG_DEFAULT_SEED;
}

unsigned long screensave_rng_next_u32(screensave_rng_state *state)
{
    if (state == NULL) {
        return SCREENSAVE_RNG_DEFAULT_SEED;
    }

    state->state = (state->state * 1664525UL) + 1013904223UL;
    return state->state;
}

int screensave_rng_next_range(screensave_rng_state *state, int upper_bound)
{
    unsigned long value;

    if (upper_bound <= 0) {
        return 0;
    }

    value = screensave_rng_next_u32(state);
    return (int)(value % (unsigned long)upper_bound);
}

unsigned long screensave_rng_seed_from_text(const char *text, unsigned long fallback_seed)
{
    unsigned long seed;

    seed = fallback_seed != 0UL ? fallback_seed : 2166136261UL;
    if (text == NULL) {
        return seed;
    }

    while (*text != '\0') {
        seed ^= (unsigned long)(unsigned char)*text;
        seed *= 16777619UL;
        ++text;
    }

    if (seed == 0UL) {
        seed = SCREENSAVE_RNG_DEFAULT_SEED;
    }

    return seed;
}
