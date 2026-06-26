#include "ricochet_internal.h"

#include <stdlib.h>
#include <string.h>

typedef struct ricochet_v1_draw_bridge_tag {
    screensave_renderer *renderer;
} ricochet_v1_draw_bridge;

void ricochet_rng_seed(ricochet_rng_state *state, unsigned long seed)
{
    if (state == NULL) {
        return;
    }
    state->state = seed != 0UL ? seed : 0x09C0C710UL;
}

unsigned long ricochet_rng_next(ricochet_rng_state *state)
{
    if (state == NULL) {
        return 0UL;
    }
    state->state = state->state * 1664525UL + 1013904223UL;
    return state->state;
}

unsigned long ricochet_rng_range(ricochet_rng_state *state, unsigned long upper_bound)
{
    if (upper_bound == 0UL) {
        return 0UL;
    }
    return ricochet_rng_next(state) % upper_bound;
}

static ss_v2_color_rgba8 ricochet_v1_color(screensave_color color)
{
    ss_v2_color_rgba8 converted;

    converted.struct_size = (ss_u32)sizeof(converted);
    converted.abi_version = SS_V2_ABI_VERSION;
    converted.red = color.red;
    converted.green = color.green;
    converted.blue = color.blue;
    converted.alpha = color.alpha;
    return converted;
}

static screensave_color ricochet_v1_renderer_color(const ss_v2_color_rgba8 *color)
{
    screensave_color converted;

    converted.red = color->red;
    converted.green = color->green;
    converted.blue = color->blue;
    converted.alpha = color->alpha;
    return converted;
}

static ss_u32 ricochet_v1_renderer_clear(void *user_data, const ss_v2_color_rgba8 *color)
{
    ricochet_v1_draw_bridge *bridge;

    bridge = (ricochet_v1_draw_bridge *)user_data;
    if (bridge == NULL || bridge->renderer == NULL || color == NULL) {
        return SS_V2_STATUS_BAD_ARGUMENT;
    }
    screensave_renderer_clear(bridge->renderer, ricochet_v1_renderer_color(color));
    return SS_V2_STATUS_OK;
}

static ss_u32 ricochet_v1_renderer_fill_rect(
    void *user_data,
    const ss_v2_recti *rect,
    const ss_v2_color_rgba8 *color)
{
    ricochet_v1_draw_bridge *bridge;
    screensave_recti converted;

    bridge = (ricochet_v1_draw_bridge *)user_data;
    if (bridge == NULL || bridge->renderer == NULL || rect == NULL || color == NULL) {
        return SS_V2_STATUS_BAD_ARGUMENT;
    }
    converted.x = (int)rect->x;
    converted.y = (int)rect->y;
    converted.width = (int)rect->width;
    converted.height = (int)rect->height;
    screensave_renderer_fill_rect(bridge->renderer, &converted, ricochet_v1_renderer_color(color));
    return SS_V2_STATUS_OK;
}

static ss_u32 ricochet_v1_renderer_frame_rect(
    void *user_data,
    const ss_v2_recti *rect,
    const ss_v2_color_rgba8 *color)
{
    ricochet_v1_draw_bridge *bridge;
    screensave_recti converted;

    bridge = (ricochet_v1_draw_bridge *)user_data;
    if (bridge == NULL || bridge->renderer == NULL || rect == NULL || color == NULL) {
        return SS_V2_STATUS_BAD_ARGUMENT;
    }
    converted.x = (int)rect->x;
    converted.y = (int)rect->y;
    converted.width = (int)rect->width;
    converted.height = (int)rect->height;
    screensave_renderer_draw_frame_rect(bridge->renderer, &converted, ricochet_v1_renderer_color(color));
    return SS_V2_STATUS_OK;
}

static ss_u32 ricochet_v1_renderer_line(
    void *user_data,
    const ss_v2_pointi *start_point,
    const ss_v2_pointi *end_point,
    const ss_v2_color_rgba8 *color)
{
    ricochet_v1_draw_bridge *bridge;
    screensave_pointi converted_start;
    screensave_pointi converted_end;

    bridge = (ricochet_v1_draw_bridge *)user_data;
    if (bridge == NULL || bridge->renderer == NULL || start_point == NULL || end_point == NULL || color == NULL) {
        return SS_V2_STATUS_BAD_ARGUMENT;
    }
    converted_start.x = (int)start_point->x;
    converted_start.y = (int)start_point->y;
    converted_end.x = (int)end_point->x;
    converted_end.y = (int)end_point->y;
    screensave_renderer_draw_line(bridge->renderer, &converted_start, &converted_end, ricochet_v1_renderer_color(color));
    return SS_V2_STATUS_OK;
}

static ss_u32 ricochet_v1_renderer_polyline(
    void *user_data,
    const ss_v2_pointi *points,
    ss_u32 point_count,
    const ss_v2_color_rgba8 *color)
{
    ricochet_v1_draw_bridge *bridge;
    screensave_pointi converted[8];
    ss_u32 index;

    bridge = (ricochet_v1_draw_bridge *)user_data;
    if (bridge == NULL || bridge->renderer == NULL || points == NULL || color == NULL || point_count > 8U) {
        return SS_V2_STATUS_BAD_ARGUMENT;
    }
    for (index = 0U; index < point_count; ++index) {
        converted[index].x = (int)points[index].x;
        converted[index].y = (int)points[index].y;
    }
    screensave_renderer_draw_polyline(bridge->renderer, converted, (unsigned int)point_count, ricochet_v1_renderer_color(color));
    return SS_V2_STATUS_OK;
}

static const ss_v2_draw_ops g_ricochet_v1_draw_ops = {
    (ss_u32)sizeof(ss_v2_draw_ops),
    SS_V2_ABI_VERSION,
    SS_V2_DRAW_CAP_CLEAR |
        SS_V2_DRAW_CAP_FILL_RECT |
        SS_V2_DRAW_CAP_FRAME_RECT |
        SS_V2_DRAW_CAP_LINE |
        SS_V2_DRAW_CAP_POLYLINE,
    ricochet_v1_renderer_clear,
    ricochet_v1_renderer_fill_rect,
    ricochet_v1_renderer_frame_rect,
    ricochet_v1_renderer_line,
    ricochet_v1_renderer_polyline
};

static const screensave_theme_descriptor *ricochet_resolve_theme(const screensave_saver_environment *environment)
{
    const screensave_common_config *common_config;
    const screensave_theme_descriptor *theme;

    if (environment == NULL || environment->config_binding == NULL) {
        return ricochet_find_theme_descriptor(RICOCHET_DEFAULT_THEME_KEY);
    }
    common_config = environment->config_binding->common_config;
    if (common_config == NULL) {
        return ricochet_find_theme_descriptor(RICOCHET_DEFAULT_THEME_KEY);
    }
    theme = ricochet_find_theme_descriptor(common_config->theme_key);
    if (theme == NULL) {
        theme = ricochet_find_theme_descriptor(RICOCHET_DEFAULT_THEME_KEY);
    }
    return theme;
}

static void ricochet_core_theme_from_v1(
    const screensave_theme_descriptor *theme,
    ricochet_core_theme *core_theme)
{
    screensave_color fallback_primary;
    screensave_color fallback_accent;

    if (core_theme == NULL) {
        return;
    }
    if (theme == NULL) {
        theme = ricochet_find_theme_descriptor(RICOCHET_DEFAULT_THEME_KEY);
    }
    if (theme != NULL) {
        core_theme->primary_color = ricochet_v1_color(theme->primary_color);
        core_theme->accent_color = ricochet_v1_color(theme->accent_color);
        return;
    }

    fallback_primary.red = 232;
    fallback_primary.green = 232;
    fallback_primary.blue = 228;
    fallback_primary.alpha = 255;
    fallback_accent.red = 80;
    fallback_accent.green = 180;
    fallback_accent.blue = 255;
    fallback_accent.alpha = 255;
    core_theme->primary_color = ricochet_v1_color(fallback_primary);
    core_theme->accent_color = ricochet_v1_color(fallback_accent);
}

static const ricochet_config *ricochet_resolve_config(
    const screensave_saver_environment *environment,
    ricochet_config *fallback)
{
    if (fallback != NULL) {
        fallback->object_mode = RICOCHET_OBJECT_BLOCK;
        fallback->object_count = 1;
        fallback->speed_mode = RICOCHET_SPEED_STANDARD;
        fallback->trail_mode = RICOCHET_TRAIL_SHORT;
    }
    if (
        environment != NULL &&
        environment->config_binding != NULL &&
        environment->config_binding->product_config != NULL &&
        environment->config_binding->product_config_size == sizeof(ricochet_config)
    ) {
        return (const ricochet_config *)environment->config_binding->product_config;
    }
    return fallback;
}

static ss_u32 ricochet_detail_from_environment(const screensave_saver_environment *environment)
{
    if (environment != NULL && environment->config_binding != NULL && environment->config_binding->common_config != NULL) {
        return (ss_u32)environment->config_binding->common_config->detail_level;
    }
    return RICOCHET_DETAIL_STANDARD;
}

static ss_u32 ricochet_preview_from_environment(const screensave_saver_environment *environment)
{
    return environment != NULL && environment->mode == SCREENSAVE_SESSION_MODE_PREVIEW ? 1U : 0U;
}

int ricochet_create_session(
    const screensave_saver_module *module,
    screensave_saver_session **session_out,
    const screensave_saver_environment *environment)
{
    screensave_saver_session *session;
    ricochet_config fallback_config;
    ricochet_core_theme core_theme;
    ricochet_core_desc desc;

    (void)module;
    if (session_out == NULL || environment == NULL) {
        return 0;
    }
    *session_out = NULL;
    session = (screensave_saver_session *)malloc(sizeof(*session));
    if (session == NULL) {
        return 0;
    }
    memset(session, 0, sizeof(*session));
    ricochet_core_theme_from_v1(ricochet_resolve_theme(environment), &core_theme);

    desc.config = ricochet_resolve_config(environment, &fallback_config);
    desc.theme = &core_theme;
    desc.width = (ss_u32)environment->drawable_size.width;
    desc.height = (ss_u32)environment->drawable_size.height;
    desc.detail_level = ricochet_detail_from_environment(environment);
    desc.preview_mode = ricochet_preview_from_environment(environment);
    desc.base_seed = (ss_u32)environment->seed.base_seed;
    desc.stream_seed = (ss_u32)environment->seed.stream_seed;
    if (!ricochet_core_create(&desc, &session->core)) {
        free(session);
        return 0;
    }
    *session_out = session;
    return 1;
}

void ricochet_destroy_session(screensave_saver_session *session)
{
    if (session == NULL) {
        return;
    }
    ricochet_core_destroy(session->core);
    free(session);
}

void ricochet_resize_session(
    screensave_saver_session *session,
    const screensave_saver_environment *environment)
{
    if (session == NULL || session->core == NULL || environment == NULL) {
        return;
    }
    ricochet_core_resize(
        session->core,
        (ss_u32)environment->drawable_size.width,
        (ss_u32)environment->drawable_size.height,
        ricochet_preview_from_environment(environment));
}

void ricochet_step_session(
    screensave_saver_session *session,
    const screensave_saver_environment *environment)
{
    if (session == NULL || session->core == NULL || environment == NULL) {
        return;
    }
    ricochet_core_advance(session->core, (ss_u32)environment->clock.delta_millis);
}

void ricochet_render_session(
    screensave_saver_session *session,
    const screensave_saver_environment *environment)
{
    ricochet_v1_draw_bridge bridge;
    ss_v2_draw_target target;

    if (session == NULL || session->core == NULL || environment == NULL || environment->renderer == NULL) {
        return;
    }
    bridge.renderer = environment->renderer;
    target.struct_size = (ss_u32)sizeof(target);
    target.abi_version = SS_V2_ABI_VERSION;
    target.user_data = &bridge;
    target.ops = &g_ricochet_v1_draw_ops;
    target.surface = 0;
    (void)ricochet_core_render(session->core, &target);
}
