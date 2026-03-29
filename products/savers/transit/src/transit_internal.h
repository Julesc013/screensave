#ifndef TRANSIT_INTERNAL_H
#define TRANSIT_INTERNAL_H

#include <windows.h>

#include "screensave/config_api.h"
#include "screensave/diagnostics_api.h"
#include "screensave/saver_api.h"

#define TRANSIT_PRODUCT_REGISTRY_ROOTA "Software\\Julesc013\\ScreenSave\\Products\\transit"
#define TRANSIT_DEFAULT_PRESET_KEY "wet_motorway"
#define TRANSIT_DEFAULT_THEME_KEY "wet_motorway"
#define TRANSIT_PRESET_COUNT 5U
#define TRANSIT_THEME_COUNT 5U
#define TRANSIT_MAX_LIGHTS 64U

typedef enum transit_scene_mode_tag {
    TRANSIT_SCENE_MOTORWAY = 0,
    TRANSIT_SCENE_RAIL = 1,
    TRANSIT_SCENE_HARBOR = 2
} transit_scene_mode;

typedef enum transit_speed_mode_tag {
    TRANSIT_SPEED_GLIDE = 0,
    TRANSIT_SPEED_CRUISE = 1,
    TRANSIT_SPEED_EXPRESS = 2
} transit_speed_mode;

typedef enum transit_light_mode_tag {
    TRANSIT_LIGHTS_SPARSE = 0,
    TRANSIT_LIGHTS_STANDARD = 1,
    TRANSIT_LIGHTS_DENSE = 2
} transit_light_mode;

typedef struct transit_config_tag {
    int scene_mode;
    int speed_mode;
    int light_mode;
} transit_config;

typedef struct transit_preset_values_tag {
    int scene_mode;
    int speed_mode;
    int light_mode;
} transit_preset_values;

typedef struct transit_rng_state_tag {
    unsigned long state;
} transit_rng_state;

typedef struct transit_light_tag {
    int active;
    int lane;
    int depth_fixed;
    int class_id;
    int brightness;
    unsigned int phase;
} transit_light;

extern const screensave_preset_descriptor g_transit_presets[];
extern const screensave_theme_descriptor g_transit_themes[];

struct screensave_saver_session_tag {
    transit_config config;
    const screensave_theme_descriptor *theme;
    screensave_sizei drawable_size;
    int preview_mode;
    transit_rng_state rng;
    unsigned long step_accumulator;
    unsigned long event_accumulator;
    unsigned int route_phase;
    unsigned int sway_phase;
    unsigned int event_pulse;
    transit_light lights[TRANSIT_MAX_LIGHTS];
    unsigned int light_count;
};

const screensave_saver_module *transit_get_module(void);
const screensave_preset_descriptor *transit_get_presets(unsigned int *count_out);
const screensave_theme_descriptor *transit_get_themes(unsigned int *count_out);
const transit_preset_values *transit_find_preset_values(const char *preset_key);
const screensave_theme_descriptor *transit_find_theme_descriptor(const char *theme_key);

void transit_config_set_defaults(
    screensave_common_config *common_config,
    void *product_config,
    unsigned int product_config_size
);
void transit_config_clamp(
    screensave_common_config *common_config,
    void *product_config,
    unsigned int product_config_size
);
int transit_config_load(
    const screensave_saver_module *module,
    screensave_common_config *common_config,
    void *product_config,
    unsigned int product_config_size,
    screensave_diag_context *diagnostics
);
int transit_config_save(
    const screensave_saver_module *module,
    const screensave_common_config *common_config,
    const void *product_config,
    unsigned int product_config_size,
    screensave_diag_context *diagnostics
);
INT_PTR transit_config_show_dialog(
    HINSTANCE instance,
    HWND owner_window,
    const screensave_saver_module *module,
    screensave_common_config *common_config,
    void *product_config,
    unsigned int product_config_size,
    screensave_diag_context *diagnostics
);

int transit_create_session(
    const screensave_saver_module *module,
    screensave_saver_session **session,
    const screensave_saver_environment *environment
);
void transit_destroy_session(screensave_saver_session *session);
void transit_resize_session(
    screensave_saver_session *session,
    const screensave_saver_environment *environment
);
void transit_step_session(
    screensave_saver_session *session,
    const screensave_saver_environment *environment
);
void transit_render_session(
    screensave_saver_session *session,
    const screensave_saver_environment *environment
);

void transit_rng_seed(transit_rng_state *state, unsigned long seed);
unsigned long transit_rng_next(transit_rng_state *state);
unsigned long transit_rng_range(transit_rng_state *state, unsigned long upper_bound);

const char *transit_scene_mode_name(int scene_mode);
const char *transit_speed_mode_name(int speed_mode);
const char *transit_light_mode_name(int light_mode);
void transit_apply_preset_to_config(
    const char *preset_key,
    screensave_common_config *common_config,
    transit_config *product_config
);

#endif /* TRANSIT_INTERNAL_H */
