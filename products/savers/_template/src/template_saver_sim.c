#include "template_saver_internal.h"

static void template_saver_copy_config_from_binding(
    const screensave_saver_environment *environment,
    screensave_common_config *common_config,
    template_saver_config *product_config
)
{
    const screensave_config_binding *binding;

    template_saver_config_set_defaults(common_config, product_config, sizeof(*product_config));
    if (environment == NULL) {
        return;
    }

    binding = environment->config_binding;
    if (
        binding == NULL ||
        binding->common_config == NULL ||
        binding->product_config == NULL ||
        binding->product_config_size != sizeof(*product_config)
    ) {
        return;
    }

    *common_config = *binding->common_config;
    *product_config = *(const template_saver_config *)binding->product_config;
    template_saver_config_clamp(common_config, product_config, sizeof(*product_config));
}

static int template_saver_spacing_margin(const template_saver_config *config)
{
    if (config == NULL) {
        return 24;
    }

    switch (config->spacing_mode) {
    case TEMPLATE_SAVER_SPACING_TIGHT:
        return 12;
    case TEMPLATE_SAVER_SPACING_WIDE:
        return 36;
    case TEMPLATE_SAVER_SPACING_BALANCED:
    default:
        return 24;
    }
}

static long template_saver_step_distance(
    const template_saver_config *config,
    unsigned long delta_millis,
    int preview_mode
)
{
    long base_speed;
    long distance;

    base_speed = 1L;
    if (config != NULL) {
        switch (config->motion_mode) {
        case TEMPLATE_SAVER_MOTION_FOCUS:
            base_speed = 3L;
            break;
        case TEMPLATE_SAVER_MOTION_CALM:
            base_speed = 1L;
            break;
        case TEMPLATE_SAVER_MOTION_DRIFT:
        default:
            base_speed = 2L;
            break;
        }
    }
    if (preview_mode) {
        base_speed = 1L;
    }

    distance = (long)((base_speed * (long)(delta_millis != 0UL ? delta_millis : 16UL) + 15L) / 16L);
    if (distance < 1L) {
        distance = 1L;
    }
    return distance;
}

static void template_saver_clamp_marker(screensave_saver_session *session)
{
    int margin;
    int min_x;
    int max_x;
    int min_y;
    int max_y;

    if (session == NULL) {
        return;
    }

    margin = template_saver_spacing_margin(&session->config);
    min_x = margin;
    min_y = margin;
    max_x = session->drawable_size.width - margin;
    max_y = session->drawable_size.height - margin;
    if (max_x < min_x) {
        max_x = min_x;
    }
    if (max_y < min_y) {
        max_y = min_y;
    }

    if (session->marker_x < min_x) {
        session->marker_x = min_x;
    } else if (session->marker_x > max_x) {
        session->marker_x = max_x;
    }

    if (session->marker_y < min_y) {
        session->marker_y = min_y;
    } else if (session->marker_y > max_y) {
        session->marker_y = max_y;
    }
}

int template_saver_create_session(
    const screensave_saver_module *module,
    screensave_saver_session **session_out,
    const screensave_saver_environment *environment
)
{
    screensave_saver_session *session;
    screensave_common_config common_config;
    template_saver_config product_config;

    (void)module;

    if (session_out == NULL || environment == NULL) {
        return 0;
    }

    session = (screensave_saver_session *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(*session));
    if (session == NULL) {
        return 0;
    }

    template_saver_copy_config_from_binding(environment, &common_config, &product_config);
    session->config = product_config;
    session->theme = template_saver_find_theme_descriptor(common_config.theme_key);
    if (session->theme == NULL) {
        session->theme = template_saver_find_theme_descriptor(TEMPLATE_SAVER_DEFAULT_THEME_KEY);
    }
    session->drawable_size = environment->drawable_size;
    if (session->drawable_size.width <= 0) {
        session->drawable_size.width = 1;
    }
    if (session->drawable_size.height <= 0) {
        session->drawable_size.height = 1;
    }
    session->preview_mode = environment->mode == SCREENSAVE_SESSION_MODE_PREVIEW;
    session->marker_x = session->drawable_size.width / 2;
    session->marker_y = session->drawable_size.height / 2;
    session->direction_x = 1;
    session->direction_y = 1;
    session->pulse_phase = environment->seed.base_seed;
    template_saver_clamp_marker(session);

    *session_out = session;
    return 1;
}

void template_saver_destroy_session(screensave_saver_session *session)
{
    if (session == NULL) {
        return;
    }

    HeapFree(GetProcessHeap(), 0U, session);
}

void template_saver_resize_session(
    screensave_saver_session *session,
    const screensave_saver_environment *environment
)
{
    if (session == NULL || environment == NULL) {
        return;
    }

    session->drawable_size = environment->drawable_size;
    if (session->drawable_size.width <= 0) {
        session->drawable_size.width = 1;
    }
    if (session->drawable_size.height <= 0) {
        session->drawable_size.height = 1;
    }
    template_saver_clamp_marker(session);
}

void template_saver_step_session(
    screensave_saver_session *session,
    const screensave_saver_environment *environment
)
{
    long distance;
    int margin;
    int min_x;
    int max_x;
    int min_y;
    int max_y;

    if (session == NULL || environment == NULL) {
        return;
    }

    distance = template_saver_step_distance(
        &session->config,
        environment->clock.delta_millis,
        session->preview_mode
    );
    session->pulse_phase += environment->clock.delta_millis != 0UL ? environment->clock.delta_millis : 16UL;

    session->marker_x += distance * (long)session->direction_x;
    session->marker_y += distance * (long)session->direction_y;

    margin = template_saver_spacing_margin(&session->config);
    min_x = margin;
    min_y = margin;
    max_x = session->drawable_size.width - margin;
    max_y = session->drawable_size.height - margin;
    if (max_x < min_x) {
        max_x = min_x;
    }
    if (max_y < min_y) {
        max_y = min_y;
    }

    if (session->marker_x <= min_x || session->marker_x >= max_x) {
        session->direction_x = -session->direction_x;
        if (session->marker_x < min_x) {
            session->marker_x = min_x;
        } else if (session->marker_x > max_x) {
            session->marker_x = max_x;
        }
    }
    if (session->marker_y <= min_y || session->marker_y >= max_y) {
        session->direction_y = -session->direction_y;
        if (session->marker_y < min_y) {
            session->marker_y = min_y;
        } else if (session->marker_y > max_y) {
            session->marker_y = max_y;
        }
    }
}
