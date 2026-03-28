#ifndef OSCILLOSCOPE_DREAMS_INTERNAL_H
#define OSCILLOSCOPE_DREAMS_INTERNAL_H

#include <windows.h>

#include "screensave/config_api.h"
#include "screensave/diagnostics_api.h"
#include "screensave/saver_api.h"
#include "screensave/visual_buffer_api.h"

#define OSCILLOSCOPE_DREAMS_PRODUCT_REGISTRY_ROOTA "Software\\Julesc013\\ScreenSave\\Products\\oscilloscope_dreams"
#define OSCILLOSCOPE_DREAMS_DEFAULT_PRESET_KEY "amber_scope"
#define OSCILLOSCOPE_DREAMS_DEFAULT_THEME_KEY "amber_scope"
#define OSCILLOSCOPE_DREAMS_PRESET_COUNT 5U
#define OSCILLOSCOPE_DREAMS_THEME_COUNT 5U

typedef enum oscilloscope_dreams_curve_mode_tag {
    OSCILLOSCOPE_DREAMS_CURVE_LISSAJOUS = 0,
    OSCILLOSCOPE_DREAMS_CURVE_HARMONOGRAPH = 1,
    OSCILLOSCOPE_DREAMS_CURVE_DENSE = 2
} oscilloscope_dreams_curve_mode;

typedef enum oscilloscope_dreams_persistence_mode_tag {
    OSCILLOSCOPE_DREAMS_PERSISTENCE_SHORT = 0,
    OSCILLOSCOPE_DREAMS_PERSISTENCE_STANDARD = 1,
    OSCILLOSCOPE_DREAMS_PERSISTENCE_LONG = 2
} oscilloscope_dreams_persistence_mode;

typedef enum oscilloscope_dreams_drift_mode_tag {
    OSCILLOSCOPE_DREAMS_DRIFT_CALM = 0,
    OSCILLOSCOPE_DREAMS_DRIFT_STANDARD = 1,
    OSCILLOSCOPE_DREAMS_DRIFT_WIDE = 2
} oscilloscope_dreams_drift_mode;

typedef enum oscilloscope_dreams_mirror_mode_tag {
    OSCILLOSCOPE_DREAMS_MIRROR_NONE = 0,
    OSCILLOSCOPE_DREAMS_MIRROR_HORIZONTAL = 1,
    OSCILLOSCOPE_DREAMS_MIRROR_QUAD = 2
} oscilloscope_dreams_mirror_mode;

typedef struct oscilloscope_dreams_config_tag {
    int curve_mode;
    int persistence_mode;
    int drift_mode;
    int mirror_mode;
} oscilloscope_dreams_config;

typedef struct oscilloscope_dreams_preset_values_tag {
    int curve_mode;
    int persistence_mode;
    int drift_mode;
    int mirror_mode;
} oscilloscope_dreams_preset_values;

typedef struct oscilloscope_dreams_rng_state_tag {
    unsigned long state;
} oscilloscope_dreams_rng_state;

extern const screensave_preset_descriptor g_oscilloscope_dreams_presets[];
extern const screensave_theme_descriptor g_oscilloscope_dreams_themes[];

struct screensave_saver_session_tag {
    oscilloscope_dreams_config config;
    const screensave_theme_descriptor *theme;
    screensave_sizei drawable_size;
    screensave_detail_level detail_level;
    int preview_mode;
    oscilloscope_dreams_rng_state rng;
    screensave_visual_buffer visual_buffer;
    unsigned long phase_a;
    unsigned long phase_b;
    unsigned long phase_c;
    unsigned long phase_d;
    unsigned long elapsed_millis;
    unsigned int ratio_a;
    unsigned int ratio_b;
    unsigned int ratio_c;
    unsigned int ratio_d;
};

const screensave_saver_module *oscilloscope_dreams_get_module(void);

const screensave_preset_descriptor *oscilloscope_dreams_get_presets(unsigned int *count_out);
const screensave_theme_descriptor *oscilloscope_dreams_get_themes(unsigned int *count_out);
const oscilloscope_dreams_preset_values *oscilloscope_dreams_find_preset_values(const char *preset_key);
const screensave_theme_descriptor *oscilloscope_dreams_find_theme_descriptor(const char *theme_key);

void oscilloscope_dreams_config_set_defaults(
    screensave_common_config *common_config,
    void *product_config,
    unsigned int product_config_size
);
void oscilloscope_dreams_config_clamp(
    screensave_common_config *common_config,
    void *product_config,
    unsigned int product_config_size
);
int oscilloscope_dreams_config_load(
    const screensave_saver_module *module,
    screensave_common_config *common_config,
    void *product_config,
    unsigned int product_config_size,
    screensave_diag_context *diagnostics
);
int oscilloscope_dreams_config_save(
    const screensave_saver_module *module,
    const screensave_common_config *common_config,
    const void *product_config,
    unsigned int product_config_size,
    screensave_diag_context *diagnostics
);
INT_PTR oscilloscope_dreams_config_show_dialog(
    HINSTANCE instance,
    HWND owner_window,
    const screensave_saver_module *module,
    screensave_common_config *common_config,
    void *product_config,
    unsigned int product_config_size,
    screensave_diag_context *diagnostics
);

int oscilloscope_dreams_create_session(
    const screensave_saver_module *module,
    screensave_saver_session **session,
    const screensave_saver_environment *environment
);
void oscilloscope_dreams_destroy_session(screensave_saver_session *session);
void oscilloscope_dreams_resize_session(
    screensave_saver_session *session,
    const screensave_saver_environment *environment
);
void oscilloscope_dreams_step_session(
    screensave_saver_session *session,
    const screensave_saver_environment *environment
);
void oscilloscope_dreams_render_session(
    screensave_saver_session *session,
    const screensave_saver_environment *environment
);

void oscilloscope_dreams_rng_seed(oscilloscope_dreams_rng_state *state, unsigned long seed);
unsigned long oscilloscope_dreams_rng_next(oscilloscope_dreams_rng_state *state);
unsigned long oscilloscope_dreams_rng_range(oscilloscope_dreams_rng_state *state, unsigned long upper_bound);

const char *oscilloscope_dreams_curve_mode_name(int curve_mode);
const char *oscilloscope_dreams_persistence_mode_name(int persistence_mode);
const char *oscilloscope_dreams_drift_mode_name(int drift_mode);
const char *oscilloscope_dreams_mirror_mode_name(int mirror_mode);

void oscilloscope_dreams_apply_preset_to_config(
    const char *preset_key,
    screensave_common_config *common_config,
    oscilloscope_dreams_config *product_config
);

#endif /* OSCILLOSCOPE_DREAMS_INTERNAL_H */
