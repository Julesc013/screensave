#ifndef SCREENSAVE_SCR_INTERNAL_H
#define SCREENSAVE_SCR_INTERNAL_H

#include <windows.h>

#include "screensave/scr_entry.h"

#define SCR_HOST_WINDOW_CLASSA "ScreenSaveHostWindow"
#define SCR_SETTINGS_ROOTA "Software\\Julesc013\\ScreenSave\\Products\\"
#define SCR_TIMER_ID 1
#define SCR_TIMER_INTERVAL_MS 33
#define SCR_EXIT_MOUSE_DELTA 4

typedef enum scr_run_mode_tag {
    SCR_RUN_MODE_CONFIG = 0,
    SCR_RUN_MODE_SCREEN = 1,
    SCR_RUN_MODE_PREVIEW = 2
} scr_run_mode;

typedef struct scr_settings_tag {
    int placeholder_visual_enabled;
    int diagnostics_overlay_enabled;
} scr_settings;

typedef struct scr_parsed_args_tag {
    scr_run_mode mode;
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
    screensave_product_identity product;
    scr_run_mode mode;
    HWND owner_window;
    HWND preview_parent;
    HWND main_window;
    UINT_PTR timer_id;
    DWORD start_tick;
    unsigned int paint_tick;
    int preview_mode;
    int exit_pending;
    POINT initial_cursor;
    scr_settings settings;
} scr_host_context;

void scr_settings_set_defaults(scr_settings *settings);
void scr_settings_load(const screensave_product_identity *product, scr_settings *settings);
int scr_settings_save(const screensave_product_identity *product, const scr_settings *settings);
int scr_parse_command_line(LPSTR command_line, scr_parsed_args *parsed_args);
int scr_run_window(scr_host_context *context);
INT_PTR scr_show_config_dialog(scr_host_context *context);
const char *scr_mode_label(scr_run_mode mode);
void scr_build_version_text(const scr_host_context *context, char *buffer, int buffer_size);
void scr_build_overlay_text(const scr_host_context *context, char *buffer, int buffer_size);
int scr_append_text(char *buffer, int buffer_size, const char *text);
int scr_append_number(char *buffer, int buffer_size, unsigned long value);
void scr_show_message_box(HWND owner, const screensave_product_identity *product, const char *text, UINT type);

#endif /* SCREENSAVE_SCR_INTERNAL_H */
