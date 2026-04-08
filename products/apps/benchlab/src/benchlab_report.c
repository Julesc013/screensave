#include "benchlab_internal.h"

typedef struct benchlab_report_target_tag {
    HWND release_window;
    HDC reference_dc;
    HDC target_dc;
    HBITMAP bitmap;
    HBITMAP previous_bitmap;
} benchlab_report_target;

static int benchlab_report_append_text(char *buffer, int buffer_size, const char *text)
{
    int used;
    int added;

    if (buffer == NULL || text == NULL || buffer_size <= 0) {
        return 0;
    }

    used = lstrlenA(buffer);
    added = lstrlenA(text);
    if (used + added + 1 > buffer_size) {
        return 0;
    }

    lstrcpyA(buffer + used, text);
    return 1;
}

static int benchlab_report_append_number(char *buffer, int buffer_size, unsigned long value)
{
    char text[32];

    wsprintfA(text, "%lu", value);
    return benchlab_report_append_text(buffer, buffer_size, text);
}

static int benchlab_report_append_hex(char *buffer, int buffer_size, unsigned long value)
{
    char text[32];

    wsprintfA(text, "0x%08lX", value);
    return benchlab_report_append_text(buffer, buffer_size, text);
}

static const char *benchlab_report_renderer_request_label(screensave_renderer_kind requested_kind)
{
    if (requested_kind == SCREENSAVE_RENDERER_KIND_UNKNOWN) {
        return "auto";
    }

    return screensave_renderer_kind_name(requested_kind);
}

static int benchlab_report_ensure_parent_dirs(const char *path)
{
    char partial[MAX_PATH];
    int index;
    int length;

    if (path == NULL || path[0] == '\0') {
        return 0;
    }

    length = lstrlenA(path);
    if (length <= 0 || length >= (int)sizeof(partial)) {
        return 0;
    }

    lstrcpynA(partial, path, sizeof(partial));
    for (index = 0; partial[index] != '\0'; ++index) {
        if (partial[index] != '\\' && partial[index] != '/') {
            continue;
        }

        if (index == 0) {
            continue;
        }
        if (index == 2 && partial[1] == ':') {
            continue;
        }

        partial[index] = '\0';
        if (
            partial[0] != '\0' &&
            !CreateDirectoryA(partial, NULL) &&
            GetLastError() != ERROR_ALREADY_EXISTS
        ) {
            return 0;
        }
        partial[index] = '\\';
    }

    return 1;
}

static int benchlab_report_write_text_file(const char *path, const char *text)
{
    HANDLE file_handle;
    DWORD bytes_written;
    DWORD text_length;

    if (path == NULL || path[0] == '\0' || text == NULL) {
        return 0;
    }

    file_handle = CreateFileA(
        path,
        GENERIC_WRITE,
        0,
        NULL,
        CREATE_ALWAYS,
        FILE_ATTRIBUTE_NORMAL,
        NULL
    );
    if (file_handle == INVALID_HANDLE_VALUE) {
        return 0;
    }

    text_length = (DWORD)lstrlenA(text);
    bytes_written = 0UL;
    if (
        !WriteFile(file_handle, text, text_length, &bytes_written, NULL) ||
        bytes_written != text_length
    ) {
        CloseHandle(file_handle);
        return 0;
    }

    CloseHandle(file_handle);
    return 1;
}

static int benchlab_report_build_default_path(const benchlab_app *app, char *buffer, int buffer_size)
{
    if (buffer == NULL || buffer_size <= 0 || app == NULL || app->module == NULL) {
        return 0;
    }

    buffer[0] = '\0';
    if (
        !benchlab_report_append_text(buffer, buffer_size, "validation\\captures\\sx09\\benchlab-") ||
        !benchlab_report_append_text(buffer, buffer_size, app->module->identity.product_key) ||
        !benchlab_report_append_text(buffer, buffer_size, "-") ||
        !benchlab_report_append_text(
            buffer,
            buffer_size,
            benchlab_report_renderer_request_label(app->requested_renderer_kind)
        ) ||
        !benchlab_report_append_text(buffer, buffer_size, "-") ||
        !benchlab_report_append_text(
            buffer,
            buffer_size,
            app->app_config.deterministic_mode ? "deterministic" : "session"
        ) ||
        !benchlab_report_append_text(buffer, buffer_size, ".txt")
    ) {
        buffer[0] = '\0';
        return 0;
    }

    return 1;
}

static void benchlab_report_target_release(benchlab_report_target *target)
{
    if (target == NULL) {
        return;
    }

    if (target->target_dc != NULL) {
        if (target->previous_bitmap != NULL) {
            SelectObject(target->target_dc, target->previous_bitmap);
        }
        DeleteDC(target->target_dc);
    }
    if (target->bitmap != NULL) {
        DeleteObject(target->bitmap);
    }
    if (target->reference_dc != NULL) {
        ReleaseDC(target->release_window, target->reference_dc);
    }
    ZeroMemory(target, sizeof(*target));
}

static int benchlab_report_target_init_gdi(
    HWND window,
    int width,
    int height,
    benchlab_report_target *target
)
{
    if (target == NULL) {
        return 0;
    }

    ZeroMemory(target, sizeof(*target));
    if (width <= 0) {
        width = 1;
    }
    if (height <= 0) {
        height = 1;
    }

    target->release_window = window;
    target->reference_dc = GetDC(window);
    if (target->reference_dc == NULL) {
        target->release_window = NULL;
        target->reference_dc = GetDC(NULL);
    }
    if (target->reference_dc == NULL) {
        return 0;
    }

    target->target_dc = CreateCompatibleDC(target->reference_dc);
    if (target->target_dc == NULL) {
        benchlab_report_target_release(target);
        return 0;
    }

    target->bitmap = CreateCompatibleBitmap(target->reference_dc, width, height);
    if (target->bitmap == NULL) {
        benchlab_report_target_release(target);
        return 0;
    }

    target->previous_bitmap = (HBITMAP)SelectObject(target->target_dc, target->bitmap);
    if (target->previous_bitmap == NULL || target->previous_bitmap == HGDI_ERROR) {
        benchlab_report_target_release(target);
        return 0;
    }

    return 1;
}

static int benchlab_report_render_frame(benchlab_app *app)
{
    HDC dc;
    screensave_renderer_info renderer_info;
    benchlab_report_target target;

    if (app == NULL || app->render_window == NULL) {
        return 0;
    }

    screensave_renderer_get_info(app->renderer, &renderer_info);
    if (
        renderer_info.active_kind == SCREENSAVE_RENDERER_KIND_GDI &&
        benchlab_report_target_init_gdi(
            app->render_window,
            renderer_info.drawable_size.width,
            renderer_info.drawable_size.height,
            &target
        )
    ) {
        benchlab_session_render(app, target.target_dc);
        benchlab_report_target_release(&target);
        return 1;
    }

    dc = GetDC(app->render_window);
    if (dc == NULL) {
        return 0;
    }

    benchlab_session_render(app, dc);
    ReleaseDC(app->render_window, dc);
    return 1;
}

int benchlab_write_report(benchlab_app *app)
{
    screensave_renderer_info renderer_info;
    screensave_renderer_kind policy_target_kind;
    unsigned long frame_index;
    const char *report_path;
    char degraded_path[128];
    char overlay[2048];
    char policy_target[64];
    char reason_text[128];
    char status_text[128];
    char report_text[8192];
    char default_path[MAX_PATH];
    unsigned int diag_index;

    if (app == NULL || app->module == NULL || app->renderer == NULL) {
        return 0;
    }

    report_path = app->report_path;
    if (report_path == NULL || report_path[0] == '\0') {
        if (!benchlab_report_build_default_path(app, default_path, sizeof(default_path))) {
            benchlab_emit_app_diag(
                app,
                SCREENSAVE_DIAG_LEVEL_ERROR,
                7212UL,
                "BenchLab could not build the default SX09 report path."
            );
            return 0;
        }
        report_path = default_path;
    }

    if (!benchlab_report_ensure_parent_dirs(report_path)) {
        benchlab_emit_app_diag(
            app,
            SCREENSAVE_DIAG_LEVEL_ERROR,
            7213UL,
            "BenchLab could not create the parent directories for the SX09 report path."
        );
        return 0;
    }

    if (app->report_frame_count == 0UL) {
        app->report_frame_count = BENCHLAB_DEFAULT_REPORT_FRAMES;
    }

    for (frame_index = 0UL; frame_index < app->report_frame_count; ++frame_index) {
        benchlab_session_step_once(app);
        if (!benchlab_report_render_frame(app)) {
            benchlab_emit_app_diag(
                app,
                SCREENSAVE_DIAG_LEVEL_ERROR,
                7214UL,
                "BenchLab could not render a frame for the SX09 report."
            );
            return 0;
        }
    }

    screensave_renderer_get_info(app->renderer, &renderer_info);
    if (
        screensave_display_renderer_effective_kind(
            renderer_info.requested_kind,
            renderer_info.selection_reason,
            &policy_target_kind
        )
    ) {
        lstrcpynA(
            policy_target,
            screensave_display_renderer_kind(policy_target_kind),
            sizeof(policy_target)
        );
    } else {
        lstrcpynA(
            policy_target,
            screensave_display_renderer_kind(renderer_info.requested_kind),
            sizeof(policy_target)
        );
    }
    screensave_display_renderer_degraded_path(
        renderer_info.requested_kind,
        renderer_info.selection_reason,
        renderer_info.active_kind,
        degraded_path,
        sizeof(degraded_path)
    );
    screensave_display_renderer_reason(
        renderer_info.selection_reason,
        reason_text,
        sizeof(reason_text)
    );
    screensave_display_renderer_status(
        renderer_info.status_text,
        status_text,
        sizeof(status_text)
    );

    benchlab_build_overlay_text(app, overlay, sizeof(overlay));

    report_text[0] = '\0';
    benchlab_report_append_text(report_text, sizeof(report_text), "BenchLab SX09 report\r\n");
    benchlab_report_append_text(report_text, sizeof(report_text), "Product key: ");
    benchlab_report_append_text(report_text, sizeof(report_text), app->module->identity.product_key);
    benchlab_report_append_text(report_text, sizeof(report_text), "\r\nDisplay name: ");
    benchlab_report_append_text(report_text, sizeof(report_text), app->module->identity.display_name);
    benchlab_report_append_text(report_text, sizeof(report_text), "\r\nRequested renderer: ");
    benchlab_report_append_text(
        report_text,
        sizeof(report_text),
        screensave_display_renderer_kind(app->requested_renderer_kind)
    );
    benchlab_report_append_text(report_text, sizeof(report_text), "\r\nPolicy target: ");
    benchlab_report_append_text(report_text, sizeof(report_text), policy_target);
    benchlab_report_append_text(report_text, sizeof(report_text), "\r\nActive renderer: ");
    benchlab_report_append_text(
        report_text,
        sizeof(report_text),
        screensave_display_renderer_kind(renderer_info.active_kind)
    );
    benchlab_report_append_text(report_text, sizeof(report_text), "\r\nSelected band: ");
    benchlab_report_append_text(
        report_text,
        sizeof(report_text),
        screensave_display_render_band(renderer_info.active_kind)
    );
    benchlab_report_append_text(report_text, sizeof(report_text), "\r\nDegraded path: ");
    benchlab_report_append_text(report_text, sizeof(report_text), degraded_path);
    benchlab_report_append_text(report_text, sizeof(report_text), "\r\nSelection path: ");
    benchlab_report_append_text(report_text, sizeof(report_text), reason_text);
    benchlab_report_append_text(report_text, sizeof(report_text), "\r\nFallback cause: ");
    screensave_display_renderer_reason(
        renderer_info.fallback_reason,
        reason_text,
        sizeof(reason_text)
    );
    benchlab_report_append_text(report_text, sizeof(report_text), reason_text);
    benchlab_report_append_text(report_text, sizeof(report_text), "\r\nRenderer backend: ");
    benchlab_report_append_text(
        report_text,
        sizeof(report_text),
        renderer_info.backend_name != NULL ? renderer_info.backend_name : "unknown"
    );
    benchlab_report_append_text(report_text, sizeof(report_text), "\r\nRenderer status: ");
    benchlab_report_append_text(report_text, sizeof(report_text), status_text);
    benchlab_report_append_text(report_text, sizeof(report_text), "\r\nBackend vendor: ");
    benchlab_report_append_text(
        report_text,
        sizeof(report_text),
        renderer_info.vendor_name != NULL ? renderer_info.vendor_name : "unknown"
    );
    benchlab_report_append_text(report_text, sizeof(report_text), "\r\nBackend renderer: ");
    benchlab_report_append_text(
        report_text,
        sizeof(report_text),
        renderer_info.renderer_name != NULL ? renderer_info.renderer_name : "unknown"
    );
    benchlab_report_append_text(report_text, sizeof(report_text), "\r\nBackend detail: ");
    benchlab_report_append_text(
        report_text,
        sizeof(report_text),
        renderer_info.version_name != NULL ? renderer_info.version_name : "unknown"
    );
    benchlab_report_append_text(report_text, sizeof(report_text), "\r\nDeterministic mode: ");
    benchlab_report_append_text(
        report_text,
        sizeof(report_text),
        app->app_config.deterministic_mode ? "yes" : "no"
    );
    benchlab_report_append_text(report_text, sizeof(report_text), "\r\nBase seed: ");
    benchlab_report_append_hex(report_text, sizeof(report_text), app->current_base_seed);
    benchlab_report_append_text(report_text, sizeof(report_text), "\r\nStream seed: ");
    benchlab_report_append_hex(report_text, sizeof(report_text), app->session_seed.stream_seed);
    benchlab_report_append_text(report_text, sizeof(report_text), "\r\nFrames exercised: ");
    benchlab_report_append_number(report_text, sizeof(report_text), app->report_frame_count);
    benchlab_report_append_text(report_text, sizeof(report_text), "\r\nFinal frame: ");
    benchlab_report_append_number(report_text, sizeof(report_text), app->clock.frame_index);
    benchlab_report_append_text(report_text, sizeof(report_text), "\r\nCommand line: ");
    benchlab_report_append_text(
        report_text,
        sizeof(report_text),
        app->command_line != NULL && app->command_line[0] != '\0'
            ? app->command_line
            : "(none)"
    );
    benchlab_report_append_text(report_text, sizeof(report_text), "\r\n\r\nOverlay snapshot\r\n----------------\r\n");
    benchlab_report_append_text(report_text, sizeof(report_text), overlay);
    benchlab_report_append_text(report_text, sizeof(report_text), "\r\n\r\nRecent diagnostics\r\n------------------");

    for (diag_index = 0U; diag_index < BENCHLAB_DIAG_LINE_COUNT; ++diag_index) {
        const char *line;

        line = benchlab_diag_get_recent(&app->diag_buffer, diag_index);
        if (line[0] == '\0') {
            continue;
        }

        benchlab_report_append_text(report_text, sizeof(report_text), "\r\n- ");
        benchlab_report_append_text(report_text, sizeof(report_text), line);
    }

    benchlab_report_append_text(report_text, sizeof(report_text), "\r\n");

    if (!benchlab_report_write_text_file(report_path, report_text)) {
        benchlab_emit_app_diag(
            app,
            SCREENSAVE_DIAG_LEVEL_ERROR,
            7215UL,
            "BenchLab could not write the SX09 report output file."
        );
        return 0;
    }

    lstrcpynA(app->report_path, report_path, sizeof(app->report_path));
    benchlab_emit_app_diag(
        app,
        SCREENSAVE_DIAG_LEVEL_INFO,
        7216UL,
        "BenchLab wrote the SX09 substrate validation report."
    );
    return 1;
}
