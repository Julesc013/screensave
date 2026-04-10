#ifndef PLASMA_CONTENT_H
#define PLASMA_CONTENT_H

#include "screensave/config_api.h"
#include "screensave/saver_api.h"
#include "screensave/settings_api.h"
#include "plasma_transition.h"

typedef struct plasma_preset_values_tag plasma_preset_values;

#define PLASMA_CONTENT_KEY_TEXT_LENGTH 64U

typedef enum plasma_content_source_tag {
    PLASMA_CONTENT_SOURCE_BUILT_IN = 0,
    PLASMA_CONTENT_SOURCE_PACK = 1,
    PLASMA_CONTENT_SOURCE_PORTABLE = 2,
    PLASMA_CONTENT_SOURCE_USER = 3
} plasma_content_source;

typedef enum plasma_content_channel_tag {
    PLASMA_CONTENT_CHANNEL_STABLE = 0,
    PLASMA_CONTENT_CHANNEL_EXPERIMENTAL = 1
} plasma_content_channel;

typedef struct plasma_content_preset_entry_tag {
    const char *preset_key;
    const screensave_preset_descriptor *descriptor;
    plasma_content_source source;
    plasma_content_channel channel;
    plasma_preset_morph_class morph_class;
    plasma_transition_bridge_class bridge_class;
    int advanced_capable;
    int modern_capable;
    int premium_capable;
    const char *owner_pack_key;
} plasma_content_preset_entry;

typedef struct plasma_content_theme_entry_tag {
    const char *theme_key;
    const screensave_theme_descriptor *descriptor;
    plasma_content_source source;
    plasma_content_channel channel;
    plasma_theme_morph_class morph_class;
    const char *owner_pack_key;
} plasma_content_theme_entry;

typedef struct plasma_content_pack_asset_tag {
    screensave_settings_file_kind file_kind;
    const char *relative_path;
    const char *canonical_key;
} plasma_content_pack_asset;

typedef struct plasma_content_pack_entry_tag {
    const char *pack_key;
    const char *display_name;
    const char *summary;
    plasma_content_source source;
    plasma_content_channel channel;
    screensave_renderer_kind minimum_kind;
    screensave_renderer_kind preferred_kind;
    screensave_capability_quality_class quality_class;
    const char *degraded_behavior;
    const plasma_content_pack_asset *assets;
    unsigned int asset_count;
} plasma_content_pack_entry;

typedef struct plasma_content_weighted_key_tag {
    const char *content_key;
    unsigned int weight;
} plasma_content_weighted_key;

typedef struct plasma_content_preset_set_entry_tag {
    const char *set_key;
    const char *display_name;
    const char *summary;
    plasma_content_channel channel_scope;
    const plasma_content_weighted_key *members;
    unsigned int member_count;
} plasma_content_preset_set_entry;

typedef struct plasma_content_theme_set_entry_tag {
    const char *set_key;
    const char *display_name;
    const char *summary;
    plasma_content_channel channel_scope;
    const plasma_content_weighted_key *members;
    unsigned int member_count;
} plasma_content_theme_set_entry;

typedef struct plasma_content_registry_tag {
    const plasma_content_preset_entry *preset_entries;
    unsigned int preset_count;
    const plasma_content_theme_entry *theme_entries;
    unsigned int theme_count;
    const plasma_content_pack_entry *pack_entries;
    unsigned int pack_count;
    const plasma_content_preset_set_entry *preset_sets;
    unsigned int preset_set_count;
    const plasma_content_theme_set_entry *theme_sets;
    unsigned int theme_set_count;
} plasma_content_registry;

const plasma_content_registry *plasma_content_get_registry(void);
int plasma_content_registry_validate(void);
const plasma_content_preset_entry *plasma_content_find_preset_entry(const char *preset_key);
const plasma_content_theme_entry *plasma_content_find_theme_entry(const char *theme_key);
const plasma_content_pack_entry *plasma_content_find_pack_entry(const char *pack_key);
const plasma_content_preset_set_entry *plasma_content_find_preset_set(const char *set_key);
const plasma_content_theme_set_entry *plasma_content_find_theme_set(const char *set_key);
int plasma_content_preset_index(const plasma_content_preset_entry *entry);
int plasma_content_theme_index(const plasma_content_theme_entry *entry);
int plasma_content_preset_in_set(
    const plasma_content_preset_set_entry *set_entry,
    const plasma_content_preset_entry *entry
);
int plasma_content_theme_in_set(
    const plasma_content_theme_set_entry *set_entry,
    const plasma_content_theme_entry *entry
);
int plasma_content_registry_has_channel(plasma_content_channel channel);

#endif /* PLASMA_CONTENT_H */
