#ifndef RETRO_EXPLORER_INTERNAL_H
#define RETRO_EXPLORER_INTERNAL_H

#include <windows.h>

#include "screensave/config_api.h"
#include "screensave/diagnostics_api.h"
#include "screensave/saver_api.h"

#define RETRO_EXPLORER_PRODUCT_REGISTRY_ROOTA "Software\\Julesc013\\ScreenSave\\Products\\retro_explorer"
#define RETRO_EXPLORER_DEFAULT_PRESET_KEY "quiet_night_run"
#define RETRO_EXPLORER_DEFAULT_THEME_KEY "quiet_night_run"
#define RETRO_EXPLORER_PRESET_COUNT 5U
#define RETRO_EXPLORER_THEME_COUNT 5U
#define RETRO_EXPLORER_MAX_SEGMENTS 16U

typedef enum retro_explorer_scene_mode_tag {
    RETRO_EXPLORER_SCENE_CORRIDOR = 0,
    RETRO_EXPLORER_SCENE_INDUSTRIAL = 1,
    RETRO_EXPLORER_SCENE_CANYON = 2
} retro_explorer_scene_mode;

typedef enum retro_explorer_speed_mode_tag {
    RETRO_EXPLORER_SPEED_CALM = 0,
    RETRO_EXPLORER_SPEED_STANDARD = 1,
    RETRO_EXPLORER_SPEED_BRISK = 2
} retro_explorer_speed_mode;

typedef struct retro_explorer_config_tag {
    int scene_mode;
    int speed_mode;
} retro_explorer_config;

typedef struct retro_explorer_preset_values_tag {
    int scene_mode;
    int speed_mode;
} retro_explorer_preset_values;

typedef struct retro_explorer_rng_state_tag {
    unsigned long state;
} retro_explorer_rng_state;

typedef struct retro_explorer_segment_tag {
    int variant;
    int width;
    int height;
    int curve;
    int light_interval;
    int sign_interval;
    int openness;
    unsigned int length_steps;
} retro_explorer_segment;

extern const screensave_preset_descriptor g_retro_explorer_presets[];
extern const screensave_theme_descriptor g_retro_explorer_themes[];

struct screensave_saver_session_tag {
    retro_explorer_config config;
    const screensave_theme_descriptor *theme;
    screensave_sizei drawable_size;
    screensave_detail_level detail_level;
    int preview_mode;
    retro_explorer_rng_state rng;
    unsigned long route_seed;
    unsigned long travel_millis;
    unsigned long world_cycle;
    unsigned long portal_pulse;
    unsigned int segment_count;
    unsigned int current_segment_index;
    unsigned int current_segment_step;
    int camera_offset;
    retro_explorer_segment segments[RETRO_EXPLORER_MAX_SEGMENTS];
};

const screensave_saver_module *retro_explorer_get_module(void);
const screensave_preset_descriptor *retro_explorer_get_presets(unsigned int *count_out);
const screensave_theme_descriptor *retro_explorer_get_themes(unsigned int *count_out);
const retro_explorer_preset_values *retro_explorer_find_preset_values(const char *preset_key);
const screensave_theme_descriptor *retro_explorer_find_theme_descriptor(const char *theme_key);

void retro_explorer_config_set_defaults(
    screensave_common_config *common_config,
    void *product_config,
    unsigned int product_config_size
);
void retro_explorer_config_clamp(
    screensave_common_config *common_config,
    void *product_config,
    unsigned int product_config_size
);
int retro_explorer_config_load(
    const screensave_saver_module *module,
    screensave_common_config *common_config,
    void *product_config,
    unsigned int product_config_size,
    screensave_diag_context *diagnostics
);
int retro_explorer_config_save(
    const screensave_saver_module *module,
    const screensave_common_config *common_config,
    const void *product_config,
    unsigned int product_config_size,
    screensave_diag_context *diagnostics
);
INT_PTR retro_explorer_config_show_dialog(
    HINSTANCE instance,
    HWND owner_window,
    const screensave_saver_module *module,
    screensave_common_config *common_config,
    void *product_config,
    unsigned int product_config_size,
    screensave_diag_context *diagnostics
);

int retro_explorer_create_session(
    const screensave_saver_module *module,
    screensave_saver_session **session,
    const screensave_saver_environment *environment
);
void retro_explorer_destroy_session(screensave_saver_session *session);
void retro_explorer_resize_session(
    screensave_saver_session *session,
    const screensave_saver_environment *environment
);
void retro_explorer_step_session(
    screensave_saver_session *session,
    const screensave_saver_environment *environment
);
void retro_explorer_render_session(
    screensave_saver_session *session,
    const screensave_saver_environment *environment
);

void retro_explorer_rng_seed(retro_explorer_rng_state *state, unsigned long seed);
unsigned long retro_explorer_rng_next(retro_explorer_rng_state *state);
unsigned long retro_explorer_rng_range(retro_explorer_rng_state *state, unsigned long upper_bound);

const char *retro_explorer_scene_mode_name(int scene_mode);
const char *retro_explorer_speed_mode_name(int speed_mode);
void retro_explorer_apply_preset_to_config(
    const char *preset_key,
    screensave_common_config *common_config,
    retro_explorer_config *product_config
);

#endif /* RETRO_EXPLORER_INTERNAL_H */
