#include <stdlib.h>

#include "signals_internal.h"

static const screensave_theme_descriptor *signals_resolve_theme(const screensave_config_binding *binding)
{
    if (binding != NULL && binding->common_config != NULL) {
        return signals_find_theme_descriptor(binding->common_config->theme_key);
    }

    return signals_find_theme_descriptor(SIGNALS_DEFAULT_THEME_KEY);
}

static const signals_config *signals_resolve_config(const screensave_config_binding *binding)
{
    if (binding == NULL || binding->product_config == NULL || binding->product_config_size != sizeof(signals_config)) {
        return NULL;
    }

    return (const signals_config *)binding->product_config;
}

static unsigned long signals_step_interval(const screensave_saver_session *session)
{
    unsigned long interval;

    if (session == NULL) {
        return 80UL;
    }

    switch (session->config.activity_mode) {
    case SIGNALS_ACTIVITY_QUIET:
        interval = 95UL;
        break;

    case SIGNALS_ACTIVITY_BUSY:
        interval = 40UL;
        break;

    case SIGNALS_ACTIVITY_STANDARD:
    default:
        interval = 65UL;
        break;
    }

    if (session->preview_mode && interval < 86UL) {
        interval = 86UL;
    }

    return interval;
}

static unsigned long signals_event_interval(const screensave_saver_session *session)
{
    unsigned long interval;

    interval = 1800UL;
    if (session == NULL) {
        return interval;
    }

    switch (session->config.activity_mode) {
    case SIGNALS_ACTIVITY_QUIET:
        interval = 2600UL;
        break;

    case SIGNALS_ACTIVITY_BUSY:
        interval = 1400UL;
        break;

    case SIGNALS_ACTIVITY_STANDARD:
    default:
        interval = 1800UL;
        break;
    }

    if (session->preview_mode && interval < 2200UL) {
        interval = 2200UL;
    }

    return interval;
}

static void signals_pick_focus_panel(screensave_saver_session *session)
{
    unsigned long roll;

    if (session == NULL) {
        return;
    }

    roll = signals_rng_range(&session->rng, 100UL);
    switch (session->config.layout_mode) {
    case SIGNALS_LAYOUT_SPECTRUM:
        session->focus_panel = roll < 52UL ? 0U : (roll < 76UL ? 1U : 2U);
        break;

    case SIGNALS_LAYOUT_TELEMETRY:
        session->focus_panel = roll < 30UL ? 0U : (roll < 66UL ? 1U : 2U);
        break;

    case SIGNALS_LAYOUT_OPERATIONS:
    default:
        session->focus_panel = roll < 40UL ? 0U : (roll < 70UL ? 1U : 2U);
        break;
    }
}

static void signals_shift_scope(int *values, unsigned int count, int next_value)
{
    unsigned int index;

    if (values == NULL || count == 0U) {
        return;
    }

    for (index = 1U; index < count; ++index) {
        values[index - 1U] = values[index];
    }
    values[count - 1U] = next_value;
}

static int signals_clamp_percent(int value)
{
    if (value < 0) {
        return 0;
    }
    if (value > 100) {
        return 100;
    }
    return value;
}

static void signals_seed_scope_history(screensave_saver_session *session)
{
    unsigned int scope_index;
    unsigned int sample_index;
    int value;

    if (session == NULL) {
        return;
    }

    for (scope_index = 0U; scope_index < SIGNALS_SCOPE_COUNT; ++scope_index) {
        value = 24 + (int)(scope_index * 14U);
        for (sample_index = 0U; sample_index < SIGNALS_SCOPE_SAMPLE_COUNT; ++sample_index) {
            value += (int)signals_rng_range(&session->rng, 7UL) - 3;
            if (value < 6) {
                value = 6;
            } else if (value > 94) {
                value = 94;
            }
            session->scope_values[scope_index][sample_index] = value;
        }
    }
}

static void signals_seed_meters(screensave_saver_session *session)
{
    unsigned int index;

    if (session == NULL) {
        return;
    }

    for (index = 0U; index < SIGNALS_METER_COUNT; ++index) {
        session->meter_levels[index] = 20 + (int)signals_rng_range(&session->rng, 40UL);
        session->meter_targets[index] = 20 + (int)signals_rng_range(&session->rng, 60UL);
    }
}

static void signals_seed_status(screensave_saver_session *session)
{
    unsigned int index;

    if (session == NULL) {
        return;
    }

    for (index = 0U; index < SIGNALS_STATUS_COUNT; ++index) {
        session->status_flags[index] = (signals_rng_range(&session->rng, 2UL) != 0UL);
    }
}

static void signals_initialize_session(
    screensave_saver_session *session,
    const signals_config *config,
    const screensave_saver_environment *environment
)
{
    unsigned int index;
    unsigned long seed;

    if (session == NULL || config == NULL || environment == NULL) {
        return;
    }

    session->config = *config;
    session->theme = signals_resolve_theme(environment->config_binding);
    session->drawable_size = environment->drawable_size;
    session->preview_mode = environment->mode == SCREENSAVE_SESSION_MODE_PREVIEW;
    seed = environment->seed.stream_seed != 0UL ? environment->seed.stream_seed : 0x13572468UL;
    signals_rng_seed(&session->rng, seed);

    session->sample_accumulator = 0UL;
    session->event_accumulator = 0UL;
    session->alert_millis = 0UL;
    session->focus_panel = 0U;
    session->cadence_phase = 0U;
    session->sweep_position = 0U;
    session->heartbeat_phase = 0U;
    for (index = 0U; index < 3U; ++index) {
        session->counters[index] = (unsigned int)signals_rng_range(&session->rng, 1000UL);
    }

    signals_seed_scope_history(session);
    signals_seed_meters(session);
    signals_seed_status(session);
    signals_pick_focus_panel(session);
}

static void signals_update_scope_values(screensave_saver_session *session)
{
    unsigned int scope_index;
    int previous;
    int next_value;
    int delta;

    if (session == NULL) {
        return;
    }

    for (scope_index = 0U; scope_index < SIGNALS_SCOPE_COUNT; ++scope_index) {
        previous = session->scope_values[scope_index][SIGNALS_SCOPE_SAMPLE_COUNT - 1U];
        delta = (int)signals_rng_range(&session->rng, 9UL) - 4;
        if (session->config.layout_mode == SIGNALS_LAYOUT_SPECTRUM && scope_index == 0U) {
            delta += 2;
        }
        if (session->focus_panel == scope_index) {
            delta += 2 + (int)(session->cadence_phase & 1U);
        }
        if (session->alert_millis > 0UL && scope_index == (session->heartbeat_phase % SIGNALS_SCOPE_COUNT)) {
            delta += 5;
        }
        next_value = previous + delta;
        if (session->config.activity_mode == SIGNALS_ACTIVITY_QUIET) {
            next_value = (next_value + previous) / 2;
        }
        next_value = signals_clamp_percent(next_value);
        if (next_value < 8) {
            next_value = 8;
        }
        signals_shift_scope(
            session->scope_values[scope_index],
            SIGNALS_SCOPE_SAMPLE_COUNT,
            next_value
        );
    }
}

static void signals_update_meters(screensave_saver_session *session)
{
    unsigned int index;
    int delta;

    if (session == NULL) {
        return;
    }

    for (index = 0U; index < SIGNALS_METER_COUNT; ++index) {
        if (session->meter_levels[index] < session->meter_targets[index]) {
            session->meter_levels[index] += 1 + (int)(session->heartbeat_phase & 1U);
        } else if (session->meter_levels[index] > session->meter_targets[index]) {
            session->meter_levels[index] -= 1;
        }

        delta = session->meter_targets[index] - session->meter_levels[index];
        if (delta < 0) {
            delta = -delta;
        }
        if (delta <= 2) {
            session->meter_targets[index] = 18 + (int)signals_rng_range(&session->rng, 78UL);
        }
        if ((unsigned int)(index / 2) == session->focus_panel) {
            session->meter_targets[index] += 6;
        }

        session->meter_levels[index] = signals_clamp_percent(session->meter_levels[index]);
        session->meter_targets[index] = signals_clamp_percent(session->meter_targets[index]);
    }
}

static void signals_update_status(screensave_saver_session *session)
{
    unsigned int index;

    if (session == NULL) {
        return;
    }

    for (index = 0U; index < SIGNALS_STATUS_COUNT; ++index) {
        session->status_flags[index] =
            ((session->counters[index % 3U] / (index + 2U)) + session->heartbeat_phase + index) % 2U;
    }

    session->status_flags[(session->focus_panel * 2U) % SIGNALS_STATUS_COUNT] = 1;
    session->status_flags[(session->focus_panel * 2U + 1U) % SIGNALS_STATUS_COUNT] =
        (session->cadence_phase & 1U) != 0U;

    if (session->alert_millis > 0UL) {
        session->status_flags[session->heartbeat_phase % SIGNALS_STATUS_COUNT] = 1;
    }
}

static void signals_run_step(screensave_saver_session *session)
{
    if (session == NULL) {
        return;
    }

    session->heartbeat_phase += 1U;
    session->cadence_phase = (session->cadence_phase + 1U) & 31U;
    session->sweep_position = (session->sweep_position + 3U + (unsigned int)session->config.activity_mode) % 100U;
    session->counters[0] = (session->counters[0] + 1U) % 10000U;
    session->counters[1] = (session->counters[1] + 3U) % 10000U;
    session->counters[2] = (session->counters[2] + 5U) % 10000U;
    signals_update_scope_values(session);
    signals_update_meters(session);
    signals_update_status(session);

    if (session->alert_millis > 0UL) {
        if (session->alert_millis >= signals_step_interval(session)) {
            session->alert_millis -= signals_step_interval(session);
        } else {
            session->alert_millis = 0UL;
        }
    }
}

void signals_rng_seed(signals_rng_state *state, unsigned long seed)
{
    if (state == NULL) {
        return;
    }

    state->state = seed != 0UL ? seed : 0xA511E9B3UL;
}

unsigned long signals_rng_next(signals_rng_state *state)
{
    if (state == NULL) {
        return 0UL;
    }

    state->state = state->state * 1664525UL + 1013904223UL;
    return state->state;
}

unsigned long signals_rng_range(signals_rng_state *state, unsigned long upper_bound)
{
    if (upper_bound == 0UL) {
        return 0UL;
    }

    return signals_rng_next(state) % upper_bound;
}

int signals_create_session(
    const screensave_saver_module *module,
    screensave_saver_session **session_out,
    const screensave_saver_environment *environment
)
{
    screensave_saver_session *session;
    signals_config config;
    const signals_config *bound_config;

    (void)module;

    if (session_out == NULL || environment == NULL) {
        return 0;
    }

    screensave_common_config safe_common;
    signals_config safe_config;

    signals_config_set_defaults(&safe_common, &config, sizeof(config));
    bound_config = signals_resolve_config(environment->config_binding);
    if (bound_config != NULL) {
        config = *bound_config;
    }

    if (environment->config_binding != NULL && environment->config_binding->common_config != NULL) {
        safe_common = *environment->config_binding->common_config;
    } else {
        screensave_common_config_set_defaults(&safe_common);
    }
    safe_config = config;
    signals_config_clamp(&safe_common, &safe_config, sizeof(safe_config));
    config = safe_config;

    session = (screensave_saver_session *)calloc(1U, sizeof(*session));
    if (session == NULL) {
        return 0;
    }

    signals_initialize_session(session, &config, environment);
    *session_out = session;
    return 1;
}

void signals_destroy_session(screensave_saver_session *session)
{
    if (session == NULL) {
        return;
    }

    free(session);
}

void signals_resize_session(
    screensave_saver_session *session,
    const screensave_saver_environment *environment
)
{
    if (session == NULL || environment == NULL) {
        return;
    }

    session->drawable_size = environment->drawable_size;
    session->preview_mode = environment->mode == SCREENSAVE_SESSION_MODE_PREVIEW;
    session->theme = signals_resolve_theme(environment->config_binding);
}

void signals_step_session(
    screensave_saver_session *session,
    const screensave_saver_environment *environment
)
{
    unsigned long interval;

    if (session == NULL || environment == NULL) {
        return;
    }

    session->sample_accumulator += environment->clock.delta_millis;
    session->event_accumulator += environment->clock.delta_millis;
    interval = signals_step_interval(session);

    while (session->sample_accumulator >= interval) {
        session->sample_accumulator -= interval;
        signals_run_step(session);
    }

    if (session->event_accumulator >= signals_event_interval(session)) {
        session->event_accumulator = 0UL;
        signals_pick_focus_panel(session);
        if (signals_rng_range(&session->rng, 4UL) == 0UL) {
            session->alert_millis = 600UL + signals_rng_range(&session->rng, 1200UL);
        }
        session->meter_targets[(session->focus_panel * 2U) % SIGNALS_METER_COUNT] =
            36 + (int)signals_rng_range(&session->rng, 56UL);
        session->meter_targets[(session->focus_panel * 2U + 1U) % SIGNALS_METER_COUNT] =
            26 + (int)signals_rng_range(&session->rng, 62UL);
        session->counters[session->focus_panel % 3U] =
            (session->counters[session->focus_panel % 3U] + 111U + (unsigned int)signals_rng_range(&session->rng, 777UL)) % 10000U;
    }
}
