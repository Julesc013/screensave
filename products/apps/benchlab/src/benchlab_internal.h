#ifndef BENCHLAB_INTERNAL_H
#define BENCHLAB_INTERNAL_H

#include <windows.h>

#include "screensave/config_api.h"
#include "screensave/diagnostics_api.h"
#include "screensave/renderer_api.h"
#include "screensave/saver_api.h"
#include "screensave/version.h"
#include "../../../../platform/src/core/base/display_text.h"

#define BENCHLAB_WINDOW_CLASSA "ScreenSaveBenchLabWindow"
#define BENCHLAB_RENDER_WINDOW_CLASSA "ScreenSaveBenchLabRenderWindow"
#define BENCHLAB_INFO_WINDOW_CLASSA "ScreenSaveBenchLabInfoWindow"
#define BENCHLAB_REGISTRY_ROOTA "Software\\Julesc013\\ScreenSave\\Apps\\benchlab"
#define BENCHLAB_APP_TITLEA "BenchLab"
#define BENCHLAB_TIMER_ID 1
#define BENCHLAB_TIMER_INTERVAL_MS 33
#define BENCHLAB_DEFAULT_CLIENT_WIDTH 640
#define BENCHLAB_DEFAULT_CLIENT_HEIGHT 480
#define BENCHLAB_INFO_PANEL_WIDTH 280
#define BENCHLAB_DEFAULT_FIXED_SEED 0x00000707UL
#define BENCHLAB_DEFAULT_PRODUCT_KEY "nocturne"
#define BENCHLAB_STEP_DELTA_MS 33UL
#define BENCHLAB_DIAG_LINE_COUNT 6U
#define BENCHLAB_DIAG_LINE_LENGTH 160

#define IDM_BENCHLAB_RESTART 41001
#define IDM_BENCHLAB_RESEED 41002
#define IDM_BENCHLAB_TOGGLE_DETERMINISTIC 41003
#define IDM_BENCHLAB_TOGGLE_OVERLAY 41004
#define IDM_BENCHLAB_TOGGLE_PAUSE 41005
#define IDM_BENCHLAB_STEP_FRAME 41006
#define IDM_BENCHLAB_SAVER_SETTINGS 41007
#define IDM_BENCHLAB_EXIT 41008
#define IDM_BENCHLAB_RENDERER_AUTO 41009
#define IDM_BENCHLAB_RENDERER_GDI 41010
#define IDM_BENCHLAB_RENDERER_GL11 41011
#define IDM_BENCHLAB_RENDERER_GL21 41012
#define IDM_BENCHLAB_RENDERER_GL33 41013
#define IDM_BENCHLAB_RENDERER_GL46 41014
#define IDM_BENCHLAB_PRODUCT_FIRST 41020
#define IDM_BENCHLAB_PRODUCT_LAST 41038

typedef struct benchlab_app_config_tag {
    int client_width;
    int client_height;
    int overlay_enabled;
    int deterministic_mode;
    int renderer_request;
    unsigned long fixed_seed;
    char product_key[32];
} benchlab_app_config;

typedef struct benchlab_diag_buffer_tag {
    unsigned int next_index;
    unsigned int count;
    char lines[BENCHLAB_DIAG_LINE_COUNT][BENCHLAB_DIAG_LINE_LENGTH];
} benchlab_diag_buffer;

typedef struct benchlab_app_tag {
    HINSTANCE instance;
    LPSTR command_line;
    int show_code;
    HWND main_window;
    HWND render_window;
    HWND info_window;
    HMENU menu;
    const screensave_saver_module *module;
    screensave_renderer_kind requested_renderer_kind;
    screensave_diag_context diagnostics;
    benchlab_diag_buffer diag_buffer;
    benchlab_app_config app_config;
    screensave_saver_config_state saver_config;
    screensave_saver_config_state resolved_config;
    screensave_config_binding config_binding;
    screensave_renderer *renderer;
    screensave_saver_session *session;
    screensave_runtime_clock clock;
    screensave_session_seed session_seed;
    unsigned long current_base_seed;
    unsigned long clock_start_tick;
    unsigned long last_tick;
    unsigned long random_seed_counter;
    int paused;
} benchlab_app;

unsigned int benchlab_get_available_module_count(void);
const screensave_saver_module *benchlab_get_available_module(unsigned int index);
const screensave_saver_module *benchlab_find_target_module(const char *product_key);
const screensave_saver_module *benchlab_get_target_module(void);

void benchlab_app_config_set_defaults(benchlab_app_config *config);
void benchlab_app_config_clamp(benchlab_app_config *config);
int benchlab_app_config_load(benchlab_app_config *config);
int benchlab_app_config_save(const benchlab_app_config *config);
void benchlab_app_config_apply_command_line(benchlab_app *app, LPSTR command_line);

void benchlab_diag_attach(benchlab_app *app);
void benchlab_emit_app_diag(
    benchlab_app *app,
    screensave_diag_level level,
    unsigned long code,
    const char *text
);
const char *benchlab_diag_get_recent(const benchlab_diag_buffer *buffer, unsigned int recent_index);

int benchlab_session_initialize_config(benchlab_app *app);
void benchlab_session_dispose_config(benchlab_app *app);
int benchlab_session_create_runtime(benchlab_app *app, HWND window);
void benchlab_session_destroy_runtime(benchlab_app *app);
int benchlab_session_recreate_runtime(benchlab_app *app);
void benchlab_session_resize(benchlab_app *app, int width, int height);
int benchlab_session_restart(benchlab_app *app, int reseed);
void benchlab_session_tick(benchlab_app *app);
void benchlab_session_step_once(benchlab_app *app);
void benchlab_session_render(benchlab_app *app, HDC dc);
void benchlab_session_resume_clock(benchlab_app *app);
INT_PTR benchlab_session_show_saver_dialog(benchlab_app *app);

void benchlab_draw_overlay(HDC dc, const RECT *client_rect, const benchlab_app *app);

int benchlab_app_run(HINSTANCE instance, LPSTR command_line, int show_code);

#endif /* BENCHLAB_INTERNAL_H */
