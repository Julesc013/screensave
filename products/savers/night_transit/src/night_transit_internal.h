#ifndef NIGHT_TRANSIT_INTERNAL_H
#define NIGHT_TRANSIT_INTERNAL_H

#include <windows.h>

#include "screensave/config_api.h"
#include "screensave/diagnostics_api.h"
#include "screensave/saver_api.h"

#define NIGHT_TRANSIT_PRODUCT_REGISTRY_ROOTA "Software\\Julesc013\\ScreenSave\\Products\\night_transit"
#define NIGHT_TRANSIT_DEFAULT_PRESET_KEY "wet_motorway"
#define NIGHT_TRANSIT_DEFAULT_THEME_KEY "wet_motorway"
#define NIGHT_TRANSIT_PRESET_COUNT 5U
#define NIGHT_TRANSIT_THEME_COUNT 5U
#define NIGHT_TRANSIT_MAX_LIGHTS 64U

typedef enum night_transit_scene_mode_tag {
    NIGHT_TRANSIT_SCENE_MOTORWAY = 0,
    NIGHT_TRANSIT_SCENE_RAIL = 1,
    NIGHT_TRANSIT_SCENE_HARBOR = 2
} night_transit_scene_mode;

typedef enum night_transit_speed_mode_tag {
    NIGHT_TRANSIT_SPEED_GLIDE = 0,
    NIGHT_TRANSIT_SPEED_CRUISE = 1,
    NIGHT_TRANSIT_SPEED_EXPRESS = 2
} night_transit_speed_mode;

typedef enum night_transit_light_mode_tag {
    NIGHT_TRANSIT_LIGHTS_SPARSE = 0,
    NIGHT_TRANSIT_LIGHTS_STANDARD = 1,
    NIGHT_TRANSIT_LIGHTS_DENSE = 2
} night_transit_light_mode;

typedef struct night_transit_config_tag {
    int scene_mode;
    int speed_mode;
    int light_mode;
} night_transit_config;

typedef struct night_transit_preset_values_tag {
    int scene_mode;
    int speed_mode;
    int light_mode;
} night_transit_preset_values;

typedef struct night_transit_rng_state_tag {
    unsigned long state;
} night_transit_rng_state;

typedef struct night_transit_light_tag {
    int active;
    int lane;
    int depth_fixed;
    int class_id;
    int brightness;
    unsigned int phase;
} night_transit_light;

extern const screensave_preset_descriptor g_night_transit_presets[];
extern const screensave_theme_descriptor g_night_transit_themes[];

struct screensave_saver_session_tag {
    night_transit_config config;
    const screensave_theme_descriptor *theme;
    screensave_sizei drawable_size;
    int preview_mode;
    night_transit_rng_state rng;
    unsigned long step_accumulator;
    unsigned long event_accumulator;
    unsigned int route_phase;
    unsigned int sway_phase;
    unsigned int event_pulse;
    night_transit_light lights[NIGHT_TRANSIT_MAX_LIGHTS];
    unsigned int light_count;
};

const screensave_saver_module *night_transit_get_module(void);
const screensave_preset_descriptor *night_transit_get_presets(unsigned int *count_out);
const screensave_theme_descriptor *night_transit_get_themes(unsigned int *count_out);
const night_transit_preset_values *night_transit_find_preset_values(const char *preset_key);
const screensave_theme_descriptor *night_transit_find_theme_descriptor(const char *theme_key);

void night_transit_config_set_defaults(
    screensave_common_config *common_config,
    void *product_config,
    unsigned int product_config_size
);
void night_transit_config_clamp(
    screensave_common_config *common_config,
    void *product_config,
    unsigned int product_config_size
);
int night_transit_config_load(
    const screensave_saver_module *module,
    screensave_common_config *common_config,
    void *product_config,
    unsigned int product_config_size,
    screensave_diag_context *diagnostics
);
int night_transit_config_save(
    const screensave_saver_module *module,
    const screensave_common_config *common_config,
    const void *product_config,
    unsigned int product_config_size,
    screensave_diag_context *diagnostics
);
INT_PTR night_transit_config_show_dialog(
    HINSTANCE instance,
    HWND owner_window,
    const screensave_saver_module *module,
    screensave_common_config *common_config,
    void *product_config,
    unsigned int product_config_size,
    screensave_diag_context *diagnostics
);

int night_transit_create_session(
    const screensave_saver_module *module,
    screensave_saver_session **session,
    const screensave_saver_environment *environment
);
void night_transit_destroy_session(screensave_saver_session *session);
void night_transit_resize_session(
    screensave_saver_session *session,
    const screensave_saver_environment *environment
);
void night_transit_step_session(
    screensave_saver_session *session,
    const screensave_saver_environment *environment
);
void night_transit_render_session(
    screensave_saver_session *session,
    const screensave_saver_environment *environment
);

void night_transit_rng_seed(night_transit_rng_state *state, unsigned long seed);
unsigned long night_transit_rng_next(night_transit_rng_state *state);
unsigned long night_transit_rng_range(night_transit_rng_state *state, unsigned long upper_bound);

const char *night_transit_scene_mode_name(int scene_mode);
const char *night_transit_speed_mode_name(int speed_mode);
const char *night_transit_light_mode_name(int light_mode);
void night_transit_apply_preset_to_config(
    const char *preset_key,
    screensave_common_config *common_config,
    night_transit_config *product_config
);

#endif /* NIGHT_TRANSIT_INTERNAL_H */
