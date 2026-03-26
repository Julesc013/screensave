#include "scr_internal.h"
#include "../../core/rng/rng_internal.h"

static ATOM g_scr_window_class = 0;

static LRESULT CALLBACK scr_window_proc(HWND window, UINT message, WPARAM wParam, LPARAM lParam);

static void scr_request_exit(scr_host_context *context, HWND window)
{
    if (context == NULL || context->preview_mode || context->exit_pending) {
        return;
    }

    context->exit_pending = 1;
    DestroyWindow(window);
}

static void scr_get_client_size(HWND window, screensave_sizei *size)
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

static unsigned long scr_resolve_session_seed(const scr_host_context *context)
{
    unsigned long fallback_seed;

    if (context == NULL || context->module == NULL) {
        return 0UL;
    }

    fallback_seed = (unsigned long)GetTickCount();
    if (context->settings.common.use_deterministic_seed) {
        fallback_seed = context->settings.common.deterministic_seed;
    }

    return screensave_rng_seed_from_text(context->module->identity.product_key, fallback_seed);
}

static void scr_prepare_runtime_state(scr_host_context *context)
{
    screensave_rng_state seed_rng;

    if (context == NULL) {
        return;
    }

    screensave_timebase_reset(&context->timebase);
    screensave_timebase_sample(&context->timebase, &context->clock);

    context->session_seed.base_seed = scr_resolve_session_seed(context);
    screensave_rng_seed(&seed_rng, context->session_seed.base_seed);
    context->session_seed.stream_seed = screensave_rng_next_u32(&seed_rng);
    context->session_seed.deterministic = context->settings.common.use_deterministic_seed != 0;
}

static void scr_build_saver_environment(
    scr_host_context *context,
    const screensave_sizei *drawable_size,
    screensave_saver_environment *environment
)
{
    screensave_renderer_info renderer_info;

    if (context == NULL || environment == NULL) {
        return;
    }

    ZeroMemory(environment, sizeof(*environment));
    environment->mode = context->mode;
    environment->clock = context->clock;
    environment->seed = context->session_seed;
    environment->config_binding = &context->config_binding;
    environment->renderer = context->renderer;
    environment->diagnostics = &context->diagnostics;

    if (drawable_size != NULL) {
        environment->drawable_size = *drawable_size;
    } else if (context->renderer != NULL) {
        screensave_renderer_get_info(context->renderer, &renderer_info);
        environment->drawable_size = renderer_info.drawable_size;
    } else {
        environment->drawable_size.width = 1;
        environment->drawable_size.height = 1;
    }
}

static int scr_create_renderer_and_session(scr_host_context *context, HWND window)
{
    screensave_sizei drawable_size;
    screensave_saver_environment environment;

    if (context == NULL) {
        return 0;
    }

    scr_get_client_size(window, &drawable_size);
    if (!screensave_renderer_create_for_window(
            SCREENSAVE_RENDERER_KIND_GDI,
            window,
            &drawable_size,
            &context->diagnostics,
            &context->renderer
        )) {
        scr_emit_host_diagnostic(
            context,
            SCREENSAVE_DIAG_LEVEL_ERROR,
            2305UL,
            "scr_create_renderer_and_session",
            "The mandatory GDI renderer could not be initialized."
        );
        return 0;
    }

    if (context->module->callbacks != NULL && context->module->callbacks->create_session != NULL) {
        scr_build_saver_environment(context, &drawable_size, &environment);
        if (!context->module->callbacks->create_session(context->module, &context->session, &environment)) {
            scr_emit_host_diagnostic(
                context,
                SCREENSAVE_DIAG_LEVEL_ERROR,
                2306UL,
                "scr_create_renderer_and_session",
                "The saver session could not be created."
            );
            screensave_renderer_shutdown(context->renderer);
            context->renderer = NULL;
            return 0;
        }
    }

    return 1;
}

static void scr_resize_renderer_and_session(scr_host_context *context, HWND window)
{
    screensave_sizei drawable_size;
    screensave_saver_environment environment;

    if (context == NULL || context->renderer == NULL) {
        return;
    }

    scr_get_client_size(window, &drawable_size);
    if (!screensave_renderer_resize_for_window(context->renderer, &drawable_size)) {
        scr_emit_host_diagnostic(
            context,
            SCREENSAVE_DIAG_LEVEL_WARNING,
            2307UL,
            "scr_resize_renderer_and_session",
            "The GDI renderer could not resize its backbuffer."
        );
        return;
    }

    if (
        context->session != NULL &&
        context->module->callbacks != NULL &&
        context->module->callbacks->resize_session != NULL
    ) {
        scr_build_saver_environment(context, &drawable_size, &environment);
        context->module->callbacks->resize_session(context->session, &environment);
    }
}

static void scr_destroy_renderer_and_session(scr_host_context *context)
{
    if (context == NULL) {
        return;
    }

    if (
        context->session != NULL &&
        context->module->callbacks != NULL &&
        context->module->callbacks->destroy_session != NULL
    ) {
        context->module->callbacks->destroy_session(context->session);
        context->session = NULL;
    }

    if (context->renderer != NULL) {
        screensave_renderer_shutdown(context->renderer);
        context->renderer = NULL;
    }
}

static void scr_render_black_frame(scr_host_context *context)
{
    screensave_renderer_info renderer_info;
    screensave_frame_info frame_info;
    screensave_color background;

    if (context == NULL || context->renderer == NULL) {
        return;
    }

    screensave_renderer_get_info(context->renderer, &renderer_info);
    frame_info.drawable_size = renderer_info.drawable_size;
    frame_info.frame_index = context->clock.frame_index;
    frame_info.elapsed_millis = context->clock.elapsed_millis;
    frame_info.delta_millis = context->clock.delta_millis;

    if (!screensave_renderer_begin_frame(context->renderer, &frame_info)) {
        return;
    }

    background.red = 0;
    background.green = 0;
    background.blue = 0;
    background.alpha = 255;
    screensave_renderer_clear(context->renderer, background);
    (void)screensave_renderer_end_frame(context->renderer);
}

static int scr_register_window_class(HINSTANCE instance)
{
    WNDCLASSA window_class;

    if (g_scr_window_class != 0) {
        return 1;
    }

    ZeroMemory(&window_class, sizeof(window_class));
    window_class.style = CS_HREDRAW | CS_VREDRAW;
    window_class.lpfnWndProc = scr_window_proc;
    window_class.hInstance = instance;
    window_class.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    window_class.lpszClassName = SCR_HOST_WINDOW_CLASSA;

    g_scr_window_class = RegisterClassA(&window_class);
    return g_scr_window_class != 0;
}

static void scr_draw_overlay(HDC dc, const RECT *client_rect, const scr_host_context *context)
{
    RECT text_rect;
    char overlay[512];

    text_rect = *client_rect;
    text_rect.left += 8;
    text_rect.top += 8;

    scr_build_overlay_text(context, overlay, sizeof(overlay));
    SetBkMode(dc, TRANSPARENT);
    SetTextColor(dc, RGB(0, 255, 0));
    DrawTextA(dc, overlay, -1, &text_rect, DT_LEFT | DT_NOPREFIX | DT_TOP);
}

static LRESULT CALLBACK scr_window_proc(HWND window, UINT message, WPARAM wParam, LPARAM lParam)
{
    CREATESTRUCTA *create_struct;
    PAINTSTRUCT paint;
    HDC dc;
    RECT client_rect;
    POINT cursor;
    scr_host_context *context;
    int dx;
    int dy;

    context = (scr_host_context *)GetWindowLongA(window, GWL_USERDATA);

    switch (message) {
    case WM_NCCREATE:
        create_struct = (CREATESTRUCTA *)lParam;
        context = (scr_host_context *)create_struct->lpCreateParams;
        SetWindowLongA(window, GWL_USERDATA, (LONG)context);
        context->main_window = window;
        return TRUE;

    case WM_CREATE:
        if (context != NULL) {
            context->preview_mode = context->mode == SCREENSAVE_SESSION_MODE_PREVIEW;
            scr_prepare_runtime_state(context);
            if (!scr_create_renderer_and_session(context, window)) {
                return -1;
            }
            GetCursorPos(&context->initial_cursor);
            context->timer_id = SetTimer(window, SCR_TIMER_ID, SCR_TIMER_INTERVAL_MS, NULL);
            if (context->timer_id == 0) {
                scr_emit_host_diagnostic(
                    context,
                    SCREENSAVE_DIAG_LEVEL_ERROR,
                    2301UL,
                    "scr_window_proc",
                    "The host timer could not be created."
                );
                scr_destroy_renderer_and_session(context);
                return -1;
            }
        }
        return 0;

    case WM_ERASEBKGND:
        return 1;

    case WM_TIMER:
        if (context != NULL && wParam == SCR_TIMER_ID) {
            screensave_timebase_sample(&context->timebase, &context->clock);
            if (
                context->session != NULL &&
                context->module->callbacks != NULL &&
                context->module->callbacks->step_session != NULL
            ) {
                screensave_saver_environment environment;

                scr_build_saver_environment(context, NULL, &environment);
                context->module->callbacks->step_session(context->session, &environment);
            }
            InvalidateRect(window, NULL, FALSE);
        }
        return 0;

    case WM_SIZE:
        if (context != NULL) {
            scr_resize_renderer_and_session(context, window);
        }
        InvalidateRect(window, NULL, TRUE);
        return 0;

    case WM_MOUSEMOVE:
        if (context != NULL && !context->preview_mode) {
            GetCursorPos(&cursor);
            dx = cursor.x - context->initial_cursor.x;
            dy = cursor.y - context->initial_cursor.y;
            if (dx < 0) {
                dx = -dx;
            }
            if (dy < 0) {
                dy = -dy;
            }
            if (dx > SCR_EXIT_MOUSE_DELTA || dy > SCR_EXIT_MOUSE_DELTA) {
                scr_request_exit(context, window);
            }
        }
        return 0;

    case WM_LBUTTONDOWN:
    case WM_MBUTTONDOWN:
    case WM_RBUTTONDOWN:
    case WM_KEYDOWN:
    case WM_SYSKEYDOWN:
        if (context != NULL && !context->preview_mode) {
            scr_request_exit(context, window);
            return 0;
        }
        break;

    case WM_ACTIVATEAPP:
        if (context != NULL && !context->preview_mode && wParam == FALSE) {
            scr_request_exit(context, window);
            return 0;
        }
        break;

    case WM_SETCURSOR:
        if (context != NULL && !context->preview_mode && LOWORD(lParam) == HTCLIENT) {
            SetCursor(NULL);
            return TRUE;
        }
        break;

    case WM_CLOSE:
        DestroyWindow(window);
        return 0;

    case WM_PAINT:
        dc = BeginPaint(window, &paint);
        GetClientRect(window, &client_rect);
        if (context != NULL && context->renderer != NULL) {
            screensave_renderer_set_present_dc(context->renderer, dc);
            if (
                context->session != NULL &&
                context->module->callbacks != NULL &&
                context->module->callbacks->render_session != NULL
            ) {
                screensave_renderer_info renderer_info;
                screensave_frame_info frame_info;
                screensave_saver_environment environment;

                screensave_renderer_get_info(context->renderer, &renderer_info);
                frame_info.drawable_size = renderer_info.drawable_size;
                frame_info.frame_index = context->clock.frame_index;
                frame_info.elapsed_millis = context->clock.elapsed_millis;
                frame_info.delta_millis = context->clock.delta_millis;

                if (screensave_renderer_begin_frame(context->renderer, &frame_info)) {
                    scr_build_saver_environment(context, &renderer_info.drawable_size, &environment);
                    context->module->callbacks->render_session(context->session, &environment);
                    (void)screensave_renderer_end_frame(context->renderer);
                } else {
                    scr_emit_host_diagnostic(
                        context,
                        SCREENSAVE_DIAG_LEVEL_WARNING,
                        2308UL,
                        "scr_window_proc",
                        "The renderer could not begin a saver-render frame."
                    );
                }
            } else if (context->session == NULL) {
                scr_render_validation_scene(context);
            } else {
                scr_render_black_frame(context);
            }
            screensave_renderer_clear_present_dc(context->renderer);
        } else {
            FillRect(dc, &client_rect, (HBRUSH)GetStockObject(BLACK_BRUSH));
        }
        if (context != NULL && context->settings.common.diagnostics_overlay_enabled) {
            scr_draw_overlay(dc, &client_rect, context);
        }
        EndPaint(window, &paint);
        return 0;

    case WM_DESTROY:
        if (context != NULL && context->timer_id != 0) {
            KillTimer(window, context->timer_id);
            context->timer_id = 0;
        }
        if (context != NULL) {
            scr_destroy_renderer_and_session(context);
        }
        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProcA(window, message, wParam, lParam);
}

static HWND scr_create_window(scr_host_context *context)
{
    RECT rect;
    DWORD style;
    DWORD extended_style;

    ZeroMemory(&rect, sizeof(rect));

    if (context->mode == SCREENSAVE_SESSION_MODE_PREVIEW) {
        if (!IsWindow(context->preview_parent)) {
            scr_emit_host_diagnostic(
                context,
                SCREENSAVE_DIAG_LEVEL_ERROR,
                2302UL,
                "scr_create_window",
                "Preview mode was requested with an invalid parent window."
            );
            scr_show_message_box(
                context->owner_window,
                context->module,
                "Preview mode requires a valid parent window handle.",
                MB_OK | MB_ICONERROR
            );
            return NULL;
        }

        GetClientRect(context->preview_parent, &rect);
        style = WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS;
        extended_style = 0;

        return CreateWindowExA(
            extended_style,
            SCR_HOST_WINDOW_CLASSA,
            context->module->identity.display_name,
            style,
            0,
            0,
            rect.right - rect.left,
            rect.bottom - rect.top,
            context->preview_parent,
            NULL,
            context->instance,
            context
        );
    }

    style = WS_POPUP | WS_VISIBLE;
    extended_style = WS_EX_TOPMOST;

    return CreateWindowExA(
        extended_style,
        SCR_HOST_WINDOW_CLASSA,
        context->module->identity.display_name,
        style,
        0,
        0,
        GetSystemMetrics(SM_CXSCREEN),
        GetSystemMetrics(SM_CYSCREEN),
        NULL,
        NULL,
        context->instance,
        context
    );
}

int scr_run_window(scr_host_context *context)
{
    MSG message;
    HWND window;
    int get_message_result;

    if (!scr_register_window_class(context->instance)) {
        scr_emit_host_diagnostic(
            context,
            SCREENSAVE_DIAG_LEVEL_ERROR,
            2303UL,
            "scr_run_window",
            "The host window class could not be registered."
        );
        scr_show_message_box(
            context->owner_window,
            context->module,
            "The host window class could not be registered.",
            MB_OK | MB_ICONERROR
        );
        return 1;
    }

    window = scr_create_window(context);
    if (window == NULL) {
        if (
            context->diagnostics.last_text[0] != '\0' &&
            !(context->mode == SCREENSAVE_SESSION_MODE_PREVIEW && !IsWindow(context->preview_parent))
        ) {
            scr_show_message_box(
                context->owner_window,
                context->module,
                context->diagnostics.last_text,
                MB_OK | MB_ICONERROR
            );
        }
        return 1;
    }

    ShowWindow(window, context->show_code == 0 ? SW_SHOW : context->show_code);
    UpdateWindow(window);

    while ((get_message_result = GetMessageA(&message, NULL, 0, 0)) > 0) {
        TranslateMessage(&message);
        DispatchMessageA(&message);
    }

    if (get_message_result < 0) {
        scr_emit_host_diagnostic(
            context,
            SCREENSAVE_DIAG_LEVEL_ERROR,
            2304UL,
            "scr_run_window",
            "The host message loop failed."
        );
        scr_show_message_box(
            context->owner_window,
            context->module,
            "The host message loop failed.",
            MB_OK | MB_ICONERROR
        );
        return 1;
    }

    return (int)message.wParam;
}
