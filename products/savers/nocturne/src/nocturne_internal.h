#ifndef NOCTURNE_INTERNAL_H
#define NOCTURNE_INTERNAL_H

#include <windows.h>

#include "screensave/config_api.h"
#include "screensave/diagnostics_api.h"
#include "screensave/saver_api.h"
#include "nocturne_core.h"

#define NOCTURNE_PRODUCT_REGISTRY_ROOTA "Software\\Julesc013\\ScreenSave\\Products\\nocturne"

extern const screensave_preset_descriptor g_nocturne_presets[];
extern const screensave_theme_descriptor g_nocturne_themes[];

typedef struct nocturne_rng_state_tag {
    unsigned long state;
} nocturne_rng_state;

struct screensave_saver_session_tag {
    nocturne_core_session *core;
};

const screensave_saver_module *nocturne_get_module(void);

const screensave_preset_descriptor *nocturne_get_presets(unsigned int *count_out);
const screensave_theme_descriptor *nocturne_get_themes(unsigned int *count_out);
const nocturne_preset_values *nocturne_find_preset_values(const char *preset_key);
const screensave_theme_descriptor *nocturne_find_theme_descriptor(const char *theme_key);

void nocturne_config_set_defaults(
    screensave_common_config *common_config,
    void *product_config,
    unsigned int product_config_size
);
void nocturne_config_clamp(
    screensave_common_config *common_config,
    void *product_config,
    unsigned int product_config_size
);
int nocturne_config_load(
    const screensave_saver_module *module,
    screensave_common_config *common_config,
    void *product_config,
    unsigned int product_config_size,
    screensave_diag_context *diagnostics
);
int nocturne_config_save(
    const screensave_saver_module *module,
    const screensave_common_config *common_config,
    const void *product_config,
    unsigned int product_config_size,
    screensave_diag_context *diagnostics
);
INT_PTR nocturne_config_show_dialog(
    HINSTANCE instance,
    HWND owner_window,
    const screensave_saver_module *module,
    screensave_common_config *common_config,
    void *product_config,
    unsigned int product_config_size,
    screensave_diag_context *diagnostics
);
int nocturne_config_export_settings_entries(
    const screensave_saver_module *module,
    const screensave_common_config *common_config,
    const void *product_config,
    unsigned int product_config_size,
    screensave_settings_file_kind kind,
    screensave_settings_writer *writer,
    screensave_diag_context *diagnostics
);
int nocturne_config_import_settings_entry(
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
void nocturne_config_randomize_settings(
    const screensave_saver_module *module,
    screensave_common_config *common_config,
    void *product_config,
    unsigned int product_config_size,
    const screensave_session_seed *seed,
    screensave_diag_context *diagnostics
);

int nocturne_create_session(
    const screensave_saver_module *module,
    screensave_saver_session **session,
    const screensave_saver_environment *environment
);
void nocturne_destroy_session(screensave_saver_session *session);
void nocturne_resize_session(
    screensave_saver_session *session,
    const screensave_saver_environment *environment
);
void nocturne_step_session(
    screensave_saver_session *session,
    const screensave_saver_environment *environment
);
void nocturne_render_session(
    screensave_saver_session *session,
    const screensave_saver_environment *environment
);

void nocturne_rng_seed(nocturne_rng_state *state, unsigned long seed);
unsigned long nocturne_rng_next(nocturne_rng_state *state);
unsigned long nocturne_rng_range(nocturne_rng_state *state, unsigned long upper_bound);

const char *nocturne_motion_mode_name(int motion_mode);
const char *nocturne_fade_speed_name(int fade_speed);
const char *nocturne_motion_strength_name(int motion_strength);

void nocturne_apply_preset_to_config(
    const char *preset_key,
    screensave_common_config *common_config,
    nocturne_config *product_config
);

#endif /* NOCTURNE_INTERNAL_H */
