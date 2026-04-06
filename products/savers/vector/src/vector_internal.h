#ifndef VECTOR_INTERNAL_H
#define VECTOR_INTERNAL_H

#include <windows.h>

#include "screensave/config_api.h"
#include "screensave/diagnostics_api.h"
#include "screensave/saver_api.h"

#define VECTOR_PRODUCT_REGISTRY_ROOTA "Software\\Julesc013\\ScreenSave\\Products\\vector"
#define VECTOR_DEFAULT_PRESET_KEY "quiet_vector_hall"
#define VECTOR_DEFAULT_THEME_KEY "quiet_midnight_grid"
#define VECTOR_PRESET_COUNT 7U
#define VECTOR_THEME_COUNT 7U
#define VECTOR_MAX_OBJECTS 18U

typedef enum vector_scene_mode_tag {
    VECTOR_SCENE_FIELD = 0,
    VECTOR_SCENE_TUNNEL = 1,
    VECTOR_SCENE_TERRAIN = 2
} vector_scene_mode;

typedef enum vector_speed_mode_tag {
    VECTOR_SPEED_CALM = 0,
    VECTOR_SPEED_STANDARD = 1,
    VECTOR_SPEED_SURGE = 2
} vector_speed_mode;

typedef enum vector_detail_mode_tag {
    VECTOR_DETAIL_LOW = SCREENSAVE_DETAIL_LEVEL_LOW,
    VECTOR_DETAIL_STANDARD = SCREENSAVE_DETAIL_LEVEL_STANDARD,
    VECTOR_DETAIL_HIGH = SCREENSAVE_DETAIL_LEVEL_HIGH
} vector_detail_mode;

typedef enum vector_object_kind_tag {
    VECTOR_OBJECT_CUBE = 0,
    VECTOR_OBJECT_PYRAMID = 1,
    VECTOR_OBJECT_GATE = 2,
    VECTOR_OBJECT_PILLAR = 3
} vector_object_kind;

typedef struct vector_config_tag {
    int scene_mode;
    int speed_mode;
    int detail_mode;
} vector_config;

typedef struct vector_preset_values_tag {
    int scene_mode;
    int speed_mode;
    int detail_mode;
} vector_preset_values;

typedef struct vector_rng_state_tag {
    unsigned long state;
} vector_rng_state;

typedef struct vector_object_tag {
    int x;
    int y;
    int z;
    int scale;
    int twist;
    int kind;
    unsigned int phase;
} vector_object;

extern const screensave_preset_descriptor g_vector_presets[];
extern const screensave_theme_descriptor g_vector_themes[];

struct screensave_saver_session_tag {
    vector_config config;
    const screensave_theme_descriptor *theme;
    screensave_sizei drawable_size;
    int preview_mode;
    vector_rng_state rng;
    unsigned long step_accumulator;
    unsigned long camera_phase;
    unsigned long route_phase;
    unsigned long event_phase;
    unsigned long scene_seed;
    unsigned long scene_variant;
    unsigned int object_count;
    vector_object objects[VECTOR_MAX_OBJECTS];
};

const screensave_saver_module *vector_get_module(void);

const screensave_preset_descriptor *vector_get_presets(unsigned int *count_out);
const screensave_theme_descriptor *vector_get_themes(unsigned int *count_out);
const vector_preset_values *vector_find_preset_values(const char *preset_key);
const screensave_theme_descriptor *vector_find_theme_descriptor(const char *theme_key);

void vector_config_set_defaults(
    screensave_common_config *common_config,
    void *product_config,
    unsigned int product_config_size
);
void vector_config_clamp(
    screensave_common_config *common_config,
    void *product_config,
    unsigned int product_config_size
);
int vector_config_load(
    const screensave_saver_module *module,
    screensave_common_config *common_config,
    void *product_config,
    unsigned int product_config_size,
    screensave_diag_context *diagnostics
);
int vector_config_save(
    const screensave_saver_module *module,
    const screensave_common_config *common_config,
    const void *product_config,
    unsigned int product_config_size,
    screensave_diag_context *diagnostics
);
INT_PTR vector_config_show_dialog(
    HINSTANCE instance,
    HWND owner_window,
    const screensave_saver_module *module,
    screensave_common_config *common_config,
    void *product_config,
    unsigned int product_config_size,
    screensave_diag_context *diagnostics
);
int vector_config_export_settings_entries(
    const screensave_saver_module *module,
    const screensave_common_config *common_config,
    const void *product_config,
    unsigned int product_config_size,
    screensave_settings_file_kind kind,
    screensave_settings_writer *writer,
    screensave_diag_context *diagnostics
);
int vector_config_import_settings_entry(
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
void vector_config_randomize_settings(
    const screensave_saver_module *module,
    screensave_common_config *common_config,
    void *product_config,
    unsigned int product_config_size,
    const screensave_session_seed *seed,
    screensave_diag_context *diagnostics
);

int vector_create_session(
    const screensave_saver_module *module,
    screensave_saver_session **session,
    const screensave_saver_environment *environment
);
void vector_destroy_session(screensave_saver_session *session);
void vector_resize_session(
    screensave_saver_session *session,
    const screensave_saver_environment *environment
);
void vector_step_session(
    screensave_saver_session *session,
    const screensave_saver_environment *environment
);
void vector_render_session(
    screensave_saver_session *session,
    const screensave_saver_environment *environment
);

unsigned long vector_hash(unsigned long value);
int vector_signed_range(unsigned long value, int magnitude);
void vector_rng_seed(vector_rng_state *state, unsigned long seed);
unsigned long vector_rng_next(vector_rng_state *state);
unsigned long vector_rng_range(vector_rng_state *state, unsigned long upper_bound);

const char *vector_scene_mode_name(int scene_mode);
const char *vector_speed_mode_name(int speed_mode);
const char *vector_detail_mode_name(int detail_mode);

void vector_apply_preset_to_config(
    const char *preset_key,
    screensave_common_config *common_config,
    vector_config *product_config
);

#endif /* VECTOR_INTERNAL_H */
