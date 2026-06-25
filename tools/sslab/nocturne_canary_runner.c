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

#include "nocturne_internal.h"
#include "screensave/private/soft_renderer.h"

struct screensave_renderer_tag {
    screensave_rgba8_surface *surface;
};

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

int screensave_renderer_begin_frame(screensave_renderer *renderer, const screensave_frame_info *frame_info)
{
    (void)renderer;
    (void)frame_info;
    return 1;
}

void screensave_renderer_clear(screensave_renderer *renderer, screensave_color color)
{
    if (renderer == NULL || renderer->surface == NULL) {
        return;
    }

    screensave_rgba8_surface_clear(renderer->surface, color);
}

void screensave_renderer_fill_rect(
    screensave_renderer *renderer,
    const screensave_recti *rect,
    screensave_color color
)
{
    if (renderer == NULL || renderer->surface == NULL) {
        return;
    }

    screensave_soft_fill_rect(renderer->surface, rect, color);
}

void screensave_renderer_draw_frame_rect(
    screensave_renderer *renderer,
    const screensave_recti *rect,
    screensave_color color
)
{
    if (renderer == NULL || renderer->surface == NULL) {
        return;
    }

    screensave_soft_draw_frame_rect(renderer->surface, rect, color);
}

void screensave_renderer_draw_line(
    screensave_renderer *renderer,
    const screensave_pointi *start_point,
    const screensave_pointi *end_point,
    screensave_color color
)
{
    if (renderer == NULL || renderer->surface == NULL) {
        return;
    }

    screensave_soft_draw_line(renderer->surface, start_point, end_point, color);
}

void screensave_renderer_draw_polyline(
    screensave_renderer *renderer,
    const screensave_pointi *points,
    unsigned int point_count,
    screensave_color color
)
{
    if (renderer == NULL || renderer->surface == NULL) {
        return;
    }

    screensave_soft_draw_polyline(renderer->surface, points, point_count, color);
}

int screensave_renderer_blit_bitmap(
    screensave_renderer *renderer,
    const screensave_bitmap_view *bitmap,
    const screensave_recti *destination_rect
)
{
    (void)renderer;
    (void)bitmap;
    (void)destination_rect;
    return 0;
}

int screensave_renderer_end_frame(screensave_renderer *renderer)
{
    (void)renderer;
    return 1;
}

void screensave_renderer_shutdown(screensave_renderer *renderer)
{
    (void)renderer;
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
    const char *output_path;
    const char *preset_key;
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
    preset_key = NOCTURNE_DEFAULT_PRESET_KEY;
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
        } else {
            fprintf(stderr, "unknown or incomplete argument: %s\n", argv[index]);
            return 2;
        }
    }

    if (!screensave_rgba8_surface_init(&surface, width, height)) {
        fprintf(stderr, "could not allocate rgba8 surface\n");
        return 1;
    }

    renderer.surface = &surface;
    runner_make_common_config(&common_config, &product_config, preset_key);
    runner_make_environment(&environment, &binding, &common_config, &product_config, &renderer, width, height, seed);

    if (!nocturne_create_session(NULL, &session, &environment)) {
        screensave_rgba8_surface_dispose(&surface);
        fprintf(stderr, "could not create Nocturne product session\n");
        return 1;
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

    if (!runner_write_ppm(&surface, output_path)) {
        nocturne_destroy_session(session);
        screensave_rgba8_surface_dispose(&surface);
        fprintf(stderr, "could not write output capture\n");
        return 1;
    }

    printf(
        "compiled-nocturne product-session width=%d height=%d seed=%lu frames=%d delta_ms=%lu checksum=%lu output=%s\n",
        width,
        height,
        seed,
        frames,
        delta_ms,
        screensave_rgba8_surface_checksum(&surface),
        output_path
    );

    nocturne_destroy_session(session);
    screensave_rgba8_surface_dispose(&surface);
    return 0;
}
