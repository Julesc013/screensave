#ifndef SCREENSAVE_CONFIG_API_H
#define SCREENSAVE_CONFIG_API_H

#include "screensave/types.h"

typedef enum screensave_detail_level_tag {
    SCREENSAVE_DETAIL_LEVEL_LOW = 0,
    SCREENSAVE_DETAIL_LEVEL_STANDARD = 1,
    SCREENSAVE_DETAIL_LEVEL_HIGH = 2
} screensave_detail_level;

typedef enum screensave_randomization_mode_tag {
    SCREENSAVE_RANDOMIZATION_MODE_OFF = 0,
    SCREENSAVE_RANDOMIZATION_MODE_SESSION = 1
} screensave_randomization_mode;

#define SCREENSAVE_CONFIG_SCHEMA_VERSION 1UL
#define SCREENSAVE_RANDOMIZATION_SCOPE_PRESET 0x00000001UL
#define SCREENSAVE_RANDOMIZATION_SCOPE_THEME 0x00000002UL
#define SCREENSAVE_RANDOMIZATION_SCOPE_DETAIL 0x00000004UL
#define SCREENSAVE_RANDOMIZATION_SCOPE_PRODUCT 0x00000008UL

#define SCREENSAVE_CONFIG_ISSUE_BAD_DETAIL 0x00000001UL
#define SCREENSAVE_CONFIG_ISSUE_EMPTY_PRESET 0x00000002UL
#define SCREENSAVE_CONFIG_ISSUE_EMPTY_THEME 0x00000004UL
#define SCREENSAVE_CONFIG_ISSUE_BAD_SCHEMA_VERSION 0x00000008UL
#define SCREENSAVE_CONFIG_ISSUE_BAD_RANDOMIZATION_MODE 0x00000010UL
#define SCREENSAVE_CONFIG_ISSUE_BAD_RANDOMIZATION_SCOPE 0x00000020UL

typedef struct screensave_theme_descriptor_tag {
    const char *theme_key;
    const char *display_name;
    const char *description;
    screensave_color primary_color;
    screensave_color accent_color;
} screensave_theme_descriptor;

typedef struct screensave_preset_descriptor_tag {
    const char *preset_key;
    const char *display_name;
    const char *description;
    const char *theme_key;
    screensave_detail_level detail_level;
    int use_fixed_seed;
    unsigned long fixed_seed;
} screensave_preset_descriptor;

typedef struct screensave_common_config_tag {
    unsigned long schema_version;
    screensave_detail_level detail_level;
    int diagnostics_overlay_enabled;
    int use_deterministic_seed;
    unsigned long deterministic_seed;
    screensave_randomization_mode randomization_mode;
    unsigned long randomization_scope;
    const char *preset_key;
    const char *theme_key;
} screensave_common_config;

typedef struct screensave_config_binding_tag {
    const screensave_common_config *common_config;
    const void *product_config;
    unsigned int product_config_size;
} screensave_config_binding;

void screensave_common_config_set_defaults(screensave_common_config *config);
void screensave_common_config_clamp(screensave_common_config *config);
int screensave_common_config_validate(const screensave_common_config *config, unsigned long *issue_flags);
void screensave_config_binding_init(
    screensave_config_binding *binding,
    const screensave_common_config *common_config,
    const void *product_config,
    unsigned int product_config_size
);
const screensave_preset_descriptor *screensave_find_preset(
    const screensave_preset_descriptor *presets,
    unsigned int preset_count,
    const char *preset_key
);
const screensave_theme_descriptor *screensave_find_theme(
    const screensave_theme_descriptor *themes,
    unsigned int theme_count,
    const char *theme_key
);
const char *screensave_detail_level_name(screensave_detail_level level);
const char *screensave_seed_mode_name(const screensave_common_config *config);
const char *screensave_randomization_mode_name(screensave_randomization_mode mode);
unsigned long screensave_randomization_default_scope(void);

#endif /* SCREENSAVE_CONFIG_API_H */
