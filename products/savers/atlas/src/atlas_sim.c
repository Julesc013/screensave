#include <stdlib.h>
#include <string.h>

#include "atlas_internal.h"

static const atlas_waypoint g_atlas_voyage_points[ATLAS_ROUTE_POINT_COUNT] = {
    { -0.7436438870, -0.1318259040, 0.0900000000, 0.0, 0.0 },
    { -0.7436438870, -0.1318259040, 0.0280000000, 0.0, 0.0 },
    { -0.7452900000, 0.1130200000, 0.0500000000, 0.0, 0.0 },
    { -0.1011000000, 0.9563000000, 0.1800000000, 0.0, 0.0 },
    { -1.2492000000, 0.0200000000, 0.2200000000, 0.0, 0.0 }
};

static const atlas_waypoint g_atlas_atlas_points[ATLAS_ROUTE_POINT_COUNT] = {
    { -0.5000000000, 0.0000000000, 1.4000000000, 0.0, 0.0 },
    { -1.2600000000, 0.0200000000, 0.2600000000, 0.0, 0.0 },
    { 0.3000000000, 0.5600000000, 0.2000000000, 0.0, 0.0 },
    { -0.1500000000, -0.6500000000, 0.1800000000, 0.0, 0.0 },
    { -0.7600000000, 0.1200000000, 0.1000000000, 0.0, 0.0 }
};

static const atlas_waypoint g_atlas_julia_points[ATLAS_ROUTE_POINT_COUNT] = {
    { 0.0000000000, 0.0000000000, 1.6000000000, -0.8000000000, 0.1560000000 },
    { 0.0000000000, 0.0000000000, 1.6000000000, -0.7269000000, 0.1889000000 },
    { 0.0000000000, 0.0000000000, 1.5500000000, 0.2850000000, 0.0100000000 },
    { 0.0000000000, 0.0000000000, 1.5000000000, -0.4000000000, 0.6000000000 },
    { 0.0000000000, 0.0000000000, 1.6000000000, -0.7017600000, -0.3842000000 }
};

static const screensave_theme_descriptor *atlas_resolve_theme(const screensave_config_binding *binding)
{
    if (binding != NULL && binding->common_config != NULL) {
        return atlas_find_theme_descriptor(binding->common_config->theme_key);
    }

    return atlas_find_theme_descriptor(ATLAS_DEFAULT_THEME_KEY);
}

static const atlas_config *atlas_resolve_config(const screensave_config_binding *binding)
{
    if (binding == NULL || binding->product_config == NULL || binding->product_config_size != sizeof(atlas_config)) {
        return NULL;
    }

    return (const atlas_config *)binding->product_config;
}

static unsigned long atlas_step_interval(const screensave_saver_session *session)
{
    if (session == NULL) {
        return 66UL;
    }

    switch (session->config.speed_mode) {
    case ATLAS_SPEED_STILL:
        return 90UL;
    case ATLAS_SPEED_BRISK:
        return 28UL;
    case ATLAS_SPEED_STANDARD:
    default:
        return 52UL;
    }
}

static unsigned int atlas_hold_ticks(const screensave_saver_session *session)
{
    if (session == NULL) {
        return 80U;
    }

    switch (session->config.speed_mode) {
    case ATLAS_SPEED_STILL:
        return session->config.mode == ATLAS_MODE_ATLAS ? 170U : 120U;
    case ATLAS_SPEED_BRISK:
        return 56U;
    case ATLAS_SPEED_STANDARD:
    default:
        return session->config.mode == ATLAS_MODE_ATLAS ? 110U : 78U;
    }
}

static unsigned int atlas_rows_per_step(const screensave_saver_session *session)
{
    unsigned int rows;

    if (session == NULL) {
        return 4U;
    }

    switch (session->config.refinement_mode) {
    case ATLAS_REFINEMENT_DRAFT:
        rows = 8U;
        break;
    case ATLAS_REFINEMENT_FINE:
        rows = 3U;
        break;
    case ATLAS_REFINEMENT_STANDARD:
    default:
        rows = 5U;
        break;
    }

    if (session->detail_level == SCREENSAVE_DETAIL_LEVEL_HIGH && rows > 1U) {
        rows -= 1U;
    }
    if (session->detail_level == SCREENSAVE_DETAIL_LEVEL_LOW) {
        rows += 2U;
    }
    if (session->preview_mode) {
        rows += 2U;
    }

    return rows;
}

static int atlas_target_width(const screensave_saver_session *session)
{
    int width;

    if (session == NULL) {
        return 128;
    }

    width = 144;
    if (session->detail_level == SCREENSAVE_DETAIL_LEVEL_LOW) {
        width = 112;
    } else if (session->detail_level == SCREENSAVE_DETAIL_LEVEL_HIGH) {
        width = 188;
    }

    if (session->config.refinement_mode == ATLAS_REFINEMENT_DRAFT) {
        width -= 24;
    } else if (session->config.refinement_mode == ATLAS_REFINEMENT_FINE) {
        width += 24;
    }

    if (session->preview_mode && width > 116) {
        width = 116;
    }

    if (width < 72) {
        width = 72;
    }
    return width;
}

static void atlas_pick_field_size(screensave_saver_session *session)
{
    int width;
    int height;

    if (session == NULL) {
        return;
    }

    width = atlas_target_width(session);
    if (session->drawable_size.width > 0 && session->drawable_size.height > 0) {
        height = (width * session->drawable_size.height) / session->drawable_size.width;
    } else {
        height = (width * 3) / 4;
    }
    if (height < 54) {
        height = 54;
    }

    session->field_size.width = width;
    session->field_size.height = height;
}

static int atlas_iteration_limit(const screensave_saver_session *session)
{
    int limit;

    if (session == NULL) {
        return 72;
    }

    limit = 72;
    if (session->detail_level == SCREENSAVE_DETAIL_LEVEL_LOW) {
        limit = 48;
    } else if (session->detail_level == SCREENSAVE_DETAIL_LEVEL_HIGH) {
        limit = 112;
    }

    if (session->config.refinement_mode == ATLAS_REFINEMENT_DRAFT) {
        limit -= 16;
    } else if (session->config.refinement_mode == ATLAS_REFINEMENT_FINE) {
        limit += 24;
    }

    if (session->config.mode == ATLAS_MODE_JULIA) {
        limit += 8;
    }
    if (session->preview_mode && limit > 84) {
        limit = 84;
    }
    if (limit < 24) {
        limit = 24;
    }
    return limit;
}

static void atlas_zero_escape_values(screensave_saver_session *session)
{
    size_t count;

    if (session == NULL || session->escape_values == NULL) {
        return;
    }

    count = (size_t)session->field_size.width * (size_t)session->field_size.height;
    memset(session->escape_values, 0, count * sizeof(unsigned short));
}

static int atlas_allocate_escape_values(screensave_saver_session *session)
{
    unsigned short *values;
    size_t count;

    if (session == NULL || session->field_size.width <= 0 || session->field_size.height <= 0) {
        return 0;
    }

    count = (size_t)session->field_size.width * (size_t)session->field_size.height;
    values = (unsigned short *)malloc(count * sizeof(unsigned short));
    if (values == NULL) {
        return 0;
    }

    memset(values, 0, count * sizeof(unsigned short));
    free(session->escape_values);
    session->escape_values = values;
    return 1;
}

static const atlas_waypoint *atlas_active_route(const screensave_saver_session *session)
{
    if (session == NULL) {
        return g_atlas_atlas_points;
    }

    switch (session->config.mode) {
    case ATLAS_MODE_VOYAGE:
        return g_atlas_voyage_points;
    case ATLAS_MODE_JULIA:
        return g_atlas_julia_points;
    case ATLAS_MODE_ATLAS:
    default:
        return g_atlas_atlas_points;
    }
}

static void atlas_refresh_view(screensave_saver_session *session)
{
    const atlas_waypoint *route;
    unsigned int index;

    if (session == NULL) {
        return;
    }

    route = atlas_active_route(session);
    index = session->route_index % ATLAS_ROUTE_POINT_COUNT;
    session->current_view = route[index];
}

static void atlas_restart_refinement(screensave_saver_session *session)
{
    screensave_color clear_color;

    if (session == NULL) {
        return;
    }

    session->refinement_row = 0;
    session->refresh_pending = 0;
    atlas_zero_escape_values(session);
    clear_color.red = 0;
    clear_color.green = 0;
    clear_color.blue = 0;
    clear_color.alpha = 255;
    screensave_visual_buffer_clear(&session->visual_buffer, clear_color);
}

static int atlas_prepare_buffers(screensave_saver_session *session)
{
    if (session == NULL) {
        return 0;
    }

    atlas_pick_field_size(session);
    session->max_iterations = atlas_iteration_limit(session);
    if (session->visual_buffer.pixels == NULL) {
        if (!screensave_visual_buffer_init(&session->visual_buffer, &session->field_size)) {
            return 0;
        }
    } else if (!screensave_visual_buffer_resize(&session->visual_buffer, &session->field_size)) {
        return 0;
    }

    if (!atlas_allocate_escape_values(session)) {
        return 0;
    }

    return 1;
}

static unsigned short atlas_sample_escape(
    const screensave_saver_session *session,
    int x,
    int y
)
{
    double real;
    double imag;
    double z_real;
    double z_imag;
    double c_real;
    double c_imag;
    double span_x;
    double span_y;
    double x_ratio;
    double y_ratio;
    int iteration;

    if (session == NULL || session->field_size.width <= 1 || session->field_size.height <= 1) {
        return 0U;
    }

    span_x = session->current_view.scale;
    span_y = session->current_view.scale *
        ((double)session->field_size.height / (double)session->field_size.width);
    x_ratio = ((double)x / (double)(session->field_size.width - 1)) - 0.5;
    y_ratio = ((double)y / (double)(session->field_size.height - 1)) - 0.5;
    real = session->current_view.center_x + (x_ratio * span_x * 2.0);
    imag = session->current_view.center_y + (y_ratio * span_y * 2.0);

    if (session->config.mode == ATLAS_MODE_JULIA) {
        z_real = real;
        z_imag = imag;
        c_real = session->current_view.julia_real;
        c_imag = session->current_view.julia_imag;
    } else {
        z_real = 0.0;
        z_imag = 0.0;
        c_real = real;
        c_imag = imag;
    }

    for (iteration = 0; iteration < session->max_iterations; ++iteration) {
        double next_real;
        double next_imag;

        next_real = (z_real * z_real) - (z_imag * z_imag) + c_real;
        next_imag = (2.0 * z_real * z_imag) + c_imag;
        z_real = next_real;
        z_imag = next_imag;
        if ((z_real * z_real) + (z_imag * z_imag) > 4.0) {
            return (unsigned short)iteration;
        }
    }

    return (unsigned short)session->max_iterations;
}

static void atlas_refine_rows(screensave_saver_session *session, unsigned int row_count)
{
    unsigned int row_index;

    if (session == NULL || session->escape_values == NULL || session->refinement_row >= session->field_size.height) {
        return;
    }

    for (row_index = 0U; row_index < row_count; ++row_index) {
        int x;
        int y;

        y = session->refinement_row;
        if (y >= session->field_size.height) {
            break;
        }
        for (x = 0; x < session->field_size.width; ++x) {
            session->escape_values[(y * session->field_size.width) + x] =
                atlas_sample_escape(session, x, y);
        }
        session->refinement_row += 1;
    }
}

static void atlas_advance_route(screensave_saver_session *session)
{
    if (session == NULL) {
        return;
    }

    session->route_index = (session->route_index + 1U) % ATLAS_ROUTE_POINT_COUNT;
    session->route_phase = 0U;
    atlas_refresh_view(session);
    atlas_restart_refinement(session);
}

static void atlas_run_tick(screensave_saver_session *session)
{
    if (session == NULL) {
        return;
    }

    session->palette_phase = (session->palette_phase + 1U + (unsigned int)session->config.speed_mode) & 255U;
    if (session->refinement_row < session->field_size.height) {
        atlas_refine_rows(session, atlas_rows_per_step(session));
        return;
    }

    session->route_phase += 1U;
    if (session->route_phase >= atlas_hold_ticks(session)) {
        atlas_advance_route(session);
    }
}

static void atlas_initialize_session(
    screensave_saver_session *session,
    const atlas_config *config,
    screensave_detail_level detail_level,
    const screensave_saver_environment *environment
)
{
    unsigned long seed;

    if (session == NULL || config == NULL || environment == NULL) {
        return;
    }

    session->config = *config;
    session->theme = atlas_resolve_theme(environment->config_binding);
    session->drawable_size = environment->drawable_size;
    session->detail_level = detail_level;
    session->preview_mode = environment->mode == SCREENSAVE_SESSION_MODE_PREVIEW;
    seed = environment->seed.stream_seed != 0UL ? environment->seed.stream_seed : 0x0F4A71A5UL;
    atlas_rng_seed(&session->rng, seed);
    session->step_accumulator = 0UL;
    session->phase_counter = 0UL;
    session->route_index = (unsigned int)atlas_rng_range(&session->rng, ATLAS_ROUTE_POINT_COUNT);
    session->route_phase = 0U;
    session->palette_phase = (unsigned int)atlas_rng_range(&session->rng, 256UL);
    session->escape_values = NULL;
    ZeroMemory(&session->visual_buffer, sizeof(session->visual_buffer));

    if (!atlas_prepare_buffers(session)) {
        session->field_size.width = 0;
        session->field_size.height = 0;
        return;
    }

    atlas_refresh_view(session);
    atlas_restart_refinement(session);
}

void atlas_rng_seed(atlas_rng_state *state, unsigned long seed)
{
    if (state == NULL) {
        return;
    }

    state->state = seed != 0UL ? seed : 0x13579BDFUL;
}

unsigned long atlas_rng_next(atlas_rng_state *state)
{
    if (state == NULL) {
        return 0UL;
    }

    state->state = state->state * 1664525UL + 1013904223UL;
    return state->state;
}

unsigned long atlas_rng_range(atlas_rng_state *state, unsigned long upper_bound)
{
    if (upper_bound == 0UL) {
        return 0UL;
    }

    return atlas_rng_next(state) % upper_bound;
}

int atlas_create_session(
    const screensave_saver_module *module,
    screensave_saver_session **session_out,
    const screensave_saver_environment *environment
)
{
    screensave_saver_session *session;
    atlas_config config;
    const atlas_config *bound_config;
    screensave_common_config safe_common;
    atlas_config safe_config;

    (void)module;

    if (session_out == NULL || environment == NULL) {
        return 0;
    }

    atlas_config_set_defaults(&safe_common, &config, sizeof(config));
    bound_config = atlas_resolve_config(environment->config_binding);
    if (bound_config != NULL) {
        config = *bound_config;
    }

    if (environment->config_binding != NULL && environment->config_binding->common_config != NULL) {
        safe_common = *environment->config_binding->common_config;
    } else {
        screensave_common_config_set_defaults(&safe_common);
    }
    safe_config = config;
    atlas_config_clamp(&safe_common, &safe_config, sizeof(safe_config));

    session = (screensave_saver_session *)calloc(1U, sizeof(*session));
    if (session == NULL) {
        return 0;
    }

    atlas_initialize_session(session, &safe_config, safe_common.detail_level, environment);
    if (session->field_size.width <= 0 || session->field_size.height <= 0 || session->escape_values == NULL) {
        atlas_destroy_session(session);
        return 0;
    }

    *session_out = session;
    return 1;
}

void atlas_destroy_session(screensave_saver_session *session)
{
    if (session == NULL) {
        return;
    }

    free(session->escape_values);
    screensave_visual_buffer_dispose(&session->visual_buffer);
    free(session);
}

void atlas_resize_session(
    screensave_saver_session *session,
    const screensave_saver_environment *environment
)
{
    if (session == NULL || environment == NULL) {
        return;
    }

    session->drawable_size = environment->drawable_size;
    if (!atlas_prepare_buffers(session)) {
        return;
    }

    atlas_refresh_view(session);
    atlas_restart_refinement(session);
}

void atlas_step_session(
    screensave_saver_session *session,
    const screensave_saver_environment *environment
)
{
    unsigned long delta;
    unsigned long interval;

    if (session == NULL || environment == NULL) {
        return;
    }

    delta = environment->clock.delta_millis != 0UL ? environment->clock.delta_millis : atlas_step_interval(session);
    interval = atlas_step_interval(session);
    session->step_accumulator += delta;
    while (session->step_accumulator >= interval) {
        session->step_accumulator -= interval;
        atlas_run_tick(session);
    }
}
