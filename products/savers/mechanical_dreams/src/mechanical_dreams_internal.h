#ifndef MECHANICAL_DREAMS_INTERNAL_H
#define MECHANICAL_DREAMS_INTERNAL_H

#include <windows.h>

#include "screensave/config_api.h"
#include "screensave/diagnostics_api.h"
#include "screensave/saver_api.h"

#define MECHANICAL_DREAMS_PRODUCT_REGISTRY_ROOTA "Software\\Julesc013\\ScreenSave\\Products\\mechanical_dreams"
#define MECHANICAL_DREAMS_DEFAULT_PRESET_KEY "brass_gear_train"
#define MECHANICAL_DREAMS_DEFAULT_THEME_KEY "brass_workshop"
#define MECHANICAL_DREAMS_PRESET_COUNT 5U
#define MECHANICAL_DREAMS_THEME_COUNT 5U
#define MECHANICAL_DREAMS_MAX_GEARS 6U
#define MECHANICAL_DREAMS_MAX_DIALS 4U
#define MECHANICAL_DREAMS_MAX_FOLLOWERS 5U

typedef enum mechanical_dreams_scene_mode_tag {
    MECHANICAL_DREAMS_SCENE_GEAR_TRAIN = 0,
    MECHANICAL_DREAMS_SCENE_CAM_BANK = 1,
    MECHANICAL_DREAMS_SCENE_DIAL_ASSEMBLY = 2
} mechanical_dreams_scene_mode;

typedef enum mechanical_dreams_speed_mode_tag {
    MECHANICAL_DREAMS_SPEED_PATIENT = 0,
    MECHANICAL_DREAMS_SPEED_STANDARD = 1,
    MECHANICAL_DREAMS_SPEED_BRISK = 2
} mechanical_dreams_speed_mode;

typedef enum mechanical_dreams_density_mode_tag {
    MECHANICAL_DREAMS_DENSITY_SPARSE = 0,
    MECHANICAL_DREAMS_DENSITY_STANDARD = 1,
    MECHANICAL_DREAMS_DENSITY_DENSE = 2
} mechanical_dreams_density_mode;

typedef struct mechanical_dreams_config_tag {
    int scene_mode;
    int speed_mode;
    int density_mode;
} mechanical_dreams_config;

typedef struct mechanical_dreams_preset_values_tag {
    int scene_mode;
    int speed_mode;
    int density_mode;
} mechanical_dreams_preset_values;

typedef struct mechanical_dreams_rng_state_tag {
    unsigned long state;
} mechanical_dreams_rng_state;

typedef struct mechanical_dreams_gear_tag {
    int x;
    int y;
    int radius;
    int direction;
    int active;
    unsigned int phase;
} mechanical_dreams_gear;

typedef struct mechanical_dreams_dial_tag {
    int x;
    int y;
    int radius;
    int active;
    unsigned int angle;
    unsigned int target_angle;
} mechanical_dreams_dial;

extern const screensave_preset_descriptor g_mechanical_dreams_presets[];
extern const screensave_theme_descriptor g_mechanical_dreams_themes[];

struct screensave_saver_session_tag {
    mechanical_dreams_config config;
    const screensave_theme_descriptor *theme;
    screensave_sizei drawable_size;
    int preview_mode;
    mechanical_dreams_rng_state rng;
    unsigned long step_accumulator;
    unsigned long event_accumulator;
    unsigned int master_phase;
    unsigned int event_pulse;
    mechanical_dreams_gear gears[MECHANICAL_DREAMS_MAX_GEARS];
    mechanical_dreams_dial dials[MECHANICAL_DREAMS_MAX_DIALS];
    int follower_heights[MECHANICAL_DREAMS_MAX_FOLLOWERS];
    int follower_targets[MECHANICAL_DREAMS_MAX_FOLLOWERS];
    unsigned int counter_window;
};

const screensave_saver_module *mechanical_dreams_get_module(void);

const screensave_preset_descriptor *mechanical_dreams_get_presets(unsigned int *count_out);
const screensave_theme_descriptor *mechanical_dreams_get_themes(unsigned int *count_out);
const mechanical_dreams_preset_values *mechanical_dreams_find_preset_values(const char *preset_key);
const screensave_theme_descriptor *mechanical_dreams_find_theme_descriptor(const char *theme_key);

void mechanical_dreams_config_set_defaults(
    screensave_common_config *common_config,
    void *product_config,
    unsigned int product_config_size
);
void mechanical_dreams_config_clamp(
    screensave_common_config *common_config,
    void *product_config,
    unsigned int product_config_size
);
int mechanical_dreams_config_load(
    const screensave_saver_module *module,
    screensave_common_config *common_config,
    void *product_config,
    unsigned int product_config_size,
    screensave_diag_context *diagnostics
);
int mechanical_dreams_config_save(
    const screensave_saver_module *module,
    const screensave_common_config *common_config,
    const void *product_config,
    unsigned int product_config_size,
    screensave_diag_context *diagnostics
);
INT_PTR mechanical_dreams_config_show_dialog(
    HINSTANCE instance,
    HWND owner_window,
    const screensave_saver_module *module,
    screensave_common_config *common_config,
    void *product_config,
    unsigned int product_config_size,
    screensave_diag_context *diagnostics
);

int mechanical_dreams_create_session(
    const screensave_saver_module *module,
    screensave_saver_session **session,
    const screensave_saver_environment *environment
);
void mechanical_dreams_destroy_session(screensave_saver_session *session);
void mechanical_dreams_resize_session(
    screensave_saver_session *session,
    const screensave_saver_environment *environment
);
void mechanical_dreams_step_session(
    screensave_saver_session *session,
    const screensave_saver_environment *environment
);
void mechanical_dreams_render_session(
    screensave_saver_session *session,
    const screensave_saver_environment *environment
);

void mechanical_dreams_rng_seed(mechanical_dreams_rng_state *state, unsigned long seed);
unsigned long mechanical_dreams_rng_next(mechanical_dreams_rng_state *state);
unsigned long mechanical_dreams_rng_range(mechanical_dreams_rng_state *state, unsigned long upper_bound);

const char *mechanical_dreams_scene_mode_name(int scene_mode);
const char *mechanical_dreams_speed_mode_name(int speed_mode);
const char *mechanical_dreams_density_mode_name(int density_mode);

void mechanical_dreams_apply_preset_to_config(
    const char *preset_key,
    screensave_common_config *common_config,
    mechanical_dreams_config *product_config
);

#endif /* MECHANICAL_DREAMS_INTERNAL_H */
