#ifndef OBSERVATORY_INTERNAL_H
#define OBSERVATORY_INTERNAL_H

#include <windows.h>

#include "screensave/config_api.h"
#include "screensave/diagnostics_api.h"
#include "screensave/saver_api.h"

#define OBSERVATORY_PRODUCT_REGISTRY_ROOTA "Software\\Julesc013\\ScreenSave\\Products\\observatory"
#define OBSERVATORY_DEFAULT_PRESET_KEY "brass_orrery"
#define OBSERVATORY_DEFAULT_THEME_KEY "brass_observatory"
#define OBSERVATORY_PRESET_COUNT 7U
#define OBSERVATORY_THEME_COUNT 7U
#define OBSERVATORY_MAX_BODIES 12U
#define OBSERVATORY_MAX_STARS 64U

typedef enum observatory_scene_mode_tag {
    OBSERVATORY_SCENE_ORRERY = 0,
    OBSERVATORY_SCENE_CHART_ROOM = 1,
    OBSERVATORY_SCENE_DOME = 2
} observatory_scene_mode;

typedef enum observatory_speed_mode_tag {
    OBSERVATORY_SPEED_STILL = 0,
    OBSERVATORY_SPEED_STANDARD = 1,
    OBSERVATORY_SPEED_BRISK = 2
} observatory_speed_mode;

typedef enum observatory_detail_mode_tag {
    OBSERVATORY_DETAIL_SPARSE = 0,
    OBSERVATORY_DETAIL_STANDARD = 1,
    OBSERVATORY_DETAIL_RICH = 2
} observatory_detail_mode;

typedef struct observatory_config_tag {
    int scene_mode;
    int speed_mode;
    int detail_mode;
} observatory_config;

typedef struct observatory_preset_values_tag {
    int scene_mode;
    int speed_mode;
    int detail_mode;
} observatory_preset_values;

typedef struct observatory_rng_state_tag {
    unsigned long state;
} observatory_rng_state;

typedef struct observatory_body_tag {
    int orbit_radius;
    unsigned int phase;
    unsigned int rate;
    int size;
    int brightness;
} observatory_body;

typedef struct observatory_star_tag {
    int x;
    int y;
    int brightness;
} observatory_star;

extern const screensave_preset_descriptor g_observatory_presets[];
extern const screensave_theme_descriptor g_observatory_themes[];

struct screensave_saver_session_tag {
    observatory_config config;
    const screensave_theme_descriptor *theme;
    screensave_sizei drawable_size;
    int preview_mode;
    observatory_rng_state rng;
    unsigned long step_accumulator;
    unsigned long event_accumulator;
    unsigned int ambient_phase;
    unsigned int sweep_phase;
    unsigned int central_pulse;
    observatory_body bodies[OBSERVATORY_MAX_BODIES];
    observatory_star stars[OBSERVATORY_MAX_STARS];
    unsigned int body_count;
    unsigned int star_count;
};

const screensave_saver_module *observatory_get_module(void);
const screensave_preset_descriptor *observatory_get_presets(unsigned int *count_out);
const screensave_theme_descriptor *observatory_get_themes(unsigned int *count_out);
const observatory_preset_values *observatory_find_preset_values(const char *preset_key);
const screensave_theme_descriptor *observatory_find_theme_descriptor(const char *theme_key);

void observatory_config_set_defaults(
    screensave_common_config *common_config,
    void *product_config,
    unsigned int product_config_size
);
void observatory_config_clamp(
    screensave_common_config *common_config,
    void *product_config,
    unsigned int product_config_size
);
int observatory_config_load(
    const screensave_saver_module *module,
    screensave_common_config *common_config,
    void *product_config,
    unsigned int product_config_size,
    screensave_diag_context *diagnostics
);
int observatory_config_save(
    const screensave_saver_module *module,
    const screensave_common_config *common_config,
    const void *product_config,
    unsigned int product_config_size,
    screensave_diag_context *diagnostics
);
INT_PTR observatory_config_show_dialog(
    HINSTANCE instance,
    HWND owner_window,
    const screensave_saver_module *module,
    screensave_common_config *common_config,
    void *product_config,
    unsigned int product_config_size,
    screensave_diag_context *diagnostics
);
int observatory_config_export_settings_entries(
    const screensave_saver_module *module,
    const screensave_common_config *common_config,
    const void *product_config,
    unsigned int product_config_size,
    screensave_settings_file_kind kind,
    screensave_settings_writer *writer,
    screensave_diag_context *diagnostics
);
int observatory_config_import_settings_entry(
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
void observatory_config_randomize_settings(
    const screensave_saver_module *module,
    screensave_common_config *common_config,
    void *product_config,
    unsigned int product_config_size,
    const screensave_session_seed *seed,
    screensave_diag_context *diagnostics
);

int observatory_create_session(
    const screensave_saver_module *module,
    screensave_saver_session **session,
    const screensave_saver_environment *environment
);
void observatory_destroy_session(screensave_saver_session *session);
void observatory_resize_session(
    screensave_saver_session *session,
    const screensave_saver_environment *environment
);
void observatory_step_session(
    screensave_saver_session *session,
    const screensave_saver_environment *environment
);
void observatory_render_session(
    screensave_saver_session *session,
    const screensave_saver_environment *environment
);

void observatory_rng_seed(observatory_rng_state *state, unsigned long seed);
unsigned long observatory_rng_next(observatory_rng_state *state);
unsigned long observatory_rng_range(observatory_rng_state *state, unsigned long upper_bound);

const char *observatory_scene_mode_name(int scene_mode);
const char *observatory_speed_mode_name(int speed_mode);
const char *observatory_detail_mode_name(int detail_mode);
void observatory_apply_preset_to_config(
    const char *preset_key,
    screensave_common_config *common_config,
    observatory_config *product_config
);

#endif /* OBSERVATORY_INTERNAL_H */
