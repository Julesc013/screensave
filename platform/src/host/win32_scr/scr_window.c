#include "scr_internal.h"

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

static void scr_draw_placeholder(HDC dc, const RECT *client_rect, const scr_host_context *context)
{
    HBRUSH brush;
    RECT moving_rect;
    int width;
    int height;
    int size;
    int span;
    int x;
    int y;
    COLORREF accent;

    width = client_rect->right - client_rect->left;
    height = client_rect->bottom - client_rect->top;
    if (width <= 0 || height <= 0) {
        return;
    }

    size = height / 5;
    if (size < 8) {
        size = 8;
    }
    if (size > width) {
        size = width;
    }

    span = width - size;
    if (span <= 0) {
        x = 0;
    } else {
        x = (int)((context->paint_tick * 4U) % (unsigned int)(span + 1));
    }
    y = (height - size) / 2;

    SetRect(&moving_rect, x, y, x + size, y + size);
    accent = context->preview_mode ? RGB(72, 72, 72) : RGB(0, 96, 24);
    brush = CreateSolidBrush(accent);
    if (brush != NULL) {
        FillRect(dc, &moving_rect, brush);
        DeleteObject(brush);
    }
}

static void scr_draw_overlay(HDC dc, const RECT *client_rect, const scr_host_context *context)
{
    RECT text_rect;
    char overlay[256];

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
            context->preview_mode = context->mode == SCR_RUN_MODE_PREVIEW;
            context->start_tick = GetTickCount();
            context->paint_tick = 0;
            GetCursorPos(&context->initial_cursor);
            context->timer_id = SetTimer(window, SCR_TIMER_ID, SCR_TIMER_INTERVAL_MS, NULL);
        }
        return 0;

    case WM_ERASEBKGND:
        return 1;

    case WM_TIMER:
        if (context != NULL && wParam == SCR_TIMER_ID) {
            context->paint_tick += 1U;
            InvalidateRect(window, NULL, FALSE);
        }
        return 0;

    case WM_SIZE:
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
        FillRect(dc, &client_rect, (HBRUSH)GetStockObject(BLACK_BRUSH));
        if (context != NULL && context->settings.placeholder_visual_enabled) {
            /*
             * Temporary Series 03 liveness marker.
             * This stays host-local until the shared renderer layer exists.
             */
            scr_draw_placeholder(dc, &client_rect, context);
        }
        if (context != NULL && context->settings.diagnostics_overlay_enabled) {
            scr_draw_overlay(dc, &client_rect, context);
        }
        EndPaint(window, &paint);
        return 0;

    case WM_DESTROY:
        if (context != NULL && context->timer_id != 0) {
            KillTimer(window, context->timer_id);
            context->timer_id = 0;
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

    if (context->mode == SCR_RUN_MODE_PREVIEW) {
        if (!IsWindow(context->preview_parent)) {
            scr_show_message_box(
                context->owner_window,
                &context->product,
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
            context->product.display_name,
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
        context->product.display_name,
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
        scr_show_message_box(
            context->owner_window,
            &context->product,
            "The Series 03 host window class could not be registered.",
            MB_OK | MB_ICONERROR
        );
        return 1;
    }

    window = scr_create_window(context);
    if (window == NULL) {
        return 1;
    }

    ShowWindow(window, context->show_code == 0 ? SW_SHOW : context->show_code);
    UpdateWindow(window);

    while ((get_message_result = GetMessageA(&message, NULL, 0, 0)) > 0) {
        TranslateMessage(&message);
        DispatchMessageA(&message);
    }

    if (get_message_result < 0) {
        scr_show_message_box(
            context->owner_window,
            &context->product,
            "The host message loop failed.",
            MB_OK | MB_ICONERROR
        );
        return 1;
    }

    return (int)message.wParam;
}
