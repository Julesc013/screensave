#ifndef ATLAS_INTERNAL_H
#define ATLAS_INTERNAL_H

#include <windows.h>

#include "screensave/config_api.h"
#include "screensave/diagnostics_api.h"
#include "screensave/saver_api.h"
#include "screensave/visual_buffer_api.h"

#define ATLAS_PRODUCT_REGISTRY_ROOTA "Software\\Julesc013\\ScreenSave\\Products\\atlas"
#define ATLAS_DEFAULT_PRESET_KEY "atlas_monochrome"
#define ATLAS_DEFAULT_THEME_KEY "atlas_monochrome"
#define ATLAS_PRESET_COUNT 5U
#define ATLAS_THEME_COUNT 5U
#define ATLAS_ROUTE_POINT_COUNT 5U

typedef enum atlas_mode_tag {
    ATLAS_MODE_VOYAGE = 0,
    ATLAS_MODE_ATLAS = 1,
    ATLAS_MODE_JULIA = 2
} atlas_mode;

typedef enum atlas_speed_mode_tag {
    ATLAS_SPEED_STILL = 0,
    ATLAS_SPEED_STANDARD = 1,
    ATLAS_SPEED_BRISK = 2
} atlas_speed_mode;

typedef enum atlas_refinement_mode_tag {
    ATLAS_REFINEMENT_DRAFT = 0,
    ATLAS_REFINEMENT_STANDARD = 1,
    ATLAS_REFINEMENT_FINE = 2
} atlas_refinement_mode;

typedef struct atlas_config_tag {
    int mode;
    int speed_mode;
    int refinement_mode;
} atlas_config;

typedef struct atlas_preset_values_tag {
    int mode;
    int speed_mode;
    int refinement_mode;
} atlas_preset_values;

typedef struct atlas_rng_state_tag {
    unsigned long state;
} atlas_rng_state;

typedef struct atlas_waypoint_tag {
    double center_x;
    double center_y;
    double scale;
    double julia_real;
    double julia_imag;
} atlas_waypoint;

extern const screensave_preset_descriptor g_atlas_presets[];
extern const screensave_theme_descriptor g_atlas_themes[];

struct screensave_saver_session_tag {
    atlas_config config;
    const screensave_theme_descriptor *theme;
    screensave_sizei drawable_size;
    screensave_sizei field_size;
    screensave_detail_level detail_level;
    int preview_mode;
    int max_iterations;
    int refinement_row;
    int refresh_pending;
    atlas_rng_state rng;
    screensave_visual_buffer visual_buffer;
    unsigned short *escape_values;
    unsigned long step_accumulator;
    unsigned long phase_counter;
    unsigned int route_index;
    unsigned int route_phase;
    unsigned int palette_phase;
    atlas_waypoint current_view;
};

const screensave_saver_module *atlas_get_module(void);
const screensave_preset_descriptor *atlas_get_presets(unsigned int *count_out);
const screensave_theme_descriptor *atlas_get_themes(unsigned int *count_out);
const atlas_preset_values *atlas_find_preset_values(const char *preset_key);
const screensave_theme_descriptor *atlas_find_theme_descriptor(const char *theme_key);

void atlas_config_set_defaults(
    screensave_common_config *common_config,
    void *product_config,
    unsigned int product_config_size
);
void atlas_config_clamp(
    screensave_common_config *common_config,
    void *product_config,
    unsigned int product_config_size
);
int atlas_config_load(
    const screensave_saver_module *module,
    screensave_common_config *common_config,
    void *product_config,
    unsigned int product_config_size,
    screensave_diag_context *diagnostics
);
int atlas_config_save(
    const screensave_saver_module *module,
    const screensave_common_config *common_config,
    const void *product_config,
    unsigned int product_config_size,
    screensave_diag_context *diagnostics
);
INT_PTR atlas_config_show_dialog(
    HINSTANCE instance,
    HWND owner_window,
    const screensave_saver_module *module,
    screensave_common_config *common_config,
    void *product_config,
    unsigned int product_config_size,
    screensave_diag_context *diagnostics
);
int atlas_config_export_settings_entries(
    const screensave_saver_module *module,
    const screensave_common_config *common_config,
    const void *product_config,
    unsigned int product_config_size,
    screensave_settings_file_kind kind,
    screensave_settings_writer *writer,
    screensave_diag_context *diagnostics
);
int atlas_config_import_settings_entry(
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
void atlas_config_randomize_settings(
    const screensave_saver_module *module,
    screensave_common_config *common_config,
    void *product_config,
    unsigned int product_config_size,
    const screensave_session_seed *seed,
    screensave_diag_context *diagnostics
);

int atlas_create_session(
    const screensave_saver_module *module,
    screensave_saver_session **session,
    const screensave_saver_environment *environment
);
void atlas_destroy_session(screensave_saver_session *session);
void atlas_resize_session(
    screensave_saver_session *session,
    const screensave_saver_environment *environment
);
void atlas_step_session(
    screensave_saver_session *session,
    const screensave_saver_environment *environment
);
void atlas_render_session(
    screensave_saver_session *session,
    const screensave_saver_environment *environment
);

void atlas_rng_seed(atlas_rng_state *state, unsigned long seed);
unsigned long atlas_rng_next(atlas_rng_state *state);
unsigned long atlas_rng_range(atlas_rng_state *state, unsigned long upper_bound);

const char *atlas_mode_name(int mode);
const char *atlas_speed_mode_name(int speed_mode);
const char *atlas_refinement_mode_name(int refinement_mode);
void atlas_apply_preset_to_config(
    const char *preset_key,
    screensave_common_config *common_config,
    atlas_config *product_config
);

#endif /* ATLAS_INTERNAL_H */
