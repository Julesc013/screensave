#ifndef NOCTURNE_INTERNAL_H
#define NOCTURNE_INTERNAL_H

#include <windows.h>

#include "screensave/config_api.h"
#include "screensave/diagnostics_api.h"
#include "screensave/saver_api.h"

#define NOCTURNE_PRODUCT_REGISTRY_ROOTA "Software\\Julesc013\\ScreenSave\\Products\\nocturne"
#define NOCTURNE_DEFAULT_PRESET_KEY "observatory_night"
#define NOCTURNE_DEFAULT_THEME_KEY "gray_black"
#define NOCTURNE_PRESET_COUNT 9U
#define NOCTURNE_THEME_COUNT 7U

typedef enum nocturne_motion_mode_tag {
    NOCTURNE_MOTION_NONE = 0,
    NOCTURNE_MOTION_DRIFT_MARK = 1,
    NOCTURNE_MOTION_QUIET_LINE = 2,
    NOCTURNE_MOTION_MONOLITH = 3,
    NOCTURNE_MOTION_BREATH = 4
} nocturne_motion_mode;

typedef enum nocturne_fade_speed_tag {
    NOCTURNE_FADE_SLOW = 0,
    NOCTURNE_FADE_STANDARD = 1,
    NOCTURNE_FADE_GENTLE = 2
} nocturne_fade_speed;

typedef enum nocturne_motion_strength_tag {
    NOCTURNE_STRENGTH_STILL = 0,
    NOCTURNE_STRENGTH_SUBTLE = 1,
    NOCTURNE_STRENGTH_SOFT = 2
} nocturne_motion_strength;

typedef struct nocturne_config_tag {
    int motion_mode;
    int fade_speed;
    int motion_strength;
} nocturne_config;

typedef struct nocturne_preset_values_tag {
    int motion_mode;
    int fade_speed;
    int motion_strength;
} nocturne_preset_values;

extern const screensave_preset_descriptor g_nocturne_presets[];
extern const screensave_theme_descriptor g_nocturne_themes[];

typedef struct nocturne_rng_state_tag {
    unsigned long state;
} nocturne_rng_state;

struct screensave_saver_session_tag {
    nocturne_config config;
    const screensave_theme_descriptor *theme;
    screensave_sizei drawable_size;
    screensave_detail_level detail_level;
    int preview_mode;
    nocturne_rng_state rng;
    unsigned long cycle_index;
    unsigned long cycle_duration_millis;
    unsigned long stage_elapsed_millis;
    unsigned long drift_refresh_millis;
    unsigned long reseed_count;
    int stage;
    int fade_level;
    long primary_x;
    long primary_y;
    long primary_vx;
    long primary_vy;
    long secondary_x;
    long secondary_y;
    long secondary_vx;
    long secondary_vy;
    int breath_direction;
    int breath_level;
    int ambient_level;
};

const screensave_saver_module *nocturne_get_module(void);

const screensave_preset_descriptor *nocturne_get_presets(unsigned int *count_out);
const screensave_theme_descriptor *nocturne_get_themes(unsigned int *count_out);
const nocturne_preset_values *nocturne_find_preset_values(const char *preset_key);
const screensave_theme_descriptor *nocturne_find_theme_descriptor(const char *theme_key);

void nocturne_config_set_defaults(
    screensave_common_config *common_config,
    void *product_config,
    unsigned int product_config_size
);
void nocturne_config_clamp(
    screensave_common_config *common_config,
    void *product_config,
    unsigned int product_config_size
);
int nocturne_config_load(
    const screensave_saver_module *module,
    screensave_common_config *common_config,
    void *product_config,
    unsigned int product_config_size,
    screensave_diag_context *diagnostics
);
int nocturne_config_save(
    const screensave_saver_module *module,
    const screensave_common_config *common_config,
    const void *product_config,
    unsigned int product_config_size,
    screensave_diag_context *diagnostics
);
INT_PTR nocturne_config_show_dialog(
    HINSTANCE instance,
    HWND owner_window,
    const screensave_saver_module *module,
    screensave_common_config *common_config,
    void *product_config,
    unsigned int product_config_size,
    screensave_diag_context *diagnostics
);
int nocturne_config_export_settings_entries(
    const screensave_saver_module *module,
    const screensave_common_config *common_config,
    const void *product_config,
    unsigned int product_config_size,
    screensave_settings_file_kind kind,
    screensave_settings_writer *writer,
    screensave_diag_context *diagnostics
);
int nocturne_config_import_settings_entry(
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
void nocturne_config_randomize_settings(
    const screensave_saver_module *module,
    screensave_common_config *common_config,
    void *product_config,
    unsigned int product_config_size,
    const screensave_session_seed *seed,
    screensave_diag_context *diagnostics
);

int nocturne_create_session(
    const screensave_saver_module *module,
    screensave_saver_session **session,
    const screensave_saver_environment *environment
);
void nocturne_destroy_session(screensave_saver_session *session);
void nocturne_resize_session(
    screensave_saver_session *session,
    const screensave_saver_environment *environment
);
void nocturne_step_session(
    screensave_saver_session *session,
    const screensave_saver_environment *environment
);
void nocturne_render_session(
    screensave_saver_session *session,
    const screensave_saver_environment *environment
);

void nocturne_rng_seed(nocturne_rng_state *state, unsigned long seed);
unsigned long nocturne_rng_next(nocturne_rng_state *state);
unsigned long nocturne_rng_range(nocturne_rng_state *state, unsigned long upper_bound);

const char *nocturne_motion_mode_name(int motion_mode);
const char *nocturne_fade_speed_name(int fade_speed);
const char *nocturne_motion_strength_name(int motion_strength);

void nocturne_apply_preset_to_config(
    const char *preset_key,
    screensave_common_config *common_config,
    nocturne_config *product_config
);

#endif /* NOCTURNE_INTERNAL_H */
