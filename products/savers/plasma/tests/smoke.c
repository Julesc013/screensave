#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "screensave/private/renderer_runtime.h"
#include "../src/plasma_internal.h"

#define PLASMA_SMOKE_CAPTURE_LIMIT 32U
#define PLASMA_SMOKE_ADVANCED_COMPONENTS \
    (PLASMA_ADVANCED_COMPONENT_HISTORY_FEEDBACK | \
        PLASMA_ADVANCED_COMPONENT_DOMAIN_WARP | \
        PLASMA_ADVANCED_COMPONENT_FLOW_TURBULENCE | \
        PLASMA_ADVANCED_COMPONENT_GLOW_POST)
#define PLASMA_SMOKE_MODERN_COMPONENTS \
    (PLASMA_MODERN_COMPONENT_REFINED_FIELD | \
        PLASMA_MODERN_COMPONENT_REFINED_FILTER | \
        PLASMA_MODERN_COMPONENT_PRESENTATION_BUFFER)
#define PLASMA_SMOKE_PREMIUM_COMPONENTS \
    (PLASMA_PREMIUM_COMPONENT_EXTENDED_HISTORY | \
        PLASMA_PREMIUM_COMPONENT_POST_CHAIN | \
        PLASMA_PREMIUM_COMPONENT_HEIGHTFIELD_PRESENTATION)

typedef struct plasma_smoke_capture_entry_tag {
    char section[32];
    char key[64];
    char value[256];
} plasma_smoke_capture_entry;

typedef struct plasma_smoke_capture_tag {
    plasma_smoke_capture_entry entries[PLASMA_SMOKE_CAPTURE_LIMIT];
    unsigned int count;
} plasma_smoke_capture;

typedef struct plasma_smoke_render_signature_tag {
    unsigned long treated_hash;
    unsigned long presented_hash;
    unsigned int treated_lit_pixels;
    unsigned int presented_lit_pixels;
    screensave_sizei treated_size;
    int treated_stride_bytes;
    unsigned char *treated_pixels;
    screensave_sizei presented_size;
    int presented_stride_bytes;
    unsigned char *presented_pixels;
} plasma_smoke_render_signature;

static int plasma_smoke_renderer_begin_frame(
    screensave_renderer *renderer,
    const screensave_frame_info *frame_info
)
{
    (void)renderer;
    (void)frame_info;
    return 1;
}

static void plasma_smoke_renderer_clear(screensave_renderer *renderer, screensave_color color)
{
    (void)renderer;
    (void)color;
}

static void plasma_smoke_renderer_fill_rect(
    screensave_renderer *renderer,
    const screensave_recti *rect,
    screensave_color color
)
{
    (void)renderer;
    (void)rect;
    (void)color;
}

static void plasma_smoke_renderer_draw_frame_rect(
    screensave_renderer *renderer,
    const screensave_recti *rect,
    screensave_color color
)
{
    (void)renderer;
    (void)rect;
    (void)color;
}

static void plasma_smoke_renderer_draw_line(
    screensave_renderer *renderer,
    const screensave_pointi *start_point,
    const screensave_pointi *end_point,
    screensave_color color
)
{
    (void)renderer;
    (void)start_point;
    (void)end_point;
    (void)color;
}

static void plasma_smoke_renderer_draw_polyline(
    screensave_renderer *renderer,
    const screensave_pointi *points,
    unsigned int point_count,
    screensave_color color
)
{
    (void)renderer;
    (void)points;
    (void)point_count;
    (void)color;
}

static int plasma_smoke_renderer_blit_bitmap(
    screensave_renderer *renderer,
    const screensave_bitmap_view *bitmap,
    const screensave_recti *destination_rect
)
{
    (void)renderer;
    (void)bitmap;
    (void)destination_rect;
    return 1;
}

static int plasma_smoke_renderer_end_frame(screensave_renderer *renderer)
{
    (void)renderer;
    return 1;
}

static void plasma_smoke_renderer_shutdown(screensave_renderer *renderer)
{
    (void)renderer;
}

static const screensave_renderer_vtable g_plasma_smoke_renderer_vtable = {
    plasma_smoke_renderer_begin_frame,
    plasma_smoke_renderer_clear,
    plasma_smoke_renderer_fill_rect,
    plasma_smoke_renderer_draw_frame_rect,
    plasma_smoke_renderer_draw_line,
    plasma_smoke_renderer_draw_polyline,
    plasma_smoke_renderer_blit_bitmap,
    plasma_smoke_renderer_end_frame,
    plasma_smoke_renderer_shutdown
};

static void plasma_smoke_copy_text(char *buffer, unsigned int buffer_size, const char *text)
{
    if (buffer == NULL || buffer_size == 0U) {
        return;
    }

    buffer[0] = '\0';
    if (text == NULL) {
        return;
    }

    strncpy(buffer, text, (size_t)(buffer_size - 1U));
    buffer[buffer_size - 1U] = '\0';
}

static int plasma_smoke_capture_write_string(
    void *context,
    const char *section,
    const char *key,
    const char *value
)
{
    plasma_smoke_capture *capture;
    plasma_smoke_capture_entry *entry;

    capture = (plasma_smoke_capture *)context;
    if (capture == NULL || capture->count >= PLASMA_SMOKE_CAPTURE_LIMIT) {
        return 0;
    }

    entry = &capture->entries[capture->count++];
    plasma_smoke_copy_text(entry->section, (unsigned int)sizeof(entry->section), section);
    plasma_smoke_copy_text(entry->key, (unsigned int)sizeof(entry->key), key);
    plasma_smoke_copy_text(entry->value, (unsigned int)sizeof(entry->value), value);
    return 1;
}

static int plasma_smoke_capture_has_string(
    const plasma_smoke_capture *capture,
    const char *section,
    const char *key,
    const char *value
)
{
    unsigned int index;

    if (capture == NULL || section == NULL || key == NULL || value == NULL) {
        return 0;
    }

    for (index = 0U; index < capture->count; ++index) {
        if (
            strcmp(capture->entries[index].section, section) == 0 &&
            strcmp(capture->entries[index].key, key) == 0 &&
            strcmp(capture->entries[index].value, value) == 0
        ) {
            return 1;
        }
    }

    return 0;
}

static int plasma_compile_direct_plan(
    const screensave_saver_module *module,
    const char *preset_key,
    const char *theme_key,
    plasma_plan *plan_out
)
{
    screensave_common_config common_config;
    plasma_config product_config;
    screensave_config_binding binding;
    screensave_saver_environment environment;

    if (module == NULL || plan_out == NULL) {
        return 0;
    }

    plasma_config_set_defaults(&common_config, &product_config, sizeof(product_config));
    if (preset_key != NULL) {
        plasma_apply_preset_to_config(preset_key, &common_config, &product_config);
        common_config.preset_key = preset_key;
    }
    if (theme_key != NULL) {
        common_config.theme_key = theme_key;
    }

    screensave_config_binding_init(&binding, &common_config, &product_config, sizeof(product_config));
    ZeroMemory(&environment, sizeof(environment));
    environment.mode = SCREENSAVE_SESSION_MODE_WINDOWED;
    environment.drawable_size.width = 320;
    environment.drawable_size.height = 240;
    environment.seed.base_seed = 0x00000A55UL;
    environment.seed.stream_seed = 0x00000A77UL;
    environment.seed.deterministic = common_config.use_deterministic_seed;
    environment.config_binding = &binding;

    return plasma_plan_compile(plan_out, module, &environment);
}

static int plasma_compile_selection_plan(
    const screensave_saver_module *module,
    const char *preset_key,
    const char *theme_key,
    const plasma_selection_preferences *selection_preferences,
    plasma_plan *plan_out
)
{
    screensave_common_config common_config;
    plasma_config product_config;
    screensave_config_binding binding;
    screensave_saver_environment environment;

    if (module == NULL || plan_out == NULL) {
        return 0;
    }

    plasma_config_set_defaults(&common_config, &product_config, sizeof(product_config));
    if (preset_key != NULL) {
        plasma_apply_preset_to_config(preset_key, &common_config, &product_config);
        common_config.preset_key = preset_key;
    }
    if (theme_key != NULL) {
        common_config.theme_key = theme_key;
    }
    if (selection_preferences != NULL) {
        product_config.selection = *selection_preferences;
    }

    screensave_config_binding_init(&binding, &common_config, &product_config, sizeof(product_config));
    ZeroMemory(&environment, sizeof(environment));
    environment.mode = SCREENSAVE_SESSION_MODE_WINDOWED;
    environment.drawable_size.width = 320;
    environment.drawable_size.height = 240;
    environment.seed.base_seed = 0x00000A55UL;
    environment.seed.stream_seed = 0x00000A77UL;
    environment.seed.deterministic = common_config.use_deterministic_seed;
    environment.config_binding = &binding;

    return plasma_plan_compile(plan_out, module, &environment);
}

static void plasma_smoke_init_fake_renderer(
    screensave_renderer *renderer,
    screensave_renderer_kind requested_kind,
    screensave_renderer_kind active_kind,
    const screensave_sizei *drawable_size
)
{
    screensave_renderer_info renderer_info;

    ZeroMemory(&renderer_info, sizeof(renderer_info));
    renderer_info.requested_kind = requested_kind;
    renderer_info.active_kind = active_kind;
    renderer_info.capability_flags = SCREENSAVE_RENDERER_CAP_CLEAR | SCREENSAVE_RENDERER_CAP_BITMAP;
    if (drawable_size != NULL) {
        renderer_info.drawable_size = *drawable_size;
    } else {
        renderer_info.drawable_size.width = 320;
        renderer_info.drawable_size.height = 240;
    }
    renderer_info.backend_name = "smoke-fake";
    renderer_info.status_text = "smoke-active";
    renderer_info.selection_reason = "smoke-forced";

    ZeroMemory(renderer, sizeof(*renderer));
    screensave_renderer_init_dispatch(renderer, &g_plasma_smoke_renderer_vtable, NULL, &renderer_info);
}

static int plasma_compile_plan_for_renderer(
    const screensave_saver_module *module,
    const char *preset_key,
    const char *theme_key,
    const plasma_selection_preferences *selection_preferences,
    screensave_renderer_kind requested_kind,
    screensave_renderer_kind active_kind,
    plasma_plan *plan_out
)
{
    screensave_common_config common_config;
    plasma_config product_config;
    screensave_config_binding binding;
    screensave_saver_environment environment;
    screensave_renderer renderer;
    screensave_sizei drawable_size;

    if (module == NULL || plan_out == NULL) {
        return 0;
    }

    plasma_config_set_defaults(&common_config, &product_config, sizeof(product_config));
    if (preset_key != NULL) {
        plasma_apply_preset_to_config(preset_key, &common_config, &product_config);
        common_config.preset_key = preset_key;
    }
    if (theme_key != NULL) {
        common_config.theme_key = theme_key;
    }
    if (selection_preferences != NULL) {
        product_config.selection = *selection_preferences;
    }

    screensave_config_binding_init(&binding, &common_config, &product_config, sizeof(product_config));
    ZeroMemory(&environment, sizeof(environment));
    drawable_size.width = 320;
    drawable_size.height = 240;
    plasma_smoke_init_fake_renderer(&renderer, requested_kind, active_kind, &drawable_size);
    environment.mode = SCREENSAVE_SESSION_MODE_WINDOWED;
    environment.drawable_size = drawable_size;
    environment.seed.base_seed = 0x00000A55UL;
    environment.seed.stream_seed = 0x00000A77UL;
    environment.seed.deterministic = common_config.use_deterministic_seed;
    environment.config_binding = &binding;
    environment.renderer = &renderer;

    return plasma_plan_compile(plan_out, module, &environment);
}

static int plasma_compile_configured_plan_for_renderer(
    const screensave_saver_module *module,
    const screensave_common_config *common_config,
    const plasma_config *product_config,
    screensave_renderer_kind requested_kind,
    screensave_renderer_kind active_kind,
    plasma_plan *plan_out
)
{
    screensave_common_config safe_common_config;
    plasma_config safe_product_config;
    screensave_config_binding binding;
    screensave_saver_environment environment;
    screensave_renderer renderer;
    screensave_sizei drawable_size;

    if (
        module == NULL ||
        common_config == NULL ||
        product_config == NULL ||
        plan_out == NULL
    ) {
        return 0;
    }

    safe_common_config = *common_config;
    safe_product_config = *product_config;
    screensave_config_binding_init(
        &binding,
        &safe_common_config,
        &safe_product_config,
        sizeof(safe_product_config)
    );
    ZeroMemory(&environment, sizeof(environment));
    drawable_size.width = 320;
    drawable_size.height = 240;
    plasma_smoke_init_fake_renderer(&renderer, requested_kind, active_kind, &drawable_size);
    environment.mode = SCREENSAVE_SESSION_MODE_WINDOWED;
    environment.drawable_size = drawable_size;
    environment.seed.base_seed = 0x00000A55UL;
    environment.seed.stream_seed = 0x00000A77UL;
    environment.seed.deterministic = safe_common_config.use_deterministic_seed;
    environment.config_binding = &binding;
    environment.renderer = &renderer;

    return plasma_plan_compile(plan_out, module, &environment);
}

static int plasma_compile_transition_plan(
    const screensave_saver_module *module,
    const char *preset_key,
    const char *theme_key,
    const plasma_selection_preferences *selection_preferences,
    const plasma_transition_preferences *transition_preferences,
    screensave_renderer_kind requested_kind,
    screensave_renderer_kind active_kind,
    plasma_plan *plan_out
)
{
    screensave_common_config common_config;
    plasma_config product_config;
    screensave_config_binding binding;
    screensave_saver_environment environment;
    screensave_renderer renderer;
    screensave_sizei drawable_size;

    if (module == NULL || plan_out == NULL) {
        return 0;
    }

    plasma_config_set_defaults(&common_config, &product_config, sizeof(product_config));
    if (preset_key != NULL) {
        plasma_apply_preset_to_config(preset_key, &common_config, &product_config);
        common_config.preset_key = preset_key;
    }
    if (theme_key != NULL) {
        common_config.theme_key = theme_key;
    }
    if (selection_preferences != NULL) {
        product_config.selection = *selection_preferences;
    }
    if (transition_preferences != NULL) {
        product_config.transition = *transition_preferences;
    }

    screensave_config_binding_init(&binding, &common_config, &product_config, sizeof(product_config));
    ZeroMemory(&environment, sizeof(environment));
    environment.mode = SCREENSAVE_SESSION_MODE_WINDOWED;
    environment.drawable_size.width = 320;
    environment.drawable_size.height = 240;
    environment.seed.base_seed = 0x00000A55UL;
    environment.seed.stream_seed = 0x00000A77UL;
    environment.seed.deterministic = common_config.use_deterministic_seed;
    environment.config_binding = &binding;
    if (active_kind != SCREENSAVE_RENDERER_KIND_UNKNOWN || requested_kind != SCREENSAVE_RENDERER_KIND_UNKNOWN) {
        drawable_size = environment.drawable_size;
        plasma_smoke_init_fake_renderer(&renderer, requested_kind, active_kind, &drawable_size);
        environment.renderer = &renderer;
    }

    return plasma_plan_compile(plan_out, module, &environment);
}

static void plasma_smoke_step_session_delta(
    screensave_saver_session *session,
    screensave_saver_environment *environment,
    unsigned long delta_millis
)
{
    if (session == NULL || environment == NULL) {
        return;
    }

    environment->clock.delta_millis = delta_millis;
    plasma_step_session(session, environment);
}

static int plasma_smoke_resolve_repo_root(
    char *repo_root_out,
    unsigned int repo_root_size
)
{
    const char *match;
    size_t length;

    if (repo_root_out == NULL || repo_root_size == 0U) {
        return 0;
    }

    if (
        GetFileAttributesA("products\\savers\\plasma\\packs\\lava_remix\\pack.ini") !=
        INVALID_FILE_ATTRIBUTES
    ) {
        lstrcpynA(repo_root_out, ".", (int)repo_root_size);
        return 1;
    }

    match = strstr(__FILE__, "products\\savers\\plasma\\tests\\smoke.c");
    if (match == NULL) {
        match = strstr(__FILE__, "products/savers/plasma/tests/smoke.c");
    }
    if (match == NULL) {
        lstrcpynA(repo_root_out, ".", (int)repo_root_size);
        return 1;
    }

    length = (size_t)(match - __FILE__);
    if (length == 0U) {
        lstrcpynA(repo_root_out, ".", (int)repo_root_size);
        return 1;
    }
    if (length + 1U > repo_root_size) {
        return 0;
    }

    memcpy(repo_root_out, __FILE__, length);
    repo_root_out[length] = '\0';
    while (
        length > 0U &&
        (repo_root_out[length - 1U] == '\\' || repo_root_out[length - 1U] == '/')
    ) {
        repo_root_out[length - 1U] = '\0';
        --length;
    }
    return 1;
}

static unsigned int plasma_smoke_count_non_black_pixels(
    const screensave_visual_buffer *visual_buffer
)
{
    unsigned int lit_count;
    int x;
    int y;

    if (visual_buffer == NULL || visual_buffer->pixels == NULL) {
        return 0U;
    }

    lit_count = 0U;
    for (y = 0; y < visual_buffer->size.height; ++y) {
        const unsigned char *row;

        row = visual_buffer->pixels + ((size_t)y * (size_t)visual_buffer->stride_bytes);
        for (x = 0; x < visual_buffer->size.width; ++x) {
            if (
                row[(x * 4) + 0] != 0U ||
                row[(x * 4) + 1] != 0U ||
                row[(x * 4) + 2] != 0U
            ) {
                ++lit_count;
            }
        }
    }

    return lit_count;
}

static void plasma_smoke_render_signature_init(
    plasma_smoke_render_signature *signature
)
{
    if (signature == NULL) {
        return;
    }

    ZeroMemory(signature, sizeof(*signature));
}

static void plasma_smoke_render_signature_release(
    plasma_smoke_render_signature *signature
)
{
    if (signature == NULL) {
        return;
    }

    if (signature->treated_pixels != NULL) {
        free(signature->treated_pixels);
    }
    if (signature->presented_pixels != NULL) {
        free(signature->presented_pixels);
    }
    ZeroMemory(signature, sizeof(*signature));
}

static unsigned long plasma_smoke_hash_bytes(
    const unsigned char *bytes,
    size_t byte_count
)
{
    unsigned long hash;
    size_t index;

    if (bytes == NULL) {
        return 0UL;
    }

    hash = 2166136261UL;
    for (index = 0U; index < byte_count; ++index) {
        hash ^= (unsigned long)bytes[index];
        hash *= 16777619UL;
    }

    return hash;
}

static unsigned int plasma_smoke_count_non_black_bitmap(
    const screensave_bitmap_view *bitmap
)
{
    unsigned int lit_count;
    const unsigned char *pixels;
    int x;
    int y;

    if (
        bitmap == NULL ||
        bitmap->pixels == NULL ||
        bitmap->size.width <= 0 ||
        bitmap->size.height <= 0 ||
        bitmap->stride_bytes <= 0
    ) {
        return 0U;
    }

    lit_count = 0U;
    pixels = (const unsigned char *)bitmap->pixels;
    for (y = 0; y < bitmap->size.height; ++y) {
        const unsigned char *row;

        row = pixels + ((size_t)y * (size_t)bitmap->stride_bytes);
        for (x = 0; x < bitmap->size.width; ++x) {
            if (
                row[(x * 4) + 0] != 0U ||
                row[(x * 4) + 1] != 0U ||
                row[(x * 4) + 2] != 0U
            ) {
                ++lit_count;
            }
        }
    }

    return lit_count;
}

static int plasma_smoke_copy_pixel_bytes(
    unsigned char **copy_out,
    screensave_sizei *size_out,
    int *stride_bytes_out,
    const void *pixels,
    const screensave_sizei *size,
    int stride_bytes
)
{
    size_t byte_count;
    unsigned char *copy;

    if (
        copy_out == NULL ||
        size_out == NULL ||
        stride_bytes_out == NULL ||
        pixels == NULL ||
        size == NULL ||
        size->width <= 0 ||
        size->height <= 0 ||
        stride_bytes <= 0
    ) {
        return 0;
    }

    byte_count = (size_t)stride_bytes * (size_t)size->height;
    copy = (unsigned char *)malloc(byte_count);
    if (copy == NULL) {
        return 0;
    }

    memcpy(copy, pixels, byte_count);
    *copy_out = copy;
    *size_out = *size;
    *stride_bytes_out = stride_bytes;
    return 1;
}

static unsigned int plasma_smoke_count_pixel_differences(
    const unsigned char *left_pixels,
    const screensave_sizei *left_size,
    int left_stride_bytes,
    const unsigned char *right_pixels,
    const screensave_sizei *right_size,
    int right_stride_bytes
)
{
    unsigned int difference_count;
    int max_width;
    int max_height;
    int x;
    int y;

    if (
        left_pixels == NULL ||
        left_size == NULL ||
        right_pixels == NULL ||
        right_size == NULL ||
        left_stride_bytes <= 0 ||
        right_stride_bytes <= 0
    ) {
        return 0U;
    }

    if (
        left_size->width != right_size->width ||
        left_size->height != right_size->height
    ) {
        max_width = left_size->width > right_size->width ? left_size->width : right_size->width;
        max_height = left_size->height > right_size->height ? left_size->height : right_size->height;
        return (unsigned int)(max_width * max_height);
    }

    difference_count = 0U;
    for (y = 0; y < left_size->height; ++y) {
        const unsigned char *left_row;
        const unsigned char *right_row;

        left_row = left_pixels + ((size_t)y * (size_t)left_stride_bytes);
        right_row = right_pixels + ((size_t)y * (size_t)right_stride_bytes);
        for (x = 0; x < left_size->width; ++x) {
            if (
                left_row[(x * 4) + 0] != right_row[(x * 4) + 0] ||
                left_row[(x * 4) + 1] != right_row[(x * 4) + 1] ||
                left_row[(x * 4) + 2] != right_row[(x * 4) + 2]
            ) {
                ++difference_count;
            }
        }
    }

    return difference_count;
}

static unsigned int plasma_smoke_render_difference_count(
    const plasma_smoke_render_signature *left,
    const plasma_smoke_render_signature *right,
    int compare_presented
)
{
    if (compare_presented) {
        return plasma_smoke_count_pixel_differences(
            left != NULL ? left->presented_pixels : NULL,
            left != NULL ? &left->presented_size : NULL,
            left != NULL ? left->presented_stride_bytes : 0,
            right != NULL ? right->presented_pixels : NULL,
            right != NULL ? &right->presented_size : NULL,
            right != NULL ? right->presented_stride_bytes : 0
        );
    }

    return plasma_smoke_count_pixel_differences(
        left != NULL ? left->treated_pixels : NULL,
        left != NULL ? &left->treated_size : NULL,
        left != NULL ? left->treated_stride_bytes : 0,
        right != NULL ? right->treated_pixels : NULL,
        right != NULL ? &right->treated_size : NULL,
        right != NULL ? right->treated_stride_bytes : 0
    );
}

static int plasma_smoke_signatures_meaningfully_different(
    const plasma_smoke_render_signature *left,
    const plasma_smoke_render_signature *right,
    int compare_presented,
    unsigned int minimum_pixel_difference
)
{
    unsigned long left_hash;
    unsigned long right_hash;

    if (left == NULL || right == NULL) {
        return 0;
    }

    left_hash = compare_presented ? left->presented_hash : left->treated_hash;
    right_hash = compare_presented ? right->presented_hash : right->treated_hash;
    if (left_hash == 0UL || right_hash == 0UL || left_hash == right_hash) {
        return 0;
    }

    return plasma_smoke_render_difference_count(left, right, compare_presented) >= minimum_pixel_difference;
}

static int plasma_smoke_capture_render_signature(
    const screensave_saver_module *module,
    const screensave_common_config *common_config,
    const plasma_config *product_config,
    screensave_renderer_kind requested_kind,
    screensave_renderer_kind active_kind,
    unsigned long delta_millis,
    plasma_smoke_render_signature *signature_out
)
{
    screensave_common_config safe_common_config;
    plasma_config safe_product_config;
    screensave_config_binding binding;
    screensave_saver_environment environment;
    screensave_renderer renderer;
    screensave_sizei drawable_size;
    screensave_saver_session *session;
    plasma_output_frame output_frame;
    plasma_treated_frame treated_frame;
    plasma_presentation_target presentation_target;
    size_t byte_count;
    int result;

    if (
        module == NULL ||
        common_config == NULL ||
        product_config == NULL ||
        signature_out == NULL
    ) {
        return 0;
    }

    plasma_smoke_render_signature_release(signature_out);
    plasma_smoke_render_signature_init(signature_out);

    safe_common_config = *common_config;
    safe_product_config = *product_config;
    screensave_config_binding_init(
        &binding,
        &safe_common_config,
        &safe_product_config,
        sizeof(safe_product_config)
    );

    ZeroMemory(&environment, sizeof(environment));
    drawable_size.width = 320;
    drawable_size.height = 240;
    plasma_smoke_init_fake_renderer(&renderer, requested_kind, active_kind, &drawable_size);
    environment.mode = SCREENSAVE_SESSION_MODE_WINDOWED;
    environment.drawable_size = drawable_size;
    environment.seed.base_seed = 0x2468ACE0UL;
    environment.seed.stream_seed = 0x13579BDFUL;
    environment.seed.deterministic = safe_common_config.use_deterministic_seed;
    environment.config_binding = &binding;
    environment.renderer = &renderer;

    session = NULL;
    if (!plasma_create_session(module, &session, &environment) || session == NULL) {
        return 0;
    }

    result = 0;
    plasma_smoke_step_session_delta(session, &environment, delta_millis);
    if (
        !plasma_output_build(&session->plan, &session->state, &output_frame) ||
        !plasma_treatment_apply(
            &session->plan,
            &session->state,
            &output_frame,
            &session->state.visual_buffer,
            &treated_frame
        ) ||
        !plasma_presentation_prepare(
            &session->plan,
            &session->state,
            &treated_frame,
            &presentation_target
        ) ||
        session->state.visual_buffer.pixels == NULL ||
        presentation_target.bitmap_view.pixels == NULL
    ) {
        plasma_destroy_session(session);
        plasma_smoke_render_signature_release(signature_out);
        return 0;
    }

    byte_count = (size_t)session->state.visual_buffer.stride_bytes *
        (size_t)session->state.visual_buffer.size.height;
    signature_out->treated_hash = plasma_smoke_hash_bytes(
        session->state.visual_buffer.pixels,
        byte_count
    );
    signature_out->treated_lit_pixels = plasma_smoke_count_non_black_pixels(
        &session->state.visual_buffer
    );
    if (
        !plasma_smoke_copy_pixel_bytes(
            &signature_out->treated_pixels,
            &signature_out->treated_size,
            &signature_out->treated_stride_bytes,
            session->state.visual_buffer.pixels,
            &session->state.visual_buffer.size,
            session->state.visual_buffer.stride_bytes
        )
    ) {
        plasma_destroy_session(session);
        plasma_smoke_render_signature_release(signature_out);
        return 0;
    }

    byte_count = (size_t)presentation_target.bitmap_view.stride_bytes *
        (size_t)presentation_target.bitmap_view.size.height;
    signature_out->presented_hash = plasma_smoke_hash_bytes(
        (const unsigned char *)presentation_target.bitmap_view.pixels,
        byte_count
    );
    signature_out->presented_lit_pixels = plasma_smoke_count_non_black_bitmap(
        &presentation_target.bitmap_view
    );
    if (
        !plasma_smoke_copy_pixel_bytes(
            &signature_out->presented_pixels,
            &signature_out->presented_size,
            &signature_out->presented_stride_bytes,
            presentation_target.bitmap_view.pixels,
            &presentation_target.bitmap_view.size,
            presentation_target.bitmap_view.stride_bytes
        )
    ) {
        plasma_destroy_session(session);
        plasma_smoke_render_signature_release(signature_out);
        return 0;
    }

    result = 1;
    plasma_destroy_session(session);
    return result;
}

int main(void)
{
    static const char *const g_required_preset_keys[] = {
        "plasma_lava",
        "aurora_plasma",
        "ocean_interference",
        "museum_phosphor",
        "quiet_darkroom",
        "midnight_interference",
        "amber_terminal",
        "lava_isolines",
        "aurora_bands",
        "wire_glow",
        "phosphor_topography",
        "stipple_bands",
        "emboss_current",
        "crt_signal_bands",
        "ascii_reactor",
        "matrix_lattice",
        "cellular_bloom",
        "quasi_crystal_bands",
        "caustic_waterlight",
        "aurora_curtain",
        "ribbon_aurora",
        "substrate_relief",
        "filament_extrusion"
    };
    static const char *const g_required_theme_keys[] = {
        "plasma_lava",
        "aurora_cool",
        "oceanic_blue",
        "museum_phosphor",
        "quiet_darkroom",
        "midnight_interference",
        "amber_terminal"
    };
    screensave_common_config common_config;
    plasma_config product_config;
    screensave_config_binding binding;
    screensave_saver_environment environment;
    screensave_saver_environment preview_environment;
    screensave_saver_environment windowed_environment;
    screensave_saver_session *session;
    screensave_saver_session *preview_session;
    screensave_saver_session *windowed_session;
    unsigned long issue_flags;
    const screensave_saver_module *module;
    const screensave_preset_descriptor *preset_descriptor;
    const screensave_theme_descriptor *theme_descriptor;
    const plasma_settings_descriptor *settings_descriptor;
    const plasma_settings_descriptor *settings_catalog;
    const plasma_validation_matrix_entry *matrix_entry;
    const plasma_performance_envelope_entry *envelope_entry;
    const plasma_known_limit_entry *known_limit_entry;
    plasma_plan plan;
    plasma_output_frame output_frame;
    plasma_treated_frame treated_frame;
    plasma_presentation_target presentation_target;
    const plasma_content_registry *registry;
    const plasma_content_pack_entry *pack_entry;
    screensave_pack_manifest pack_manifest;
    plasma_settings_context settings_context;
    plasma_settings_resolution settings_resolution;
    plasma_selection_preferences selection_preferences;
    plasma_transition_preferences transition_preferences;
    plasma_benchlab_forcing benchlab_forcing;
    plasma_benchlab_snapshot benchlab_snapshot;
    screensave_saver_config_state benchlab_config_state;
    plasma_smoke_capture settings_capture;
    plasma_smoke_render_signature baseline_render_signature;
    plasma_smoke_render_signature variant_render_signature;
    screensave_settings_writer settings_writer;
    screensave_session_seed random_seed;
    screensave_renderer fake_renderer;
    screensave_renderer preview_renderer;
    screensave_renderer windowed_renderer;
    screensave_sizei fake_size;
    screensave_color source_primary;
    screensave_color source_accent;
    screensave_color target_primary;
    screensave_color target_accent;
    screensave_color blended_primary;
    screensave_color blended_accent;
    int smoothing_enabled;
    unsigned int smoothing_blend;
    unsigned int transition_progress;
    unsigned int matrix_count;
    unsigned int envelope_count;
    unsigned int known_limit_count;
    unsigned int soak_iteration;
    unsigned int expected_journey_index;
    unsigned int settle_iteration;
    unsigned long mid_speed_units;
    unsigned long preview_phase_before;
    unsigned long preview_phase_after;
    unsigned long settle_delta;
    screensave_renderer_kind soak_renderer_kind;
    unsigned long windowed_phase_before;
    unsigned long windowed_phase_after;
    char benchlab_overlay[2048];
    char benchlab_report[8192];
    char authoring_message[260];
    char repo_root[MAX_PATH];
    unsigned int settings_count;
    unsigned int index;

    module = plasma_get_module();
    if (!screensave_saver_module_is_valid(module)) {
        return 1;
    }
    if (
        module->identity.product_key == NULL ||
        strcmp(module->identity.product_key, "plasma") != 0 ||
        module->identity.display_name == NULL ||
        strcmp(module->identity.display_name, "Plasma") != 0
    ) {
        return 2;
    }
    if (
        module->routing_policy.minimum_kind != SCREENSAVE_RENDERER_KIND_GDI ||
        module->routing_policy.preferred_kind != SCREENSAVE_RENDERER_KIND_GL11 ||
        module->routing_policy.quality_class != SCREENSAVE_CAPABILITY_QUALITY_SAFE
    ) {
        return 3;
    }
    if (
        !(module->capability_flags & SCREENSAVE_SAVER_CAP_GDI) ||
        !(module->capability_flags & SCREENSAVE_SAVER_CAP_GL11) ||
        !(module->capability_flags & SCREENSAVE_SAVER_CAP_GL21) ||
        !(module->capability_flags & SCREENSAVE_SAVER_CAP_GL33) ||
        !(module->capability_flags & SCREENSAVE_SAVER_CAP_GL46) ||
        !(module->capability_flags & SCREENSAVE_SAVER_CAP_LONG_RUN_STABLE) ||
        !(module->capability_flags & SCREENSAVE_SAVER_CAP_PREVIEW_SAFE)
    ) {
        return 4;
    }

    plasma_config_set_defaults(&common_config, &product_config, sizeof(product_config));
    plasma_config_clamp(&common_config, &product_config, sizeof(product_config));
    if (!screensave_common_config_validate(&common_config, &issue_flags)) {
        return 5;
    }
    if (
        common_config.preset_key == NULL ||
        strcmp(common_config.preset_key, PLASMA_DEFAULT_PRESET_KEY) != 0 ||
        common_config.theme_key == NULL ||
        strcmp(common_config.theme_key, PLASMA_DEFAULT_THEME_KEY) != 0
    ) {
        return 6;
    }
    if (
        plasma_preset_count() != PLASMA_PRESET_COUNT ||
        plasma_theme_count() != PLASMA_THEME_COUNT ||
        module->preset_count != plasma_preset_count() ||
        module->theme_count != plasma_theme_count() ||
        plasma_classic_preset_count() != plasma_preset_count() ||
        plasma_classic_theme_count() != plasma_theme_count()
    ) {
        return 7;
    }

    for (index = 0U; index < (unsigned int)(sizeof(g_required_preset_keys) / sizeof(g_required_preset_keys[0])); ++index) {
        if (
            !plasma_is_known_preset_key(g_required_preset_keys[index]) ||
            !plasma_classic_is_known_preset_key(g_required_preset_keys[index])
        ) {
            return 8;
        }
    }
    for (index = 0U; index < (unsigned int)(sizeof(g_required_theme_keys) / sizeof(g_required_theme_keys[0])); ++index) {
        if (
            !plasma_is_known_theme_key(g_required_theme_keys[index]) ||
            !plasma_classic_is_known_theme_key(g_required_theme_keys[index])
        ) {
            return 9;
        }
    }
    if (
        plasma_canonical_content_key("ember_lava") == NULL ||
        strcmp(plasma_canonical_content_key("ember_lava"), "plasma_lava") != 0 ||
        plasma_classic_canonical_key("ember_lava") == NULL ||
        strcmp(plasma_classic_canonical_key("ember_lava"), "plasma_lava") != 0 ||
        strcmp(
            plasma_classic_canonical_key("ember_lava"),
            plasma_canonical_content_key("ember_lava")
        ) != 0
    ) {
        return 10;
    }
    preset_descriptor = plasma_find_preset_descriptor("ember_lava");
    if (preset_descriptor == NULL || strcmp(preset_descriptor->preset_key, "plasma_lava") != 0) {
        return 11;
    }
    if (plasma_find_preset_values("ember_lava") == NULL) {
        return 12;
    }
    theme_descriptor = plasma_find_theme_descriptor("ember_lava");
    if (theme_descriptor == NULL || strcmp(theme_descriptor->theme_key, "plasma_lava") != 0) {
        return 13;
    }
    if (module->config_hooks == NULL || module->config_hooks->randomize_settings == NULL) {
        return 14;
    }

    settings_catalog = plasma_settings_get_catalog(&settings_count);
    if (
        !plasma_settings_catalog_validate() ||
        settings_catalog == NULL ||
        settings_count < 20U
    ) {
        return 172;
    }
    if (product_config.settings_surface != PLASMA_SETTINGS_SURFACE_BASIC) {
        return 173;
    }
    if (
        plasma_settings_find_descriptor("preset_key") == NULL ||
        plasma_settings_find_descriptor("detail_level") == NULL ||
        plasma_settings_find_descriptor("content_filter") == NULL ||
        plasma_settings_find_descriptor("transitions_enabled") == NULL ||
        plasma_settings_find_descriptor("preset_set_key") == NULL ||
        plasma_settings_find_descriptor("output_family") == NULL ||
        plasma_settings_find_descriptor("filter_treatment") == NULL ||
        plasma_settings_find_descriptor("presentation_mode") == NULL ||
        plasma_settings_find_descriptor("transition_policy") == NULL ||
        plasma_settings_find_descriptor("journey_key") == NULL ||
        plasma_settings_find_descriptor("diagnostics_overlay_enabled") == NULL
    ) {
        return 174;
    }
    settings_descriptor = plasma_settings_find_descriptor("detail_level");
    if (
        settings_descriptor == NULL ||
        settings_descriptor->surface != PLASMA_SETTINGS_SURFACE_BASIC ||
        plasma_settings_find_descriptor("content_filter")->surface != PLASMA_SETTINGS_SURFACE_ADVANCED ||
        plasma_settings_find_descriptor("transitions_enabled")->surface != PLASMA_SETTINGS_SURFACE_ADVANCED ||
        plasma_settings_find_descriptor("output_family")->surface != PLASMA_SETTINGS_SURFACE_ADVANCED ||
        plasma_settings_find_descriptor("transition_policy")->surface != PLASMA_SETTINGS_SURFACE_AUTHOR_LAB ||
        plasma_settings_find_descriptor("preset_set_key")->surface != PLASMA_SETTINGS_SURFACE_AUTHOR_LAB
    ) {
        return 435;
    }
    if (
        !plasma_settings_surface_contains_setting_key(PLASMA_SETTINGS_SURFACE_BASIC, "preset_key") ||
        !plasma_settings_surface_contains_setting_key(PLASMA_SETTINGS_SURFACE_BASIC, "theme_key") ||
        plasma_settings_surface_contains_setting_key(PLASMA_SETTINGS_SURFACE_BASIC, "content_filter") ||
        plasma_settings_surface_contains_setting_key(PLASMA_SETTINGS_SURFACE_BASIC, "transitions_enabled") ||
        plasma_settings_surface_contains_setting_key(PLASMA_SETTINGS_SURFACE_BASIC, "output_family") ||
        !plasma_settings_surface_contains_setting_key(PLASMA_SETTINGS_SURFACE_ADVANCED, "effect_mode") ||
        !plasma_settings_surface_contains_setting_key(PLASMA_SETTINGS_SURFACE_ADVANCED, "content_filter") ||
        !plasma_settings_surface_contains_setting_key(PLASMA_SETTINGS_SURFACE_ADVANCED, "transitions_enabled") ||
        !plasma_settings_surface_contains_setting_key(PLASMA_SETTINGS_SURFACE_ADVANCED, "output_family") ||
        !plasma_settings_surface_contains_setting_key(PLASMA_SETTINGS_SURFACE_ADVANCED, "use_deterministic_seed") ||
        plasma_settings_surface_contains_setting_key(PLASMA_SETTINGS_SURFACE_ADVANCED, "preset_key") ||
        plasma_settings_surface_contains_setting_key(PLASMA_SETTINGS_SURFACE_ADVANCED, "transition_policy") ||
        !plasma_settings_surface_contains_setting_key(PLASMA_SETTINGS_SURFACE_AUTHOR_LAB, "preset_set_key") ||
        !plasma_settings_surface_contains_setting_key(PLASMA_SETTINGS_SURFACE_AUTHOR_LAB, "transition_policy") ||
        !plasma_settings_surface_contains_setting_key(PLASMA_SETTINGS_SURFACE_AUTHOR_LAB, "deterministic_seed") ||
        plasma_settings_surface_contains_setting_key(PLASMA_SETTINGS_SURFACE_AUTHOR_LAB, "use_deterministic_seed")
    ) {
        return 436;
    }

    plasma_settings_context_init(
        &settings_context,
        module,
        &common_config,
        &product_config,
        SCREENSAVE_RENDERER_KIND_UNKNOWN,
        SCREENSAVE_RENDERER_KIND_GL11
    );
    if (!plasma_settings_resolve(&settings_resolution, &settings_context)) {
        return 175;
    }
    if (
        settings_resolution.surface != PLASMA_SETTINGS_SURFACE_BASIC ||
        settings_resolution.detail_level != common_config.detail_level ||
        settings_resolution.effect_mode != product_config.effect_mode ||
        settings_resolution.speed_mode != product_config.speed_mode ||
        settings_resolution.resolution_mode != product_config.resolution_mode ||
        settings_resolution.smoothing_mode != product_config.smoothing_mode ||
        settings_resolution.output_family != product_config.output_family ||
        settings_resolution.output_mode != product_config.output_mode ||
        settings_resolution.sampling_treatment != product_config.sampling_treatment ||
        settings_resolution.filter_treatment != product_config.filter_treatment ||
        settings_resolution.emulation_treatment != product_config.emulation_treatment ||
        settings_resolution.accent_treatment != product_config.accent_treatment ||
        settings_resolution.presentation_mode != product_config.presentation_mode ||
        settings_resolution.transitions_enabled ||
        settings_resolution.transition_policy != PLASMA_TRANSITION_POLICY_DISABLED ||
        settings_resolution.content_filter != PLASMA_CONTENT_FILTER_STABLE_ONLY ||
        settings_resolution.favorites_only ||
        strcmp(settings_resolution.preset_set_key, "") != 0 ||
        strcmp(settings_resolution.theme_set_key, "") != 0 ||
        strcmp(settings_resolution.journey_key, "") != 0
    ) {
        return 176;
    }

    settings_descriptor = plasma_settings_find_descriptor("content_filter");
    if (
        settings_descriptor == NULL ||
        !plasma_settings_is_available(settings_descriptor, &settings_context)
    ) {
        return 177;
    }
    registry = plasma_content_get_registry();
    if (
        registry == NULL ||
        !plasma_content_registry_has_channel(PLASMA_CONTENT_CHANNEL_EXPERIMENTAL) ||
        plasma_content_find_preset_entry("lava_isolines") == NULL ||
        plasma_content_find_preset_entry("lava_isolines")->channel != PLASMA_CONTENT_CHANNEL_EXPERIMENTAL ||
        plasma_content_find_preset_entry("aurora_bands") == NULL ||
        plasma_content_find_preset_entry("aurora_bands")->channel != PLASMA_CONTENT_CHANNEL_EXPERIMENTAL ||
        plasma_content_find_preset_entry("phosphor_topography") == NULL ||
        plasma_content_find_preset_entry("phosphor_topography")->channel != PLASMA_CONTENT_CHANNEL_EXPERIMENTAL ||
        plasma_content_find_preset_entry("ascii_reactor") == NULL ||
        plasma_content_find_preset_entry("ascii_reactor")->channel != PLASMA_CONTENT_CHANNEL_EXPERIMENTAL ||
        plasma_content_find_preset_entry("matrix_lattice") == NULL ||
        plasma_content_find_preset_entry("matrix_lattice")->channel != PLASMA_CONTENT_CHANNEL_EXPERIMENTAL ||
        plasma_content_find_preset_entry("cellular_bloom") == NULL ||
        plasma_content_find_preset_entry("cellular_bloom")->channel != PLASMA_CONTENT_CHANNEL_EXPERIMENTAL ||
        plasma_content_find_preset_entry("quasi_crystal_bands") == NULL ||
        plasma_content_find_preset_entry("quasi_crystal_bands")->channel != PLASMA_CONTENT_CHANNEL_EXPERIMENTAL ||
        plasma_content_find_preset_entry("caustic_waterlight") == NULL ||
        plasma_content_find_preset_entry("caustic_waterlight")->channel != PLASMA_CONTENT_CHANNEL_EXPERIMENTAL ||
        plasma_content_find_preset_entry("midnight_interference") == NULL ||
        plasma_content_find_preset_entry("midnight_interference")->channel != PLASMA_CONTENT_CHANNEL_EXPERIMENTAL ||
        plasma_content_find_preset_entry("amber_terminal") == NULL ||
        plasma_content_find_preset_entry("amber_terminal")->channel != PLASMA_CONTENT_CHANNEL_EXPERIMENTAL ||
        plasma_content_find_preset_entry("aurora_curtain") == NULL ||
        plasma_content_find_preset_entry("aurora_curtain")->channel != PLASMA_CONTENT_CHANNEL_EXPERIMENTAL ||
        plasma_content_find_preset_entry("ribbon_aurora") == NULL ||
        plasma_content_find_preset_entry("ribbon_aurora")->channel != PLASMA_CONTENT_CHANNEL_EXPERIMENTAL ||
        plasma_content_find_preset_entry("substrate_relief") == NULL ||
        plasma_content_find_preset_entry("substrate_relief")->channel != PLASMA_CONTENT_CHANNEL_EXPERIMENTAL ||
        plasma_content_find_preset_entry("filament_extrusion") == NULL ||
        plasma_content_find_preset_entry("filament_extrusion")->channel != PLASMA_CONTENT_CHANNEL_EXPERIMENTAL ||
        plasma_content_find_theme_entry("midnight_interference") == NULL ||
        plasma_content_find_theme_entry("midnight_interference")->channel != PLASMA_CONTENT_CHANNEL_EXPERIMENTAL ||
        plasma_content_find_theme_entry("amber_terminal") == NULL ||
        plasma_content_find_theme_entry("amber_terminal")->channel != PLASMA_CONTENT_CHANNEL_EXPERIMENTAL ||
        plasma_content_preset_is_primary_visible(plasma_content_find_preset_entry("plasma_lava")) == 0 ||
        plasma_content_preset_is_primary_visible(plasma_content_find_preset_entry("ribbon_aurora")) == 0 ||
        plasma_content_preset_is_primary_visible(plasma_content_find_preset_entry("midnight_interference")) != 0 ||
        plasma_content_preset_is_primary_visible(plasma_content_find_preset_entry("amber_terminal")) != 0 ||
        plasma_content_preset_is_primary_visible(plasma_content_find_preset_entry("aurora_curtain")) != 0 ||
        plasma_content_preset_is_primary_visible(plasma_content_find_preset_entry("substrate_relief")) != 0 ||
        plasma_content_preset_is_primary_visible(plasma_content_find_preset_entry("filament_extrusion")) != 0 ||
        plasma_content_theme_is_primary_visible(plasma_content_find_theme_entry("plasma_lava")) == 0 ||
        plasma_content_theme_is_primary_visible(plasma_content_find_theme_entry("midnight_interference")) != 0 ||
        plasma_content_theme_is_primary_visible(plasma_content_find_theme_entry("amber_terminal")) != 0
    ) {
        return 360;
    }
    settings_descriptor = plasma_settings_find_descriptor("preset_set_key");
    if (
        settings_descriptor == NULL ||
        !plasma_settings_is_available(settings_descriptor, &settings_context)
    ) {
        return 178;
    }
    settings_descriptor = plasma_settings_find_descriptor("theme_set_key");
    if (
        settings_descriptor == NULL ||
        !plasma_settings_is_available(settings_descriptor, &settings_context)
    ) {
        return 179;
    }
    settings_descriptor = plasma_settings_find_descriptor("favorites_only");
    if (
        settings_descriptor == NULL ||
        plasma_settings_is_exposed(settings_descriptor, &settings_context) ||
        plasma_settings_is_available(settings_descriptor, &settings_context)
    ) {
        return 180;
    }
    lstrcpyA(product_config.selection.favorite_preset_keys, "plasma_lava");
    plasma_settings_context_init(
        &settings_context,
        module,
        &common_config,
        &product_config,
        SCREENSAVE_RENDERER_KIND_UNKNOWN,
        SCREENSAVE_RENDERER_KIND_GL11
    );
    if (!plasma_settings_is_available(settings_descriptor, &settings_context)) {
        return 181;
    }
    if (plasma_settings_is_exposed(settings_descriptor, &settings_context)) {
        return 453;
    }
    lstrcpyA(product_config.selection.favorite_preset_keys, "");

    settings_descriptor = plasma_settings_find_descriptor("deterministic_seed");
    if (
        settings_descriptor == NULL ||
        plasma_settings_is_available(settings_descriptor, &settings_context)
    ) {
        return 182;
    }
    common_config.use_deterministic_seed = 1;
    common_config.deterministic_seed = 424242UL;
    plasma_settings_context_init(
        &settings_context,
        module,
        &common_config,
        &product_config,
        SCREENSAVE_RENDERER_KIND_UNKNOWN,
        SCREENSAVE_RENDERER_KIND_GL11
    );
    if (
        !plasma_settings_is_available(settings_descriptor, &settings_context) ||
        !plasma_settings_resolve(&settings_resolution, &settings_context) ||
        !settings_resolution.use_deterministic_seed ||
        settings_resolution.deterministic_seed != 424242UL
    ) {
        return 183;
    }

    plasma_transition_preferences_set_defaults(&product_config.transition);
    product_config.transition.enabled = 1;
    product_config.transition.policy = PLASMA_TRANSITION_POLICY_JOURNEY;
    lstrcpyA(product_config.transition.journey_key, "classic_cycle");
    settings_descriptor = plasma_settings_find_descriptor("journey_key");
    plasma_settings_context_init(
        &settings_context,
        module,
        &common_config,
        &product_config,
        SCREENSAVE_RENDERER_KIND_UNKNOWN,
        SCREENSAVE_RENDERER_KIND_GL11
    );
    if (
        settings_descriptor == NULL ||
        !plasma_settings_is_available(settings_descriptor, &settings_context) ||
        !plasma_settings_resolve(&settings_resolution, &settings_context) ||
        !settings_resolution.transitions_enabled ||
        settings_resolution.transition_policy != PLASMA_TRANSITION_POLICY_JOURNEY ||
        strcmp(settings_resolution.journey_key, "classic_cycle") != 0
    ) {
        return 184;
    }

    plasma_config_set_defaults(&common_config, &product_config, sizeof(product_config));
    plasma_config_clamp(&common_config, &product_config, sizeof(product_config));

    if (!plasma_compile_direct_plan(module, PLASMA_DEFAULT_PRESET_KEY, NULL, &plan)) {
        return 15;
    }
    if (!plasma_plan_validate(&plan, module)) {
        return 16;
    }
    if (
        plan.preset_key == NULL ||
        strcmp(plan.preset_key, PLASMA_DEFAULT_PRESET_KEY) != 0 ||
        plan.theme_key == NULL ||
        strcmp(plan.theme_key, PLASMA_DEFAULT_THEME_KEY) != 0 ||
        plan.output_family != PLASMA_OUTPUT_FAMILY_RASTER ||
        plan.output_mode != PLASMA_OUTPUT_MODE_NATIVE_RASTER ||
        plan.sampling_treatment != PLASMA_SAMPLING_TREATMENT_NONE ||
        plan.filter_treatment != PLASMA_FILTER_TREATMENT_NONE ||
        plan.emulation_treatment != PLASMA_EMULATION_TREATMENT_NONE ||
        plan.accent_treatment != PLASMA_ACCENT_TREATMENT_NONE ||
        plan.presentation_mode != PLASMA_PRESENTATION_MODE_FLAT ||
        plan.minimum_kind != SCREENSAVE_RENDERER_KIND_GDI ||
        plan.preferred_kind != SCREENSAVE_RENDERER_KIND_GL11 ||
        plan.quality_class != SCREENSAVE_CAPABILITY_QUALITY_SAFE ||
        plan.requested_renderer_kind != SCREENSAVE_RENDERER_KIND_UNKNOWN ||
        plan.active_renderer_kind != SCREENSAVE_RENDERER_KIND_GDI ||
        !plan.advanced_capable ||
        plan.advanced_requested ||
        plan.advanced_enabled ||
        plan.advanced_degraded ||
        plan.advanced_components != 0UL ||
        plan.advanced_degrade_policy == 0UL ||
        !plan.modern_capable ||
        plan.modern_requested ||
        plan.modern_enabled ||
        plan.modern_degraded ||
        plan.modern_components != 0UL ||
        plan.modern_degrade_policy == 0UL ||
        !plan.premium_capable ||
        plan.premium_requested ||
        plan.premium_enabled ||
        plan.premium_degraded ||
        plan.premium_components != 0UL ||
        plan.premium_degrade_policy == 0UL ||
        plan.transition_requested ||
        plan.transition_enabled ||
        plan.transition_policy != PLASMA_TRANSITION_POLICY_DISABLED ||
        plan.transition_fallback_policy != PLASMA_TRANSITION_FALLBACK_THEME_MORPH ||
        plan.transition_seed_policy != PLASMA_TRANSITION_SEED_CONTINUITY_KEEP_STREAM ||
        plan.transition_supported_types !=
            (PLASMA_TRANSITION_SUPPORTED_THEME_MORPH |
                PLASMA_TRANSITION_SUPPORTED_PRESET_MORPH |
                PLASMA_TRANSITION_SUPPORTED_FALLBACK) ||
        plan.transition_interval_millis != 12000UL ||
        plan.transition_duration_millis != 2400UL ||
        plan.journey != NULL ||
        !plasma_output_validate_plan(&plan) ||
        !plasma_treatment_validate_plan(&plan) ||
        !plasma_presentation_validate_plan(&plan) ||
        !plasma_plan_is_lower_band_baseline(&plan) ||
        !plasma_plan_validate_for_renderer_kind(&plan, module, SCREENSAVE_RENDERER_KIND_GDI) ||
        !plasma_plan_validate_for_renderer_kind(&plan, module, SCREENSAVE_RENDERER_KIND_GL11) ||
        !plasma_plan_validate_lower_band_baseline(&plan, module)
    ) {
        return 17;
    }
    if (
        plasma_plan_validate_for_renderer_kind(&plan, module, SCREENSAVE_RENDERER_KIND_GL21) ||
        plasma_plan_validate_for_renderer_kind(&plan, module, SCREENSAVE_RENDERER_KIND_GL33) ||
        plasma_plan_validate_for_renderer_kind(&plan, module, SCREENSAVE_RENDERER_KIND_GL46) ||
        plasma_plan_validate_for_renderer_kind(&plan, module, SCREENSAVE_RENDERER_KIND_UNKNOWN) ||
        !plasma_is_lower_band_kind(SCREENSAVE_RENDERER_KIND_GDI) ||
        !plasma_is_lower_band_kind(SCREENSAVE_RENDERER_KIND_GL11) ||
        plasma_is_lower_band_kind(SCREENSAVE_RENDERER_KIND_GL21) ||
        plasma_is_lower_band_kind(SCREENSAVE_RENDERER_KIND_GL33)
    ) {
        return 18;
    }
    if (!plasma_compile_direct_plan(module, "ember_lava", "ember_lava", &plan)) {
        return 19;
    }
    if (
        plan.preset_key == NULL ||
        strcmp(plan.preset_key, "plasma_lava") != 0 ||
        plan.theme_key == NULL ||
        strcmp(plan.theme_key, "plasma_lava") != 0
    ) {
        return 20;
    }
    if (
        !plasma_compile_plan_for_renderer(
            module,
            "aurora_plasma",
            "aurora_cool",
            NULL,
            SCREENSAVE_RENDERER_KIND_GDI,
            SCREENSAVE_RENDERER_KIND_GDI,
            &plan
        ) ||
        plan.output_family != PLASMA_OUTPUT_FAMILY_RASTER ||
        plan.output_mode != PLASMA_OUTPUT_MODE_NATIVE_RASTER ||
        plan.filter_treatment != PLASMA_FILTER_TREATMENT_GLOW_EDGE ||
        plan.emulation_treatment != PLASMA_EMULATION_TREATMENT_NONE ||
        plan.accent_treatment != PLASMA_ACCENT_TREATMENT_NONE
    ) {
        return 436;
    }
    if (
        !plasma_compile_plan_for_renderer(
            module,
            "museum_phosphor",
            "museum_phosphor",
            NULL,
            SCREENSAVE_RENDERER_KIND_GDI,
            SCREENSAVE_RENDERER_KIND_GDI,
            &plan
        ) ||
        plan.filter_treatment != PLASMA_FILTER_TREATMENT_NONE ||
        plan.emulation_treatment != PLASMA_EMULATION_TREATMENT_PHOSPHOR ||
        plan.accent_treatment != PLASMA_ACCENT_TREATMENT_NONE
    ) {
        return 437;
    }
    if (
        !plasma_compile_plan_for_renderer(
            module,
            "quiet_darkroom",
            "quiet_darkroom",
            NULL,
            SCREENSAVE_RENDERER_KIND_GDI,
            SCREENSAVE_RENDERER_KIND_GDI,
            &plan
        ) ||
        plan.filter_treatment != PLASMA_FILTER_TREATMENT_NONE ||
        plan.emulation_treatment != PLASMA_EMULATION_TREATMENT_CRT ||
        plan.accent_treatment != PLASMA_ACCENT_TREATMENT_NONE
    ) {
        return 438;
    }
    if (
        !plasma_compile_plan_for_renderer(
            module,
            "midnight_interference",
            "midnight_interference",
            NULL,
            SCREENSAVE_RENDERER_KIND_GDI,
            SCREENSAVE_RENDERER_KIND_GDI,
            &plan
        ) ||
        plan.filter_treatment != PLASMA_FILTER_TREATMENT_EMBOSS_EDGE ||
        plan.emulation_treatment != PLASMA_EMULATION_TREATMENT_CRT ||
        plan.accent_treatment != PLASMA_ACCENT_TREATMENT_ACCENT_PASS
    ) {
        return 439;
    }
    if (
        !plasma_compile_plan_for_renderer(
            module,
            "amber_terminal",
            "amber_terminal",
            NULL,
            SCREENSAVE_RENDERER_KIND_GDI,
            SCREENSAVE_RENDERER_KIND_GDI,
            &plan
        ) ||
        plan.filter_treatment != PLASMA_FILTER_TREATMENT_GLOW_EDGE ||
        plan.emulation_treatment != PLASMA_EMULATION_TREATMENT_PHOSPHOR ||
        plan.accent_treatment != PLASMA_ACCENT_TREATMENT_ACCENT_PASS
    ) {
        return 440;
    }

    if (
        !plasma_compile_plan_for_renderer(
            module,
            PLASMA_DEFAULT_PRESET_KEY,
            NULL,
            NULL,
            SCREENSAVE_RENDERER_KIND_GL21,
            SCREENSAVE_RENDERER_KIND_GL21,
            &plan
        )
    ) {
        return 32;
    }
    if (
        !plasma_plan_validate(&plan, module) ||
        plan.requested_renderer_kind != SCREENSAVE_RENDERER_KIND_GL21 ||
        plan.active_renderer_kind != SCREENSAVE_RENDERER_KIND_GL21 ||
        !plan.advanced_capable ||
        !plan.advanced_requested ||
        !plan.advanced_enabled ||
        plan.advanced_degraded ||
        plan.advanced_components != PLASMA_SMOKE_ADVANCED_COMPONENTS ||
        plan.filter_treatment != PLASMA_FILTER_TREATMENT_NONE ||
        plan.accent_treatment != PLASMA_ACCENT_TREATMENT_NONE ||
        !plan.modern_capable ||
        plan.modern_requested ||
        plan.modern_enabled ||
        plan.modern_degraded ||
        plan.modern_components != 0UL ||
        !plan.premium_capable ||
        plan.premium_requested ||
        plan.premium_enabled ||
        plan.premium_degraded ||
        plan.premium_components != 0UL ||
        plasma_plan_is_lower_band_baseline(&plan) ||
        !plasma_plan_validate_for_renderer_kind(&plan, module, SCREENSAVE_RENDERER_KIND_GL21) ||
        plasma_plan_validate_for_renderer_kind(&plan, module, SCREENSAVE_RENDERER_KIND_GL33) ||
        plasma_plan_validate_for_renderer_kind(&plan, module, SCREENSAVE_RENDERER_KIND_GL46) ||
        plasma_plan_validate_for_renderer_kind(&plan, module, SCREENSAVE_RENDERER_KIND_GDI) ||
        plasma_plan_validate_for_renderer_kind(&plan, module, SCREENSAVE_RENDERER_KIND_GL11)
    ) {
        return 33;
    }

    if (
        !plasma_compile_plan_for_renderer(
            module,
            PLASMA_DEFAULT_PRESET_KEY,
            NULL,
            NULL,
            SCREENSAVE_RENDERER_KIND_GL21,
            SCREENSAVE_RENDERER_KIND_GL11,
            &plan
        )
    ) {
        return 34;
    }
    if (
        !plasma_plan_validate(&plan, module) ||
        plan.requested_renderer_kind != SCREENSAVE_RENDERER_KIND_GL21 ||
        plan.active_renderer_kind != SCREENSAVE_RENDERER_KIND_GL11 ||
        !plan.advanced_capable ||
        !plan.advanced_requested ||
        plan.advanced_enabled ||
        !plan.advanced_degraded ||
        plan.advanced_components != 0UL ||
        plan.filter_treatment != PLASMA_FILTER_TREATMENT_NONE ||
        plan.accent_treatment != PLASMA_ACCENT_TREATMENT_NONE ||
        !plan.modern_capable ||
        plan.modern_requested ||
        plan.modern_enabled ||
        plan.modern_degraded ||
        plan.modern_components != 0UL ||
        !plan.premium_capable ||
        plan.premium_requested ||
        plan.premium_enabled ||
        plan.premium_degraded ||
        plan.premium_components != 0UL ||
        !plasma_plan_is_lower_band_baseline(&plan) ||
        !plasma_plan_validate_for_renderer_kind(&plan, module, SCREENSAVE_RENDERER_KIND_GDI) ||
        !plasma_plan_validate_for_renderer_kind(&plan, module, SCREENSAVE_RENDERER_KIND_GL11) ||
        plasma_plan_validate_for_renderer_kind(&plan, module, SCREENSAVE_RENDERER_KIND_GL21) ||
        plasma_plan_validate_for_renderer_kind(&plan, module, SCREENSAVE_RENDERER_KIND_GL33)
    ) {
        return 35;
    }

    if (
        !plasma_compile_plan_for_renderer(
            module,
            PLASMA_DEFAULT_PRESET_KEY,
            NULL,
            NULL,
            SCREENSAVE_RENDERER_KIND_GL33,
            SCREENSAVE_RENDERER_KIND_GL33,
            &plan
        )
    ) {
        return 36;
    }
    if (
        !plasma_plan_validate(&plan, module) ||
        plan.requested_renderer_kind != SCREENSAVE_RENDERER_KIND_GL33 ||
        plan.active_renderer_kind != SCREENSAVE_RENDERER_KIND_GL33 ||
        !plan.advanced_capable ||
        !plan.advanced_requested ||
        !plan.advanced_enabled ||
        plan.advanced_degraded ||
        plan.advanced_components != PLASMA_SMOKE_ADVANCED_COMPONENTS ||
        !plan.modern_capable ||
        !plan.modern_requested ||
        !plan.modern_enabled ||
        plan.modern_degraded ||
        plan.modern_components != PLASMA_SMOKE_MODERN_COMPONENTS ||
        !plan.premium_capable ||
        plan.premium_requested ||
        plan.premium_enabled ||
        plan.premium_degraded ||
        plan.premium_components != 0UL ||
        plan.filter_treatment != PLASMA_FILTER_TREATMENT_NONE ||
        plan.accent_treatment != PLASMA_ACCENT_TREATMENT_NONE ||
        plasma_plan_is_lower_band_baseline(&plan) ||
        !plasma_plan_validate_for_renderer_kind(&plan, module, SCREENSAVE_RENDERER_KIND_GL33) ||
        plasma_plan_validate_for_renderer_kind(&plan, module, SCREENSAVE_RENDERER_KIND_GL21) ||
        plasma_plan_validate_for_renderer_kind(&plan, module, SCREENSAVE_RENDERER_KIND_GL46) ||
        plasma_plan_validate_for_renderer_kind(&plan, module, SCREENSAVE_RENDERER_KIND_GDI) ||
        plasma_plan_validate_for_renderer_kind(&plan, module, SCREENSAVE_RENDERER_KIND_GL11)
    ) {
        return 37;
    }

    if (
        !plasma_compile_plan_for_renderer(
            module,
            PLASMA_DEFAULT_PRESET_KEY,
            NULL,
            NULL,
            SCREENSAVE_RENDERER_KIND_GL33,
            SCREENSAVE_RENDERER_KIND_GL21,
            &plan
        )
    ) {
        return 38;
    }
    if (
        !plasma_plan_validate(&plan, module) ||
        plan.requested_renderer_kind != SCREENSAVE_RENDERER_KIND_GL33 ||
        plan.active_renderer_kind != SCREENSAVE_RENDERER_KIND_GL21 ||
        !plan.advanced_capable ||
        !plan.advanced_requested ||
        !plan.advanced_enabled ||
        plan.advanced_degraded ||
        !plan.modern_capable ||
        !plan.modern_requested ||
        plan.modern_enabled ||
        !plan.modern_degraded ||
        plan.modern_components != 0UL ||
        !plan.premium_capable ||
        plan.premium_requested ||
        plan.premium_enabled ||
        plan.premium_degraded ||
        plan.premium_components != 0UL ||
        !plasma_plan_validate_for_renderer_kind(&plan, module, SCREENSAVE_RENDERER_KIND_GL21) ||
        plasma_plan_validate_for_renderer_kind(&plan, module, SCREENSAVE_RENDERER_KIND_GL33)
    ) {
        return 39;
    }

    if (
        !plasma_compile_plan_for_renderer(
            module,
            PLASMA_DEFAULT_PRESET_KEY,
            NULL,
            NULL,
            SCREENSAVE_RENDERER_KIND_GL33,
            SCREENSAVE_RENDERER_KIND_GL11,
            &plan
        )
    ) {
        return 40;
    }
    if (
        !plasma_plan_validate(&plan, module) ||
        plan.requested_renderer_kind != SCREENSAVE_RENDERER_KIND_GL33 ||
        plan.active_renderer_kind != SCREENSAVE_RENDERER_KIND_GL11 ||
        !plan.advanced_requested ||
        plan.advanced_enabled ||
        !plan.advanced_degraded ||
        !plan.modern_requested ||
        plan.modern_enabled ||
        !plan.modern_degraded ||
        !plan.premium_capable ||
        plan.premium_requested ||
        plan.premium_enabled ||
        plan.premium_degraded ||
        plan.premium_components != 0UL ||
        !plasma_plan_is_lower_band_baseline(&plan) ||
        !plasma_plan_validate_for_renderer_kind(&plan, module, SCREENSAVE_RENDERER_KIND_GDI) ||
        !plasma_plan_validate_for_renderer_kind(&plan, module, SCREENSAVE_RENDERER_KIND_GL11) ||
        plasma_plan_validate_for_renderer_kind(&plan, module, SCREENSAVE_RENDERER_KIND_GL21) ||
        plasma_plan_validate_for_renderer_kind(&plan, module, SCREENSAVE_RENDERER_KIND_GL33)
    ) {
        return 41;
    }

    if (
        !plasma_compile_plan_for_renderer(
            module,
            PLASMA_DEFAULT_PRESET_KEY,
            NULL,
            NULL,
            SCREENSAVE_RENDERER_KIND_GL46,
            SCREENSAVE_RENDERER_KIND_GL46,
            &plan
        )
    ) {
        return 42;
    }
    if (
        !plasma_plan_validate(&plan, module) ||
        plan.requested_renderer_kind != SCREENSAVE_RENDERER_KIND_GL46 ||
        plan.active_renderer_kind != SCREENSAVE_RENDERER_KIND_GL46 ||
        !plan.advanced_capable ||
        !plan.advanced_requested ||
        !plan.advanced_enabled ||
        !plan.modern_capable ||
        !plan.modern_requested ||
        !plan.modern_enabled ||
        !plan.premium_capable ||
        !plan.premium_requested ||
        !plan.premium_enabled ||
        plan.premium_degraded ||
        plan.premium_components != PLASMA_SMOKE_PREMIUM_COMPONENTS ||
        plan.presentation_mode != PLASMA_PRESENTATION_MODE_FLAT ||
        !plasma_plan_validate_for_renderer_kind(&plan, module, SCREENSAVE_RENDERER_KIND_GL46) ||
        plasma_plan_validate_for_renderer_kind(&plan, module, SCREENSAVE_RENDERER_KIND_GL33) ||
        plasma_plan_validate_for_renderer_kind(&plan, module, SCREENSAVE_RENDERER_KIND_GL21)
    ) {
        return 43;
    }

    if (
        !plasma_compile_plan_for_renderer(
            module,
            PLASMA_DEFAULT_PRESET_KEY,
            NULL,
            NULL,
            SCREENSAVE_RENDERER_KIND_GL46,
            SCREENSAVE_RENDERER_KIND_GL33,
            &plan
        )
    ) {
        return 44;
    }
    if (
        !plasma_plan_validate(&plan, module) ||
        plan.requested_renderer_kind != SCREENSAVE_RENDERER_KIND_GL46 ||
        plan.active_renderer_kind != SCREENSAVE_RENDERER_KIND_GL33 ||
        !plan.advanced_enabled ||
        !plan.modern_enabled ||
        !plan.premium_capable ||
        !plan.premium_requested ||
        plan.premium_enabled ||
        !plan.premium_degraded ||
        plan.premium_components != 0UL ||
        plan.presentation_mode != PLASMA_PRESENTATION_MODE_FLAT ||
        !plasma_plan_validate_for_renderer_kind(&plan, module, SCREENSAVE_RENDERER_KIND_GL33) ||
        plasma_plan_validate_for_renderer_kind(&plan, module, SCREENSAVE_RENDERER_KIND_GL46)
    ) {
        return 45;
    }

    if (
        !plasma_compile_plan_for_renderer(
            module,
            PLASMA_DEFAULT_PRESET_KEY,
            NULL,
            NULL,
            SCREENSAVE_RENDERER_KIND_GL46,
            SCREENSAVE_RENDERER_KIND_GL21,
            &plan
        )
    ) {
        return 46;
    }
    if (
        !plasma_plan_validate(&plan, module) ||
        plan.requested_renderer_kind != SCREENSAVE_RENDERER_KIND_GL46 ||
        plan.active_renderer_kind != SCREENSAVE_RENDERER_KIND_GL21 ||
        !plan.advanced_enabled ||
        !plan.modern_requested ||
        plan.modern_enabled ||
        !plan.modern_degraded ||
        !plan.premium_requested ||
        plan.premium_enabled ||
        !plan.premium_degraded ||
        plan.presentation_mode != PLASMA_PRESENTATION_MODE_FLAT ||
        !plasma_plan_validate_for_renderer_kind(&plan, module, SCREENSAVE_RENDERER_KIND_GL21) ||
        plasma_plan_validate_for_renderer_kind(&plan, module, SCREENSAVE_RENDERER_KIND_GL46)
    ) {
        return 47;
    }

    if (
        !plasma_compile_plan_for_renderer(
            module,
            PLASMA_DEFAULT_PRESET_KEY,
            NULL,
            NULL,
            SCREENSAVE_RENDERER_KIND_GL46,
            SCREENSAVE_RENDERER_KIND_GL11,
            &plan
        )
    ) {
        return 48;
    }
    if (
        !plasma_plan_validate(&plan, module) ||
        plan.requested_renderer_kind != SCREENSAVE_RENDERER_KIND_GL46 ||
        plan.active_renderer_kind != SCREENSAVE_RENDERER_KIND_GL11 ||
        !plan.advanced_requested ||
        plan.advanced_enabled ||
        !plan.advanced_degraded ||
        !plan.modern_requested ||
        plan.modern_enabled ||
        !plan.modern_degraded ||
        !plan.premium_requested ||
        plan.premium_enabled ||
        !plan.premium_degraded ||
        !plasma_plan_is_lower_band_baseline(&plan) ||
        !plasma_plan_validate_for_renderer_kind(&plan, module, SCREENSAVE_RENDERER_KIND_GL11) ||
        plasma_plan_validate_for_renderer_kind(&plan, module, SCREENSAVE_RENDERER_KIND_GL46)
    ) {
        return 49;
    }

    plasma_config_set_defaults(&common_config, &product_config, sizeof(product_config));
    plasma_apply_preset_to_config("plasma_lava", &common_config, &product_config);
    common_config.preset_key = "plasma_lava";
    common_config.theme_key = "plasma_lava";
    common_config.detail_level = SCREENSAVE_DETAIL_LEVEL_HIGH;
    product_config.effect_mode = PLASMA_EFFECT_AURORA;
    product_config.speed_mode = PLASMA_SPEED_LIVELY;
    product_config.resolution_mode = PLASMA_RESOLUTION_FINE;
    product_config.smoothing_mode = PLASMA_SMOOTHING_GLOW;
    product_config.output_family = PLASMA_OUTPUT_FAMILY_CONTOUR;
    product_config.output_mode = PLASMA_OUTPUT_MODE_CONTOUR_BANDS;
    product_config.filter_treatment = PLASMA_FILTER_TREATMENT_GLOW_EDGE;
    product_config.emulation_treatment = PLASMA_EMULATION_TREATMENT_PHOSPHOR;
    product_config.accent_treatment = PLASMA_ACCENT_TREATMENT_ACCENT_PASS;
    product_config.presentation_mode = PLASMA_PRESENTATION_MODE_HEIGHTFIELD;
    if (
        !plasma_compile_configured_plan_for_renderer(
            module,
            &common_config,
            &product_config,
            SCREENSAVE_RENDERER_KIND_GL46,
            SCREENSAVE_RENDERER_KIND_GL46,
            &plan
        ) ||
        plan.requested_detail_level != SCREENSAVE_DETAIL_LEVEL_HIGH ||
        plan.requested_effect_mode != PLASMA_EFFECT_AURORA ||
        plan.requested_speed_mode != PLASMA_SPEED_LIVELY ||
        plan.requested_resolution_mode != PLASMA_RESOLUTION_FINE ||
        plan.requested_smoothing_mode != PLASMA_SMOOTHING_GLOW ||
        plan.requested_output_family != PLASMA_OUTPUT_FAMILY_CONTOUR ||
        plan.requested_output_mode != PLASMA_OUTPUT_MODE_CONTOUR_BANDS ||
        plan.requested_filter_treatment != PLASMA_FILTER_TREATMENT_GLOW_EDGE ||
        plan.requested_emulation_treatment != PLASMA_EMULATION_TREATMENT_PHOSPHOR ||
        plan.requested_accent_treatment != PLASMA_ACCENT_TREATMENT_ACCENT_PASS ||
        plan.requested_presentation_mode != PLASMA_PRESENTATION_MODE_HEIGHTFIELD ||
        plan.detail_level != SCREENSAVE_DETAIL_LEVEL_HIGH ||
        plan.effect_mode != PLASMA_EFFECT_AURORA ||
        plan.speed_mode != PLASMA_SPEED_LIVELY ||
        plan.resolution_mode != PLASMA_RESOLUTION_FINE ||
        plan.smoothing_mode != PLASMA_SMOOTHING_GLOW ||
        plan.output_family != PLASMA_OUTPUT_FAMILY_CONTOUR ||
        plan.output_mode != PLASMA_OUTPUT_MODE_CONTOUR_BANDS ||
        plan.filter_treatment != PLASMA_FILTER_TREATMENT_GLOW_EDGE ||
        plan.emulation_treatment != PLASMA_EMULATION_TREATMENT_PHOSPHOR ||
        plan.accent_treatment != PLASMA_ACCENT_TREATMENT_ACCENT_PASS ||
        plan.presentation_mode != PLASMA_PRESENTATION_MODE_HEIGHTFIELD ||
        !plan.premium_requested ||
        !plan.premium_enabled ||
        plan.premium_degraded ||
        !plasma_plan_validate_for_renderer_kind(&plan, module, SCREENSAVE_RENDERER_KIND_GL46)
    ) {
        return 429;
    }
    if (
        !plasma_compile_configured_plan_for_renderer(
            module,
            &common_config,
            &product_config,
            SCREENSAVE_RENDERER_KIND_GL46,
            SCREENSAVE_RENDERER_KIND_GL11,
            &plan
        ) ||
        plan.requested_effect_mode != PLASMA_EFFECT_AURORA ||
        plan.requested_output_family != PLASMA_OUTPUT_FAMILY_CONTOUR ||
        plan.requested_output_mode != PLASMA_OUTPUT_MODE_CONTOUR_BANDS ||
        plan.requested_filter_treatment != PLASMA_FILTER_TREATMENT_GLOW_EDGE ||
        plan.requested_presentation_mode != PLASMA_PRESENTATION_MODE_HEIGHTFIELD ||
        plan.effect_mode != PLASMA_EFFECT_AURORA ||
        plan.output_family != PLASMA_OUTPUT_FAMILY_CONTOUR ||
        plan.output_mode != PLASMA_OUTPUT_MODE_CONTOUR_BANDS ||
        plan.filter_treatment != PLASMA_FILTER_TREATMENT_GLOW_EDGE ||
        plan.presentation_mode != PLASMA_PRESENTATION_MODE_FLAT ||
        !plan.premium_requested ||
        plan.premium_enabled ||
        !plan.premium_degraded ||
        !plasma_plan_validate_for_renderer_kind(&plan, module, SCREENSAVE_RENDERER_KIND_GL11)
    ) {
        return 430;
    }

    plasma_config_set_defaults(&common_config, &product_config, sizeof(product_config));
    plasma_apply_preset_to_config("plasma_lava", &common_config, &product_config);
    common_config.preset_key = "plasma_lava";
    common_config.theme_key = "plasma_lava";
    product_config.filter_treatment = PLASMA_FILTER_TREATMENT_BLUR;
    product_config.accent_treatment = PLASMA_ACCENT_TREATMENT_OVERLAY_PASS;
    if (
        !plasma_compile_configured_plan_for_renderer(
            module,
            &common_config,
            &product_config,
            SCREENSAVE_RENDERER_KIND_GL21,
            SCREENSAVE_RENDERER_KIND_GL21,
            &plan
        ) ||
        plan.requested_filter_treatment != PLASMA_FILTER_TREATMENT_BLUR ||
        plan.requested_accent_treatment != PLASMA_ACCENT_TREATMENT_OVERLAY_PASS ||
        plan.filter_treatment != PLASMA_FILTER_TREATMENT_BLUR ||
        plan.accent_treatment != PLASMA_ACCENT_TREATMENT_OVERLAY_PASS ||
        !plan.advanced_requested ||
        !plan.advanced_enabled ||
        plan.advanced_degraded ||
        !plasma_plan_validate_for_renderer_kind(&plan, module, SCREENSAVE_RENDERER_KIND_GL21)
    ) {
        return 431;
    }
    if (
        !plasma_compile_configured_plan_for_renderer(
            module,
            &common_config,
            &product_config,
            SCREENSAVE_RENDERER_KIND_GL21,
            SCREENSAVE_RENDERER_KIND_GL11,
            &plan
        ) ||
        plan.requested_filter_treatment != PLASMA_FILTER_TREATMENT_BLUR ||
        plan.requested_accent_treatment != PLASMA_ACCENT_TREATMENT_OVERLAY_PASS ||
        plan.filter_treatment != PLASMA_FILTER_TREATMENT_NONE ||
        plan.accent_treatment != PLASMA_ACCENT_TREATMENT_NONE ||
        !plan.advanced_requested ||
        plan.advanced_enabled ||
        !plan.advanced_degraded ||
        !plasma_plan_is_lower_band_baseline(&plan) ||
        !plasma_plan_validate_for_renderer_kind(&plan, module, SCREENSAVE_RENDERER_KIND_GL11)
    ) {
        return 432;
    }
    screensave_config_binding_init(&binding, &common_config, &product_config, sizeof(product_config));
    ZeroMemory(&environment, sizeof(environment));
    fake_size.width = 320;
    fake_size.height = 240;
    plasma_smoke_init_fake_renderer(
        &fake_renderer,
        SCREENSAVE_RENDERER_KIND_GL21,
        SCREENSAVE_RENDERER_KIND_GL11,
        &fake_size
    );
    environment.mode = SCREENSAVE_SESSION_MODE_WINDOWED;
    environment.drawable_size = fake_size;
    environment.seed.base_seed = 0x51525354UL;
    environment.seed.stream_seed = 0x55565758UL;
    environment.seed.deterministic = common_config.use_deterministic_seed;
    environment.config_binding = &binding;
    environment.renderer = &fake_renderer;
    session = NULL;
    if (!plasma_create_session(module, &session, &environment) || session == NULL) {
        return 459;
    }
    benchlab_config_state.common = common_config;
    benchlab_config_state.product_config = &product_config;
    benchlab_config_state.product_config_size = sizeof(product_config);
    if (
        !plasma_benchlab_build_snapshot(
            session,
            &benchlab_config_state,
            SCREENSAVE_RENDERER_KIND_GL21,
            &benchlab_snapshot
        ) ||
        strcmp(benchlab_snapshot.requested_filter_treatment, "blur") != 0 ||
        strcmp(benchlab_snapshot.filter_treatment, "none") != 0 ||
        strcmp(benchlab_snapshot.requested_accent_treatment, "overlay_pass") != 0 ||
        strcmp(benchlab_snapshot.accent_treatment, "none") != 0
    ) {
        plasma_destroy_session(session);
        return 460;
    }
    if (
        !plasma_benchlab_build_report_section(
            session,
            &benchlab_config_state,
            SCREENSAVE_RENDERER_KIND_GL21,
            benchlab_report,
            (unsigned int)sizeof(benchlab_report)
        ) ||
        strstr(benchlab_report, "Requested filter treatment: blur") == NULL ||
        strstr(benchlab_report, "Filter treatment: none") == NULL ||
        strstr(benchlab_report, "Requested accent treatment: overlay_pass") == NULL ||
        strstr(benchlab_report, "Accent treatment: none") == NULL
    ) {
        plasma_destroy_session(session);
        return 461;
    }
    plasma_destroy_session(session);

    for (index = 0U; index < (unsigned int)(sizeof(g_required_preset_keys) / sizeof(g_required_preset_keys[0])); ++index) {
        if (!plasma_compile_direct_plan(module, g_required_preset_keys[index], NULL, &plan)) {
            fprintf(stderr, "smoke: failed to compile preset plan for %s\n", g_required_preset_keys[index]);
            return 21;
        }
        preset_descriptor = plasma_find_preset_descriptor(g_required_preset_keys[index]);
        if (
            preset_descriptor == NULL ||
            plan.preset_key == NULL ||
            plan.theme_key == NULL ||
            strcmp(plan.preset_key, preset_descriptor->preset_key) != 0 ||
            strcmp(plan.theme_key, preset_descriptor->theme_key) != 0 ||
            !plasma_plan_validate_for_renderer_kind(&plan, module, SCREENSAVE_RENDERER_KIND_GDI) ||
            !plasma_plan_validate_for_renderer_kind(&plan, module, SCREENSAVE_RENDERER_KIND_GL11)
        ) {
            fprintf(
                stderr,
                "smoke: preset %s failed validation desc=%p plan_preset=%s plan_theme=%s desc_theme=%s gdi_ok=%d gl11_ok=%d active=%d adv=%d mod=%d prem=%d output=%d/%d filter=%d accent=%d presentation=%d\n",
                g_required_preset_keys[index],
                (const void *)preset_descriptor,
                plan.preset_key != NULL ? plan.preset_key : "(null)",
                plan.theme_key != NULL ? plan.theme_key : "(null)",
                preset_descriptor != NULL && preset_descriptor->theme_key != NULL
                    ? preset_descriptor->theme_key
                    : "(null)",
                plasma_plan_validate_for_renderer_kind(&plan, module, SCREENSAVE_RENDERER_KIND_GDI),
                plasma_plan_validate_for_renderer_kind(&plan, module, SCREENSAVE_RENDERER_KIND_GL11),
                (int)plan.active_renderer_kind,
                plan.advanced_enabled,
                plan.modern_enabled,
                plan.premium_enabled,
                (int)plan.output_family,
                (int)plan.output_mode,
                (int)plan.filter_treatment,
                (int)plan.accent_treatment,
                (int)plan.presentation_mode
            );
            return 22;
        }
    }

    registry = plasma_content_get_registry();
    if (
        !plasma_content_registry_validate() ||
        registry == NULL ||
        registry->preset_count != PLASMA_PRESET_COUNT ||
        registry->theme_count != PLASMA_THEME_COUNT ||
        registry->pack_count != 1U ||
        registry->preset_set_count != 7U ||
        registry->theme_set_count != 4U ||
        !plasma_content_registry_has_channel(PLASMA_CONTENT_CHANNEL_STABLE) ||
        !plasma_content_registry_has_channel(PLASMA_CONTENT_CHANNEL_EXPERIMENTAL)
    ) {
        return 101;
    }

    if (
        plasma_content_find_preset_set("fire_classics") == NULL ||
        plasma_content_find_preset_set("plasma_classics") == NULL ||
        plasma_content_find_preset_set("interference_classics") == NULL ||
        plasma_content_find_preset_set("warm_bridge_classics") == NULL ||
        plasma_content_find_preset_set("cool_bridge_classics") == NULL ||
        plasma_content_find_theme_set("warm_classics") == NULL ||
        plasma_content_find_theme_set("cool_classics") == NULL
    ) {
        return 102;
    }

    pack_entry = plasma_content_find_pack_entry("lava_remix");
    if (
        pack_entry == NULL ||
        strcmp(pack_entry->pack_key, "lava_remix") != 0 ||
        pack_entry->source != PLASMA_CONTENT_SOURCE_PACK ||
        pack_entry->channel != PLASMA_CONTENT_CHANNEL_STABLE ||
        pack_entry->minimum_kind != SCREENSAVE_RENDERER_KIND_GDI ||
        pack_entry->preferred_kind != SCREENSAVE_RENDERER_KIND_GL11 ||
        pack_entry->quality_class != SCREENSAVE_CAPABILITY_QUALITY_BALANCED ||
        pack_entry->asset_count != 2U ||
        strcmp(pack_entry->assets[0].canonical_key, "plasma_lava") != 0 ||
        strcmp(pack_entry->assets[1].canonical_key, "plasma_lava") != 0 ||
        plasma_content_find_preset_entry("plasma_lava") == NULL ||
        !plasma_content_find_preset_entry("plasma_lava")->advanced_capable ||
        !plasma_content_find_preset_entry("plasma_lava")->modern_capable ||
        !plasma_content_find_preset_entry("plasma_lava")->premium_capable
    ) {
        return 103;
    }

    ZeroMemory(&pack_manifest, sizeof(pack_manifest));
    if (
        !screensave_pack_manifest_load(
            "products/savers/plasma/packs/lava_remix/pack.ini",
            SCREENSAVE_PACK_SOURCE_BUILTIN,
            &pack_manifest,
            NULL
        ) ||
        strcmp(pack_manifest.pack_key, "lava_remix") != 0 ||
        strcmp(pack_manifest.product_key, "plasma") != 0 ||
        pack_manifest.preset_file_count != 1U ||
        pack_manifest.theme_file_count != 1U ||
        strcmp(pack_manifest.preset_files[0], "presets/lava_remix.preset.ini") != 0 ||
        strcmp(pack_manifest.theme_files[0], "themes/lava_remix.theme.ini") != 0
    ) {
        return 104;
    }

    if (
        !plasma_smoke_resolve_repo_root(repo_root, (unsigned int)sizeof(repo_root)) ||
        !plasma_authoring_validate_repo_surface(
            repo_root,
            authoring_message,
            (unsigned int)sizeof(authoring_message))
    ) {
        fprintf(stderr, "smoke: authoring surface validation failed repo_root=%s message=%s\n", repo_root, authoring_message);
        return 243;
    }

    if (
        product_config.selection.content_filter != PLASMA_CONTENT_FILTER_STABLE_ONLY ||
        product_config.selection.favorites_only != 0 ||
        product_config.selection.preset_set_key[0] != '\0' ||
        product_config.selection.theme_set_key[0] != '\0' ||
        strcmp(product_config.selection.favorite_preset_keys, "none") != 0 ||
        strcmp(product_config.selection.excluded_preset_keys, "none") != 0 ||
        strcmp(product_config.selection.favorite_theme_keys, "none") != 0 ||
        strcmp(product_config.selection.excluded_theme_keys, "none") != 0
    ) {
        return 105;
    }

    if (
        !plasma_compile_direct_plan(module, PLASMA_DEFAULT_PRESET_KEY, NULL, &plan) ||
        plan.selection.selected_preset == NULL ||
        plan.selection.selected_theme == NULL ||
        plan.selection.active_preset_set != NULL ||
        plan.selection.active_theme_set != NULL ||
        plan.selection.content_filter != PLASMA_CONTENT_FILTER_STABLE_ONLY ||
        plan.selection.favorites_only_requested != 0 ||
        plan.selection.favorites_only_applied != 0 ||
        plan.selection.favorite_preset_mask != 0UL ||
        plan.selection.excluded_preset_mask != 0UL ||
        plan.selection.favorite_theme_mask != 0UL ||
        plan.selection.excluded_theme_mask != 0UL ||
        strcmp(plan.selection.selected_preset->preset_key, PLASMA_DEFAULT_PRESET_KEY) != 0 ||
        strcmp(plan.selection.selected_theme->theme_key, PLASMA_DEFAULT_THEME_KEY) != 0
    ) {
        return 106;
    }

    plasma_selection_preferences_set_defaults(&selection_preferences);
    lstrcpyA(selection_preferences.preset_set_key, "ghost_set");
    lstrcpyA(selection_preferences.theme_set_key, "ghost_set");
    lstrcpyA(selection_preferences.favorite_preset_keys, "ghost,ember_lava");
    lstrcpyA(selection_preferences.excluded_preset_keys, "ghost");
    lstrcpyA(selection_preferences.favorite_theme_keys, "ghost,ember_lava");
    lstrcpyA(selection_preferences.excluded_theme_keys, "ghost");
    plasma_selection_preferences_clamp(&selection_preferences);
    if (
        selection_preferences.preset_set_key[0] != '\0' ||
        selection_preferences.theme_set_key[0] != '\0' ||
        strcmp(selection_preferences.favorite_preset_keys, "plasma_lava") != 0 ||
        strcmp(selection_preferences.excluded_preset_keys, "none") != 0 ||
        strcmp(selection_preferences.favorite_theme_keys, "plasma_lava") != 0 ||
        strcmp(selection_preferences.excluded_theme_keys, "none") != 0
    ) {
        return 107;
    }

    plasma_selection_preferences_set_defaults(&selection_preferences);
    lstrcpyA(selection_preferences.preset_set_key, "dark_room_classics");
    lstrcpyA(selection_preferences.theme_set_key, "dark_room_classics");
    if (
        !plasma_compile_selection_plan(
            module,
            PLASMA_DEFAULT_PRESET_KEY,
            PLASMA_DEFAULT_THEME_KEY,
            &selection_preferences,
            &plan
        ) ||
        plan.selection.active_preset_set == NULL ||
        plan.selection.active_theme_set == NULL ||
        strcmp(plan.selection.active_preset_set->set_key, "dark_room_classics") != 0 ||
        strcmp(plan.selection.active_theme_set->set_key, "dark_room_classics") != 0 ||
        strcmp(plan.preset_key, PLASMA_DEFAULT_PRESET_KEY) != 0 ||
        strcmp(plan.theme_key, PLASMA_DEFAULT_THEME_KEY) != 0 ||
        !plan.selection.explicit_preset_preserved ||
        !plan.selection.explicit_theme_preserved ||
        plan.selection.favorites_only_requested != 0 ||
        plan.selection.favorites_only_applied != 0
    ) {
        return 108;
    }

    plasma_selection_preferences_set_defaults(&selection_preferences);
    selection_preferences.favorites_only = 1;
    lstrcpyA(selection_preferences.favorite_preset_keys, "quiet_darkroom,amber_terminal");
    lstrcpyA(selection_preferences.excluded_preset_keys, "quiet_darkroom");
    lstrcpyA(selection_preferences.favorite_theme_keys, "quiet_darkroom,amber_terminal");
    lstrcpyA(selection_preferences.excluded_theme_keys, "quiet_darkroom");
    if (
        !plasma_compile_selection_plan(
            module,
            PLASMA_DEFAULT_PRESET_KEY,
            PLASMA_DEFAULT_THEME_KEY,
            &selection_preferences,
            &plan
        ) ||
        strcmp(plan.preset_key, PLASMA_DEFAULT_PRESET_KEY) != 0 ||
        strcmp(plan.theme_key, PLASMA_DEFAULT_THEME_KEY) != 0 ||
        !plan.selection.explicit_preset_preserved ||
        !plan.selection.explicit_theme_preserved ||
        !plan.selection.favorites_only_requested ||
        plan.selection.favorites_only_applied
    ) {
        return 109;
    }

    plasma_selection_preferences_set_defaults(&selection_preferences);
    lstrcpyA(selection_preferences.preset_set_key, "classic_core");
    lstrcpyA(selection_preferences.theme_set_key, "classic_core");
    if (
        !plasma_compile_selection_plan(
            module,
            "ghost_preset",
            "ghost_theme",
            &selection_preferences,
            &plan
        ) ||
        strcmp(plan.preset_key, "plasma_lava") != 0 ||
        strcmp(plan.theme_key, "plasma_lava") != 0
    ) {
        fprintf(
            stderr,
            "smoke: ghost classic_core fallback failed preset=%s theme=%s\n",
            plan.preset_key != NULL ? plan.preset_key : "(null)",
            plan.theme_key != NULL ? plan.theme_key : "(null)"
        );
        return 244;
    }

    plasma_selection_preferences_set_defaults(&selection_preferences);
    lstrcpyA(selection_preferences.preset_set_key, "classic_core");
    lstrcpyA(selection_preferences.theme_set_key, "classic_core");
    lstrcpyA(selection_preferences.excluded_preset_keys, "plasma_lava");
    lstrcpyA(selection_preferences.excluded_theme_keys, "plasma_lava");
    if (
        !plasma_compile_selection_plan(
            module,
            "ghost_preset",
            "ghost_theme",
            &selection_preferences,
            &plan
        ) ||
        strcmp(plan.preset_key, "quiet_darkroom") != 0 ||
        strcmp(plan.theme_key, "quiet_darkroom") != 0
    ) {
        fprintf(
            stderr,
            "smoke: ghost fallback with exclusions failed preset=%s theme=%s\n",
            plan.preset_key != NULL ? plan.preset_key : "(null)",
            plan.theme_key != NULL ? plan.theme_key : "(null)"
        );
        return 245;
    }

    plasma_selection_preferences_set_defaults(&selection_preferences);
    selection_preferences.content_filter = PLASMA_CONTENT_FILTER_EXPERIMENTAL_ONLY;
    if (
        !plasma_compile_selection_plan(
            module,
            PLASMA_DEFAULT_PRESET_KEY,
            PLASMA_DEFAULT_THEME_KEY,
            &selection_preferences,
            &plan
        ) ||
        plan.selection.content_filter != PLASMA_CONTENT_FILTER_EXPERIMENTAL_ONLY ||
        strcmp(plan.preset_key, PLASMA_DEFAULT_PRESET_KEY) != 0 ||
        strcmp(plan.theme_key, PLASMA_DEFAULT_THEME_KEY) != 0 ||
        !plan.selection.explicit_preset_preserved ||
        !plan.selection.explicit_theme_preserved
    ) {
        return 110;
    }

    plasma_config_set_defaults(&common_config, &product_config, sizeof(product_config));
    if (
        !plasma_config_import_settings_entry(
            module,
            &common_config,
            &product_config,
            sizeof(product_config),
            SCREENSAVE_SETTINGS_FILE_PRESET,
            "content",
            "content_filter",
            "stable_and_experimental",
            NULL
        ) ||
        !plasma_config_import_settings_entry(
            module,
            &common_config,
            &product_config,
            sizeof(product_config),
            SCREENSAVE_SETTINGS_FILE_PRESET,
            "content",
            "favorites_only",
            "true",
            NULL
        ) ||
        !plasma_config_import_settings_entry(
            module,
            &common_config,
            &product_config,
            sizeof(product_config),
            SCREENSAVE_SETTINGS_FILE_PRESET,
            "content",
            "favorite_preset_keys",
            "ember_lava,amber_terminal,ghost",
            NULL
        ) ||
        !plasma_config_import_settings_entry(
            module,
            &common_config,
            &product_config,
            sizeof(product_config),
            SCREENSAVE_SETTINGS_FILE_PRESET,
            "content",
            "favorite_theme_keys",
            "ember_lava,amber_terminal,ghost",
            NULL
        )
    ) {
        return 111;
    }

    plasma_config_clamp(&common_config, &product_config, sizeof(product_config));
    if (
        product_config.selection.content_filter != PLASMA_CONTENT_FILTER_STABLE_AND_EXPERIMENTAL ||
        !product_config.selection.favorites_only ||
        strcmp(product_config.selection.favorite_preset_keys, "plasma_lava,amber_terminal") != 0 ||
        strcmp(product_config.selection.excluded_preset_keys, "none") != 0 ||
        strcmp(product_config.selection.favorite_theme_keys, "plasma_lava,amber_terminal") != 0 ||
        strcmp(product_config.selection.excluded_theme_keys, "none") != 0 ||
        common_config.preset_key == NULL ||
        common_config.theme_key == NULL ||
        strcmp(common_config.preset_key, PLASMA_DEFAULT_PRESET_KEY) != 0 ||
        strcmp(common_config.theme_key, PLASMA_DEFAULT_THEME_KEY) != 0
    ) {
        return 112;
    }

    ZeroMemory(&settings_capture, sizeof(settings_capture));
    ZeroMemory(&settings_writer, sizeof(settings_writer));
    settings_writer.context = &settings_capture;
    settings_writer.write_string = plasma_smoke_capture_write_string;
    if (
        !plasma_config_export_settings_entries(
            module,
            &common_config,
            &product_config,
            sizeof(product_config),
            SCREENSAVE_SETTINGS_FILE_PRESET,
            &settings_writer,
            NULL
        ) ||
        !plasma_smoke_capture_has_string(
            &settings_capture,
            "content",
            "content_filter",
            "stable_and_experimental"
        ) ||
        !plasma_smoke_capture_has_string(
            &settings_capture,
            "content",
            "favorites_only",
            "true"
        ) ||
        !plasma_smoke_capture_has_string(
            &settings_capture,
            "content",
            "favorite_preset_keys",
            "plasma_lava,amber_terminal"
        ) ||
        !plasma_smoke_capture_has_string(
            &settings_capture,
            "content",
            "favorite_theme_keys",
            "plasma_lava,amber_terminal"
        )
    ) {
        return 113;
    }

    plasma_config_set_defaults(&common_config, &product_config, sizeof(product_config));
    if (
        !plasma_config_import_settings_entry(
            module,
            &common_config,
            &product_config,
            sizeof(product_config),
            SCREENSAVE_SETTINGS_FILE_PRESET,
            "product",
            "effect",
            "aurora",
            NULL
        ) ||
        !plasma_config_import_settings_entry(
            module,
            &common_config,
            &product_config,
            sizeof(product_config),
            SCREENSAVE_SETTINGS_FILE_PRESET,
            "product",
            "speed",
            "lively",
            NULL
        ) ||
        !plasma_config_import_settings_entry(
            module,
            &common_config,
            &product_config,
            sizeof(product_config),
            SCREENSAVE_SETTINGS_FILE_PRESET,
            "product",
            "output_family",
            "contour",
            NULL
        ) ||
        !plasma_config_import_settings_entry(
            module,
            &common_config,
            &product_config,
            sizeof(product_config),
            SCREENSAVE_SETTINGS_FILE_PRESET,
            "product",
            "output_mode",
            "contour_bands",
            NULL
        ) ||
        !plasma_config_import_settings_entry(
            module,
            &common_config,
            &product_config,
            sizeof(product_config),
            SCREENSAVE_SETTINGS_FILE_PRESET,
            "product",
            "sampling",
            "none",
            NULL
        ) ||
        !plasma_config_import_settings_entry(
            module,
            &common_config,
            &product_config,
            sizeof(product_config),
            SCREENSAVE_SETTINGS_FILE_PRESET,
            "product",
            "filter",
            "glow_edge",
            NULL
        ) ||
        !plasma_config_import_settings_entry(
            module,
            &common_config,
            &product_config,
            sizeof(product_config),
            SCREENSAVE_SETTINGS_FILE_PRESET,
            "product",
            "emulation",
            "phosphor",
            NULL
        ) ||
        !plasma_config_import_settings_entry(
            module,
            &common_config,
            &product_config,
            sizeof(product_config),
            SCREENSAVE_SETTINGS_FILE_PRESET,
            "product",
            "accent",
            "accent_pass",
            NULL
        ) ||
        !plasma_config_import_settings_entry(
            module,
            &common_config,
            &product_config,
            sizeof(product_config),
            SCREENSAVE_SETTINGS_FILE_PRESET,
            "product",
            "presentation",
            "heightfield",
            NULL
        )
    ) {
        return 426;
    }
    plasma_config_clamp(&common_config, &product_config, sizeof(product_config));
    if (
        product_config.effect_mode != PLASMA_EFFECT_AURORA ||
        product_config.speed_mode != PLASMA_SPEED_LIVELY ||
        product_config.output_family != PLASMA_OUTPUT_FAMILY_CONTOUR ||
        product_config.output_mode != PLASMA_OUTPUT_MODE_CONTOUR_BANDS ||
        product_config.sampling_treatment != PLASMA_SAMPLING_TREATMENT_NONE ||
        product_config.filter_treatment != PLASMA_FILTER_TREATMENT_GLOW_EDGE ||
        product_config.emulation_treatment != PLASMA_EMULATION_TREATMENT_PHOSPHOR ||
        product_config.accent_treatment != PLASMA_ACCENT_TREATMENT_ACCENT_PASS ||
        product_config.presentation_mode != PLASMA_PRESENTATION_MODE_HEIGHTFIELD
    ) {
        return 427;
    }

    ZeroMemory(&settings_capture, sizeof(settings_capture));
    ZeroMemory(&settings_writer, sizeof(settings_writer));
    settings_writer.context = &settings_capture;
    settings_writer.write_string = plasma_smoke_capture_write_string;
    if (
        !plasma_config_export_settings_entries(
            module,
            &common_config,
            &product_config,
            sizeof(product_config),
            SCREENSAVE_SETTINGS_FILE_PRESET,
            &settings_writer,
            NULL
        ) ||
        !plasma_smoke_capture_has_string(
            &settings_capture,
            "product",
            "effect",
            "aurora_curtain_ribbon"
        ) ||
        !plasma_smoke_capture_has_string(&settings_capture, "product", "speed", "lively") ||
        !plasma_smoke_capture_has_string(&settings_capture, "product", "output_family", "contour") ||
        !plasma_smoke_capture_has_string(&settings_capture, "product", "output_mode", "contour_bands") ||
        !plasma_smoke_capture_has_string(&settings_capture, "product", "sampling_treatment", "none") ||
        !plasma_smoke_capture_has_string(&settings_capture, "product", "filter_treatment", "glow_edge") ||
        !plasma_smoke_capture_has_string(&settings_capture, "product", "emulation_treatment", "phosphor") ||
        !plasma_smoke_capture_has_string(&settings_capture, "product", "accent_treatment", "accent_pass") ||
        !plasma_smoke_capture_has_string(&settings_capture, "product", "presentation_mode", "heightfield")
    ) {
        return 428;
    }

    plasma_selection_preferences_set_defaults(&selection_preferences);
    if (
        !plasma_compile_selection_plan(
            module,
            "lava_isolines",
            "plasma_lava",
            &selection_preferences,
            &plan
        ) ||
        strcmp(plan.preset_key, "lava_isolines") != 0 ||
        !plan.selection.explicit_preset_preserved ||
        plan.output_family != PLASMA_OUTPUT_FAMILY_CONTOUR ||
        plan.output_mode != PLASMA_OUTPUT_MODE_CONTOUR_ONLY
    ) {
        return 361;
    }
    plasma_selection_preferences_set_defaults(&selection_preferences);
    selection_preferences.content_filter = PLASMA_CONTENT_FILTER_STABLE_AND_EXPERIMENTAL;
    if (
        !plasma_compile_selection_plan(
            module,
            "lava_isolines",
            "plasma_lava",
            &selection_preferences,
            &plan
        ) ||
        strcmp(plan.preset_key, "lava_isolines") != 0 ||
        plan.output_family != PLASMA_OUTPUT_FAMILY_CONTOUR ||
        plan.output_mode != PLASMA_OUTPUT_MODE_CONTOUR_ONLY
    ) {
        return 362;
    }

    random_seed.base_seed = 0x13572468UL;
    random_seed.stream_seed = 0x24681357UL;
    random_seed.deterministic = 0;
    selection_preferences = product_config.selection;
    plasma_config_randomize_settings(
        module,
        &common_config,
        &product_config,
        sizeof(product_config),
        &random_seed,
        NULL
    );
    if (memcmp(&selection_preferences, &product_config.selection, sizeof(selection_preferences)) != 0) {
        return 140;
    }

    plasma_config_set_defaults(&common_config, &product_config, sizeof(product_config));
    if (
        product_config.transition.enabled != 0 ||
        product_config.transition.policy != PLASMA_TRANSITION_POLICY_DISABLED ||
        product_config.transition.fallback_policy != PLASMA_TRANSITION_FALLBACK_THEME_MORPH ||
        product_config.transition.seed_policy != PLASMA_TRANSITION_SEED_CONTINUITY_KEEP_STREAM ||
        product_config.transition.interval_millis != 12000UL ||
        product_config.transition.duration_millis != 2400UL ||
        product_config.transition.journey_key[0] != '\0'
    ) {
        return 141;
    }

    if (
        !plasma_config_import_settings_entry(
            module,
            &common_config,
            &product_config,
            sizeof(product_config),
            SCREENSAVE_SETTINGS_FILE_PRESET,
            "transition",
            "enabled",
            "true",
            NULL
        ) ||
        !plasma_config_import_settings_entry(
            module,
            &common_config,
            &product_config,
            sizeof(product_config),
            SCREENSAVE_SETTINGS_FILE_PRESET,
            "transition",
            "policy",
            "journey",
            NULL
        ) ||
        !plasma_config_import_settings_entry(
            module,
            &common_config,
            &product_config,
            sizeof(product_config),
            SCREENSAVE_SETTINGS_FILE_PRESET,
            "transition",
            "fallback_policy",
            "theme_morph",
            NULL
        ) ||
        !plasma_config_import_settings_entry(
            module,
            &common_config,
            &product_config,
            sizeof(product_config),
            SCREENSAVE_SETTINGS_FILE_PRESET,
            "transition",
            "seed_continuity_policy",
            "reseed_target",
            NULL
        ) ||
        !plasma_config_import_settings_entry(
            module,
            &common_config,
            &product_config,
            sizeof(product_config),
            SCREENSAVE_SETTINGS_FILE_PRESET,
            "transition",
            "interval_millis",
            "4500",
            NULL
        ) ||
        !plasma_config_import_settings_entry(
            module,
            &common_config,
            &product_config,
            sizeof(product_config),
            SCREENSAVE_SETTINGS_FILE_PRESET,
            "transition",
            "duration_millis",
            "1200",
            NULL
        ) ||
        !plasma_config_import_settings_entry(
            module,
            &common_config,
            &product_config,
            sizeof(product_config),
            SCREENSAVE_SETTINGS_FILE_PRESET,
            "transition",
            "journey_key",
            "classic_cycle",
            NULL
        )
    ) {
        return 142;
    }

    plasma_config_clamp(&common_config, &product_config, sizeof(product_config));
    if (
        !product_config.transition.enabled ||
        product_config.transition.policy != PLASMA_TRANSITION_POLICY_JOURNEY ||
        product_config.transition.fallback_policy != PLASMA_TRANSITION_FALLBACK_THEME_MORPH ||
        product_config.transition.seed_policy != PLASMA_TRANSITION_SEED_CONTINUITY_RESEED_TARGET ||
        product_config.transition.interval_millis != 4500UL ||
        product_config.transition.duration_millis != 1200UL ||
        strcmp(product_config.transition.journey_key, "classic_cycle") != 0
    ) {
        return 143;
    }

    ZeroMemory(&settings_capture, sizeof(settings_capture));
    ZeroMemory(&settings_writer, sizeof(settings_writer));
    settings_writer.context = &settings_capture;
    settings_writer.write_string = plasma_smoke_capture_write_string;
    if (
        !plasma_config_export_settings_entries(
            module,
            &common_config,
            &product_config,
            sizeof(product_config),
            SCREENSAVE_SETTINGS_FILE_PRESET,
            &settings_writer,
            NULL
        ) ||
        !plasma_smoke_capture_has_string(&settings_capture, "transition", "enabled", "true") ||
        !plasma_smoke_capture_has_string(&settings_capture, "transition", "policy", "journey") ||
        !plasma_smoke_capture_has_string(
            &settings_capture,
            "transition",
            "fallback_policy",
            "theme_morph"
        ) ||
        !plasma_smoke_capture_has_string(
            &settings_capture,
            "transition",
            "seed_continuity_policy",
            "reseed_target"
        ) ||
        !plasma_smoke_capture_has_string(
            &settings_capture,
            "transition",
            "interval_millis",
            "4500"
        ) ||
        !plasma_smoke_capture_has_string(
            &settings_capture,
            "transition",
            "duration_millis",
            "1200"
        ) ||
        !plasma_smoke_capture_has_string(
            &settings_capture,
            "transition",
            "journey_key",
            "classic_cycle"
        )
    ) {
        return 144;
    }

    plasma_selection_preferences_set_defaults(&selection_preferences);
    lstrcpyA(selection_preferences.theme_set_key, "warm_classics");
    plasma_transition_preferences_set_defaults(&transition_preferences);
    transition_preferences.enabled = 1;
    transition_preferences.policy = PLASMA_TRANSITION_POLICY_THEME_SET;
    transition_preferences.duration_millis = 600UL;
    transition_preferences.interval_millis = 800UL;
    if (
        !plasma_compile_transition_plan(
            module,
            PLASMA_DEFAULT_PRESET_KEY,
            PLASMA_DEFAULT_THEME_KEY,
            &selection_preferences,
            &transition_preferences,
            SCREENSAVE_RENDERER_KIND_UNKNOWN,
            SCREENSAVE_RENDERER_KIND_UNKNOWN,
            &plan
        ) ||
        !plan.transition_requested ||
        !plan.transition_enabled ||
        plan.transition_policy != PLASMA_TRANSITION_POLICY_THEME_SET ||
        plan.transition_supported_types !=
            (PLASMA_TRANSITION_SUPPORTED_THEME_MORPH |
                PLASMA_TRANSITION_SUPPORTED_PRESET_MORPH |
                PLASMA_TRANSITION_SUPPORTED_FALLBACK) ||
        plan.selection.active_theme_set == NULL ||
        strcmp(plan.selection.active_theme_set->set_key, "warm_classics") != 0 ||
        plan.selection.active_preset_set != NULL ||
        plan.journey != NULL ||
        !plasma_plan_validate_for_renderer_kind(&plan, module, SCREENSAVE_RENDERER_KIND_GDI) ||
        !plasma_plan_validate_for_renderer_kind(&plan, module, SCREENSAVE_RENDERER_KIND_GL11) ||
        plasma_plan_is_lower_band_baseline(&plan)
    ) {
        return 145;
    }

    plasma_selection_preferences_set_defaults(&selection_preferences);
    lstrcpyA(selection_preferences.preset_set_key, "plasma_classics");
    plasma_transition_preferences_set_defaults(&transition_preferences);
    transition_preferences.enabled = 1;
    transition_preferences.policy = PLASMA_TRANSITION_POLICY_PRESET_SET;
    transition_preferences.duration_millis = 600UL;
    transition_preferences.interval_millis = 800UL;
    if (
        !plasma_compile_transition_plan(
            module,
            "aurora_plasma",
            "aurora_cool",
            &selection_preferences,
            &transition_preferences,
            SCREENSAVE_RENDERER_KIND_UNKNOWN,
            SCREENSAVE_RENDERER_KIND_UNKNOWN,
            &plan
        ) ||
        !plan.transition_requested ||
        !plan.transition_enabled ||
        plan.transition_policy != PLASMA_TRANSITION_POLICY_PRESET_SET ||
        plan.selection.active_preset_set == NULL ||
        strcmp(plan.selection.active_preset_set->set_key, "plasma_classics") != 0 ||
        strcmp(plan.preset_key, "aurora_plasma") != 0 ||
        strcmp(plan.theme_key, "aurora_cool") != 0
    ) {
        return 146;
    }

    plasma_selection_preferences_set_defaults(&selection_preferences);
    plasma_transition_preferences_set_defaults(&transition_preferences);
    transition_preferences.enabled = 1;
    transition_preferences.policy = PLASMA_TRANSITION_POLICY_JOURNEY;
    transition_preferences.duration_millis = 600UL;
    transition_preferences.interval_millis = 900UL;
    lstrcpyA(transition_preferences.journey_key, "classic_cycle");
    if (
        !plasma_compile_transition_plan(
            module,
            "plasma_lava",
            "plasma_lava",
            &selection_preferences,
            &transition_preferences,
            SCREENSAVE_RENDERER_KIND_GL21,
            SCREENSAVE_RENDERER_KIND_GL21,
            &plan
        ) ||
        !plan.transition_requested ||
        !plan.transition_enabled ||
        plan.transition_policy != PLASMA_TRANSITION_POLICY_JOURNEY ||
        plan.journey == NULL ||
        strcmp(plan.journey->journey_key, "classic_cycle") != 0 ||
        (plan.transition_supported_types & PLASMA_TRANSITION_SUPPORTED_BRIDGE_MORPH) == 0UL ||
        !plasma_plan_validate_for_renderer_kind(&plan, module, SCREENSAVE_RENDERER_KIND_GL21)
    ) {
        return 147;
    }

    plasma_config_set_defaults(&common_config, &product_config, sizeof(product_config));
    screensave_config_binding_init(&binding, &common_config, &product_config, sizeof(product_config));
    ZeroMemory(&environment, sizeof(environment));
    environment.mode = SCREENSAVE_SESSION_MODE_WINDOWED;
    environment.drawable_size.width = 320;
    environment.drawable_size.height = 240;
    environment.seed.base_seed = 0x00000A55UL;
    environment.seed.stream_seed = 0x00000A77UL;
    environment.seed.deterministic = common_config.use_deterministic_seed;
    environment.config_binding = &binding;

    session = NULL;
    if (!plasma_create_session(module, &session, &environment) || session == NULL) {
        return 23;
    }
    if (
        session->plan.preset_key == NULL ||
        strcmp(session->plan.preset_key, PLASMA_DEFAULT_PRESET_KEY) != 0 ||
        session->plan.theme_key == NULL ||
        strcmp(session->plan.theme_key, PLASMA_DEFAULT_THEME_KEY) != 0 ||
        session->state.active_renderer_kind != SCREENSAVE_RENDERER_KIND_GDI ||
        !session->plan.modern_capable ||
        session->plan.modern_requested ||
        session->plan.modern_enabled ||
        session->plan.modern_degraded ||
        !session->plan.premium_capable ||
        session->plan.premium_requested ||
        session->plan.premium_enabled ||
        session->plan.premium_degraded ||
        session->plan.transition_requested ||
        session->plan.transition_enabled ||
        session->plan.transition_policy != PLASMA_TRANSITION_POLICY_DISABLED ||
        session->plan.transition_supported_types !=
            (PLASMA_TRANSITION_SUPPORTED_THEME_MORPH |
                PLASMA_TRANSITION_SUPPORTED_PRESET_MORPH |
                PLASMA_TRANSITION_SUPPORTED_FALLBACK) ||
        !plasma_plan_validate_for_renderer_kind(&session->plan, module, session->state.active_renderer_kind)
    ) {
        plasma_destroy_session(session);
        return 24;
    }

    environment.clock.delta_millis = 33UL;
    plasma_step_session(session, &environment);
    if (!plasma_output_build(&session->plan, &session->state, &output_frame)) {
        plasma_destroy_session(session);
        return 25;
    }
    if (
        !plasma_treatment_apply(
            &session->plan,
            &session->state,
            &output_frame,
            &session->state.visual_buffer,
            &treated_frame
        )
    ) {
        plasma_destroy_session(session);
        return 26;
    }
    if (
        !plasma_presentation_prepare(
            &session->plan,
            &session->state,
            &treated_frame,
            &presentation_target
        )
    ) {
        plasma_destroy_session(session);
        return 27;
    }
    if (
        session->state.visual_buffer.pixels == NULL ||
        session->state.field_primary == NULL ||
        !plasma_plan_validate(&session->plan, module) ||
        !plasma_plan_validate_lower_band_baseline(&session->plan, module)
    ) {
        plasma_destroy_session(session);
        return 28;
    }

    plan = session->plan;
    plan.output_family = PLASMA_OUTPUT_FAMILY_GLYPH;
    if (
        plasma_output_validate_plan(&plan) ||
        plasma_output_build(&plan, &session->state, &output_frame) ||
        plasma_plan_validate_for_renderer_kind(&plan, module, SCREENSAVE_RENDERER_KIND_GDI) ||
        plasma_plan_validate_for_renderer_kind(&plan, module, SCREENSAVE_RENDERER_KIND_GL11)
    ) {
        plasma_destroy_session(session);
        return 29;
    }

    plan = session->plan;
    plan.filter_treatment = PLASMA_FILTER_TREATMENT_BLUR;
    if (
        plasma_treatment_validate_plan(&plan) ||
        plasma_treatment_apply(
            &plan,
            &session->state,
            &output_frame,
            &session->state.visual_buffer,
            &treated_frame
        ) ||
        plasma_plan_validate_for_renderer_kind(&plan, module, SCREENSAVE_RENDERER_KIND_GDI) ||
        plasma_plan_validate_for_renderer_kind(&plan, module, SCREENSAVE_RENDERER_KIND_GL11)
    ) {
        plasma_destroy_session(session);
        return 30;
    }

    plan = session->plan;
    plan.presentation_mode = PLASMA_PRESENTATION_MODE_HEIGHTFIELD;
    if (
        plasma_presentation_validate_plan(&plan) ||
        plasma_presentation_prepare(
            &plan,
            &session->state,
            &treated_frame,
            &presentation_target
        ) ||
        plasma_plan_validate_for_renderer_kind(&plan, module, SCREENSAVE_RENDERER_KIND_GDI) ||
        plasma_plan_validate_for_renderer_kind(&plan, module, SCREENSAVE_RENDERER_KIND_GL11)
    ) {
        plasma_destroy_session(session);
        return 31;
    }

    plasma_destroy_session(session);

    plasma_config_set_defaults(&common_config, &product_config, sizeof(product_config));
    screensave_config_binding_init(&binding, &common_config, &product_config, sizeof(product_config));
    ZeroMemory(&environment, sizeof(environment));
    fake_size.width = 320;
    fake_size.height = 240;
    plasma_smoke_init_fake_renderer(
        &fake_renderer,
        SCREENSAVE_RENDERER_KIND_GDI,
        SCREENSAVE_RENDERER_KIND_GDI,
        &fake_size
    );
    environment.mode = SCREENSAVE_SESSION_MODE_WINDOWED;
    environment.drawable_size = fake_size;
    environment.seed.base_seed = 0x1718191AUL;
    environment.seed.stream_seed = 0x1B1C1D1EUL;
    environment.seed.deterministic = common_config.use_deterministic_seed;
    environment.config_binding = &binding;
    environment.renderer = &fake_renderer;
    session = NULL;
    if (!plasma_create_session(module, &session, &environment) || session == NULL) {
        return 382;
    }
    plasma_selection_preferences_set_defaults(&selection_preferences);
    selection_preferences.content_filter = PLASMA_CONTENT_FILTER_STABLE_AND_EXPERIMENTAL;

    if (
        !plasma_compile_plan_for_renderer(
            module,
            "lava_isolines",
            "plasma_lava",
            &selection_preferences,
            SCREENSAVE_RENDERER_KIND_GDI,
            SCREENSAVE_RENDERER_KIND_GDI,
            &plan
        ) ||
        strcmp(plan.preset_key, "lava_isolines") != 0 ||
        plan.output_family != PLASMA_OUTPUT_FAMILY_CONTOUR ||
        plan.output_mode != PLASMA_OUTPUT_MODE_CONTOUR_ONLY ||
        plan.filter_treatment != PLASMA_FILTER_TREATMENT_NONE ||
        plan.emulation_treatment != PLASMA_EMULATION_TREATMENT_NONE ||
        plan.accent_treatment != PLASMA_ACCENT_TREATMENT_NONE ||
        !plasma_output_validate_plan(&plan) ||
        !plasma_treatment_validate_plan(&plan) ||
        !plasma_output_build(&plan, &session->state, &output_frame) ||
        !plasma_treatment_apply(
            &plan,
            &session->state,
            &output_frame,
            &session->state.visual_buffer,
            &treated_frame
        ) ||
        !plasma_plan_validate_for_renderer_kind(&plan, module, SCREENSAVE_RENDERER_KIND_GDI) ||
        !plasma_plan_validate_for_renderer_kind(&plan, module, SCREENSAVE_RENDERER_KIND_GL11)
    ) {
        return 363;
    }
    if (
        !plasma_compile_plan_for_renderer(
            module,
            "aurora_bands",
            "aurora_cool",
            &selection_preferences,
            SCREENSAVE_RENDERER_KIND_GDI,
            SCREENSAVE_RENDERER_KIND_GDI,
            &plan
        ) ||
        strcmp(plan.preset_key, "aurora_bands") != 0 ||
        plan.output_family != PLASMA_OUTPUT_FAMILY_BANDED ||
        plan.output_mode != PLASMA_OUTPUT_MODE_POSTERIZED_BANDS ||
        plan.filter_treatment != PLASMA_FILTER_TREATMENT_GLOW_EDGE ||
        plan.emulation_treatment != PLASMA_EMULATION_TREATMENT_NONE ||
        plan.accent_treatment != PLASMA_ACCENT_TREATMENT_NONE ||
        !plasma_output_validate_plan(&plan) ||
        !plasma_treatment_validate_plan(&plan) ||
        !plasma_output_build(&plan, &session->state, &output_frame) ||
        !plasma_treatment_apply(
            &plan,
            &session->state,
            &output_frame,
            &session->state.visual_buffer,
            &treated_frame
        )
    ) {
        return 364;
    }
    if (
        !plasma_compile_plan_for_renderer(
            module,
            "wire_glow",
            "midnight_interference",
            &selection_preferences,
            SCREENSAVE_RENDERER_KIND_GDI,
            SCREENSAVE_RENDERER_KIND_GDI,
            &plan
        ) ||
        plan.output_family != PLASMA_OUTPUT_FAMILY_CONTOUR ||
        plan.output_mode != PLASMA_OUTPUT_MODE_CONTOUR_ONLY ||
        plan.filter_treatment != PLASMA_FILTER_TREATMENT_GLOW_EDGE ||
        !plasma_treatment_validate_plan(&plan) ||
        !plasma_output_validate_plan(&plan) ||
        !plasma_output_build(&plan, &session->state, &output_frame) ||
        !plasma_treatment_apply(
            &plan,
            &session->state,
            &output_frame,
            &session->state.visual_buffer,
            &treated_frame
        )
    ) {
        return 365;
    }
    if (
        !plasma_compile_plan_for_renderer(
            module,
            "phosphor_topography",
            "museum_phosphor",
            &selection_preferences,
            SCREENSAVE_RENDERER_KIND_GL46,
            SCREENSAVE_RENDERER_KIND_GL46,
            &plan
        ) ||
        strcmp(plan.preset_key, "phosphor_topography") != 0 ||
        plan.active_renderer_kind != SCREENSAVE_RENDERER_KIND_GL46 ||
        !plan.advanced_enabled ||
        !plan.modern_enabled ||
        plan.premium_enabled ||
        plan.output_family != PLASMA_OUTPUT_FAMILY_CONTOUR ||
        plan.output_mode != PLASMA_OUTPUT_MODE_CONTOUR_BANDS ||
        plan.emulation_treatment != PLASMA_EMULATION_TREATMENT_PHOSPHOR ||
        !plasma_output_validate_plan(&plan) ||
        !plasma_treatment_validate_plan(&plan) ||
        !plasma_output_build(&plan, &session->state, &output_frame) ||
        !plasma_treatment_apply(
            &plan,
            &session->state,
            &output_frame,
            &session->state.visual_buffer,
            &treated_frame
        ) ||
        !plasma_plan_validate_for_renderer_kind(&plan, module, SCREENSAVE_RENDERER_KIND_GL46)
    ) {
        return 366;
    }
    if (
        !plasma_compile_plan_for_renderer(
            module,
            "stipple_bands",
            "quiet_darkroom",
            &selection_preferences,
            SCREENSAVE_RENDERER_KIND_GDI,
            SCREENSAVE_RENDERER_KIND_GDI,
            &plan
        ) ||
        plan.output_family != PLASMA_OUTPUT_FAMILY_BANDED ||
        plan.output_mode != PLASMA_OUTPUT_MODE_POSTERIZED_BANDS ||
        plan.filter_treatment != PLASMA_FILTER_TREATMENT_HALFTONE_STIPPLE ||
        !plasma_treatment_validate_plan(&plan)
    ) {
        return 367;
    }
    if (
        !plasma_compile_plan_for_renderer(
            module,
            "emboss_current",
            "oceanic_blue",
            &selection_preferences,
            SCREENSAVE_RENDERER_KIND_GDI,
            SCREENSAVE_RENDERER_KIND_GDI,
            &plan
        ) ||
        plan.output_family != PLASMA_OUTPUT_FAMILY_CONTOUR ||
        plan.output_mode != PLASMA_OUTPUT_MODE_CONTOUR_BANDS ||
        plan.filter_treatment != PLASMA_FILTER_TREATMENT_EMBOSS_EDGE ||
        plan.accent_treatment != PLASMA_ACCENT_TREATMENT_ACCENT_PASS ||
        !plasma_treatment_validate_plan(&plan)
    ) {
        return 368;
    }
    if (
        !plasma_compile_plan_for_renderer(
            module,
            "crt_signal_bands",
            "museum_phosphor",
            &selection_preferences,
            SCREENSAVE_RENDERER_KIND_GDI,
            SCREENSAVE_RENDERER_KIND_GDI,
            &plan
        ) ||
        plan.output_family != PLASMA_OUTPUT_FAMILY_BANDED ||
        plan.output_mode != PLASMA_OUTPUT_MODE_POSTERIZED_BANDS ||
        plan.emulation_treatment != PLASMA_EMULATION_TREATMENT_CRT ||
        plan.accent_treatment != PLASMA_ACCENT_TREATMENT_ACCENT_PASS ||
        !plasma_treatment_validate_plan(&plan)
    ) {
        return 369;
    }
    if (
        !plasma_output_family_is_supported(PLASMA_OUTPUT_FAMILY_RASTER) ||
        !plasma_output_family_is_supported(PLASMA_OUTPUT_FAMILY_BANDED) ||
        !plasma_output_family_is_supported(PLASMA_OUTPUT_FAMILY_CONTOUR) ||
        !plasma_output_family_is_supported(PLASMA_OUTPUT_FAMILY_GLYPH) ||
        plasma_output_family_is_supported(PLASMA_OUTPUT_FAMILY_SURFACE) ||
        !plasma_output_mode_is_supported(PLASMA_OUTPUT_MODE_NATIVE_RASTER) ||
        !plasma_output_mode_is_supported(PLASMA_OUTPUT_MODE_POSTERIZED_BANDS) ||
        !plasma_output_mode_is_supported(PLASMA_OUTPUT_MODE_CONTOUR_ONLY) ||
        !plasma_output_mode_is_supported(PLASMA_OUTPUT_MODE_CONTOUR_BANDS) ||
        !plasma_output_mode_is_supported(PLASMA_OUTPUT_MODE_ASCII_GLYPH) ||
        !plasma_output_mode_is_supported(PLASMA_OUTPUT_MODE_MATRIX_GLYPH) ||
        plasma_output_mode_is_supported(PLASMA_OUTPUT_MODE_DITHERED_RASTER) ||
        plasma_output_mode_is_supported(PLASMA_OUTPUT_MODE_HEIGHTFIELD_SURFACE) ||
        plasma_output_mode_is_supported(PLASMA_OUTPUT_MODE_CURTAIN_SURFACE) ||
        plasma_output_mode_is_supported(PLASMA_OUTPUT_MODE_RIBBON_SURFACE) ||
        plasma_output_mode_is_supported(PLASMA_OUTPUT_MODE_EXTRUDED_CONTOUR_SURFACE) ||
        plasma_output_family_supports_mode(PLASMA_OUTPUT_FAMILY_SURFACE, PLASMA_OUTPUT_MODE_HEIGHTFIELD_SURFACE) ||
        strcmp(plasma_output_family_token(PLASMA_OUTPUT_FAMILY_SURFACE), "unsupported") != 0 ||
        strcmp(plasma_output_mode_token(PLASMA_OUTPUT_MODE_DITHERED_RASTER), "unsupported") != 0
    ) {
        return 463;
    }
    if (
        !plasma_sampling_treatment_is_supported(PLASMA_SAMPLING_TREATMENT_NONE) ||
        plasma_sampling_treatment_is_supported(PLASMA_SAMPLING_TREATMENT_NEAREST) ||
        !plasma_filter_treatment_is_supported(PLASMA_FILTER_TREATMENT_BLUR) ||
        !plasma_filter_treatment_is_supported(PLASMA_FILTER_TREATMENT_GLOW_EDGE) ||
        !plasma_filter_treatment_is_supported(PLASMA_FILTER_TREATMENT_HALFTONE_STIPPLE) ||
        !plasma_filter_treatment_is_supported(PLASMA_FILTER_TREATMENT_EMBOSS_EDGE) ||
        plasma_filter_treatment_is_supported(PLASMA_FILTER_TREATMENT_KALEIDOSCOPE_MIRROR) ||
        plasma_filter_treatment_is_supported(PLASMA_FILTER_TREATMENT_RESTRAINED_GLITCH) ||
        !plasma_filter_treatment_requires_advanced(PLASMA_FILTER_TREATMENT_BLUR) ||
        plasma_filter_treatment_requires_advanced(PLASMA_FILTER_TREATMENT_GLOW_EDGE) ||
        !plasma_emulation_treatment_is_supported(PLASMA_EMULATION_TREATMENT_PHOSPHOR) ||
        !plasma_emulation_treatment_is_supported(PLASMA_EMULATION_TREATMENT_CRT) ||
        !plasma_accent_treatment_is_supported(PLASMA_ACCENT_TREATMENT_OVERLAY_PASS) ||
        !plasma_accent_treatment_is_supported(PLASMA_ACCENT_TREATMENT_ACCENT_PASS) ||
        !plasma_accent_treatment_requires_advanced(PLASMA_ACCENT_TREATMENT_OVERLAY_PASS) ||
        plasma_accent_treatment_requires_advanced(PLASMA_ACCENT_TREATMENT_ACCENT_PASS) ||
        !plasma_presentation_mode_is_supported(PLASMA_PRESENTATION_MODE_RIBBON) ||
        !plasma_presentation_mode_supports_output_family(
            PLASMA_PRESENTATION_MODE_CONTOUR_EXTRUSION,
            PLASMA_OUTPUT_FAMILY_CONTOUR
        ) ||
        plasma_presentation_mode_supports_output_family(
            PLASMA_PRESENTATION_MODE_CONTOUR_EXTRUSION,
            PLASMA_OUTPUT_FAMILY_RASTER
        ) ||
        !plasma_filter_treatment_is_primary_visible(PLASMA_FILTER_TREATMENT_GLOW_EDGE) ||
        plasma_filter_treatment_is_primary_visible(PLASMA_FILTER_TREATMENT_BLUR) ||
        !plasma_accent_treatment_is_primary_visible(PLASMA_ACCENT_TREATMENT_ACCENT_PASS) ||
        plasma_accent_treatment_is_primary_visible(PLASMA_ACCENT_TREATMENT_OVERLAY_PASS) ||
        !plasma_presentation_mode_is_primary_visible(PLASMA_PRESENTATION_MODE_FLAT) ||
        !plasma_presentation_mode_is_primary_visible(PLASMA_PRESENTATION_MODE_HEIGHTFIELD) ||
        !plasma_presentation_mode_is_primary_visible(PLASMA_PRESENTATION_MODE_RIBBON) ||
        plasma_presentation_mode_is_primary_visible(PLASMA_PRESENTATION_MODE_CURTAIN) ||
        plasma_presentation_mode_is_primary_visible(PLASMA_PRESENTATION_MODE_CONTOUR_EXTRUSION) ||
        plasma_presentation_mode_is_primary_visible(PLASMA_PRESENTATION_MODE_BOUNDED_SURFACE) ||
        strcmp(plasma_filter_treatment_token(PLASMA_FILTER_TREATMENT_KALEIDOSCOPE_MIRROR), "unsupported") != 0 ||
        strcmp(plasma_sampling_treatment_token(PLASMA_SAMPLING_TREATMENT_DITHER), "unsupported") != 0 ||
        strcmp(plasma_presentation_mode_token(PLASMA_PRESENTATION_MODE_BOUNDED_BILLBOARD_VOLUME), "unsupported") != 0
    ) {
        return 466;
    }
    plasma_destroy_session(session);

    plasma_config_set_defaults(&common_config, &product_config, sizeof(product_config));
    plasma_apply_preset_to_config("ascii_reactor", &common_config, &product_config);
    common_config.preset_key = "ascii_reactor";
    common_config.theme_key = "amber_terminal";
    product_config.selection.content_filter = PLASMA_CONTENT_FILTER_STABLE_AND_EXPERIMENTAL;
    screensave_config_binding_init(&binding, &common_config, &product_config, sizeof(product_config));
    ZeroMemory(&environment, sizeof(environment));
    fake_size.width = 320;
    fake_size.height = 240;
    plasma_smoke_init_fake_renderer(
        &fake_renderer,
        SCREENSAVE_RENDERER_KIND_GDI,
        SCREENSAVE_RENDERER_KIND_GDI,
        &fake_size
    );
    environment.mode = SCREENSAVE_SESSION_MODE_WINDOWED;
    environment.drawable_size = fake_size;
    environment.seed.base_seed = 0x21222324UL;
    environment.seed.stream_seed = 0x25262728UL;
    environment.seed.deterministic = common_config.use_deterministic_seed;
    environment.config_binding = &binding;
    environment.renderer = &fake_renderer;
    session = NULL;
    if (!plasma_create_session(module, &session, &environment) || session == NULL) {
        return 383;
    }
    plasma_smoke_step_session_delta(session, &environment, 33UL);
    if (
        strcmp(session->plan.preset_key, "ascii_reactor") != 0 ||
        session->plan.effect_mode != PLASMA_EFFECT_CHEMICAL ||
        session->plan.output_family != PLASMA_OUTPUT_FAMILY_GLYPH ||
        session->plan.output_mode != PLASMA_OUTPUT_MODE_ASCII_GLYPH ||
        !plasma_output_validate_plan(&session->plan) ||
        !plasma_treatment_validate_plan(&session->plan) ||
        !plasma_output_build(&session->plan, &session->state, &output_frame) ||
        !plasma_treatment_apply(
            &session->plan,
            &session->state,
            &output_frame,
            &session->state.visual_buffer,
            &treated_frame
        ) ||
        plasma_smoke_count_non_black_pixels(&session->state.visual_buffer) == 0U ||
        !plasma_plan_validate_for_renderer_kind(&session->plan, module, SCREENSAVE_RENDERER_KIND_GDI) ||
        !plasma_plan_validate_for_renderer_kind(&session->plan, module, SCREENSAVE_RENDERER_KIND_GL11)
    ) {
        plasma_destroy_session(session);
        return 384;
    }
    plasma_destroy_session(session);

    plasma_config_set_defaults(&common_config, &product_config, sizeof(product_config));
    plasma_apply_preset_to_config("matrix_lattice", &common_config, &product_config);
    common_config.preset_key = "matrix_lattice";
    common_config.theme_key = "museum_phosphor";
    product_config.selection.content_filter = PLASMA_CONTENT_FILTER_STABLE_AND_EXPERIMENTAL;
    screensave_config_binding_init(&binding, &common_config, &product_config, sizeof(product_config));
    ZeroMemory(&environment, sizeof(environment));
    fake_size.width = 320;
    fake_size.height = 240;
    plasma_smoke_init_fake_renderer(
        &fake_renderer,
        SCREENSAVE_RENDERER_KIND_GL11,
        SCREENSAVE_RENDERER_KIND_GL11,
        &fake_size
    );
    environment.mode = SCREENSAVE_SESSION_MODE_WINDOWED;
    environment.drawable_size = fake_size;
    environment.seed.base_seed = 0x31323334UL;
    environment.seed.stream_seed = 0x35363738UL;
    environment.seed.deterministic = common_config.use_deterministic_seed;
    environment.config_binding = &binding;
    environment.renderer = &fake_renderer;
    session = NULL;
    if (!plasma_create_session(module, &session, &environment) || session == NULL) {
        return 385;
    }
    plasma_smoke_step_session_delta(session, &environment, 66UL);
    if (
        strcmp(session->plan.preset_key, "matrix_lattice") != 0 ||
        session->plan.effect_mode != PLASMA_EFFECT_LATTICE ||
        session->plan.output_family != PLASMA_OUTPUT_FAMILY_GLYPH ||
        session->plan.output_mode != PLASMA_OUTPUT_MODE_MATRIX_GLYPH ||
        !plasma_output_validate_plan(&session->plan) ||
        !plasma_treatment_validate_plan(&session->plan) ||
        !plasma_output_build(&session->plan, &session->state, &output_frame) ||
        !plasma_treatment_apply(
            &session->plan,
            &session->state,
            &output_frame,
            &session->state.visual_buffer,
            &treated_frame
        ) ||
        plasma_smoke_count_non_black_pixels(&session->state.visual_buffer) == 0U ||
        !plasma_plan_validate_for_renderer_kind(&session->plan, module, SCREENSAVE_RENDERER_KIND_GDI) ||
        !plasma_plan_validate_for_renderer_kind(&session->plan, module, SCREENSAVE_RENDERER_KIND_GL11)
    ) {
        plasma_destroy_session(session);
        return 386;
    }
    benchlab_config_state.common = common_config;
    benchlab_config_state.product_config = &product_config;
    benchlab_config_state.product_config_size = sizeof(product_config);
    if (
        !plasma_benchlab_build_snapshot(
            session,
            &benchlab_config_state,
            SCREENSAVE_RENDERER_KIND_GL11,
            &benchlab_snapshot
        ) ||
        strcmp(benchlab_snapshot.profile_class, "experimental") != 0 ||
        strcmp(benchlab_snapshot.generator_family, "lattice_quasi_crystal") != 0 ||
        strcmp(benchlab_snapshot.output_family, "glyph") != 0 ||
        strcmp(benchlab_snapshot.output_mode, "matrix_glyph") != 0
    ) {
        plasma_destroy_session(session);
        return 387;
    }
    if (
        !plasma_benchlab_build_report_section(
            session,
            &benchlab_config_state,
            SCREENSAVE_RENDERER_KIND_GL11,
            benchlab_report,
            (unsigned int)sizeof(benchlab_report)
        ) ||
        strstr(benchlab_report, "Generator family: lattice_quasi_crystal") == NULL ||
        strstr(benchlab_report, "Output family: glyph") == NULL ||
        strstr(benchlab_report, "Output mode: matrix_glyph") == NULL
    ) {
        plasma_destroy_session(session);
        return 388;
    }
    plasma_destroy_session(session);

    if (
        !plasma_compile_plan_for_renderer(
            module,
            "cellular_bloom",
            "aurora_cool",
            &selection_preferences,
            SCREENSAVE_RENDERER_KIND_GDI,
            SCREENSAVE_RENDERER_KIND_GDI,
            &plan
        ) ||
        plan.effect_mode != PLASMA_EFFECT_CHEMICAL ||
        plan.output_family != PLASMA_OUTPUT_FAMILY_RASTER ||
        plan.output_mode != PLASMA_OUTPUT_MODE_NATIVE_RASTER ||
        plan.filter_treatment != PLASMA_FILTER_TREATMENT_GLOW_EDGE
    ) {
        return 389;
    }
    if (
        !plasma_compile_plan_for_renderer(
            module,
            "quasi_crystal_bands",
            "midnight_interference",
            &selection_preferences,
            SCREENSAVE_RENDERER_KIND_GDI,
            SCREENSAVE_RENDERER_KIND_GDI,
            &plan
        ) ||
        plan.effect_mode != PLASMA_EFFECT_LATTICE ||
        plan.output_family != PLASMA_OUTPUT_FAMILY_BANDED ||
        plan.output_mode != PLASMA_OUTPUT_MODE_POSTERIZED_BANDS ||
        plan.filter_treatment != PLASMA_FILTER_TREATMENT_EMBOSS_EDGE ||
        plan.accent_treatment != PLASMA_ACCENT_TREATMENT_ACCENT_PASS
    ) {
        return 390;
    }
    plasma_config_set_defaults(&common_config, &product_config, sizeof(product_config));
    plasma_apply_preset_to_config("caustic_waterlight", &common_config, &product_config);
    common_config.preset_key = "caustic_waterlight";
    common_config.theme_key = "oceanic_blue";
    product_config.selection.content_filter = PLASMA_CONTENT_FILTER_STABLE_AND_EXPERIMENTAL;
    screensave_config_binding_init(&binding, &common_config, &product_config, sizeof(product_config));
    ZeroMemory(&environment, sizeof(environment));
    fake_size.width = 320;
    fake_size.height = 240;
    plasma_smoke_init_fake_renderer(
        &fake_renderer,
        SCREENSAVE_RENDERER_KIND_GDI,
        SCREENSAVE_RENDERER_KIND_GDI,
        &fake_size
    );
    environment.mode = SCREENSAVE_SESSION_MODE_WINDOWED;
    environment.drawable_size = fake_size;
    environment.seed.base_seed = 0x41424344UL;
    environment.seed.stream_seed = 0x45464748UL;
    environment.seed.deterministic = common_config.use_deterministic_seed;
    environment.config_binding = &binding;
    environment.renderer = &fake_renderer;
    session = NULL;
    if (!plasma_create_session(module, &session, &environment) || session == NULL) {
        return 391;
    }
    plasma_smoke_step_session_delta(session, &environment, 33UL);
    if (
        strcmp(session->plan.preset_key, "caustic_waterlight") != 0 ||
        session->plan.effect_mode != PLASMA_EFFECT_CAUSTIC ||
        session->plan.output_family != PLASMA_OUTPUT_FAMILY_CONTOUR ||
        session->plan.output_mode != PLASMA_OUTPUT_MODE_CONTOUR_BANDS ||
        session->plan.filter_treatment != PLASMA_FILTER_TREATMENT_GLOW_EDGE ||
        !plasma_output_validate_plan(&session->plan) ||
        !plasma_treatment_validate_plan(&session->plan) ||
        !plasma_output_build(&session->plan, &session->state, &output_frame) ||
        !plasma_treatment_apply(
            &session->plan,
            &session->state,
            &output_frame,
            &session->state.visual_buffer,
            &treated_frame
        ) ||
        plasma_smoke_count_non_black_pixels(&session->state.visual_buffer) == 0U
    ) {
        plasma_destroy_session(session);
        return 392;
    }
    plasma_destroy_session(session);

    plasma_selection_preferences_set_defaults(&selection_preferences);
    if (
        !plasma_compile_plan_for_renderer(
            module,
            "aurora_curtain",
            "aurora_cool",
            &selection_preferences,
            SCREENSAVE_RENDERER_KIND_GL46,
            SCREENSAVE_RENDERER_KIND_GL46,
            &plan
        ) ||
        strcmp(plan.preset_key, "aurora_curtain") != 0 ||
        plan.effect_mode != PLASMA_EFFECT_AURORA ||
        plan.output_family != PLASMA_OUTPUT_FAMILY_RASTER ||
        plan.output_mode != PLASMA_OUTPUT_MODE_NATIVE_RASTER ||
        plan.filter_treatment != PLASMA_FILTER_TREATMENT_GLOW_EDGE ||
        plan.presentation_mode != PLASMA_PRESENTATION_MODE_CURTAIN ||
        !plan.premium_requested ||
        !plan.premium_enabled ||
        plan.premium_degraded ||
        !plasma_output_validate_plan(&plan) ||
        !plasma_treatment_validate_plan(&plan) ||
        !plasma_presentation_validate_plan(&plan) ||
        !plasma_plan_validate_for_renderer_kind(&plan, module, SCREENSAVE_RENDERER_KIND_GL46)
    ) {
        return 410;
    }
    if (
        !plasma_compile_plan_for_renderer(
            module,
            "aurora_curtain",
            "aurora_cool",
            &selection_preferences,
            SCREENSAVE_RENDERER_KIND_GL46,
            SCREENSAVE_RENDERER_KIND_GL11,
            &plan
        ) ||
        plan.effect_mode != PLASMA_EFFECT_AURORA ||
        plan.presentation_mode != PLASMA_PRESENTATION_MODE_FLAT ||
        !plan.premium_requested ||
        plan.premium_enabled ||
        !plan.premium_degraded ||
        !plasma_plan_validate_for_renderer_kind(&plan, module, SCREENSAVE_RENDERER_KIND_GL11)
    ) {
        return 411;
    }

    plasma_config_set_defaults(&common_config, &product_config, sizeof(product_config));
    plasma_apply_preset_to_config("ribbon_aurora", &common_config, &product_config);
    common_config.preset_key = "ribbon_aurora";
    common_config.theme_key = "aurora_cool";
    product_config.selection.content_filter = PLASMA_CONTENT_FILTER_STABLE_AND_EXPERIMENTAL;
    screensave_config_binding_init(&binding, &common_config, &product_config, sizeof(product_config));
    ZeroMemory(&environment, sizeof(environment));
    fake_size.width = 320;
    fake_size.height = 240;
    plasma_smoke_init_fake_renderer(
        &fake_renderer,
        SCREENSAVE_RENDERER_KIND_GL46,
        SCREENSAVE_RENDERER_KIND_GL46,
        &fake_size
    );
    environment.mode = SCREENSAVE_SESSION_MODE_WINDOWED;
    environment.drawable_size = fake_size;
    environment.seed.base_seed = 0x51525354UL;
    environment.seed.stream_seed = 0x55565758UL;
    environment.seed.deterministic = common_config.use_deterministic_seed;
    environment.config_binding = &binding;
    environment.renderer = &fake_renderer;
    session = NULL;
    if (!plasma_create_session(module, &session, &environment) || session == NULL) {
        return 412;
    }
    plasma_smoke_step_session_delta(session, &environment, 33UL);
    if (
        strcmp(session->plan.preset_key, "ribbon_aurora") != 0 ||
        session->plan.effect_mode != PLASMA_EFFECT_AURORA ||
        session->plan.output_family != PLASMA_OUTPUT_FAMILY_BANDED ||
        session->plan.output_mode != PLASMA_OUTPUT_MODE_POSTERIZED_BANDS ||
        session->plan.accent_treatment != PLASMA_ACCENT_TREATMENT_ACCENT_PASS ||
        session->plan.presentation_mode != PLASMA_PRESENTATION_MODE_RIBBON ||
        !session->plan.premium_enabled ||
        !plasma_output_validate_plan(&session->plan) ||
        !plasma_treatment_validate_plan(&session->plan) ||
        !plasma_presentation_validate_plan(&session->plan) ||
        !plasma_output_build(&session->plan, &session->state, &output_frame) ||
        !plasma_treatment_apply(
            &session->plan,
            &session->state,
            &output_frame,
            &session->state.visual_buffer,
            &treated_frame
        ) ||
        !plasma_presentation_prepare(
            &session->plan,
            &session->state,
            &treated_frame,
            &presentation_target
        )
    ) {
        plasma_destroy_session(session);
        return 413;
    }
    benchlab_config_state.common = common_config;
    benchlab_config_state.product_config = &product_config;
    benchlab_config_state.product_config_size = sizeof(product_config);
    if (
        !plasma_benchlab_build_snapshot(
            session,
            &benchlab_config_state,
            SCREENSAVE_RENDERER_KIND_GL46,
            &benchlab_snapshot
        ) ||
        strcmp(benchlab_snapshot.profile_class, "experimental") != 0 ||
        strcmp(benchlab_snapshot.generator_family, "aurora_curtain_ribbon") != 0 ||
        strcmp(benchlab_snapshot.output_family, "banded") != 0 ||
        strcmp(benchlab_snapshot.output_mode, "posterized_bands") != 0 ||
        strcmp(benchlab_snapshot.presentation_mode, "ribbon") != 0 ||
        strcmp(benchlab_snapshot.requested_lane, "premium") != 0 ||
        strcmp(benchlab_snapshot.resolved_lane, "premium") != 0
    ) {
        plasma_destroy_session(session);
        return 414;
    }
    plasma_destroy_session(session);

    if (
        !plasma_compile_plan_for_renderer(
            module,
            "substrate_relief",
            "quiet_darkroom",
            &selection_preferences,
            SCREENSAVE_RENDERER_KIND_GL46,
            SCREENSAVE_RENDERER_KIND_GL46,
            &plan
        ) ||
        plan.effect_mode != PLASMA_EFFECT_SUBSTRATE ||
        plan.output_family != PLASMA_OUTPUT_FAMILY_RASTER ||
        plan.output_mode != PLASMA_OUTPUT_MODE_NATIVE_RASTER ||
        plan.filter_treatment != PLASMA_FILTER_TREATMENT_EMBOSS_EDGE ||
        plan.emulation_treatment != PLASMA_EMULATION_TREATMENT_PHOSPHOR ||
        plan.presentation_mode != PLASMA_PRESENTATION_MODE_BOUNDED_SURFACE ||
        !plan.premium_enabled ||
        !plasma_output_validate_plan(&plan) ||
        !plasma_treatment_validate_plan(&plan) ||
        !plasma_presentation_validate_plan(&plan) ||
        !plasma_plan_validate_for_renderer_kind(&plan, module, SCREENSAVE_RENDERER_KIND_GL46)
    ) {
        return 415;
    }
    plasma_config_set_defaults(&common_config, &product_config, sizeof(product_config));
    plasma_apply_preset_to_config("substrate_relief", &common_config, &product_config);
    common_config.preset_key = "substrate_relief";
    common_config.theme_key = "quiet_darkroom";
    product_config.selection.content_filter = PLASMA_CONTENT_FILTER_STABLE_AND_EXPERIMENTAL;
    product_config.presentation_mode = PLASMA_PRESENTATION_MODE_CONTOUR_EXTRUSION;
    screensave_config_binding_init(&binding, &common_config, &product_config, sizeof(product_config));
    ZeroMemory(&environment, sizeof(environment));
    fake_size.width = 320;
    fake_size.height = 240;
    plasma_smoke_init_fake_renderer(
        &fake_renderer,
        SCREENSAVE_RENDERER_KIND_GL46,
        SCREENSAVE_RENDERER_KIND_GL46,
        &fake_size
    );
    environment.mode = SCREENSAVE_SESSION_MODE_WINDOWED;
    environment.drawable_size = fake_size;
    environment.seed.base_seed = 0x71727374UL;
    environment.seed.stream_seed = 0x75767778UL;
    environment.seed.deterministic = common_config.use_deterministic_seed;
    environment.config_binding = &binding;
    environment.renderer = &fake_renderer;
    session = NULL;
    if (!plasma_create_session(module, &session, &environment) || session == NULL) {
        return 467;
    }
    benchlab_config_state.common = common_config;
    benchlab_config_state.product_config = &product_config;
    benchlab_config_state.product_config_size = sizeof(product_config);
    if (
        !session->plan.premium_enabled ||
        session->plan.requested_presentation_mode != PLASMA_PRESENTATION_MODE_CONTOUR_EXTRUSION ||
        session->plan.presentation_mode != PLASMA_PRESENTATION_MODE_FLAT ||
        !plasma_presentation_validate_plan(&session->plan) ||
        !plasma_benchlab_build_report_section(
            session,
            &benchlab_config_state,
            SCREENSAVE_RENDERER_KIND_GL46,
            benchlab_report,
            (unsigned int)sizeof(benchlab_report)
        ) ||
        strstr(benchlab_report, "Requested presentation mode: contour_extrusion") == NULL ||
        strstr(benchlab_report, "Presentation mode: flat") == NULL
    ) {
        plasma_destroy_session(session);
        return 468;
    }
    plasma_destroy_session(session);

    plasma_config_set_defaults(&common_config, &product_config, sizeof(product_config));
    plasma_apply_preset_to_config("filament_extrusion", &common_config, &product_config);
    common_config.preset_key = "filament_extrusion";
    common_config.theme_key = "midnight_interference";
    product_config.selection.content_filter = PLASMA_CONTENT_FILTER_STABLE_AND_EXPERIMENTAL;
    screensave_config_binding_init(&binding, &common_config, &product_config, sizeof(product_config));
    ZeroMemory(&environment, sizeof(environment));
    fake_size.width = 320;
    fake_size.height = 240;
    plasma_smoke_init_fake_renderer(
        &fake_renderer,
        SCREENSAVE_RENDERER_KIND_GL46,
        SCREENSAVE_RENDERER_KIND_GL46,
        &fake_size
    );
    environment.mode = SCREENSAVE_SESSION_MODE_WINDOWED;
    environment.drawable_size = fake_size;
    environment.seed.base_seed = 0x61626364UL;
    environment.seed.stream_seed = 0x65666768UL;
    environment.seed.deterministic = common_config.use_deterministic_seed;
    environment.config_binding = &binding;
    environment.renderer = &fake_renderer;
    session = NULL;
    if (!plasma_create_session(module, &session, &environment) || session == NULL) {
        return 416;
    }
    plasma_smoke_step_session_delta(session, &environment, 33UL);
    if (
        strcmp(session->plan.preset_key, "filament_extrusion") != 0 ||
        session->plan.effect_mode != PLASMA_EFFECT_ARC ||
        session->plan.output_family != PLASMA_OUTPUT_FAMILY_CONTOUR ||
        session->plan.output_mode != PLASMA_OUTPUT_MODE_CONTOUR_BANDS ||
        session->plan.filter_treatment != PLASMA_FILTER_TREATMENT_GLOW_EDGE ||
        session->plan.accent_treatment != PLASMA_ACCENT_TREATMENT_ACCENT_PASS ||
        session->plan.presentation_mode != PLASMA_PRESENTATION_MODE_CONTOUR_EXTRUSION ||
        !session->plan.premium_enabled ||
        !plasma_output_validate_plan(&session->plan) ||
        !plasma_treatment_validate_plan(&session->plan) ||
        !plasma_presentation_validate_plan(&session->plan) ||
        !plasma_output_build(&session->plan, &session->state, &output_frame) ||
        !plasma_treatment_apply(
            &session->plan,
            &session->state,
            &output_frame,
            &session->state.visual_buffer,
            &treated_frame
        ) ||
        !plasma_presentation_prepare(
            &session->plan,
            &session->state,
            &treated_frame,
            &presentation_target
        )
    ) {
        plasma_destroy_session(session);
        return 417;
    }
    benchlab_config_state.common = common_config;
    benchlab_config_state.product_config = &product_config;
    benchlab_config_state.product_config_size = sizeof(product_config);
    if (
        !plasma_benchlab_build_report_section(
            session,
            &benchlab_config_state,
            SCREENSAVE_RENDERER_KIND_GL46,
            benchlab_report,
            (unsigned int)sizeof(benchlab_report)
        ) ||
        strstr(benchlab_report, "Generator family: arc_discharge") == NULL ||
        strstr(benchlab_report, "Output family: contour") == NULL ||
        strstr(benchlab_report, "Presentation mode: contour_extrusion") == NULL
    ) {
        plasma_destroy_session(session);
        return 418;
    }
    plasma_destroy_session(session);

    plasma_config_set_defaults(&common_config, &product_config, sizeof(product_config));
    lstrcpyA(product_config.selection.theme_set_key, "warm_classics");
    plasma_transition_preferences_set_defaults(&product_config.transition);
    product_config.transition.enabled = 1;
    product_config.transition.policy = PLASMA_TRANSITION_POLICY_THEME_SET;
    product_config.transition.duration_millis = 600UL;
    product_config.transition.interval_millis = 800UL;
    screensave_config_binding_init(&binding, &common_config, &product_config, sizeof(product_config));
    ZeroMemory(&environment, sizeof(environment));
    environment.mode = SCREENSAVE_SESSION_MODE_WINDOWED;
    environment.drawable_size.width = 320;
    environment.drawable_size.height = 240;
    environment.seed.base_seed = 0x31323334UL;
    environment.seed.stream_seed = 0x35363738UL;
    environment.seed.deterministic = common_config.use_deterministic_seed;
    environment.config_binding = &binding;

    session = NULL;
    if (!plasma_create_session(module, &session, &environment) || session == NULL) {
        return 148;
    }
    plasma_smoke_step_session_delta(session, &environment, 800UL);
    if (
        !session->state.transition.active ||
        session->state.transition.active_type != PLASMA_TRANSITION_TYPE_THEME_MORPH ||
        session->state.transition.source_theme == NULL ||
        session->state.transition.target_theme == NULL ||
        strcmp(session->state.transition.source_theme->theme_key, "plasma_lava") != 0 ||
        strcmp(session->state.transition.target_theme->theme_key, "quiet_darkroom") != 0
    ) {
        plasma_destroy_session(session);
        return 149;
    }
    source_primary = session->state.transition.source_theme->descriptor->primary_color;
    source_accent = session->state.transition.source_theme->descriptor->accent_color;
    target_primary = session->state.transition.target_theme->descriptor->primary_color;
    target_accent = session->state.transition.target_theme->descriptor->accent_color;
    plasma_smoke_step_session_delta(session, &environment, 300UL);
    transition_progress = plasma_transition_progress_amount(&session->state);
    plasma_transition_resolve_theme_colors(
        &session->plan,
        &session->state,
        &blended_primary,
        &blended_accent
    );
    if (
        transition_progress == 0U ||
        transition_progress >= 255U ||
        (blended_primary.red == source_primary.red &&
            blended_primary.green == source_primary.green &&
            blended_primary.blue == source_primary.blue) ||
        (blended_primary.red == target_primary.red &&
            blended_primary.green == target_primary.green &&
            blended_primary.blue == target_primary.blue) ||
        (blended_accent.red == source_accent.red &&
            blended_accent.green == source_accent.green &&
            blended_accent.blue == source_accent.blue) ||
        (blended_accent.red == target_accent.red &&
            blended_accent.green == target_accent.green &&
            blended_accent.blue == target_accent.blue)
    ) {
        plasma_destroy_session(session);
        return 150;
    }
    plasma_smoke_step_session_delta(session, &environment, 400UL);
    if (
        session->state.transition.active ||
        strcmp(session->plan.theme_key, "quiet_darkroom") != 0
    ) {
        plasma_destroy_session(session);
        return 151;
    }
    plasma_destroy_session(session);

    plasma_config_set_defaults(&common_config, &product_config, sizeof(product_config));
    plasma_apply_preset_to_config("aurora_plasma", &common_config, &product_config);
    common_config.preset_key = "aurora_plasma";
    common_config.theme_key = "aurora_cool";
    lstrcpyA(product_config.selection.preset_set_key, "plasma_classics");
    plasma_transition_preferences_set_defaults(&product_config.transition);
    product_config.transition.enabled = 1;
    product_config.transition.policy = PLASMA_TRANSITION_POLICY_PRESET_SET;
    product_config.transition.duration_millis = 600UL;
    product_config.transition.interval_millis = 800UL;
    screensave_config_binding_init(&binding, &common_config, &product_config, sizeof(product_config));
    ZeroMemory(&environment, sizeof(environment));
    environment.mode = SCREENSAVE_SESSION_MODE_WINDOWED;
    environment.drawable_size.width = 320;
    environment.drawable_size.height = 240;
    environment.seed.base_seed = 0x41424344UL;
    environment.seed.stream_seed = 0x45464748UL;
    environment.seed.deterministic = common_config.use_deterministic_seed;
    environment.config_binding = &binding;

    session = NULL;
    if (!plasma_create_session(module, &session, &environment) || session == NULL) {
        return 152;
    }
    plasma_smoke_step_session_delta(session, &environment, 800UL);
    if (
        !session->state.transition.active ||
        session->state.transition.active_type != PLASMA_TRANSITION_TYPE_PRESET_MORPH ||
        session->state.transition.source_preset == NULL ||
        session->state.transition.target_preset == NULL ||
        strcmp(session->state.transition.source_preset->preset_key, "aurora_plasma") != 0 ||
        strcmp(session->state.transition.target_preset->preset_key, "museum_phosphor") != 0
    ) {
        plasma_destroy_session(session);
        return 153;
    }
    plasma_smoke_step_session_delta(session, &environment, 300UL);
    transition_progress = plasma_transition_progress_amount(&session->state);
    mid_speed_units = plasma_transition_effective_speed_units(&session->plan, &session->state, 0UL);
    smoothing_enabled = 0;
    smoothing_blend = 0U;
    if (
        transition_progress == 0U ||
        transition_progress >= 255U ||
        mid_speed_units <= 2UL ||
        mid_speed_units >= 4UL ||
        !plasma_transition_resolve_smoothing(
            &session->plan,
            &session->state,
            &smoothing_enabled,
            &smoothing_blend
        ) ||
        !smoothing_enabled ||
        smoothing_blend == 0U ||
        smoothing_blend >= 128U
    ) {
        plasma_destroy_session(session);
        return 154;
    }
    plasma_smoke_step_session_delta(session, &environment, 400UL);
    if (
        session->state.transition.active ||
        strcmp(session->plan.preset_key, "museum_phosphor") != 0 ||
        strcmp(session->plan.theme_key, "museum_phosphor") != 0
    ) {
        plasma_destroy_session(session);
        return 155;
    }
    plasma_destroy_session(session);

    plasma_config_set_defaults(&common_config, &product_config, sizeof(product_config));
    plasma_apply_preset_to_config("aurora_plasma", &common_config, &product_config);
    common_config.preset_key = "aurora_plasma";
    common_config.theme_key = "aurora_cool";
    product_config.effect_mode = PLASMA_EFFECT_CHEMICAL;
    product_config.speed_mode = PLASMA_SPEED_LIVELY;
    product_config.output_family = PLASMA_OUTPUT_FAMILY_CONTOUR;
    product_config.output_mode = PLASMA_OUTPUT_MODE_CONTOUR_ONLY;
    product_config.filter_treatment = PLASMA_FILTER_TREATMENT_EMBOSS_EDGE;
    lstrcpyA(product_config.selection.preset_set_key, "plasma_classics");
    plasma_transition_preferences_set_defaults(&product_config.transition);
    product_config.transition.enabled = 1;
    product_config.transition.policy = PLASMA_TRANSITION_POLICY_PRESET_SET;
    product_config.transition.duration_millis = 600UL;
    product_config.transition.interval_millis = 800UL;
    screensave_config_binding_init(&binding, &common_config, &product_config, sizeof(product_config));
    ZeroMemory(&environment, sizeof(environment));
    environment.mode = SCREENSAVE_SESSION_MODE_WINDOWED;
    environment.drawable_size.width = 320;
    environment.drawable_size.height = 240;
    environment.seed.base_seed = 0x71727374UL;
    environment.seed.stream_seed = 0x75767778UL;
    environment.seed.deterministic = common_config.use_deterministic_seed;
    environment.config_binding = &binding;

    session = NULL;
    if (!plasma_create_session(module, &session, &environment) || session == NULL) {
        return 433;
    }
    plasma_smoke_step_session_delta(session, &environment, 800UL);
    plasma_smoke_step_session_delta(session, &environment, 700UL);
    if (
        session->state.transition.active ||
        strcmp(session->plan.preset_key, "museum_phosphor") != 0 ||
        strcmp(session->plan.theme_key, "museum_phosphor") != 0 ||
        session->plan.requested_effect_mode != PLASMA_EFFECT_CHEMICAL ||
        session->plan.requested_speed_mode != PLASMA_SPEED_LIVELY ||
        session->plan.requested_output_family != PLASMA_OUTPUT_FAMILY_CONTOUR ||
        session->plan.requested_output_mode != PLASMA_OUTPUT_MODE_CONTOUR_ONLY ||
        session->plan.requested_filter_treatment != PLASMA_FILTER_TREATMENT_EMBOSS_EDGE ||
        session->plan.effect_mode != PLASMA_EFFECT_CHEMICAL ||
        session->plan.speed_mode != PLASMA_SPEED_LIVELY ||
        session->plan.output_family != PLASMA_OUTPUT_FAMILY_CONTOUR ||
        session->plan.output_mode != PLASMA_OUTPUT_MODE_CONTOUR_ONLY ||
        session->plan.filter_treatment != PLASMA_FILTER_TREATMENT_EMBOSS_EDGE ||
        !plasma_plan_validate_for_renderer_kind(
            &session->plan,
            module,
            session->state.active_renderer_kind
        )
    ) {
        plasma_destroy_session(session);
        return 434;
    }
    plasma_destroy_session(session);

    plasma_config_set_defaults(&common_config, &product_config, sizeof(product_config));
    plasma_apply_preset_to_config("quiet_darkroom", &common_config, &product_config);
    common_config.preset_key = "quiet_darkroom";
    common_config.theme_key = "quiet_darkroom";
    lstrcpyA(product_config.selection.preset_set_key, "dark_room_classics");
    lstrcpyA(product_config.selection.theme_set_key, "warm_classics");
    plasma_transition_preferences_set_defaults(&product_config.transition);
    product_config.transition.enabled = 1;
    product_config.transition.policy = PLASMA_TRANSITION_POLICY_PRESET_SET;
    product_config.transition.fallback_policy = PLASMA_TRANSITION_FALLBACK_THEME_MORPH;
    product_config.transition.duration_millis = 600UL;
    product_config.transition.interval_millis = 800UL;
    screensave_config_binding_init(&binding, &common_config, &product_config, sizeof(product_config));
    ZeroMemory(&environment, sizeof(environment));
    environment.mode = SCREENSAVE_SESSION_MODE_WINDOWED;
    environment.drawable_size.width = 320;
    environment.drawable_size.height = 240;
    environment.seed.base_seed = 0x51525354UL;
    environment.seed.stream_seed = 0x55565758UL;
    environment.seed.deterministic = common_config.use_deterministic_seed;
    environment.config_binding = &binding;

    session = NULL;
    if (!plasma_create_session(module, &session, &environment) || session == NULL) {
        return 156;
    }
    plasma_smoke_step_session_delta(session, &environment, 800UL);
    if (
        !session->state.transition.active ||
        session->state.transition.active_type != PLASMA_TRANSITION_TYPE_FALLBACK ||
        session->state.transition.target_preset == NULL ||
        session->state.transition.target_theme == NULL ||
        strcmp(session->state.transition.target_preset->preset_key, "museum_phosphor") != 0 ||
        strcmp(session->state.transition.target_theme->theme_key, "plasma_lava") != 0
    ) {
        plasma_destroy_session(session);
        return 157;
    }
    plasma_smoke_step_session_delta(session, &environment, 300UL);
    transition_progress = plasma_transition_progress_amount(&session->state);
    if (transition_progress == 0U || transition_progress >= 255U) {
        plasma_destroy_session(session);
        return 158;
    }
    plasma_smoke_step_session_delta(session, &environment, 400UL);
    if (
        session->state.transition.active ||
        strcmp(session->plan.preset_key, "museum_phosphor") != 0 ||
        strcmp(session->plan.theme_key, "plasma_lava") != 0
    ) {
        plasma_destroy_session(session);
        return 159;
    }
    plasma_destroy_session(session);

    plasma_config_set_defaults(&common_config, &product_config, sizeof(product_config));
    plasma_apply_preset_to_config("quiet_darkroom", &common_config, &product_config);
    common_config.preset_key = "quiet_darkroom";
    common_config.theme_key = "quiet_darkroom";
    lstrcpyA(product_config.selection.preset_set_key, "dark_room_classics");
    lstrcpyA(product_config.selection.theme_set_key, "warm_classics");
    plasma_transition_preferences_set_defaults(&product_config.transition);
    product_config.transition.enabled = 1;
    product_config.transition.policy = PLASMA_TRANSITION_POLICY_PRESET_SET;
    product_config.transition.fallback_policy = PLASMA_TRANSITION_FALLBACK_REJECT;
    product_config.transition.duration_millis = 600UL;
    product_config.transition.interval_millis = 800UL;
    screensave_config_binding_init(&binding, &common_config, &product_config, sizeof(product_config));
    ZeroMemory(&environment, sizeof(environment));
    environment.mode = SCREENSAVE_SESSION_MODE_WINDOWED;
    environment.drawable_size.width = 320;
    environment.drawable_size.height = 240;
    environment.seed.base_seed = 0x61626364UL;
    environment.seed.stream_seed = 0x65666768UL;
    environment.seed.deterministic = common_config.use_deterministic_seed;
    environment.config_binding = &binding;

    session = NULL;
    if (!plasma_create_session(module, &session, &environment) || session == NULL) {
        return 160;
    }
    plasma_smoke_step_session_delta(session, &environment, 800UL);
    if (
        session->state.transition.active ||
        session->state.transition.active_type != PLASMA_TRANSITION_TYPE_REJECTED ||
        strcmp(session->plan.preset_key, "quiet_darkroom") != 0 ||
        strcmp(session->plan.theme_key, "quiet_darkroom") != 0
    ) {
        plasma_destroy_session(session);
        return 161;
    }
    plasma_destroy_session(session);

    plasma_config_set_defaults(&common_config, &product_config, sizeof(product_config));
    plasma_apply_preset_to_config("quiet_darkroom", &common_config, &product_config);
    common_config.preset_key = "quiet_darkroom";
    common_config.theme_key = "quiet_darkroom";
    lstrcpyA(product_config.selection.preset_set_key, "dark_room_classics");
    lstrcpyA(product_config.selection.theme_set_key, "warm_classics");
    plasma_transition_preferences_set_defaults(&product_config.transition);
    product_config.transition.enabled = 1;
    product_config.transition.policy = PLASMA_TRANSITION_POLICY_PRESET_SET;
    product_config.transition.fallback_policy = PLASMA_TRANSITION_FALLBACK_HARD_CUT;
    product_config.transition.duration_millis = 600UL;
    product_config.transition.interval_millis = 800UL;
    screensave_config_binding_init(&binding, &common_config, &product_config, sizeof(product_config));
    ZeroMemory(&environment, sizeof(environment));
    environment.mode = SCREENSAVE_SESSION_MODE_WINDOWED;
    environment.drawable_size.width = 320;
    environment.drawable_size.height = 240;
    environment.seed.base_seed = 0x696A6B6CUL;
    environment.seed.stream_seed = 0x6D6E6F70UL;
    environment.seed.deterministic = common_config.use_deterministic_seed;
    environment.config_binding = &binding;

    session = NULL;
    if (!plasma_create_session(module, &session, &environment) || session == NULL) {
        return 241;
    }
    plasma_smoke_step_session_delta(session, &environment, 800UL);
    if (
        session->state.transition.active ||
        session->state.transition.active_type != PLASMA_TRANSITION_TYPE_HARD_CUT ||
        session->state.transition.requested_type != PLASMA_TRANSITION_TYPE_PRESET_MORPH ||
        strcmp(session->plan.preset_key, "museum_phosphor") != 0 ||
        strcmp(session->plan.theme_key, "plasma_lava") != 0
    ) {
        plasma_destroy_session(session);
        return 242;
    }
    plasma_destroy_session(session);

    plasma_config_set_defaults(&common_config, &product_config, sizeof(product_config));
    plasma_apply_preset_to_config("quiet_darkroom", &common_config, &product_config);
    common_config.preset_key = "quiet_darkroom";
    common_config.theme_key = "quiet_darkroom";
    lstrcpyA(product_config.selection.preset_set_key, "dark_room_classics");
    plasma_transition_preferences_set_defaults(&product_config.transition);
    product_config.transition.enabled = 1;
    product_config.transition.policy = PLASMA_TRANSITION_POLICY_PRESET_SET;
    product_config.transition.duration_millis = 600UL;
    product_config.transition.interval_millis = 800UL;
    screensave_config_binding_init(&binding, &common_config, &product_config, sizeof(product_config));
    ZeroMemory(&environment, sizeof(environment));
    fake_size.width = 320;
    fake_size.height = 240;
    plasma_smoke_init_fake_renderer(
        &fake_renderer,
        SCREENSAVE_RENDERER_KIND_GL21,
        SCREENSAVE_RENDERER_KIND_GL21,
        &fake_size
    );
    environment.mode = SCREENSAVE_SESSION_MODE_WINDOWED;
    environment.drawable_size = fake_size;
    environment.seed.base_seed = 0x71727374UL;
    environment.seed.stream_seed = 0x75767778UL;
    environment.seed.deterministic = common_config.use_deterministic_seed;
    environment.config_binding = &binding;
    environment.renderer = &fake_renderer;

    session = NULL;
    if (!plasma_create_session(module, &session, &environment) || session == NULL) {
        return 162;
    }
    plasma_smoke_step_session_delta(session, &environment, 800UL);
    if (
        !session->state.transition.active ||
        session->state.transition.active_type != PLASMA_TRANSITION_TYPE_BRIDGE_MORPH ||
        session->state.transition.target_preset == NULL ||
        strcmp(session->state.transition.target_preset->preset_key, "museum_phosphor") != 0
    ) {
        plasma_destroy_session(session);
        return 163;
    }
    plasma_smoke_step_session_delta(session, &environment, 300UL);
    if (
        !session->state.transition.bridge_switched ||
        strcmp(session->plan.preset_key, "museum_phosphor") != 0
    ) {
        plasma_destroy_session(session);
        return 164;
    }
    plasma_smoke_step_session_delta(session, &environment, 300UL);
    if (session->state.transition.active) {
        plasma_destroy_session(session);
        return 165;
    }
    plasma_destroy_session(session);

    plasma_config_set_defaults(&common_config, &product_config, sizeof(product_config));
    plasma_apply_preset_to_config("quiet_darkroom", &common_config, &product_config);
    common_config.preset_key = "quiet_darkroom";
    common_config.theme_key = "quiet_darkroom";
    plasma_transition_preferences_set_defaults(&product_config.transition);
    product_config.transition.enabled = 1;
    product_config.transition.policy = PLASMA_TRANSITION_POLICY_JOURNEY;
    product_config.transition.duration_millis = 600UL;
    product_config.transition.interval_millis = 900UL;
    lstrcpyA(product_config.transition.journey_key, "warm_bridge_cycle");
    screensave_config_binding_init(&binding, &common_config, &product_config, sizeof(product_config));
    ZeroMemory(&environment, sizeof(environment));
    fake_size.width = 320;
    fake_size.height = 240;
    plasma_smoke_init_fake_renderer(
        &fake_renderer,
        SCREENSAVE_RENDERER_KIND_GL21,
        SCREENSAVE_RENDERER_KIND_GL21,
        &fake_size
    );
    environment.mode = SCREENSAVE_SESSION_MODE_WINDOWED;
    environment.drawable_size = fake_size;
    environment.seed.base_seed = 0x818A8384UL;
    environment.seed.stream_seed = 0x858E8788UL;
    environment.seed.deterministic = common_config.use_deterministic_seed;
    environment.config_binding = &binding;
    environment.renderer = &fake_renderer;

    session = NULL;
    if (!plasma_create_session(module, &session, &environment) || session == NULL) {
        fprintf(stderr, "smoke: failed to create warm bridge journey session\n");
        return 243;
    }
    plasma_smoke_step_session_delta(session, &environment, 9000UL);
    if (
        !session->state.transition.active ||
        session->state.transition.active_type != PLASMA_TRANSITION_TYPE_BRIDGE_MORPH ||
        session->state.transition.requested_type != PLASMA_TRANSITION_TYPE_PRESET_MORPH ||
        session->state.transition.fallback_type != PLASMA_TRANSITION_TYPE_NONE ||
        session->state.transition.target_preset == NULL ||
        session->state.transition.target_theme == NULL ||
        strcmp(session->state.transition.target_preset->preset_key, "museum_phosphor") != 0 ||
        strcmp(session->state.transition.target_theme->theme_key, "plasma_lava") != 0
    ) {
        plasma_destroy_session(session);
        return 244;
    }
    benchlab_config_state.common = common_config;
    benchlab_config_state.product_config = &product_config;
    benchlab_config_state.product_config_size = sizeof(product_config);
    if (
        !plasma_benchlab_build_snapshot(
            session,
            &benchlab_config_state,
            SCREENSAVE_RENDERER_KIND_GL21,
            &benchlab_snapshot
        ) ||
        strcmp(benchlab_snapshot.transition_type, "bridge_morph") != 0 ||
        strcmp(benchlab_snapshot.transition_requested_type, "preset_morph") != 0 ||
        strcmp(benchlab_snapshot.transition_resolved_type, "bridge_morph") != 0 ||
        strcmp(benchlab_snapshot.transition_fallback_type, "none") != 0
    ) {
        plasma_destroy_session(session);
        return 245;
    }
    if (
        !plasma_benchlab_build_report_section(
            session,
            &benchlab_config_state,
            SCREENSAVE_RENDERER_KIND_GL21,
            benchlab_report,
            (unsigned int)sizeof(benchlab_report)
        ) ||
        strstr(benchlab_report, "Transition requested type: preset_morph") == NULL ||
        strstr(benchlab_report, "Transition resolved type: bridge_morph") == NULL ||
        strstr(benchlab_report, "Transition fallback type: none") == NULL
    ) {
        plasma_destroy_session(session);
        return 246;
    }
    plasma_smoke_step_session_delta(session, &environment, 600UL);
    if (session->state.transition.active) {
        /* Bridge morphs can commit the target before clearing active on the next tick. */
        plasma_smoke_step_session_delta(session, &environment, 1UL);
    }
    if (
        session->state.transition.active ||
        strcmp(session->plan.preset_key, "museum_phosphor") != 0 ||
        strcmp(session->plan.theme_key, "plasma_lava") != 0
    ) {
        fprintf(
            stderr,
            "smoke: warm bridge settle mismatch active=%d preset=%s theme=%s\n",
            session->state.transition.active,
            session->plan.preset_key != NULL ? session->plan.preset_key : "(null)",
            session->plan.theme_key != NULL ? session->plan.theme_key : "(null)"
        );
        plasma_destroy_session(session);
        return 247;
    }
    plasma_destroy_session(session);

    plasma_config_set_defaults(&common_config, &product_config, sizeof(product_config));
    plasma_apply_preset_to_config("aurora_plasma", &common_config, &product_config);
    common_config.preset_key = "aurora_plasma";
    common_config.theme_key = "aurora_cool";
    plasma_transition_preferences_set_defaults(&product_config.transition);
    product_config.transition.enabled = 1;
    product_config.transition.policy = PLASMA_TRANSITION_POLICY_JOURNEY;
    product_config.transition.duration_millis = 600UL;
    product_config.transition.interval_millis = 900UL;
    lstrcpyA(product_config.transition.journey_key, "cool_bridge_cycle");
    screensave_config_binding_init(&binding, &common_config, &product_config, sizeof(product_config));
    ZeroMemory(&environment, sizeof(environment));
    fake_size.width = 320;
    fake_size.height = 240;
    plasma_smoke_init_fake_renderer(
        &fake_renderer,
        SCREENSAVE_RENDERER_KIND_GL11,
        SCREENSAVE_RENDERER_KIND_GL11,
        &fake_size
    );
    environment.mode = SCREENSAVE_SESSION_MODE_WINDOWED;
    environment.drawable_size = fake_size;
    environment.seed.base_seed = 0x919A9394UL;
    environment.seed.stream_seed = 0x959E9798UL;
    environment.seed.deterministic = common_config.use_deterministic_seed;
    environment.config_binding = &binding;
    environment.renderer = &fake_renderer;

    session = NULL;
    if (!plasma_create_session(module, &session, &environment) || session == NULL) {
        return 248;
    }
    plasma_smoke_step_session_delta(session, &environment, 9000UL);
    if (
        !session->state.transition.active ||
        session->state.transition.active_type != PLASMA_TRANSITION_TYPE_FALLBACK ||
        session->state.transition.requested_type != PLASMA_TRANSITION_TYPE_PRESET_MORPH ||
        session->state.transition.fallback_type != PLASMA_TRANSITION_TYPE_THEME_MORPH ||
        session->state.transition.target_preset == NULL ||
        session->state.transition.target_theme == NULL ||
        strcmp(session->state.transition.target_preset->preset_key, "ocean_interference") != 0 ||
        strcmp(session->state.transition.target_theme->theme_key, "oceanic_blue") != 0
    ) {
        plasma_destroy_session(session);
        return 249;
    }
    benchlab_config_state.common = common_config;
    benchlab_config_state.product_config = &product_config;
    benchlab_config_state.product_config_size = sizeof(product_config);
    if (
        !plasma_benchlab_build_snapshot(
            session,
            &benchlab_config_state,
            SCREENSAVE_RENDERER_KIND_GL11,
            &benchlab_snapshot
        ) ||
        strcmp(benchlab_snapshot.transition_type, "fallback_transition") != 0 ||
        strcmp(benchlab_snapshot.transition_requested_type, "preset_morph") != 0 ||
        strcmp(benchlab_snapshot.transition_resolved_type, "fallback_transition") != 0 ||
        strcmp(benchlab_snapshot.transition_fallback_type, "theme_morph") != 0
    ) {
        plasma_destroy_session(session);
        return 250;
    }
    if (
        !plasma_benchlab_build_report_section(
            session,
            &benchlab_config_state,
            SCREENSAVE_RENDERER_KIND_GL11,
            benchlab_report,
            (unsigned int)sizeof(benchlab_report)
        ) ||
        strstr(benchlab_report, "Transition requested type: preset_morph") == NULL ||
        strstr(benchlab_report, "Transition resolved type: fallback_transition") == NULL ||
        strstr(benchlab_report, "Transition fallback type: theme_morph") == NULL
    ) {
        plasma_destroy_session(session);
        return 251;
    }
    plasma_smoke_step_session_delta(session, &environment, 600UL);
    if (
        session->state.transition.active ||
        strcmp(session->plan.preset_key, "ocean_interference") != 0 ||
        strcmp(session->plan.theme_key, "oceanic_blue") != 0
    ) {
        plasma_destroy_session(session);
        return 252;
    }
    plasma_destroy_session(session);

    plasma_config_set_defaults(&common_config, &product_config, sizeof(product_config));
    plasma_apply_preset_to_config("plasma_lava", &common_config, &product_config);
    common_config.preset_key = "plasma_lava";
    common_config.theme_key = "plasma_lava";
    plasma_transition_preferences_set_defaults(&product_config.transition);
    product_config.transition.enabled = 1;
    product_config.transition.policy = PLASMA_TRANSITION_POLICY_JOURNEY;
    product_config.transition.duration_millis = 600UL;
    product_config.transition.interval_millis = 900UL;
    lstrcpyA(product_config.transition.journey_key, "classic_cycle");
    screensave_config_binding_init(&binding, &common_config, &product_config, sizeof(product_config));
    ZeroMemory(&environment, sizeof(environment));
    fake_size.width = 320;
    fake_size.height = 240;
    plasma_smoke_init_fake_renderer(
        &fake_renderer,
        SCREENSAVE_RENDERER_KIND_GL21,
        SCREENSAVE_RENDERER_KIND_GL21,
        &fake_size
    );
    environment.mode = SCREENSAVE_SESSION_MODE_WINDOWED;
    environment.drawable_size = fake_size;
    environment.seed.base_seed = 0x81828384UL;
    environment.seed.stream_seed = 0x85868788UL;
    environment.seed.deterministic = common_config.use_deterministic_seed;
    environment.config_binding = &binding;
    environment.renderer = &fake_renderer;

    session = NULL;
    if (!plasma_create_session(module, &session, &environment) || session == NULL) {
        return 166;
    }
    plasma_smoke_step_session_delta(session, &environment, 9000UL);
    if (!session->state.transition.active) {
        plasma_destroy_session(session);
        return 253;
    }
    if (session->state.transition.active_type != PLASMA_TRANSITION_TYPE_PRESET_MORPH) {
        plasma_destroy_session(session);
        return 254;
    }
    if (session->state.transition.journey == NULL) {
        plasma_destroy_session(session);
        return 255;
    }
    if (session->state.transition.journey_step_index != 0U) {
        plasma_destroy_session(session);
        return 256;
    }
    plasma_smoke_step_session_delta(session, &environment, 600UL);
    if (
        session->state.transition.active ||
        session->state.transition.journey_step_index != 1U ||
        strcmp(session->plan.preset_key, "quiet_darkroom") != 0
    ) {
        plasma_destroy_session(session);
        return 257;
    }
    plasma_destroy_session(session);

    plasma_config_set_defaults(&common_config, &product_config, sizeof(product_config));
    screensave_config_binding_init(&binding, &common_config, &product_config, sizeof(product_config));
    ZeroMemory(&environment, sizeof(environment));
    fake_size.width = 320;
    fake_size.height = 240;
    plasma_smoke_init_fake_renderer(
        &fake_renderer,
        SCREENSAVE_RENDERER_KIND_GL21,
        SCREENSAVE_RENDERER_KIND_GL21,
        &fake_size
    );
    environment.mode = SCREENSAVE_SESSION_MODE_WINDOWED;
    environment.drawable_size = fake_size;
    environment.seed.base_seed = 0x01020304UL;
    environment.seed.stream_seed = 0x05060708UL;
    environment.seed.deterministic = common_config.use_deterministic_seed;
    environment.config_binding = &binding;
    environment.renderer = &fake_renderer;

    session = NULL;
    if (!plasma_create_session(module, &session, &environment) || session == NULL) {
        return 114;
    }
    if (
        session->state.active_renderer_kind != SCREENSAVE_RENDERER_KIND_GL21 ||
        session->plan.active_renderer_kind != SCREENSAVE_RENDERER_KIND_GL21 ||
        !session->plan.advanced_enabled ||
        session->plan.advanced_degraded ||
        !session->plan.modern_capable ||
        session->plan.modern_requested ||
        session->plan.modern_enabled ||
        session->plan.modern_degraded ||
        !session->plan.premium_capable ||
        session->plan.premium_requested ||
        session->plan.premium_enabled ||
        session->plan.premium_degraded ||
        session->state.field_history == NULL ||
        session->state.advanced_treatment_buffer.pixels == NULL ||
        session->state.modern_treatment_buffer.pixels != NULL ||
        session->state.modern_presentation_buffer.pixels != NULL ||
        session->state.premium_treatment_buffer.pixels != NULL ||
        session->state.premium_presentation_buffer.pixels != NULL ||
        !plasma_plan_validate_for_renderer_kind(&session->plan, module, SCREENSAVE_RENDERER_KIND_GL21) ||
        plasma_plan_validate_lower_band_baseline(&session->plan, module)
    ) {
        plasma_destroy_session(session);
        return 115;
    }

    environment.clock.delta_millis = 33UL;
    plasma_step_session(session, &environment);
    if (!plasma_output_build(&session->plan, &session->state, &output_frame)) {
        plasma_destroy_session(session);
        return 116;
    }
    if (
        !plasma_treatment_apply(
            &session->plan,
            &session->state,
            &output_frame,
            &session->state.visual_buffer,
            &treated_frame
        )
    ) {
        plasma_destroy_session(session);
        return 117;
    }
    if (
        !plasma_presentation_prepare(
            &session->plan,
            &session->state,
            &treated_frame,
            &presentation_target
        )
    ) {
        plasma_destroy_session(session);
        return 118;
    }
    plasma_render_session(session, &environment);
    if (
        session->state.visual_buffer.pixels == NULL ||
        session->state.advanced_treatment_buffer.pixels == NULL ||
        session->state.field_history == NULL ||
        session->state.modern_treatment_buffer.pixels != NULL ||
        session->state.modern_presentation_buffer.pixels != NULL ||
        session->state.premium_treatment_buffer.pixels != NULL ||
        session->state.premium_presentation_buffer.pixels != NULL
    ) {
        plasma_destroy_session(session);
        return 119;
    }

    plan = session->plan;
    plan.active_renderer_kind = SCREENSAVE_RENDERER_KIND_GL11;
    if (
        plasma_plan_validate_for_renderer_kind(&plan, module, SCREENSAVE_RENDERER_KIND_GL21) ||
        plasma_plan_validate(&plan, module)
    ) {
        plasma_destroy_session(session);
        return 120;
    }

    plasma_destroy_session(session);

    screensave_config_binding_init(&binding, &common_config, &product_config, sizeof(product_config));
    ZeroMemory(&environment, sizeof(environment));
    fake_size.width = 320;
    fake_size.height = 240;
    plasma_smoke_init_fake_renderer(
        &fake_renderer,
        SCREENSAVE_RENDERER_KIND_GL33,
        SCREENSAVE_RENDERER_KIND_GL33,
        &fake_size
    );
    environment.mode = SCREENSAVE_SESSION_MODE_WINDOWED;
    environment.drawable_size = fake_size;
    environment.seed.base_seed = 0x11121314UL;
    environment.seed.stream_seed = 0x15161718UL;
    environment.seed.deterministic = common_config.use_deterministic_seed;
    environment.config_binding = &binding;
    environment.renderer = &fake_renderer;

    session = NULL;
    if (!plasma_create_session(module, &session, &environment) || session == NULL) {
        return 121;
    }
    if (
        session->state.active_renderer_kind != SCREENSAVE_RENDERER_KIND_GL33 ||
        session->plan.active_renderer_kind != SCREENSAVE_RENDERER_KIND_GL33 ||
        !session->plan.advanced_enabled ||
        !session->plan.modern_enabled ||
        session->plan.advanced_degraded ||
        session->plan.modern_degraded ||
        !session->plan.premium_capable ||
        session->plan.premium_requested ||
        session->plan.premium_enabled ||
        session->plan.premium_degraded ||
        session->state.field_history == NULL ||
        session->state.advanced_treatment_buffer.pixels == NULL ||
        session->state.modern_treatment_buffer.pixels == NULL ||
        session->state.modern_presentation_buffer.pixels == NULL ||
        session->state.premium_treatment_buffer.pixels != NULL ||
        session->state.premium_presentation_buffer.pixels != NULL ||
        !plasma_plan_validate_for_renderer_kind(&session->plan, module, SCREENSAVE_RENDERER_KIND_GL33) ||
        plasma_plan_validate_for_renderer_kind(&session->plan, module, SCREENSAVE_RENDERER_KIND_GL21) ||
        plasma_plan_validate_for_renderer_kind(&session->plan, module, SCREENSAVE_RENDERER_KIND_GL46) ||
        plasma_plan_validate_lower_band_baseline(&session->plan, module)
    ) {
        plasma_destroy_session(session);
        return 122;
    }

    environment.clock.delta_millis = 33UL;
    plasma_step_session(session, &environment);
    if (!plasma_output_build(&session->plan, &session->state, &output_frame)) {
        plasma_destroy_session(session);
        return 123;
    }
    if (
        !plasma_treatment_apply(
            &session->plan,
            &session->state,
            &output_frame,
            &session->state.visual_buffer,
            &treated_frame
        )
    ) {
        plasma_destroy_session(session);
        return 124;
    }
    if (
        !plasma_presentation_prepare(
            &session->plan,
            &session->state,
            &treated_frame,
            &presentation_target
        )
    ) {
        plasma_destroy_session(session);
        return 125;
    }
    plasma_render_session(session, &environment);
    if (
        presentation_target.bitmap_view.pixels != (const void *)session->state.modern_presentation_buffer.pixels ||
        session->state.modern_treatment_buffer.pixels == NULL ||
        session->state.modern_presentation_buffer.pixels == NULL ||
        session->state.premium_treatment_buffer.pixels != NULL ||
        session->state.premium_presentation_buffer.pixels != NULL
    ) {
        plasma_destroy_session(session);
        return 126;
    }

    plan = session->plan;
    plan.active_renderer_kind = SCREENSAVE_RENDERER_KIND_GL21;
    if (
        plasma_plan_validate_for_renderer_kind(&plan, module, SCREENSAVE_RENDERER_KIND_GL33) ||
        plasma_plan_validate(&plan, module)
    ) {
        plasma_destroy_session(session);
        return 127;
    }

    plasma_destroy_session(session);

    screensave_config_binding_init(&binding, &common_config, &product_config, sizeof(product_config));
    ZeroMemory(&environment, sizeof(environment));
    fake_size.width = 320;
    fake_size.height = 240;
    plasma_smoke_init_fake_renderer(
        &fake_renderer,
        SCREENSAVE_RENDERER_KIND_GL46,
        SCREENSAVE_RENDERER_KIND_GL46,
        &fake_size
    );
    environment.mode = SCREENSAVE_SESSION_MODE_WINDOWED;
    environment.drawable_size = fake_size;
    environment.seed.base_seed = 0x21222324UL;
    environment.seed.stream_seed = 0x25262728UL;
    environment.seed.deterministic = common_config.use_deterministic_seed;
    environment.config_binding = &binding;
    environment.renderer = &fake_renderer;

    session = NULL;
    if (!plasma_create_session(module, &session, &environment) || session == NULL) {
        return 128;
    }
    if (
        session->state.active_renderer_kind != SCREENSAVE_RENDERER_KIND_GL46 ||
        session->plan.active_renderer_kind != SCREENSAVE_RENDERER_KIND_GL46 ||
        !session->plan.advanced_enabled ||
        !session->plan.modern_enabled ||
        !session->plan.premium_enabled ||
        session->plan.advanced_degraded ||
        session->plan.modern_degraded ||
        session->plan.premium_degraded ||
        session->plan.presentation_mode != PLASMA_PRESENTATION_MODE_FLAT ||
        session->state.field_history == NULL ||
        session->state.advanced_treatment_buffer.pixels == NULL ||
        session->state.modern_treatment_buffer.pixels == NULL ||
        session->state.modern_presentation_buffer.pixels == NULL ||
        session->state.premium_treatment_buffer.pixels == NULL ||
        session->state.premium_presentation_buffer.pixels == NULL ||
        !plasma_plan_validate_for_renderer_kind(&session->plan, module, SCREENSAVE_RENDERER_KIND_GL46) ||
        plasma_plan_validate_for_renderer_kind(&session->plan, module, SCREENSAVE_RENDERER_KIND_GL33) ||
        plasma_plan_validate_lower_band_baseline(&session->plan, module)
    ) {
        plasma_destroy_session(session);
        return 129;
    }

    environment.clock.delta_millis = 33UL;
    plasma_step_session(session, &environment);
    if (!plasma_output_build(&session->plan, &session->state, &output_frame)) {
        plasma_destroy_session(session);
        return 130;
    }
    if (
        !plasma_treatment_apply(
            &session->plan,
            &session->state,
            &output_frame,
            &session->state.visual_buffer,
            &treated_frame
        )
    ) {
        plasma_destroy_session(session);
        return 131;
    }
    if (
        !plasma_presentation_prepare(
            &session->plan,
            &session->state,
            &treated_frame,
            &presentation_target
        )
    ) {
        plasma_destroy_session(session);
        return 132;
    }
    plasma_render_session(session, &environment);
    if (
        presentation_target.bitmap_view.pixels != (const void *)session->state.modern_presentation_buffer.pixels ||
        presentation_target.bitmap_view.size.width != session->state.field_size.width ||
        presentation_target.bitmap_view.size.height != session->state.field_size.height ||
        session->state.premium_treatment_buffer.pixels == NULL ||
        session->state.premium_presentation_buffer.pixels == NULL
    ) {
        plasma_destroy_session(session);
        return 133;
    }

    plan = session->plan;
    plan.active_renderer_kind = SCREENSAVE_RENDERER_KIND_GL33;
    if (
        plasma_plan_validate_for_renderer_kind(&plan, module, SCREENSAVE_RENDERER_KIND_GL46) ||
        plasma_plan_validate(&plan, module)
    ) {
        plasma_destroy_session(session);
        return 134;
    }

    plasma_destroy_session(session);

    plasma_benchlab_forcing_set_defaults(&benchlab_forcing);
    if (
        benchlab_forcing.active ||
        benchlab_forcing.clamp_flags != 0UL ||
        benchlab_forcing.content_filter_override != -1 ||
        benchlab_forcing.favorites_only_override != -1 ||
        benchlab_forcing.transitions_enabled_override != -1 ||
        benchlab_forcing.transition_policy_override != -1 ||
        benchlab_forcing.transition_fallback_override != -1 ||
        benchlab_forcing.transition_seed_policy_override != -1 ||
        benchlab_forcing.presentation_request != PLASMA_BENCHLAB_PRESENTATION_AUTO
    ) {
        return 135;
    }

    if (
        !plasma_benchlab_parse_command_line(
            "/plasma-preset:ember_lava /plasma-theme:plasma_lava /plasma-preset-set:classic_core "
            "/plasma-theme-set:warm_classics /plasma-journey:classic_cycle "
            "/plasma-content-filter:stable_only /plasma-favorites-only:true "
            "/plasma-transitions:true /plasma-transition-policy:journey "
            "/plasma-transition-fallback:theme_morph /plasma-transition-seed:reseed_target "
            "/plasma-transition-interval:9000 /plasma-transition-duration:1800 "
            "/plasma-presentation:heightfield",
            &benchlab_forcing,
            NULL
        ) ||
        !benchlab_forcing.active ||
        strcmp(benchlab_forcing.preset_key, "plasma_lava") != 0 ||
        strcmp(benchlab_forcing.theme_key, "plasma_lava") != 0 ||
        strcmp(benchlab_forcing.preset_set_key, "classic_core") != 0 ||
        strcmp(benchlab_forcing.theme_set_key, "warm_classics") != 0 ||
        strcmp(benchlab_forcing.journey_key, "classic_cycle") != 0 ||
        benchlab_forcing.content_filter_override != (int)PLASMA_CONTENT_FILTER_STABLE_ONLY ||
        !benchlab_forcing.favorites_only_override ||
        !benchlab_forcing.transitions_enabled_override ||
        benchlab_forcing.transition_policy_override != (int)PLASMA_TRANSITION_POLICY_JOURNEY ||
        benchlab_forcing.transition_fallback_override != (int)PLASMA_TRANSITION_FALLBACK_THEME_MORPH ||
        benchlab_forcing.transition_seed_policy_override != (int)PLASMA_TRANSITION_SEED_CONTINUITY_RESEED_TARGET ||
        !benchlab_forcing.transition_interval_override_enabled ||
        benchlab_forcing.transition_interval_millis != 9000UL ||
        !benchlab_forcing.transition_duration_override_enabled ||
        benchlab_forcing.transition_duration_millis != 1800UL ||
        benchlab_forcing.presentation_request != PLASMA_BENCHLAB_PRESENTATION_HEIGHTFIELD
    ) {
        return 136;
    }

    if (
        !plasma_benchlab_parse_command_line(
            "/plasma-theme:not_real /plasma-transition-duration:5 /plasma-presentation:ribbon",
            &benchlab_forcing,
            NULL
        ) ||
        !benchlab_forcing.active ||
        benchlab_forcing.theme_key[0] != '\0' ||
        !benchlab_forcing.transition_duration_override_enabled ||
        benchlab_forcing.transition_duration_millis != 400UL ||
        benchlab_forcing.presentation_request != PLASMA_BENCHLAB_PRESENTATION_RIBBON ||
        (benchlab_forcing.clamp_flags & PLASMA_BENCHLAB_CLAMP_THEME_KEY) == 0UL ||
        (benchlab_forcing.clamp_flags & PLASMA_BENCHLAB_CLAMP_TRANSITION_DURATION) == 0UL ||
        (benchlab_forcing.clamp_flags & PLASMA_BENCHLAB_CLAMP_PRESENTATION) != 0UL
    ) {
        return 137;
    }

    plasma_config_set_defaults(&common_config, &product_config, sizeof(product_config));
    plasma_benchlab_forcing_set_defaults(&product_config.benchlab);
    if (!plasma_compile_direct_plan(module, NULL, NULL, &plan) || !plasma_plan_is_lower_band_baseline(&plan)) {
        return 138;
    }

    if (
        !plasma_benchlab_parse_command_line(
            "/plasma-preset:ember_lava /plasma-theme:plasma_lava /plasma-preset-set:classic_core "
            "/plasma-theme-set:warm_classics /plasma-journey:classic_cycle "
            "/plasma-content-filter:stable_only /plasma-favorites-only:true "
            "/plasma-transitions:true /plasma-transition-policy:journey "
            "/plasma-transition-fallback:theme_morph /plasma-transition-seed:reseed_target "
            "/plasma-transition-interval:9000 /plasma-transition-duration:1800 "
            "/plasma-presentation:heightfield",
            &benchlab_forcing,
            NULL
        )
    ) {
        return 139;
    }

    plasma_config_set_defaults(&common_config, &product_config, sizeof(product_config));
    product_config.benchlab = benchlab_forcing;
    plasma_benchlab_apply_forcing_to_config(&product_config.benchlab, &common_config, &product_config);
    plasma_config_clamp(&common_config, &product_config, sizeof(product_config));
    if (
        common_config.preset_key == NULL ||
        strcmp(common_config.preset_key, "plasma_lava") != 0 ||
        common_config.theme_key == NULL ||
        strcmp(common_config.theme_key, "plasma_lava") != 0 ||
        strcmp(product_config.selection.preset_set_key, "classic_core") != 0 ||
        strcmp(product_config.selection.theme_set_key, "warm_classics") != 0 ||
        !product_config.selection.favorites_only ||
        !product_config.transition.enabled ||
        product_config.transition.policy != PLASMA_TRANSITION_POLICY_JOURNEY ||
        product_config.transition.fallback_policy != PLASMA_TRANSITION_FALLBACK_THEME_MORPH ||
        product_config.transition.seed_policy != PLASMA_TRANSITION_SEED_CONTINUITY_RESEED_TARGET ||
        product_config.transition.interval_millis != 9000UL ||
        product_config.transition.duration_millis != 1800UL ||
        strcmp(product_config.transition.journey_key, "classic_cycle") != 0
    ) {
        return 140;
    }

    screensave_config_binding_init(&binding, &common_config, &product_config, sizeof(product_config));
    ZeroMemory(&environment, sizeof(environment));
    fake_size.width = 320;
    fake_size.height = 240;
    plasma_smoke_init_fake_renderer(
        &fake_renderer,
        SCREENSAVE_RENDERER_KIND_GL46,
        SCREENSAVE_RENDERER_KIND_GL46,
        &fake_size
    );
    environment.mode = SCREENSAVE_SESSION_MODE_WINDOWED;
    environment.drawable_size = fake_size;
    environment.seed.base_seed = 0x31323334UL;
    environment.seed.stream_seed = 0x35363738UL;
    environment.seed.deterministic = common_config.use_deterministic_seed;
    environment.config_binding = &binding;
    environment.renderer = &fake_renderer;

    session = NULL;
    if (!plasma_create_session(module, &session, &environment) || session == NULL) {
        return 141;
    }
    if (
        !session->plan.premium_enabled ||
        session->plan.presentation_mode != PLASMA_PRESENTATION_MODE_HEIGHTFIELD ||
        !session->plan.transition_requested ||
        !session->plan.transition_enabled ||
        session->plan.transition_policy != PLASMA_TRANSITION_POLICY_JOURNEY
    ) {
        plasma_destroy_session(session);
        return 142;
    }

    benchlab_config_state.common = common_config;
    benchlab_config_state.product_config = &product_config;
    benchlab_config_state.product_config_size = sizeof(product_config);
    if (
        !plasma_benchlab_build_snapshot(
            session,
            &benchlab_config_state,
            SCREENSAVE_RENDERER_KIND_GL46,
            &benchlab_snapshot
        ) ||
        strcmp(benchlab_snapshot.requested_lane, "premium") != 0 ||
        strcmp(benchlab_snapshot.resolved_lane, "premium") != 0 ||
        strcmp(benchlab_snapshot.degraded_from_lane, "none") != 0 ||
        strcmp(benchlab_snapshot.preset_key, "plasma_lava") != 0 ||
        strcmp(benchlab_snapshot.presentation_mode, "heightfield") != 0 ||
        !benchlab_snapshot.transition_requested ||
        !benchlab_snapshot.transition_enabled ||
        !benchlab_snapshot.forcing_active ||
        (benchlab_snapshot.clamp_flags & PLASMA_BENCHLAB_CLAMP_FAVORITES_ONLY) == 0UL
    ) {
        plasma_destroy_session(session);
        return 143;
    }
    if (
        !plasma_benchlab_build_overlay_summary(
            session,
            &benchlab_config_state,
            SCREENSAVE_RENDERER_KIND_GL46,
            benchlab_overlay,
            (unsigned int)sizeof(benchlab_overlay)
        ) ||
        strstr(benchlab_overlay, "Plasma BenchLab") == NULL ||
        strstr(benchlab_overlay, "Presentation: heightfield") == NULL ||
        strstr(benchlab_overlay, "Clamps: favorites_only") == NULL
    ) {
        plasma_destroy_session(session);
        return 144;
    }
    if (
        !plasma_benchlab_build_report_section(
            session,
            &benchlab_config_state,
            SCREENSAVE_RENDERER_KIND_GL46,
            benchlab_report,
            (unsigned int)sizeof(benchlab_report)
        ) ||
        strstr(benchlab_report, "Plasma BenchLab") == NULL ||
        strstr(benchlab_report, "Requested lane: premium") == NULL ||
        strstr(benchlab_report, "Requested presentation mode: heightfield") == NULL ||
        strstr(benchlab_report, "Presentation mode: heightfield") == NULL ||
        strstr(benchlab_report, "BenchLab forcing active: yes") == NULL
    ) {
        plasma_destroy_session(session);
        return 145;
    }
    plasma_destroy_session(session);

    plasma_config_set_defaults(&common_config, &product_config, sizeof(product_config));
    plasma_apply_preset_to_config("aurora_bands", &common_config, &product_config);
    common_config.preset_key = "aurora_bands";
    common_config.theme_key = "aurora_cool";
    product_config.selection.content_filter = PLASMA_CONTENT_FILTER_STABLE_AND_EXPERIMENTAL;
    screensave_config_binding_init(&binding, &common_config, &product_config, sizeof(product_config));
    ZeroMemory(&environment, sizeof(environment));
    fake_size.width = 320;
    fake_size.height = 240;
    plasma_smoke_init_fake_renderer(
        &fake_renderer,
        SCREENSAVE_RENDERER_KIND_GL11,
        SCREENSAVE_RENDERER_KIND_GL11,
        &fake_size
    );
    environment.mode = SCREENSAVE_SESSION_MODE_WINDOWED;
    environment.drawable_size = fake_size;
    environment.seed.base_seed = 0x41424344UL;
    environment.seed.stream_seed = 0x45464748UL;
    environment.seed.deterministic = common_config.use_deterministic_seed;
    environment.config_binding = &binding;
    environment.renderer = &fake_renderer;
    session = NULL;
    if (!plasma_create_session(module, &session, &environment) || session == NULL) {
        return 370;
    }
    benchlab_config_state.common = common_config;
    benchlab_config_state.product_config = &product_config;
    benchlab_config_state.product_config_size = sizeof(product_config);
    if (
        !plasma_benchlab_build_snapshot(
            session,
            &benchlab_config_state,
            SCREENSAVE_RENDERER_KIND_GL11,
            &benchlab_snapshot
        ) ||
        strcmp(benchlab_snapshot.profile_class, "experimental") != 0 ||
        strcmp(benchlab_snapshot.preset_key, "aurora_bands") != 0 ||
        strcmp(benchlab_snapshot.output_family, "banded") != 0 ||
        strcmp(benchlab_snapshot.output_mode, "posterized_bands") != 0 ||
        strcmp(benchlab_snapshot.filter_treatment, "glow_edge") != 0 ||
        strcmp(benchlab_snapshot.emulation_treatment, "none") != 0 ||
        strcmp(benchlab_snapshot.accent_treatment, "none") != 0
    ) {
        plasma_destroy_session(session);
        return 371;
    }
    if (
        !plasma_benchlab_build_overlay_summary(
            session,
            &benchlab_config_state,
            SCREENSAVE_RENDERER_KIND_GL11,
            benchlab_overlay,
            (unsigned int)sizeof(benchlab_overlay)
        ) ||
        strstr(benchlab_overlay, "Profile: experimental / safe") == NULL ||
        strstr(benchlab_overlay, "Output: banded / posterized_bands") == NULL ||
        strstr(benchlab_overlay, "Treatments: none | glow_edge | none | none") == NULL
    ) {
        plasma_destroy_session(session);
        return 372;
    }
    if (
        !plasma_benchlab_build_report_section(
            session,
            &benchlab_config_state,
            SCREENSAVE_RENDERER_KIND_GL11,
            benchlab_report,
            (unsigned int)sizeof(benchlab_report)
        ) ||
        strstr(benchlab_report, "Profile class: experimental") == NULL ||
        strstr(benchlab_report, "Requested generator family: plasma") == NULL ||
        strstr(benchlab_report, "Requested output family: banded") == NULL ||
        strstr(benchlab_report, "Output family: banded") == NULL ||
        strstr(benchlab_report, "Requested output mode: posterized_bands") == NULL ||
        strstr(benchlab_report, "Output mode: posterized_bands") == NULL ||
        strstr(benchlab_report, "Requested filter treatment: glow_edge") == NULL ||
        strstr(benchlab_report, "Filter treatment: glow_edge") == NULL ||
        strstr(benchlab_report, "Accent treatment: none") == NULL
    ) {
        plasma_destroy_session(session);
        return 373;
    }
    plasma_destroy_session(session);

    plasma_config_set_defaults(&common_config, &product_config, sizeof(product_config));
    product_config.benchlab = benchlab_forcing;
    plasma_benchlab_apply_forcing_to_config(&product_config.benchlab, &common_config, &product_config);
    plasma_config_clamp(&common_config, &product_config, sizeof(product_config));
    screensave_config_binding_init(&binding, &common_config, &product_config, sizeof(product_config));
    ZeroMemory(&environment, sizeof(environment));
    fake_size.width = 320;
    fake_size.height = 240;
    plasma_smoke_init_fake_renderer(
        &fake_renderer,
        SCREENSAVE_RENDERER_KIND_GL46,
        SCREENSAVE_RENDERER_KIND_GL11,
        &fake_size
    );
    environment.mode = SCREENSAVE_SESSION_MODE_WINDOWED;
    environment.drawable_size = fake_size;
    environment.seed.base_seed = 0x41424344UL;
    environment.seed.stream_seed = 0x45464748UL;
    environment.seed.deterministic = common_config.use_deterministic_seed;
    environment.config_binding = &binding;
    environment.renderer = &fake_renderer;

    session = NULL;
    if (!plasma_create_session(module, &session, &environment) || session == NULL) {
        return 146;
    }
    benchlab_config_state.common = common_config;
    benchlab_config_state.product_config = &product_config;
    benchlab_config_state.product_config_size = sizeof(product_config);
    if (
        session->plan.premium_enabled ||
        session->plan.presentation_mode != PLASMA_PRESENTATION_MODE_FLAT ||
        !plasma_benchlab_build_snapshot(
            session,
            &benchlab_config_state,
            SCREENSAVE_RENDERER_KIND_GL46,
            &benchlab_snapshot
        ) ||
        strcmp(benchlab_snapshot.requested_lane, "premium") != 0 ||
        strcmp(benchlab_snapshot.resolved_lane, "compat") != 0 ||
        strcmp(benchlab_snapshot.degraded_from_lane, "premium") != 0 ||
        strcmp(benchlab_snapshot.degraded_to_lane, "compat") != 0 ||
        (benchlab_snapshot.clamp_flags & PLASMA_BENCHLAB_CLAMP_PRESENTATION) == 0UL
    ) {
        plasma_destroy_session(session);
        return 147;
    }
    if (
        !plasma_benchlab_build_report_section(
            session,
            &benchlab_config_state,
            SCREENSAVE_RENDERER_KIND_GL46,
            benchlab_report,
            (unsigned int)sizeof(benchlab_report)
        ) ||
        strstr(benchlab_report, "Requested presentation mode: heightfield") == NULL ||
        strstr(benchlab_report, "Presentation mode: flat") == NULL
    ) {
        plasma_destroy_session(session);
        return 462;
    }
    plasma_destroy_session(session);

    matrix_entry = NULL;
    envelope_entry = NULL;
    known_limit_entry = NULL;
    if (
        !plasma_validation_matrix_is_consistent() ||
        !plasma_validation_performance_envelopes_are_consistent() ||
        !plasma_validation_known_limits_are_consistent()
    ) {
        return 201;
    }
    (void)plasma_validation_get_matrix(&matrix_count);
    (void)plasma_validation_get_performance_envelopes(&envelope_count);
    (void)plasma_validation_get_known_limits(&known_limit_count);
    if (matrix_count < 28U || envelope_count < 14U || known_limit_count < 16U) {
        return 202;
    }

    matrix_entry = plasma_validation_find_matrix_entry("default_stable_path", "gdi");
    if (
        matrix_entry == NULL ||
        matrix_entry->status != PLASMA_VALIDATION_STATUS_VALIDATED ||
        strcmp(plasma_validation_status_name(matrix_entry->status), "validated") != 0
    ) {
        return 203;
    }
    if (
        plasma_validation_find_matrix_entry("classic_default", "gdi") != matrix_entry ||
        strcmp(matrix_entry->area_key, "default_stable_path") != 0
    ) {
        return 457;
    }
    matrix_entry = plasma_validation_find_matrix_entry("premium_request_degrade", "auto");
    if (
        matrix_entry == NULL ||
        matrix_entry->status != PLASMA_VALIDATION_STATUS_VALIDATED
    ) {
        return 204;
    }
    matrix_entry = plasma_validation_find_matrix_entry("experimental_pool", "product");
    if (
        matrix_entry == NULL ||
        matrix_entry->status != PLASMA_VALIDATION_STATUS_PARTIAL
    ) {
        return 205;
    }
    matrix_entry = plasma_validation_find_matrix_entry("contour_output_subset", "product");
    if (
        matrix_entry == NULL ||
        matrix_entry->status != PLASMA_VALIDATION_STATUS_PARTIAL
    ) {
        return 374;
    }
    matrix_entry = plasma_validation_find_matrix_entry("banded_output_subset", "product");
    if (
        matrix_entry == NULL ||
        matrix_entry->status != PLASMA_VALIDATION_STATUS_PARTIAL
    ) {
        return 375;
    }
    matrix_entry = plasma_validation_find_matrix_entry("treatment_family_subset", "product");
    if (
        matrix_entry == NULL ||
        matrix_entry->status != PLASMA_VALIDATION_STATUS_PARTIAL
    ) {
        return 376;
    }
    matrix_entry = plasma_validation_find_matrix_entry("glyph_output_subset", "product");
    if (
        matrix_entry == NULL ||
        matrix_entry->status != PLASMA_VALIDATION_STATUS_PARTIAL
    ) {
        return 393;
    }
    matrix_entry = plasma_validation_find_matrix_entry("field_family_i_subset", "product");
    if (
        matrix_entry == NULL ||
        matrix_entry->status != PLASMA_VALIDATION_STATUS_PARTIAL
    ) {
        return 394;
    }
    matrix_entry = plasma_validation_find_matrix_entry("field_family_ii_subset", "product");
    if (
        matrix_entry == NULL ||
        matrix_entry->status != PLASMA_VALIDATION_STATUS_PARTIAL
    ) {
        return 419;
    }
    matrix_entry = plasma_validation_find_matrix_entry("dimensional_presentation_subset", "gl46");
    if (
        matrix_entry == NULL ||
        matrix_entry->status != PLASMA_VALIDATION_STATUS_PARTIAL
    ) {
        return 420;
    }
    matrix_entry = plasma_validation_find_matrix_entry("cross_driver_ladder", "all");
    if (
        matrix_entry == NULL ||
        matrix_entry->status != PLASMA_VALIDATION_STATUS_BLOCKED
    ) {
        return 206;
    }
    matrix_entry = plasma_validation_find_matrix_entry("hardware_matrix", "product");
    if (
        matrix_entry == NULL ||
        matrix_entry->status != PLASMA_VALIDATION_STATUS_PARTIAL
    ) {
        return 230;
    }
    matrix_entry = plasma_validation_find_matrix_entry("performance_soak_posture", "product");
    if (
        matrix_entry == NULL ||
        matrix_entry->status != PLASMA_VALIDATION_STATUS_PARTIAL
    ) {
        return 231;
    }
    matrix_entry = plasma_validation_find_matrix_entry("multi_monitor_baseline", "product");
    if (
        matrix_entry == NULL ||
        matrix_entry->status != PLASMA_VALIDATION_STATUS_PARTIAL
    ) {
        return 232;
    }
    matrix_entry = plasma_validation_find_matrix_entry("environment_safety_surface", "product");
    if (
        matrix_entry == NULL ||
        matrix_entry->status != PLASMA_VALIDATION_STATUS_PARTIAL
    ) {
        return 233;
    }
    matrix_entry = plasma_validation_find_matrix_entry("pack_provenance_surface", "product");
    if (
        matrix_entry == NULL ||
        matrix_entry->status != PLASMA_VALIDATION_STATUS_PARTIAL
    ) {
        return 246;
    }
    matrix_entry = plasma_validation_find_matrix_entry("authoring_substrate", "product");
    if (
        matrix_entry == NULL ||
        matrix_entry->status != PLASMA_VALIDATION_STATUS_PARTIAL
    ) {
        fprintf(stderr, "smoke: validation matrix missing or wrong for authoring_substrate\n");
        return 247;
    }
    matrix_entry = plasma_validation_find_matrix_entry("lab_shell_surface", "product");
    if (
        matrix_entry == NULL ||
        matrix_entry->status != PLASMA_VALIDATION_STATUS_PARTIAL
    ) {
        return 248;
    }
    matrix_entry = plasma_validation_find_matrix_entry("ops_toolchain_surface", "product");
    if (
        matrix_entry == NULL ||
        matrix_entry->status != PLASMA_VALIDATION_STATUS_PARTIAL
    ) {
        return 424;
    }
    matrix_entry = plasma_validation_find_matrix_entry("selection_foundation", "product");
    if (
        matrix_entry == NULL ||
        matrix_entry->status != PLASMA_VALIDATION_STATUS_PARTIAL
    ) {
        return 249;
    }

    envelope_entry = plasma_validation_find_performance_envelope("default_path_gdi");
    if (
        envelope_entry == NULL ||
        envelope_entry->status != PLASMA_VALIDATION_STATUS_VALIDATED ||
        strcmp(envelope_entry->lane_key, "gdi") != 0 ||
        plasma_validation_find_performance_envelope("classic_gdi") != envelope_entry
    ) {
        return 458;
    }
    envelope_entry = plasma_validation_find_performance_envelope("default_path_gl11");
    if (
        envelope_entry == NULL ||
        envelope_entry->status != PLASMA_VALIDATION_STATUS_VALIDATED ||
        strcmp(envelope_entry->lane_key, "gl11") != 0 ||
        plasma_validation_find_performance_envelope("classic_gl11") != envelope_entry
    ) {
        return 456;
    }
    envelope_entry = plasma_validation_find_performance_envelope("premium_gl46_heightfield");
    if (
        envelope_entry == NULL ||
        envelope_entry->status != PLASMA_VALIDATION_STATUS_VALIDATED ||
        strcmp(envelope_entry->lane_key, "gl46") != 0
    ) {
        return 207;
    }
    envelope_entry = plasma_validation_find_performance_envelope("transition_subset");
    if (
        envelope_entry == NULL ||
        envelope_entry->status != PLASMA_VALIDATION_STATUS_PARTIAL
    ) {
        return 208;
    }
    envelope_entry = plasma_validation_find_performance_envelope("lower_band_restart_soak");
    if (
        envelope_entry == NULL ||
        envelope_entry->status != PLASMA_VALIDATION_STATUS_VALIDATED ||
        strcmp(envelope_entry->measurement_mode, "measured") != 0
    ) {
        return 234;
    }
    envelope_entry = plasma_validation_find_performance_envelope("preview_safe_runtime");
    if (
        envelope_entry == NULL ||
        envelope_entry->status != PLASMA_VALIDATION_STATUS_PARTIAL
    ) {
        return 235;
    }
    envelope_entry = plasma_validation_find_performance_envelope("contour_and_banded_subset");
    if (
        envelope_entry == NULL ||
        envelope_entry->status != PLASMA_VALIDATION_STATUS_PARTIAL
    ) {
        return 377;
    }
    envelope_entry = plasma_validation_find_performance_envelope("treatment_subset");
    if (
        envelope_entry == NULL ||
        envelope_entry->status != PLASMA_VALIDATION_STATUS_PARTIAL
    ) {
        return 378;
    }
    envelope_entry = plasma_validation_find_performance_envelope("glyph_subset");
    if (
        envelope_entry == NULL ||
        envelope_entry->status != PLASMA_VALIDATION_STATUS_PARTIAL
    ) {
        return 395;
    }
    envelope_entry = plasma_validation_find_performance_envelope("field_family_i_subset");
    if (
        envelope_entry == NULL ||
        envelope_entry->status != PLASMA_VALIDATION_STATUS_PARTIAL
    ) {
        return 396;
    }
    envelope_entry = plasma_validation_find_performance_envelope("field_family_ii_subset");
    if (
        envelope_entry == NULL ||
        envelope_entry->status != PLASMA_VALIDATION_STATUS_PARTIAL
    ) {
        return 421;
    }
    envelope_entry = plasma_validation_find_performance_envelope("premium_dimensional_subset");
    if (
        envelope_entry == NULL ||
        envelope_entry->status != PLASMA_VALIDATION_STATUS_PARTIAL
    ) {
        return 422;
    }

    known_limit_entry = plasma_validation_find_known_limit("experimental_pool_bounded");
    if (
        known_limit_entry == NULL ||
        known_limit_entry->status != PLASMA_VALIDATION_STATUS_PARTIAL
    ) {
        return 379;
    }
    known_limit_entry = plasma_validation_find_known_limit("dimensional_presentation_subset_bounded");
    if (
        known_limit_entry == NULL ||
        known_limit_entry->status != PLASMA_VALIDATION_STATUS_PARTIAL
    ) {
        return 209;
    }
    known_limit_entry = plasma_validation_find_known_limit("cross_hardware_gap");
    if (
        known_limit_entry == NULL ||
        known_limit_entry->status != PLASMA_VALIDATION_STATUS_BLOCKED
    ) {
        return 210;
    }
    known_limit_entry = plasma_validation_find_known_limit("multi_monitor_baseline_bounded");
    if (
        known_limit_entry == NULL ||
        known_limit_entry->status != PLASMA_VALIDATION_STATUS_PARTIAL
    ) {
        return 236;
    }
    known_limit_entry = plasma_validation_find_known_limit("environment_safety_bounded");
    if (
        known_limit_entry == NULL ||
        known_limit_entry->status != PLASMA_VALIDATION_STATUS_PARTIAL
    ) {
        return 237;
    }
    known_limit_entry = plasma_validation_find_known_limit("authored_registry_partial");
    if (
        known_limit_entry == NULL ||
        known_limit_entry->status != PLASMA_VALIDATION_STATUS_PARTIAL
    ) {
        return 250;
    }
    known_limit_entry = plasma_validation_find_known_limit("lab_shell_cli_only");
    if (
        known_limit_entry == NULL ||
        known_limit_entry->status != PLASMA_VALIDATION_STATUS_PARTIAL
    ) {
        return 251;
    }
    known_limit_entry = plasma_validation_find_known_limit("capture_diff_bounded");
    if (
        known_limit_entry == NULL ||
        known_limit_entry->status != PLASMA_VALIDATION_STATUS_PARTIAL
    ) {
        return 425;
    }
    known_limit_entry = plasma_validation_find_known_limit("output_family_subset_bounded");
    if (
        known_limit_entry == NULL ||
        known_limit_entry->status != PLASMA_VALIDATION_STATUS_PARTIAL
    ) {
        return 380;
    }
    known_limit_entry = plasma_validation_find_known_limit("treatment_family_subset_bounded");
    if (
        known_limit_entry == NULL ||
        known_limit_entry->status != PLASMA_VALIDATION_STATUS_PARTIAL
    ) {
        return 381;
    }
    known_limit_entry = plasma_validation_find_known_limit("glyph_output_subset_bounded");
    if (
        known_limit_entry == NULL ||
        known_limit_entry->status != PLASMA_VALIDATION_STATUS_PARTIAL
    ) {
        return 397;
    }
    known_limit_entry = plasma_validation_find_known_limit("field_family_i_subset_bounded");
    if (
        known_limit_entry == NULL ||
        known_limit_entry->status != PLASMA_VALIDATION_STATUS_PARTIAL
    ) {
        return 398;
    }
    known_limit_entry = plasma_validation_find_known_limit("field_family_ii_subset_bounded");
    if (
        known_limit_entry == NULL ||
        known_limit_entry->status != PLASMA_VALIDATION_STATUS_PARTIAL
    ) {
        return 423;
    }
    known_limit_entry = plasma_validation_find_known_limit("selection_foundation_bounded");
    if (
        known_limit_entry == NULL ||
        known_limit_entry->status != PLASMA_VALIDATION_STATUS_PARTIAL
    ) {
        return 252;
    }

    plasma_smoke_render_signature_init(&baseline_render_signature);
    plasma_smoke_render_signature_init(&variant_render_signature);

    plasma_config_set_defaults(&common_config, &product_config, sizeof(product_config));
    common_config.use_deterministic_seed = 1;
    common_config.deterministic_seed = 0x2468ACE0UL;
    common_config.preset_key = "plasma_lava";
    common_config.theme_key = "plasma_lava";
    common_config.detail_level = SCREENSAVE_DETAIL_LEVEL_STANDARD;
    product_config.effect_mode = PLASMA_EFFECT_PLASMA;
    product_config.speed_mode = PLASMA_SPEED_GENTLE;
    product_config.resolution_mode = PLASMA_RESOLUTION_STANDARD;
    product_config.smoothing_mode = PLASMA_SMOOTHING_SOFT;
    product_config.output_family = PLASMA_OUTPUT_FAMILY_RASTER;
    product_config.output_mode = PLASMA_OUTPUT_MODE_NATIVE_RASTER;
    product_config.sampling_treatment = PLASMA_SAMPLING_TREATMENT_NONE;
    product_config.filter_treatment = PLASMA_FILTER_TREATMENT_NONE;
    product_config.emulation_treatment = PLASMA_EMULATION_TREATMENT_NONE;
    product_config.accent_treatment = PLASMA_ACCENT_TREATMENT_NONE;
    product_config.presentation_mode = PLASMA_PRESENTATION_MODE_FLAT;

    if (
        !plasma_smoke_capture_render_signature(
            module,
            &common_config,
            &product_config,
            SCREENSAVE_RENDERER_KIND_GDI,
            SCREENSAVE_RENDERER_KIND_GDI,
            250UL,
            &baseline_render_signature
        ) ||
        baseline_render_signature.treated_hash == 0UL ||
        baseline_render_signature.presented_hash == 0UL ||
        baseline_render_signature.treated_lit_pixels == 0U ||
        baseline_render_signature.presented_lit_pixels == 0U
    ) {
        plasma_smoke_render_signature_release(&baseline_render_signature);
        plasma_smoke_render_signature_release(&variant_render_signature);
        return 441;
    }

    product_config.effect_mode = PLASMA_EFFECT_INTERFERENCE;
    if (
        !plasma_smoke_capture_render_signature(
            module,
            &common_config,
            &product_config,
            SCREENSAVE_RENDERER_KIND_GDI,
            SCREENSAVE_RENDERER_KIND_GDI,
            250UL,
            &variant_render_signature
        ) ||
        !plasma_smoke_signatures_meaningfully_different(
            &baseline_render_signature,
            &variant_render_signature,
            0,
            128U
        )
    ) {
        plasma_smoke_render_signature_release(&baseline_render_signature);
        plasma_smoke_render_signature_release(&variant_render_signature);
        return 442;
    }
    plasma_smoke_render_signature_release(&variant_render_signature);

    product_config.effect_mode = PLASMA_EFFECT_PLASMA;
    common_config.detail_level = SCREENSAVE_DETAIL_LEVEL_HIGH;
    if (
        !plasma_smoke_capture_render_signature(
            module,
            &common_config,
            &product_config,
            SCREENSAVE_RENDERER_KIND_GDI,
            SCREENSAVE_RENDERER_KIND_GDI,
            250UL,
            &variant_render_signature
        ) ||
        !plasma_smoke_signatures_meaningfully_different(
            &baseline_render_signature,
            &variant_render_signature,
            0,
            64U
        )
    ) {
        plasma_smoke_render_signature_release(&baseline_render_signature);
        plasma_smoke_render_signature_release(&variant_render_signature);
        return 443;
    }
    plasma_smoke_render_signature_release(&variant_render_signature);

    common_config.detail_level = SCREENSAVE_DETAIL_LEVEL_STANDARD;
    product_config.speed_mode = PLASMA_SPEED_LIVELY;
    if (
        !plasma_smoke_capture_render_signature(
            module,
            &common_config,
            &product_config,
            SCREENSAVE_RENDERER_KIND_GDI,
            SCREENSAVE_RENDERER_KIND_GDI,
            250UL,
            &variant_render_signature
        ) ||
        !plasma_smoke_signatures_meaningfully_different(
            &baseline_render_signature,
            &variant_render_signature,
            0,
            64U
        )
    ) {
        plasma_smoke_render_signature_release(&baseline_render_signature);
        plasma_smoke_render_signature_release(&variant_render_signature);
        return 444;
    }
    plasma_smoke_render_signature_release(&variant_render_signature);

    product_config.speed_mode = PLASMA_SPEED_GENTLE;
    product_config.resolution_mode = PLASMA_RESOLUTION_FINE;
    if (
        !plasma_smoke_capture_render_signature(
            module,
            &common_config,
            &product_config,
            SCREENSAVE_RENDERER_KIND_GDI,
            SCREENSAVE_RENDERER_KIND_GDI,
            250UL,
            &variant_render_signature
        ) ||
        !plasma_smoke_signatures_meaningfully_different(
            &baseline_render_signature,
            &variant_render_signature,
            0,
            128U
        )
    ) {
        plasma_smoke_render_signature_release(&baseline_render_signature);
        plasma_smoke_render_signature_release(&variant_render_signature);
        return 445;
    }
    plasma_smoke_render_signature_release(&variant_render_signature);

    product_config.resolution_mode = PLASMA_RESOLUTION_STANDARD;
    product_config.smoothing_mode = PLASMA_SMOOTHING_GLOW;
    if (
        !plasma_smoke_capture_render_signature(
            module,
            &common_config,
            &product_config,
            SCREENSAVE_RENDERER_KIND_GDI,
            SCREENSAVE_RENDERER_KIND_GDI,
            250UL,
            &variant_render_signature
        ) ||
        !plasma_smoke_signatures_meaningfully_different(
            &baseline_render_signature,
            &variant_render_signature,
            0,
            64U
        )
    ) {
        plasma_smoke_render_signature_release(&baseline_render_signature);
        plasma_smoke_render_signature_release(&variant_render_signature);
        return 446;
    }
    plasma_smoke_render_signature_release(&variant_render_signature);

    product_config.smoothing_mode = PLASMA_SMOOTHING_SOFT;
    product_config.output_family = PLASMA_OUTPUT_FAMILY_CONTOUR;
    product_config.output_mode = PLASMA_OUTPUT_MODE_CONTOUR_BANDS;
    if (
        !plasma_smoke_capture_render_signature(
            module,
            &common_config,
            &product_config,
            SCREENSAVE_RENDERER_KIND_GDI,
            SCREENSAVE_RENDERER_KIND_GDI,
            250UL,
            &variant_render_signature
        ) ||
        !plasma_smoke_signatures_meaningfully_different(
            &baseline_render_signature,
            &variant_render_signature,
            0,
            128U
        )
    ) {
        plasma_smoke_render_signature_release(&baseline_render_signature);
        plasma_smoke_render_signature_release(&variant_render_signature);
        return 447;
    }
    plasma_smoke_render_signature_release(&variant_render_signature);

    product_config.output_family = PLASMA_OUTPUT_FAMILY_BANDED;
    product_config.output_mode = PLASMA_OUTPUT_MODE_POSTERIZED_BANDS;
    if (
        !plasma_smoke_capture_render_signature(
            module,
            &common_config,
            &product_config,
            SCREENSAVE_RENDERER_KIND_GDI,
            SCREENSAVE_RENDERER_KIND_GDI,
            250UL,
            &variant_render_signature
        ) ||
        !plasma_smoke_signatures_meaningfully_different(
            &baseline_render_signature,
            &variant_render_signature,
            0,
            128U
        )
    ) {
        plasma_smoke_render_signature_release(&baseline_render_signature);
        plasma_smoke_render_signature_release(&variant_render_signature);
        return 464;
    }
    plasma_smoke_render_signature_release(&variant_render_signature);

    product_config.output_family = PLASMA_OUTPUT_FAMILY_GLYPH;
    product_config.output_mode = PLASMA_OUTPUT_MODE_ASCII_GLYPH;
    if (
        !plasma_smoke_capture_render_signature(
            module,
            &common_config,
            &product_config,
            SCREENSAVE_RENDERER_KIND_GDI,
            SCREENSAVE_RENDERER_KIND_GDI,
            250UL,
            &variant_render_signature
        ) ||
        !plasma_smoke_signatures_meaningfully_different(
            &baseline_render_signature,
            &variant_render_signature,
            0,
            128U
        )
    ) {
        plasma_smoke_render_signature_release(&baseline_render_signature);
        plasma_smoke_render_signature_release(&variant_render_signature);
        return 465;
    }
    plasma_smoke_render_signature_release(&variant_render_signature);

    product_config.output_family = PLASMA_OUTPUT_FAMILY_RASTER;
    product_config.output_mode = PLASMA_OUTPUT_MODE_NATIVE_RASTER;
    product_config.filter_treatment = PLASMA_FILTER_TREATMENT_GLOW_EDGE;
    if (
        !plasma_smoke_capture_render_signature(
            module,
            &common_config,
            &product_config,
            SCREENSAVE_RENDERER_KIND_GDI,
            SCREENSAVE_RENDERER_KIND_GDI,
            250UL,
            &variant_render_signature
        ) ||
        !plasma_smoke_signatures_meaningfully_different(
            &baseline_render_signature,
            &variant_render_signature,
            0,
            128U
        )
    ) {
        plasma_smoke_render_signature_release(&baseline_render_signature);
        plasma_smoke_render_signature_release(&variant_render_signature);
        return 448;
    }
    plasma_smoke_render_signature_release(&variant_render_signature);

    product_config.filter_treatment = PLASMA_FILTER_TREATMENT_NONE;
    product_config.emulation_treatment = PLASMA_EMULATION_TREATMENT_PHOSPHOR;
    if (
        !plasma_smoke_capture_render_signature(
            module,
            &common_config,
            &product_config,
            SCREENSAVE_RENDERER_KIND_GDI,
            SCREENSAVE_RENDERER_KIND_GDI,
            250UL,
            &variant_render_signature
        ) ||
        !plasma_smoke_signatures_meaningfully_different(
            &baseline_render_signature,
            &variant_render_signature,
            0,
            128U
        )
    ) {
        plasma_smoke_render_signature_release(&baseline_render_signature);
        plasma_smoke_render_signature_release(&variant_render_signature);
        return 449;
    }
    plasma_smoke_render_signature_release(&variant_render_signature);

    product_config.emulation_treatment = PLASMA_EMULATION_TREATMENT_NONE;
    product_config.filter_treatment = PLASMA_FILTER_TREATMENT_HALFTONE_STIPPLE;
    if (
        !plasma_smoke_capture_render_signature(
            module,
            &common_config,
            &product_config,
            SCREENSAVE_RENDERER_KIND_GDI,
            SCREENSAVE_RENDERER_KIND_GDI,
            250UL,
            &variant_render_signature
        ) ||
        !plasma_smoke_signatures_meaningfully_different(
            &baseline_render_signature,
            &variant_render_signature,
            0,
            128U
        )
    ) {
        plasma_smoke_render_signature_release(&baseline_render_signature);
        plasma_smoke_render_signature_release(&variant_render_signature);
        return 469;
    }
    plasma_smoke_render_signature_release(&variant_render_signature);

    product_config.filter_treatment = PLASMA_FILTER_TREATMENT_NONE;
    product_config.emulation_treatment = PLASMA_EMULATION_TREATMENT_CRT;
    if (
        !plasma_smoke_capture_render_signature(
            module,
            &common_config,
            &product_config,
            SCREENSAVE_RENDERER_KIND_GDI,
            SCREENSAVE_RENDERER_KIND_GDI,
            250UL,
            &variant_render_signature
        ) ||
        !plasma_smoke_signatures_meaningfully_different(
            &baseline_render_signature,
            &variant_render_signature,
            0,
            64U
        )
    ) {
        plasma_smoke_render_signature_release(&baseline_render_signature);
        plasma_smoke_render_signature_release(&variant_render_signature);
        return 470;
    }
    plasma_smoke_render_signature_release(&variant_render_signature);

    product_config.emulation_treatment = PLASMA_EMULATION_TREATMENT_NONE;
    product_config.accent_treatment = PLASMA_ACCENT_TREATMENT_ACCENT_PASS;
    if (
        !plasma_smoke_capture_render_signature(
            module,
            &common_config,
            &product_config,
            SCREENSAVE_RENDERER_KIND_GDI,
            SCREENSAVE_RENDERER_KIND_GDI,
            250UL,
            &variant_render_signature
        ) ||
        !plasma_smoke_signatures_meaningfully_different(
            &baseline_render_signature,
            &variant_render_signature,
            0,
            64U
        )
    ) {
        plasma_smoke_render_signature_release(&baseline_render_signature);
        plasma_smoke_render_signature_release(&variant_render_signature);
        return 450;
    }
    plasma_smoke_render_signature_release(&variant_render_signature);
    plasma_smoke_render_signature_release(&baseline_render_signature);

    plasma_smoke_render_signature_init(&baseline_render_signature);
    plasma_smoke_render_signature_init(&variant_render_signature);
    plasma_config_set_defaults(&common_config, &product_config, sizeof(product_config));
    common_config.use_deterministic_seed = 1;
    common_config.deterministic_seed = 0x11223344UL;
    common_config.preset_key = "aurora_curtain";
    common_config.theme_key = "aurora_cool";
    common_config.detail_level = SCREENSAVE_DETAIL_LEVEL_HIGH;
    product_config.effect_mode = PLASMA_EFFECT_AURORA;
    product_config.speed_mode = PLASMA_SPEED_STANDARD;
    product_config.resolution_mode = PLASMA_RESOLUTION_FINE;
    product_config.smoothing_mode = PLASMA_SMOOTHING_OFF;
    product_config.output_family = PLASMA_OUTPUT_FAMILY_BANDED;
    product_config.output_mode = PLASMA_OUTPUT_MODE_POSTERIZED_BANDS;
    product_config.sampling_treatment = PLASMA_SAMPLING_TREATMENT_NONE;
    product_config.filter_treatment = PLASMA_FILTER_TREATMENT_NONE;
    product_config.emulation_treatment = PLASMA_EMULATION_TREATMENT_NONE;
    product_config.accent_treatment = PLASMA_ACCENT_TREATMENT_NONE;
    product_config.presentation_mode = PLASMA_PRESENTATION_MODE_FLAT;

    if (
        !plasma_smoke_capture_render_signature(
            module,
            &common_config,
            &product_config,
            SCREENSAVE_RENDERER_KIND_GL46,
            SCREENSAVE_RENDERER_KIND_GL46,
            250UL,
            &baseline_render_signature
        )
    ) {
        plasma_smoke_render_signature_release(&baseline_render_signature);
        plasma_smoke_render_signature_release(&variant_render_signature);
        return 451;
    }

    product_config.presentation_mode = PLASMA_PRESENTATION_MODE_RIBBON;
    if (
        !plasma_smoke_capture_render_signature(
            module,
            &common_config,
            &product_config,
            SCREENSAVE_RENDERER_KIND_GL46,
            SCREENSAVE_RENDERER_KIND_GL46,
            250UL,
            &variant_render_signature
        ) ||
        !plasma_smoke_signatures_meaningfully_different(
            &baseline_render_signature,
            &variant_render_signature,
            1,
            128U
        )
    ) {
        plasma_smoke_render_signature_release(&baseline_render_signature);
        plasma_smoke_render_signature_release(&variant_render_signature);
        return 452;
    }
    plasma_smoke_render_signature_release(&baseline_render_signature);
    plasma_smoke_render_signature_release(&variant_render_signature);

    plasma_config_set_defaults(&common_config, &product_config, sizeof(product_config));
    screensave_config_binding_init(&binding, &common_config, &product_config, sizeof(product_config));
    ZeroMemory(&preview_environment, sizeof(preview_environment));
    ZeroMemory(&windowed_environment, sizeof(windowed_environment));
    fake_size.width = 320;
    fake_size.height = 240;
    plasma_smoke_init_fake_renderer(
        &preview_renderer,
        SCREENSAVE_RENDERER_KIND_GDI,
        SCREENSAVE_RENDERER_KIND_GDI,
        &fake_size
    );
    plasma_smoke_init_fake_renderer(
        &windowed_renderer,
        SCREENSAVE_RENDERER_KIND_GDI,
        SCREENSAVE_RENDERER_KIND_GDI,
        &fake_size
    );
    preview_environment.mode = SCREENSAVE_SESSION_MODE_PREVIEW;
    preview_environment.drawable_size = fake_size;
    preview_environment.seed.base_seed = 0x71727374UL;
    preview_environment.seed.stream_seed = 0x75767778UL;
    preview_environment.seed.deterministic = common_config.use_deterministic_seed;
    preview_environment.config_binding = &binding;
    preview_environment.renderer = &preview_renderer;
    windowed_environment = preview_environment;
    windowed_environment.mode = SCREENSAVE_SESSION_MODE_WINDOWED;
    windowed_environment.renderer = &windowed_renderer;

    preview_session = NULL;
    windowed_session = NULL;
    if (
        !plasma_create_session(module, &preview_session, &preview_environment) ||
        preview_session == NULL ||
        !plasma_create_session(module, &windowed_session, &windowed_environment) ||
        windowed_session == NULL
    ) {
        if (preview_session != NULL) {
            plasma_destroy_session(preview_session);
        }
        if (windowed_session != NULL) {
            plasma_destroy_session(windowed_session);
        }
        return 238;
    }
    if (
        !preview_session->state.preview_mode ||
        windowed_session->state.preview_mode ||
        preview_session->state.active_renderer_kind != SCREENSAVE_RENDERER_KIND_GDI ||
        windowed_session->state.active_renderer_kind != SCREENSAVE_RENDERER_KIND_GDI ||
        !plasma_plan_validate_lower_band_baseline(&preview_session->plan, module) ||
        !plasma_plan_validate_lower_band_baseline(&windowed_session->plan, module) ||
        preview_session->state.field_size.width >= windowed_session->state.field_size.width ||
        preview_session->state.field_size.height >= windowed_session->state.field_size.height
    ) {
        plasma_destroy_session(preview_session);
        plasma_destroy_session(windowed_session);
        return 239;
    }

    preview_phase_before = preview_session->state.phase_millis;
    windowed_phase_before = windowed_session->state.phase_millis;
    plasma_smoke_step_session_delta(preview_session, &preview_environment, 1000UL);
    plasma_smoke_step_session_delta(windowed_session, &windowed_environment, 1000UL);
    preview_phase_after = preview_session->state.phase_millis;
    windowed_phase_after = windowed_session->state.phase_millis;
    if (
        preview_phase_after <= preview_phase_before ||
        windowed_phase_after <= windowed_phase_before ||
        (preview_phase_after - preview_phase_before) >=
            (windowed_phase_after - windowed_phase_before)
    ) {
        plasma_destroy_session(preview_session);
        plasma_destroy_session(windowed_session);
        return 240;
    }
    plasma_destroy_session(preview_session);
    plasma_destroy_session(windowed_session);

    for (soak_iteration = 0U; soak_iteration < 12U; ++soak_iteration) {
        soak_renderer_kind =
            (soak_iteration & 1U) == 0U
                ? SCREENSAVE_RENDERER_KIND_GDI
                : SCREENSAVE_RENDERER_KIND_GL11;
        plasma_config_set_defaults(&common_config, &product_config, sizeof(product_config));
        screensave_config_binding_init(&binding, &common_config, &product_config, sizeof(product_config));
        ZeroMemory(&environment, sizeof(environment));
        fake_size.width = 320;
        fake_size.height = 240;
        plasma_smoke_init_fake_renderer(
            &fake_renderer,
            soak_renderer_kind,
            soak_renderer_kind,
            &fake_size
        );
        environment.mode = SCREENSAVE_SESSION_MODE_WINDOWED;
        environment.drawable_size = fake_size;
        environment.seed.base_seed = 0x51510000UL + (unsigned long)soak_iteration;
        environment.seed.stream_seed = 0x61610000UL + (unsigned long)soak_iteration;
        environment.seed.deterministic = common_config.use_deterministic_seed;
        environment.config_binding = &binding;
        environment.renderer = &fake_renderer;

        session = NULL;
        if (!plasma_create_session(module, &session, &environment) || session == NULL) {
            return 211;
        }
        if (!plasma_plan_validate_for_renderer_kind(&session->plan, module, soak_renderer_kind)) {
            plasma_destroy_session(session);
            return 212;
        }

        for (index = 0U; index < 24U; ++index) {
            plasma_smoke_step_session_delta(session, &environment, 33UL);
            plasma_render_session(session, &environment);
            if (!plasma_plan_validate_for_renderer_kind(&session->plan, module, soak_renderer_kind)) {
                plasma_destroy_session(session);
                return 213;
            }
        }
        if (session->state.phase_millis == 0UL) {
            plasma_destroy_session(session);
            return 214;
        }
        plasma_destroy_session(session);
    }

    plasma_config_set_defaults(&common_config, &product_config, sizeof(product_config));
    plasma_apply_preset_to_config("plasma_lava", &common_config, &product_config);
    common_config.preset_key = "plasma_lava";
    common_config.theme_key = "plasma_lava";
    plasma_transition_preferences_set_defaults(&product_config.transition);
    product_config.transition.enabled = 1;
    product_config.transition.policy = PLASMA_TRANSITION_POLICY_JOURNEY;
    product_config.transition.duration_millis = 600UL;
    product_config.transition.interval_millis = 900UL;
    lstrcpyA(product_config.transition.journey_key, "classic_cycle");
    screensave_config_binding_init(&binding, &common_config, &product_config, sizeof(product_config));
    ZeroMemory(&environment, sizeof(environment));
    fake_size.width = 320;
    fake_size.height = 240;
    plasma_smoke_init_fake_renderer(
        &fake_renderer,
        SCREENSAVE_RENDERER_KIND_GL21,
        SCREENSAVE_RENDERER_KIND_GL21,
        &fake_size
    );
    environment.mode = SCREENSAVE_SESSION_MODE_WINDOWED;
    environment.drawable_size = fake_size;
    environment.seed.base_seed = 0x91929394UL;
    environment.seed.stream_seed = 0x95969798UL;
    environment.seed.deterministic = common_config.use_deterministic_seed;
    environment.config_binding = &binding;
    environment.renderer = &fake_renderer;

    session = NULL;
    if (!plasma_create_session(module, &session, &environment) || session == NULL) {
        return 215;
    }
    for (soak_iteration = 0U; soak_iteration < 8U; ++soak_iteration) {
        unsigned long dwell_millis;
        plasma_transition_type active_type;

        if (
            session->state.transition.journey == NULL ||
            session->state.transition.journey->step_count == 0U
        ) {
            plasma_destroy_session(session);
            return 216;
        }

        expected_journey_index =
            (session->state.transition.journey_step_index + 1U) %
            session->state.transition.journey->step_count;
        dwell_millis =
            session->state.transition.journey->steps[
                session->state.transition.journey_step_index %
                session->state.transition.journey->step_count
            ].dwell_millis;
        if (dwell_millis == 0UL) {
            dwell_millis = session->plan.transition_interval_millis;
        }

        plasma_smoke_step_session_delta(session, &environment, dwell_millis);
        plasma_render_session(session, &environment);
        active_type = session->state.transition.active_type;
        if (
            !plasma_plan_validate_for_renderer_kind(&session->plan, module, SCREENSAVE_RENDERER_KIND_GL21) ||
            !session->plan.transition_requested ||
            !session->plan.transition_enabled ||
            !session->state.transition.active ||
            (
                active_type != PLASMA_TRANSITION_TYPE_PRESET_MORPH &&
                active_type != PLASMA_TRANSITION_TYPE_BRIDGE_MORPH
            ) ||
            session->state.transition.journey == NULL
        ) {
            plasma_destroy_session(session);
            return 216;
        }

        settle_delta = session->plan.transition_duration_millis / 2UL;
        if (settle_delta == 0UL) {
            settle_delta = session->plan.transition_duration_millis;
        }
        for (settle_iteration = 0U; settle_iteration < 4U; ++settle_iteration) {
            if (!session->state.transition.active) {
                break;
            }

            plasma_smoke_step_session_delta(session, &environment, settle_delta);
            plasma_render_session(session, &environment);
        }
        if (
            !plasma_plan_validate_for_renderer_kind(&session->plan, module, SCREENSAVE_RENDERER_KIND_GL21) ||
            session->state.transition.active ||
            session->state.transition.journey == NULL ||
            session->state.transition.journey_step_index != expected_journey_index
        ) {
            plasma_destroy_session(session);
            return 217;
        }
    }
    if (
        session->plan.transition_policy != PLASMA_TRANSITION_POLICY_JOURNEY ||
        session->state.phase_millis == 0UL
    ) {
        plasma_destroy_session(session);
        return 218;
    }
    plasma_destroy_session(session);

    return 0;
}
