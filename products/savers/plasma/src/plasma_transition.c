#include <string.h>

#include "plasma_internal.h"

static const plasma_content_journey_step g_classic_cycle_steps[] = {
    { "fire_classics", "warm_classics", PLASMA_TRANSITION_POLICY_PRESET_SET, 9000UL },
    { "dark_room_classics", "warm_classics", PLASMA_TRANSITION_POLICY_PRESET_SET, 12000UL }
};

static const plasma_content_journey_step g_dark_room_cycle_steps[] = {
    { "dark_room_classics", "warm_classics", PLASMA_TRANSITION_POLICY_PRESET_SET, 12000UL }
};

static const plasma_content_journey_step g_warm_bridge_cycle_steps[] = {
    { "warm_bridge_classics", "warm_classics", PLASMA_TRANSITION_POLICY_PRESET_SET, 9000UL }
};

static const plasma_content_journey_step g_cool_bridge_cycle_steps[] = {
    { "cool_bridge_classics", "cool_classics", PLASMA_TRANSITION_POLICY_PRESET_SET, 9000UL }
};

static const plasma_content_journey_entry g_transition_journeys[] = {
    {
        "classic_cycle",
        "Classic Cycle",
        "Ordered journey across the preserved classic core and dark-room subsets.",
        g_classic_cycle_steps,
        (unsigned int)(sizeof(g_classic_cycle_steps) / sizeof(g_classic_cycle_steps[0]))
    },
    {
        "dark_room_cycle",
        "Dark-Room Cycle",
        "Ordered journey across the quieter dark-room-safe classic subset.",
        g_dark_room_cycle_steps,
        (unsigned int)(sizeof(g_dark_room_cycle_steps) / sizeof(g_dark_room_cycle_steps[0]))
    },
    {
        "warm_bridge_cycle",
        "Warm Bridge Cycle",
        "Ordered journey across the warm classic subset that alternates direct preset morphs with bounded warm-bridge crossings.",
        g_warm_bridge_cycle_steps,
        (unsigned int)(sizeof(g_warm_bridge_cycle_steps) / sizeof(g_warm_bridge_cycle_steps[0]))
    },
    {
        "cool_bridge_cycle",
        "Cool Bridge Cycle",
        "Ordered journey across the cool classic subset that alternates direct preset morphs with bounded cool-field bridge crossings.",
        g_cool_bridge_cycle_steps,
        (unsigned int)(sizeof(g_cool_bridge_cycle_steps) / sizeof(g_cool_bridge_cycle_steps[0]))
    }
};

static unsigned long plasma_transition_supported_types_for_kind(
    screensave_renderer_kind renderer_kind
)
{
    if (plasma_is_lower_band_kind(renderer_kind)) {
        return
            PLASMA_TRANSITION_SUPPORTED_THEME_MORPH |
            PLASMA_TRANSITION_SUPPORTED_PRESET_MORPH |
            PLASMA_TRANSITION_SUPPORTED_FALLBACK;
    }

    if (
        renderer_kind == SCREENSAVE_RENDERER_KIND_GL21 ||
        renderer_kind == SCREENSAVE_RENDERER_KIND_GL33 ||
        renderer_kind == SCREENSAVE_RENDERER_KIND_GL46
    ) {
        return
            PLASMA_TRANSITION_SUPPORTED_THEME_MORPH |
            PLASMA_TRANSITION_SUPPORTED_PRESET_MORPH |
            PLASMA_TRANSITION_SUPPORTED_BRIDGE_MORPH |
            PLASMA_TRANSITION_SUPPORTED_FALLBACK;
    }

    return 0UL;
}

static int plasma_transition_policy_has_surface(
    const struct plasma_plan_tag *plan
)
{
    if (plan == NULL) {
        return 0;
    }

    switch (plan->transition_policy) {
    case PLASMA_TRANSITION_POLICY_THEME_SET:
        return plan->selection.active_theme_set != NULL;

    case PLASMA_TRANSITION_POLICY_PRESET_SET:
        return plan->selection.active_preset_set != NULL;

    case PLASMA_TRANSITION_POLICY_JOURNEY:
        return plan->journey != NULL;

    case PLASMA_TRANSITION_POLICY_AUTO:
        return
            plan->journey != NULL ||
            plan->selection.active_preset_set != NULL ||
            plan->selection.active_theme_set != NULL;

    case PLASMA_TRANSITION_POLICY_DISABLED:
    default:
        return 0;
    }
}

static unsigned int plasma_transition_clamp_amount(
    unsigned long numerator,
    unsigned long denominator
)
{
    if (denominator == 0UL || numerator >= denominator) {
        return 255U;
    }

    return (unsigned int)((numerator * 255UL) / denominator);
}

static unsigned long plasma_transition_speed_units_for_mode(int speed_mode)
{
    switch (speed_mode) {
    case PLASMA_SPEED_GENTLE:
        return 2UL;

    case PLASMA_SPEED_LIVELY:
        return 7UL;

    case PLASMA_SPEED_STANDARD:
    default:
        return 4UL;
    }
}

static unsigned int plasma_transition_smoothing_amount_for_mode(int smoothing_mode)
{
    switch (smoothing_mode) {
    case PLASMA_SMOOTHING_GLOW:
        return 128U;

    case PLASMA_SMOOTHING_SOFT:
        return 72U;

    case PLASMA_SMOOTHING_OFF:
    default:
        return 0U;
    }
}

static const plasma_content_journey_step *plasma_transition_active_journey_step(
    const struct plasma_plan_tag *plan,
    const plasma_transition_runtime *runtime
)
{
    if (
        plan == NULL ||
        runtime == NULL ||
        runtime->journey == NULL ||
        runtime->journey->step_count == 0U ||
        (plan->transition_policy != PLASMA_TRANSITION_POLICY_JOURNEY &&
            plan->transition_policy != PLASMA_TRANSITION_POLICY_AUTO)
    ) {
        return NULL;
    }

    return &runtime->journey->steps[runtime->journey_step_index % runtime->journey->step_count];
}

static const plasma_content_preset_set_entry *plasma_transition_active_preset_set(
    const struct plasma_plan_tag *plan,
    const plasma_transition_runtime *runtime,
    plasma_transition_policy *policy_out,
    unsigned long *interval_out
)
{
    const plasma_content_journey_step *step;
    plasma_transition_policy policy;

    if (policy_out != NULL) {
        *policy_out = plan != NULL ? plan->transition_policy : PLASMA_TRANSITION_POLICY_DISABLED;
    }
    if (interval_out != NULL) {
        *interval_out = plan != NULL ? plan->transition_interval_millis : 0UL;
    }
    if (plan == NULL) {
        return NULL;
    }

    policy = plan->transition_policy;
    step = plasma_transition_active_journey_step(plan, runtime);
    if (step != NULL) {
        policy = step->policy;
        if (policy == PLASMA_TRANSITION_POLICY_DISABLED) {
            policy = PLASMA_TRANSITION_POLICY_AUTO;
        }
        if (policy_out != NULL) {
            *policy_out = policy;
        }
        if (interval_out != NULL && step->dwell_millis > 0UL) {
            *interval_out = step->dwell_millis;
        }
        if (step->preset_set_key != NULL && step->preset_set_key[0] != '\0') {
            return plasma_content_find_preset_set(step->preset_set_key);
        }
    }

    if (policy == PLASMA_TRANSITION_POLICY_PRESET_SET || policy == PLASMA_TRANSITION_POLICY_AUTO) {
        return plan->selection.active_preset_set;
    }

    return NULL;
}

static const plasma_content_theme_set_entry *plasma_transition_active_theme_set(
    const struct plasma_plan_tag *plan,
    const plasma_transition_runtime *runtime,
    plasma_transition_policy *policy_out,
    unsigned long *interval_out
)
{
    const plasma_content_journey_step *step;
    plasma_transition_policy policy;

    if (policy_out != NULL) {
        *policy_out = plan != NULL ? plan->transition_policy : PLASMA_TRANSITION_POLICY_DISABLED;
    }
    if (interval_out != NULL) {
        *interval_out = plan != NULL ? plan->transition_interval_millis : 0UL;
    }
    if (plan == NULL) {
        return NULL;
    }

    policy = plan->transition_policy;
    step = plasma_transition_active_journey_step(plan, runtime);
    if (step != NULL) {
        policy = step->policy;
        if (policy == PLASMA_TRANSITION_POLICY_DISABLED) {
            policy = PLASMA_TRANSITION_POLICY_AUTO;
        }
        if (policy_out != NULL) {
            *policy_out = policy;
        }
        if (interval_out != NULL && step->dwell_millis > 0UL) {
            *interval_out = step->dwell_millis;
        }
        if (step->theme_set_key != NULL && step->theme_set_key[0] != '\0') {
            return plasma_content_find_theme_set(step->theme_set_key);
        }
    }

    if (
        policy == PLASMA_TRANSITION_POLICY_THEME_SET ||
        policy == PLASMA_TRANSITION_POLICY_PRESET_SET ||
        policy == PLASMA_TRANSITION_POLICY_AUTO
    ) {
        return plan->selection.active_theme_set;
    }

    return NULL;
}

static const plasma_content_preset_entry *plasma_transition_next_preset(
    const plasma_content_preset_set_entry *set_entry,
    const plasma_content_preset_entry *current_entry,
    unsigned int *cursor_out
)
{
    unsigned int index;

    if (set_entry == NULL || set_entry->member_count == 0U) {
        return NULL;
    }

    if (current_entry != NULL) {
        for (index = 0U; index < set_entry->member_count; ++index) {
            if (strcmp(set_entry->members[index].content_key, current_entry->preset_key) == 0) {
                break;
            }
        }
        if (index >= set_entry->member_count) {
            index = 0U;
        } else {
            index = (index + 1U) % set_entry->member_count;
        }
    } else {
        index = 0U;
    }

    if (cursor_out != NULL) {
        *cursor_out = index;
    }
    return plasma_content_find_preset_entry(set_entry->members[index].content_key);
}

static const plasma_content_theme_entry *plasma_transition_next_theme(
    const plasma_content_theme_set_entry *set_entry,
    const plasma_content_theme_entry *current_entry,
    unsigned int *cursor_out
)
{
    unsigned int index;

    if (set_entry == NULL || set_entry->member_count == 0U) {
        return NULL;
    }

    if (current_entry != NULL) {
        for (index = 0U; index < set_entry->member_count; ++index) {
            if (strcmp(set_entry->members[index].content_key, current_entry->theme_key) == 0) {
                break;
            }
        }
        if (index >= set_entry->member_count) {
            index = 0U;
        } else {
            index = (index + 1U) % set_entry->member_count;
        }
    } else {
        index = 0U;
    }

    if (cursor_out != NULL) {
        *cursor_out = index;
    }
    return plasma_content_find_theme_entry(set_entry->members[index].content_key);
}

static int plasma_transition_direct_preset_supported(
    const plasma_content_preset_entry *source_preset,
    const plasma_content_preset_entry *target_preset
)
{
    if (source_preset == NULL || target_preset == NULL) {
        return 0;
    }

    return
        source_preset->morph_class != PLASMA_PRESET_MORPH_CLASS_NONE &&
        source_preset->morph_class == target_preset->morph_class;
}

static int plasma_transition_direct_theme_supported(
    const plasma_content_theme_entry *source_theme,
    const plasma_content_theme_entry *target_theme
)
{
    if (source_theme == NULL || target_theme == NULL) {
        return 0;
    }

    return
        source_theme->morph_class != PLASMA_THEME_MORPH_CLASS_NONE &&
        source_theme->morph_class == target_theme->morph_class;
}

static int plasma_transition_bridge_supported(
    const plasma_content_preset_entry *source_preset,
    const plasma_content_preset_entry *target_preset
)
{
    if (source_preset == NULL || target_preset == NULL) {
        return 0;
    }

    return
        source_preset->bridge_class != PLASMA_TRANSITION_BRIDGE_CLASS_NONE &&
        source_preset->bridge_class == target_preset->bridge_class &&
        source_preset->morph_class != target_preset->morph_class;
}

static unsigned long plasma_transition_hash_text(const char *text)
{
    unsigned long hash;
    const unsigned char *cursor;

    if (text == NULL) {
        return 0UL;
    }

    hash = 2166136261UL;
    cursor = (const unsigned char *)text;
    while (*cursor != '\0') {
        hash ^= (unsigned long)(*cursor);
        hash *= 16777619UL;
        ++cursor;
    }

    return hash;
}

static unsigned long plasma_transition_commit_target(
    struct plasma_plan_tag *plan,
    struct plasma_execution_state_tag *state,
    const screensave_saver_module *module
)
{
    plasma_transition_runtime *runtime;
    const plasma_preset_values *preset_values;
    int resolution_changed;
    int detail_changed;
    int effect_changed;

    if (plan == NULL || state == NULL) {
        return 0UL;
    }

    runtime = &state->transition;
    if (runtime->target_preset == NULL || runtime->target_theme == NULL) {
        return 0UL;
    }

    preset_values = plasma_find_preset_values(runtime->target_preset->preset_key);
    if (preset_values == NULL) {
        return 0UL;
    }

    resolution_changed = plan->resolution_mode != preset_values->resolution_mode;
    detail_changed = plan->detail_level != runtime->target_preset->descriptor->detail_level;
    effect_changed = plan->effect_mode != preset_values->effect_mode;

    plan->selection.selected_preset = runtime->target_preset;
    plan->selection.selected_theme = runtime->target_theme;
    plan->preset_key = runtime->target_preset->preset_key;
    plan->preset = runtime->target_preset->descriptor;
    plan->theme_key = runtime->target_theme->theme_key;
    plan->theme = runtime->target_theme->descriptor;
    plan->detail_level = runtime->target_preset->descriptor->detail_level;
    plan->effect_mode = preset_values->effect_mode;
    plan->speed_mode = preset_values->speed_mode;
    plan->resolution_mode = preset_values->resolution_mode;
    plan->smoothing_mode = preset_values->smoothing_mode;
    if (
        plan->transition_seed_policy == PLASMA_TRANSITION_SEED_CONTINUITY_RESEED_TARGET &&
        runtime->target_preset->descriptor->use_fixed_seed
    ) {
        plan->seed_policy = PLASMA_PLAN_SEED_POLICY_FIXED;
        plan->configured_seed = runtime->target_preset->descriptor->fixed_seed;
        plan->resolved_rng_seed =
            runtime->target_preset->descriptor->fixed_seed ^
            plan->stream_seed ^
            plasma_transition_hash_text(runtime->target_preset->preset_key);
        plan->deterministic = 1;
        plasma_rng_seed(&state->rng, plan->resolved_rng_seed);
    }

    plasma_plan_bind_renderer_kind(
        plan,
        module,
        plan->requested_renderer_kind,
        state->active_renderer_kind
    );
    plasma_transition_bind_plan(plan, module);

    if (runtime->journey != NULL && runtime->journey->step_count > 0U) {
        runtime->journey_step_index =
            (runtime->journey_step_index + 1U) % runtime->journey->step_count;
    }

    return
        PLASMA_TRANSITION_RUNTIME_COMMITTED_TARGET |
        ((resolution_changed || detail_changed) ? PLASMA_TRANSITION_RUNTIME_REQUIRE_RESIZE : 0UL) |
        ((effect_changed || plan->transition_seed_policy == PLASMA_TRANSITION_SEED_CONTINUITY_RESEED_TARGET)
            ? PLASMA_TRANSITION_RUNTIME_REQUIRE_WARM_START
            : 0UL);
}

static void plasma_transition_begin(
    plasma_transition_runtime *runtime,
    const plasma_content_preset_entry *source_preset,
    const plasma_content_preset_entry *target_preset,
    const plasma_content_theme_entry *source_theme,
    const plasma_content_theme_entry *target_theme,
    plasma_transition_type active_type,
    unsigned long duration_millis
)
{
    if (runtime == NULL) {
        return;
    }

    runtime->active = active_type != PLASMA_TRANSITION_TYPE_NONE;
    runtime->bridge_switched = 0;
    runtime->elapsed_millis = 0UL;
    runtime->duration_millis = duration_millis;
    runtime->active_type = active_type;
    runtime->fallback_type = active_type == PLASMA_TRANSITION_TYPE_FALLBACK
        ? PLASMA_TRANSITION_TYPE_THEME_MORPH
        : PLASMA_TRANSITION_TYPE_NONE;
    runtime->source_preset = source_preset;
    runtime->target_preset = target_preset;
    runtime->source_theme = source_theme;
    runtime->target_theme = target_theme;
}

static unsigned long plasma_transition_start_next(
    struct plasma_plan_tag *plan,
    struct plasma_execution_state_tag *state,
    const screensave_saver_module *module
)
{
    plasma_transition_runtime *runtime;
    const plasma_content_preset_entry *target_preset;
    const plasma_content_theme_entry *target_theme;
    const plasma_content_preset_set_entry *preset_set;
    const plasma_content_theme_set_entry *theme_set;
    plasma_transition_policy active_policy;
    unsigned long interval_millis;

    if (plan == NULL || state == NULL) {
        return 0UL;
    }

    runtime = &state->transition;
    target_preset = NULL;
    target_theme = NULL;
    active_policy = plan->transition_policy;
    interval_millis = plan->transition_interval_millis;
    preset_set = plasma_transition_active_preset_set(plan, runtime, &active_policy, &interval_millis);
    theme_set = plasma_transition_active_theme_set(plan, runtime, &active_policy, &interval_millis);
    runtime->requested_type = PLASMA_TRANSITION_TYPE_NONE;

    if (active_policy == PLASMA_TRANSITION_POLICY_THEME_SET) {
        target_preset = plan->selection.selected_preset;
        target_theme = plasma_transition_next_theme(
            theme_set,
            plan->selection.selected_theme,
            &runtime->theme_member_index
        );
        runtime->requested_type = PLASMA_TRANSITION_TYPE_THEME_MORPH;
    } else if (active_policy == PLASMA_TRANSITION_POLICY_PRESET_SET) {
        target_preset = plasma_transition_next_preset(
            preset_set,
            plan->selection.selected_preset,
            &runtime->preset_member_index
        );
        target_theme = theme_set != NULL
            ? plasma_transition_next_theme(
                theme_set,
                plan->selection.selected_theme,
                &runtime->theme_member_index
            )
            : (target_preset != NULL
                ? plasma_content_find_theme_entry(target_preset->descriptor->theme_key)
                : NULL);
        runtime->requested_type = PLASMA_TRANSITION_TYPE_PRESET_MORPH;
    } else if (active_policy == PLASMA_TRANSITION_POLICY_AUTO) {
        if (preset_set != NULL) {
            target_preset = plasma_transition_next_preset(
                preset_set,
                plan->selection.selected_preset,
                &runtime->preset_member_index
            );
            target_theme = theme_set != NULL
                ? plasma_transition_next_theme(
                    theme_set,
                    plan->selection.selected_theme,
                    &runtime->theme_member_index
                )
                : (target_preset != NULL
                    ? plasma_content_find_theme_entry(target_preset->descriptor->theme_key)
                    : NULL);
            runtime->requested_type = PLASMA_TRANSITION_TYPE_PRESET_MORPH;
        } else if (theme_set != NULL) {
            target_preset = plan->selection.selected_preset;
            target_theme = plasma_transition_next_theme(
                theme_set,
                plan->selection.selected_theme,
                &runtime->theme_member_index
            );
            runtime->requested_type = PLASMA_TRANSITION_TYPE_THEME_MORPH;
        }
    }

    if (target_preset == NULL) {
        target_preset = plan->selection.selected_preset;
    }
    if (target_theme == NULL) {
        target_theme = plan->selection.selected_theme;
    }

    if (
        target_preset == NULL ||
        target_theme == NULL ||
        (target_preset == plan->selection.selected_preset && target_theme == plan->selection.selected_theme)
    ) {
        runtime->idle_elapsed_millis = 0UL;
        return 0UL;
    }

    if (
        target_preset == plan->selection.selected_preset &&
        target_theme != plan->selection.selected_theme &&
        plasma_transition_direct_theme_supported(plan->selection.selected_theme, target_theme) &&
        (runtime->supported_types & PLASMA_TRANSITION_SUPPORTED_THEME_MORPH) != 0UL
    ) {
        plasma_transition_begin(
            runtime,
            plan->selection.selected_preset,
            target_preset,
            plan->selection.selected_theme,
            target_theme,
            PLASMA_TRANSITION_TYPE_THEME_MORPH,
            plan->transition_duration_millis
        );
        return 0UL;
    }

    if (
        target_preset != plan->selection.selected_preset &&
        plasma_transition_direct_preset_supported(plan->selection.selected_preset, target_preset) &&
        (runtime->supported_types & PLASMA_TRANSITION_SUPPORTED_PRESET_MORPH) != 0UL
    ) {
        plasma_transition_begin(
            runtime,
            plan->selection.selected_preset,
            target_preset,
            plan->selection.selected_theme,
            target_theme,
            PLASMA_TRANSITION_TYPE_PRESET_MORPH,
            plan->transition_duration_millis
        );
        return 0UL;
    }

    if (
        target_preset != plan->selection.selected_preset &&
        plasma_transition_bridge_supported(plan->selection.selected_preset, target_preset) &&
        (runtime->supported_types & PLASMA_TRANSITION_SUPPORTED_BRIDGE_MORPH) != 0UL
    ) {
        plasma_transition_begin(
            runtime,
            plan->selection.selected_preset,
            target_preset,
            plan->selection.selected_theme,
            target_theme,
            PLASMA_TRANSITION_TYPE_BRIDGE_MORPH,
            plan->transition_duration_millis
        );
        return 0UL;
    }

    if (
        plan->transition_fallback_policy == PLASMA_TRANSITION_FALLBACK_THEME_MORPH &&
        target_theme != plan->selection.selected_theme &&
        plasma_transition_direct_theme_supported(plan->selection.selected_theme, target_theme) &&
        (runtime->supported_types & PLASMA_TRANSITION_SUPPORTED_FALLBACK) != 0UL &&
        (runtime->supported_types & PLASMA_TRANSITION_SUPPORTED_THEME_MORPH) != 0UL
    ) {
        plasma_transition_begin(
            runtime,
            plan->selection.selected_preset,
            target_preset,
            plan->selection.selected_theme,
            target_theme,
            PLASMA_TRANSITION_TYPE_FALLBACK,
            plan->transition_duration_millis
        );
        return 0UL;
    }

    if (plan->transition_fallback_policy == PLASMA_TRANSITION_FALLBACK_HARD_CUT) {
        runtime->source_preset = plan->selection.selected_preset;
        runtime->target_preset = target_preset;
        runtime->source_theme = plan->selection.selected_theme;
        runtime->target_theme = target_theme;
        runtime->active = 0;
        runtime->active_type = PLASMA_TRANSITION_TYPE_HARD_CUT;
        runtime->idle_elapsed_millis = 0UL;
        return plasma_transition_commit_target(plan, state, module);
    }

    runtime->active = 0;
    runtime->active_type = PLASMA_TRANSITION_TYPE_REJECTED;
    runtime->idle_elapsed_millis = 0UL;
    return 0UL;
}

const char *plasma_transition_policy_name(plasma_transition_policy policy)
{
    switch (policy) {
    case PLASMA_TRANSITION_POLICY_AUTO:
        return "auto";
    case PLASMA_TRANSITION_POLICY_THEME_SET:
        return "theme_set";
    case PLASMA_TRANSITION_POLICY_PRESET_SET:
        return "preset_set";
    case PLASMA_TRANSITION_POLICY_JOURNEY:
        return "journey";
    case PLASMA_TRANSITION_POLICY_DISABLED:
    default:
        return "disabled";
    }
}

const char *plasma_transition_type_name(plasma_transition_type type)
{
    switch (type) {
    case PLASMA_TRANSITION_TYPE_THEME_MORPH:
        return "theme_morph";
    case PLASMA_TRANSITION_TYPE_PRESET_MORPH:
        return "preset_morph";
    case PLASMA_TRANSITION_TYPE_BRIDGE_MORPH:
        return "bridge_morph";
    case PLASMA_TRANSITION_TYPE_FALLBACK:
        return "fallback_transition";
    case PLASMA_TRANSITION_TYPE_HARD_CUT:
        return "hard_cut";
    case PLASMA_TRANSITION_TYPE_REJECTED:
        return "rejected";
    case PLASMA_TRANSITION_TYPE_NONE:
    default:
        return "none";
    }
}

const char *plasma_transition_fallback_policy_name(plasma_transition_fallback_policy policy)
{
    switch (policy) {
    case PLASMA_TRANSITION_FALLBACK_THEME_MORPH:
        return "theme_morph";
    case PLASMA_TRANSITION_FALLBACK_REJECT:
        return "reject";
    case PLASMA_TRANSITION_FALLBACK_HARD_CUT:
    default:
        return "hard_cut";
    }
}

const char *plasma_transition_seed_policy_name(plasma_transition_seed_continuity_policy policy)
{
    switch (policy) {
    case PLASMA_TRANSITION_SEED_CONTINUITY_RESEED_TARGET:
        return "reseed_target";
    case PLASMA_TRANSITION_SEED_CONTINUITY_KEEP_STREAM:
    default:
        return "keep_stream";
    }
}

int plasma_transition_parse_policy(const char *text, plasma_transition_policy *policy_out)
{
    if (text == NULL || policy_out == NULL) {
        return 0;
    }
    if (lstrcmpiA(text, "disabled") == 0) {
        *policy_out = PLASMA_TRANSITION_POLICY_DISABLED;
        return 1;
    }
    if (lstrcmpiA(text, "auto") == 0) {
        *policy_out = PLASMA_TRANSITION_POLICY_AUTO;
        return 1;
    }
    if (lstrcmpiA(text, "theme_set") == 0) {
        *policy_out = PLASMA_TRANSITION_POLICY_THEME_SET;
        return 1;
    }
    if (lstrcmpiA(text, "preset_set") == 0) {
        *policy_out = PLASMA_TRANSITION_POLICY_PRESET_SET;
        return 1;
    }
    if (lstrcmpiA(text, "journey") == 0) {
        *policy_out = PLASMA_TRANSITION_POLICY_JOURNEY;
        return 1;
    }

    return 0;
}

int plasma_transition_parse_fallback_policy(
    const char *text,
    plasma_transition_fallback_policy *policy_out
)
{
    if (text == NULL || policy_out == NULL) {
        return 0;
    }
    if (lstrcmpiA(text, "hard_cut") == 0) {
        *policy_out = PLASMA_TRANSITION_FALLBACK_HARD_CUT;
        return 1;
    }
    if (lstrcmpiA(text, "theme_morph") == 0) {
        *policy_out = PLASMA_TRANSITION_FALLBACK_THEME_MORPH;
        return 1;
    }
    if (lstrcmpiA(text, "reject") == 0) {
        *policy_out = PLASMA_TRANSITION_FALLBACK_REJECT;
        return 1;
    }

    return 0;
}

int plasma_transition_parse_seed_policy(
    const char *text,
    plasma_transition_seed_continuity_policy *policy_out
)
{
    if (text == NULL || policy_out == NULL) {
        return 0;
    }
    if (lstrcmpiA(text, "keep_stream") == 0) {
        *policy_out = PLASMA_TRANSITION_SEED_CONTINUITY_KEEP_STREAM;
        return 1;
    }
    if (lstrcmpiA(text, "reseed_target") == 0) {
        *policy_out = PLASMA_TRANSITION_SEED_CONTINUITY_RESEED_TARGET;
        return 1;
    }

    return 0;
}

void plasma_transition_preferences_set_defaults(plasma_transition_preferences *preferences)
{
    if (preferences == NULL) {
        return;
    }

    ZeroMemory(preferences, sizeof(*preferences));
    preferences->enabled = 0;
    preferences->policy = PLASMA_TRANSITION_POLICY_DISABLED;
    preferences->fallback_policy = PLASMA_TRANSITION_FALLBACK_THEME_MORPH;
    preferences->seed_policy = PLASMA_TRANSITION_SEED_CONTINUITY_KEEP_STREAM;
    preferences->interval_millis = 12000UL;
    preferences->duration_millis = 2400UL;
}

void plasma_transition_preferences_clamp(plasma_transition_preferences *preferences)
{
    if (preferences == NULL) {
        return;
    }

    preferences->enabled = preferences->enabled ? 1 : 0;
    if (
        preferences->policy < PLASMA_TRANSITION_POLICY_DISABLED ||
        preferences->policy > PLASMA_TRANSITION_POLICY_JOURNEY
    ) {
        preferences->policy = PLASMA_TRANSITION_POLICY_DISABLED;
    }
    if (
        preferences->fallback_policy < PLASMA_TRANSITION_FALLBACK_HARD_CUT ||
        preferences->fallback_policy > PLASMA_TRANSITION_FALLBACK_REJECT
    ) {
        preferences->fallback_policy = PLASMA_TRANSITION_FALLBACK_THEME_MORPH;
    }
    if (
        preferences->seed_policy < PLASMA_TRANSITION_SEED_CONTINUITY_KEEP_STREAM ||
        preferences->seed_policy > PLASMA_TRANSITION_SEED_CONTINUITY_RESEED_TARGET
    ) {
        preferences->seed_policy = PLASMA_TRANSITION_SEED_CONTINUITY_KEEP_STREAM;
    }
    if (preferences->duration_millis < 400UL) {
        preferences->duration_millis = 400UL;
    } else if (preferences->duration_millis > 12000UL) {
        preferences->duration_millis = 12000UL;
    }
    if (preferences->interval_millis < preferences->duration_millis) {
        preferences->interval_millis = preferences->duration_millis + 1000UL;
    } else if (preferences->interval_millis > 60000UL) {
        preferences->interval_millis = 60000UL;
    }
    if (preferences->journey_key[0] != '\0' && plasma_transition_find_journey(preferences->journey_key) == NULL) {
        preferences->journey_key[0] = '\0';
    }
    if (!preferences->enabled) {
        preferences->policy = PLASMA_TRANSITION_POLICY_DISABLED;
        preferences->journey_key[0] = '\0';
    }
}

const plasma_content_journey_entry *plasma_transition_get_journeys(unsigned int *count_out)
{
    if (count_out != NULL) {
        *count_out = (unsigned int)(sizeof(g_transition_journeys) / sizeof(g_transition_journeys[0]));
    }

    return g_transition_journeys;
}

const plasma_content_journey_entry *plasma_transition_find_journey(const char *journey_key)
{
    unsigned int index;

    if (journey_key == NULL || journey_key[0] == '\0') {
        return NULL;
    }

    for (index = 0U; index < (unsigned int)(sizeof(g_transition_journeys) / sizeof(g_transition_journeys[0])); ++index) {
        if (strcmp(g_transition_journeys[index].journey_key, journey_key) == 0) {
            return &g_transition_journeys[index];
        }
    }

    return NULL;
}

void plasma_transition_plan_init(struct plasma_plan_tag *plan)
{
    if (plan == NULL) {
        return;
    }

    plan->transition_requested = 0;
    plan->transition_enabled = 0;
    plan->transition_policy = PLASMA_TRANSITION_POLICY_DISABLED;
    plan->transition_fallback_policy = PLASMA_TRANSITION_FALLBACK_THEME_MORPH;
    plan->transition_seed_policy = PLASMA_TRANSITION_SEED_CONTINUITY_KEEP_STREAM;
    plan->transition_supported_types = 0UL;
    plan->transition_interval_millis = 12000UL;
    plan->transition_duration_millis = 2400UL;
    plan->journey = NULL;
}

void plasma_transition_bind_plan(
    struct plasma_plan_tag *plan,
    const screensave_saver_module *module
)
{
    (void)module;

    if (plan == NULL) {
        return;
    }

    plan->transition_supported_types =
        plasma_transition_supported_types_for_kind(plan->active_renderer_kind);
    plan->transition_enabled =
        plan->transition_requested &&
        plasma_transition_policy_has_surface(plan) &&
        plan->transition_supported_types != 0UL;
}

int plasma_transition_validate_plan(
    const struct plasma_plan_tag *plan,
    const screensave_saver_module *module
)
{
    (void)module;

    if (plan == NULL) {
        return 0;
    }

    if (
        (plan->transition_requested != 0 && plan->transition_requested != 1) ||
        (plan->transition_enabled != 0 && plan->transition_enabled != 1) ||
        plan->transition_policy < PLASMA_TRANSITION_POLICY_DISABLED ||
        plan->transition_policy > PLASMA_TRANSITION_POLICY_JOURNEY ||
        plan->transition_fallback_policy < PLASMA_TRANSITION_FALLBACK_HARD_CUT ||
        plan->transition_fallback_policy > PLASMA_TRANSITION_FALLBACK_REJECT ||
        plan->transition_seed_policy < PLASMA_TRANSITION_SEED_CONTINUITY_KEEP_STREAM ||
        plan->transition_seed_policy > PLASMA_TRANSITION_SEED_CONTINUITY_RESEED_TARGET ||
        plan->transition_duration_millis == 0UL ||
        plan->transition_interval_millis < plan->transition_duration_millis
    ) {
        return 0;
    }

    if (
        plan->transition_requested &&
        plan->transition_policy == PLASMA_TRANSITION_POLICY_JOURNEY &&
        plan->journey == NULL
    ) {
        return 0;
    }

    if (plan->journey != NULL && plan->journey->step_count == 0U) {
        return 0;
    }

    if (plan->transition_enabled) {
        if (plan->transition_supported_types == 0UL || !plasma_transition_policy_has_surface(plan)) {
            return 0;
        }
        if (
            plasma_is_lower_band_kind(plan->active_renderer_kind) &&
            (plan->transition_supported_types & PLASMA_TRANSITION_SUPPORTED_BRIDGE_MORPH) != 0UL
        ) {
            return 0;
        }
    }

    return 1;
}

void plasma_transition_runtime_init(plasma_transition_runtime *runtime)
{
    if (runtime == NULL) {
        return;
    }

    ZeroMemory(runtime, sizeof(*runtime));
    runtime->active_type = PLASMA_TRANSITION_TYPE_NONE;
    runtime->requested_type = PLASMA_TRANSITION_TYPE_NONE;
    runtime->fallback_type = PLASMA_TRANSITION_TYPE_NONE;
}

void plasma_transition_runtime_bind(
    plasma_transition_runtime *runtime,
    const struct plasma_plan_tag *plan
)
{
    if (runtime == NULL) {
        return;
    }

    plasma_transition_runtime_init(runtime);
    if (plan == NULL) {
        return;
    }

    runtime->supported_types = plan->transition_supported_types;
    runtime->journey = plan->journey;
    runtime->source_preset = plan->selection.selected_preset;
    runtime->source_theme = plan->selection.selected_theme;
}

unsigned long plasma_transition_step(
    struct plasma_plan_tag *plan,
    struct plasma_execution_state_tag *state,
    const screensave_saver_module *module,
    const screensave_saver_environment *environment
)
{
    plasma_transition_runtime *runtime;
    unsigned long delta_millis;
    unsigned long interval_millis;
    plasma_transition_policy active_policy;

    if (plan == NULL || state == NULL || environment == NULL) {
        return 0UL;
    }

    runtime = &state->transition;
    if (!plan->transition_enabled) {
        runtime->active = 0;
        runtime->active_type = PLASMA_TRANSITION_TYPE_NONE;
        runtime->idle_elapsed_millis = 0UL;
        return 0UL;
    }

    delta_millis = environment->clock.delta_millis;
    active_policy = plan->transition_policy;
    interval_millis = plan->transition_interval_millis;
    (void)plasma_transition_active_preset_set(plan, runtime, &active_policy, &interval_millis);
    (void)plasma_transition_active_theme_set(plan, runtime, &active_policy, &interval_millis);

    if (runtime->active) {
        runtime->elapsed_millis += delta_millis;
        if (
            runtime->active_type == PLASMA_TRANSITION_TYPE_BRIDGE_MORPH &&
            !runtime->bridge_switched &&
            runtime->elapsed_millis * 2UL >= runtime->duration_millis
        ) {
            runtime->bridge_switched = 1;
            return plasma_transition_commit_target(plan, state, module);
        }

        if (runtime->elapsed_millis >= runtime->duration_millis) {
            unsigned long commit_flags;

            commit_flags = 0UL;
            if (!runtime->bridge_switched) {
                commit_flags = plasma_transition_commit_target(plan, state, module);
            }
            runtime->active = 0;
            runtime->elapsed_millis = 0UL;
            runtime->idle_elapsed_millis = 0UL;
            runtime->active_type = PLASMA_TRANSITION_TYPE_NONE;
            runtime->source_preset = plan->selection.selected_preset;
            runtime->source_theme = plan->selection.selected_theme;
            runtime->target_preset = NULL;
            runtime->target_theme = NULL;
            return commit_flags;
        }

        return 0UL;
    }

    runtime->idle_elapsed_millis += delta_millis;
    if (runtime->idle_elapsed_millis < interval_millis) {
        return 0UL;
    }

    return plasma_transition_start_next(plan, state, module);
}

void plasma_transition_resolve_theme_colors(
    const struct plasma_plan_tag *plan,
    const struct plasma_execution_state_tag *state,
    screensave_color *primary_out,
    screensave_color *accent_out
)
{
    const plasma_transition_runtime *runtime;
    unsigned int amount;

    if (primary_out == NULL || accent_out == NULL) {
        return;
    }

    if (plan == NULL || plan->theme == NULL) {
        ZeroMemory(primary_out, sizeof(*primary_out));
        ZeroMemory(accent_out, sizeof(*accent_out));
        return;
    }

    *primary_out = plan->theme->primary_color;
    *accent_out = plan->theme->accent_color;
    if (state == NULL) {
        return;
    }

    runtime = &state->transition;
    if (
        !runtime->active ||
        runtime->source_theme == NULL ||
        runtime->target_theme == NULL
    ) {
        return;
    }

    amount = plasma_transition_progress_amount(state);
    *primary_out = screensave_color_lerp(
        runtime->source_theme->descriptor->primary_color,
        runtime->target_theme->descriptor->primary_color,
        amount
    );
    *accent_out = screensave_color_lerp(
        runtime->source_theme->descriptor->accent_color,
        runtime->target_theme->descriptor->accent_color,
        amount
    );
}

unsigned long plasma_transition_effective_speed_units(
    const struct plasma_plan_tag *plan,
    const struct plasma_execution_state_tag *state,
    unsigned long base_speed_units
)
{
    const plasma_transition_runtime *runtime;
    const plasma_preset_values *source_values;
    const plasma_preset_values *target_values;
    unsigned long source_speed;
    unsigned long target_speed;
    unsigned int amount;

    if (plan == NULL || state == NULL) {
        return base_speed_units;
    }

    runtime = &state->transition;
    if (
        !runtime->active ||
        (runtime->active_type != PLASMA_TRANSITION_TYPE_PRESET_MORPH &&
            runtime->active_type != PLASMA_TRANSITION_TYPE_BRIDGE_MORPH) ||
        runtime->source_preset == NULL ||
        runtime->target_preset == NULL
    ) {
        return base_speed_units;
    }

    source_values = plasma_find_preset_values(runtime->source_preset->preset_key);
    target_values = plasma_find_preset_values(runtime->target_preset->preset_key);
    if (source_values == NULL || target_values == NULL) {
        return base_speed_units;
    }

    amount = plasma_transition_progress_amount(state);
    source_speed = plasma_transition_speed_units_for_mode(source_values->speed_mode);
    target_speed = plasma_transition_speed_units_for_mode(target_values->speed_mode);
    return ((source_speed * (255UL - amount)) + (target_speed * amount)) / 255UL;
}

int plasma_transition_resolve_smoothing(
    const struct plasma_plan_tag *plan,
    const struct plasma_execution_state_tag *state,
    int *enabled_out,
    unsigned int *blend_amount_out
)
{
    const plasma_transition_runtime *runtime;
    const plasma_preset_values *source_values;
    const plasma_preset_values *target_values;
    unsigned int amount;
    unsigned int source_blend;
    unsigned int target_blend;

    if (enabled_out == NULL || blend_amount_out == NULL || plan == NULL) {
        return 0;
    }

    *blend_amount_out = plasma_transition_smoothing_amount_for_mode(plan->smoothing_mode);
    *enabled_out = *blend_amount_out > 0U;
    if (state == NULL) {
        return 1;
    }

    runtime = &state->transition;
    if (
        !runtime->active ||
        (runtime->active_type != PLASMA_TRANSITION_TYPE_PRESET_MORPH &&
            runtime->active_type != PLASMA_TRANSITION_TYPE_BRIDGE_MORPH) ||
        runtime->source_preset == NULL ||
        runtime->target_preset == NULL
    ) {
        return 1;
    }

    source_values = plasma_find_preset_values(runtime->source_preset->preset_key);
    target_values = plasma_find_preset_values(runtime->target_preset->preset_key);
    if (source_values == NULL || target_values == NULL) {
        return 1;
    }

    amount = plasma_transition_progress_amount(state);
    source_blend = plasma_transition_smoothing_amount_for_mode(source_values->smoothing_mode);
    target_blend = plasma_transition_smoothing_amount_for_mode(target_values->smoothing_mode);
    *blend_amount_out =
        (unsigned int)(((unsigned long)source_blend * (255UL - amount) +
            (unsigned long)target_blend * amount) / 255UL);
    *enabled_out = *blend_amount_out > 0U;
    return 1;
}

unsigned int plasma_transition_progress_amount(
    const struct plasma_execution_state_tag *state
)
{
    if (
        state == NULL ||
        !state->transition.active ||
        state->transition.duration_millis == 0UL
    ) {
        return 0U;
    }

    return plasma_transition_clamp_amount(
        state->transition.elapsed_millis,
        state->transition.duration_millis
    );
}
