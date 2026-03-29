#include "benchlab_internal.h"

static ATOM g_benchlab_window_class = 0;
static ATOM g_benchlab_render_class = 0;
static ATOM g_benchlab_info_class = 0;

static LRESULT CALLBACK benchlab_window_proc(HWND window, UINT message, WPARAM wParam, LPARAM lParam);
static LRESULT CALLBACK benchlab_render_window_proc(HWND window, UINT message, WPARAM wParam, LPARAM lParam);
static LRESULT CALLBACK benchlab_info_window_proc(HWND window, UINT message, WPARAM wParam, LPARAM lParam);

static void benchlab_show_message(HWND owner, const char *text, UINT type)
{
    MessageBoxA(owner, text, BENCHLAB_APP_TITLEA, type | MB_SETFOREGROUND);
}

static benchlab_app *benchlab_get_window_app(HWND window)
{
    return (benchlab_app *)GetWindowLongA(window, GWL_USERDATA);
}

static const char *benchlab_renderer_request_label(screensave_renderer_kind requested_kind)
{
    if (requested_kind == SCREENSAVE_RENDERER_KIND_UNKNOWN) {
        return "auto";
    }

    return screensave_renderer_kind_name(requested_kind);
}

static void benchlab_attach_window_app(HWND window, LPARAM lParam)
{
    CREATESTRUCTA *create_struct;
    benchlab_app *app;

    create_struct = (CREATESTRUCTA *)lParam;
    app = (benchlab_app *)create_struct->lpCreateParams;
    SetWindowLongA(window, GWL_USERDATA, (LONG)app);
}

static HMENU benchlab_create_menu(void)
{
    HMENU root_menu;
    HMENU run_menu;
    HMENU saver_menu;
    HMENU options_menu;
    unsigned int index;
    const screensave_saver_module *module;

    root_menu = CreateMenu();
    run_menu = CreatePopupMenu();
    saver_menu = CreatePopupMenu();
    options_menu = CreatePopupMenu();
    if (root_menu == NULL || run_menu == NULL || saver_menu == NULL || options_menu == NULL) {
        if (options_menu != NULL) {
            DestroyMenu(options_menu);
        }
        if (saver_menu != NULL) {
            DestroyMenu(saver_menu);
        }
        if (run_menu != NULL) {
            DestroyMenu(run_menu);
        }
        if (root_menu != NULL) {
            DestroyMenu(root_menu);
        }
        return NULL;
    }

    AppendMenuA(run_menu, MF_STRING, IDM_BENCHLAB_RESTART, "&Restart\tF5");
    AppendMenuA(run_menu, MF_STRING, IDM_BENCHLAB_RESEED, "R&eseed\tF6");
    AppendMenuA(run_menu, MF_STRING, IDM_BENCHLAB_TOGGLE_PAUSE, "&Pause\tF8");
    AppendMenuA(run_menu, MF_STRING, IDM_BENCHLAB_STEP_FRAME, "S&tep Frame\tF9");
    AppendMenuA(run_menu, MF_SEPARATOR, 0U, NULL);
    AppendMenuA(run_menu, MF_STRING, IDM_BENCHLAB_EXIT, "E&xit");

    for (index = 0U; index < benchlab_get_available_module_count(); ++index) {
        module = benchlab_get_available_module(index);
        if (module == NULL) {
            continue;
        }

        AppendMenuA(
            saver_menu,
            MF_STRING,
            IDM_BENCHLAB_PRODUCT_FIRST + index,
            module->identity.display_name
        );
    }
    AppendMenuA(saver_menu, MF_SEPARATOR, 0U, NULL);
    AppendMenuA(saver_menu, MF_STRING, IDM_BENCHLAB_SAVER_SETTINGS, "Saver &Settings...");

    AppendMenuA(options_menu, MF_STRING, IDM_BENCHLAB_TOGGLE_DETERMINISTIC, "&Deterministic Mode\tF7");
    AppendMenuA(options_menu, MF_STRING, IDM_BENCHLAB_TOGGLE_OVERLAY, "&Info Panel");
    AppendMenuA(options_menu, MF_SEPARATOR, 0U, NULL);
    AppendMenuA(options_menu, MF_STRING, IDM_BENCHLAB_RENDERER_AUTO, "Renderer &Auto");
    AppendMenuA(options_menu, MF_STRING, IDM_BENCHLAB_RENDERER_GDI, "Renderer &GDI");
    AppendMenuA(options_menu, MF_STRING, IDM_BENCHLAB_RENDERER_GL11, "Renderer &GL11");
    AppendMenuA(options_menu, MF_STRING, IDM_BENCHLAB_RENDERER_GL21, "Renderer GL&21");
    AppendMenuA(options_menu, MF_STRING, IDM_BENCHLAB_RENDERER_GL33, "Renderer GL&33");
    AppendMenuA(options_menu, MF_STRING, IDM_BENCHLAB_RENDERER_GL46, "Renderer GL&46");

    AppendMenuA(root_menu, MF_POPUP, (UINT_PTR)run_menu, "&Run");
    AppendMenuA(root_menu, MF_POPUP, (UINT_PTR)saver_menu, "&Saver");
    AppendMenuA(root_menu, MF_POPUP, (UINT_PTR)options_menu, "&Options");
    return root_menu;
}

static UINT benchlab_current_product_command(const benchlab_app *app)
{
    unsigned int index;

    if (app == NULL || app->module == NULL) {
        return IDM_BENCHLAB_PRODUCT_FIRST;
    }

    for (index = 0U; index < benchlab_get_available_module_count(); ++index) {
        if (benchlab_get_available_module(index) == app->module) {
            return IDM_BENCHLAB_PRODUCT_FIRST + index;
        }
    }

    return IDM_BENCHLAB_PRODUCT_FIRST;
}

static UINT benchlab_current_renderer_command(const benchlab_app *app)
{
    switch (app->requested_renderer_kind) {
    case SCREENSAVE_RENDERER_KIND_GDI:
        return IDM_BENCHLAB_RENDERER_GDI;

    case SCREENSAVE_RENDERER_KIND_GL11:
        return IDM_BENCHLAB_RENDERER_GL11;

    case SCREENSAVE_RENDERER_KIND_GL21:
        return IDM_BENCHLAB_RENDERER_GL21;

    case SCREENSAVE_RENDERER_KIND_GL33:
        return IDM_BENCHLAB_RENDERER_GL33;

    case SCREENSAVE_RENDERER_KIND_GL46:
        return IDM_BENCHLAB_RENDERER_GL46;

    case SCREENSAVE_RENDERER_KIND_UNKNOWN:
    case SCREENSAVE_RENDERER_KIND_NULL:
    default:
        return IDM_BENCHLAB_RENDERER_AUTO;
    }
}

static void benchlab_update_menu_state(benchlab_app *app)
{
    if (app == NULL || app->menu == NULL || app->main_window == NULL) {
        return;
    }

    CheckMenuItem(
        app->menu,
        IDM_BENCHLAB_TOGGLE_DETERMINISTIC,
        MF_BYCOMMAND | (app->app_config.deterministic_mode ? MF_CHECKED : MF_UNCHECKED)
    );
    CheckMenuItem(
        app->menu,
        IDM_BENCHLAB_TOGGLE_OVERLAY,
        MF_BYCOMMAND | (app->app_config.overlay_enabled ? MF_CHECKED : MF_UNCHECKED)
    );
    CheckMenuItem(
        app->menu,
        IDM_BENCHLAB_TOGGLE_PAUSE,
        MF_BYCOMMAND | (app->paused ? MF_CHECKED : MF_UNCHECKED)
    );
    CheckMenuRadioItem(
        app->menu,
        IDM_BENCHLAB_PRODUCT_FIRST,
        IDM_BENCHLAB_PRODUCT_FIRST + benchlab_get_available_module_count() - 1U,
        benchlab_current_product_command(app),
        MF_BYCOMMAND
    );
    CheckMenuRadioItem(
        app->menu,
        IDM_BENCHLAB_RENDERER_AUTO,
        IDM_BENCHLAB_RENDERER_GL46,
        benchlab_current_renderer_command(app),
        MF_BYCOMMAND
    );
    EnableMenuItem(
        app->menu,
        IDM_BENCHLAB_STEP_FRAME,
        MF_BYCOMMAND | (app->paused ? MF_ENABLED : MF_GRAYED)
    );
    DrawMenuBar(app->main_window);
}

static void benchlab_update_window_title(benchlab_app *app)
{
    char title[256];
    screensave_renderer_info renderer_info;

    if (app == NULL || app->main_window == NULL || app->module == NULL) {
        return;
    }

    title[0] = '\0';
    lstrcpyA(title, BENCHLAB_APP_TITLEA);
    lstrcatA(title, " - ");
    lstrcatA(title, app->module->identity.display_name);
    lstrcatA(title, " [");
    lstrcatA(title, benchlab_renderer_request_label(app->requested_renderer_kind));
    if (app->renderer != NULL) {
        screensave_renderer_get_info(app->renderer, &renderer_info);
        lstrcatA(title, " -> ");
        lstrcatA(title, screensave_renderer_kind_name(renderer_info.active_kind));
    }
    lstrcatA(title, "]");
    if (app->app_config.deterministic_mode) {
        lstrcatA(title, " [deterministic]");
    }
    if (app->paused) {
        lstrcatA(title, " [paused]");
    }

    SetWindowTextA(app->main_window, title);
}

static int benchlab_register_window_classes(HINSTANCE instance)
{
    WNDCLASSA window_class;

    if (g_benchlab_window_class != 0 && g_benchlab_render_class != 0 && g_benchlab_info_class != 0) {
        return 1;
    }

    ZeroMemory(&window_class, sizeof(window_class));
    window_class.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
    window_class.lpfnWndProc = benchlab_window_proc;
    window_class.hInstance = instance;
    window_class.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    window_class.hCursor = LoadCursor(NULL, IDC_ARROW);
    window_class.lpszClassName = BENCHLAB_WINDOW_CLASSA;
    g_benchlab_window_class = RegisterClassA(&window_class);
    if (g_benchlab_window_class == 0) {
        return 0;
    }

    ZeroMemory(&window_class, sizeof(window_class));
    window_class.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    window_class.lpfnWndProc = benchlab_render_window_proc;
    window_class.hInstance = instance;
    window_class.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    window_class.hCursor = LoadCursor(NULL, IDC_ARROW);
    window_class.lpszClassName = BENCHLAB_RENDER_WINDOW_CLASSA;
    g_benchlab_render_class = RegisterClassA(&window_class);
    if (g_benchlab_render_class == 0) {
        return 0;
    }

    ZeroMemory(&window_class, sizeof(window_class));
    window_class.style = CS_HREDRAW | CS_VREDRAW;
    window_class.lpfnWndProc = benchlab_info_window_proc;
    window_class.hInstance = instance;
    window_class.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    window_class.hCursor = LoadCursor(NULL, IDC_ARROW);
    window_class.lpszClassName = BENCHLAB_INFO_WINDOW_CLASSA;
    g_benchlab_info_class = RegisterClassA(&window_class);
    return g_benchlab_info_class != 0;
}

static HWND benchlab_create_window(benchlab_app *app)
{
    RECT rect;

    if (app == NULL) {
        return NULL;
    }

    rect.left = 0;
    rect.top = 0;
    rect.right = app->app_config.client_width;
    rect.bottom = app->app_config.client_height;
    AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, TRUE);

    return CreateWindowExA(
        0,
        BENCHLAB_WINDOW_CLASSA,
        BENCHLAB_APP_TITLEA,
        WS_OVERLAPPEDWINDOW | WS_VISIBLE | WS_CLIPCHILDREN,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        rect.right - rect.left,
        rect.bottom - rect.top,
        NULL,
        NULL,
        app->instance,
        app
    );
}

static HWND benchlab_create_render_window(benchlab_app *app)
{
    return CreateWindowExA(
        0,
        BENCHLAB_RENDER_WINDOW_CLASSA,
        NULL,
        WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
        0,
        0,
        1,
        1,
        app->main_window,
        NULL,
        app->instance,
        app
    );
}

static HWND benchlab_create_info_window(benchlab_app *app)
{
    return CreateWindowExA(
        0,
        BENCHLAB_INFO_WINDOW_CLASSA,
        NULL,
        WS_CHILD | WS_VISIBLE,
        0,
        0,
        1,
        1,
        app->main_window,
        NULL,
        app->instance,
        app
    );
}

static void benchlab_layout_windows(benchlab_app *app)
{
    RECT client_rect;
    int client_width;
    int client_height;
    int info_width;
    int render_width;

    if (app == NULL || app->main_window == NULL || app->render_window == NULL) {
        return;
    }

    GetClientRect(app->main_window, &client_rect);
    client_width = client_rect.right - client_rect.left;
    client_height = client_rect.bottom - client_rect.top;
    if (client_width < 1) {
        client_width = 1;
    }
    if (client_height < 1) {
        client_height = 1;
    }

    info_width = 0;
    if (app->app_config.overlay_enabled && app->info_window != NULL && client_width > BENCHLAB_INFO_PANEL_WIDTH + 200) {
        info_width = BENCHLAB_INFO_PANEL_WIDTH;
    }

    render_width = client_width - info_width;
    if (render_width < 1) {
        render_width = 1;
    }

    MoveWindow(app->render_window, 0, 0, render_width, client_height, TRUE);
    if (app->info_window != NULL) {
        if (info_width > 0) {
            MoveWindow(app->info_window, render_width, 0, info_width, client_height, TRUE);
            ShowWindow(app->info_window, SW_SHOWNOACTIVATE);
        } else {
            ShowWindow(app->info_window, SW_HIDE);
        }
    }

    benchlab_session_resize(app, render_width, client_height);
    InvalidateRect(app->render_window, NULL, TRUE);
    if (app->info_window != NULL && info_width > 0) {
        InvalidateRect(app->info_window, NULL, TRUE);
    }
}

static int benchlab_recreate_render_runtime(benchlab_app *app)
{
    HWND new_render_window;

    if (app == NULL || app->main_window == NULL) {
        return 0;
    }

    benchlab_session_destroy_runtime(app);
    if (app->render_window != NULL) {
        DestroyWindow(app->render_window);
        app->render_window = NULL;
    }

    new_render_window = benchlab_create_render_window(app);
    if (new_render_window == NULL) {
        return 0;
    }

    app->render_window = new_render_window;
    benchlab_layout_windows(app);
    if (!benchlab_session_create_runtime(app, app->render_window)) {
        return 0;
    }

    benchlab_update_window_title(app);
    if (app->info_window != NULL) {
        InvalidateRect(app->info_window, NULL, TRUE);
    }
    return 1;
}

static int benchlab_handle_renderer_request(benchlab_app *app, screensave_renderer_kind requested_kind)
{
    screensave_renderer_kind previous_requested_kind;
    int previous_renderer_request;

    if (app == NULL) {
        return 0;
    }

    if (app->requested_renderer_kind == requested_kind) {
        benchlab_update_menu_state(app);
        return 1;
    }

    previous_requested_kind = app->requested_renderer_kind;
    previous_renderer_request = app->app_config.renderer_request;
    app->requested_renderer_kind = requested_kind;
    app->app_config.renderer_request = (int)requested_kind;
    if (!benchlab_recreate_render_runtime(app)) {
        app->requested_renderer_kind = previous_requested_kind;
        app->app_config.renderer_request = previous_renderer_request;
        (void)benchlab_recreate_render_runtime(app);
        benchlab_show_message(
            app->main_window,
            app->diagnostics.last_text[0] != '\0'
                ? app->diagnostics.last_text
                : "BenchLab could not switch the requested renderer.",
            MB_OK | MB_ICONERROR
        );
        benchlab_update_menu_state(app);
        benchlab_update_window_title(app);
        return 0;
    }

    benchlab_update_menu_state(app);
    benchlab_update_window_title(app);
    return 1;
}

static int benchlab_handle_module_request(benchlab_app *app, const screensave_saver_module *module)
{
    const screensave_saver_module *previous_module;
    char previous_product_key[32];

    if (app == NULL || module == NULL) {
        return 0;
    }

    if (app->module == module) {
        benchlab_update_menu_state(app);
        return 1;
    }

    previous_module = app->module;
    lstrcpynA(previous_product_key, app->app_config.product_key, sizeof(previous_product_key));

    benchlab_session_destroy_runtime(app);
    benchlab_session_dispose_config(app);

    app->module = module;
    lstrcpynA(app->app_config.product_key, module->identity.product_key, sizeof(app->app_config.product_key));
    if (
        !benchlab_session_initialize_config(app) ||
        !benchlab_session_create_runtime(app, app->render_window)
    ) {
        benchlab_session_destroy_runtime(app);
        benchlab_session_dispose_config(app);

        app->module = previous_module;
        lstrcpynA(app->app_config.product_key, previous_product_key, sizeof(app->app_config.product_key));
        if (app->module != NULL) {
            (void)benchlab_session_initialize_config(app);
            (void)benchlab_session_create_runtime(app, app->render_window);
        }

        benchlab_show_message(
            app->main_window,
            app->diagnostics.last_text[0] != '\0'
                ? app->diagnostics.last_text
                : "BenchLab could not switch the active saver product.",
            MB_OK | MB_ICONERROR
        );
        benchlab_update_menu_state(app);
        benchlab_update_window_title(app);
        if (app->info_window != NULL) {
            InvalidateRect(app->info_window, NULL, FALSE);
        }
        return 0;
    }

    benchlab_update_menu_state(app);
    benchlab_update_window_title(app);
    if (app->render_window != NULL) {
        InvalidateRect(app->render_window, NULL, FALSE);
    }
    if (app->info_window != NULL) {
        InvalidateRect(app->info_window, NULL, FALSE);
    }
    return 1;
}

static int benchlab_handle_command(benchlab_app *app, WORD command_id)
{
    INT_PTR dialog_result;
    const screensave_saver_module *module;
    unsigned int module_index;

    if (app == NULL) {
        return 0;
    }

    if (
        command_id >= IDM_BENCHLAB_PRODUCT_FIRST &&
        command_id < IDM_BENCHLAB_PRODUCT_FIRST + benchlab_get_available_module_count()
    ) {
        module_index = (unsigned int)(command_id - IDM_BENCHLAB_PRODUCT_FIRST);
        module = benchlab_get_available_module(module_index);
        return benchlab_handle_module_request(app, module);
    }

    switch (command_id) {
    case IDM_BENCHLAB_RESTART:
        if (!benchlab_session_restart(app, 0)) {
            benchlab_show_message(app->main_window, app->diagnostics.last_text, MB_OK | MB_ICONERROR);
        }
        if (app->render_window != NULL) {
            InvalidateRect(app->render_window, NULL, FALSE);
        }
        return 1;

    case IDM_BENCHLAB_RESEED:
        if (!benchlab_session_restart(app, 1)) {
            benchlab_show_message(app->main_window, app->diagnostics.last_text, MB_OK | MB_ICONERROR);
        }
        if (app->render_window != NULL) {
            InvalidateRect(app->render_window, NULL, FALSE);
        }
        return 1;

    case IDM_BENCHLAB_TOGGLE_DETERMINISTIC:
        app->app_config.deterministic_mode = !app->app_config.deterministic_mode;
        if (!benchlab_session_restart(app, 0)) {
            benchlab_show_message(app->main_window, app->diagnostics.last_text, MB_OK | MB_ICONERROR);
        }
        benchlab_update_menu_state(app);
        benchlab_update_window_title(app);
        if (app->render_window != NULL) {
            InvalidateRect(app->render_window, NULL, FALSE);
        }
        if (app->info_window != NULL) {
            InvalidateRect(app->info_window, NULL, FALSE);
        }
        return 1;

    case IDM_BENCHLAB_TOGGLE_OVERLAY:
        app->app_config.overlay_enabled = !app->app_config.overlay_enabled;
        benchlab_layout_windows(app);
        benchlab_update_menu_state(app);
        return 1;

    case IDM_BENCHLAB_TOGGLE_PAUSE:
        app->paused = !app->paused;
        if (!app->paused) {
            benchlab_session_resume_clock(app);
        }
        benchlab_update_menu_state(app);
        benchlab_update_window_title(app);
        if (app->info_window != NULL) {
            InvalidateRect(app->info_window, NULL, FALSE);
        }
        return 1;

    case IDM_BENCHLAB_STEP_FRAME:
        if (app->paused) {
            benchlab_session_step_once(app);
            if (app->render_window != NULL) {
                InvalidateRect(app->render_window, NULL, FALSE);
            }
            if (app->info_window != NULL) {
                InvalidateRect(app->info_window, NULL, FALSE);
            }
        }
        return 1;

    case IDM_BENCHLAB_SAVER_SETTINGS:
        dialog_result = benchlab_session_show_saver_dialog(app);
        if (dialog_result == -1) {
            benchlab_show_message(
                app->main_window,
                app->diagnostics.last_text[0] != '\0'
                    ? app->diagnostics.last_text
                    : "The saver settings dialog could not be completed.",
                MB_OK | MB_ICONERROR
            );
        }
        benchlab_update_menu_state(app);
        benchlab_update_window_title(app);
        if (app->render_window != NULL) {
            InvalidateRect(app->render_window, NULL, FALSE);
        }
        if (app->info_window != NULL) {
            InvalidateRect(app->info_window, NULL, FALSE);
        }
        return 1;

    case IDM_BENCHLAB_RENDERER_AUTO:
        return benchlab_handle_renderer_request(app, SCREENSAVE_RENDERER_KIND_UNKNOWN);

    case IDM_BENCHLAB_RENDERER_GDI:
        return benchlab_handle_renderer_request(app, SCREENSAVE_RENDERER_KIND_GDI);

    case IDM_BENCHLAB_RENDERER_GL11:
        return benchlab_handle_renderer_request(app, SCREENSAVE_RENDERER_KIND_GL11);

    case IDM_BENCHLAB_RENDERER_GL21:
        return benchlab_handle_renderer_request(app, SCREENSAVE_RENDERER_KIND_GL21);

    case IDM_BENCHLAB_RENDERER_GL33:
        return benchlab_handle_renderer_request(app, SCREENSAVE_RENDERER_KIND_GL33);

    case IDM_BENCHLAB_RENDERER_GL46:
        return benchlab_handle_renderer_request(app, SCREENSAVE_RENDERER_KIND_GL46);

    case IDM_BENCHLAB_EXIT:
        DestroyWindow(app->main_window);
        return 1;
    }

    return 0;
}

static int benchlab_handle_keydown(benchlab_app *app, WPARAM key_code)
{
    switch (key_code) {
    case VK_F5:
        return benchlab_handle_command(app, IDM_BENCHLAB_RESTART);

    case VK_F6:
        return benchlab_handle_command(app, IDM_BENCHLAB_RESEED);

    case VK_F7:
        return benchlab_handle_command(app, IDM_BENCHLAB_TOGGLE_DETERMINISTIC);

    case VK_F8:
        return benchlab_handle_command(app, IDM_BENCHLAB_TOGGLE_PAUSE);

    case VK_F9:
        return benchlab_handle_command(app, IDM_BENCHLAB_STEP_FRAME);
    }

    return 0;
}

static LRESULT CALLBACK benchlab_render_window_proc(HWND window, UINT message, WPARAM wParam, LPARAM lParam)
{
    PAINTSTRUCT paint;
    RECT client_rect;
    benchlab_app *app;

    app = benchlab_get_window_app(window);

    switch (message) {
    case WM_NCCREATE:
        benchlab_attach_window_app(window, lParam);
        return TRUE;

    case WM_ERASEBKGND:
        return 1;

    case WM_KEYDOWN:
        if (app != NULL && app->main_window != NULL) {
            return SendMessageA(app->main_window, WM_KEYDOWN, wParam, lParam);
        }
        break;

    case WM_PAINT:
        BeginPaint(window, &paint);
        GetClientRect(window, &client_rect);
        if (app != NULL) {
            benchlab_session_render(app, paint.hdc);
        } else {
            FillRect(paint.hdc, &client_rect, (HBRUSH)GetStockObject(BLACK_BRUSH));
        }
        EndPaint(window, &paint);
        return 0;

    case WM_DESTROY:
        if (app != NULL && app->render_window == window) {
            app->render_window = NULL;
        }
        return 0;
    }

    return DefWindowProcA(window, message, wParam, lParam);
}

static LRESULT CALLBACK benchlab_info_window_proc(HWND window, UINT message, WPARAM wParam, LPARAM lParam)
{
    PAINTSTRUCT paint;
    RECT client_rect;
    benchlab_app *app;

    app = benchlab_get_window_app(window);

    switch (message) {
    case WM_NCCREATE:
        benchlab_attach_window_app(window, lParam);
        return TRUE;

    case WM_ERASEBKGND:
        return 1;

    case WM_PAINT:
        BeginPaint(window, &paint);
        GetClientRect(window, &client_rect);
        FillRect(paint.hdc, &client_rect, (HBRUSH)GetStockObject(BLACK_BRUSH));
        if (app != NULL) {
            benchlab_draw_overlay(paint.hdc, &client_rect, app);
        }
        EndPaint(window, &paint);
        return 0;

    case WM_DESTROY:
        if (app != NULL && app->info_window == window) {
            app->info_window = NULL;
        }
        return 0;
    }

    return DefWindowProcA(window, message, wParam, lParam);
}

static LRESULT CALLBACK benchlab_window_proc(HWND window, UINT message, WPARAM wParam, LPARAM lParam)
{
    PAINTSTRUCT paint;
    benchlab_app *app;

    app = benchlab_get_window_app(window);

    switch (message) {
    case WM_NCCREATE:
        benchlab_attach_window_app(window, lParam);
        app = benchlab_get_window_app(window);
        if (app != NULL) {
            app->main_window = window;
        }
        return TRUE;

    case WM_CREATE:
        if (app == NULL) {
            return -1;
        }

        app->menu = benchlab_create_menu();
        if (app->menu != NULL) {
            SetMenu(window, app->menu);
        }

        app->render_window = benchlab_create_render_window(app);
        app->info_window = benchlab_create_info_window(app);
        if (app->render_window == NULL || app->info_window == NULL) {
            return -1;
        }

        benchlab_layout_windows(app);
        benchlab_update_menu_state(app);
        benchlab_update_window_title(app);
        if (!benchlab_session_create_runtime(app, app->render_window)) {
            benchlab_show_message(
                window,
                app->diagnostics.last_text[0] != '\0'
                    ? app->diagnostics.last_text
                    : "BenchLab could not create the initial harness session.",
                MB_OK | MB_ICONERROR
            );
            return -1;
        }

        benchlab_update_window_title(app);
        SetTimer(window, BENCHLAB_TIMER_ID, BENCHLAB_TIMER_INTERVAL_MS, NULL);
        return 0;

    case WM_GETMINMAXINFO:
        if (lParam != 0) {
            MINMAXINFO *info;

            info = (MINMAXINFO *)lParam;
            info->ptMinTrackSize.x = 320;
            info->ptMinTrackSize.y = 240;
        }
        return 0;

    case WM_ERASEBKGND:
        return 1;

    case WM_SIZE:
        if (app != NULL && wParam != SIZE_MINIMIZED) {
            app->app_config.client_width = LOWORD(lParam);
            app->app_config.client_height = HIWORD(lParam);
            benchlab_app_config_clamp(&app->app_config);
            benchlab_layout_windows(app);
        }
        return 0;

    case WM_TIMER:
        if (app != NULL && wParam == BENCHLAB_TIMER_ID) {
            if (!app->paused) {
                benchlab_session_tick(app);
                if (app->render_window != NULL) {
                    InvalidateRect(app->render_window, NULL, FALSE);
                }
                if (app->info_window != NULL && app->app_config.overlay_enabled) {
                    InvalidateRect(app->info_window, NULL, FALSE);
                }
            }
        }
        return 0;

    case WM_KEYDOWN:
        if (app != NULL && benchlab_handle_keydown(app, wParam)) {
            return 0;
        }
        break;

    case WM_COMMAND:
        if (app != NULL && benchlab_handle_command(app, LOWORD(wParam))) {
            return 0;
        }
        break;

    case WM_PAINT:
        BeginPaint(window, &paint);
        EndPaint(window, &paint);
        return 0;

    case WM_DESTROY:
        KillTimer(window, BENCHLAB_TIMER_ID);
        if (app != NULL) {
            benchlab_session_destroy_runtime(app);
        }
        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProcA(window, message, wParam, lParam);
}

int benchlab_app_run(HINSTANCE instance, LPSTR command_line, int show_code)
{
    MSG message;
    int get_message_result;
    int result;
    benchlab_app app;

    ZeroMemory(&app, sizeof(app));
    app.instance = instance;
    app.command_line = command_line;
    app.show_code = show_code;

    screensave_diag_context_init(&app.diagnostics, SCREENSAVE_DIAG_LEVEL_DEBUG);
    benchlab_diag_attach(&app);
    benchlab_app_config_load(&app.app_config);
    benchlab_app_config_apply_command_line(&app, command_line);
    benchlab_app_config_clamp(&app.app_config);
    app.module = benchlab_find_target_module(app.app_config.product_key);
    if (app.module == NULL) {
        app.module = benchlab_get_target_module();
        if (app.module != NULL) {
            lstrcpynA(app.app_config.product_key, app.module->identity.product_key, sizeof(app.app_config.product_key));
        }
    }
    if (!screensave_saver_module_is_valid(app.module)) {
        benchlab_show_message(
            NULL,
            "BenchLab could not start because the selected saver module is invalid.",
            MB_OK | MB_ICONERROR
        );
        return 1;
    }
    app.requested_renderer_kind = (screensave_renderer_kind)app.app_config.renderer_request;

    if (!benchlab_session_initialize_config(&app)) {
        benchlab_show_message(
            NULL,
            "BenchLab could not initialize the saver configuration state.",
            MB_OK | MB_ICONERROR
        );
        return 1;
    }

    if (!benchlab_register_window_classes(instance)) {
        benchlab_session_dispose_config(&app);
        benchlab_show_message(
            NULL,
            "BenchLab could not register its window classes.",
            MB_OK | MB_ICONERROR
        );
        return 1;
    }

    if (benchlab_create_window(&app) == NULL) {
        benchlab_session_dispose_config(&app);
        benchlab_show_message(
            NULL,
            app.diagnostics.last_text[0] != '\0'
                ? app.diagnostics.last_text
                : "BenchLab could not create its main window.",
            MB_OK | MB_ICONERROR
        );
        return 1;
    }

    ShowWindow(app.main_window, show_code == 0 ? SW_SHOW : show_code);
    UpdateWindow(app.main_window);

    while ((get_message_result = GetMessageA(&message, NULL, 0, 0)) > 0) {
        TranslateMessage(&message);
        DispatchMessageA(&message);
    }

    result = 0;
    if (get_message_result < 0) {
        result = 1;
        benchlab_show_message(
            app.main_window,
            "BenchLab encountered a message-loop failure.",
            MB_OK | MB_ICONERROR
        );
    } else {
        result = (int)message.wParam;
    }

    benchlab_app_config_save(&app.app_config);
    benchlab_session_dispose_config(&app);
    return result;
}
