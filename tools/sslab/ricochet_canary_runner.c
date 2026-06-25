/*
 * Compiled Ricochet canary runner for ScreenSave Proof Kernel v1.
 *
 * This proof tool drives the real Ricochet product session and render
 * functions through the proof-local RGBA8 renderer shim. It is current
 * Windows/32-bit v1 proof only, not the portable v2 semantic contract.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "capture.h"
#include "renderer_rgba8.h"
#include "ricochet_internal.h"

#define RICOCHET_RUNNER_MAX_CAPTURE_FRAMES 16

static int runner_parse_int_arg(const char *text, int *value_out)
{
    char *end_ptr;
    long value;

    if (text == NULL || value_out == NULL) {
        return 0;
    }

    value = strtol(text, &end_ptr, 10);
    if (end_ptr == text || *end_ptr != '\0') {
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

    value = strtoul(text, &end_ptr, 10);
    if (end_ptr == text || *end_ptr != '\0') {
        return 0;
    }
    *value_out = value;
    return 1;
}

static int runner_parse_capture_frames(
    const char *text,
    unsigned long *frames,
    unsigned int *frame_count
)
{
    const char *cursor;
    char *end_ptr;
    unsigned long value;
    unsigned int count;

    if (text == NULL || frames == NULL || frame_count == NULL) {
        return 0;
    }

    cursor = text;
    count = 0U;
    while (*cursor != '\0') {
        if (count >= RICOCHET_RUNNER_MAX_CAPTURE_FRAMES) {
            return 0;
        }
        value = strtoul(cursor, &end_ptr, 10);
        if (end_ptr == cursor) {
            return 0;
        }
        frames[count] = value;
        ++count;
        if (*end_ptr == ',') {
            cursor = end_ptr + 1;
        } else if (*end_ptr == '\0') {
            cursor = end_ptr;
        } else {
            return 0;
        }
    }

    *frame_count = count;
    return count > 0U;
}

static int runner_should_capture(
    unsigned long frame_index,
    const unsigned long *frames,
    unsigned int frame_count
)
{
    unsigned int index;

    for (index = 0U; index < frame_count; ++index) {
        if (frames[index] == frame_index) {
            return 1;
        }
    }
    return 0;
}

static unsigned long runner_max_frame(const unsigned long *frames, unsigned int frame_count)
{
    unsigned int index;
    unsigned long max_frame;

    max_frame = 0UL;
    for (index = 0U; index < frame_count; ++index) {
        if (frames[index] > max_frame) {
            max_frame = frames[index];
        }
    }
    return max_frame;
}

static void runner_make_environment(
    screensave_saver_environment *environment,
    screensave_config_binding *binding,
    screensave_common_config *common_config,
    ricochet_config *product_config,
    screensave_renderer *renderer,
    int width,
    int height,
    unsigned long seed
)
{
    screensave_common_config_set_defaults(common_config);
    ricochet_apply_preset_to_config(RICOCHET_DEFAULT_PRESET_KEY, common_config, product_config);
    screensave_config_binding_init(binding, common_config, product_config, sizeof(*product_config));

    memset(environment, 0, sizeof(*environment));
    environment->mode = SCREENSAVE_SESSION_MODE_WINDOWED;
    environment->drawable_size.width = width;
    environment->drawable_size.height = height;
    environment->clock.session_start_millis = 0UL;
    environment->clock.elapsed_millis = 0UL;
    environment->clock.delta_millis = 0UL;
    environment->clock.frame_index = 0UL;
    environment->seed.base_seed = seed;
    environment->seed.stream_seed = seed ^ 0x9E3779B9UL;
    environment->seed.deterministic = 1;
    environment->config_binding = binding;
    environment->renderer = renderer;
    environment->diagnostics = NULL;
}

static int runner_capture_frame(
    screensave_saver_session *session,
    screensave_saver_environment *environment,
    const screensave_rgba8_surface *surface,
    const char *output_dir,
    unsigned long frame_index
)
{
    char ppm_path[512];
    char raw_path[512];

    ricochet_render_session(session, environment);
    sprintf(ppm_path, "%s/frame-%04lu.ppm", output_dir, frame_index);
    sprintf(raw_path, "%s/frame-%04lu.rgba", output_dir, frame_index);
    if (!sslab_write_review_ppm(surface, ppm_path)) {
        return 0;
    }
    if (!sslab_write_raw_rgba(surface, raw_path)) {
        return 0;
    }
    printf("frame=%lu raw=%s ppm=%s\n", frame_index, raw_path, ppm_path);
    return 1;
}

int main(int argc, char **argv)
{
    int width;
    int height;
    unsigned long seed;
    unsigned long delta_ms;
    const char *output_dir;
    const char *capture_frame_text;
    unsigned long capture_frames[RICOCHET_RUNNER_MAX_CAPTURE_FRAMES];
    unsigned int capture_frame_count;
    unsigned long max_frame;
    unsigned long frame_index;
    unsigned long elapsed_ms;
    int index;
    screensave_rgba8_surface surface;
    screensave_renderer renderer;
    screensave_saver_environment environment;
    screensave_config_binding binding;
    screensave_common_config common_config;
    ricochet_config product_config;
    screensave_saver_session *session;

    width = 128;
    height = 72;
    seed = 2048UL;
    delta_ms = 100UL;
    output_dir = ".";
    capture_frame_text = "0,4,8,32";
    capture_frame_count = 0U;
    session = NULL;

    if (sizeof(unsigned long) != 4U) {
        fprintf(stderr, "unsupported ABI width: Ricochet v1 proof requires 32-bit unsigned long\n");
        return 3;
    }

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
        } else if (strcmp(argv[index], "--capture-frames") == 0 && index + 1 < argc) {
            ++index;
            capture_frame_text = argv[index];
        } else if (strcmp(argv[index], "--output-dir") == 0 && index + 1 < argc) {
            ++index;
            output_dir = argv[index];
        } else {
            fprintf(stderr, "unknown or incomplete argument: %s\n", argv[index]);
            return 2;
        }
    }

    if (!runner_parse_capture_frames(capture_frame_text, capture_frames, &capture_frame_count)) {
        fprintf(stderr, "invalid capture frame list\n");
        return 2;
    }

    if (!screensave_rgba8_surface_init(&surface, width, height)) {
        fprintf(stderr, "could not allocate rgba8 surface\n");
        return 1;
    }

    sslab_rgba8_renderer_init(&renderer, &surface);
    runner_make_environment(&environment, &binding, &common_config, &product_config, &renderer, width, height, seed);

    if (!ricochet_create_session(NULL, &session, &environment)) {
        screensave_rgba8_surface_dispose(&surface);
        fprintf(stderr, "could not create Ricochet product session\n");
        return 1;
    }

    max_frame = runner_max_frame(capture_frames, capture_frame_count);
    elapsed_ms = 0UL;
    for (frame_index = 0UL; frame_index <= max_frame; ++frame_index) {
        environment.clock.frame_index = frame_index;
        environment.clock.elapsed_millis = elapsed_ms;
        if (runner_should_capture(frame_index, capture_frames, capture_frame_count)) {
            if (!runner_capture_frame(session, &environment, &surface, output_dir, frame_index)) {
                ricochet_destroy_session(session);
                screensave_rgba8_surface_dispose(&surface);
                fprintf(stderr, "could not write Ricochet capture\n");
                return 1;
            }
        }
        if (frame_index == max_frame) {
            break;
        }
        elapsed_ms += delta_ms;
        environment.clock.delta_millis = delta_ms;
        environment.clock.elapsed_millis = elapsed_ms;
        ricochet_step_session(session, &environment);
    }

    printf(
        "compiled-ricochet product-session width=%d height=%d seed=%lu delta_ms=%lu frames=%s\n",
        width,
        height,
        seed,
        delta_ms,
        capture_frame_text
    );

    ricochet_destroy_session(session);
    screensave_rgba8_surface_dispose(&surface);
    return 0;
}
