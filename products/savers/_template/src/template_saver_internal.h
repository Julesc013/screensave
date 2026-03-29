#ifndef TEMPLATE_SAVER_INTERNAL_H
#define TEMPLATE_SAVER_INTERNAL_H

#include <windows.h>

#include "screensave/config_api.h"
#include "screensave/diagnostics_api.h"
#include "screensave/saver_api.h"

#define TEMPLATE_SAVER_PRODUCT_REGISTRY_ROOTA "Software\\Julesc013\\ScreenSave\\Products\\template_saver"
#define TEMPLATE_SAVER_DEFAULT_PRESET_KEY "starter_drift"
#define TEMPLATE_SAVER_DEFAULT_THEME_KEY "starter_blue"
#define TEMPLATE_SAVER_PRESET_COUNT 6U
#define TEMPLATE_SAVER_THEME_COUNT 3U

typedef enum template_saver_motion_mode_tag {
    TEMPLATE_SAVER_MOTION_DRIFT = 0,
    TEMPLATE_SAVER_MOTION_FOCUS = 1,
    TEMPLATE_SAVER_MOTION_CALM = 2
} template_saver_motion_mode;

typedef enum template_saver_accent_mode_tag {
    TEMPLATE_SAVER_ACCENT_FRAME = 0,
    TEMPLATE_SAVER_ACCENT_PULSE = 1,
    TEMPLATE_SAVER_ACCENT_BANDS = 2
} template_saver_accent_mode;

typedef enum template_saver_spacing_mode_tag {
    TEMPLATE_SAVER_SPACING_TIGHT = 0,
    TEMPLATE_SAVER_SPACING_BALANCED = 1,
    TEMPLATE_SAVER_SPACING_WIDE = 2
} template_saver_spacing_mode;

typedef struct template_saver_config_tag {
    int motion_mode;
    int accent_mode;
    int spacing_mode;
} template_saver_config;

typedef struct template_saver_preset_values_tag {
    int motion_mode;
    int accent_mode;
    int spacing_mode;
} template_saver_preset_values;

extern const screensave_preset_descriptor g_template_saver_presets[];
extern const screensave_theme_descriptor g_template_saver_themes[];

struct screensave_saver_session_tag {
    template_saver_config config;
    const screensave_theme_descriptor *theme;
    screensave_sizei drawable_size;
    int preview_mode;
    long marker_x;
    long marker_y;
    int direction_x;
    int direction_y;
    unsigned long pulse_phase;
};

const screensave_saver_module *template_saver_get_module(void);

const screensave_preset_descriptor *template_saver_get_presets(unsigned int *count_out);
const screensave_theme_descriptor *template_saver_get_themes(unsigned int *count_out);
const template_saver_preset_values *template_saver_find_preset_values(const char *preset_key);
const screensave_theme_descriptor *template_saver_find_theme_descriptor(const char *theme_key);

void template_saver_config_set_defaults(
    screensave_common_config *common_config,
    void *product_config,
    unsigned int product_config_size
);
void template_saver_config_clamp(
    screensave_common_config *common_config,
    void *product_config,
    unsigned int product_config_size
);
int template_saver_config_load(
    const screensave_saver_module *module,
    screensave_common_config *common_config,
    void *product_config,
    unsigned int product_config_size,
    screensave_diag_context *diagnostics
);
int template_saver_config_save(
    const screensave_saver_module *module,
    const screensave_common_config *common_config,
    const void *product_config,
    unsigned int product_config_size,
    screensave_diag_context *diagnostics
);
INT_PTR template_saver_config_show_dialog(
    HINSTANCE instance,
    HWND owner_window,
    const screensave_saver_module *module,
    screensave_common_config *common_config,
    void *product_config,
    unsigned int product_config_size,
    screensave_diag_context *diagnostics
);
int template_saver_config_export_settings_entries(
    const screensave_saver_module *module,
    const screensave_common_config *common_config,
    const void *product_config,
    unsigned int product_config_size,
    screensave_settings_file_kind kind,
    screensave_settings_writer *writer,
    screensave_diag_context *diagnostics
);
int template_saver_config_import_settings_entry(
    const screensave_saver_module *module,
    screensave_common_config *common_config,
    void *product_config,
    unsigned int product_config_size,
    screensave_settings_file_kind kind,
    const char *section,
    const char *key,
    const char *value,
    screensave_diag_context *diagnostics
);
void template_saver_config_randomize_settings(
    const screensave_saver_module *module,
    screensave_common_config *common_config,
    void *product_config,
    unsigned int product_config_size,
    const screensave_session_seed *seed,
    screensave_diag_context *diagnostics
);

int template_saver_create_session(
    const screensave_saver_module *module,
    screensave_saver_session **session,
    const screensave_saver_environment *environment
);
void template_saver_destroy_session(screensave_saver_session *session);
void template_saver_resize_session(
    screensave_saver_session *session,
    const screensave_saver_environment *environment
);
void template_saver_step_session(
    screensave_saver_session *session,
    const screensave_saver_environment *environment
);
void template_saver_render_session(
    screensave_saver_session *session,
    const screensave_saver_environment *environment
);

const char *template_saver_motion_mode_name(int motion_mode);
const char *template_saver_accent_mode_name(int accent_mode);
const char *template_saver_spacing_mode_name(int spacing_mode);
int template_saver_motion_mode_from_name(const char *name, int fallback_value);
int template_saver_accent_mode_from_name(const char *name, int fallback_value);
int template_saver_spacing_mode_from_name(const char *name, int fallback_value);

void template_saver_apply_preset_to_config(
    const char *preset_key,
    screensave_common_config *common_config,
    template_saver_config *product_config
);

#endif /* TEMPLATE_SAVER_INTERNAL_H */
