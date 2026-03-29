#include <stdlib.h>

#include "gl_gallery_internal.h"

static const screensave_theme_descriptor *gl_gallery_resolve_theme(const screensave_config_binding *binding)
{
    if (binding != NULL && binding->common_config != NULL) {
        return gl_gallery_find_theme_descriptor(binding->common_config->theme_key);
    }

    return gl_gallery_find_theme_descriptor(GL_GALLERY_DEFAULT_THEME_KEY);
}

static const gl_gallery_config *gl_gallery_resolve_config(const screensave_config_binding *binding)
{
    if (binding == NULL || binding->product_config == NULL || binding->product_config_size != sizeof(gl_gallery_config)) {
        return NULL;
    }

    return (const gl_gallery_config *)binding->product_config;
}

static gl_gallery_renderer_tier gl_gallery_resolve_tier(const screensave_renderer *renderer)
{
    screensave_renderer_info info;

    if (renderer == NULL) {
        return GL_GALLERY_TIER_GDI;
    }

    screensave_renderer_get_info(renderer, &info);
    switch (info.active_kind) {
    case SCREENSAVE_RENDERER_KIND_GL46:
    case SCREENSAVE_RENDERER_KIND_GL33:
    case SCREENSAVE_RENDERER_KIND_GL21:
        return GL_GALLERY_TIER_ADVANCED;
    case SCREENSAVE_RENDERER_KIND_GL11:
        return GL_GALLERY_TIER_GL11;
    case SCREENSAVE_RENDERER_KIND_NULL:
    case SCREENSAVE_RENDERER_KIND_GDI:
    case SCREENSAVE_RENDERER_KIND_UNKNOWN:
    default:
        return GL_GALLERY_TIER_GDI;
    }
}

static unsigned long gl_gallery_motion_step(const screensave_saver_session *session)
{
    if (session == NULL) {
        return 48UL;
    }

    switch (session->config.motion_mode) {
    case GL_GALLERY_MOTION_CALM:
        return 72UL;
    case GL_GALLERY_MOTION_BRISK:
        return 28UL;
    case GL_GALLERY_MOTION_STANDARD:
    default:
        return 48UL;
    }
}

static unsigned int gl_gallery_marker_count(const screensave_saver_session *session)
{
    unsigned int count;

    if (session == NULL) {
        return 4U;
    }

    switch (session->detail_level) {
    case SCREENSAVE_DETAIL_LEVEL_LOW:
        count = 4U;
        break;
    case SCREENSAVE_DETAIL_LEVEL_HIGH:
        count = 10U;
        break;
    case SCREENSAVE_DETAIL_LEVEL_STANDARD:
    default:
        count = 7U;
        break;
    }

    if (session->tier == GL_GALLERY_TIER_ADVANCED && count < 12U) {
        count += 2U;
    } else if (session->tier == GL_GALLERY_TIER_GL11 && count < 10U) {
        count += 1U;
    }

    if (session->preview_mode && count > 5U) {
        count = 5U;
    }

    if (count > 12U) {
        count = 12U;
    }
    return count;
}

static int gl_gallery_is_preview_mode(screensave_session_mode mode)
{
    return mode == SCREENSAVE_SESSION_MODE_PREVIEW;
}

static void gl_gallery_seed_markers(screensave_saver_session *session)
{
    unsigned int index;
    unsigned int count;

    if (session == NULL) {
        return;
    }

    count = gl_gallery_marker_count(session);
    session->marker_count = count;
    for (index = 0U; index < count; ++index) {
        session->markers[index].x = (int)((session->drawable_size.width * (index + 1U)) / (count + 1U));
        session->markers[index].y = (int)((session->drawable_size.height * (index + 2U)) / (count + 2U));
        session->markers[index].radius = 8 + (int)((index % 3U) * 2U);
        session->markers[index].phase = (int)(index * 21U);
    }
}

int gl_gallery_create_session(
    const screensave_saver_module *module,
    screensave_saver_session **session,
    const screensave_saver_environment *environment
)
{
    screensave_saver_session *new_session;
    const screensave_common_config *common_config;
    const gl_gallery_config *config;
    gl_gallery_renderer_tier tier;

    if (session == NULL || module == NULL || environment == NULL || environment->config_binding == NULL) {
        return 0;
    }

    *session = NULL;
    config = gl_gallery_resolve_config(environment->config_binding);
    common_config = environment->config_binding->common_config;
    if (config == NULL || common_config == NULL) {
        return 0;
    }

    new_session = (screensave_saver_session *)calloc(1, sizeof(*new_session));
    if (new_session == NULL) {
        return 0;
    }

    new_session->config = *config;
    new_session->theme = gl_gallery_resolve_theme(environment->config_binding);
    new_session->drawable_size = environment->drawable_size;
    new_session->detail_level = common_config->detail_level;
    new_session->preview_mode = gl_gallery_is_preview_mode(environment->mode);
    tier = gl_gallery_resolve_tier(environment->renderer);
    new_session->tier = tier;
    new_session->phase_counter = 0UL;
    new_session->tier_pulse = 0UL;
    new_session->scene_phase = 0U;
    new_session->drift_phase = 0U;

    gl_gallery_seed_markers(new_session);
    *session = new_session;
    return 1;
}

void gl_gallery_destroy_session(screensave_saver_session *session)
{
    if (session == NULL) {
        return;
    }

    free(session);
}

void gl_gallery_resize_session(
    screensave_saver_session *session,
    const screensave_saver_environment *environment
)
{
    if (session == NULL || environment == NULL) {
        return;
    }

    session->drawable_size = environment->drawable_size;
    gl_gallery_seed_markers(session);
}

void gl_gallery_step_session(
    screensave_saver_session *session,
    const screensave_saver_environment *environment
)
{
    unsigned long step;
    unsigned int marker_count;
    unsigned int index;

    if (session == NULL || environment == NULL) {
        return;
    }

    session->phase_counter += 1UL;
    session->tier = gl_gallery_resolve_tier(environment->renderer);
    step = gl_gallery_motion_step(session);
    session->tier_pulse += step;
    session->scene_phase = (unsigned int)((session->phase_counter / (step / 4UL + 1UL)) % 3UL);
    marker_count = gl_gallery_marker_count(session);
    if (marker_count == 0U) {
        marker_count = 1U;
    }
    session->drift_phase = (session->drift_phase + 1U) % marker_count;

    for (index = 0U; index < session->marker_count; ++index) {
        session->markers[index].phase = (int)((session->markers[index].phase + 1) & 63);
        if ((session->phase_counter + index) % (step / 2UL + 6UL) == 0UL) {
            session->markers[index].radius += (session->markers[index].phase & 1) ? 1 : -1;
            if (session->markers[index].radius < 6) {
                session->markers[index].radius = 6;
            }
            if (session->markers[index].radius > 18) {
                session->markers[index].radius = 18;
            }
        }
    }
}
