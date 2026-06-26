#ifndef RICOCHET_INTERNAL_H
#define RICOCHET_INTERNAL_H

#include <windows.h>

#include "screensave/config_api.h"
#include "screensave/diagnostics_api.h"
#include "screensave/saver_api.h"
#include "ricochet_core.h"

#define RICOCHET_PRODUCT_REGISTRY_ROOTA "Software\\Julesc013\\ScreenSave\\Products\\ricochet"

typedef struct ricochet_rng_state_tag {
    unsigned long state;
} ricochet_rng_state;

extern const screensave_preset_descriptor g_ricochet_presets[];
extern const screensave_theme_descriptor g_ricochet_themes[];

struct screensave_saver_session_tag {
    ricochet_core_session *core;
};

const screensave_saver_module *ricochet_get_module(void);

const screensave_preset_descriptor *ricochet_get_presets(unsigned int *count_out);
const screensave_theme_descriptor *ricochet_get_themes(unsigned int *count_out);
const ricochet_preset_values *ricochet_find_preset_values(const char *preset_key);
const screensave_theme_descriptor *ricochet_find_theme_descriptor(const char *theme_key);

void ricochet_config_set_defaults(
    screensave_common_config *common_config,
    void *product_config,
    unsigned int product_config_size
);
void ricochet_config_clamp(
    screensave_common_config *common_config,
    void *product_config,
    unsigned int product_config_size
);
int ricochet_config_load(
    const screensave_saver_module *module,
    screensave_common_config *common_config,
    void *product_config,
    unsigned int product_config_size,
    screensave_diag_context *diagnostics
);
int ricochet_config_save(
    const screensave_saver_module *module,
    const screensave_common_config *common_config,
    const void *product_config,
    unsigned int product_config_size,
    screensave_diag_context *diagnostics
);
INT_PTR ricochet_config_show_dialog(
    HINSTANCE instance,
    HWND owner_window,
    const screensave_saver_module *module,
    screensave_common_config *common_config,
    void *product_config,
    unsigned int product_config_size,
    screensave_diag_context *diagnostics
);
int ricochet_config_export_settings_entries(
    const screensave_saver_module *module,
    const screensave_common_config *common_config,
    const void *product_config,
    unsigned int product_config_size,
    screensave_settings_file_kind kind,
    screensave_settings_writer *writer,
    screensave_diag_context *diagnostics
);
int ricochet_config_import_settings_entry(
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
void ricochet_config_randomize_settings(
    const screensave_saver_module *module,
    screensave_common_config *common_config,
    void *product_config,
    unsigned int product_config_size,
    const screensave_session_seed *seed,
    screensave_diag_context *diagnostics
);

int ricochet_create_session(
    const screensave_saver_module *module,
    screensave_saver_session **session,
    const screensave_saver_environment *environment
);
void ricochet_destroy_session(screensave_saver_session *session);
void ricochet_resize_session(
    screensave_saver_session *session,
    const screensave_saver_environment *environment
);
void ricochet_step_session(
    screensave_saver_session *session,
    const screensave_saver_environment *environment
);
void ricochet_render_session(
    screensave_saver_session *session,
    const screensave_saver_environment *environment
);

void ricochet_rng_seed(ricochet_rng_state *state, unsigned long seed);
unsigned long ricochet_rng_next(ricochet_rng_state *state);
unsigned long ricochet_rng_range(ricochet_rng_state *state, unsigned long upper_bound);

const char *ricochet_object_mode_name(int object_mode);
const char *ricochet_speed_mode_name(int speed_mode);
const char *ricochet_trail_mode_name(int trail_mode);

void ricochet_apply_preset_to_config(
    const char *preset_key,
    screensave_common_config *common_config,
    ricochet_config *product_config
);

#endif /* RICOCHET_INTERNAL_H */
