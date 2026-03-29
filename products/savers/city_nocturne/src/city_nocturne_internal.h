#ifndef CITY_NOCTURNE_INTERNAL_H
#define CITY_NOCTURNE_INTERNAL_H

#include <windows.h>

#include "screensave/config_api.h"
#include "screensave/diagnostics_api.h"
#include "screensave/saver_api.h"

#define CITY_NOCTURNE_PRODUCT_REGISTRY_ROOTA "Software\\Julesc013\\ScreenSave\\Products\\city_nocturne"
#define CITY_NOCTURNE_DEFAULT_PRESET_KEY "quiet_city_sleep"
#define CITY_NOCTURNE_DEFAULT_THEME_KEY "quiet_city_sleep"
#define CITY_NOCTURNE_PRESET_COUNT 5U
#define CITY_NOCTURNE_THEME_COUNT 5U
#define CITY_NOCTURNE_MAX_STRUCTURES 18U
#define CITY_NOCTURNE_MAX_LIGHTS 24U
#define CITY_NOCTURNE_MAX_FOG_BANDS 5U

typedef enum city_nocturne_scene_mode_tag {
    CITY_NOCTURNE_SCENE_SKYLINE = 0,
    CITY_NOCTURNE_SCENE_HARBOR = 1,
    CITY_NOCTURNE_SCENE_WINDOW_RIVER = 2
} city_nocturne_scene_mode;

typedef enum city_nocturne_speed_mode_tag {
    CITY_NOCTURNE_SPEED_STILL = 0,
    CITY_NOCTURNE_SPEED_STANDARD = 1,
    CITY_NOCTURNE_SPEED_BRISK = 2
} city_nocturne_speed_mode;

typedef enum city_nocturne_density_mode_tag {
    CITY_NOCTURNE_DENSITY_SPARSE = 0,
    CITY_NOCTURNE_DENSITY_STANDARD = 1,
    CITY_NOCTURNE_DENSITY_DENSE = 2
} city_nocturne_density_mode;

typedef struct city_nocturne_config_tag {
    int scene_mode;
    int speed_mode;
    int density_mode;
} city_nocturne_config;

typedef struct city_nocturne_preset_values_tag {
    int scene_mode;
    int speed_mode;
    int density_mode;
} city_nocturne_preset_values;

typedef struct city_nocturne_rng_state_tag {
    unsigned long state;
} city_nocturne_rng_state;

typedef struct city_nocturne_structure_tag {
    int world_x;
    int width;
    int height;
    int depth;
    int roof_style;
    int window_rows;
    int window_columns;
    int light_bias;
    int drift;
    unsigned int seed_offset;
} city_nocturne_structure;

typedef struct city_nocturne_light_tag {
    int world_x;
    int world_y;
    int width;
    int height;
    int brightness;
    int depth;
    unsigned int phase;
} city_nocturne_light;

typedef struct city_nocturne_fog_band_tag {
    int y;
    int height;
    int drift;
    int opacity;
} city_nocturne_fog_band;

extern const screensave_preset_descriptor g_city_nocturne_presets[];
extern const screensave_theme_descriptor g_city_nocturne_themes[];

struct screensave_saver_session_tag {
    city_nocturne_config config;
    const screensave_theme_descriptor *theme;
    screensave_sizei drawable_size;
    int preview_mode;
    city_nocturne_rng_state rng;
    unsigned long step_accumulator;
    unsigned long event_accumulator;
    unsigned int camera_phase;
    unsigned int light_phase;
    unsigned int pulse_phase;
    int route_offset;
    city_nocturne_structure structures[CITY_NOCTURNE_MAX_STRUCTURES];
    city_nocturne_light lights[CITY_NOCTURNE_MAX_LIGHTS];
    city_nocturne_fog_band fog_bands[CITY_NOCTURNE_MAX_FOG_BANDS];
    unsigned int structure_count;
    unsigned int light_count;
    unsigned int fog_count;
};

const screensave_saver_module *city_nocturne_get_module(void);
const screensave_preset_descriptor *city_nocturne_get_presets(unsigned int *count_out);
const screensave_theme_descriptor *city_nocturne_get_themes(unsigned int *count_out);
const city_nocturne_preset_values *city_nocturne_find_preset_values(const char *preset_key);
const screensave_theme_descriptor *city_nocturne_find_theme_descriptor(const char *theme_key);

void city_nocturne_config_set_defaults(
    screensave_common_config *common_config,
    void *product_config,
    unsigned int product_config_size
);
void city_nocturne_config_clamp(
    screensave_common_config *common_config,
    void *product_config,
    unsigned int product_config_size
);
int city_nocturne_config_load(
    const screensave_saver_module *module,
    screensave_common_config *common_config,
    void *product_config,
    unsigned int product_config_size,
    screensave_diag_context *diagnostics
);
int city_nocturne_config_save(
    const screensave_saver_module *module,
    const screensave_common_config *common_config,
    const void *product_config,
    unsigned int product_config_size,
    screensave_diag_context *diagnostics
);
INT_PTR city_nocturne_config_show_dialog(
    HINSTANCE instance,
    HWND owner_window,
    const screensave_saver_module *module,
    screensave_common_config *common_config,
    void *product_config,
    unsigned int product_config_size,
    screensave_diag_context *diagnostics
);

int city_nocturne_create_session(
    const screensave_saver_module *module,
    screensave_saver_session **session,
    const screensave_saver_environment *environment
);
void city_nocturne_destroy_session(screensave_saver_session *session);
void city_nocturne_resize_session(
    screensave_saver_session *session,
    const screensave_saver_environment *environment
);
void city_nocturne_step_session(
    screensave_saver_session *session,
    const screensave_saver_environment *environment
);
void city_nocturne_render_session(
    screensave_saver_session *session,
    const screensave_saver_environment *environment
);

void city_nocturne_rng_seed(city_nocturne_rng_state *state, unsigned long seed);
unsigned long city_nocturne_rng_next(city_nocturne_rng_state *state);
unsigned long city_nocturne_rng_range(city_nocturne_rng_state *state, unsigned long upper_bound);

const char *city_nocturne_scene_mode_name(int scene_mode);
const char *city_nocturne_speed_mode_name(int speed_mode);
const char *city_nocturne_density_mode_name(int density_mode);
void city_nocturne_apply_preset_to_config(
    const char *preset_key,
    screensave_common_config *common_config,
    city_nocturne_config *product_config
);

#endif /* CITY_NOCTURNE_INTERNAL_H */
