#include <stdlib.h>

#include "city_internal.h"

static const screensave_theme_descriptor *city_resolve_theme(const screensave_config_binding *binding)
{
    if (binding != NULL && binding->common_config != NULL) {
        return city_find_theme_descriptor(binding->common_config->theme_key);
    }

    return city_find_theme_descriptor(CITY_DEFAULT_THEME_KEY);
}

static const city_config *city_resolve_config(const screensave_config_binding *binding)
{
    if (binding == NULL || binding->product_config == NULL || binding->product_config_size != sizeof(city_config)) {
        return NULL;
    }

    return (const city_config *)binding->product_config;
}

static unsigned long city_step_interval(const screensave_saver_session *session)
{
    if (session == NULL) {
        return 72UL;
    }

    switch (session->config.speed_mode) {
    case CITY_SPEED_STILL:
        return 104UL;
    case CITY_SPEED_BRISK:
        return 40UL;
    case CITY_SPEED_STANDARD:
    default:
        return 64UL;
    }
}

static unsigned long city_event_interval(const screensave_saver_session *session)
{
    if (session == NULL) {
        return 1400UL;
    }

    switch (session->config.speed_mode) {
    case CITY_SPEED_STILL:
        return 2400UL;
    case CITY_SPEED_BRISK:
        return 1000UL;
    case CITY_SPEED_STANDARD:
    default:
        return 1600UL;
    }
}

static int city_scene_span(const screensave_saver_session *session)
{
    int width;

    if (session == NULL || session->drawable_size.width <= 0) {
        return 512;
    }

    width = session->drawable_size.width;
    return width * 2 + 192;
}

static int city_horizon_y(const screensave_saver_session *session)
{
    int height;
    int horizon;

    if (session == NULL || session->drawable_size.height <= 0) {
        return 1;
    }

    height = session->drawable_size.height;
    switch (session->config.scene_mode) {
    case CITY_SCENE_HARBOR:
        horizon = (height * 3) / 5;
        break;
    case CITY_SCENE_WINDOW_RIVER:
        horizon = (height * 11) / 20;
        break;
    case CITY_SCENE_SKYLINE:
    default:
        horizon = (height * 2) / 3;
        break;
    }

    if (horizon < 24) {
        horizon = 24;
    }
    if (horizon > height - 18) {
        horizon = height - 18;
    }

    return horizon;
}

static unsigned int city_structure_target(const screensave_saver_session *session)
{
    unsigned int count;

    if (session == NULL) {
        return 10U;
    }

    switch (session->config.density_mode) {
    case CITY_DENSITY_SPARSE:
        count = 9U;
        break;
    case CITY_DENSITY_DENSE:
        count = 16U;
        break;
    case CITY_DENSITY_STANDARD:
    default:
        count = 12U;
        break;
    }

    if (session->config.scene_mode == CITY_SCENE_SKYLINE) {
        count += 1U;
    }
    if (session->preview_mode && count > 10U) {
        count = 10U;
    }
    if (count > CITY_MAX_STRUCTURES) {
        count = CITY_MAX_STRUCTURES;
    }

    return count;
}

static unsigned int city_light_target(const screensave_saver_session *session)
{
    unsigned int count;

    if (session == NULL) {
        return 12U;
    }

    switch (session->config.density_mode) {
    case CITY_DENSITY_SPARSE:
        count = 10U;
        break;
    case CITY_DENSITY_DENSE:
        count = 20U;
        break;
    case CITY_DENSITY_STANDARD:
    default:
        count = 15U;
        break;
    }

    if (session->config.scene_mode == CITY_SCENE_HARBOR) {
        count += 2U;
    }
    if (session->preview_mode && count > 14U) {
        count = 14U;
    }
    if (count > CITY_MAX_LIGHTS) {
        count = CITY_MAX_LIGHTS;
    }

    return count;
}

static unsigned int city_fog_target(const screensave_saver_session *session)
{
    unsigned int count;

    if (session == NULL) {
        return 3U;
    }

    switch (session->config.density_mode) {
    case CITY_DENSITY_SPARSE:
        count = 2U;
        break;
    case CITY_DENSITY_DENSE:
        count = 4U;
        break;
    case CITY_DENSITY_STANDARD:
    default:
        count = 3U;
        break;
    }

    if (session->preview_mode && count > 3U) {
        count = 3U;
    }
    if (count > CITY_MAX_FOG_BANDS) {
        count = CITY_MAX_FOG_BANDS;
    }

    return count;
}

static int city_speed_step(const screensave_saver_session *session)
{
    if (session == NULL) {
        return 2;
    }

    switch (session->config.speed_mode) {
    case CITY_SPEED_STILL:
        return 1;
    case CITY_SPEED_BRISK:
        return 4;
    case CITY_SPEED_STANDARD:
    default:
        return 2;
    }
}

static void city_seed_structure(
    screensave_saver_session *session,
    city_structure *structure,
    unsigned int index,
    int x_cursor
)
{
    int width_min;
    int width_max;
    int height_min;
    int height_max;
    int horizon;

    if (session == NULL || structure == NULL) {
        return;
    }

    (void)index;
    horizon = city_horizon_y(session);
    width_min = 28;
    width_max = 64;
    height_min = 32;
    height_max = 104;

    if (session->config.density_mode == CITY_DENSITY_SPARSE) {
        width_min = 24;
        width_max = 52;
        height_min = 28;
        height_max = 92;
    } else if (session->config.density_mode == CITY_DENSITY_DENSE) {
        width_min = 32;
        width_max = 74;
        height_min = 40;
        height_max = 136;
    }

    structure->world_x = x_cursor;
    structure->width = width_min + (int)city_rng_range(&session->rng, (unsigned long)(width_max - width_min + 1));
    structure->height = height_min + (int)city_rng_range(&session->rng, (unsigned long)(height_max - height_min + 1));
    if (structure->height > horizon - 8) {
        structure->height = horizon - 8;
    }
    if (structure->height < 16) {
        structure->height = 16;
    }
    structure->depth = (int)city_rng_range(&session->rng, 3UL);
    structure->roof_style = (int)city_rng_range(&session->rng, 3UL);
    structure->window_rows = 2 + (int)city_rng_range(&session->rng, 4UL);
    structure->window_columns = 2 + (int)city_rng_range(&session->rng, 3UL);
    if (structure->window_columns < 2) {
        structure->window_columns = 2;
    }
    if (structure->window_rows < 2) {
        structure->window_rows = 2;
    }
    structure->light_bias = 92 + (int)city_rng_range(&session->rng, 92UL);
    if (session->config.scene_mode == CITY_SCENE_HARBOR) {
        structure->light_bias += 18;
    }
    structure->drift = 1 + (int)city_rng_range(&session->rng, 3UL);
    structure->seed_offset = (unsigned int)city_rng_range(&session->rng, 255UL);
}

static void city_seed_light(screensave_saver_session *session, city_light *light, unsigned int index)
{
    unsigned int structure_index;
    int horizon;
    int span;
    const city_structure *structure;

    if (session == NULL || light == NULL || session->structure_count == 0U) {
        return;
    }

    horizon = city_horizon_y(session);
    span = city_scene_span(session);
    structure_index = index % session->structure_count;
    structure = &session->structures[structure_index];
    light->world_x = structure->world_x + (structure->width / 2) + (int)city_rng_range(&session->rng, 32UL) - 16;
    if (light->world_x < -span) {
        light->world_x += span;
    }
    light->world_y = horizon - structure->height + 16 + (int)city_rng_range(&session->rng, (unsigned long)(structure->height / 2 + 1));
    if (light->world_y < 8) {
        light->world_y = 8;
    }
    light->width = 2 + (int)city_rng_range(&session->rng, 2UL);
    light->height = 5 + (int)city_rng_range(&session->rng, 7UL);
    light->brightness = 120 + (int)city_rng_range(&session->rng, 96UL);
    light->depth = structure->depth;
    light->phase = (unsigned int)city_rng_range(&session->rng, 255UL);
}

static void city_seed_fog(screensave_saver_session *session, city_fog_band *fog_band, unsigned int index)
{
    int horizon;
    int height;

    if (session == NULL || fog_band == NULL) {
        return;
    }

    horizon = city_horizon_y(session);
    height = 14 + (int)city_rng_range(&session->rng, 12UL);
    fog_band->y = horizon - height - (int)(index * 5U);
    fog_band->height = height;
    fog_band->drift = (int)city_rng_range(&session->rng, 3UL) - 1;
    fog_band->opacity = 36 + (int)city_rng_range(&session->rng, 64UL);
}

static void city_initialize_scene(screensave_saver_session *session)
{
    unsigned int index;
    unsigned int structure_count;
    unsigned int light_count;
    unsigned int fog_count;
    int span;
    int x_cursor;
    int gap_min;
    int gap_max;

    if (session == NULL) {
        return;
    }

    structure_count = city_structure_target(session);
    light_count = city_light_target(session);
    fog_count = city_fog_target(session);
    span = city_scene_span(session);
    gap_min = 18;
    gap_max = 64;
    if (session->config.density_mode == CITY_DENSITY_SPARSE) {
        gap_min = 32;
        gap_max = 72;
    } else if (session->config.density_mode == CITY_DENSITY_DENSE) {
        gap_min = 12;
        gap_max = 44;
    }

    session->structure_count = structure_count;
    session->light_count = light_count;
    session->fog_count = fog_count;
    session->camera_phase = (unsigned int)city_rng_range(&session->rng, 255UL);
    session->light_phase = (unsigned int)city_rng_range(&session->rng, 255UL);
    session->pulse_phase = 0U;
    session->route_offset = (int)city_rng_range(&session->rng, (unsigned long)span);

    x_cursor = -span / 6;
    for (index = 0U; index < structure_count; ++index) {
        x_cursor += gap_min + (int)city_rng_range(&session->rng, (unsigned long)(gap_max - gap_min + 1));
        city_seed_structure(session, &session->structures[index], index, x_cursor);
    }
    for (index = 0U; index < light_count; ++index) {
        city_seed_light(session, &session->lights[index], index);
    }
    for (index = 0U; index < fog_count; ++index) {
        city_seed_fog(session, &session->fog_bands[index], index);
    }
}

static void city_refresh_scene(screensave_saver_session *session)
{
    unsigned int index;

    if (session == NULL) {
        return;
    }

    for (index = 0U; index < session->light_count; ++index) {
        if ((city_rng_next(&session->rng) & 3UL) == 0UL) {
            session->lights[index].brightness = 112 + (int)city_rng_range(&session->rng, 104UL);
            session->lights[index].phase = (unsigned int)city_rng_range(&session->rng, 255UL);
        }
    }
    for (index = 0U; index < session->structure_count; ++index) {
        if ((city_rng_next(&session->rng) & 7UL) == 0UL) {
            session->structures[index].light_bias = 88 + (int)city_rng_range(&session->rng, 112UL);
        }
    }
    for (index = 0U; index < session->fog_count; ++index) {
        session->fog_bands[index].drift = (int)city_rng_range(&session->rng, 3UL) - 1;
        session->fog_bands[index].opacity = 32 + (int)city_rng_range(&session->rng, 72UL);
    }
    if (session->light_count > 0U) {
        city_seed_light(
            session,
            &session->lights[city_rng_range(&session->rng, (unsigned long)session->light_count)],
            (unsigned int)city_rng_range(&session->rng, (unsigned long)session->light_count)
        );
    }
    if (session->fog_count > 0U) {
        city_seed_fog(
            session,
            &session->fog_bands[city_rng_range(&session->rng, (unsigned long)session->fog_count)],
            (unsigned int)city_rng_range(&session->rng, (unsigned long)session->fog_count)
        );
    }
    session->pulse_phase = (session->pulse_phase + 1U) & 255U;
}

static void city_initialize_session(
    screensave_saver_session *session,
    const city_config *config,
    const screensave_saver_environment *environment
)
{
    unsigned long seed;

    if (session == NULL || config == NULL || environment == NULL) {
        return;
    }

    session->config = *config;
    session->theme = city_resolve_theme(environment->config_binding);
    session->drawable_size = environment->drawable_size;
    session->preview_mode = environment->mode == SCREENSAVE_SESSION_MODE_PREVIEW;
    seed = environment->seed.stream_seed != 0UL ? environment->seed.stream_seed : 0x43697479UL;
    city_rng_seed(&session->rng, seed);
    session->step_accumulator = 0UL;
    session->event_accumulator = 0UL;
    session->route_offset = 0;
    city_initialize_scene(session);
}

static void city_run_step(screensave_saver_session *session)
{
    int span;
    int speed_step;
    unsigned int index;

    if (session == NULL) {
        return;
    }

    span = city_scene_span(session);
    speed_step = city_speed_step(session);
    session->camera_phase = (session->camera_phase + 1U + (unsigned int)session->config.speed_mode) & 255U;
    session->light_phase = (session->light_phase + 1U + (unsigned int)session->config.density_mode) & 255U;
    session->pulse_phase = (session->pulse_phase + 1U) & 255U;
    session->route_offset += speed_step;
    if (session->route_offset >= span) {
        session->route_offset -= span;
    }
    for (index = 0U; index < session->light_count; ++index) {
        session->lights[index].phase = (session->lights[index].phase + 1U + (unsigned int)session->lights[index].depth) & 255U;
    }
}

void city_rng_seed(city_rng_state *state, unsigned long seed)
{
    if (state == NULL) {
        return;
    }

    state->state = seed != 0UL ? seed : 0x0C1D0001UL;
}

unsigned long city_rng_next(city_rng_state *state)
{
    if (state == NULL) {
        return 0UL;
    }

    state->state = state->state * 1664525UL + 1013904223UL;
    return state->state;
}

unsigned long city_rng_range(city_rng_state *state, unsigned long upper_bound)
{
    if (upper_bound == 0UL) {
        return 0UL;
    }

    return city_rng_next(state) % upper_bound;
}

int city_create_session(
    const screensave_saver_module *module,
    screensave_saver_session **session_out,
    const screensave_saver_environment *environment
)
{
    screensave_saver_session *session;
    city_config config;
    const city_config *bound_config;
    screensave_common_config safe_common;
    city_config safe_config;

    (void)module;

    if (session_out == NULL || environment == NULL) {
        return 0;
    }

    city_config_set_defaults(&safe_common, &config, sizeof(config));
    bound_config = city_resolve_config(environment->config_binding);
    if (bound_config != NULL) {
        config = *bound_config;
    }

    if (environment->config_binding != NULL && environment->config_binding->common_config != NULL) {
        safe_common = *environment->config_binding->common_config;
    } else {
        screensave_common_config_set_defaults(&safe_common);
    }
    safe_config = config;
    city_config_clamp(&safe_common, &safe_config, sizeof(safe_config));

    session = (screensave_saver_session *)calloc(1U, sizeof(*session));
    if (session == NULL) {
        return 0;
    }

    city_initialize_session(session, &safe_config, environment);
    *session_out = session;
    return 1;
}

void city_destroy_session(screensave_saver_session *session)
{
    if (session == NULL) {
        return;
    }

    free(session);
}

void city_resize_session(screensave_saver_session *session, const screensave_saver_environment *environment)
{
    if (session == NULL || environment == NULL) {
        return;
    }

    session->drawable_size = environment->drawable_size;
    session->preview_mode = environment->mode == SCREENSAVE_SESSION_MODE_PREVIEW;
    session->theme = city_resolve_theme(environment->config_binding);
    city_initialize_scene(session);
}

void city_step_session(
    screensave_saver_session *session,
    const screensave_saver_environment *environment
)
{
    unsigned long delta_millis;
    unsigned long step_interval;
    unsigned long event_interval;

    if (session == NULL || environment == NULL) {
        return;
    }

    delta_millis = environment->clock.delta_millis;
    step_interval = city_step_interval(session);
    event_interval = city_event_interval(session);
    session->step_accumulator += delta_millis;
    session->event_accumulator += delta_millis;
    while (session->step_accumulator >= step_interval) {
        session->step_accumulator -= step_interval;
        city_run_step(session);
    }
    if (session->event_accumulator >= event_interval) {
        session->event_accumulator -= event_interval;
        city_refresh_scene(session);
    }
}
