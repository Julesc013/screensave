#ifndef PLASMA_TRANSITION_H
#define PLASMA_TRANSITION_H

#include "screensave/saver_api.h"
#include "screensave/types.h"

struct plasma_plan_tag;
struct plasma_execution_state_tag;
struct plasma_content_preset_entry_tag;
struct plasma_content_theme_entry_tag;
struct plasma_content_preset_set_entry_tag;
struct plasma_content_theme_set_entry_tag;

#define PLASMA_TRANSITION_SUPPORTED_THEME_MORPH 0x00000001UL
#define PLASMA_TRANSITION_SUPPORTED_PRESET_MORPH 0x00000002UL
#define PLASMA_TRANSITION_SUPPORTED_BRIDGE_MORPH 0x00000004UL
#define PLASMA_TRANSITION_SUPPORTED_FALLBACK 0x00000008UL

#define PLASMA_TRANSITION_RUNTIME_REQUIRE_RESIZE 0x00000001UL
#define PLASMA_TRANSITION_RUNTIME_REQUIRE_WARM_START 0x00000002UL
#define PLASMA_TRANSITION_RUNTIME_COMMITTED_TARGET 0x00000004UL

#define PLASMA_TRANSITION_KEY_TEXT_LENGTH 64U

typedef enum plasma_preset_morph_class_tag {
    PLASMA_PRESET_MORPH_CLASS_NONE = 0,
    PLASMA_PRESET_MORPH_CLASS_FIRE = 1,
    PLASMA_PRESET_MORPH_CLASS_PLASMA = 2,
    PLASMA_PRESET_MORPH_CLASS_INTERFERENCE = 3
} plasma_preset_morph_class;

typedef enum plasma_theme_morph_class_tag {
    PLASMA_THEME_MORPH_CLASS_NONE = 0,
    PLASMA_THEME_MORPH_CLASS_WARM = 1,
    PLASMA_THEME_MORPH_CLASS_COOL = 2,
    PLASMA_THEME_MORPH_CLASS_PHOSPHOR = 3
} plasma_theme_morph_class;

typedef enum plasma_transition_bridge_class_tag {
    PLASMA_TRANSITION_BRIDGE_CLASS_NONE = 0,
    PLASMA_TRANSITION_BRIDGE_CLASS_WARM_CLASSIC = 1,
    PLASMA_TRANSITION_BRIDGE_CLASS_COOL_FIELD = 2
} plasma_transition_bridge_class;

typedef enum plasma_transition_policy_tag {
    PLASMA_TRANSITION_POLICY_DISABLED = 0,
    PLASMA_TRANSITION_POLICY_AUTO = 1,
    PLASMA_TRANSITION_POLICY_THEME_SET = 2,
    PLASMA_TRANSITION_POLICY_PRESET_SET = 3,
    PLASMA_TRANSITION_POLICY_JOURNEY = 4
} plasma_transition_policy;

typedef enum plasma_transition_type_tag {
    PLASMA_TRANSITION_TYPE_NONE = 0,
    PLASMA_TRANSITION_TYPE_THEME_MORPH = 1,
    PLASMA_TRANSITION_TYPE_PRESET_MORPH = 2,
    PLASMA_TRANSITION_TYPE_BRIDGE_MORPH = 3,
    PLASMA_TRANSITION_TYPE_FALLBACK = 4,
    PLASMA_TRANSITION_TYPE_HARD_CUT = 5,
    PLASMA_TRANSITION_TYPE_REJECTED = 6
} plasma_transition_type;

typedef enum plasma_transition_fallback_policy_tag {
    PLASMA_TRANSITION_FALLBACK_HARD_CUT = 0,
    PLASMA_TRANSITION_FALLBACK_THEME_MORPH = 1,
    PLASMA_TRANSITION_FALLBACK_REJECT = 2
} plasma_transition_fallback_policy;

typedef enum plasma_transition_seed_continuity_policy_tag {
    PLASMA_TRANSITION_SEED_CONTINUITY_KEEP_STREAM = 0,
    PLASMA_TRANSITION_SEED_CONTINUITY_RESEED_TARGET = 1
} plasma_transition_seed_continuity_policy;

typedef struct plasma_transition_preferences_tag {
    int enabled;
    plasma_transition_policy policy;
    plasma_transition_fallback_policy fallback_policy;
    plasma_transition_seed_continuity_policy seed_policy;
    unsigned long interval_millis;
    unsigned long duration_millis;
    char journey_key[PLASMA_TRANSITION_KEY_TEXT_LENGTH];
} plasma_transition_preferences;

typedef struct plasma_content_journey_step_tag {
    const char *preset_set_key;
    const char *theme_set_key;
    plasma_transition_policy policy;
    unsigned long dwell_millis;
} plasma_content_journey_step;

typedef struct plasma_content_journey_entry_tag {
    const char *journey_key;
    const char *display_name;
    const char *summary;
    const plasma_content_journey_step *steps;
    unsigned int step_count;
} plasma_content_journey_entry;

typedef struct plasma_transition_runtime_tag {
    int active;
    int bridge_switched;
    unsigned long idle_elapsed_millis;
    unsigned long elapsed_millis;
    unsigned long duration_millis;
    unsigned long supported_types;
    plasma_transition_type active_type;
    plasma_transition_type requested_type;
    plasma_transition_type fallback_type;
    const plasma_content_journey_entry *journey;
    unsigned int journey_step_index;
    unsigned int preset_member_index;
    unsigned int theme_member_index;
    const struct plasma_content_preset_entry_tag *source_preset;
    const struct plasma_content_preset_entry_tag *target_preset;
    const struct plasma_content_theme_entry_tag *source_theme;
    const struct plasma_content_theme_entry_tag *target_theme;
} plasma_transition_runtime;

const char *plasma_transition_policy_name(plasma_transition_policy policy);
const char *plasma_transition_type_name(plasma_transition_type type);
const char *plasma_transition_fallback_policy_name(plasma_transition_fallback_policy policy);
const char *plasma_transition_seed_policy_name(plasma_transition_seed_continuity_policy policy);
int plasma_transition_parse_policy(const char *text, plasma_transition_policy *policy_out);
int plasma_transition_parse_fallback_policy(
    const char *text,
    plasma_transition_fallback_policy *policy_out
);
int plasma_transition_parse_seed_policy(
    const char *text,
    plasma_transition_seed_continuity_policy *policy_out
);
void plasma_transition_preferences_set_defaults(plasma_transition_preferences *preferences);
void plasma_transition_preferences_clamp(plasma_transition_preferences *preferences);
const plasma_content_journey_entry *plasma_transition_find_journey(const char *journey_key);

void plasma_transition_plan_init(struct plasma_plan_tag *plan);
void plasma_transition_bind_plan(
    struct plasma_plan_tag *plan,
    const screensave_saver_module *module
);
int plasma_transition_validate_plan(
    const struct plasma_plan_tag *plan,
    const screensave_saver_module *module
);

void plasma_transition_runtime_init(plasma_transition_runtime *runtime);
void plasma_transition_runtime_bind(
    plasma_transition_runtime *runtime,
    const struct plasma_plan_tag *plan
);
unsigned long plasma_transition_step(
    struct plasma_plan_tag *plan,
    struct plasma_execution_state_tag *state,
    const screensave_saver_module *module,
    const screensave_saver_environment *environment
);
void plasma_transition_resolve_theme_colors(
    const struct plasma_plan_tag *plan,
    const struct plasma_execution_state_tag *state,
    screensave_color *primary_out,
    screensave_color *accent_out
);
unsigned long plasma_transition_effective_speed_units(
    const struct plasma_plan_tag *plan,
    const struct plasma_execution_state_tag *state,
    unsigned long base_speed_units
);
int plasma_transition_resolve_smoothing(
    const struct plasma_plan_tag *plan,
    const struct plasma_execution_state_tag *state,
    int *enabled_out,
    unsigned int *blend_amount_out
);
unsigned int plasma_transition_progress_amount(
    const struct plasma_execution_state_tag *state
);

#endif /* PLASMA_TRANSITION_H */
