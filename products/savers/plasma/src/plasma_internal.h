#ifndef PLASMA_INTERNAL_H
#define PLASMA_INTERNAL_H

#include <windows.h>

#include "screensave/config_api.h"
#include "screensave/diagnostics_api.h"
#include "screensave/saver_api.h"
#include "screensave/visual_buffer_api.h"

#define PLASMA_PRODUCT_REGISTRY_ROOTA "Software\\Julesc013\\ScreenSave\\Products\\plasma"
#define PLASMA_DEFAULT_PRESET_KEY "plasma_lava"
#define PLASMA_DEFAULT_THEME_KEY "plasma_lava"
#define PLASMA_PRESET_COUNT 5U
#define PLASMA_THEME_COUNT 5U

typedef enum plasma_effect_mode_tag {
    PLASMA_EFFECT_PLASMA = 0,
    PLASMA_EFFECT_FIRE = 1,
    PLASMA_EFFECT_INTERFERENCE = 2
} plasma_effect_mode;

typedef enum plasma_speed_mode_tag {
    PLASMA_SPEED_GENTLE = 0,
    PLASMA_SPEED_STANDARD = 1,
    PLASMA_SPEED_LIVELY = 2
} plasma_speed_mode;

typedef enum plasma_resolution_mode_tag {
    PLASMA_RESOLUTION_COARSE = 0,
    PLASMA_RESOLUTION_STANDARD = 1,
    PLASMA_RESOLUTION_FINE = 2
} plasma_resolution_mode;

typedef enum plasma_smoothing_mode_tag {
    PLASMA_SMOOTHING_OFF = 0,
    PLASMA_SMOOTHING_SOFT = 1,
    PLASMA_SMOOTHING_GLOW = 2
} plasma_smoothing_mode;

typedef struct plasma_config_tag {
    int effect_mode;
    int speed_mode;
    int resolution_mode;
    int smoothing_mode;
} plasma_config;

typedef struct plasma_preset_values_tag {
    int effect_mode;
    int speed_mode;
    int resolution_mode;
    int smoothing_mode;
} plasma_preset_values;

typedef struct plasma_rng_state_tag {
    unsigned long state;
} plasma_rng_state;

extern const screensave_preset_descriptor g_plasma_presets[];
extern const screensave_theme_descriptor g_plasma_themes[];

struct screensave_saver_session_tag {
    plasma_config config;
    const screensave_theme_descriptor *theme;
    screensave_sizei drawable_size;
    screensave_sizei field_size;
    screensave_detail_level detail_level;
    int preview_mode;
    plasma_rng_state rng;
    screensave_visual_buffer visual_buffer;
    unsigned char *field_primary;
    unsigned char *field_secondary;
    unsigned long phase_millis;
    unsigned long palette_phase;
    unsigned long source_phase_a;
    unsigned long source_phase_b;
    unsigned long source_phase_c;
};

const screensave_saver_module *plasma_get_module(void);

const screensave_preset_descriptor *plasma_get_presets(unsigned int *count_out);
const screensave_theme_descriptor *plasma_get_themes(unsigned int *count_out);
const plasma_preset_values *plasma_find_preset_values(const char *preset_key);
const screensave_theme_descriptor *plasma_find_theme_descriptor(const char *theme_key);

void plasma_config_set_defaults(
    screensave_common_config *common_config,
    void *product_config,
    unsigned int product_config_size
);
void plasma_config_clamp(
    screensave_common_config *common_config,
    void *product_config,
    unsigned int product_config_size
);
int plasma_config_load(
    const screensave_saver_module *module,
    screensave_common_config *common_config,
    void *product_config,
    unsigned int product_config_size,
    screensave_diag_context *diagnostics
);
int plasma_config_save(
    const screensave_saver_module *module,
    const screensave_common_config *common_config,
    const void *product_config,
    unsigned int product_config_size,
    screensave_diag_context *diagnostics
);
INT_PTR plasma_config_show_dialog(
    HINSTANCE instance,
    HWND owner_window,
    const screensave_saver_module *module,
    screensave_common_config *common_config,
    void *product_config,
    unsigned int product_config_size,
    screensave_diag_context *diagnostics
);

int plasma_create_session(
    const screensave_saver_module *module,
    screensave_saver_session **session,
    const screensave_saver_environment *environment
);
void plasma_destroy_session(screensave_saver_session *session);
void plasma_resize_session(
    screensave_saver_session *session,
    const screensave_saver_environment *environment
);
void plasma_step_session(
    screensave_saver_session *session,
    const screensave_saver_environment *environment
);
void plasma_render_session(
    screensave_saver_session *session,
    const screensave_saver_environment *environment
);

void plasma_rng_seed(plasma_rng_state *state, unsigned long seed);
unsigned long plasma_rng_next(plasma_rng_state *state);
unsigned long plasma_rng_range(plasma_rng_state *state, unsigned long upper_bound);

const char *plasma_effect_mode_name(int effect_mode);
const char *plasma_speed_mode_name(int speed_mode);
const char *plasma_resolution_mode_name(int resolution_mode);
const char *plasma_smoothing_mode_name(int smoothing_mode);

void plasma_apply_preset_to_config(
    const char *preset_key,
    screensave_common_config *common_config,
    plasma_config *product_config
);

#endif /* PLASMA_INTERNAL_H */
