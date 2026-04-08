#include "suite_internal.h"

static ATOM g_suite_run_window_class = 0;

static LRESULT CALLBACK suite_run_window_proc(HWND window, UINT message, WPARAM wParam, LPARAM lParam);

static unsigned long suite_stream_seed_from_base(unsigned long base_seed)
{
    return (base_seed * 1664525UL) + 1013904223UL;
}

static void suite_get_client_size(HWND window, screensave_sizei *size)
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

static unsigned long suite_generate_random_seed(unsigned long *counter)
{
    unsigned long tick;

    tick = GetTickCount();
    if (counter != NULL) {
        *counter += 1UL;
        tick ^= (*counter * 2654435761UL);
    }

    return tick ^ 0x53554954UL;
}

static void suite_assign_seed(
    screensave_common_config *common_config,
    unsigned long *base_seed,
    unsigned long *random_seed_counter,
    screensave_session_seed *seed_out,
    int reseed
)
{
    if (common_config == NULL || base_seed == NULL || seed_out == NULL) {
        return;
    }

    if (common_config->use_deterministic_seed) {
        if (common_config->deterministic_seed == 0UL) {
            common_config->deterministic_seed = SUITE_DEFAULT_DETERMINISTIC_SEED;
        }
        *base_seed = common_config->deterministic_seed;
        if (reseed) {
            *base_seed += 1UL;
        }
        seed_out->deterministic = 1;
    } else {
        *base_seed = suite_generate_random_seed(random_seed_counter);
        seed_out->deterministic = 0;
    }

    seed_out->base_seed = *base_seed;
    seed_out->stream_seed = suite_stream_seed_from_base(*base_seed);
}

static void suite_reset_clock(
    screensave_runtime_clock *clock,
    unsigned long *clock_start_tick,
    unsigned long *last_tick
)
{
    unsigned long now;

    now = GetTickCount();
    if (clock != NULL) {
        ZeroMemory(clock, sizeof(*clock));
        clock->session_start_millis = now;
    }
    if (clock_start_tick != NULL) {
        *clock_start_tick = now;
    }
    if (last_tick != NULL) {
        *last_tick = now;
    }
}

static void suite_advance_clock(screensave_runtime_clock *clock, unsigned long delta_millis)
{
    if (clock == NULL) {
        return;
    }

    clock->delta_millis = delta_millis;
    clock->elapsed_millis += delta_millis;
    clock->frame_index += 1UL;
}

static void suite_preview_build_environment(
    suite_app *app,
    const screensave_sizei *drawable_size,
    const screensave_session_seed *seed,
    screensave_saver_environment *environment
)
{
    if (app == NULL || environment == NULL) {
        return;
    }

    ZeroMemory(environment, sizeof(*environment));
    environment->mode = SCREENSAVE_SESSION_MODE_PREVIEW;
    environment->clock = app->preview_runtime.clock;
    if (seed != NULL) {
        environment->seed = *seed;
    }
    if (drawable_size != NULL) {
        environment->drawable_size = *drawable_size;
    } else {
        environment->drawable_size.width = 1;
        environment->drawable_size.height = 1;
    }
    environment->config_binding = &app->config_binding;
    environment->renderer = app->preview_runtime.renderer;
    environment->diagnostics = &app->diagnostics;
}

static int suite_preview_resolve_config(
    suite_app *app,
    const screensave_saver_module *module,
    const screensave_session_seed *seed
)
{
    if (app == NULL || module == NULL || seed == NULL) {
        return 0;
    }

    if (!screensave_saver_config_state_copy(module, &app->resolved_config, &app->working_config)) {
        return 0;
    }

    if (!screensave_saver_config_state_resolve_for_session(
            module,
            &app->working_config,
            seed,
            &app->resolved_config,
            &app->diagnostics
        )) {
        screensave_saver_config_state_clamp(module, &app->resolved_config);
        screensave_diag_emit(
            &app->diagnostics,
            SCREENSAVE_DIAG_LEVEL_WARNING,
            SCREENSAVE_DIAG_DOMAIN_APP,
            9011UL,
            "suite_preview_resolve_config",
            "Suite fell back to the stored saver settings because session randomization could not be resolved."
        );
    }

    screensave_config_binding_init(
        &app->config_binding,
        &app->resolved_config.common,
        app->resolved_config.product_config,
        app->resolved_config.product_config_size
    );
    return 1;
}

static void suite_preview_destroy_session(suite_app *app)
{
    const screensave_saver_module *module;

    if (app == NULL) {
        return;
    }

    module = suite_get_selected_module(app);
    if (
        module != NULL &&
        app->preview_runtime.session != NULL &&
        module->callbacks != NULL &&
        module->callbacks->destroy_session != NULL
    ) {
        module->callbacks->destroy_session(app->preview_runtime.session);
    }

    app->preview_runtime.session = NULL;
}

int suite_preview_start(suite_app *app)
{
    const suite_catalog_entry *entry;
    const screensave_saver_module *module;
    screensave_saver_environment environment;
    screensave_sizei drawable_size;
    screensave_renderer_kind requested_kind;

    if (app == NULL || app->preview_window == NULL) {
        return 0;
    }

    entry = suite_get_selected_entry(app);
    module = suite_get_selected_module(app);
    if (entry == NULL || !screensave_saver_module_is_valid(module)) {
        return 0;
    }

    suite_get_client_size(app->preview_window, &drawable_size);
    requested_kind = (screensave_renderer_kind)app->app_config.renderer_request;
    if (!screensave_renderer_create_for_saver_window(
            module,
            &app->working_config.common,
            requested_kind,
            app->preview_window,
            &drawable_size,
            &app->diagnostics,
            &app->preview_runtime.renderer
        )) {
        return 0;
    }

    suite_assign_seed(
        &app->working_config.common,
        &app->preview_runtime.base_seed,
        &app->preview_runtime.random_seed_counter,
        &app->preview_runtime.seed,
        0
    );
    if (!suite_preview_resolve_config(app, module, &app->preview_runtime.seed)) {
        screensave_renderer_shutdown(app->preview_runtime.renderer);
        app->preview_runtime.renderer = NULL;
        return 0;
    }

    suite_reset_clock(
        &app->preview_runtime.clock,
        &app->preview_runtime.clock_start_tick,
        &app->preview_runtime.last_tick
    );
    suite_preview_build_environment(app, &drawable_size, &app->preview_runtime.seed, &environment);
    if (
        module->callbacks == NULL ||
        module->callbacks->create_session == NULL ||
        !module->callbacks->create_session(module, &app->preview_runtime.session, &environment)
    ) {
        screensave_renderer_shutdown(app->preview_runtime.renderer);
        app->preview_runtime.renderer = NULL;
        return 0;
    }

    return 1;
}

void suite_preview_stop(suite_app *app)
{
    if (app == NULL) {
        return;
    }

    suite_preview_destroy_session(app);
    if (app->preview_runtime.renderer != NULL) {
        screensave_renderer_shutdown(app->preview_runtime.renderer);
        app->preview_runtime.renderer = NULL;
    }
}

int suite_preview_restart(suite_app *app, int reseed)
{
    const screensave_saver_module *module;
    screensave_saver_environment environment;
    screensave_sizei drawable_size;

    if (app == NULL) {
        return 0;
    }

    suite_preview_stop(app);
    if (!suite_preview_start(app)) {
        return 0;
    }

    if (!reseed) {
        return 1;
    }

    module = suite_get_selected_module(app);
    if (module == NULL || app->preview_runtime.renderer == NULL) {
        return 0;
    }

    suite_preview_destroy_session(app);
    suite_assign_seed(
        &app->working_config.common,
        &app->preview_runtime.base_seed,
        &app->preview_runtime.random_seed_counter,
        &app->preview_runtime.seed,
        1
    );
    if (!suite_preview_resolve_config(app, module, &app->preview_runtime.seed)) {
        return 0;
    }
    suite_get_client_size(app->preview_window, &drawable_size);
    suite_reset_clock(
        &app->preview_runtime.clock,
        &app->preview_runtime.clock_start_tick,
        &app->preview_runtime.last_tick
    );
    suite_preview_build_environment(app, &drawable_size, &app->preview_runtime.seed, &environment);
    if (
        module->callbacks == NULL ||
        module->callbacks->create_session == NULL ||
        !module->callbacks->create_session(module, &app->preview_runtime.session, &environment)
    ) {
        return 0;
    }

    return 1;
}

void suite_preview_tick(suite_app *app)
{
    const screensave_saver_module *module;
    screensave_saver_environment environment;
    screensave_renderer_info renderer_info;
    unsigned long now;
    unsigned long delta_millis;

    if (app == NULL || app->preview_runtime.session == NULL || app->preview_runtime.renderer == NULL) {
        return;
    }

    module = suite_get_selected_module(app);
    if (module == NULL || module->callbacks == NULL || module->callbacks->step_session == NULL) {
        return;
    }

    now = GetTickCount();
    delta_millis = now - app->preview_runtime.last_tick;
    app->preview_runtime.last_tick = now;
    if (delta_millis > 200UL) {
        delta_millis = 200UL;
    }

    suite_advance_clock(&app->preview_runtime.clock, delta_millis);
    screensave_renderer_get_info(app->preview_runtime.renderer, &renderer_info);
    suite_preview_build_environment(app, &renderer_info.drawable_size, &app->preview_runtime.seed, &environment);
    module->callbacks->step_session(app->preview_runtime.session, &environment);
}

void suite_preview_render(suite_app *app, HDC dc)
{
    const screensave_saver_module *module;
    screensave_renderer_info renderer_info;
    screensave_frame_info frame_info;
    screensave_saver_environment environment;
    screensave_color background;
    RECT client_rect;

    if (app == NULL || dc == NULL) {
        return;
    }

    if (app->preview_runtime.renderer == NULL) {
        GetClientRect(app->preview_window, &client_rect);
        FillRect(dc, &client_rect, (HBRUSH)GetStockObject(BLACK_BRUSH));
        return;
    }

    module = suite_get_selected_module(app);
    screensave_renderer_set_present_dc(app->preview_runtime.renderer, dc);
    screensave_renderer_get_info(app->preview_runtime.renderer, &renderer_info);

    frame_info.drawable_size = renderer_info.drawable_size;
    frame_info.frame_index = app->preview_runtime.clock.frame_index;
    frame_info.elapsed_millis = app->preview_runtime.clock.elapsed_millis;
    frame_info.delta_millis = app->preview_runtime.clock.delta_millis;
    if (!screensave_renderer_begin_frame(app->preview_runtime.renderer, &frame_info)) {
        screensave_renderer_clear_present_dc(app->preview_runtime.renderer);
        return;
    }

    background.red = 0;
    background.green = 0;
    background.blue = 0;
    background.alpha = 255;
    if (
        module != NULL &&
        app->preview_runtime.session != NULL &&
        module->callbacks != NULL &&
        module->callbacks->render_session != NULL
    ) {
        suite_preview_build_environment(
            app,
            &renderer_info.drawable_size,
            &app->preview_runtime.seed,
            &environment
        );
        module->callbacks->render_session(app->preview_runtime.session, &environment);
    } else {
        screensave_renderer_clear(app->preview_runtime.renderer, background);
    }

    (void)screensave_renderer_end_frame(app->preview_runtime.renderer);
    screensave_renderer_clear_present_dc(app->preview_runtime.renderer);
}

void suite_preview_resize(suite_app *app, int width, int height)
{
    const screensave_saver_module *module;
    screensave_sizei drawable_size;
    screensave_saver_environment environment;

    if (app == NULL || app->preview_runtime.renderer == NULL) {
        return;
    }

    if (width <= 0 || height <= 0) {
        return;
    }

    module = suite_get_selected_module(app);
    drawable_size.width = width;
    drawable_size.height = height;
    if (!screensave_renderer_resize_for_window(app->preview_runtime.renderer, &drawable_size)) {
        return;
    }

    if (
        module != NULL &&
        app->preview_runtime.session != NULL &&
        module->callbacks != NULL &&
        module->callbacks->resize_session != NULL
    ) {
        suite_preview_build_environment(app, &drawable_size, &app->preview_runtime.seed, &environment);
        module->callbacks->resize_session(app->preview_runtime.session, &environment);
    }
}

static suite_run_window *suite_get_run_window(HWND window)
{
    return (suite_run_window *)GetWindowLongPtrA(window, GWLP_USERDATA);
}

static void suite_attach_run_window(HWND window, LPARAM lParam)
{
    CREATESTRUCTA *create_struct;
    suite_run_window *run_window;

    create_struct = (CREATESTRUCTA *)lParam;
    run_window = (suite_run_window *)create_struct->lpCreateParams;
    SetWindowLongPtrA(window, GWLP_USERDATA, (LONG_PTR)run_window);
}

static void suite_run_build_environment(
    suite_run_window *run_window,
    const screensave_sizei *drawable_size,
    const screensave_session_seed *seed,
    screensave_saver_environment *environment
)
{
    if (run_window == NULL || environment == NULL) {
        return;
    }

    ZeroMemory(environment, sizeof(*environment));
    environment->mode = SCREENSAVE_SESSION_MODE_WINDOWED;
    environment->clock = run_window->clock;
    if (seed != NULL) {
        environment->seed = *seed;
    }
    if (drawable_size != NULL) {
        environment->drawable_size = *drawable_size;
    } else {
        environment->drawable_size.width = 1;
        environment->drawable_size.height = 1;
    }
    environment->config_binding = &run_window->config_binding;
    environment->renderer = run_window->renderer;
    environment->diagnostics = &run_window->diagnostics;
}

static int suite_run_resolve_config(
    suite_run_window *run_window,
    const screensave_saver_module *module,
    const screensave_session_seed *seed
)
{
    if (run_window == NULL || module == NULL || seed == NULL) {
        return 0;
    }

    if (!screensave_saver_config_state_copy(module, &run_window->resolved_config, &run_window->working_config)) {
        return 0;
    }

    if (!screensave_saver_config_state_resolve_for_session(
            module,
            &run_window->working_config,
            seed,
            &run_window->resolved_config,
            &run_window->diagnostics
        )) {
        screensave_saver_config_state_clamp(module, &run_window->resolved_config);
    }

    screensave_config_binding_init(
        &run_window->config_binding,
        &run_window->resolved_config.common,
        run_window->resolved_config.product_config,
        run_window->resolved_config.product_config_size
    );
    return 1;
}

static void suite_run_destroy_runtime(suite_run_window *run_window)
{
    const screensave_saver_module *module;

    if (run_window == NULL) {
        return;
    }

    module = run_window->entry != NULL ? run_window->entry->module : NULL;
    if (
        module != NULL &&
        run_window->session != NULL &&
        module->callbacks != NULL &&
        module->callbacks->destroy_session != NULL
    ) {
        module->callbacks->destroy_session(run_window->session);
    }
    run_window->session = NULL;

    if (run_window->renderer != NULL) {
        screensave_renderer_shutdown(run_window->renderer);
        run_window->renderer = NULL;
    }
}

static int suite_run_start_runtime(suite_run_window *run_window)
{
    const screensave_saver_module *module;
    screensave_sizei drawable_size;
    screensave_saver_environment environment;

    if (run_window == NULL || run_window->window == NULL || run_window->entry == NULL) {
        return 0;
    }

    module = run_window->entry->module;
    if (!screensave_saver_module_is_valid(module)) {
        return 0;
    }

    suite_get_client_size(run_window->window, &drawable_size);
    if (!screensave_renderer_create_for_saver_window(
            module,
            &run_window->working_config.common,
            run_window->requested_renderer_kind,
            run_window->window,
            &drawable_size,
            &run_window->diagnostics,
            &run_window->renderer
        )) {
        return 0;
    }

    suite_assign_seed(
        &run_window->working_config.common,
        &run_window->base_seed,
        &run_window->random_seed_counter,
        &run_window->seed,
        0
    );
    if (!suite_run_resolve_config(run_window, module, &run_window->seed)) {
        screensave_renderer_shutdown(run_window->renderer);
        run_window->renderer = NULL;
        return 0;
    }

    suite_reset_clock(&run_window->clock, &run_window->clock_start_tick, &run_window->last_tick);
    suite_run_build_environment(run_window, &drawable_size, &run_window->seed, &environment);
    if (
        module->callbacks == NULL ||
        module->callbacks->create_session == NULL ||
        !module->callbacks->create_session(module, &run_window->session, &environment)
    ) {
        screensave_renderer_shutdown(run_window->renderer);
        run_window->renderer = NULL;
        return 0;
    }

    return 1;
}

static void suite_run_tick(suite_run_window *run_window)
{
    const screensave_saver_module *module;
    screensave_saver_environment environment;
    screensave_renderer_info renderer_info;
    unsigned long now;
    unsigned long delta_millis;

    if (run_window == NULL || run_window->session == NULL || run_window->renderer == NULL) {
        return;
    }

    module = run_window->entry != NULL ? run_window->entry->module : NULL;
    if (module == NULL || module->callbacks == NULL || module->callbacks->step_session == NULL) {
        return;
    }

    now = GetTickCount();
    delta_millis = now - run_window->last_tick;
    run_window->last_tick = now;
    if (delta_millis > 200UL) {
        delta_millis = 200UL;
    }

    suite_advance_clock(&run_window->clock, delta_millis);
    screensave_renderer_get_info(run_window->renderer, &renderer_info);
    suite_run_build_environment(run_window, &renderer_info.drawable_size, &run_window->seed, &environment);
    module->callbacks->step_session(run_window->session, &environment);
}

static void suite_run_render(suite_run_window *run_window, HDC dc)
{
    const screensave_saver_module *module;
    screensave_renderer_info renderer_info;
    screensave_frame_info frame_info;
    screensave_saver_environment environment;
    screensave_color background;
    RECT client_rect;

    if (run_window == NULL || dc == NULL) {
        return;
    }

    if (run_window->renderer == NULL) {
        GetClientRect(run_window->window, &client_rect);
        FillRect(dc, &client_rect, (HBRUSH)GetStockObject(BLACK_BRUSH));
        return;
    }

    module = run_window->entry != NULL ? run_window->entry->module : NULL;
    screensave_renderer_set_present_dc(run_window->renderer, dc);
    screensave_renderer_get_info(run_window->renderer, &renderer_info);

    frame_info.drawable_size = renderer_info.drawable_size;
    frame_info.frame_index = run_window->clock.frame_index;
    frame_info.elapsed_millis = run_window->clock.elapsed_millis;
    frame_info.delta_millis = run_window->clock.delta_millis;
    if (!screensave_renderer_begin_frame(run_window->renderer, &frame_info)) {
        screensave_renderer_clear_present_dc(run_window->renderer);
        return;
    }

    background.red = 0;
    background.green = 0;
    background.blue = 0;
    background.alpha = 255;
    if (
        module != NULL &&
        run_window->session != NULL &&
        module->callbacks != NULL &&
        module->callbacks->render_session != NULL
    ) {
        suite_run_build_environment(run_window, &renderer_info.drawable_size, &run_window->seed, &environment);
        module->callbacks->render_session(run_window->session, &environment);
    } else {
        screensave_renderer_clear(run_window->renderer, background);
    }

    (void)screensave_renderer_end_frame(run_window->renderer);
    screensave_renderer_clear_present_dc(run_window->renderer);
}

static void suite_run_resize(suite_run_window *run_window, int width, int height)
{
    const screensave_saver_module *module;
    screensave_sizei drawable_size;
    screensave_saver_environment environment;

    if (run_window == NULL || run_window->renderer == NULL || width <= 0 || height <= 0) {
        return;
    }

    module = run_window->entry != NULL ? run_window->entry->module : NULL;
    drawable_size.width = width;
    drawable_size.height = height;
    if (!screensave_renderer_resize_for_window(run_window->renderer, &drawable_size)) {
        return;
    }

    if (
        module != NULL &&
        run_window->session != NULL &&
        module->callbacks != NULL &&
        module->callbacks->resize_session != NULL
    ) {
        suite_run_build_environment(run_window, &drawable_size, &run_window->seed, &environment);
        module->callbacks->resize_session(run_window->session, &environment);
    }
}

static LRESULT CALLBACK suite_run_window_proc(HWND window, UINT message, WPARAM wParam, LPARAM lParam)
{
    PAINTSTRUCT paint;
    suite_run_window *run_window;

    run_window = suite_get_run_window(window);

    switch (message) {
    case WM_NCCREATE:
        suite_attach_run_window(window, lParam);
        run_window = suite_get_run_window(window);
        if (run_window != NULL) {
            run_window->window = window;
        }
        return TRUE;

    case WM_CREATE:
        if (run_window == NULL || !suite_run_start_runtime(run_window)) {
            return -1;
        }
        SetTimer(window, SUITE_TIMER_ID, SUITE_TIMER_INTERVAL_MS, NULL);
        return 0;

    case WM_ERASEBKGND:
        return 1;

    case WM_SIZE:
        if (run_window != NULL && wParam != SIZE_MINIMIZED) {
            suite_run_resize(run_window, LOWORD(lParam), HIWORD(lParam));
        }
        return 0;

    case WM_TIMER:
        if (run_window != NULL && wParam == SUITE_TIMER_ID) {
            suite_run_tick(run_window);
            InvalidateRect(window, NULL, FALSE);
        }
        return 0;

    case WM_KEYDOWN:
        if (wParam == VK_ESCAPE) {
            DestroyWindow(window);
            return 0;
        }
        break;

    case WM_PAINT:
        BeginPaint(window, &paint);
        suite_run_render(run_window, paint.hdc);
        EndPaint(window, &paint);
        return 0;

    case WM_DESTROY:
        KillTimer(window, SUITE_TIMER_ID);
        suite_run_destroy_runtime(run_window);
        return 0;

    case WM_NCDESTROY:
        if (run_window != NULL) {
            suite_detached_window_closed(run_window);
        }
        return 0;
    }

    return DefWindowProcA(window, message, wParam, lParam);
}

int suite_register_run_window_class(HINSTANCE instance)
{
    WNDCLASSA window_class;

    if (g_suite_run_window_class != 0) {
        return 1;
    }

    ZeroMemory(&window_class, sizeof(window_class));
    window_class.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    window_class.lpfnWndProc = suite_run_window_proc;
    window_class.hInstance = instance;
    window_class.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    window_class.hCursor = LoadCursor(NULL, IDC_ARROW);
    window_class.lpszClassName = SUITE_RUN_WINDOW_CLASSA;
    g_suite_run_window_class = RegisterClassA(&window_class);
    return g_suite_run_window_class != 0;
}

int suite_open_windowed_run(suite_app *app)
{
    const suite_catalog_entry *entry;
    const screensave_saver_module *module;
    suite_run_window *run_window;
    RECT rect;
    char title[128];

    if (app == NULL) {
        return 0;
    }

    if (app->detached_run != NULL && app->detached_run->window != NULL) {
        ShowWindow(app->detached_run->window, SW_SHOWNORMAL);
        SetForegroundWindow(app->detached_run->window);
        return 1;
    }

    entry = suite_get_selected_entry(app);
    module = suite_get_selected_module(app);
    if (entry == NULL || module == NULL) {
        return 0;
    }

    if (!suite_register_run_window_class(app->instance)) {
        return 0;
    }

    run_window = (suite_run_window *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(*run_window));
    if (run_window == NULL) {
        return 0;
    }

    run_window->owner = app;
    run_window->entry = entry;
    run_window->requested_renderer_kind = (screensave_renderer_kind)app->app_config.renderer_request;
    screensave_diag_context_init(&run_window->diagnostics, SCREENSAVE_DIAG_LEVEL_DEBUG);
    if (
        !screensave_saver_config_state_init(module, &run_window->working_config) ||
        !screensave_saver_config_state_init(module, &run_window->resolved_config) ||
        !screensave_saver_config_state_copy(module, &run_window->working_config, &app->working_config) ||
        !screensave_saver_config_state_copy(module, &run_window->resolved_config, &app->working_config)
    ) {
        screensave_saver_config_state_dispose(&run_window->working_config);
        screensave_saver_config_state_dispose(&run_window->resolved_config);
        HeapFree(GetProcessHeap(), 0U, run_window);
        return 0;
    }
    screensave_config_binding_init(
        &run_window->config_binding,
        &run_window->resolved_config.common,
        run_window->resolved_config.product_config,
        run_window->resolved_config.product_config_size
    );

    rect.left = 0;
    rect.top = 0;
    rect.right = app->app_config.client_width > 960 ? 960 : app->app_config.client_width;
    rect.bottom = app->app_config.client_height > 640 ? 640 : app->app_config.client_height;
    AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, FALSE);

    title[0] = '\0';
    lstrcpyA(title, SUITE_APP_TITLEA);
    lstrcatA(title, " - ");
    lstrcatA(title, entry->manifest.name);
    lstrcatA(title, " Windowed Run");

    run_window->window = CreateWindowExA(
        0,
        SUITE_RUN_WINDOW_CLASSA,
        title,
        WS_OVERLAPPEDWINDOW | WS_VISIBLE,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        rect.right - rect.left,
        rect.bottom - rect.top,
        app->main_window,
        NULL,
        app->instance,
        run_window
    );
    if (run_window->window == NULL) {
        screensave_saver_config_state_dispose(&run_window->working_config);
        screensave_saver_config_state_dispose(&run_window->resolved_config);
        HeapFree(GetProcessHeap(), 0U, run_window);
        return 0;
    }

    app->detached_run = run_window;
    ShowWindow(run_window->window, SW_SHOWNORMAL);
    UpdateWindow(run_window->window);
    return 1;
}

void suite_detached_window_closed(suite_run_window *run_window)
{
    if (run_window == NULL) {
        return;
    }

    if (run_window->owner != NULL && run_window->owner->detached_run == run_window) {
        run_window->owner->detached_run = NULL;
    }

    screensave_saver_config_state_dispose(&run_window->working_config);
    screensave_saver_config_state_dispose(&run_window->resolved_config);
    HeapFree(GetProcessHeap(), 0U, run_window);
}
