#ifndef PLASMA_INTERNAL_H
#define PLASMA_INTERNAL_H

#include <windows.h>

#include "screensave/config_api.h"
#include "screensave/diagnostics_api.h"
#include "screensave/saver_api.h"
#include "screensave/visual_buffer_api.h"
#include "plasma_classic.h"
#include "plasma_advanced.h"
#include "plasma_modern.h"
#include "plasma_premium.h"
#include "plasma_transition.h"
#include "plasma_content.h"
#include "plasma_authoring.h"
#include "plasma_selection.h"
#include "plasma_settings.h"
#include "plasma_benchlab.h"
#include "plasma_plan.h"
#include "plasma_validate.h"

#define PLASMA_PRODUCT_REGISTRY_ROOTA "Software\\Julesc013\\ScreenSave\\Products\\plasma"
#define PLASMA_DEFAULT_PRESET_KEY "plasma_lava"
#define PLASMA_DEFAULT_THEME_KEY "plasma_lava"
#define PLASMA_PRESET_COUNT 23U
#define PLASMA_THEME_COUNT 7U

typedef enum plasma_effect_mode_tag {
    PLASMA_EFFECT_PLASMA = 0,
    PLASMA_EFFECT_FIRE = 1,
    PLASMA_EFFECT_INTERFERENCE = 2,
    PLASMA_EFFECT_CHEMICAL = 3,
    PLASMA_EFFECT_LATTICE = 4,
    PLASMA_EFFECT_CAUSTIC = 5,
    PLASMA_EFFECT_AURORA = 6,
    PLASMA_EFFECT_SUBSTRATE = 7,
    PLASMA_EFFECT_ARC = 8
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
    int settings_surface;
    int effect_mode;
    int speed_mode;
    int resolution_mode;
    int smoothing_mode;
    plasma_selection_preferences selection;
    plasma_transition_preferences transition;
    plasma_benchlab_forcing benchlab;
} plasma_config;

struct plasma_preset_values_tag {
    int effect_mode;
    int speed_mode;
    int resolution_mode;
    int smoothing_mode;
    plasma_output_family output_family;
    plasma_output_mode output_mode;
    plasma_sampling_treatment sampling_treatment;
    plasma_filter_treatment filter_treatment;
    plasma_emulation_treatment emulation_treatment;
    plasma_accent_treatment accent_treatment;
    plasma_presentation_mode presentation_mode;
};

typedef struct plasma_rng_state_tag {
    unsigned long state;
} plasma_rng_state;

extern const screensave_preset_descriptor g_plasma_presets[];
extern const screensave_theme_descriptor g_plasma_themes[];

typedef struct plasma_execution_state_tag {
    screensave_sizei drawable_size;
    screensave_sizei field_size;
    screensave_renderer_kind active_renderer_kind;
    int preview_mode;
    plasma_rng_state rng;
    screensave_visual_buffer visual_buffer;
    screensave_visual_buffer advanced_treatment_buffer;
    screensave_visual_buffer modern_treatment_buffer;
    screensave_visual_buffer modern_presentation_buffer;
    screensave_visual_buffer premium_treatment_buffer;
    screensave_visual_buffer premium_presentation_buffer;
    plasma_transition_runtime transition;
    unsigned char *field_primary;
    unsigned char *field_secondary;
    unsigned char *field_history;
    unsigned long phase_millis;
    unsigned long palette_phase;
    unsigned long source_phase_a;
    unsigned long source_phase_b;
    unsigned long source_phase_c;
    unsigned long variation_elapsed_millis;
} plasma_execution_state;

struct screensave_saver_session_tag {
    plasma_plan plan;
    plasma_execution_state state;
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
int plasma_config_export_settings_entries(
    const screensave_saver_module *module,
    const screensave_common_config *common_config,
    const void *product_config,
    unsigned int product_config_size,
    screensave_settings_file_kind kind,
    screensave_settings_writer *writer,
    screensave_diag_context *diagnostics
);
int plasma_config_import_settings_entry(
    const screensave_saver_module *module,
    screensave_common_config *common_config,
    void *product_config,
    unsigned int product_config_size,
    screensave_settings_file_kind kind,
    const char *section,
    const char *key,
    const char *value,
    screensave_diag_context *diagnostics
);
void plasma_config_randomize_settings(
    const screensave_saver_module *module,
    screensave_common_config *common_config,
    void *product_config,
    unsigned int product_config_size,
    const screensave_session_seed *seed,
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
