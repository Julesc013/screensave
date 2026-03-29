#include "anthology_internal.h"

void anthology_render_session(
    screensave_saver_session *session,
    const screensave_saver_environment *environment
)
{
    screensave_color background;

    if (environment == NULL || environment->renderer == NULL) {
        return;
    }

    if (
        session != NULL &&
        session->inner_session != NULL &&
        session->inner_module != NULL &&
        session->inner_module->callbacks != NULL &&
        session->inner_module->callbacks->render_session != NULL
    ) {
        screensave_saver_environment inner_environment;

        inner_environment = *environment;
        inner_environment.config_binding = &session->inner_config_binding;
        inner_environment.seed = session->inner_seed;
        session->inner_module->callbacks->render_session(session->inner_session, &inner_environment);
        return;
    }

    background.red = 0;
    background.green = 0;
    background.blue = 0;
    background.alpha = 255;
    screensave_renderer_clear(environment->renderer, background);
}
