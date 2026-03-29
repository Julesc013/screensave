#ifndef SIGNALS_INTERNAL_H
#define SIGNALS_INTERNAL_H

#include <windows.h>

#include "screensave/config_api.h"
#include "screensave/diagnostics_api.h"
#include "screensave/saver_api.h"

#define SIGNALS_PRODUCT_REGISTRY_ROOTA "Software\\Julesc013\\ScreenSave\\Products\\signals"
#define SIGNALS_DEFAULT_PRESET_KEY "green_terminal"
#define SIGNALS_DEFAULT_THEME_KEY "green_terminal"
#define SIGNALS_PRESET_COUNT 5U
#define SIGNALS_THEME_COUNT 5U
#define SIGNALS_SCOPE_COUNT 3U
#define SIGNALS_SCOPE_SAMPLE_COUNT 48U
#define SIGNALS_METER_COUNT 6U
#define SIGNALS_STATUS_COUNT 8U

typedef enum signals_layout_mode_tag {
    SIGNALS_LAYOUT_OPERATIONS = 0,
    SIGNALS_LAYOUT_SPECTRUM = 1,
    SIGNALS_LAYOUT_TELEMETRY = 2
} signals_layout_mode;

typedef enum signals_activity_mode_tag {
    SIGNALS_ACTIVITY_QUIET = 0,
    SIGNALS_ACTIVITY_STANDARD = 1,
    SIGNALS_ACTIVITY_BUSY = 2
} signals_activity_mode;

typedef enum signals_overlay_mode_tag {
    SIGNALS_OVERLAY_OFF = 0,
    SIGNALS_OVERLAY_GRID = 1,
    SIGNALS_OVERLAY_SCAN = 2
} signals_overlay_mode;

typedef struct signals_config_tag {
    int layout_mode;
    int activity_mode;
    int overlay_mode;
} signals_config;

typedef struct signals_preset_values_tag {
    int layout_mode;
    int activity_mode;
    int overlay_mode;
} signals_preset_values;

typedef struct signals_rng_state_tag {
    unsigned long state;
} signals_rng_state;

extern const screensave_preset_descriptor g_signals_presets[];
extern const screensave_theme_descriptor g_signals_themes[];

struct screensave_saver_session_tag {
    signals_config config;
    const screensave_theme_descriptor *theme;
    screensave_sizei drawable_size;
    int preview_mode;
    signals_rng_state rng;
    unsigned long sample_accumulator;
    unsigned long event_accumulator;
    unsigned long alert_millis;
    unsigned int sweep_position;
    unsigned int heartbeat_phase;
    unsigned int counters[3];
    int meter_levels[SIGNALS_METER_COUNT];
    int meter_targets[SIGNALS_METER_COUNT];
    int scope_values[SIGNALS_SCOPE_COUNT][SIGNALS_SCOPE_SAMPLE_COUNT];
    int status_flags[SIGNALS_STATUS_COUNT];
};

const screensave_saver_module *signals_get_module(void);

const screensave_preset_descriptor *signals_get_presets(unsigned int *count_out);
const screensave_theme_descriptor *signals_get_themes(unsigned int *count_out);
const signals_preset_values *signals_find_preset_values(const char *preset_key);
const screensave_theme_descriptor *signals_find_theme_descriptor(const char *theme_key);

void signals_config_set_defaults(
    screensave_common_config *common_config,
    void *product_config,
    unsigned int product_config_size
);
void signals_config_clamp(
    screensave_common_config *common_config,
    void *product_config,
    unsigned int product_config_size
);
int signals_config_load(
    const screensave_saver_module *module,
    screensave_common_config *common_config,
    void *product_config,
    unsigned int product_config_size,
    screensave_diag_context *diagnostics
);
int signals_config_save(
    const screensave_saver_module *module,
    const screensave_common_config *common_config,
    const void *product_config,
    unsigned int product_config_size,
    screensave_diag_context *diagnostics
);
INT_PTR signals_config_show_dialog(
    HINSTANCE instance,
    HWND owner_window,
    const screensave_saver_module *module,
    screensave_common_config *common_config,
    void *product_config,
    unsigned int product_config_size,
    screensave_diag_context *diagnostics
);

int signals_create_session(
    const screensave_saver_module *module,
    screensave_saver_session **session,
    const screensave_saver_environment *environment
);
void signals_destroy_session(screensave_saver_session *session);
void signals_resize_session(
    screensave_saver_session *session,
    const screensave_saver_environment *environment
);
void signals_step_session(
    screensave_saver_session *session,
    const screensave_saver_environment *environment
);
void signals_render_session(
    screensave_saver_session *session,
    const screensave_saver_environment *environment
);

void signals_rng_seed(signals_rng_state *state, unsigned long seed);
unsigned long signals_rng_next(signals_rng_state *state);
unsigned long signals_rng_range(signals_rng_state *state, unsigned long upper_bound);

const char *signals_layout_mode_name(int layout_mode);
const char *signals_activity_mode_name(int activity_mode);
const char *signals_overlay_mode_name(int overlay_mode);

void signals_apply_preset_to_config(
    const char *preset_key,
    screensave_common_config *common_config,
    signals_config *product_config
);

#endif /* SIGNALS_INTERNAL_H */
