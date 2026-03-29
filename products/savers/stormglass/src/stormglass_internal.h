#ifndef STORMGLASS_INTERNAL_H
#define STORMGLASS_INTERNAL_H

#include <windows.h>

#include "screensave/config_api.h"
#include "screensave/diagnostics_api.h"
#include "screensave/saver_api.h"

#define STORMGLASS_PRODUCT_REGISTRY_ROOTA "Software\\Julesc013\\ScreenSave\\Products\\stormglass"
#define STORMGLASS_DEFAULT_PRESET_KEY "quiet_midnight_rain"
#define STORMGLASS_DEFAULT_THEME_KEY "quiet_midnight_rain"
#define STORMGLASS_PRESET_COUNT 5U
#define STORMGLASS_THEME_COUNT 5U
#define STORMGLASS_MAX_DROPLETS 72U
#define STORMGLASS_MAX_LIGHTS 12U
#define STORMGLASS_MAX_BANDS 6U

typedef enum stormglass_scene_mode_tag {
    STORMGLASS_SCENE_RAIN = 0,
    STORMGLASS_SCENE_FOGGED = 1,
    STORMGLASS_SCENE_WINTER = 2
} stormglass_scene_mode;

typedef enum stormglass_intensity_mode_tag {
    STORMGLASS_INTENSITY_QUIET = 0,
    STORMGLASS_INTENSITY_STANDARD = 1,
    STORMGLASS_INTENSITY_HEAVY = 2
} stormglass_intensity_mode;

typedef enum stormglass_pane_mode_tag {
    STORMGLASS_PANE_CLEAR = 0,
    STORMGLASS_PANE_MISTED = 1,
    STORMGLASS_PANE_CONDENSED = 2
} stormglass_pane_mode;

typedef struct stormglass_config_tag {
    int scene_mode;
    int intensity_mode;
    int pane_mode;
} stormglass_config;

typedef struct stormglass_preset_values_tag {
    int scene_mode;
    int intensity_mode;
    int pane_mode;
} stormglass_preset_values;

typedef struct stormglass_rng_state_tag {
    unsigned long state;
} stormglass_rng_state;

typedef struct stormglass_droplet_tag {
    int active;
    int x_fixed;
    int y_fixed;
    int speed_fixed;
    int drift_fixed;
    int trail_length;
    int brightness;
    unsigned int phase;
} stormglass_droplet;

typedef struct stormglass_light_tag {
    int x;
    int y;
    int width;
    int height;
    int brightness;
    int drift;
    unsigned int phase;
} stormglass_light;

typedef struct stormglass_band_tag {
    int y;
    int thickness;
    int drift;
    int brightness;
} stormglass_band;

extern const screensave_preset_descriptor g_stormglass_presets[];
extern const screensave_theme_descriptor g_stormglass_themes[];

struct screensave_saver_session_tag {
    stormglass_config config;
    const screensave_theme_descriptor *theme;
    screensave_sizei drawable_size;
    int preview_mode;
    stormglass_rng_state rng;
    unsigned long step_accumulator;
    unsigned long event_accumulator;
    unsigned int ambient_phase;
    unsigned int event_pulse;
    int light_shift;
    stormglass_droplet droplets[STORMGLASS_MAX_DROPLETS];
    stormglass_light lights[STORMGLASS_MAX_LIGHTS];
    stormglass_band bands[STORMGLASS_MAX_BANDS];
    unsigned int droplet_count;
    unsigned int light_count;
    unsigned int band_count;
};

const screensave_saver_module *stormglass_get_module(void);
const screensave_preset_descriptor *stormglass_get_presets(unsigned int *count_out);
const screensave_theme_descriptor *stormglass_get_themes(unsigned int *count_out);
const stormglass_preset_values *stormglass_find_preset_values(const char *preset_key);
const screensave_theme_descriptor *stormglass_find_theme_descriptor(const char *theme_key);

void stormglass_config_set_defaults(
    screensave_common_config *common_config,
    void *product_config,
    unsigned int product_config_size
);
void stormglass_config_clamp(
    screensave_common_config *common_config,
    void *product_config,
    unsigned int product_config_size
);
int stormglass_config_load(
    const screensave_saver_module *module,
    screensave_common_config *common_config,
    void *product_config,
    unsigned int product_config_size,
    screensave_diag_context *diagnostics
);
int stormglass_config_save(
    const screensave_saver_module *module,
    const screensave_common_config *common_config,
    const void *product_config,
    unsigned int product_config_size,
    screensave_diag_context *diagnostics
);
INT_PTR stormglass_config_show_dialog(
    HINSTANCE instance,
    HWND owner_window,
    const screensave_saver_module *module,
    screensave_common_config *common_config,
    void *product_config,
    unsigned int product_config_size,
    screensave_diag_context *diagnostics
);

int stormglass_create_session(
    const screensave_saver_module *module,
    screensave_saver_session **session,
    const screensave_saver_environment *environment
);
void stormglass_destroy_session(screensave_saver_session *session);
void stormglass_resize_session(
    screensave_saver_session *session,
    const screensave_saver_environment *environment
);
void stormglass_step_session(
    screensave_saver_session *session,
    const screensave_saver_environment *environment
);
void stormglass_render_session(
    screensave_saver_session *session,
    const screensave_saver_environment *environment
);

void stormglass_rng_seed(stormglass_rng_state *state, unsigned long seed);
unsigned long stormglass_rng_next(stormglass_rng_state *state);
unsigned long stormglass_rng_range(stormglass_rng_state *state, unsigned long upper_bound);

const char *stormglass_scene_mode_name(int scene_mode);
const char *stormglass_intensity_mode_name(int intensity_mode);
const char *stormglass_pane_mode_name(int pane_mode);
void stormglass_apply_preset_to_config(
    const char *preset_key,
    screensave_common_config *common_config,
    stormglass_config *product_config
);

#endif /* STORMGLASS_INTERNAL_H */
