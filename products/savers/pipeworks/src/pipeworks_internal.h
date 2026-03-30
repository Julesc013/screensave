#ifndef PIPEWORKS_INTERNAL_H
#define PIPEWORKS_INTERNAL_H

#include <windows.h>

#include "screensave/config_api.h"
#include "screensave/diagnostics_api.h"
#include "screensave/grid_buffer_api.h"
#include "screensave/saver_api.h"

#define PIPEWORKS_PRODUCT_REGISTRY_ROOTA "Software\\Julesc013\\ScreenSave\\Products\\pipeworks"
#define PIPEWORKS_DEFAULT_PRESET_KEY "industrial_pipes"
#define PIPEWORKS_DEFAULT_THEME_KEY "industrial_pipes"
#define PIPEWORKS_PRESET_COUNT 7U
#define PIPEWORKS_THEME_COUNT 7U
#define PIPEWORKS_MAX_HEADS 40U
#define PIPEWORKS_MAX_PULSES 20U

#define PIPEWORKS_CELL_NORTH 0x01U
#define PIPEWORKS_CELL_EAST 0x02U
#define PIPEWORKS_CELL_SOUTH 0x04U
#define PIPEWORKS_CELL_WEST 0x08U

typedef enum pipeworks_density_mode_tag {
    PIPEWORKS_DENSITY_TIGHT = 0,
    PIPEWORKS_DENSITY_STANDARD = 1,
    PIPEWORKS_DENSITY_OPEN = 2
} pipeworks_density_mode;

typedef enum pipeworks_speed_mode_tag {
    PIPEWORKS_SPEED_PATIENT = 0,
    PIPEWORKS_SPEED_STANDARD = 1,
    PIPEWORKS_SPEED_BRISK = 2
} pipeworks_speed_mode;

typedef enum pipeworks_branch_mode_tag {
    PIPEWORKS_BRANCH_ORDERLY = 0,
    PIPEWORKS_BRANCH_BALANCED = 1,
    PIPEWORKS_BRANCH_WILD = 2
} pipeworks_branch_mode;

typedef enum pipeworks_rebuild_mode_tag {
    PIPEWORKS_REBUILD_PATIENT = 0,
    PIPEWORKS_REBUILD_CYCLE = 1,
    PIPEWORKS_REBUILD_FREQUENT = 2
} pipeworks_rebuild_mode;

typedef enum pipeworks_stage_tag {
    PIPEWORKS_STAGE_GROW = 0,
    PIPEWORKS_STAGE_FLOW = 1,
    PIPEWORKS_STAGE_CLEAR = 2
} pipeworks_stage;

typedef struct pipeworks_config_tag {
    int density_mode;
    int speed_mode;
    int branch_mode;
    int rebuild_mode;
} pipeworks_config;

typedef struct pipeworks_preset_values_tag {
    int density_mode;
    int speed_mode;
    int branch_mode;
    int rebuild_mode;
} pipeworks_preset_values;

typedef struct pipeworks_rng_state_tag {
    unsigned long state;
} pipeworks_rng_state;

typedef struct pipeworks_head_tag {
    int x;
    int y;
    int direction;
    int active;
    unsigned int age;
} pipeworks_head;

typedef struct pipeworks_pulse_tag {
    int x;
    int y;
    int direction;
    int active;
    unsigned int life;
} pipeworks_pulse;

extern const screensave_preset_descriptor g_pipeworks_presets[];
extern const screensave_theme_descriptor g_pipeworks_themes[];

struct screensave_saver_session_tag {
    pipeworks_config config;
    const screensave_theme_descriptor *theme;
    screensave_sizei drawable_size;
    screensave_sizei grid_size;
    int cell_size;
    screensave_detail_level detail_level;
    int preview_mode;
    pipeworks_rng_state rng;
    screensave_byte_grid cells;
    screensave_byte_grid glow;
    unsigned int occupied_cells;
    unsigned long stage_millis;
    unsigned long step_accumulator;
    unsigned long pulse_spawn_accumulator;
    unsigned long rebuild_count;
    unsigned int clear_scan_start;
    unsigned int clear_stall_ticks;
    int stage;
    pipeworks_head heads[PIPEWORKS_MAX_HEADS];
    unsigned int head_count;
    pipeworks_pulse pulses[PIPEWORKS_MAX_PULSES];
    unsigned int pulse_count;
};

const screensave_saver_module *pipeworks_get_module(void);

const screensave_preset_descriptor *pipeworks_get_presets(unsigned int *count_out);
const screensave_theme_descriptor *pipeworks_get_themes(unsigned int *count_out);
const pipeworks_preset_values *pipeworks_find_preset_values(const char *preset_key);
const screensave_theme_descriptor *pipeworks_find_theme_descriptor(const char *theme_key);

void pipeworks_config_set_defaults(
    screensave_common_config *common_config,
    void *product_config,
    unsigned int product_config_size
);
void pipeworks_config_clamp(
    screensave_common_config *common_config,
    void *product_config,
    unsigned int product_config_size
);
int pipeworks_config_load(
    const screensave_saver_module *module,
    screensave_common_config *common_config,
    void *product_config,
    unsigned int product_config_size,
    screensave_diag_context *diagnostics
);
int pipeworks_config_save(
    const screensave_saver_module *module,
    const screensave_common_config *common_config,
    const void *product_config,
    unsigned int product_config_size,
    screensave_diag_context *diagnostics
);
INT_PTR pipeworks_config_show_dialog(
    HINSTANCE instance,
    HWND owner_window,
    const screensave_saver_module *module,
    screensave_common_config *common_config,
    void *product_config,
    unsigned int product_config_size,
    screensave_diag_context *diagnostics
);
int pipeworks_config_export_settings_entries(
    const screensave_saver_module *module,
    const screensave_common_config *common_config,
    const void *product_config,
    unsigned int product_config_size,
    screensave_settings_file_kind kind,
    screensave_settings_writer *writer,
    screensave_diag_context *diagnostics
);
int pipeworks_config_import_settings_entry(
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
void pipeworks_config_randomize_settings(
    const screensave_saver_module *module,
    screensave_common_config *common_config,
    void *product_config,
    unsigned int product_config_size,
    const screensave_session_seed *seed,
    screensave_diag_context *diagnostics
);

int pipeworks_create_session(
    const screensave_saver_module *module,
    screensave_saver_session **session,
    const screensave_saver_environment *environment
);
void pipeworks_destroy_session(screensave_saver_session *session);
void pipeworks_resize_session(
    screensave_saver_session *session,
    const screensave_saver_environment *environment
);
void pipeworks_step_session(
    screensave_saver_session *session,
    const screensave_saver_environment *environment
);
void pipeworks_render_session(
    screensave_saver_session *session,
    const screensave_saver_environment *environment
);

void pipeworks_rng_seed(pipeworks_rng_state *state, unsigned long seed);
unsigned long pipeworks_rng_next(pipeworks_rng_state *state);
unsigned long pipeworks_rng_range(pipeworks_rng_state *state, unsigned long upper_bound);

const char *pipeworks_density_mode_name(int density_mode);
const char *pipeworks_speed_mode_name(int speed_mode);
const char *pipeworks_branch_mode_name(int branch_mode);
const char *pipeworks_rebuild_mode_name(int rebuild_mode);

void pipeworks_apply_preset_to_config(
    const char *preset_key,
    screensave_common_config *common_config,
    pipeworks_config *product_config
);

#endif /* PIPEWORKS_INTERNAL_H */
