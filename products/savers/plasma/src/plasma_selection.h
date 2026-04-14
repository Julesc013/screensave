#ifndef PLASMA_SELECTION_H
#define PLASMA_SELECTION_H

#include "screensave/config_api.h"
#include "plasma_content.h"

#define PLASMA_CONTENT_KEY_LIST_LENGTH 256U

typedef enum plasma_content_filter_tag {
    PLASMA_CONTENT_FILTER_STABLE_ONLY = 0,
    PLASMA_CONTENT_FILTER_STABLE_AND_EXPERIMENTAL = 1,
    PLASMA_CONTENT_FILTER_EXPERIMENTAL_ONLY = 2
} plasma_content_filter;

typedef struct plasma_selection_preferences_tag {
    plasma_content_filter content_filter;
    int favorites_only;
    char preset_set_key[PLASMA_CONTENT_KEY_TEXT_LENGTH];
    char theme_set_key[PLASMA_CONTENT_KEY_TEXT_LENGTH];
    char favorite_preset_keys[PLASMA_CONTENT_KEY_LIST_LENGTH];
    char excluded_preset_keys[PLASMA_CONTENT_KEY_LIST_LENGTH];
    char favorite_theme_keys[PLASMA_CONTENT_KEY_LIST_LENGTH];
    char excluded_theme_keys[PLASMA_CONTENT_KEY_LIST_LENGTH];
} plasma_selection_preferences;

typedef struct plasma_selection_state_tag {
    const plasma_content_preset_entry *selected_preset;
    const plasma_content_theme_entry *selected_theme;
    const plasma_content_preset_set_entry *active_preset_set;
    const plasma_content_theme_set_entry *active_theme_set;
    plasma_content_filter content_filter;
    int favorites_only_requested;
    int favorites_only_applied;
    int explicit_preset_preserved;
    int explicit_theme_preserved;
    unsigned long favorite_preset_mask;
    unsigned long excluded_preset_mask;
    unsigned long favorite_theme_mask;
    unsigned long excluded_theme_mask;
} plasma_selection_state;

const char *plasma_selection_content_filter_name(plasma_content_filter filter);
int plasma_selection_parse_content_filter(const char *text, plasma_content_filter *filter_out);
void plasma_selection_preferences_set_defaults(plasma_selection_preferences *preferences);
void plasma_selection_preferences_clamp(plasma_selection_preferences *preferences);
int plasma_selection_resolve(
    plasma_selection_state *state,
    screensave_common_config *common_config,
    plasma_selection_preferences *preferences
);
int plasma_selection_state_validate(const plasma_selection_state *state);

#endif /* PLASMA_SELECTION_H */
