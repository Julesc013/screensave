#ifndef PLASMA_SETTINGS_H
#define PLASMA_SETTINGS_H

#include "screensave/config_api.h"
#include "screensave/saver_api.h"
#include "plasma_selection.h"
#include "plasma_transition.h"

struct plasma_config_tag;

typedef enum plasma_settings_surface_tag {
    PLASMA_SETTINGS_SURFACE_BASIC = 0,
    PLASMA_SETTINGS_SURFACE_ADVANCED = 1,
    PLASMA_SETTINGS_SURFACE_AUTHOR_LAB = 2
} plasma_settings_surface;

typedef enum plasma_settings_value_type_tag {
    PLASMA_SETTINGS_VALUE_BOOL = 0,
    PLASMA_SETTINGS_VALUE_ENUM = 1,
    PLASMA_SETTINGS_VALUE_UNSIGNED = 2,
    PLASMA_SETTINGS_VALUE_CONTENT_KEY = 3,
    PLASMA_SETTINGS_VALUE_CONTENT_KEY_LIST = 4
} plasma_settings_value_type;

typedef enum plasma_settings_persistence_scope_tag {
    PLASMA_SETTINGS_PERSIST_USER = 0,
    PLASMA_SETTINGS_PERSIST_SESSION = 1,
    PLASMA_SETTINGS_PERSIST_CONTENT = 2
} plasma_settings_persistence_scope;

#define PLASMA_SETTINGS_AFFECTS_CONTENT_SELECTION 0x00000001UL
#define PLASMA_SETTINGS_AFFECTS_RUNTIME_PLAN 0x00000002UL
#define PLASMA_SETTINGS_AFFECTS_EXECUTION_STATE 0x00000004UL
#define PLASMA_SETTINGS_AFFECTS_TRANSITION 0x00000008UL
#define PLASMA_SETTINGS_AFFECTS_PRESENTATION 0x00000010UL
#define PLASMA_SETTINGS_AFFECTS_DIAGNOSTICS 0x00000020UL

typedef struct plasma_settings_descriptor_tag {
    const char *setting_key;
    const char *display_name;
    const char *summary;
    plasma_settings_surface surface;
    const char *category_key;
    plasma_settings_value_type value_type;
    const char *default_value_text;
    const char *domain_summary;
    plasma_settings_persistence_scope persistence_scope;
    unsigned long affects_mask;
    int benchlab_exposable;
} plasma_settings_descriptor;

typedef struct plasma_settings_context_tag {
    const screensave_saver_module *module;
    const screensave_common_config *common_config;
    const struct plasma_config_tag *product_config;
    screensave_renderer_kind requested_renderer_kind;
    screensave_renderer_kind active_renderer_kind;
    int experimental_content_available;
    int preset_sets_available;
    int theme_sets_available;
    int favorites_configured;
    int journeys_available;
} plasma_settings_context;

typedef struct plasma_settings_resolution_tag {
    plasma_settings_surface surface;
    screensave_detail_level detail_level;
    int diagnostics_overlay_enabled;
    int use_deterministic_seed;
    unsigned long deterministic_seed;
    int effect_mode;
    int speed_mode;
    int resolution_mode;
    int smoothing_mode;
    plasma_content_filter content_filter;
    int favorites_only;
    const char *preset_set_key;
    const char *theme_set_key;
    int transitions_enabled;
    plasma_transition_policy transition_policy;
    plasma_transition_fallback_policy transition_fallback_policy;
    plasma_transition_seed_continuity_policy transition_seed_policy;
    unsigned long transition_interval_millis;
    unsigned long transition_duration_millis;
    const char *journey_key;
} plasma_settings_resolution;

const char *plasma_settings_surface_name(plasma_settings_surface surface);
const char *plasma_settings_surface_display_name(plasma_settings_surface surface);
int plasma_settings_parse_surface(const char *text, plasma_settings_surface *surface_out);

void plasma_settings_context_init(
    plasma_settings_context *context,
    const screensave_saver_module *module,
    const screensave_common_config *common_config,
    const struct plasma_config_tag *product_config,
    screensave_renderer_kind requested_renderer_kind,
    screensave_renderer_kind active_renderer_kind
);

const plasma_settings_descriptor *plasma_settings_get_catalog(unsigned int *count_out);
const plasma_settings_descriptor *plasma_settings_find_descriptor(const char *setting_key);
int plasma_settings_catalog_validate(void);
int plasma_settings_is_available(
    const plasma_settings_descriptor *descriptor,
    const plasma_settings_context *context
);

void plasma_settings_config_set_defaults(struct plasma_config_tag *product_config);
void plasma_settings_config_clamp(struct plasma_config_tag *product_config);
int plasma_settings_resolve(
    plasma_settings_resolution *resolution,
    const plasma_settings_context *context
);

#endif /* PLASMA_SETTINGS_H */
