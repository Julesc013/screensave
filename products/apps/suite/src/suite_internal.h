#ifndef SUITE_INTERNAL_H
#define SUITE_INTERNAL_H

#include <windows.h>

#include "screensave/config_api.h"
#include "screensave/diagnostics_api.h"
#include "screensave/renderer_api.h"
#include "screensave/saver_api.h"
#include "screensave/settings_api.h"

#define SUITE_WINDOW_CLASSA "ScreenSaveSuiteWindow"
#define SUITE_PREVIEW_WINDOW_CLASSA "ScreenSaveSuitePreviewWindow"
#define SUITE_INFO_WINDOW_CLASSA "ScreenSaveSuiteInfoWindow"
#define SUITE_RUN_WINDOW_CLASSA "ScreenSaveSuiteRunWindow"
#define SUITE_APP_TITLEA "ScreenSave Suite"
#define SUITE_REGISTRY_ROOTA "Software\\Julesc013\\ScreenSave\\Apps\\suite"
#define SUITE_TIMER_ID 1
#define SUITE_TIMER_INTERVAL_MS 33
#define SUITE_DEFAULT_CLIENT_WIDTH 1120
#define SUITE_DEFAULT_CLIENT_HEIGHT 720
#define SUITE_DEFAULT_PRODUCT_KEY "nocturne"
#define SUITE_DEFAULT_DETERMINISTIC_SEED 0x00000909UL
#define SUITE_PRODUCT_LIMIT 19U
#define SUITE_PACK_LIMIT 8U
#define SUITE_INFO_TEXT_LENGTH 4096
#define SUITE_DETAIL_TEXT_LENGTH 256
#define SUITE_PATH_LENGTH 260U
#define SUITE_COMBO_CURRENT_ITEM ((LPARAM)-1)

#define IDC_SUITE_BROWSER 42001
#define IDC_SUITE_PRESET 42002
#define IDC_SUITE_THEME 42003
#define IDC_SUITE_RENDERER 42004
#define IDC_SUITE_RANDOMIZE 42005
#define IDC_SUITE_RESTART 42006
#define IDC_SUITE_WINDOWED 42007
#define IDC_SUITE_SETTINGS 42008
#define IDC_SUITE_SAVE 42009
#define IDC_SUITE_RESET 42010
#define IDC_SUITE_PRESET_LABEL 42011
#define IDC_SUITE_THEME_LABEL 42012
#define IDC_SUITE_RENDERER_LABEL 42013

typedef struct suite_pack_summary_tag {
    char pack_key[SCREENSAVE_PACK_TEXT_LENGTH];
    char display_name[SCREENSAVE_PACK_TEXT_LENGTH];
} suite_pack_summary;

typedef struct suite_manifest_info_tag {
    char key[32];
    char name[64];
    char version[16];
    char summary[SUITE_DETAIL_TEXT_LENGTH];
    char default_preset[64];
    char default_theme[64];
    char scope[64];
    char renderer[64];
    int preview_safe;
    int long_run_stable;
    int supports_gdi;
    int supports_gl11;
    int supports_gl21;
} suite_manifest_info;

typedef struct suite_catalog_entry_tag {
    const screensave_saver_module *module;
    suite_manifest_info manifest;
    char family_label[64];
    char role_label[64];
    char browser_label[96];
    int artifact_available;
    char artifact_path[SUITE_PATH_LENGTH];
    unsigned int pack_count;
    suite_pack_summary packs[SUITE_PACK_LIMIT];
} suite_catalog_entry;

typedef struct suite_app_config_tag {
    int client_width;
    int client_height;
    int renderer_request;
    char product_key[32];
} suite_app_config;

typedef struct suite_preview_runtime_tag {
    screensave_renderer *renderer;
    screensave_saver_session *session;
    screensave_runtime_clock clock;
    screensave_session_seed seed;
    unsigned long base_seed;
    unsigned long random_seed_counter;
    unsigned long clock_start_tick;
    unsigned long last_tick;
} suite_preview_runtime;

struct suite_app_tag;

typedef struct suite_run_window_tag {
    struct suite_app_tag *owner;
    HWND window;
    const suite_catalog_entry *entry;
    screensave_saver_config_state working_config;
    screensave_saver_config_state resolved_config;
    screensave_config_binding config_binding;
    screensave_renderer *renderer;
    screensave_saver_session *session;
    screensave_runtime_clock clock;
    screensave_session_seed seed;
    unsigned long base_seed;
    unsigned long random_seed_counter;
    unsigned long clock_start_tick;
    unsigned long last_tick;
    screensave_renderer_kind requested_renderer_kind;
    screensave_diag_context diagnostics;
} suite_run_window;

typedef struct suite_app_tag {
    HINSTANCE instance;
    LPSTR command_line;
    int show_code;
    HWND main_window;
    HWND browser_window;
    HWND preview_window;
    HWND info_window;
    HWND preset_label;
    HWND preset_combo;
    HWND theme_label;
    HWND theme_combo;
    HWND renderer_label;
    HWND renderer_combo;
    HWND randomize_check;
    HWND restart_button;
    HWND windowed_button;
    HWND settings_button;
    HWND save_button;
    HWND reset_button;
    screensave_diag_context diagnostics;
    suite_app_config app_config;
    suite_catalog_entry catalog[SUITE_PRODUCT_LIMIT];
    unsigned int catalog_count;
    unsigned int selected_index;
    int syncing_controls;
    screensave_saver_config_state stored_config;
    screensave_saver_config_state working_config;
    screensave_saver_config_state resolved_config;
    screensave_config_binding config_binding;
    suite_preview_runtime preview_runtime;
    suite_run_window *detached_run;
} suite_app;

unsigned int suite_get_available_module_count(void);
const screensave_saver_module *suite_get_available_module(unsigned int index);
const screensave_saver_module *suite_find_target_module(const char *product_key);
int suite_catalog_build(suite_app *app);
int suite_find_catalog_index(const suite_app *app, const char *product_key);
const suite_catalog_entry *suite_get_selected_entry(const suite_app *app);
const screensave_saver_module *suite_get_selected_module(const suite_app *app);

void suite_app_config_set_defaults(suite_app_config *config);
void suite_app_config_clamp(suite_app_config *config);
int suite_app_config_load(suite_app_config *config);
int suite_app_config_save(const suite_app_config *config);

void suite_populate_browser(suite_app *app);
void suite_draw_info(HDC dc, const RECT *client_rect, const suite_app *app);

void suite_dispose_selected_config(suite_app *app);
int suite_select_entry(suite_app *app, unsigned int index);
void suite_reset_working_config(suite_app *app);
int suite_handle_preset_combo_change(suite_app *app);
int suite_handle_theme_combo_change(suite_app *app);
int suite_handle_renderer_combo_change(suite_app *app);
int suite_handle_randomization_toggle(suite_app *app);
int suite_save_working_config(suite_app *app);
INT_PTR suite_show_selected_settings_dialog(suite_app *app);
void suite_sync_controls_from_working_config(suite_app *app);

int suite_preview_start(suite_app *app);
void suite_preview_stop(suite_app *app);
int suite_preview_restart(suite_app *app, int reseed);
void suite_preview_tick(suite_app *app);
void suite_preview_render(suite_app *app, HDC dc);
void suite_preview_resize(suite_app *app, int width, int height);
int suite_register_run_window_class(HINSTANCE instance);
int suite_open_windowed_run(suite_app *app);
void suite_detached_window_closed(suite_run_window *run_window);

int suite_app_run(HINSTANCE instance, LPSTR command_line, int show_code);

#endif /* SUITE_INTERNAL_H */
