#include <stdlib.h>

#include "mechanize_internal.h"

static const screensave_theme_descriptor *mechanize_resolve_theme(
    const screensave_config_binding *binding
)
{
    if (binding != NULL && binding->common_config != NULL) {
        return mechanize_find_theme_descriptor(binding->common_config->theme_key);
    }

    return mechanize_find_theme_descriptor(MECHANIZE_DEFAULT_THEME_KEY);
}

static const mechanize_config *mechanize_resolve_config(
    const screensave_config_binding *binding
)
{
    if (
        binding == NULL ||
        binding->product_config == NULL ||
        binding->product_config_size != sizeof(mechanize_config)
    ) {
        return NULL;
    }

    return (const mechanize_config *)binding->product_config;
}

static unsigned long mechanize_step_interval(const screensave_saver_session *session)
{
    unsigned long interval;

    if (session == NULL) {
        return 70UL;
    }

    switch (session->config.speed_mode) {
    case MECHANIZE_SPEED_PATIENT:
        interval = 95UL;
        break;

    case MECHANIZE_SPEED_BRISK:
        interval = 36UL;
        break;

    case MECHANIZE_SPEED_STANDARD:
    default:
        interval = 62UL;
        break;
    }

    if (session->preview_mode && interval < 72UL) {
        interval = 72UL;
    }

    return interval;
}

static unsigned int mechanize_density_count(
    int density_mode,
    unsigned int sparse_value,
    unsigned int standard_value,
    unsigned int dense_value
)
{
    switch (density_mode) {
    case MECHANIZE_DENSITY_SPARSE:
        return sparse_value;

    case MECHANIZE_DENSITY_DENSE:
        return dense_value;

    case MECHANIZE_DENSITY_STANDARD:
    default:
        return standard_value;
    }
}

static int mechanize_min_int(int a, int b)
{
    return a < b ? a : b;
}

static void mechanize_clear_layout(screensave_saver_session *session)
{
    unsigned int index;

    if (session == NULL) {
        return;
    }

    for (index = 0U; index < MECHANIZE_MAX_GEARS; ++index) {
        session->gears[index].active = 0;
        session->gears[index].phase = 0U;
    }
    for (index = 0U; index < MECHANIZE_MAX_DIALS; ++index) {
        session->dials[index].active = 0;
        session->dials[index].angle = 0U;
        session->dials[index].target_angle = 0U;
    }
    for (index = 0U; index < MECHANIZE_MAX_FOLLOWERS; ++index) {
        session->follower_heights[index] = 0;
        session->follower_targets[index] = 0;
    }
}

static void mechanize_build_gear_train(screensave_saver_session *session)
{
    unsigned int count;
    unsigned int index;
    int spacing;
    int baseline;
    int radius;

    if (session == NULL) {
        return;
    }

    count = mechanize_density_count(session->config.density_mode, 3U, 4U, 5U);
    if (session->preview_mode && count > 3U) {
        count = 3U;
    }

    radius = mechanize_min_int(session->drawable_size.width / (int)(count * 3U), session->drawable_size.height / 5);
    if (radius < 14) {
        radius = 14;
    }

    spacing = (session->drawable_size.width - (radius * 2)) / (int)(count + 1U);
    baseline = session->drawable_size.height / 2;
    if ((session->layout_variant & 1U) != 0U) {
        baseline -= radius / 3;
    } else if ((session->layout_variant % 3U) == 2U) {
        baseline += radius / 4;
    }

    for (index = 0U; index < count; ++index) {
        session->gears[index].active = 1;
        session->gears[index].x = spacing * (int)(index + 1U);
        session->gears[index].y = baseline + ((index & 1U) ? radius / 3 : -(radius / 3));
        session->gears[index].radius = radius - ((int)index % 2) - (int)(session->layout_variant % 2U);
        session->gears[index].direction = (index & 1U) ? -1 : 1;
        session->gears[index].phase = (unsigned int)mechanize_rng_range(&session->rng, 256UL);
    }
}

static void mechanize_build_cam_bank(screensave_saver_session *session)
{
    unsigned int count;
    unsigned int index;
    int spacing;
    int y;
    int radius;

    if (session == NULL) {
        return;
    }

    count = mechanize_density_count(session->config.density_mode, 3U, 4U, 5U);
    if (session->preview_mode && count > 3U) {
        count = 3U;
    }

    radius = mechanize_min_int(session->drawable_size.width / (int)(count * 4U), session->drawable_size.height / 6);
    if (radius < 12) {
        radius = 12;
    }
    spacing = session->drawable_size.width / (int)(count + 1U);
    y = (session->drawable_size.height * 2) / 3;
    if ((session->layout_variant % 3U) == 1U) {
        y -= radius / 3;
    } else if ((session->layout_variant % 3U) == 2U) {
        y += radius / 4;
    }

    for (index = 0U; index < count; ++index) {
        session->gears[index].active = 1;
        session->gears[index].x = spacing * (int)(index + 1U);
        session->gears[index].y = y;
        session->gears[index].radius = radius;
        session->gears[index].direction = 1;
        session->gears[index].phase = (unsigned int)mechanize_rng_range(&session->rng, 256UL);
        session->follower_heights[index] = radius + (int)((session->layout_variant + index) % 3U);
        session->follower_targets[index] = radius + (int)((session->layout_variant + index) % 4U);
    }
}

static void mechanize_build_dial_assembly(screensave_saver_session *session)
{
    unsigned int count;
    unsigned int index;
    int columns;
    int rows;
    int spacing_x;
    int spacing_y;
    int radius;

    if (session == NULL) {
        return;
    }

    count = mechanize_density_count(session->config.density_mode, 2U, 3U, 4U);
    if (session->preview_mode && count > 2U) {
        count = 2U;
    }

    columns = 2;
    rows = 2;
    radius = mechanize_min_int(session->drawable_size.width / 7, session->drawable_size.height / 5);
    if (radius < 16) {
        radius = 16;
    }
    spacing_x = session->drawable_size.width / 3;
    spacing_y = session->drawable_size.height / 3;
    if ((session->layout_variant & 1U) != 0U) {
        spacing_y -= radius / 5;
    }

    for (index = 0U; index < count; ++index) {
        session->dials[index].active = 1;
        session->dials[index].x = spacing_x * (int)((index % columns) + 1U);
        session->dials[index].y = spacing_y * (int)((index / columns) + 1U);
        session->dials[index].radius = radius;
        session->dials[index].angle = (unsigned int)mechanize_rng_range(&session->rng, 256UL);
        session->dials[index].target_angle =
            (unsigned int)((mechanize_rng_range(&session->rng, 256UL) + session->layout_variant * 24U) & 255U);
    }

    for (index = 0U; index < count; ++index) {
        session->gears[index].active = 1;
        session->gears[index].x = spacing_x * (int)(index + 1U) - spacing_x / 2;
        session->gears[index].y = session->drawable_size.height - radius * 2;
        session->gears[index].radius = radius / 2;
        session->gears[index].direction = (index & 1U) ? -1 : 1;
        session->gears[index].phase = (unsigned int)mechanize_rng_range(&session->rng, 256UL);
    }
}

static void mechanize_configure_layout(screensave_saver_session *session)
{
    if (session == NULL) {
        return;
    }

    mechanize_clear_layout(session);
    switch (session->config.scene_mode) {
    case MECHANIZE_SCENE_CAM_BANK:
        mechanize_build_cam_bank(session);
        break;

    case MECHANIZE_SCENE_DIAL_ASSEMBLY:
        mechanize_build_dial_assembly(session);
        break;

    case MECHANIZE_SCENE_GEAR_TRAIN:
    default:
        mechanize_build_gear_train(session);
        break;
    }
}

static void mechanize_initialize_session(
    screensave_saver_session *session,
    const mechanize_config *config,
    const screensave_saver_environment *environment
)
{
    unsigned long seed;

    if (session == NULL || config == NULL || environment == NULL) {
        return;
    }

    session->config = *config;
    session->theme = mechanize_resolve_theme(environment->config_binding);
    session->drawable_size = environment->drawable_size;
    session->preview_mode = environment->mode == SCREENSAVE_SESSION_MODE_PREVIEW;
    seed = environment->seed.stream_seed != 0UL ? environment->seed.stream_seed : 0x24681357UL;
    mechanize_rng_seed(&session->rng, seed);
    session->step_accumulator = 0UL;
    session->event_accumulator = 0UL;
    session->master_phase = 0U;
    session->event_pulse = 0U;
    session->event_cycle = 0U;
    session->layout_variant = (unsigned int)mechanize_rng_range(&session->rng, 3UL);
    session->counter_window = 0U;
    mechanize_configure_layout(session);
}

static unsigned int mechanize_triangle_wave(unsigned int phase)
{
    unsigned int local_phase;

    local_phase = phase & 255U;
    if (local_phase < 128U) {
        return local_phase * 2U;
    }

    return (255U - local_phase) * 2U;
}

static void mechanize_update_gears(screensave_saver_session *session)
{
    unsigned int index;
    unsigned int phase_step;

    if (session == NULL) {
        return;
    }

    phase_step = (unsigned int)(session->config.speed_mode + 1);
    for (index = 0U; index < MECHANIZE_MAX_GEARS; ++index) {
        if (!session->gears[index].active) {
            continue;
        }

        if (session->gears[index].direction >= 0) {
            session->gears[index].phase = (session->gears[index].phase + phase_step + index) & 255U;
        } else {
            session->gears[index].phase = (session->gears[index].phase - (phase_step + index)) & 255U;
        }
    }
}

static void mechanize_update_cam_followers(screensave_saver_session *session)
{
    unsigned int index;

    if (session == NULL) {
        return;
    }

    for (index = 0U; index < MECHANIZE_MAX_FOLLOWERS; ++index) {
        if (!session->gears[index].active) {
            continue;
        }

        session->follower_targets[index] = (int)(mechanize_triangle_wave(session->gears[index].phase) / 12U);
        if (session->follower_heights[index] < session->follower_targets[index]) {
            session->follower_heights[index] += 1;
        } else if (session->follower_heights[index] > session->follower_targets[index]) {
            session->follower_heights[index] -= 1;
        }
    }
}

static void mechanize_update_dials(screensave_saver_session *session)
{
    unsigned int index;
    int delta;

    if (session == NULL) {
        return;
    }

    for (index = 0U; index < MECHANIZE_MAX_DIALS; ++index) {
        if (!session->dials[index].active) {
            continue;
        }

        delta = (int)session->dials[index].target_angle - (int)session->dials[index].angle;
        if (delta > 128) {
            delta -= 256;
        } else if (delta < -128) {
            delta += 256;
        }

        if (delta > 0) {
            session->dials[index].angle = (session->dials[index].angle + 1U + (unsigned int)(delta > 12)) & 255U;
        } else if (delta < 0) {
            session->dials[index].angle = (session->dials[index].angle - (1U + (unsigned int)(delta < -12))) & 255U;
        } else {
            session->dials[index].target_angle = (unsigned int)mechanize_rng_range(&session->rng, 256UL);
        }
    }
}

static void mechanize_run_step(screensave_saver_session *session)
{
    if (session == NULL) {
        return;
    }

    session->master_phase = (session->master_phase + (unsigned int)(session->config.speed_mode + 1)) & 255U;
    session->counter_window = (session->counter_window + 1U) % 1000U;
    mechanize_update_gears(session);
    if (session->config.scene_mode == MECHANIZE_SCENE_CAM_BANK) {
        mechanize_update_cam_followers(session);
    }
    if (session->config.scene_mode == MECHANIZE_SCENE_DIAL_ASSEMBLY) {
        mechanize_update_dials(session);
    }

    if (session->event_pulse > 0U) {
        session->event_pulse -= 1U;
    }
}

void mechanize_rng_seed(mechanize_rng_state *state, unsigned long seed)
{
    if (state == NULL) {
        return;
    }

    state->state = seed != 0UL ? seed : 0x9151C0DEUL;
}

unsigned long mechanize_rng_next(mechanize_rng_state *state)
{
    if (state == NULL) {
        return 0UL;
    }

    state->state = state->state * 1664525UL + 1013904223UL;
    return state->state;
}

unsigned long mechanize_rng_range(mechanize_rng_state *state, unsigned long upper_bound)
{
    if (upper_bound == 0UL) {
        return 0UL;
    }

    return mechanize_rng_next(state) % upper_bound;
}

int mechanize_create_session(
    const screensave_saver_module *module,
    screensave_saver_session **session_out,
    const screensave_saver_environment *environment
)
{
    screensave_saver_session *session;
    mechanize_config config;
    const mechanize_config *bound_config;
    screensave_common_config safe_common;
    mechanize_config safe_config;

    (void)module;

    if (session_out == NULL || environment == NULL) {
        return 0;
    }

    mechanize_config_set_defaults(&safe_common, &config, sizeof(config));
    bound_config = mechanize_resolve_config(environment->config_binding);
    if (bound_config != NULL) {
        config = *bound_config;
    }

    if (environment->config_binding != NULL && environment->config_binding->common_config != NULL) {
        safe_common = *environment->config_binding->common_config;
    } else {
        screensave_common_config_set_defaults(&safe_common);
    }
    safe_config = config;
    mechanize_config_clamp(&safe_common, &safe_config, sizeof(safe_config));

    session = (screensave_saver_session *)calloc(1U, sizeof(*session));
    if (session == NULL) {
        return 0;
    }

    mechanize_initialize_session(session, &safe_config, environment);
    *session_out = session;
    return 1;
}

void mechanize_destroy_session(screensave_saver_session *session)
{
    if (session == NULL) {
        return;
    }

    free(session);
}

void mechanize_resize_session(
    screensave_saver_session *session,
    const screensave_saver_environment *environment
)
{
    if (session == NULL || environment == NULL) {
        return;
    }

    session->drawable_size = environment->drawable_size;
    session->preview_mode = environment->mode == SCREENSAVE_SESSION_MODE_PREVIEW;
    session->theme = mechanize_resolve_theme(environment->config_binding);
    mechanize_configure_layout(session);
}

void mechanize_step_session(
    screensave_saver_session *session,
    const screensave_saver_environment *environment
)
{
    unsigned long interval;

    if (session == NULL || environment == NULL) {
        return;
    }

    session->step_accumulator += environment->clock.delta_millis;
    session->event_accumulator += environment->clock.delta_millis;
    interval = mechanize_step_interval(session);

    while (session->step_accumulator >= interval) {
        session->step_accumulator -= interval;
        mechanize_run_step(session);
    }

    if (session->event_accumulator >= (session->preview_mode ? 2600UL : 2200UL)) {
        session->event_accumulator = 0UL;
        session->event_pulse = 18U;
        session->event_cycle += 1U;
        if ((session->event_cycle % 2U) == 0U) {
            session->layout_variant = (session->layout_variant + 1U) % 3U;
            mechanize_configure_layout(session);
        }
        if (session->config.scene_mode == MECHANIZE_SCENE_DIAL_ASSEMBLY) {
            unsigned int index;

            for (index = 0U; index < MECHANIZE_MAX_DIALS; ++index) {
                if (session->dials[index].active) {
                    session->dials[index].target_angle = (unsigned int)mechanize_rng_range(&session->rng, 256UL);
                }
            }
        } else if (session->config.scene_mode == MECHANIZE_SCENE_CAM_BANK) {
            unsigned int index;

            for (index = 0U; index < MECHANIZE_MAX_FOLLOWERS; ++index) {
                session->follower_targets[index] = 10 + (int)mechanize_rng_range(&session->rng, 28UL);
            }
        }
    }
}
