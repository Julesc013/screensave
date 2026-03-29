#ifndef PHOSPHOR_INTERNAL_H
#define PHOSPHOR_INTERNAL_H

#include <windows.h>

#include "screensave/config_api.h"
#include "screensave/diagnostics_api.h"
#include "screensave/saver_api.h"
#include "screensave/visual_buffer_api.h"

#define PHOSPHOR_PRODUCT_REGISTRY_ROOTA "Software\\Julesc013\\ScreenSave\\Products\\phosphor"
#define PHOSPHOR_DEFAULT_PRESET_KEY "amber_scope"
#define PHOSPHOR_DEFAULT_THEME_KEY "amber_scope"
#define PHOSPHOR_PRESET_COUNT 5U
#define PHOSPHOR_THEME_COUNT 5U

typedef enum phosphor_curve_mode_tag {
    PHOSPHOR_CURVE_LISSAJOUS = 0,
    PHOSPHOR_CURVE_HARMONOGRAPH = 1,
    PHOSPHOR_CURVE_DENSE = 2
} phosphor_curve_mode;

typedef enum phosphor_persistence_mode_tag {
    PHOSPHOR_PERSISTENCE_SHORT = 0,
    PHOSPHOR_PERSISTENCE_STANDARD = 1,
    PHOSPHOR_PERSISTENCE_LONG = 2
} phosphor_persistence_mode;

typedef enum phosphor_drift_mode_tag {
    PHOSPHOR_DRIFT_CALM = 0,
    PHOSPHOR_DRIFT_STANDARD = 1,
    PHOSPHOR_DRIFT_WIDE = 2
} phosphor_drift_mode;

typedef enum phosphor_mirror_mode_tag {
    PHOSPHOR_MIRROR_NONE = 0,
    PHOSPHOR_MIRROR_HORIZONTAL = 1,
    PHOSPHOR_MIRROR_QUAD = 2
} phosphor_mirror_mode;

typedef struct phosphor_config_tag {
    int curve_mode;
    int persistence_mode;
    int drift_mode;
    int mirror_mode;
} phosphor_config;

typedef struct phosphor_preset_values_tag {
    int curve_mode;
    int persistence_mode;
    int drift_mode;
    int mirror_mode;
} phosphor_preset_values;

typedef struct phosphor_rng_state_tag {
    unsigned long state;
} phosphor_rng_state;

extern const screensave_preset_descriptor g_phosphor_presets[];
extern const screensave_theme_descriptor g_phosphor_themes[];

struct screensave_saver_session_tag {
    phosphor_config config;
    const screensave_theme_descriptor *theme;
    screensave_sizei drawable_size;
    screensave_detail_level detail_level;
    int preview_mode;
    phosphor_rng_state rng;
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

const screensave_saver_module *phosphor_get_module(void);

const screensave_preset_descriptor *phosphor_get_presets(unsigned int *count_out);
const screensave_theme_descriptor *phosphor_get_themes(unsigned int *count_out);
const phosphor_preset_values *phosphor_find_preset_values(const char *preset_key);
const screensave_theme_descriptor *phosphor_find_theme_descriptor(const char *theme_key);

void phosphor_config_set_defaults(
    screensave_common_config *common_config,
    void *product_config,
    unsigned int product_config_size
);
void phosphor_config_clamp(
    screensave_common_config *common_config,
    void *product_config,
    unsigned int product_config_size
);
int phosphor_config_load(
    const screensave_saver_module *module,
    screensave_common_config *common_config,
    void *product_config,
    unsigned int product_config_size,
    screensave_diag_context *diagnostics
);
int phosphor_config_save(
    const screensave_saver_module *module,
    const screensave_common_config *common_config,
    const void *product_config,
    unsigned int product_config_size,
    screensave_diag_context *diagnostics
);
INT_PTR phosphor_config_show_dialog(
    HINSTANCE instance,
    HWND owner_window,
    const screensave_saver_module *module,
    screensave_common_config *common_config,
    void *product_config,
    unsigned int product_config_size,
    screensave_diag_context *diagnostics
);

int phosphor_create_session(
    const screensave_saver_module *module,
    screensave_saver_session **session,
    const screensave_saver_environment *environment
);
void phosphor_destroy_session(screensave_saver_session *session);
void phosphor_resize_session(
    screensave_saver_session *session,
    const screensave_saver_environment *environment
);
void phosphor_step_session(
    screensave_saver_session *session,
    const screensave_saver_environment *environment
);
void phosphor_render_session(
    screensave_saver_session *session,
    const screensave_saver_environment *environment
);

void phosphor_rng_seed(phosphor_rng_state *state, unsigned long seed);
unsigned long phosphor_rng_next(phosphor_rng_state *state);
unsigned long phosphor_rng_range(phosphor_rng_state *state, unsigned long upper_bound);

const char *phosphor_curve_mode_name(int curve_mode);
const char *phosphor_persistence_mode_name(int persistence_mode);
const char *phosphor_drift_mode_name(int drift_mode);
const char *phosphor_mirror_mode_name(int mirror_mode);

void phosphor_apply_preset_to_config(
    const char *preset_key,
    screensave_common_config *common_config,
    phosphor_config *product_config
);

#endif /* PHOSPHOR_INTERNAL_H */
