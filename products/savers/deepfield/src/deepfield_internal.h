#ifndef DEEPFIELD_INTERNAL_H
#define DEEPFIELD_INTERNAL_H

#include <windows.h>

#include "screensave/config_api.h"
#include "screensave/diagnostics_api.h"
#include "screensave/saver_api.h"

#define DEEPFIELD_PRODUCT_REGISTRY_ROOTA "Software\\Julesc013\\ScreenSave\\Products\\deepfield"
#define DEEPFIELD_DEFAULT_PRESET_KEY "quiet_observatory"
#define DEEPFIELD_DEFAULT_THEME_KEY "quiet_observatory"
#define DEEPFIELD_PRESET_COUNT 8U
#define DEEPFIELD_THEME_COUNT 6U
#define DEEPFIELD_MAX_STARS 160U

typedef enum deepfield_scene_mode_tag {
    DEEPFIELD_SCENE_PARALLAX = 0,
    DEEPFIELD_SCENE_FLYTHROUGH = 1
} deepfield_scene_mode;

typedef enum deepfield_density_mode_tag {
    DEEPFIELD_DENSITY_SPARSE = 0,
    DEEPFIELD_DENSITY_STANDARD = 1,
    DEEPFIELD_DENSITY_RICH = 2
} deepfield_density_mode;

typedef enum deepfield_speed_mode_tag {
    DEEPFIELD_SPEED_CALM = 0,
    DEEPFIELD_SPEED_CRUISE = 1,
    DEEPFIELD_SPEED_SURGE = 2
} deepfield_speed_mode;

typedef enum deepfield_camera_mode_tag {
    DEEPFIELD_CAMERA_OBSERVE = 0,
    DEEPFIELD_CAMERA_DRIFT = 1,
    DEEPFIELD_CAMERA_ARC = 2
} deepfield_camera_mode;

typedef enum deepfield_pulse_mode_tag {
    DEEPFIELD_PULSE_NONE = 0,
    DEEPFIELD_PULSE_SOFT = 1,
    DEEPFIELD_PULSE_WARP = 2
} deepfield_pulse_mode;

typedef struct deepfield_config_tag {
    int scene_mode;
    int density_mode;
    int speed_mode;
    int camera_mode;
    int pulse_mode;
} deepfield_config;

typedef struct deepfield_preset_values_tag {
    int scene_mode;
    int density_mode;
    int speed_mode;
    int camera_mode;
    int pulse_mode;
} deepfield_preset_values;

typedef struct deepfield_rng_state_tag {
    unsigned long state;
} deepfield_rng_state;

typedef struct deepfield_star_tag {
    long x;
    long y;
    long z;
    unsigned char layer;
    unsigned char twinkle;
} deepfield_star;

extern const screensave_preset_descriptor g_deepfield_presets[];
extern const screensave_theme_descriptor g_deepfield_themes[];

struct screensave_saver_session_tag {
    deepfield_config config;
    const screensave_theme_descriptor *theme;
    screensave_sizei drawable_size;
    screensave_detail_level detail_level;
    int preview_mode;
    deepfield_rng_state rng;
    unsigned int star_count;
    deepfield_star stars[DEEPFIELD_MAX_STARS];
    unsigned long camera_phase_millis;
    unsigned long twinkle_elapsed_millis;
    unsigned long variation_elapsed_millis;
    unsigned long pulse_elapsed_millis;
    unsigned long pulse_remaining_millis;
};

const screensave_saver_module *deepfield_get_module(void);

const screensave_preset_descriptor *deepfield_get_presets(unsigned int *count_out);
const screensave_theme_descriptor *deepfield_get_themes(unsigned int *count_out);
const deepfield_preset_values *deepfield_find_preset_values(const char *preset_key);
const screensave_theme_descriptor *deepfield_find_theme_descriptor(const char *theme_key);

void deepfield_config_set_defaults(
    screensave_common_config *common_config,
    void *product_config,
    unsigned int product_config_size
);
void deepfield_config_clamp(
    screensave_common_config *common_config,
    void *product_config,
    unsigned int product_config_size
);
int deepfield_config_load(
    const screensave_saver_module *module,
    screensave_common_config *common_config,
    void *product_config,
    unsigned int product_config_size,
    screensave_diag_context *diagnostics
);
int deepfield_config_save(
    const screensave_saver_module *module,
    const screensave_common_config *common_config,
    const void *product_config,
    unsigned int product_config_size,
    screensave_diag_context *diagnostics
);
INT_PTR deepfield_config_show_dialog(
    HINSTANCE instance,
    HWND owner_window,
    const screensave_saver_module *module,
    screensave_common_config *common_config,
    void *product_config,
    unsigned int product_config_size,
    screensave_diag_context *diagnostics
);
int deepfield_config_export_settings_entries(
    const screensave_saver_module *module,
    const screensave_common_config *common_config,
    const void *product_config,
    unsigned int product_config_size,
    screensave_settings_file_kind kind,
    screensave_settings_writer *writer,
    screensave_diag_context *diagnostics
);
int deepfield_config_import_settings_entry(
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
void deepfield_config_randomize_settings(
    const screensave_saver_module *module,
    screensave_common_config *common_config,
    void *product_config,
    unsigned int product_config_size,
    const screensave_session_seed *seed,
    screensave_diag_context *diagnostics
);

int deepfield_create_session(
    const screensave_saver_module *module,
    screensave_saver_session **session,
    const screensave_saver_environment *environment
);
void deepfield_destroy_session(screensave_saver_session *session);
void deepfield_resize_session(
    screensave_saver_session *session,
    const screensave_saver_environment *environment
);
void deepfield_step_session(
    screensave_saver_session *session,
    const screensave_saver_environment *environment
);
void deepfield_render_session(
    screensave_saver_session *session,
    const screensave_saver_environment *environment
);

void deepfield_rng_seed(deepfield_rng_state *state, unsigned long seed);
unsigned long deepfield_rng_next(deepfield_rng_state *state);
unsigned long deepfield_rng_range(deepfield_rng_state *state, unsigned long upper_bound);

const char *deepfield_scene_mode_name(int scene_mode);
const char *deepfield_density_mode_name(int density_mode);
const char *deepfield_speed_mode_name(int speed_mode);
const char *deepfield_camera_mode_name(int camera_mode);
const char *deepfield_pulse_mode_name(int pulse_mode);

void deepfield_apply_preset_to_config(
    const char *preset_key,
    screensave_common_config *common_config,
    deepfield_config *product_config
);

#endif /* DEEPFIELD_INTERNAL_H */
