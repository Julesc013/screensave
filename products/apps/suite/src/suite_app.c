#include "suite_internal.h"

static ATOM g_suite_window_class = 0;
static ATOM g_suite_preview_class = 0;
static ATOM g_suite_info_class = 0;

static LRESULT CALLBACK suite_window_proc(HWND window, UINT message, WPARAM wParam, LPARAM lParam);
static LRESULT CALLBACK suite_preview_window_proc(HWND window, UINT message, WPARAM wParam, LPARAM lParam);
static LRESULT CALLBACK suite_info_window_proc(HWND window, UINT message, WPARAM wParam, LPARAM lParam);

static void suite_show_message(HWND owner_window, const char *text, UINT type)
{
    MessageBoxA(owner_window, text, SUITE_APP_TITLEA, type | MB_SETFOREGROUND);
}

static suite_app *suite_get_window_app(HWND window)
{
    return (suite_app *)GetWindowLongA(window, GWL_USERDATA);
}

static void suite_attach_window_app(HWND window, LPARAM lParam)
{
    CREATESTRUCTA *create_struct;
    suite_app *app;

    create_struct = (CREATESTRUCTA *)lParam;
    app = (suite_app *)create_struct->lpCreateParams;
    SetWindowLongA(window, GWL_USERDATA, (LONG)app);
}

static void suite_apply_control_font(HWND window)
{
    if (window != NULL) {
        SendMessageA(window, WM_SETFONT, (WPARAM)GetStockObject(DEFAULT_GUI_FONT), TRUE);
    }
}

static int suite_register_window_classes(HINSTANCE instance)
{
    WNDCLASSA window_class;

    if (g_suite_window_class != 0 && g_suite_preview_class != 0 && g_suite_info_class != 0) {
        return 1;
    }

    ZeroMemory(&window_class, sizeof(window_class));
    window_class.style = CS_HREDRAW | CS_VREDRAW;
    window_class.lpfnWndProc = suite_window_proc;
    window_class.hInstance = instance;
    window_class.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
    window_class.hCursor = LoadCursor(NULL, IDC_ARROW);
    window_class.lpszClassName = SUITE_WINDOW_CLASSA;
    g_suite_window_class = RegisterClassA(&window_class);
    if (g_suite_window_class == 0) {
        return 0;
    }

    ZeroMemory(&window_class, sizeof(window_class));
    window_class.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    window_class.lpfnWndProc = suite_preview_window_proc;
    window_class.hInstance = instance;
    window_class.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    window_class.hCursor = LoadCursor(NULL, IDC_ARROW);
    window_class.lpszClassName = SUITE_PREVIEW_WINDOW_CLASSA;
    g_suite_preview_class = RegisterClassA(&window_class);
    if (g_suite_preview_class == 0) {
        return 0;
    }

    ZeroMemory(&window_class, sizeof(window_class));
    window_class.style = CS_HREDRAW | CS_VREDRAW;
    window_class.lpfnWndProc = suite_info_window_proc;
    window_class.hInstance = instance;
    window_class.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
    window_class.hCursor = LoadCursor(NULL, IDC_ARROW);
    window_class.lpszClassName = SUITE_INFO_WINDOW_CLASSA;
    g_suite_info_class = RegisterClassA(&window_class);
    return g_suite_info_class != 0;
}

static HWND suite_create_label(
    suite_app *app,
    int control_id,
    const char *text
)
{
    HWND window;

    window = CreateWindowExA(
        0,
        "STATIC",
        text,
        WS_CHILD | WS_VISIBLE,
        0,
        0,
        1,
        1,
        app->main_window,
        (HMENU)(INT_PTR)control_id,
        app->instance,
        NULL
    );
    suite_apply_control_font(window);
    return window;
}

static HWND suite_create_combo(
    suite_app *app,
    int control_id
)
{
    HWND window;

    window = CreateWindowExA(
        WS_EX_CLIENTEDGE,
        "COMBOBOX",
        NULL,
        WS_CHILD | WS_VISIBLE | WS_TABSTOP | CBS_DROPDOWNLIST | WS_VSCROLL,
        0,
        0,
        1,
        160,
        app->main_window,
        (HMENU)(INT_PTR)control_id,
        app->instance,
        NULL
    );
    suite_apply_control_font(window);
    return window;
}

static HWND suite_create_button(
    suite_app *app,
    int control_id,
    const char *text,
    DWORD style
)
{
    HWND window;

    window = CreateWindowExA(
        0,
        "BUTTON",
        text,
        WS_CHILD | WS_VISIBLE | WS_TABSTOP | style,
        0,
        0,
        1,
        1,
        app->main_window,
        (HMENU)(INT_PTR)control_id,
        app->instance,
        NULL
    );
    suite_apply_control_font(window);
    return window;
}

static int suite_create_controls(suite_app *app)
{
    if (app == NULL) {
        return 0;
    }

    app->browser_window = CreateWindowExA(
        WS_EX_CLIENTEDGE,
        "LISTBOX",
        NULL,
        WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_TABSTOP | LBS_NOTIFY,
        0,
        0,
        1,
        1,
        app->main_window,
        (HMENU)(INT_PTR)IDC_SUITE_BROWSER,
        app->instance,
        NULL
    );
    suite_apply_control_font(app->browser_window);

    app->preview_window = CreateWindowExA(
        0,
        SUITE_PREVIEW_WINDOW_CLASSA,
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
    app->info_window = CreateWindowExA(
        WS_EX_CLIENTEDGE,
        SUITE_INFO_WINDOW_CLASSA,
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

    app->preset_label = suite_create_label(app, IDC_SUITE_PRESET_LABEL, "Preset");
    app->preset_combo = suite_create_combo(app, IDC_SUITE_PRESET);
    app->theme_label = suite_create_label(app, IDC_SUITE_THEME_LABEL, "Theme");
    app->theme_combo = suite_create_combo(app, IDC_SUITE_THEME);
    app->renderer_label = suite_create_label(app, IDC_SUITE_RENDERER_LABEL, "Renderer preference");
    app->renderer_combo = suite_create_combo(app, IDC_SUITE_RENDERER);
    app->randomize_check = suite_create_button(app, IDC_SUITE_RANDOMIZE, "Session randomization", BS_AUTOCHECKBOX);
    app->restart_button = suite_create_button(app, IDC_SUITE_RESTART, "Restart Preview", BS_PUSHBUTTON);
    app->windowed_button = suite_create_button(app, IDC_SUITE_WINDOWED, "Run Windowed", BS_PUSHBUTTON);
    app->settings_button = suite_create_button(app, IDC_SUITE_SETTINGS, "Settings...", BS_PUSHBUTTON);
    app->save_button = suite_create_button(app, IDC_SUITE_SAVE, "Apply", BS_PUSHBUTTON);
    app->reset_button = suite_create_button(app, IDC_SUITE_RESET, "Revert Changes", BS_PUSHBUTTON);

    if (
        app->browser_window == NULL ||
        app->preview_window == NULL ||
        app->info_window == NULL ||
        app->preset_label == NULL ||
        app->preset_combo == NULL ||
        app->theme_label == NULL ||
        app->theme_combo == NULL ||
        app->renderer_label == NULL ||
        app->renderer_combo == NULL ||
        app->randomize_check == NULL ||
        app->restart_button == NULL ||
        app->windowed_button == NULL ||
        app->settings_button == NULL ||
        app->save_button == NULL ||
        app->reset_button == NULL
    ) {
        return 0;
    }

    SendMessageA(app->renderer_combo, CB_ADDSTRING, 0U, (LPARAM)"Auto (best available)");
    SendMessageA(app->renderer_combo, CB_ADDSTRING, 0U, (LPARAM)"GDI floor");
    SendMessageA(app->renderer_combo, CB_ADDSTRING, 0U, (LPARAM)"OpenGL 1.1");
    SendMessageA(app->renderer_combo, CB_ADDSTRING, 0U, (LPARAM)"OpenGL 2.1");
    SendMessageA(app->renderer_combo, CB_ADDSTRING, 0U, (LPARAM)"OpenGL 3.3 (placeholder)");
    SendMessageA(app->renderer_combo, CB_ADDSTRING, 0U, (LPARAM)"OpenGL 4.6 (placeholder)");
    SendMessageA(app->renderer_combo, CB_SETCURSEL, 0U, 0L);
    return 1;
}

static void suite_layout_controls(suite_app *app)
{
    RECT client_rect;
    int client_width;
    int client_height;
    int margin;
    int browser_width;
    int right_left;
    int right_width;
    int preview_height;
    int label_height;
    int button_height;
    int controls_top;
    int info_top;
    int info_height;
    int column_width;
    int button_width;

    if (app == NULL || app->main_window == NULL) {
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

    margin = 10;
    browser_width = 260;
    if (client_width < 980) {
        browser_width = 220;
    }
    label_height = 16;
    button_height = 28;
    right_left = browser_width + (margin * 2);
    right_width = client_width - right_left - margin;
    if (right_width < 320) {
        right_width = 320;
    }
    preview_height = (client_height * 2) / 5;
    if (preview_height < 180) {
        preview_height = 180;
    }
    controls_top = margin + preview_height + margin;
    info_top = controls_top + 108;
    info_height = client_height - info_top - margin;
    if (info_height < 120) {
        info_height = 120;
    }
    column_width = (right_width - margin * 2) / 3;
    if (column_width < 120) {
        column_width = 120;
    }
    button_width = (right_width - margin * 4) / 5;
    if (button_width < 92) {
        button_width = 92;
    }

    MoveWindow(app->browser_window, margin, margin, browser_width, client_height - margin * 2, TRUE);
    MoveWindow(app->preview_window, right_left, margin, right_width, preview_height, TRUE);
    MoveWindow(app->info_window, right_left, info_top, right_width, info_height, TRUE);

    MoveWindow(app->preset_label, right_left, controls_top, column_width, label_height, TRUE);
    MoveWindow(app->theme_label, right_left + column_width + margin, controls_top, column_width, label_height, TRUE);
    MoveWindow(
        app->renderer_label,
        right_left + (column_width + margin) * 2,
        controls_top,
        column_width,
        label_height,
        TRUE
    );

    MoveWindow(app->preset_combo, right_left, controls_top + label_height + 2, column_width, 200, TRUE);
    MoveWindow(
        app->theme_combo,
        right_left + column_width + margin,
        controls_top + label_height + 2,
        column_width,
        200,
        TRUE
    );
    MoveWindow(
        app->renderer_combo,
        right_left + (column_width + margin) * 2,
        controls_top + label_height + 2,
        column_width,
        200,
        TRUE
    );

    MoveWindow(app->randomize_check, right_left, controls_top + 40, right_width, 20, TRUE);

    MoveWindow(app->restart_button, right_left, controls_top + 70, button_width, button_height, TRUE);
    MoveWindow(
        app->windowed_button,
        right_left + button_width + margin,
        controls_top + 70,
        button_width,
        button_height,
        TRUE
    );
    MoveWindow(
        app->settings_button,
        right_left + (button_width + margin) * 2,
        controls_top + 70,
        button_width,
        button_height,
        TRUE
    );
    MoveWindow(
        app->save_button,
        right_left + (button_width + margin) * 3,
        controls_top + 70,
        button_width,
        button_height,
        TRUE
    );
    MoveWindow(
        app->reset_button,
        right_left + (button_width + margin) * 4,
        controls_top + 70,
        button_width,
        button_height,
        TRUE
    );
}

static HWND suite_create_window(suite_app *app)
{
    RECT rect;

    if (app == NULL) {
        return NULL;
    }

    rect.left = 0;
    rect.top = 0;
    rect.right = app->app_config.client_width;
    rect.bottom = app->app_config.client_height;
    AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, FALSE);

    return CreateWindowExA(
        0,
        SUITE_WINDOW_CLASSA,
        SUITE_APP_TITLEA,
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

static int suite_handle_control_command(suite_app *app, WORD command_id, WORD notify_code)
{
    LRESULT selection;
    LRESULT item_data;

    if (app == NULL) {
        return 0;
    }

    if (command_id == IDC_SUITE_BROWSER && notify_code == LBN_SELCHANGE) {
        selection = SendMessageA(app->browser_window, LB_GETCURSEL, 0U, 0L);
        if (selection != LB_ERR) {
            item_data = SendMessageA(app->browser_window, LB_GETITEMDATA, (WPARAM)selection, 0L);
            if (item_data != LB_ERR) {
                (void)suite_select_entry(app, (unsigned int)item_data);
            }
        }
        return 1;
    }

    if (app->syncing_controls) {
        return 1;
    }

    if (command_id == IDC_SUITE_PRESET && notify_code == CBN_SELCHANGE) {
        return suite_handle_preset_combo_change(app);
    }
    if (command_id == IDC_SUITE_THEME && notify_code == CBN_SELCHANGE) {
        return suite_handle_theme_combo_change(app);
    }
    if (command_id == IDC_SUITE_RENDERER && notify_code == CBN_SELCHANGE) {
        return suite_handle_renderer_combo_change(app);
    }

    switch (command_id) {
    case IDC_SUITE_RANDOMIZE:
        if (notify_code == BN_CLICKED) {
            return suite_handle_randomization_toggle(app);
        }
        break;

    case IDC_SUITE_RESTART:
        if (notify_code == BN_CLICKED) {
            return suite_preview_restart(app, 1);
        }
        break;

    case IDC_SUITE_WINDOWED:
        if (notify_code == BN_CLICKED) {
            if (!suite_open_windowed_run(app)) {
                suite_show_message(
                    app->main_window,
                    app->diagnostics.last_text[0] != '\0'
                        ? app->diagnostics.last_text
                        : "Suite could not start the detached windowed run.",
                    MB_OK | MB_ICONERROR
                );
            }
            return 1;
        }
        break;

    case IDC_SUITE_SETTINGS:
        if (notify_code == BN_CLICKED) {
            (void)suite_show_selected_settings_dialog(app);
            return 1;
        }
        break;

    case IDC_SUITE_SAVE:
        if (notify_code == BN_CLICKED) {
            return suite_save_working_config(app);
        }
        break;

    case IDC_SUITE_RESET:
        if (notify_code == BN_CLICKED) {
            suite_reset_working_config(app);
            return 1;
        }
        break;
    }

    return 0;
}

static LRESULT CALLBACK suite_preview_window_proc(HWND window, UINT message, WPARAM wParam, LPARAM lParam)
{
    PAINTSTRUCT paint;
    RECT client_rect;
    suite_app *app;

    app = suite_get_window_app(window);

    switch (message) {
    case WM_NCCREATE:
        suite_attach_window_app(window, lParam);
        return TRUE;

    case WM_ERASEBKGND:
        return 1;

    case WM_SIZE:
        if (app != NULL && wParam != SIZE_MINIMIZED) {
            suite_preview_resize(app, LOWORD(lParam), HIWORD(lParam));
        }
        return 0;

    case WM_PAINT:
        BeginPaint(window, &paint);
        GetClientRect(window, &client_rect);
        if (app != NULL) {
            suite_preview_render(app, paint.hdc);
        } else {
            FillRect(paint.hdc, &client_rect, (HBRUSH)GetStockObject(BLACK_BRUSH));
        }
        EndPaint(window, &paint);
        return 0;

    case WM_DESTROY:
        if (app != NULL && app->preview_window == window) {
            app->preview_window = NULL;
        }
        return 0;
    }

    return DefWindowProcA(window, message, wParam, lParam);
}

static LRESULT CALLBACK suite_info_window_proc(HWND window, UINT message, WPARAM wParam, LPARAM lParam)
{
    PAINTSTRUCT paint;
    RECT client_rect;
    suite_app *app;

    app = suite_get_window_app(window);

    switch (message) {
    case WM_NCCREATE:
        suite_attach_window_app(window, lParam);
        return TRUE;

    case WM_ERASEBKGND:
        return 1;

    case WM_PAINT:
        BeginPaint(window, &paint);
        GetClientRect(window, &client_rect);
        if (app != NULL) {
            suite_draw_info(paint.hdc, &client_rect, app);
        } else {
            FillRect(paint.hdc, &client_rect, (HBRUSH)(COLOR_BTNFACE + 1));
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

static LRESULT CALLBACK suite_window_proc(HWND window, UINT message, WPARAM wParam, LPARAM lParam)
{
    PAINTSTRUCT paint;
    suite_app *app;

    app = suite_get_window_app(window);

    switch (message) {
    case WM_NCCREATE:
        suite_attach_window_app(window, lParam);
        app = suite_get_window_app(window);
        if (app != NULL) {
            app->main_window = window;
        }
        return TRUE;

    case WM_CREATE:
        if (app == NULL || !suite_create_controls(app)) {
            return -1;
        }

        suite_layout_controls(app);
        suite_populate_browser(app);
        if (app->catalog_count > 0U) {
            int selected_index;

            selected_index = suite_find_catalog_index(app, app->app_config.product_key);
            if (selected_index < 0) {
                selected_index = 0;
            }
            if (!suite_select_entry(app, (unsigned int)selected_index)) {
                suite_show_message(
                    window,
                    "Suite could not initialize the selected saver entry.",
                    MB_OK | MB_ICONERROR
                );
                return -1;
            }
        }

        SetTimer(window, SUITE_TIMER_ID, SUITE_TIMER_INTERVAL_MS, NULL);
        return 0;

    case WM_GETMINMAXINFO:
        if (lParam != 0) {
            MINMAXINFO *info;

            info = (MINMAXINFO *)lParam;
            info->ptMinTrackSize.x = 920;
            info->ptMinTrackSize.y = 640;
        }
        return 0;

    case WM_ERASEBKGND:
        return 1;

    case WM_SIZE:
        if (app != NULL && wParam != SIZE_MINIMIZED) {
            app->app_config.client_width = LOWORD(lParam);
            app->app_config.client_height = HIWORD(lParam);
            suite_app_config_clamp(&app->app_config);
            suite_layout_controls(app);
        }
        return 0;

    case WM_TIMER:
        if (app != NULL && wParam == SUITE_TIMER_ID) {
            suite_preview_tick(app);
            if (app->preview_window != NULL) {
                InvalidateRect(app->preview_window, NULL, FALSE);
            }
            if (app->info_window != NULL) {
                InvalidateRect(app->info_window, NULL, FALSE);
            }
        }
        return 0;

    case WM_COMMAND:
        if (app != NULL && suite_handle_control_command(app, LOWORD(wParam), HIWORD(wParam))) {
            return 0;
        }
        break;

    case WM_PAINT:
        BeginPaint(window, &paint);
        EndPaint(window, &paint);
        return 0;

    case WM_DESTROY:
        KillTimer(window, SUITE_TIMER_ID);
        if (app != NULL) {
            if (app->detached_run != NULL && app->detached_run->window != NULL) {
                DestroyWindow(app->detached_run->window);
            }
            suite_preview_stop(app);
        }
        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProcA(window, message, wParam, lParam);
}

int suite_app_run(HINSTANCE instance, LPSTR command_line, int show_code)
{
    MSG message;
    int get_message_result;
    int result;
    suite_app app;

    ZeroMemory(&app, sizeof(app));
    app.instance = instance;
    app.command_line = command_line;
    app.show_code = show_code;
    app.selected_index = 0U;

    screensave_diag_context_init(&app.diagnostics, SCREENSAVE_DIAG_LEVEL_DEBUG);
    suite_app_config_load(&app.app_config);
    suite_app_config_clamp(&app.app_config);

    if (!suite_catalog_build(&app)) {
        suite_show_message(
            NULL,
            "Suite could not build the saver catalog from the current product line.",
            MB_OK | MB_ICONERROR
        );
        return 1;
    }

    if (!suite_register_window_classes(instance)) {
        suite_show_message(
            NULL,
            "Suite could not register its window classes.",
            MB_OK | MB_ICONERROR
        );
        return 1;
    }

    if (suite_create_window(&app) == NULL) {
        suite_show_message(
            NULL,
            app.diagnostics.last_text[0] != '\0'
                ? app.diagnostics.last_text
                : "Suite could not create its main window.",
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

    if (get_message_result < 0) {
        result = 1;
        suite_show_message(
            app.main_window,
            "Suite encountered a message-loop failure.",
            MB_OK | MB_ICONERROR
        );
    } else {
        result = (int)message.wParam;
    }

    suite_app_config_save(&app.app_config);
    suite_dispose_selected_config(&app);
    return result;
}
