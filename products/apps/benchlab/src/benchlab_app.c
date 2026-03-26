#include "benchlab_internal.h"

static ATOM g_benchlab_window_class = 0;

static LRESULT CALLBACK benchlab_window_proc(HWND window, UINT message, WPARAM wParam, LPARAM lParam);

static void benchlab_show_message(HWND owner, const char *text, UINT type)
{
    MessageBoxA(owner, text, BENCHLAB_APP_TITLEA, type | MB_SETFOREGROUND);
}

static benchlab_app *benchlab_get_window_app(HWND window)
{
    return (benchlab_app *)GetWindowLongA(window, GWL_USERDATA);
}

static HMENU benchlab_create_menu(void)
{
    HMENU root_menu;
    HMENU run_menu;
    HMENU options_menu;

    root_menu = CreateMenu();
    run_menu = CreatePopupMenu();
    options_menu = CreatePopupMenu();
    if (root_menu == NULL || run_menu == NULL || options_menu == NULL) {
        if (options_menu != NULL) {
            DestroyMenu(options_menu);
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

    AppendMenuA(options_menu, MF_STRING, IDM_BENCHLAB_TOGGLE_DETERMINISTIC, "&Deterministic Mode\tF7");
    AppendMenuA(options_menu, MF_STRING, IDM_BENCHLAB_TOGGLE_OVERLAY, "&Overlay");
    AppendMenuA(options_menu, MF_STRING, IDM_BENCHLAB_SAVER_SETTINGS, "Saver &Settings...");

    AppendMenuA(root_menu, MF_POPUP, (UINT_PTR)run_menu, "&Run");
    AppendMenuA(root_menu, MF_POPUP, (UINT_PTR)options_menu, "&Options");
    return root_menu;
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
    EnableMenuItem(
        app->menu,
        IDM_BENCHLAB_STEP_FRAME,
        MF_BYCOMMAND | (app->paused ? MF_ENABLED : MF_GRAYED)
    );
    DrawMenuBar(app->main_window);
}

static void benchlab_update_window_title(benchlab_app *app)
{
    char title[192];

    if (app == NULL || app->main_window == NULL || app->module == NULL) {
        return;
    }

    title[0] = '\0';
    lstrcpyA(title, BENCHLAB_APP_TITLEA);
    lstrcatA(title, " - ");
    lstrcatA(title, app->module->identity.display_name);
    if (app->app_config.deterministic_mode) {
        lstrcatA(title, " [deterministic]");
    }
    if (app->paused) {
        lstrcatA(title, " [paused]");
    }

    SetWindowTextA(app->main_window, title);
}

static int benchlab_handle_command(benchlab_app *app, WORD command_id)
{
    INT_PTR dialog_result;

    if (app == NULL) {
        return 0;
    }

    switch (command_id) {
    case IDM_BENCHLAB_RESTART:
        if (!benchlab_session_restart(app, 0)) {
            benchlab_show_message(app->main_window, app->diagnostics.last_text, MB_OK | MB_ICONERROR);
        }
        InvalidateRect(app->main_window, NULL, FALSE);
        return 1;

    case IDM_BENCHLAB_RESEED:
        if (!benchlab_session_restart(app, 1)) {
            benchlab_show_message(app->main_window, app->diagnostics.last_text, MB_OK | MB_ICONERROR);
        }
        InvalidateRect(app->main_window, NULL, FALSE);
        return 1;

    case IDM_BENCHLAB_TOGGLE_DETERMINISTIC:
        app->app_config.deterministic_mode = !app->app_config.deterministic_mode;
        if (!benchlab_session_restart(app, 0)) {
            benchlab_show_message(app->main_window, app->diagnostics.last_text, MB_OK | MB_ICONERROR);
        }
        benchlab_update_menu_state(app);
        benchlab_update_window_title(app);
        InvalidateRect(app->main_window, NULL, FALSE);
        return 1;

    case IDM_BENCHLAB_TOGGLE_OVERLAY:
        app->app_config.overlay_enabled = !app->app_config.overlay_enabled;
        benchlab_update_menu_state(app);
        InvalidateRect(app->main_window, NULL, FALSE);
        return 1;

    case IDM_BENCHLAB_TOGGLE_PAUSE:
        app->paused = !app->paused;
        if (!app->paused) {
            benchlab_session_resume_clock(app);
        }
        benchlab_update_menu_state(app);
        benchlab_update_window_title(app);
        InvalidateRect(app->main_window, NULL, FALSE);
        return 1;

    case IDM_BENCHLAB_STEP_FRAME:
        if (app->paused) {
            benchlab_session_step_once(app);
            InvalidateRect(app->main_window, NULL, FALSE);
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
        InvalidateRect(app->main_window, NULL, FALSE);
        return 1;

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

static int benchlab_register_window_class(HINSTANCE instance)
{
    WNDCLASSA window_class;

    if (g_benchlab_window_class != 0) {
        return 1;
    }

    ZeroMemory(&window_class, sizeof(window_class));
    window_class.style = CS_HREDRAW | CS_VREDRAW;
    window_class.lpfnWndProc = benchlab_window_proc;
    window_class.hInstance = instance;
    window_class.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    window_class.hCursor = LoadCursor(NULL, IDC_ARROW);
    window_class.lpszClassName = BENCHLAB_WINDOW_CLASSA;

    g_benchlab_window_class = RegisterClassA(&window_class);
    return g_benchlab_window_class != 0;
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
        WS_OVERLAPPEDWINDOW | WS_VISIBLE,
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

static LRESULT CALLBACK benchlab_window_proc(HWND window, UINT message, WPARAM wParam, LPARAM lParam)
{
    CREATESTRUCTA *create_struct;
    PAINTSTRUCT paint;
    RECT client_rect;
    benchlab_app *app;

    app = benchlab_get_window_app(window);

    switch (message) {
    case WM_NCCREATE:
        create_struct = (CREATESTRUCTA *)lParam;
        app = (benchlab_app *)create_struct->lpCreateParams;
        SetWindowLongA(window, GWL_USERDATA, (LONG)app);
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

        benchlab_update_menu_state(app);
        benchlab_update_window_title(app);
        if (!benchlab_session_create_runtime(app, window)) {
            benchlab_show_message(
                window,
                app->diagnostics.last_text[0] != '\0'
                    ? app->diagnostics.last_text
                    : "BenchLab could not create the initial harness session.",
                MB_OK | MB_ICONERROR
            );
            return -1;
        }

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
            benchlab_session_resize(app, LOWORD(lParam), HIWORD(lParam));
            InvalidateRect(window, NULL, TRUE);
        }
        return 0;

    case WM_TIMER:
        if (app != NULL && wParam == BENCHLAB_TIMER_ID) {
            if (!app->paused) {
                benchlab_session_tick(app);
                InvalidateRect(window, NULL, FALSE);
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
        GetClientRect(window, &client_rect);
        if (app != NULL) {
            benchlab_session_render(app, paint.hdc);
            if (app->app_config.overlay_enabled) {
                benchlab_draw_overlay(paint.hdc, &client_rect, app);
            }
        } else {
            FillRect(paint.hdc, &client_rect, (HBRUSH)GetStockObject(BLACK_BRUSH));
        }
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
    app.module = benchlab_get_target_module();
    app.requested_renderer_kind = SCREENSAVE_RENDERER_KIND_GDI;

    if (!screensave_saver_module_is_valid(app.module)) {
        benchlab_show_message(
            NULL,
            "BenchLab could not start because the active saver module is invalid.",
            MB_OK | MB_ICONERROR
        );
        return 1;
    }

    screensave_diag_context_init(&app.diagnostics, SCREENSAVE_DIAG_LEVEL_DEBUG);
    benchlab_diag_attach(&app);
    benchlab_app_config_load(&app.app_config);
    benchlab_app_config_apply_command_line(&app, command_line);
    benchlab_app_config_clamp(&app.app_config);

    if (!benchlab_session_initialize_config(&app)) {
        benchlab_show_message(
            NULL,
            "BenchLab could not initialize the saver configuration state.",
            MB_OK | MB_ICONERROR
        );
        return 1;
    }

    if (!benchlab_register_window_class(instance)) {
        benchlab_session_dispose_config(&app);
        benchlab_show_message(
            NULL,
            "BenchLab could not register its window class.",
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
