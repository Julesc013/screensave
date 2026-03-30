#ifndef LIFEFORMS_INTERNAL_H
#define LIFEFORMS_INTERNAL_H

#include <windows.h>

#include "screensave/config_api.h"
#include "screensave/diagnostics_api.h"
#include "screensave/grid_buffer_api.h"
#include "screensave/saver_api.h"

#define LIFEFORMS_PRODUCT_REGISTRY_ROOTA "Software\\Julesc013\\ScreenSave\\Products\\lifeforms"
#define LIFEFORMS_DEFAULT_PRESET_KEY "classic_life"
#define LIFEFORMS_DEFAULT_THEME_KEY "classic_mono"
#define LIFEFORMS_PRESET_COUNT 7U
#define LIFEFORMS_THEME_COUNT 7U

typedef enum lifeforms_rule_mode_tag {
    LIFEFORMS_RULE_CONWAY = 0,
    LIFEFORMS_RULE_HIGHLIFE = 1
} lifeforms_rule_mode;

typedef enum lifeforms_density_mode_tag {
    LIFEFORMS_DENSITY_SPARSE = 0,
    LIFEFORMS_DENSITY_STANDARD = 1,
    LIFEFORMS_DENSITY_DENSE = 2
} lifeforms_density_mode;

typedef enum lifeforms_seed_mode_tag {
    LIFEFORMS_SEED_SPARSE = 0,
    LIFEFORMS_SEED_BALANCED = 1,
    LIFEFORMS_SEED_BLOOM = 2
} lifeforms_seed_mode;

typedef enum lifeforms_reseed_mode_tag {
    LIFEFORMS_RESEED_PATIENT = 0,
    LIFEFORMS_RESEED_STANDARD = 1,
    LIFEFORMS_RESEED_ALERT = 2
} lifeforms_reseed_mode;

typedef struct lifeforms_config_tag {
    int rule_mode;
    int density_mode;
    int seed_mode;
    int reseed_mode;
} lifeforms_config;

typedef struct lifeforms_preset_values_tag {
    int rule_mode;
    int density_mode;
    int seed_mode;
    int reseed_mode;
} lifeforms_preset_values;

typedef struct lifeforms_rng_state_tag {
    unsigned long state;
} lifeforms_rng_state;

extern const screensave_preset_descriptor g_lifeforms_presets[];
extern const screensave_theme_descriptor g_lifeforms_themes[];

struct screensave_saver_session_tag {
    lifeforms_config config;
    const screensave_theme_descriptor *theme;
    screensave_sizei drawable_size;
    screensave_sizei grid_size;
    int cell_size;
    screensave_detail_level detail_level;
    int preview_mode;
    lifeforms_rng_state rng;
    screensave_byte_grid current_cells;
    screensave_byte_grid next_cells;
    screensave_byte_grid ages;
    screensave_byte_grid next_ages;
    unsigned int population;
    unsigned long generation;
    unsigned long step_accumulator;
    unsigned int stagnant_ticks;
    unsigned int low_activity_ticks;
    unsigned int reseed_count;
    unsigned long recent_checksums[4];
    unsigned int recent_populations[4];
    unsigned int history_count;
    unsigned int history_index;
};

const screensave_saver_module *lifeforms_get_module(void);

const screensave_preset_descriptor *lifeforms_get_presets(unsigned int *count_out);
const screensave_theme_descriptor *lifeforms_get_themes(unsigned int *count_out);
const lifeforms_preset_values *lifeforms_find_preset_values(const char *preset_key);
const screensave_theme_descriptor *lifeforms_find_theme_descriptor(const char *theme_key);

void lifeforms_config_set_defaults(
    screensave_common_config *common_config,
    void *product_config,
    unsigned int product_config_size
);
void lifeforms_config_clamp(
    screensave_common_config *common_config,
    void *product_config,
    unsigned int product_config_size
);
int lifeforms_config_load(
    const screensave_saver_module *module,
    screensave_common_config *common_config,
    void *product_config,
    unsigned int product_config_size,
    screensave_diag_context *diagnostics
);
int lifeforms_config_save(
    const screensave_saver_module *module,
    const screensave_common_config *common_config,
    const void *product_config,
    unsigned int product_config_size,
    screensave_diag_context *diagnostics
);
INT_PTR lifeforms_config_show_dialog(
    HINSTANCE instance,
    HWND owner_window,
    const screensave_saver_module *module,
    screensave_common_config *common_config,
    void *product_config,
    unsigned int product_config_size,
    screensave_diag_context *diagnostics
);
int lifeforms_config_export_settings_entries(
    const screensave_saver_module *module,
    const screensave_common_config *common_config,
    const void *product_config,
    unsigned int product_config_size,
    screensave_settings_file_kind kind,
    screensave_settings_writer *writer,
    screensave_diag_context *diagnostics
);
int lifeforms_config_import_settings_entry(
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
void lifeforms_config_randomize_settings(
    const screensave_saver_module *module,
    screensave_common_config *common_config,
    void *product_config,
    unsigned int product_config_size,
    const screensave_session_seed *seed,
    screensave_diag_context *diagnostics
);

int lifeforms_create_session(
    const screensave_saver_module *module,
    screensave_saver_session **session,
    const screensave_saver_environment *environment
);
void lifeforms_destroy_session(screensave_saver_session *session);
void lifeforms_resize_session(
    screensave_saver_session *session,
    const screensave_saver_environment *environment
);
void lifeforms_step_session(
    screensave_saver_session *session,
    const screensave_saver_environment *environment
);
void lifeforms_render_session(
    screensave_saver_session *session,
    const screensave_saver_environment *environment
);

void lifeforms_rng_seed(lifeforms_rng_state *state, unsigned long seed);
unsigned long lifeforms_rng_next(lifeforms_rng_state *state);
unsigned long lifeforms_rng_range(lifeforms_rng_state *state, unsigned long upper_bound);

const char *lifeforms_rule_mode_name(int rule_mode);
const char *lifeforms_density_mode_name(int density_mode);
const char *lifeforms_seed_mode_name(int seed_mode);
const char *lifeforms_reseed_mode_name(int reseed_mode);

void lifeforms_apply_preset_to_config(
    const char *preset_key,
    screensave_common_config *common_config,
    lifeforms_config *product_config
);

#endif /* LIFEFORMS_INTERNAL_H */
