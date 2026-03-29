#include <string.h>

#include "../../../../platform/src/core/base/saver_registry.h"
#include "benchlab_internal.h"

const screensave_saver_module *nocturne_get_module(void);
const screensave_saver_module *ricochet_get_module(void);
const screensave_saver_module *deepfield_get_module(void);
const screensave_saver_module *ember_get_module(void);
const screensave_saver_module *oscilloscope_dreams_get_module(void);
const screensave_saver_module *pipeworks_get_module(void);
const screensave_saver_module *lifeforms_get_module(void);
const screensave_saver_module *signal_lab_get_module(void);
const screensave_saver_module *mechanical_dreams_get_module(void);
const screensave_saver_module *ecosystems_get_module(void);
const screensave_saver_module *stormglass_get_module(void);
const screensave_saver_module *night_transit_get_module(void);
const screensave_saver_module *observatory_get_module(void);

typedef const screensave_saver_module *(*benchlab_module_getter)(void);

static const benchlab_module_getter g_benchlab_module_getters[] = {
    nocturne_get_module,
    ricochet_get_module,
    deepfield_get_module,
    ember_get_module,
    oscilloscope_dreams_get_module,
    pipeworks_get_module,
    lifeforms_get_module,
    signal_lab_get_module,
    mechanical_dreams_get_module,
    ecosystems_get_module,
    stormglass_get_module,
    night_transit_get_module,
    observatory_get_module
};

static void benchlab_update_config_binding(benchlab_app *app)
{
    if (app == NULL) {
        return;
    }

    screensave_config_binding_init(
        &app->config_binding,
        &app->saver_config.common,
        app->saver_config.product_config,
        app->saver_config.product_config_size
    );
}

static void benchlab_reset_clock(benchlab_app *app)
{
    unsigned long now;

    if (app == NULL) {
        return;
    }

    now = GetTickCount();
    app->clock_start_tick = now;
    app->last_tick = now;
    ZeroMemory(&app->clock, sizeof(app->clock));
    app->clock.session_start_millis = now;
}

static unsigned long benchlab_generate_random_seed(benchlab_app *app)
{
    unsigned long tick;

    tick = GetTickCount();
    if (app != NULL) {
        app->random_seed_counter += 1UL;
        tick ^= app->random_seed_counter * 2654435761UL;
    }

    return tick ^ 0xB36E0707UL;
}

static unsigned long benchlab_stream_seed_from_base(unsigned long base_seed)
{
    return (base_seed * 1664525UL) + 1013904223UL;
}

static void benchlab_assign_session_seed(benchlab_app *app, int reseed)
{
    if (app == NULL) {
        return;
    }

    if (app->app_config.deterministic_mode) {
        if (app->app_config.fixed_seed == 0UL) {
            app->app_config.fixed_seed = BENCHLAB_DEFAULT_FIXED_SEED;
        }
        if (reseed) {
            app->app_config.fixed_seed += 1UL;
        }
        app->current_base_seed = app->app_config.fixed_seed;
        app->session_seed.deterministic = 1;
    } else {
        if (reseed || app->current_base_seed == 0UL) {
            app->current_base_seed = benchlab_generate_random_seed(app);
        }
        app->session_seed.deterministic = 0;
    }

    app->session_seed.base_seed = app->current_base_seed;
    app->session_seed.stream_seed = benchlab_stream_seed_from_base(app->current_base_seed);
}

static void benchlab_get_client_size(HWND window, screensave_sizei *size)
{
    RECT rect;

    if (size == NULL) {
        return;
    }

    size->width = 1;
    size->height = 1;
    if (window == NULL) {
        return;
    }

    GetClientRect(window, &rect);
    if (rect.right - rect.left > 0) {
        size->width = rect.right - rect.left;
    }
    if (rect.bottom - rect.top > 0) {
        size->height = rect.bottom - rect.top;
    }
}

static void benchlab_build_environment(
    benchlab_app *app,
    const screensave_sizei *drawable_size,
    screensave_saver_environment *environment
)
{
    screensave_renderer_info renderer_info;

    if (app == NULL || environment == NULL) {
        return;
    }

    ZeroMemory(environment, sizeof(*environment));
    environment->mode = SCREENSAVE_SESSION_MODE_WINDOWED;
    environment->clock = app->clock;
    environment->seed = app->session_seed;
    environment->config_binding = &app->config_binding;
    environment->renderer = app->renderer;
    environment->diagnostics = &app->diagnostics;

    if (drawable_size != NULL) {
        environment->drawable_size = *drawable_size;
    } else if (app->renderer != NULL) {
        screensave_renderer_get_info(app->renderer, &renderer_info);
        environment->drawable_size = renderer_info.drawable_size;
    } else {
        environment->drawable_size.width = 1;
        environment->drawable_size.height = 1;
    }
}

static int benchlab_create_session_only(benchlab_app *app)
{
    screensave_saver_environment environment;
    screensave_renderer_info renderer_info;

    if (app == NULL || app->renderer == NULL || app->module == NULL) {
        return 0;
    }

    if (app->module->callbacks == NULL || app->module->callbacks->create_session == NULL) {
        benchlab_emit_app_diag(
            app,
            SCREENSAVE_DIAG_LEVEL_ERROR,
            7201UL,
            "BenchLab could not create a saver session because the module has no create callback."
        );
        return 0;
    }

    screensave_renderer_get_info(app->renderer, &renderer_info);
    benchlab_build_environment(app, &renderer_info.drawable_size, &environment);
    if (!app->module->callbacks->create_session(app->module, &app->session, &environment)) {
        benchlab_emit_app_diag(
            app,
            SCREENSAVE_DIAG_LEVEL_ERROR,
            7202UL,
            "BenchLab could not create the active saver session."
        );
        return 0;
    }

    return 1;
}

static void benchlab_destroy_session_only(benchlab_app *app)
{
    if (
        app == NULL ||
        app->session == NULL ||
        app->module == NULL ||
        app->module->callbacks == NULL ||
        app->module->callbacks->destroy_session == NULL
    ) {
        return;
    }

    app->module->callbacks->destroy_session(app->session);
    app->session = NULL;
}

static void benchlab_advance_clock(benchlab_app *app, unsigned long delta_millis)
{
    if (app == NULL) {
        return;
    }

    app->clock.delta_millis = delta_millis;
    app->clock.elapsed_millis += delta_millis;
    app->clock.frame_index += 1UL;
}

static int benchlab_copy_config_state(
    screensave_saver_config_state *target,
    const screensave_saver_config_state *source
)
{
    if (target == NULL || source == NULL) {
        return 0;
    }

    target->common = source->common;
    if (
        target->product_config != NULL &&
        source->product_config != NULL &&
        target->product_config_size == source->product_config_size &&
        source->product_config_size > 0U
    ) {
        memcpy(target->product_config, source->product_config, source->product_config_size);
    }

    return 1;
}

unsigned int benchlab_get_available_module_count(void)
{
    return (unsigned int)(sizeof(g_benchlab_module_getters) / sizeof(g_benchlab_module_getters[0]));
}

const screensave_saver_module *benchlab_get_available_module(unsigned int index)
{
    if (index >= benchlab_get_available_module_count()) {
        return NULL;
    }

    return g_benchlab_module_getters[index]();
}

const screensave_saver_module *benchlab_find_target_module(const char *product_key)
{
    const screensave_saver_module *modules[
        sizeof(g_benchlab_module_getters) / sizeof(g_benchlab_module_getters[0])
    ];
    unsigned int index;

    for (index = 0U; index < benchlab_get_available_module_count(); ++index) {
        modules[index] = g_benchlab_module_getters[index]();
    }

    return screensave_saver_registry_find(modules, benchlab_get_available_module_count(), product_key);
}

const screensave_saver_module *benchlab_get_target_module(void)
{
    return benchlab_find_target_module(BENCHLAB_DEFAULT_PRODUCT_KEY);
}

int benchlab_session_initialize_config(benchlab_app *app)
{
    if (app == NULL || app->module == NULL) {
        return 0;
    }

    if (!screensave_saver_config_state_init(app->module, &app->saver_config)) {
        return 0;
    }

    screensave_saver_config_state_set_defaults(app->module, &app->saver_config);
    if (!screensave_saver_config_state_load(app->module, &app->saver_config, &app->diagnostics)) {
        benchlab_emit_app_diag(
            app,
            SCREENSAVE_DIAG_LEVEL_WARNING,
            7203UL,
            "BenchLab could not load the saver settings and is using defaults for this session."
        );
    }
    screensave_saver_config_state_clamp(app->module, &app->saver_config);
    benchlab_update_config_binding(app);
    return 1;
}

void benchlab_session_dispose_config(benchlab_app *app)
{
    if (app == NULL) {
        return;
    }

    screensave_saver_config_state_dispose(&app->saver_config);
    ZeroMemory(&app->config_binding, sizeof(app->config_binding));
}

int benchlab_session_create_runtime(benchlab_app *app, HWND window)
{
    screensave_sizei drawable_size;

    if (app == NULL) {
        return 0;
    }

    benchlab_get_client_size(window, &drawable_size);
    if (!screensave_renderer_create_for_window(
            app->requested_renderer_kind,
            window,
            &drawable_size,
            &app->diagnostics,
            &app->renderer
        )) {
        benchlab_emit_app_diag(
            app,
            SCREENSAVE_DIAG_LEVEL_ERROR,
            7204UL,
            "BenchLab could not create the shared renderer for its harness window."
        );
        return 0;
    }

    benchlab_assign_session_seed(app, 0);
    benchlab_reset_clock(app);
    if (!benchlab_create_session_only(app)) {
        screensave_renderer_shutdown(app->renderer);
        app->renderer = NULL;
        return 0;
    }

    benchlab_emit_app_diag(
        app,
        SCREENSAVE_DIAG_LEVEL_INFO,
        7205UL,
        "BenchLab attached the active saver module to the windowed harness."
    );
    return 1;
}

void benchlab_session_destroy_runtime(benchlab_app *app)
{
    if (app == NULL) {
        return;
    }

    benchlab_destroy_session_only(app);
    if (app->renderer != NULL) {
        screensave_renderer_shutdown(app->renderer);
        app->renderer = NULL;
    }
}

int benchlab_session_recreate_runtime(benchlab_app *app)
{
    HWND target_window;

    if (app == NULL) {
        return 0;
    }

    target_window = app->render_window != NULL ? app->render_window : app->main_window;
    if (target_window == NULL) {
        return 0;
    }

    benchlab_session_destroy_runtime(app);
    return benchlab_session_create_runtime(app, target_window);
}

void benchlab_session_resize(benchlab_app *app, int width, int height)
{
    screensave_sizei drawable_size;
    screensave_saver_environment environment;

    if (app == NULL || app->renderer == NULL) {
        return;
    }

    if (width <= 0 || height <= 0) {
        return;
    }

    drawable_size.width = width;
    drawable_size.height = height;
    if (!screensave_renderer_resize_for_window(app->renderer, &drawable_size)) {
        benchlab_emit_app_diag(
            app,
            SCREENSAVE_DIAG_LEVEL_WARNING,
            7206UL,
            "BenchLab could not resize the shared renderer backbuffer."
        );
        return;
    }

    if (
        app->session != NULL &&
        app->module != NULL &&
        app->module->callbacks != NULL &&
        app->module->callbacks->resize_session != NULL
    ) {
        benchlab_build_environment(app, &drawable_size, &environment);
        app->module->callbacks->resize_session(app->session, &environment);
    }
}

int benchlab_session_restart(benchlab_app *app, int reseed)
{
    if (app == NULL || app->renderer == NULL) {
        return 0;
    }

    benchlab_destroy_session_only(app);
    benchlab_assign_session_seed(app, reseed);
    benchlab_reset_clock(app);
    if (!benchlab_create_session_only(app)) {
        return 0;
    }

    if (!app->paused) {
        benchlab_session_resume_clock(app);
    }

    if (reseed) {
        benchlab_emit_app_diag(
            app,
            SCREENSAVE_DIAG_LEVEL_INFO,
            7207UL,
            "BenchLab reseeded and restarted the active saver session."
        );
    } else {
        benchlab_emit_app_diag(
            app,
            SCREENSAVE_DIAG_LEVEL_INFO,
            7208UL,
            "BenchLab restarted the active saver session."
        );
    }

    return 1;
}

void benchlab_session_tick(benchlab_app *app)
{
    unsigned long now;
    unsigned long delta_millis;
    screensave_saver_environment environment;

    if (app == NULL || app->session == NULL) {
        return;
    }

    now = GetTickCount();
    delta_millis = now - app->last_tick;
    app->last_tick = now;
    if (delta_millis > 200UL) {
        delta_millis = 200UL;
    }

    benchlab_advance_clock(app, delta_millis);
    if (
        app->module != NULL &&
        app->module->callbacks != NULL &&
        app->module->callbacks->step_session != NULL
    ) {
        benchlab_build_environment(app, NULL, &environment);
        app->module->callbacks->step_session(app->session, &environment);
    }
}

void benchlab_session_step_once(benchlab_app *app)
{
    screensave_saver_environment environment;

    if (app == NULL || app->session == NULL) {
        return;
    }

    benchlab_advance_clock(app, BENCHLAB_STEP_DELTA_MS);
    if (
        app->module != NULL &&
        app->module->callbacks != NULL &&
        app->module->callbacks->step_session != NULL
    ) {
        benchlab_build_environment(app, NULL, &environment);
        app->module->callbacks->step_session(app->session, &environment);
    }
}

void benchlab_session_render(benchlab_app *app, HDC dc)
{
    screensave_renderer_info renderer_info;
    screensave_frame_info frame_info;
    screensave_saver_environment environment;
    screensave_color background;

    if (app == NULL || app->renderer == NULL) {
        return;
    }

    screensave_renderer_set_present_dc(app->renderer, dc);
    screensave_renderer_get_info(app->renderer, &renderer_info);

    frame_info.drawable_size = renderer_info.drawable_size;
    frame_info.frame_index = app->clock.frame_index;
    frame_info.elapsed_millis = app->clock.elapsed_millis;
    frame_info.delta_millis = app->clock.delta_millis;

    if (!screensave_renderer_begin_frame(app->renderer, &frame_info)) {
        screensave_renderer_clear_present_dc(app->renderer);
        return;
    }

    background.red = 0;
    background.green = 0;
    background.blue = 0;
    background.alpha = 255;

    if (
        app->session != NULL &&
        app->module != NULL &&
        app->module->callbacks != NULL &&
        app->module->callbacks->render_session != NULL
    ) {
        benchlab_build_environment(app, &renderer_info.drawable_size, &environment);
        app->module->callbacks->render_session(app->session, &environment);
    } else {
        screensave_renderer_clear(app->renderer, background);
    }

    (void)screensave_renderer_end_frame(app->renderer);
    screensave_renderer_clear_present_dc(app->renderer);
}

void benchlab_session_resume_clock(benchlab_app *app)
{
    if (app == NULL) {
        return;
    }

    app->last_tick = GetTickCount();
}

INT_PTR benchlab_session_show_saver_dialog(benchlab_app *app)
{
    const screensave_saver_config_hooks *config_hooks;
    screensave_saver_config_state dialog_config;
    INT_PTR result;
    int was_paused;

    if (app == NULL || app->module == NULL) {
        return -1;
    }

    config_hooks = app->module->config_hooks;
    if (config_hooks == NULL || config_hooks->show_config_dialog == NULL) {
        return IDCANCEL;
    }

    if (!screensave_saver_config_state_init(app->module, &dialog_config)) {
        benchlab_emit_app_diag(
            app,
            SCREENSAVE_DIAG_LEVEL_ERROR,
            7209UL,
            "BenchLab could not allocate a temporary saver configuration state."
        );
        return -1;
    }

    screensave_saver_config_state_set_defaults(app->module, &dialog_config);
    benchlab_copy_config_state(&dialog_config, &app->saver_config);

    was_paused = app->paused;
    app->paused = 1;
    result = config_hooks->show_config_dialog(
        app->instance,
        app->main_window,
        app->module,
        &dialog_config.common,
        dialog_config.product_config,
        dialog_config.product_config_size,
        &app->diagnostics
    );

    if (result == IDOK) {
        screensave_saver_config_state_clamp(app->module, &dialog_config);
        if (!screensave_saver_config_state_save(app->module, &dialog_config, &app->diagnostics)) {
            result = -1;
        } else {
            benchlab_copy_config_state(&app->saver_config, &dialog_config);
            screensave_saver_config_state_clamp(app->module, &app->saver_config);
            benchlab_update_config_binding(app);
            if (!benchlab_session_restart(app, 0)) {
                result = -1;
            }
        }
    }

    screensave_saver_config_state_dispose(&dialog_config);
    app->paused = was_paused;
    if (!app->paused) {
        benchlab_session_resume_clock(app);
    }

    return result;
}
