/*
 * Compiled Nocturne canary runner for ScreenSave Proof Kernel v0.
 *
 * This proof tool drives the real Nocturne product session and render
 * functions through a proof-local RGBA8 renderer shim. It is not a saver
 * runtime ABI and does not publish a portable semantic contract.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "capture.h"
#include "nocturne_internal.h"
#include "renderer_rgba8.h"

void screensave_diag_emit(
    screensave_diag_context *context,
    screensave_diag_level level,
    screensave_diag_domain domain,
    unsigned long code,
    const char *origin,
    const char *text
)
{
    (void)context;
    (void)level;
    (void)domain;
    (void)code;
    (void)origin;
    (void)text;
}

const screensave_preset_descriptor *screensave_find_preset(
    const screensave_preset_descriptor *presets,
    unsigned int preset_count,
    const char *preset_key
)
{
    unsigned int index;

    if (presets == NULL || preset_key == NULL) {
        return NULL;
    }

    for (index = 0U; index < preset_count; ++index) {
        if (presets[index].preset_key != NULL && strcmp(presets[index].preset_key, preset_key) == 0) {
            return &presets[index];
        }
    }

    return NULL;
}

const screensave_theme_descriptor *screensave_find_theme(
    const screensave_theme_descriptor *themes,
    unsigned int theme_count,
    const char *theme_key
)
{
    unsigned int index;

    if (themes == NULL || theme_key == NULL) {
        return NULL;
    }

    for (index = 0U; index < theme_count; ++index) {
        if (themes[index].theme_key != NULL && strcmp(themes[index].theme_key, theme_key) == 0) {
            return &themes[index];
        }
    }

    return NULL;
}

static int runner_write_lifecycle_json(
    const char *path,
    int width,
    int height,
    int resize_session,
    int frames,
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
    fprintf(file, "  \"lifecycle_schema\": \"screensave-nocturne-canary-lifecycle-v0\",\n");
    fprintf(file, "  \"status\": \"pass\",\n");
    fprintf(file, "  \"create_session\": true,\n");
    fprintf(file, "  \"resize_session\": %s,\n", resize_session ? "true" : "false");
    fprintf(file, "  \"step_count\": %d,\n", frames);
    fprintf(file, "  \"render_session\": true,\n");
    fprintf(file, "  \"destroy_session\": %s,\n", destroy_session ? "true" : "false");
    fprintf(file, "  \"width\": %d,\n", width);
    fprintf(file, "  \"height\": %d,\n", height);
    fprintf(file, "  \"checksum\": %lu\n", checksum);
    fprintf(file, "}\n");

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

static void runner_make_common_config(
    screensave_common_config *common_config,
    nocturne_config *product_config,
    const char *preset_key
)
{
    if (common_config == NULL || product_config == NULL) {
        return;
    }

    memset(common_config, 0, sizeof(*common_config));
    memset(product_config, 0, sizeof(*product_config));
    common_config->schema_version = SCREENSAVE_CONFIG_SCHEMA_VERSION;
    common_config->detail_level = SCREENSAVE_DETAIL_LEVEL_STANDARD;
    common_config->preset_key = NOCTURNE_DEFAULT_PRESET_KEY;
    common_config->theme_key = NOCTURNE_DEFAULT_THEME_KEY;
    product_config->motion_mode = NOCTURNE_MOTION_MONOLITH;
    product_config->fade_speed = NOCTURNE_FADE_GENTLE;
    product_config->motion_strength = NOCTURNE_STRENGTH_SUBTLE;
    nocturne_apply_preset_to_config(preset_key, common_config, product_config);
}

static void runner_make_environment(
    screensave_saver_environment *environment,
    screensave_config_binding *binding,
    const screensave_common_config *common_config,
    const nocturne_config *product_config,
    screensave_renderer *renderer,
    int width,
    int height,
    unsigned long seed
)
{
    memset(environment, 0, sizeof(*environment));
    memset(binding, 0, sizeof(*binding));

    binding->common_config = common_config;
    binding->product_config = product_config;
    binding->product_config_size = (unsigned int)sizeof(*product_config);

    environment->mode = SCREENSAVE_SESSION_MODE_SCREEN;
    environment->drawable_size.width = width;
    environment->drawable_size.height = height;
    environment->seed.base_seed = seed;
    environment->seed.stream_seed = 0x0A1E0A1EUL;
    environment->seed.deterministic = 1;
    environment->config_binding = binding;
    environment->renderer = renderer;
    environment->diagnostics = NULL;
}

int main(int argc, char **argv)
{
    int width;
    int height;
    int frames;
    int index;
    unsigned long seed;
    unsigned long delta_ms;
    unsigned long elapsed_ms;
    unsigned long checksum;
    const char *output_path;
    const char *lifecycle_output_path;
    const char *preset_key;
    int exercise_resize;
    int resize_width;
    int resize_height;
    screensave_common_config common_config;
    nocturne_config product_config;
    screensave_config_binding binding;
    screensave_saver_environment environment;
    screensave_saver_session *session;
    screensave_renderer renderer;
    screensave_rgba8_surface surface;

    width = 96;
    height = 54;
    frames = 8;
    seed = 1536UL;
    delta_ms = 100UL;
    output_path = "capture.ppm";
    lifecycle_output_path = NULL;
    preset_key = NOCTURNE_DEFAULT_PRESET_KEY;
    exercise_resize = 0;
    resize_width = 0;
    resize_height = 0;
    session = NULL;

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
        } else if (strcmp(argv[index], "--preset") == 0 && index + 1 < argc) {
            ++index;
            preset_key = argv[index];
        } else if (strcmp(argv[index], "--output") == 0 && index + 1 < argc) {
            ++index;
            output_path = argv[index];
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

    if (!screensave_rgba8_surface_init(&surface, width, height)) {
        fprintf(stderr, "could not allocate rgba8 surface\n");
        return 1;
    }

    sslab_rgba8_renderer_init(&renderer, &surface);
    runner_make_common_config(&common_config, &product_config, preset_key);
    runner_make_environment(&environment, &binding, &common_config, &product_config, &renderer, width, height, seed);

    if (!nocturne_create_session(NULL, &session, &environment)) {
        screensave_rgba8_surface_dispose(&surface);
        fprintf(stderr, "could not create Nocturne product session\n");
        return 1;
    }

    if (exercise_resize) {
        screensave_rgba8_surface_dispose(&surface);
        if (!screensave_rgba8_surface_init(&surface, resize_width, resize_height)) {
            nocturne_destroy_session(session);
            fprintf(stderr, "could not allocate resized rgba8 surface\n");
            return 1;
        }
        sslab_rgba8_renderer_init(&renderer, &surface);
        environment.drawable_size.width = resize_width;
        environment.drawable_size.height = resize_height;
        width = resize_width;
        height = resize_height;
        nocturne_resize_session(session, &environment);
    }

    elapsed_ms = 0UL;
    for (index = 0; index < frames; ++index) {
        elapsed_ms += delta_ms;
        environment.clock.delta_millis = delta_ms;
        environment.clock.elapsed_millis = elapsed_ms;
        environment.clock.frame_index = (unsigned long)index;
        nocturne_step_session(session, &environment);
    }
    nocturne_render_session(session, &environment);

    if (!sslab_write_review_ppm(&surface, output_path)) {
        nocturne_destroy_session(session);
        screensave_rgba8_surface_dispose(&surface);
        fprintf(stderr, "could not write output capture\n");
        return 1;
    }

    checksum = screensave_rgba8_surface_checksum(&surface);
    printf(
        "compiled-nocturne product-session width=%d height=%d seed=%lu frames=%d delta_ms=%lu checksum=%lu output=%s\n",
        width,
        height,
        seed,
        frames,
        delta_ms,
        checksum,
        output_path
    );

    nocturne_destroy_session(session);
    session = NULL;
    if (!runner_write_lifecycle_json(lifecycle_output_path, width, height, exercise_resize, frames, checksum, 1)) {
        screensave_rgba8_surface_dispose(&surface);
        fprintf(stderr, "could not write lifecycle output\n");
        return 1;
    }
    screensave_rgba8_surface_dispose(&surface);
    return 0;
}
