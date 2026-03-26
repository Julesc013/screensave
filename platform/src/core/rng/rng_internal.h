#ifndef SCREENSAVE_RNG_INTERNAL_H
#define SCREENSAVE_RNG_INTERNAL_H

typedef struct screensave_rng_state_tag {
    unsigned long state;
} screensave_rng_state;

void screensave_rng_seed(screensave_rng_state *state, unsigned long seed);
unsigned long screensave_rng_next_u32(screensave_rng_state *state);
int screensave_rng_next_range(screensave_rng_state *state, int upper_bound);
unsigned long screensave_rng_seed_from_text(const char *text, unsigned long fallback_seed);

#endif /* SCREENSAVE_RNG_INTERNAL_H */
