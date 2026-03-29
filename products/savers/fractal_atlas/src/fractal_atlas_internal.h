#ifndef FRACTAL_ATLAS_INTERNAL_H
#define FRACTAL_ATLAS_INTERNAL_H

#include <windows.h>

#include "screensave/config_api.h"
#include "screensave/diagnostics_api.h"
#include "screensave/saver_api.h"
#include "screensave/visual_buffer_api.h"

#define FRACTAL_ATLAS_PRODUCT_REGISTRY_ROOTA "Software\\Julesc013\\ScreenSave\\Products\\fractal_atlas"
#define FRACTAL_ATLAS_DEFAULT_PRESET_KEY "atlas_monochrome"
#define FRACTAL_ATLAS_DEFAULT_THEME_KEY "atlas_monochrome"
#define FRACTAL_ATLAS_PRESET_COUNT 5U
#define FRACTAL_ATLAS_THEME_COUNT 5U
#define FRACTAL_ATLAS_ROUTE_POINT_COUNT 5U

typedef enum fractal_atlas_mode_tag {
    FRACTAL_ATLAS_MODE_VOYAGE = 0,
    FRACTAL_ATLAS_MODE_ATLAS = 1,
    FRACTAL_ATLAS_MODE_JULIA = 2
} fractal_atlas_mode;

typedef enum fractal_atlas_speed_mode_tag {
    FRACTAL_ATLAS_SPEED_STILL = 0,
    FRACTAL_ATLAS_SPEED_STANDARD = 1,
    FRACTAL_ATLAS_SPEED_BRISK = 2
} fractal_atlas_speed_mode;

typedef enum fractal_atlas_refinement_mode_tag {
    FRACTAL_ATLAS_REFINEMENT_DRAFT = 0,
    FRACTAL_ATLAS_REFINEMENT_STANDARD = 1,
    FRACTAL_ATLAS_REFINEMENT_FINE = 2
} fractal_atlas_refinement_mode;

typedef struct fractal_atlas_config_tag {
    int mode;
    int speed_mode;
    int refinement_mode;
} fractal_atlas_config;

typedef struct fractal_atlas_preset_values_tag {
    int mode;
    int speed_mode;
    int refinement_mode;
} fractal_atlas_preset_values;

typedef struct fractal_atlas_rng_state_tag {
    unsigned long state;
} fractal_atlas_rng_state;

typedef struct fractal_atlas_waypoint_tag {
    double center_x;
    double center_y;
    double scale;
    double julia_real;
    double julia_imag;
} fractal_atlas_waypoint;

extern const screensave_preset_descriptor g_fractal_atlas_presets[];
extern const screensave_theme_descriptor g_fractal_atlas_themes[];

struct screensave_saver_session_tag {
    fractal_atlas_config config;
    const screensave_theme_descriptor *theme;
    screensave_sizei drawable_size;
    screensave_sizei field_size;
    screensave_detail_level detail_level;
    int preview_mode;
    int max_iterations;
    int refinement_row;
    int refresh_pending;
    fractal_atlas_rng_state rng;
    screensave_visual_buffer visual_buffer;
    unsigned short *escape_values;
    unsigned long step_accumulator;
    unsigned long phase_counter;
    unsigned int route_index;
    unsigned int route_phase;
    unsigned int palette_phase;
    fractal_atlas_waypoint current_view;
};

const screensave_saver_module *fractal_atlas_get_module(void);
const screensave_preset_descriptor *fractal_atlas_get_presets(unsigned int *count_out);
const screensave_theme_descriptor *fractal_atlas_get_themes(unsigned int *count_out);
const fractal_atlas_preset_values *fractal_atlas_find_preset_values(const char *preset_key);
const screensave_theme_descriptor *fractal_atlas_find_theme_descriptor(const char *theme_key);

void fractal_atlas_config_set_defaults(
    screensave_common_config *common_config,
    void *product_config,
    unsigned int product_config_size
);
void fractal_atlas_config_clamp(
    screensave_common_config *common_config,
    void *product_config,
    unsigned int product_config_size
);
int fractal_atlas_config_load(
    const screensave_saver_module *module,
    screensave_common_config *common_config,
    void *product_config,
    unsigned int product_config_size,
    screensave_diag_context *diagnostics
);
int fractal_atlas_config_save(
    const screensave_saver_module *module,
    const screensave_common_config *common_config,
    const void *product_config,
    unsigned int product_config_size,
    screensave_diag_context *diagnostics
);
INT_PTR fractal_atlas_config_show_dialog(
    HINSTANCE instance,
    HWND owner_window,
    const screensave_saver_module *module,
    screensave_common_config *common_config,
    void *product_config,
    unsigned int product_config_size,
    screensave_diag_context *diagnostics
);

int fractal_atlas_create_session(
    const screensave_saver_module *module,
    screensave_saver_session **session,
    const screensave_saver_environment *environment
);
void fractal_atlas_destroy_session(screensave_saver_session *session);
void fractal_atlas_resize_session(
    screensave_saver_session *session,
    const screensave_saver_environment *environment
);
void fractal_atlas_step_session(
    screensave_saver_session *session,
    const screensave_saver_environment *environment
);
void fractal_atlas_render_session(
    screensave_saver_session *session,
    const screensave_saver_environment *environment
);

void fractal_atlas_rng_seed(fractal_atlas_rng_state *state, unsigned long seed);
unsigned long fractal_atlas_rng_next(fractal_atlas_rng_state *state);
unsigned long fractal_atlas_rng_range(fractal_atlas_rng_state *state, unsigned long upper_bound);

const char *fractal_atlas_mode_name(int mode);
const char *fractal_atlas_speed_mode_name(int speed_mode);
const char *fractal_atlas_refinement_mode_name(int refinement_mode);
void fractal_atlas_apply_preset_to_config(
    const char *preset_key,
    screensave_common_config *common_config,
    fractal_atlas_config *product_config
);

#endif /* FRACTAL_ATLAS_INTERNAL_H */
