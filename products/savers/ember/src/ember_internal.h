#ifndef EMBER_INTERNAL_H
#define EMBER_INTERNAL_H

#include <windows.h>

#include "screensave/config_api.h"
#include "screensave/diagnostics_api.h"
#include "screensave/saver_api.h"
#include "screensave/visual_buffer_api.h"

#define EMBER_PRODUCT_REGISTRY_ROOTA "Software\\Julesc013\\ScreenSave\\Products\\ember"
#define EMBER_DEFAULT_PRESET_KEY "ember_lava"
#define EMBER_DEFAULT_THEME_KEY "ember_lava"
#define EMBER_PRESET_COUNT 5U
#define EMBER_THEME_COUNT 5U

typedef enum ember_effect_mode_tag {
    EMBER_EFFECT_PLASMA = 0,
    EMBER_EFFECT_FIRE = 1,
    EMBER_EFFECT_INTERFERENCE = 2
} ember_effect_mode;

typedef enum ember_speed_mode_tag {
    EMBER_SPEED_GENTLE = 0,
    EMBER_SPEED_STANDARD = 1,
    EMBER_SPEED_LIVELY = 2
} ember_speed_mode;

typedef enum ember_resolution_mode_tag {
    EMBER_RESOLUTION_COARSE = 0,
    EMBER_RESOLUTION_STANDARD = 1,
    EMBER_RESOLUTION_FINE = 2
} ember_resolution_mode;

typedef enum ember_smoothing_mode_tag {
    EMBER_SMOOTHING_OFF = 0,
    EMBER_SMOOTHING_SOFT = 1,
    EMBER_SMOOTHING_GLOW = 2
} ember_smoothing_mode;

typedef struct ember_config_tag {
    int effect_mode;
    int speed_mode;
    int resolution_mode;
    int smoothing_mode;
} ember_config;

typedef struct ember_preset_values_tag {
    int effect_mode;
    int speed_mode;
    int resolution_mode;
    int smoothing_mode;
} ember_preset_values;

typedef struct ember_rng_state_tag {
    unsigned long state;
} ember_rng_state;

extern const screensave_preset_descriptor g_ember_presets[];
extern const screensave_theme_descriptor g_ember_themes[];

struct screensave_saver_session_tag {
    ember_config config;
    const screensave_theme_descriptor *theme;
    screensave_sizei drawable_size;
    screensave_sizei field_size;
    screensave_detail_level detail_level;
    int preview_mode;
    ember_rng_state rng;
    screensave_visual_buffer visual_buffer;
    unsigned char *field_primary;
    unsigned char *field_secondary;
    unsigned long phase_millis;
    unsigned long palette_phase;
    unsigned long source_phase_a;
    unsigned long source_phase_b;
    unsigned long source_phase_c;
};

const screensave_saver_module *ember_get_module(void);

const screensave_preset_descriptor *ember_get_presets(unsigned int *count_out);
const screensave_theme_descriptor *ember_get_themes(unsigned int *count_out);
const ember_preset_values *ember_find_preset_values(const char *preset_key);
const screensave_theme_descriptor *ember_find_theme_descriptor(const char *theme_key);

void ember_config_set_defaults(
    screensave_common_config *common_config,
    void *product_config,
    unsigned int product_config_size
);
void ember_config_clamp(
    screensave_common_config *common_config,
    void *product_config,
    unsigned int product_config_size
);
int ember_config_load(
    const screensave_saver_module *module,
    screensave_common_config *common_config,
    void *product_config,
    unsigned int product_config_size,
    screensave_diag_context *diagnostics
);
int ember_config_save(
    const screensave_saver_module *module,
    const screensave_common_config *common_config,
    const void *product_config,
    unsigned int product_config_size,
    screensave_diag_context *diagnostics
);
INT_PTR ember_config_show_dialog(
    HINSTANCE instance,
    HWND owner_window,
    const screensave_saver_module *module,
    screensave_common_config *common_config,
    void *product_config,
    unsigned int product_config_size,
    screensave_diag_context *diagnostics
);

int ember_create_session(
    const screensave_saver_module *module,
    screensave_saver_session **session,
    const screensave_saver_environment *environment
);
void ember_destroy_session(screensave_saver_session *session);
void ember_resize_session(
    screensave_saver_session *session,
    const screensave_saver_environment *environment
);
void ember_step_session(
    screensave_saver_session *session,
    const screensave_saver_environment *environment
);
void ember_render_session(
    screensave_saver_session *session,
    const screensave_saver_environment *environment
);

void ember_rng_seed(ember_rng_state *state, unsigned long seed);
unsigned long ember_rng_next(ember_rng_state *state);
unsigned long ember_rng_range(ember_rng_state *state, unsigned long upper_bound);

const char *ember_effect_mode_name(int effect_mode);
const char *ember_speed_mode_name(int speed_mode);
const char *ember_resolution_mode_name(int resolution_mode);
const char *ember_smoothing_mode_name(int smoothing_mode);

void ember_apply_preset_to_config(
    const char *preset_key,
    screensave_common_config *common_config,
    ember_config *product_config
);

#endif /* EMBER_INTERNAL_H */
