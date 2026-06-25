/*
 * Compiled Nocturne canary runner for ScreenSave Proof Kernel v0.
 *
 * This is a proof tool, not a saver runtime ABI. It mirrors the fixed
 * Nocturne observatory_night canary used by sslab.py and writes an ASCII PPM
 * capture through the private RGBA8 surface and software renderer.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "screensave/private/soft_renderer.h"

#define RUNNER_FIXED_ONE 65536L
#define RUNNER_FIXED_HALF 32768L

#define RUNNER_MOTION_MONOLITH 3
#define RUNNER_FADE_GENTLE 2
#define RUNNER_STRENGTH_SUBTLE 1

typedef struct runner_rng_state_tag {
    unsigned long state;
} runner_rng_state;

typedef struct runner_session_tag {
    int width;
    int height;
    unsigned long seed;
    runner_rng_state rng;
    unsigned long cycle_index;
    unsigned long cycle_duration_millis;
    unsigned long stage_elapsed_millis;
    unsigned long reseed_count;
    int stage;
    int fade_level;
    long primary_x;
    long primary_y;
    long primary_vx;
    long primary_vy;
    long secondary_x;
    long secondary_y;
    long secondary_vx;
    long secondary_vy;
    int breath_direction;
    int breath_level;
    int ambient_level;
} runner_session;

static void runner_rng_seed(runner_rng_state *state, unsigned long seed)
{
    if (state == NULL) {
        return;
    }
    state->state = seed != 0UL ? seed : 0x0A1E0A1EUL;
}

static unsigned long runner_rng_next(runner_rng_state *state)
{
    if (state == NULL) {
        return 0UL;
    }
    state->state = state->state * 1664525UL + 1013904223UL;
    return state->state;
}

static unsigned long runner_rng_range(runner_rng_state *state, unsigned long upper_bound)
{
    if (upper_bound == 0UL) {
        return 0UL;
    }
    return runner_rng_next(state) % upper_bound;
}

static long runner_random_velocity(runner_rng_state *rng)
{
    long speed;

    speed = 300L;
    if ((runner_rng_next(rng) & 1UL) != 0UL) {
        return speed;
    }
    return -speed;
}

static void runner_set_initial_positions(runner_session *session)
{
    if (session == NULL) {
        return;
    }

    session->primary_x =
        RUNNER_FIXED_HALF +
        (long)runner_rng_range(&session->rng, (unsigned long)((session->width > 1) ? (session->width - 1) : 1)) *
            RUNNER_FIXED_ONE;
    session->primary_y =
        RUNNER_FIXED_HALF +
        (long)runner_rng_range(&session->rng, (unsigned long)((session->height > 1) ? (session->height - 1) : 1)) *
            RUNNER_FIXED_ONE;
    session->secondary_x =
        RUNNER_FIXED_HALF +
        (long)runner_rng_range(&session->rng, (unsigned long)((session->width > 1) ? (session->width - 1) : 1)) *
            RUNNER_FIXED_ONE;
    session->secondary_y =
        RUNNER_FIXED_HALF +
        (long)runner_rng_range(&session->rng, (unsigned long)((session->height > 1) ? (session->height - 1) : 1)) *
            RUNNER_FIXED_ONE;

    if (session->width <= 1) {
        session->primary_x = ((long)session->width * RUNNER_FIXED_ONE) / 2L;
        session->secondary_x = session->primary_x;
    }
    if (session->height <= 1) {
        session->primary_y = ((long)session->height * RUNNER_FIXED_ONE) / 2L;
        session->secondary_y = session->primary_y;
    }
}

static void runner_reset_cycle(runner_session *session)
{
    if (session == NULL) {
        return;
    }

    session->cycle_index += 1UL;
    session->stage = 0;
    session->stage_elapsed_millis = 0UL;
    session->fade_level = 0;
    session->cycle_duration_millis = 70000UL;
    session->primary_vx = runner_random_velocity(&session->rng);
    session->primary_vy = runner_random_velocity(&session->rng);
    session->secondary_vx = runner_random_velocity(&session->rng);
    session->secondary_vy = runner_random_velocity(&session->rng);
    session->breath_direction = 1;
    session->breath_level = 48 + (int)runner_rng_range(&session->rng, 48UL);
    session->ambient_level = 28 + (int)runner_rng_range(&session->rng, 28UL);
    runner_set_initial_positions(session);
}

static void runner_make_session(runner_session *session, int width, int height, unsigned long seed)
{
    if (session == NULL) {
        return;
    }

    memset(session, 0, sizeof(*session));
    session->width = width;
    session->height = height;
    session->seed = seed;
    runner_rng_seed(&session->rng, (seed ^ 0x0A1E0A1EUL));
    runner_reset_cycle(session);
}

static void runner_advance_axis(long *position, long *velocity, int limit, unsigned long delta_millis)
{
    long minimum;
    long maximum;

    if (position == NULL || velocity == NULL) {
        return;
    }

    minimum = RUNNER_FIXED_HALF;
    maximum = (long)((limit > 1 ? limit - 1 : 1) * RUNNER_FIXED_ONE) - RUNNER_FIXED_HALF;
    *position += *velocity * (long)delta_millis;
    if (*position < minimum) {
        *position = minimum;
        *velocity = -*velocity;
    } else if (*position > maximum) {
        *position = maximum;
        *velocity = -*velocity;
    }
}

static void runner_step_session(runner_session *session, unsigned long delta_millis)
{
    unsigned long fade_delta;

    if (session == NULL) {
        return;
    }
    if (delta_millis > 200UL) {
        delta_millis = 200UL;
    }

    runner_advance_axis(&session->primary_x, &session->primary_vx, session->width, delta_millis);
    runner_advance_axis(&session->primary_y, &session->primary_vy, session->height, delta_millis);
    runner_advance_axis(&session->secondary_x, &session->secondary_vx, session->width, delta_millis);
    runner_advance_axis(&session->secondary_y, &session->secondary_vy, session->height, delta_millis);

    fade_delta = (delta_millis * 72UL) / 1000UL;
    if (fade_delta == 0UL && delta_millis > 0UL) {
        fade_delta = 1UL;
    }

    session->stage_elapsed_millis += delta_millis;
    if (session->stage == 0) {
        session->fade_level += (int)fade_delta;
        if (session->fade_level >= 255) {
            session->fade_level = 255;
            session->stage = 1;
            session->stage_elapsed_millis = 0UL;
        }
    } else if (session->stage == 1) {
        if (session->stage_elapsed_millis >= session->cycle_duration_millis) {
            session->stage = 2;
            session->stage_elapsed_millis = 0UL;
        }
    } else {
        session->fade_level -= (int)fade_delta;
        if (session->fade_level <= 0) {
            session->fade_level = 0;
            session->reseed_count += 1UL;
            runner_reset_cycle(session);
        }
    }
}

static screensave_color runner_color(unsigned char red, unsigned char green, unsigned char blue)
{
    screensave_color color;

    color.red = red;
    color.green = green;
    color.blue = blue;
    color.alpha = 255U;
    return color;
}

static screensave_color runner_scale_color(screensave_color color, int fade_level)
{
    unsigned int scale;

    scale = (unsigned int)fade_level;
    color.red = (unsigned char)(((unsigned int)color.red * scale) / 255U);
    color.green = (unsigned char)(((unsigned int)color.green * scale) / 255U);
    color.blue = (unsigned char)(((unsigned int)color.blue * scale) / 255U);
    return color;
}

static screensave_color runner_scale_color_amount(screensave_color color, unsigned int scale)
{
    if (scale > 255U) {
        scale = 255U;
    }
    color.red = (unsigned char)(((unsigned int)color.red * scale) / 255U);
    color.green = (unsigned char)(((unsigned int)color.green * scale) / 255U);
    color.blue = (unsigned char)(((unsigned int)color.blue * scale) / 255U);
    return color;
}

static int runner_fixed_to_int(long value)
{
    return (int)(value / RUNNER_FIXED_ONE);
}

static void runner_render_monolith_at(
    screensave_rgba8_surface *surface,
    int center_x,
    int center_y,
    int width,
    int height,
    screensave_color primary,
    screensave_color accent
)
{
    screensave_recti rect;

    rect.x = center_x - (width / 2);
    rect.y = center_y - (height / 2);
    rect.width = width;
    rect.height = height;
    screensave_soft_fill_rect(surface, &rect, primary);
    screensave_soft_draw_frame_rect(surface, &rect, accent);
}

static void runner_render_session(runner_session *session, screensave_rgba8_surface *surface)
{
    screensave_color background;
    screensave_color primary;
    screensave_color accent;
    screensave_color ghost_fill;
    screensave_color ghost_outline;
    screensave_recti frame_rect;
    int width;
    int height;

    if (session == NULL || surface == NULL) {
        return;
    }

    background = runner_color(0U, 0U, 0U);
    screensave_rgba8_surface_clear(surface, background);

    primary = runner_scale_color(runner_color(8U, 8U, 8U), session->fade_level);
    accent = runner_scale_color(runner_color(20U, 20U, 20U), session->fade_level);

    width = 6;
    height = session->height / 3;
    if (height < 12) {
        height = 12;
    }

    ghost_fill = runner_scale_color_amount(primary, (unsigned int)(28 + session->ambient_level));
    ghost_outline = runner_scale_color_amount(accent, (unsigned int)(52 + session->ambient_level));
    runner_render_monolith_at(
        surface,
        runner_fixed_to_int(session->secondary_x),
        runner_fixed_to_int(session->secondary_y),
        width,
        (height * 9) / 10,
        ghost_fill,
        ghost_outline
    );
    runner_render_monolith_at(
        surface,
        runner_fixed_to_int(session->primary_x),
        runner_fixed_to_int(session->primary_y),
        width,
        height,
        primary,
        accent
    );

    frame_rect.x = 6;
    frame_rect.y = 6;
    frame_rect.width = session->width - 12;
    frame_rect.height = session->height - 12;
    if (frame_rect.width > 0 && frame_rect.height > 0) {
        screensave_soft_draw_frame_rect(
            surface,
            &frame_rect,
            runner_scale_color_amount(accent, (unsigned int)(18 + session->ambient_level))
        );
    }
}

static int runner_write_ppm(const screensave_rgba8_surface *surface, const char *path)
{
    FILE *file;
    int x;
    int y;
    const unsigned char *pixel;

    if (surface == NULL || surface->pixels == NULL || path == NULL) {
        return 0;
    }

    file = fopen(path, "w");
    if (file == NULL) {
        return 0;
    }

    fprintf(file, "P3\n%d %d\n255\n", surface->width, surface->height);
    for (y = 0; y < surface->height; ++y) {
        for (x = 0; x < surface->width; ++x) {
            pixel = surface->pixels + (y * surface->stride_bytes) + (x * 4);
            if (x > 0) {
                fprintf(file, " ");
            }
            fprintf(file, "%u %u %u", (unsigned int)pixel[0], (unsigned int)pixel[1], (unsigned int)pixel[2]);
        }
        fprintf(file, "\n");
    }

    fclose(file);
    return 1;
}

static int runner_parse_int_arg(const char *text, int *value_out)
{
    char *end_ptr;
    long value;

    if (text == NULL || value_out == NULL) {
        return 0;
    }

    end_ptr = NULL;
    value = strtol(text, &end_ptr, 10);
    if (end_ptr == text || *end_ptr != '\0' || value < 1L || value > 1000000L) {
        return 0;
    }
    *value_out = (int)value;
    return 1;
}

static int runner_parse_ulong_arg(const char *text, unsigned long *value_out)
{
    char *end_ptr;
    unsigned long value;

    if (text == NULL || value_out == NULL) {
        return 0;
    }

    end_ptr = NULL;
    value = strtoul(text, &end_ptr, 10);
    if (end_ptr == text || *end_ptr != '\0') {
        return 0;
    }
    *value_out = value;
    return 1;
}

int main(int argc, char **argv)
{
    int width;
    int height;
    int frames;
    int index;
    unsigned long seed;
    unsigned long delta_ms;
    const char *output_path;
    runner_session session;
    screensave_rgba8_surface surface;

    width = 96;
    height = 54;
    frames = 8;
    seed = 1536UL;
    delta_ms = 100UL;
    output_path = "capture.ppm";

    for (index = 1; index < argc; ++index) {
        if (strcmp(argv[index], "--width") == 0 && index + 1 < argc) {
            ++index;
            if (!runner_parse_int_arg(argv[index], &width)) {
                return 2;
            }
        } else if (strcmp(argv[index], "--height") == 0 && index + 1 < argc) {
            ++index;
            if (!runner_parse_int_arg(argv[index], &height)) {
                return 2;
            }
        } else if (strcmp(argv[index], "--frames") == 0 && index + 1 < argc) {
            ++index;
            if (!runner_parse_int_arg(argv[index], &frames)) {
                return 2;
            }
        } else if (strcmp(argv[index], "--seed") == 0 && index + 1 < argc) {
            ++index;
            if (!runner_parse_ulong_arg(argv[index], &seed)) {
                return 2;
            }
        } else if (strcmp(argv[index], "--delta-ms") == 0 && index + 1 < argc) {
            ++index;
            if (!runner_parse_ulong_arg(argv[index], &delta_ms)) {
                return 2;
            }
        } else if (strcmp(argv[index], "--output") == 0 && index + 1 < argc) {
            ++index;
            output_path = argv[index];
        } else {
            fprintf(stderr, "unknown or incomplete argument: %s\n", argv[index]);
            return 2;
        }
    }

    if (!screensave_rgba8_surface_init(&surface, width, height)) {
        fprintf(stderr, "could not allocate rgba8 surface\n");
        return 1;
    }

    runner_make_session(&session, width, height, seed);
    for (index = 0; index < frames; ++index) {
        runner_step_session(&session, delta_ms);
    }
    runner_render_session(&session, &surface);

    if (!runner_write_ppm(&surface, output_path)) {
        screensave_rgba8_surface_dispose(&surface);
        fprintf(stderr, "could not write output capture\n");
        return 1;
    }

    printf(
        "compiled-nocturne width=%d height=%d seed=%lu frames=%d delta_ms=%lu checksum=%lu output=%s\n",
        width,
        height,
        seed,
        frames,
        delta_ms,
        screensave_rgba8_surface_checksum(&surface),
        output_path
    );
    screensave_rgba8_surface_dispose(&surface);
    return 0;
}
