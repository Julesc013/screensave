#ifndef ANTHOLOGY_INTERNAL_H
#define ANTHOLOGY_INTERNAL_H

#include <windows.h>

#include "screensave/config_api.h"
#include "screensave/diagnostics_api.h"
#include "screensave/renderer_api.h"
#include "screensave/saver_api.h"
#include "screensave/settings_api.h"

#define ANTHOLOGY_PRODUCT_REGISTRY_ROOTA "Software\\Julesc013\\ScreenSave\\Products\\anthology"
#define ANTHOLOGY_DEFAULT_PRESET_KEY "balanced_mix"
#define ANTHOLOGY_DEFAULT_THEME_KEY "anthology_neutral"
#define ANTHOLOGY_PRESET_COUNT 3U
#define ANTHOLOGY_THEME_COUNT 1U
#define ANTHOLOGY_INNER_SAVER_COUNT 18U
#define ANTHOLOGY_DEFAULT_INTERVAL_SECONDS 90UL
#define ANTHOLOGY_MIN_INTERVAL_SECONDS 15UL
#define ANTHOLOGY_MAX_INTERVAL_SECONDS 900UL
#define ANTHOLOGY_DEFAULT_WEIGHT 100UL
#define ANTHOLOGY_FAVORITE_WEIGHT_BONUS 100UL
#define ANTHOLOGY_TIER_WEIGHT_BONUS 50UL

#define ANTHOLOGY_FAMILY_QUIET 0x00000001UL
#define ANTHOLOGY_FAMILY_MOTION 0x00000002UL
#define ANTHOLOGY_FAMILY_FRAMEBUFFER_VECTOR 0x00000004UL
#define ANTHOLOGY_FAMILY_GRID_SIMULATION 0x00000008UL
#define ANTHOLOGY_FAMILY_SYSTEMS_AMBIENT 0x00000010UL
#define ANTHOLOGY_FAMILY_PLACES_ATMOSPHERE 0x00000020UL
#define ANTHOLOGY_FAMILY_HEAVYWEIGHT_WORLDS 0x00000040UL
#define ANTHOLOGY_FAMILY_SHOWCASE 0x00000080UL

typedef enum anthology_selection_mode_tag {
    ANTHOLOGY_SELECTION_RANDOM_UNIFORM = 0,
    ANTHOLOGY_SELECTION_WEIGHTED_RANDOM = 1,
    ANTHOLOGY_SELECTION_FAVORITES_FIRST = 2
} anthology_selection_mode;

typedef enum anthology_renderer_policy_tag {
    ANTHOLOGY_RENDERER_POLICY_BASELINE_COMPATIBLE = 0,
    ANTHOLOGY_RENDERER_POLICY_PREFER_EXPLICIT_TIER = 1,
    ANTHOLOGY_RENDERER_POLICY_REQUIRE_EXPLICIT_TIER = 2
} anthology_renderer_policy;

typedef struct anthology_config_tag {
    unsigned long selection_mode;
    unsigned long interval_seconds;
    unsigned long family_mask;
    unsigned long include_mask;
    unsigned long favorite_mask;
    unsigned long renderer_policy;
    int randomize_inner_presets;
    int randomize_inner_themes;
    int respect_inner_randomization;
    unsigned long saver_weights[ANTHOLOGY_INNER_SAVER_COUNT];
} anthology_config;

typedef struct anthology_preset_values_tag {
    unsigned long selection_mode;
    unsigned long interval_seconds;
    unsigned long family_mask;
    unsigned long include_mask;
    unsigned long favorite_mask;
    unsigned long renderer_policy;
    int randomize_inner_presets;
    int randomize_inner_themes;
    int respect_inner_randomization;
    unsigned long saver_weights[ANTHOLOGY_INNER_SAVER_COUNT];
} anthology_preset_values;

struct screensave_saver_session_tag {
    anthology_config config;
    screensave_sizei drawable_size;
    int preview_mode;
    unsigned long interval_millis;
    unsigned long selection_stream;
    unsigned long switch_count;
    unsigned long last_elapsed_millis;
    unsigned long next_switch_elapsed_millis;
    unsigned long last_effective_weight;
    unsigned long last_family_flags;
    screensave_renderer_kind last_renderer_kind;
    unsigned int last_candidate_count;
    int last_favorites_only;
    int current_catalog_index;
    int previous_catalog_index;
    const screensave_saver_module *inner_module;
    screensave_saver_session *inner_session;
    screensave_saver_config_state inner_stored_config;
    screensave_saver_config_state inner_resolved_config;
    screensave_config_binding inner_config_binding;
    screensave_session_seed inner_seed;
    char last_selection_reason[160];
    char last_filter_summary[160];
};

extern const screensave_preset_descriptor g_anthology_presets[];
extern const screensave_theme_descriptor g_anthology_themes[];

const screensave_saver_module *anthology_get_module(void);

const screensave_preset_descriptor *anthology_get_presets(unsigned int *count_out);
const screensave_theme_descriptor *anthology_get_themes(unsigned int *count_out);
const anthology_preset_values *anthology_find_preset_values(const char *preset_key);
const screensave_theme_descriptor *anthology_find_theme_descriptor(const char *theme_key);
void anthology_apply_preset_to_config(
    const char *preset_key,
    screensave_common_config *common_config,
    anthology_config *product_config
);

unsigned int anthology_catalog_count(void);
const screensave_saver_module *anthology_catalog_get_module(unsigned int index);
const char *anthology_catalog_get_product_key(unsigned int index);
const char *anthology_catalog_get_display_name(unsigned int index);
unsigned long anthology_catalog_get_family_flags(unsigned int index);
unsigned long anthology_catalog_get_default_weight(unsigned int index);
int anthology_catalog_find_index(const char *product_key);
unsigned long anthology_all_family_flags(void);
unsigned long anthology_all_saver_bits(void);

const char *anthology_selection_mode_name(unsigned long selection_mode);
const char *anthology_renderer_policy_name(unsigned long renderer_policy);
const char *anthology_family_display_name(unsigned long family_flag);
int anthology_session_build_summary(
    const screensave_saver_session *session,
    char *buffer,
    unsigned int buffer_size
);

void anthology_config_set_defaults(
    screensave_common_config *common_config,
    void *product_config,
    unsigned int product_config_size
);
void anthology_config_clamp(
    screensave_common_config *common_config,
    void *product_config,
    unsigned int product_config_size
);
int anthology_config_load(
    const screensave_saver_module *module,
    screensave_common_config *common_config,
    void *product_config,
    unsigned int product_config_size,
    screensave_diag_context *diagnostics
);
int anthology_config_save(
    const screensave_saver_module *module,
    const screensave_common_config *common_config,
    const void *product_config,
    unsigned int product_config_size,
    screensave_diag_context *diagnostics
);
INT_PTR anthology_config_show_dialog(
    HINSTANCE instance,
    HWND owner_window,
    const screensave_saver_module *module,
    screensave_common_config *common_config,
    void *product_config,
    unsigned int product_config_size,
    screensave_diag_context *diagnostics
);
int anthology_config_export_settings_entries(
    const screensave_saver_module *module,
    const screensave_common_config *common_config,
    const void *product_config,
    unsigned int product_config_size,
    screensave_settings_file_kind kind,
    screensave_settings_writer *writer,
    screensave_diag_context *diagnostics
);
int anthology_config_import_settings_entry(
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
void anthology_config_randomize_settings(
    const screensave_saver_module *module,
    screensave_common_config *common_config,
    void *product_config,
    unsigned int product_config_size,
    const screensave_session_seed *seed,
    screensave_diag_context *diagnostics
);

int anthology_create_session(
    const screensave_saver_module *module,
    screensave_saver_session **session,
    const screensave_saver_environment *environment
);
void anthology_destroy_session(screensave_saver_session *session);
void anthology_resize_session(
    screensave_saver_session *session,
    const screensave_saver_environment *environment
);
void anthology_step_session(
    screensave_saver_session *session,
    const screensave_saver_environment *environment
);
void anthology_render_session(
    screensave_saver_session *session,
    const screensave_saver_environment *environment
);

#endif /* ANTHOLOGY_INTERNAL_H */
