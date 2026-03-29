#ifndef VECTOR_WORLDS_INTERNAL_H
#define VECTOR_WORLDS_INTERNAL_H

#include <windows.h>

#include "screensave/config_api.h"
#include "screensave/diagnostics_api.h"
#include "screensave/saver_api.h"

#define VECTOR_WORLDS_PRODUCT_REGISTRY_ROOTA "Software\\Julesc013\\ScreenSave\\Products\\vector_worlds"
#define VECTOR_WORLDS_DEFAULT_PRESET_KEY "quiet_vector_hall"
#define VECTOR_WORLDS_DEFAULT_THEME_KEY "quiet_midnight_grid"
#define VECTOR_WORLDS_PRESET_COUNT 5U
#define VECTOR_WORLDS_THEME_COUNT 5U
#define VECTOR_WORLDS_MAX_OBJECTS 18U

typedef enum vector_worlds_scene_mode_tag {
    VECTOR_WORLDS_SCENE_FIELD = 0,
    VECTOR_WORLDS_SCENE_TUNNEL = 1,
    VECTOR_WORLDS_SCENE_TERRAIN = 2
} vector_worlds_scene_mode;

typedef enum vector_worlds_speed_mode_tag {
    VECTOR_WORLDS_SPEED_CALM = 0,
    VECTOR_WORLDS_SPEED_STANDARD = 1,
    VECTOR_WORLDS_SPEED_SURGE = 2
} vector_worlds_speed_mode;

typedef enum vector_worlds_detail_mode_tag {
    VECTOR_WORLDS_DETAIL_LOW = SCREENSAVE_DETAIL_LEVEL_LOW,
    VECTOR_WORLDS_DETAIL_STANDARD = SCREENSAVE_DETAIL_LEVEL_STANDARD,
    VECTOR_WORLDS_DETAIL_HIGH = SCREENSAVE_DETAIL_LEVEL_HIGH
} vector_worlds_detail_mode;

typedef enum vector_worlds_object_kind_tag {
    VECTOR_WORLDS_OBJECT_CUBE = 0,
    VECTOR_WORLDS_OBJECT_PYRAMID = 1,
    VECTOR_WORLDS_OBJECT_GATE = 2,
    VECTOR_WORLDS_OBJECT_PILLAR = 3
} vector_worlds_object_kind;

typedef struct vector_worlds_config_tag {
    int scene_mode;
    int speed_mode;
    int detail_mode;
} vector_worlds_config;

typedef struct vector_worlds_preset_values_tag {
    int scene_mode;
    int speed_mode;
    int detail_mode;
} vector_worlds_preset_values;

typedef struct vector_worlds_rng_state_tag {
    unsigned long state;
} vector_worlds_rng_state;

typedef struct vector_worlds_object_tag {
    int x;
    int y;
    int z;
    int scale;
    int twist;
    int kind;
    unsigned int phase;
} vector_worlds_object;

extern const screensave_preset_descriptor g_vector_worlds_presets[];
extern const screensave_theme_descriptor g_vector_worlds_themes[];

struct screensave_saver_session_tag {
    vector_worlds_config config;
    const screensave_theme_descriptor *theme;
    screensave_sizei drawable_size;
    int preview_mode;
    vector_worlds_rng_state rng;
    unsigned long step_accumulator;
    unsigned long camera_phase;
    unsigned long route_phase;
    unsigned long event_phase;
    unsigned long scene_seed;
    unsigned long scene_variant;
    unsigned int object_count;
    vector_worlds_object objects[VECTOR_WORLDS_MAX_OBJECTS];
};

const screensave_saver_module *vector_worlds_get_module(void);

const screensave_preset_descriptor *vector_worlds_get_presets(unsigned int *count_out);
const screensave_theme_descriptor *vector_worlds_get_themes(unsigned int *count_out);
const vector_worlds_preset_values *vector_worlds_find_preset_values(const char *preset_key);
const screensave_theme_descriptor *vector_worlds_find_theme_descriptor(const char *theme_key);

void vector_worlds_config_set_defaults(
    screensave_common_config *common_config,
    void *product_config,
    unsigned int product_config_size
);
void vector_worlds_config_clamp(
    screensave_common_config *common_config,
    void *product_config,
    unsigned int product_config_size
);
int vector_worlds_config_load(
    const screensave_saver_module *module,
    screensave_common_config *common_config,
    void *product_config,
    unsigned int product_config_size,
    screensave_diag_context *diagnostics
);
int vector_worlds_config_save(
    const screensave_saver_module *module,
    const screensave_common_config *common_config,
    const void *product_config,
    unsigned int product_config_size,
    screensave_diag_context *diagnostics
);
INT_PTR vector_worlds_config_show_dialog(
    HINSTANCE instance,
    HWND owner_window,
    const screensave_saver_module *module,
    screensave_common_config *common_config,
    void *product_config,
    unsigned int product_config_size,
    screensave_diag_context *diagnostics
);

int vector_worlds_create_session(
    const screensave_saver_module *module,
    screensave_saver_session **session,
    const screensave_saver_environment *environment
);
void vector_worlds_destroy_session(screensave_saver_session *session);
void vector_worlds_resize_session(
    screensave_saver_session *session,
    const screensave_saver_environment *environment
);
void vector_worlds_step_session(
    screensave_saver_session *session,
    const screensave_saver_environment *environment
);
void vector_worlds_render_session(
    screensave_saver_session *session,
    const screensave_saver_environment *environment
);

void vector_worlds_rng_seed(vector_worlds_rng_state *state, unsigned long seed);
unsigned long vector_worlds_rng_next(vector_worlds_rng_state *state);
unsigned long vector_worlds_rng_range(vector_worlds_rng_state *state, unsigned long upper_bound);

const char *vector_worlds_scene_mode_name(int scene_mode);
const char *vector_worlds_speed_mode_name(int speed_mode);
const char *vector_worlds_detail_mode_name(int detail_mode);

void vector_worlds_apply_preset_to_config(
    const char *preset_key,
    screensave_common_config *common_config,
    vector_worlds_config *product_config
);

#endif /* VECTOR_WORLDS_INTERNAL_H */
