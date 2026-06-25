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

static int runner_write_lifecycle_json(
    const char *path,
    int width,
    int height,
    int resize_width,
    int resize_height,
    int resize_session,
    unsigned long frames,
    unsigned long create_destroy_cycles,
    unsigned long checksum,
    int destroy_session
)
{
    FILE *file;

    if (path == NULL) {
        return 1;
    }

    file = fopen(path, "w");
    if (file == NULL) {
        return 0;
    }

    fprintf(file, "{\n");
    fprintf(file, "  \"lifecycle_schema\": \"screensave-product-lifecycle-v0\",\n");
    fprintf(file, "  \"product\": \"ricochet\",\n");
    fprintf(file, "  \"status\": \"pass\",\n");
    fprintf(file, "  \"create_session\": true,\n");
    fprintf(file, "  \"resize_session\": %s,\n", resize_session ? "true" : "false");
    fprintf(file, "  \"step_count\": %lu,\n", frames);
    fprintf(file, "  \"render_session\": true,\n");
    fprintf(file, "  \"destroy_session\": %s,\n", destroy_session ? "true" : "false");
    fprintf(file, "  \"create_destroy_cycles\": %lu,\n", create_destroy_cycles);
    fprintf(file, "  \"width\": %d,\n", width);
    fprintf(file, "  \"height\": %d,\n", height);
    fprintf(file, "  \"resize_width\": %d,\n", resize_width);
    fprintf(file, "  \"resize_height\": %d,\n", resize_height);
    fprintf(file, "  \"checksum\": %lu\n", checksum);
    fprintf(file, "}\n");

    fclose(file);
    return 1;
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

static int runner_run_lifecycle_cycles(
    int width,
    int height,
    int resize_width,
    int resize_height,
    int exercise_resize,
    unsigned long seed,
    unsigned long delta_ms,
    unsigned long frames,
    unsigned long create_destroy_cycles,
    unsigned long *checksum_out
)
{
    unsigned long cycle_index;
    unsigned long frame_index;
    unsigned long elapsed_ms;
    unsigned long checksum;
    screensave_rgba8_surface surface;
    screensave_renderer renderer;
    screensave_saver_environment environment;
    screensave_config_binding binding;
    screensave_common_config common_config;
    ricochet_config product_config;
    screensave_saver_session *session;

    if (checksum_out == NULL || create_destroy_cycles == 0UL) {
        return 0;
    }

    checksum = 0UL;
    for (cycle_index = 0UL; cycle_index < create_destroy_cycles; ++cycle_index) {
        session = NULL;
        if (!screensave_rgba8_surface_init(&surface, width, height)) {
            return 0;
        }
        sslab_rgba8_renderer_init(&renderer, &surface);
        runner_make_environment(&environment, &binding, &common_config, &product_config, &renderer, width, height, seed);

        if (!ricochet_create_session(NULL, &session, &environment)) {
            screensave_rgba8_surface_dispose(&surface);
            return 0;
        }

        if (exercise_resize) {
            screensave_rgba8_surface_dispose(&surface);
            if (!screensave_rgba8_surface_init(&surface, resize_width, resize_height)) {
                ricochet_destroy_session(session);
                return 0;
            }
            sslab_rgba8_renderer_init(&renderer, &surface);
            environment.drawable_size.width = resize_width;
            environment.drawable_size.height = resize_height;
            ricochet_resize_session(session, &environment);
        }

        elapsed_ms = 0UL;
        for (frame_index = 0UL; frame_index < frames; ++frame_index) {
            elapsed_ms += delta_ms;
            environment.clock.delta_millis = delta_ms;
            environment.clock.elapsed_millis = elapsed_ms;
            environment.clock.frame_index = frame_index;
            ricochet_step_session(session, &environment);
        }
        ricochet_render_session(session, &environment);
        checksum ^= screensave_rgba8_surface_checksum(&surface) + (cycle_index * 2654435761UL);
        ricochet_destroy_session(session);
        screensave_rgba8_surface_dispose(&surface);
    }

    *checksum_out = checksum;
    return 1;
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
    const char *lifecycle_output_path;
    const char *capture_frame_text;
    unsigned long capture_frames[RICOCHET_RUNNER_MAX_CAPTURE_FRAMES];
    unsigned int capture_frame_count;
    unsigned long max_frame;
    unsigned long frame_index;
    unsigned long elapsed_ms;
    unsigned long lifecycle_checksum;
    unsigned long create_destroy_cycles;
    int index;
    int exercise_resize;
    int resize_width;
    int resize_height;
    int original_width;
    int original_height;
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
    lifecycle_output_path = NULL;
    capture_frame_text = "0,4,8,32";
    capture_frame_count = 0U;
    lifecycle_checksum = 0UL;
    create_destroy_cycles = 1UL;
    exercise_resize = 0;
    resize_width = 0;
    resize_height = 0;
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
        } else if (strcmp(argv[index], "--lifecycle-output") == 0 && index + 1 < argc) {
            ++index;
            lifecycle_output_path = argv[index];
        } else if (strcmp(argv[index], "--exercise-resize") == 0) {
            exercise_resize = 1;
        } else if (strcmp(argv[index], "--resize-width") == 0 && index + 1 < argc) {
            ++index;
            if (!runner_parse_int_arg(argv[index], &resize_width)) {
                return 2;
            }
        } else if (strcmp(argv[index], "--resize-height") == 0 && index + 1 < argc) {
            ++index;
            if (!runner_parse_int_arg(argv[index], &resize_height)) {
                return 2;
            }
        } else if (strcmp(argv[index], "--create-destroy-cycles") == 0 && index + 1 < argc) {
            ++index;
            if (!runner_parse_ulong_arg(argv[index], &create_destroy_cycles)) {
                return 2;
            }
        } else {
            fprintf(stderr, "unknown or incomplete argument: %s\n", argv[index]);
            return 2;
        }
    }

    if (resize_width <= 0) {
        resize_width = width;
    }
    if (resize_height <= 0) {
        resize_height = height;
    }
    original_width = width;
    original_height = height;
    if (create_destroy_cycles == 0UL) {
        fprintf(stderr, "invalid create/destroy cycle count\n");
        return 2;
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

    if (exercise_resize) {
        screensave_rgba8_surface_dispose(&surface);
        if (!screensave_rgba8_surface_init(&surface, resize_width, resize_height)) {
            ricochet_destroy_session(session);
            fprintf(stderr, "could not allocate resized rgba8 surface\n");
            return 1;
        }
        sslab_rgba8_renderer_init(&renderer, &surface);
        environment.drawable_size.width = resize_width;
        environment.drawable_size.height = resize_height;
        width = resize_width;
        height = resize_height;
        ricochet_resize_session(session, &environment);
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
    if (lifecycle_output_path != NULL) {
        if (!runner_run_lifecycle_cycles(
                original_width,
                original_height,
                resize_width,
                resize_height,
                exercise_resize,
                seed,
                delta_ms,
                max_frame,
                create_destroy_cycles,
                &lifecycle_checksum
            )) {
            fprintf(stderr, "could not run Ricochet lifecycle cycles\n");
            return 1;
        }
        if (!runner_write_lifecycle_json(
                lifecycle_output_path,
                original_width,
                original_height,
                resize_width,
                resize_height,
                exercise_resize,
                max_frame,
                create_destroy_cycles,
                lifecycle_checksum,
                1
            )) {
            fprintf(stderr, "could not write lifecycle output\n");
            return 1;
        }
    }
    return 0;
}
