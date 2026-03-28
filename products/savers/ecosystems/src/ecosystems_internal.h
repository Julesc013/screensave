#ifndef ECOSYSTEMS_INTERNAL_H
#define ECOSYSTEMS_INTERNAL_H

#include <windows.h>

#include "screensave/config_api.h"
#include "screensave/diagnostics_api.h"
#include "screensave/saver_api.h"

#define ECOSYSTEMS_PRODUCT_REGISTRY_ROOTA "Software\\Julesc013\\ScreenSave\\Products\\ecosystems"
#define ECOSYSTEMS_DEFAULT_PRESET_KEY "aquarium_current"
#define ECOSYSTEMS_DEFAULT_THEME_KEY "aquarium_current"
#define ECOSYSTEMS_PRESET_COUNT 5U
#define ECOSYSTEMS_THEME_COUNT 5U
#define ECOSYSTEMS_MAX_AGENTS 64U

typedef enum ecosystems_habitat_mode_tag {
    ECOSYSTEMS_HABITAT_AQUARIUM = 0,
    ECOSYSTEMS_HABITAT_AVIARY = 1,
    ECOSYSTEMS_HABITAT_FIREFLIES = 2
} ecosystems_habitat_mode;

typedef enum ecosystems_density_mode_tag {
    ECOSYSTEMS_DENSITY_SPARSE = 0,
    ECOSYSTEMS_DENSITY_STANDARD = 1,
    ECOSYSTEMS_DENSITY_LUSH = 2
} ecosystems_density_mode;

typedef enum ecosystems_activity_mode_tag {
    ECOSYSTEMS_ACTIVITY_CALM = 0,
    ECOSYSTEMS_ACTIVITY_STANDARD = 1,
    ECOSYSTEMS_ACTIVITY_LIVELY = 2
} ecosystems_activity_mode;

typedef struct ecosystems_config_tag {
    int habitat_mode;
    int density_mode;
    int activity_mode;
} ecosystems_config;

typedef struct ecosystems_preset_values_tag {
    int habitat_mode;
    int density_mode;
    int activity_mode;
} ecosystems_preset_values;

typedef struct ecosystems_rng_state_tag {
    unsigned long state;
} ecosystems_rng_state;

typedef struct ecosystems_agent_tag {
    int active;
    int x_fixed;
    int y_fixed;
    int vx_fixed;
    int vy_fixed;
    int brightness;
    unsigned int phase;
    unsigned int group;
} ecosystems_agent;

extern const screensave_preset_descriptor g_ecosystems_presets[];
extern const screensave_theme_descriptor g_ecosystems_themes[];

struct screensave_saver_session_tag {
    ecosystems_config config;
    const screensave_theme_descriptor *theme;
    screensave_sizei drawable_size;
    int preview_mode;
    ecosystems_rng_state rng;
    unsigned long step_accumulator;
    unsigned long event_accumulator;
    unsigned int ambient_phase;
    int regroup_x;
    int regroup_y;
    ecosystems_agent agents[ECOSYSTEMS_MAX_AGENTS];
    unsigned int agent_count;
};

const screensave_saver_module *ecosystems_get_module(void);

const screensave_preset_descriptor *ecosystems_get_presets(unsigned int *count_out);
const screensave_theme_descriptor *ecosystems_get_themes(unsigned int *count_out);
const ecosystems_preset_values *ecosystems_find_preset_values(const char *preset_key);
const screensave_theme_descriptor *ecosystems_find_theme_descriptor(const char *theme_key);

void ecosystems_config_set_defaults(
    screensave_common_config *common_config,
    void *product_config,
    unsigned int product_config_size
);
void ecosystems_config_clamp(
    screensave_common_config *common_config,
    void *product_config,
    unsigned int product_config_size
);
int ecosystems_config_load(
    const screensave_saver_module *module,
    screensave_common_config *common_config,
    void *product_config,
    unsigned int product_config_size,
    screensave_diag_context *diagnostics
);
int ecosystems_config_save(
    const screensave_saver_module *module,
    const screensave_common_config *common_config,
    const void *product_config,
    unsigned int product_config_size,
    screensave_diag_context *diagnostics
);
INT_PTR ecosystems_config_show_dialog(
    HINSTANCE instance,
    HWND owner_window,
    const screensave_saver_module *module,
    screensave_common_config *common_config,
    void *product_config,
    unsigned int product_config_size,
    screensave_diag_context *diagnostics
);

int ecosystems_create_session(
    const screensave_saver_module *module,
    screensave_saver_session **session,
    const screensave_saver_environment *environment
);
void ecosystems_destroy_session(screensave_saver_session *session);
void ecosystems_resize_session(
    screensave_saver_session *session,
    const screensave_saver_environment *environment
);
void ecosystems_step_session(
    screensave_saver_session *session,
    const screensave_saver_environment *environment
);
void ecosystems_render_session(
    screensave_saver_session *session,
    const screensave_saver_environment *environment
);

void ecosystems_rng_seed(ecosystems_rng_state *state, unsigned long seed);
unsigned long ecosystems_rng_next(ecosystems_rng_state *state);
unsigned long ecosystems_rng_range(ecosystems_rng_state *state, unsigned long upper_bound);

const char *ecosystems_habitat_mode_name(int habitat_mode);
const char *ecosystems_density_mode_name(int density_mode);
const char *ecosystems_activity_mode_name(int activity_mode);

void ecosystems_apply_preset_to_config(
    const char *preset_key,
    screensave_common_config *common_config,
    ecosystems_config *product_config
);

#endif /* ECOSYSTEMS_INTERNAL_H */
