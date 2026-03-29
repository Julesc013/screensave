#ifndef SCREENSAVE_SCR_INTERNAL_H
#define SCREENSAVE_SCR_INTERNAL_H

#include <windows.h>

#include "screensave/config_api.h"
#include "screensave/diagnostics_api.h"
#include "screensave/saver_api.h"
#include "../../core/timing/timing_internal.h"

#define SCR_HOST_WINDOW_CLASSA "ScreenSaveHostWindow"
#define SCR_SETTINGS_ROOTA "Software\\Julesc013\\ScreenSave\\Products\\"
#define SCR_HOST_REGISTRY_ROOTA "Software\\Julesc013\\ScreenSave\\Host"
#define SCR_TIMER_ID 1
#define SCR_TIMER_INTERVAL_MS 33
#define SCR_EXIT_MOUSE_DELTA 4

typedef screensave_saver_config_state scr_settings;

typedef struct scr_parsed_args_tag {
    screensave_session_mode mode;
    HWND preview_parent;
    HWND config_owner;
    int show_invalid_argument_message;
    int invalid_preview_parent;
    char invalid_argument[64];
} scr_parsed_args;

typedef struct scr_host_context_tag {
    HINSTANCE instance;
    HINSTANCE previous_instance;
    LPSTR command_line;
    int show_code;
    const screensave_saver_module *default_module;
    const screensave_saver_module *const *available_modules;
    unsigned int available_module_count;
    const screensave_saver_module *module;
    screensave_session_mode mode;
    HWND owner_window;
    HWND preview_parent;
    HWND main_window;
    UINT_PTR timer_id;
    int preview_mode;
    int exit_pending;
    POINT initial_cursor;
    scr_settings settings;
    screensave_timebase timebase;
    screensave_runtime_clock clock;
    screensave_session_seed session_seed;
    screensave_config_binding config_binding;
    screensave_diag_context diagnostics;
    screensave_renderer_kind requested_renderer_kind;
    screensave_renderer *renderer;
    screensave_saver_session *session;
} scr_host_context;

int scr_settings_init(const screensave_saver_module *module, scr_settings *settings);
void scr_settings_dispose(scr_settings *settings);
void scr_settings_set_defaults(const screensave_saver_module *module, scr_settings *settings);
void scr_settings_clamp(const screensave_saver_module *module, scr_settings *settings);
int scr_settings_load(
    const screensave_saver_module *module,
    scr_settings *settings,
    screensave_diag_context *diagnostics
);
int scr_settings_save(
    const screensave_saver_module *module,
    const scr_settings *settings,
    screensave_diag_context *diagnostics
);
int scr_load_selected_product_key(char *buffer, unsigned int buffer_size);
int scr_save_selected_product_key(const char *product_key);
screensave_renderer_kind scr_load_renderer_request(void);
int scr_save_renderer_request(screensave_renderer_kind requested_kind);
int scr_parse_command_line(LPSTR command_line, scr_parsed_args *parsed_args);
int scr_run_window(scr_host_context *context);
INT_PTR scr_show_config_dialog(scr_host_context *context);
void scr_render_validation_scene(scr_host_context *context);
const char *scr_mode_label(screensave_session_mode mode);
void scr_build_version_text(const scr_host_context *context, char *buffer, int buffer_size);
void scr_build_overlay_text(const scr_host_context *context, char *buffer, int buffer_size);
int scr_append_text(char *buffer, int buffer_size, const char *text);
int scr_append_number(char *buffer, int buffer_size, unsigned long value);
void scr_emit_host_diagnostic(
    scr_host_context *context,
    screensave_diag_level level,
    unsigned long code,
    const char *origin,
    const char *text
);
void scr_show_message_box(HWND owner, const screensave_saver_module *module, const char *text, UINT type);

#endif /* SCREENSAVE_SCR_INTERNAL_H */
