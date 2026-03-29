#ifndef MECHANIZE_INTERNAL_H
#define MECHANIZE_INTERNAL_H

#include <windows.h>

#include "screensave/config_api.h"
#include "screensave/diagnostics_api.h"
#include "screensave/saver_api.h"

#define MECHANIZE_PRODUCT_REGISTRY_ROOTA "Software\\Julesc013\\ScreenSave\\Products\\mechanize"
#define MECHANIZE_DEFAULT_PRESET_KEY "brass_gear_train"
#define MECHANIZE_DEFAULT_THEME_KEY "brass_workshop"
#define MECHANIZE_PRESET_COUNT 5U
#define MECHANIZE_THEME_COUNT 5U
#define MECHANIZE_MAX_GEARS 6U
#define MECHANIZE_MAX_DIALS 4U
#define MECHANIZE_MAX_FOLLOWERS 5U

typedef enum mechanize_scene_mode_tag {
    MECHANIZE_SCENE_GEAR_TRAIN = 0,
    MECHANIZE_SCENE_CAM_BANK = 1,
    MECHANIZE_SCENE_DIAL_ASSEMBLY = 2
} mechanize_scene_mode;

typedef enum mechanize_speed_mode_tag {
    MECHANIZE_SPEED_PATIENT = 0,
    MECHANIZE_SPEED_STANDARD = 1,
    MECHANIZE_SPEED_BRISK = 2
} mechanize_speed_mode;

typedef enum mechanize_density_mode_tag {
    MECHANIZE_DENSITY_SPARSE = 0,
    MECHANIZE_DENSITY_STANDARD = 1,
    MECHANIZE_DENSITY_DENSE = 2
} mechanize_density_mode;

typedef struct mechanize_config_tag {
    int scene_mode;
    int speed_mode;
    int density_mode;
} mechanize_config;

typedef struct mechanize_preset_values_tag {
    int scene_mode;
    int speed_mode;
    int density_mode;
} mechanize_preset_values;

typedef struct mechanize_rng_state_tag {
    unsigned long state;
} mechanize_rng_state;

typedef struct mechanize_gear_tag {
    int x;
    int y;
    int radius;
    int direction;
    int active;
    unsigned int phase;
} mechanize_gear;

typedef struct mechanize_dial_tag {
    int x;
    int y;
    int radius;
    int active;
    unsigned int angle;
    unsigned int target_angle;
} mechanize_dial;

extern const screensave_preset_descriptor g_mechanize_presets[];
extern const screensave_theme_descriptor g_mechanize_themes[];

struct screensave_saver_session_tag {
    mechanize_config config;
    const screensave_theme_descriptor *theme;
    screensave_sizei drawable_size;
    int preview_mode;
    mechanize_rng_state rng;
    unsigned long step_accumulator;
    unsigned long event_accumulator;
    unsigned int master_phase;
    unsigned int event_pulse;
    mechanize_gear gears[MECHANIZE_MAX_GEARS];
    mechanize_dial dials[MECHANIZE_MAX_DIALS];
    int follower_heights[MECHANIZE_MAX_FOLLOWERS];
    int follower_targets[MECHANIZE_MAX_FOLLOWERS];
    unsigned int counter_window;
};

const screensave_saver_module *mechanize_get_module(void);

const screensave_preset_descriptor *mechanize_get_presets(unsigned int *count_out);
const screensave_theme_descriptor *mechanize_get_themes(unsigned int *count_out);
const mechanize_preset_values *mechanize_find_preset_values(const char *preset_key);
const screensave_theme_descriptor *mechanize_find_theme_descriptor(const char *theme_key);

void mechanize_config_set_defaults(
    screensave_common_config *common_config,
    void *product_config,
    unsigned int product_config_size
);
void mechanize_config_clamp(
    screensave_common_config *common_config,
    void *product_config,
    unsigned int product_config_size
);
int mechanize_config_load(
    const screensave_saver_module *module,
    screensave_common_config *common_config,
    void *product_config,
    unsigned int product_config_size,
    screensave_diag_context *diagnostics
);
int mechanize_config_save(
    const screensave_saver_module *module,
    const screensave_common_config *common_config,
    const void *product_config,
    unsigned int product_config_size,
    screensave_diag_context *diagnostics
);
INT_PTR mechanize_config_show_dialog(
    HINSTANCE instance,
    HWND owner_window,
    const screensave_saver_module *module,
    screensave_common_config *common_config,
    void *product_config,
    unsigned int product_config_size,
    screensave_diag_context *diagnostics
);

int mechanize_create_session(
    const screensave_saver_module *module,
    screensave_saver_session **session,
    const screensave_saver_environment *environment
);
void mechanize_destroy_session(screensave_saver_session *session);
void mechanize_resize_session(
    screensave_saver_session *session,
    const screensave_saver_environment *environment
);
void mechanize_step_session(
    screensave_saver_session *session,
    const screensave_saver_environment *environment
);
void mechanize_render_session(
    screensave_saver_session *session,
    const screensave_saver_environment *environment
);

void mechanize_rng_seed(mechanize_rng_state *state, unsigned long seed);
unsigned long mechanize_rng_next(mechanize_rng_state *state);
unsigned long mechanize_rng_range(mechanize_rng_state *state, unsigned long upper_bound);

const char *mechanize_scene_mode_name(int scene_mode);
const char *mechanize_speed_mode_name(int speed_mode);
const char *mechanize_density_mode_name(int density_mode);

void mechanize_apply_preset_to_config(
    const char *preset_key,
    screensave_common_config *common_config,
    mechanize_config *product_config
);

#endif /* MECHANIZE_INTERNAL_H */
