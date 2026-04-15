#include <string.h>

#include "plasma_internal.h"

static char plasma_benchlab_upper_ascii(char value)
{
    if (value >= 'a' && value <= 'z') {
        return (char)(value - ('a' - 'A'));
    }

    return value;
}

static void plasma_benchlab_emit_diag(
    screensave_diag_context *diagnostics,
    screensave_diag_level level,
    unsigned long code,
    const char *text
)
{
    if (diagnostics == NULL || text == NULL) {
        return;
    }

    screensave_diag_emit(
        diagnostics,
        level,
        SCREENSAVE_DIAG_DOMAIN_SAVER,
        code,
        "plasma_benchlab",
        text
    );
}

static int plasma_benchlab_append_text(
    char *buffer,
    unsigned int buffer_size,
    const char *text
)
{
    unsigned int used;
    unsigned int needed;

    if (buffer == NULL || buffer_size == 0U || text == NULL) {
        return 0;
    }

    used = (unsigned int)lstrlenA(buffer);
    needed = (unsigned int)lstrlenA(text);
    if (used + needed + 1U > buffer_size) {
        return 0;
    }

    lstrcpyA(buffer + used, text);
    return 1;
}

static int plasma_benchlab_append_label_text(
    char *buffer,
    unsigned int buffer_size,
    const char *label,
    const char *value
)
{
    return plasma_benchlab_append_text(buffer, buffer_size, label) &&
        plasma_benchlab_append_text(buffer, buffer_size, value != NULL ? value : "") &&
        plasma_benchlab_append_text(buffer, buffer_size, "\r\n");
}

static int plasma_benchlab_append_label_bool(
    char *buffer,
    unsigned int buffer_size,
    const char *label,
    int value
)
{
    return plasma_benchlab_append_label_text(buffer, buffer_size, label, value ? "yes" : "no");
}

static int plasma_benchlab_append_label_hex(
    char *buffer,
    unsigned int buffer_size,
    const char *label,
    unsigned long value
)
{
    char text[32];

    wsprintfA(text, "0x%08lX", value);
    return plasma_benchlab_append_label_text(buffer, buffer_size, label, text);
}

static int plasma_benchlab_append_flag_name(
    char *buffer,
    unsigned int buffer_size,
    const char *name
)
{
    if (buffer[0] != '\0') {
        if (!plasma_benchlab_append_text(buffer, buffer_size, ", ")) {
            return 0;
        }
    }

    return plasma_benchlab_append_text(buffer, buffer_size, name);
}

static int plasma_benchlab_parse_unsigned_long(const char *text, unsigned long *value_out)
{
    unsigned long value;

    if (text == NULL || text[0] == '\0' || value_out == NULL) {
        return 0;
    }

    value = 0UL;
    while (*text != '\0') {
        if (*text < '0' || *text > '9') {
            return 0;
        }

        value = (value * 10UL) + (unsigned long)(*text - '0');
        ++text;
    }

    *value_out = value;
    return 1;
}

static int plasma_benchlab_parse_bool(const char *text, int *value_out)
{
    if (text == NULL || value_out == NULL) {
        return 0;
    }

    if (
        lstrcmpiA(text, "1") == 0 ||
        lstrcmpiA(text, "true") == 0 ||
        lstrcmpiA(text, "yes") == 0 ||
        lstrcmpiA(text, "on") == 0
    ) {
        *value_out = 1;
        return 1;
    }
    if (
        lstrcmpiA(text, "0") == 0 ||
        lstrcmpiA(text, "false") == 0 ||
        lstrcmpiA(text, "no") == 0 ||
        lstrcmpiA(text, "off") == 0
    ) {
        *value_out = 0;
        return 1;
    }

    return 0;
}

static int plasma_benchlab_command_starts_with(
    const char *token,
    const char *prefix
)
{
    const char *cursor;

    if (token == NULL || prefix == NULL) {
        return 0;
    }

    cursor = token;
    if (cursor[0] == '/' || cursor[0] == '-') {
        ++cursor;
    }

    while (*prefix != '\0') {
        if (*cursor == '\0') {
            return 0;
        }
        if (
            plasma_benchlab_upper_ascii(*cursor) !=
            plasma_benchlab_upper_ascii(*prefix)
        ) {
            return 0;
        }
        ++cursor;
        ++prefix;
    }

    return 1;
}

static const char *plasma_benchlab_command_payload(
    const char *token,
    const char *name_a,
    const char *name_b
)
{
    const char *cursor;
    unsigned int name_length;

    if (token == NULL) {
        return NULL;
    }

    cursor = token;
    if (cursor[0] == '/' || cursor[0] == '-') {
        ++cursor;
    }

    if (name_a != NULL && plasma_benchlab_command_starts_with(token, name_a)) {
        name_length = (unsigned int)lstrlenA(name_a);
    } else if (name_b != NULL && plasma_benchlab_command_starts_with(token, name_b)) {
        name_length = (unsigned int)lstrlenA(name_b);
    } else {
        return NULL;
    }

    cursor += name_length;
    if (*cursor == ':') {
        ++cursor;
    }

    return cursor;
}

static const char *plasma_benchlab_lane_name_for_kind(
    screensave_renderer_kind renderer_kind
)
{
    switch (renderer_kind) {
    case SCREENSAVE_RENDERER_KIND_GDI:
        return "universal";

    case SCREENSAVE_RENDERER_KIND_GL11:
        return "compat";

    case SCREENSAVE_RENDERER_KIND_GL21:
        return "advanced";

    case SCREENSAVE_RENDERER_KIND_GL33:
        return "modern";

    case SCREENSAVE_RENDERER_KIND_GL46:
        return "premium";

    case SCREENSAVE_RENDERER_KIND_UNKNOWN:
    default:
        return "auto";
    }
}

static const char *plasma_benchlab_resolved_lane(const plasma_plan *plan)
{
    if (plan == NULL) {
        return "unknown";
    }
    if (plan->premium_enabled) {
        return "premium";
    }
    if (plan->modern_enabled) {
        return "modern";
    }
    if (plan->advanced_enabled) {
        return "advanced";
    }
    if (plan->active_renderer_kind == SCREENSAVE_RENDERER_KIND_GL11) {
        return "compat";
    }
    if (plan->active_renderer_kind == SCREENSAVE_RENDERER_KIND_GDI) {
        return "universal";
    }

    return "unknown";
}

static const char *plasma_benchlab_requested_lane(
    const plasma_config *config,
    const plasma_plan *plan,
    screensave_renderer_kind requested_renderer_kind
)
{
    if (requested_renderer_kind != SCREENSAVE_RENDERER_KIND_UNKNOWN) {
        return plasma_benchlab_lane_name_for_kind(requested_renderer_kind);
    }

    if (
        config != NULL &&
        config->benchlab.active &&
        config->benchlab.presentation_request != PLASMA_BENCHLAB_PRESENTATION_AUTO
    ) {
        return "premium";
    }
    if (plan != NULL && plan->premium_requested) {
        return "premium";
    }

    return "auto";
}

static const char *plasma_benchlab_degraded_from_lane(
    const plasma_plan *plan,
    const plasma_config *config,
    screensave_renderer_kind requested_renderer_kind
)
{
    const char *resolved_lane;

    if (plan == NULL) {
        return "none";
    }

    resolved_lane = plasma_benchlab_resolved_lane(plan);
    if (
        config != NULL &&
        config->benchlab.active &&
        config->benchlab.presentation_request != PLASMA_BENCHLAB_PRESENTATION_AUTO &&
        !plan->premium_enabled
    ) {
        return "premium";
    }
    if (plan->premium_degraded) {
        return "premium";
    }

    if (
        requested_renderer_kind == SCREENSAVE_RENDERER_KIND_GL46 &&
        strcmp(resolved_lane, "premium") != 0
    ) {
        return "premium";
    }
    if (
        requested_renderer_kind == SCREENSAVE_RENDERER_KIND_GL33 &&
        strcmp(resolved_lane, "modern") != 0
    ) {
        return "modern";
    }
    if (
        requested_renderer_kind == SCREENSAVE_RENDERER_KIND_GL21 &&
        strcmp(resolved_lane, "advanced") != 0
    ) {
        return "advanced";
    }
    if (
        requested_renderer_kind == SCREENSAVE_RENDERER_KIND_GL11 &&
        strcmp(resolved_lane, "compat") != 0
    ) {
        return "compat";
    }
    if (
        requested_renderer_kind == SCREENSAVE_RENDERER_KIND_GDI &&
        strcmp(resolved_lane, "universal") != 0
    ) {
        return "universal";
    }

    return "none";
}

static const char *plasma_benchlab_degraded_to_lane(
    const char *degraded_from_lane,
    const plasma_plan *plan
)
{
    if (
        degraded_from_lane == NULL ||
        strcmp(degraded_from_lane, "none") == 0 ||
        plan == NULL
    ) {
        return "none";
    }

    return plasma_benchlab_resolved_lane(plan);
}

static const char *plasma_benchlab_value_or_none(const char *text)
{
    if (text == NULL || text[0] == '\0') {
        return "none";
    }

    return text;
}

static const char *plasma_benchlab_generator_family_name_for_effect_mode(int effect_mode)
{
    switch (effect_mode) {
    case PLASMA_EFFECT_PLASMA:
        return "plasma";

    case PLASMA_EFFECT_INTERFERENCE:
        return "interference";

    case PLASMA_EFFECT_CHEMICAL:
        return "chemical_cellular_growth";

    case PLASMA_EFFECT_LATTICE:
        return "lattice_quasi_crystal";

    case PLASMA_EFFECT_CAUSTIC:
        return "caustic_marbling";

    case PLASMA_EFFECT_AURORA:
        return "aurora_curtain_ribbon";

    case PLASMA_EFFECT_SUBSTRATE:
        return "substrate_vein_coral";

    case PLASMA_EFFECT_ARC:
        return "arc_discharge";

    case PLASMA_EFFECT_FIRE:
    default:
        return "fire";
    }
}

static const char *plasma_benchlab_generator_family_name(const plasma_plan *plan)
{
    if (plan == NULL) {
        return "unknown";
    }

    return plasma_benchlab_generator_family_name_for_effect_mode(plan->effect_mode);
}

static const char *plasma_benchlab_content_source_name(
    plasma_content_source source
)
{
    switch (source) {
    case PLASMA_CONTENT_SOURCE_PACK:
        return "pack";

    case PLASMA_CONTENT_SOURCE_PORTABLE:
        return "portable";

    case PLASMA_CONTENT_SOURCE_USER:
        return "user";

    case PLASMA_CONTENT_SOURCE_BUILT_IN:
    default:
        return "built_in";
    }
}

static const char *plasma_benchlab_content_channel_name(
    plasma_content_channel channel
)
{
    switch (channel) {
    case PLASMA_CONTENT_CHANNEL_EXPERIMENTAL:
        return "experimental";

    case PLASMA_CONTENT_CHANNEL_STABLE:
    default:
        return "stable";
    }
}

static const char *plasma_benchlab_output_family_name(
    plasma_output_family family
)
{
    return plasma_output_family_token(family);
}

static const char *plasma_benchlab_output_mode_name(
    plasma_output_mode mode
)
{
    return plasma_output_mode_token(mode);
}

static const char *plasma_benchlab_sampling_treatment_name(
    plasma_sampling_treatment treatment
)
{
    return plasma_sampling_treatment_token(treatment);
}

static const char *plasma_benchlab_filter_treatment_name(
    plasma_filter_treatment treatment
)
{
    return plasma_filter_treatment_token(treatment);
}

static const char *plasma_benchlab_emulation_treatment_name(
    plasma_emulation_treatment treatment
)
{
    return plasma_emulation_treatment_token(treatment);
}

static const char *plasma_benchlab_accent_treatment_name(
    plasma_accent_treatment treatment
)
{
    return plasma_accent_treatment_token(treatment);
}

static const char *plasma_benchlab_presentation_mode_name(
    plasma_presentation_mode mode
)
{
    return plasma_presentation_mode_token(mode);
}

static const char *plasma_benchlab_preset_morph_class_name(
    plasma_preset_morph_class morph_class
)
{
    switch (morph_class) {
    case PLASMA_PRESET_MORPH_CLASS_FIRE:
        return "fire";

    case PLASMA_PRESET_MORPH_CLASS_PLASMA:
        return "plasma";

    case PLASMA_PRESET_MORPH_CLASS_INTERFERENCE:
        return "interference";

    case PLASMA_PRESET_MORPH_CLASS_NONE:
    default:
        return "none";
    }
}

static const char *plasma_benchlab_theme_morph_class_name(
    plasma_theme_morph_class morph_class
)
{
    switch (morph_class) {
    case PLASMA_THEME_MORPH_CLASS_WARM:
        return "warm";

    case PLASMA_THEME_MORPH_CLASS_COOL:
        return "cool";

    case PLASMA_THEME_MORPH_CLASS_PHOSPHOR:
        return "phosphor";

    case PLASMA_THEME_MORPH_CLASS_NONE:
    default:
        return "none";
    }
}

static const char *plasma_benchlab_bridge_class_name(
    plasma_transition_bridge_class bridge_class
)
{
    switch (bridge_class) {
    case PLASMA_TRANSITION_BRIDGE_CLASS_WARM_CLASSIC:
        return "warm_classic";

    case PLASMA_TRANSITION_BRIDGE_CLASS_COOL_FIELD:
        return "cool_field";

    case PLASMA_TRANSITION_BRIDGE_CLASS_NONE:
    default:
        return "none";
    }
}

static const char *plasma_benchlab_seed_policy_name(
    plasma_plan_seed_policy seed_policy
)
{
    switch (seed_policy) {
    case PLASMA_PLAN_SEED_POLICY_FIXED:
        return "fixed";

    case PLASMA_PLAN_SEED_POLICY_INHERIT:
    default:
        return "inherit";
    }
}

static const plasma_config *plasma_benchlab_get_config(
    const screensave_saver_config_state *resolved_config
)
{
    if (
        resolved_config == NULL ||
        resolved_config->product_config == NULL ||
        resolved_config->product_config_size != sizeof(plasma_config)
    ) {
        return NULL;
    }

    return (const plasma_config *)resolved_config->product_config;
}

static const plasma_content_preset_entry *plasma_benchlab_selected_preset_entry(
    const screensave_saver_session *session
)
{
    if (session == NULL) {
        return NULL;
    }

    return session->plan.selection.selected_preset;
}

static const plasma_content_theme_entry *plasma_benchlab_selected_theme_entry(
    const screensave_saver_session *session
)
{
    if (session == NULL) {
        return NULL;
    }

    return session->plan.selection.selected_theme;
}

static const char *plasma_benchlab_transition_type_text(
    const plasma_transition_runtime *runtime
)
{
    if (runtime == NULL) {
        return "none";
    }
    if (runtime->active_type != PLASMA_TRANSITION_TYPE_NONE) {
        return plasma_transition_type_name(runtime->active_type);
    }
    if (runtime->requested_type != PLASMA_TRANSITION_TYPE_NONE) {
        return plasma_transition_type_name(runtime->requested_type);
    }
    if (runtime->fallback_type != PLASMA_TRANSITION_TYPE_NONE) {
        return plasma_transition_type_name(runtime->fallback_type);
    }

    return "none";
}

static const char *plasma_benchlab_requested_transition_type_text(
    const plasma_transition_runtime *runtime
)
{
    if (runtime == NULL || runtime->requested_type == PLASMA_TRANSITION_TYPE_NONE) {
        return "none";
    }

    return plasma_transition_type_name(runtime->requested_type);
}

static const char *plasma_benchlab_resolved_transition_type_text(
    const plasma_transition_runtime *runtime
)
{
    if (runtime == NULL || runtime->active_type == PLASMA_TRANSITION_TYPE_NONE) {
        return "none";
    }

    return plasma_transition_type_name(runtime->active_type);
}

static const char *plasma_benchlab_fallback_transition_type_text(
    const plasma_transition_runtime *runtime
)
{
    if (runtime == NULL || runtime->fallback_type == PLASMA_TRANSITION_TYPE_NONE) {
        return "none";
    }

    return plasma_transition_type_name(runtime->fallback_type);
}

static void plasma_benchlab_forcing_copy_key(
    char *buffer,
    unsigned int buffer_size,
    const char *value
)
{
    if (buffer == NULL || buffer_size == 0U) {
        return;
    }

    buffer[0] = '\0';
    if (value == NULL || value[0] == '\0') {
        return;
    }

    lstrcpynA(buffer, value, (int)buffer_size);
}

static void plasma_benchlab_forcing_mark_invalid(
    plasma_benchlab_forcing *forcing,
    unsigned long clamp_flag
)
{
    if (forcing == NULL) {
        return;
    }

    forcing->active = 1;
    forcing->clamp_flags |= clamp_flag;
}

static int plasma_benchlab_parse_content_filter_override(
    const char *text,
    int *value_out
)
{
    plasma_content_filter filter;

    if (text == NULL || value_out == NULL) {
        return 0;
    }
    if (!plasma_selection_parse_content_filter(text, &filter)) {
        return 0;
    }

    *value_out = (int)filter;
    return 1;
}

static int plasma_benchlab_parse_presentation_request(
    const char *text,
    plasma_benchlab_presentation_request *request_out
)
{
    if (text == NULL || request_out == NULL) {
        return 0;
    }

    if (lstrcmpiA(text, "auto") == 0) {
        *request_out = PLASMA_BENCHLAB_PRESENTATION_AUTO;
        return 1;
    }
    if (lstrcmpiA(text, "heightfield") == 0) {
        *request_out = PLASMA_BENCHLAB_PRESENTATION_HEIGHTFIELD;
        return 1;
    }
    if (lstrcmpiA(text, "curtain") == 0) {
        *request_out = PLASMA_BENCHLAB_PRESENTATION_CURTAIN;
        return 1;
    }
    if (lstrcmpiA(text, "ribbon") == 0) {
        *request_out = PLASMA_BENCHLAB_PRESENTATION_RIBBON;
        return 1;
    }
    if (
        lstrcmpiA(text, "contour_extrusion") == 0 ||
        lstrcmpiA(text, "contour-extrusion") == 0
    ) {
        *request_out = PLASMA_BENCHLAB_PRESENTATION_CONTOUR_EXTRUSION;
        return 1;
    }
    if (
        lstrcmpiA(text, "bounded_surface") == 0 ||
        lstrcmpiA(text, "bounded-surface") == 0
    ) {
        *request_out = PLASMA_BENCHLAB_PRESENTATION_BOUNDED_SURFACE;
        return 1;
    }

    return 0;
}

static plasma_presentation_mode plasma_benchlab_requested_presentation_mode(
    plasma_benchlab_presentation_request request
)
{
    switch (request) {
    case PLASMA_BENCHLAB_PRESENTATION_HEIGHTFIELD:
        return PLASMA_PRESENTATION_MODE_HEIGHTFIELD;

    case PLASMA_BENCHLAB_PRESENTATION_CURTAIN:
        return PLASMA_PRESENTATION_MODE_CURTAIN;

    case PLASMA_BENCHLAB_PRESENTATION_RIBBON:
        return PLASMA_PRESENTATION_MODE_RIBBON;

    case PLASMA_BENCHLAB_PRESENTATION_CONTOUR_EXTRUSION:
        return PLASMA_PRESENTATION_MODE_CONTOUR_EXTRUSION;

    case PLASMA_BENCHLAB_PRESENTATION_BOUNDED_SURFACE:
        return PLASMA_PRESENTATION_MODE_BOUNDED_SURFACE;

    case PLASMA_BENCHLAB_PRESENTATION_AUTO:
    default:
        return PLASMA_PRESENTATION_MODE_FLAT;
    }
}

static int plasma_benchlab_plan_supports_requested_presentation(
    const plasma_plan *plan,
    plasma_benchlab_presentation_request request
)
{
    plasma_presentation_mode requested_mode;

    if (plan == NULL || request == PLASMA_BENCHLAB_PRESENTATION_AUTO) {
        return 0;
    }

    requested_mode = plasma_benchlab_requested_presentation_mode(request);
    if (
        plasma_presentation_mode_requires_premium(requested_mode) &&
        !plan->premium_enabled
    ) {
        return 0;
    }
    if (!plasma_presentation_mode_is_supported(requested_mode)) {
        return 0;
    }
    if (!plasma_presentation_mode_supports_output_family(requested_mode, plan->output_family)) {
        return 0;
    }

    return 1;
}

void plasma_benchlab_forcing_set_defaults(plasma_benchlab_forcing *forcing)
{
    if (forcing == NULL) {
        return;
    }

    ZeroMemory(forcing, sizeof(*forcing));
    forcing->content_filter_override = -1;
    forcing->favorites_only_override = -1;
    forcing->transitions_enabled_override = -1;
    forcing->transition_policy_override = -1;
    forcing->transition_fallback_override = -1;
    forcing->transition_seed_policy_override = -1;
    forcing->presentation_request = PLASMA_BENCHLAB_PRESENTATION_AUTO;
}

void plasma_benchlab_forcing_clamp(plasma_benchlab_forcing *forcing)
{
    plasma_transition_preferences transition_defaults;
    int has_explicit_override;

    if (forcing == NULL) {
        return;
    }

    forcing->active = forcing->active ? 1 : 0;

    if (forcing->preset_key[0] != '\0') {
        const char *canonical_key;

        canonical_key = plasma_canonical_content_key(forcing->preset_key);
        if (plasma_content_find_preset_entry(canonical_key) == NULL) {
            forcing->preset_key[0] = '\0';
            forcing->clamp_flags |= PLASMA_BENCHLAB_CLAMP_PRESET_KEY;
        } else if (canonical_key != forcing->preset_key) {
            plasma_benchlab_forcing_copy_key(
                forcing->preset_key,
                (unsigned int)sizeof(forcing->preset_key),
                canonical_key
            );
        }
    }

    if (forcing->theme_key[0] != '\0') {
        const char *canonical_key;

        canonical_key = plasma_canonical_content_key(forcing->theme_key);
        if (plasma_content_find_theme_entry(canonical_key) == NULL) {
            forcing->theme_key[0] = '\0';
            forcing->clamp_flags |= PLASMA_BENCHLAB_CLAMP_THEME_KEY;
        } else if (canonical_key != forcing->theme_key) {
            plasma_benchlab_forcing_copy_key(
                forcing->theme_key,
                (unsigned int)sizeof(forcing->theme_key),
                canonical_key
            );
        }
    }

    if (
        forcing->preset_set_key[0] != '\0' &&
        plasma_content_find_preset_set(forcing->preset_set_key) == NULL
    ) {
        forcing->preset_set_key[0] = '\0';
        forcing->clamp_flags |= PLASMA_BENCHLAB_CLAMP_PRESET_SET_KEY;
    }
    if (
        forcing->theme_set_key[0] != '\0' &&
        plasma_content_find_theme_set(forcing->theme_set_key) == NULL
    ) {
        forcing->theme_set_key[0] = '\0';
        forcing->clamp_flags |= PLASMA_BENCHLAB_CLAMP_THEME_SET_KEY;
    }
    if (
        forcing->journey_key[0] != '\0' &&
        plasma_transition_find_journey(forcing->journey_key) == NULL
    ) {
        forcing->journey_key[0] = '\0';
        forcing->clamp_flags |= PLASMA_BENCHLAB_CLAMP_JOURNEY_KEY;
    }

    if (
        forcing->content_filter_override < -1 ||
        forcing->content_filter_override > (int)PLASMA_CONTENT_FILTER_EXPERIMENTAL_ONLY
    ) {
        forcing->content_filter_override = -1;
        forcing->clamp_flags |= PLASMA_BENCHLAB_CLAMP_CONTENT_FILTER;
    }
    if (
        forcing->favorites_only_override < -1 ||
        forcing->favorites_only_override > 1
    ) {
        forcing->favorites_only_override = -1;
        forcing->clamp_flags |= PLASMA_BENCHLAB_CLAMP_FAVORITES_ONLY;
    }
    if (
        forcing->transitions_enabled_override < -1 ||
        forcing->transitions_enabled_override > 1
    ) {
        forcing->transitions_enabled_override = -1;
        forcing->clamp_flags |= PLASMA_BENCHLAB_CLAMP_TRANSITIONS_ENABLED;
    }
    if (
        forcing->transition_policy_override < -1 ||
        forcing->transition_policy_override > (int)PLASMA_TRANSITION_POLICY_JOURNEY
    ) {
        forcing->transition_policy_override = -1;
        forcing->clamp_flags |= PLASMA_BENCHLAB_CLAMP_TRANSITION_POLICY;
    }
    if (
        forcing->transition_fallback_override < -1 ||
        forcing->transition_fallback_override > (int)PLASMA_TRANSITION_FALLBACK_REJECT
    ) {
        forcing->transition_fallback_override = -1;
        forcing->clamp_flags |= PLASMA_BENCHLAB_CLAMP_TRANSITION_FALLBACK;
    }
    if (
        forcing->transition_seed_policy_override < -1 ||
        forcing->transition_seed_policy_override > (int)PLASMA_TRANSITION_SEED_CONTINUITY_RESEED_TARGET
    ) {
        forcing->transition_seed_policy_override = -1;
        forcing->clamp_flags |= PLASMA_BENCHLAB_CLAMP_TRANSITION_SEED;
    }

    plasma_transition_preferences_set_defaults(&transition_defaults);
    if (forcing->transition_duration_override_enabled) {
        if (forcing->transition_duration_millis < 400UL) {
            forcing->transition_duration_millis = 400UL;
            forcing->clamp_flags |= PLASMA_BENCHLAB_CLAMP_TRANSITION_DURATION;
        } else if (forcing->transition_duration_millis > 12000UL) {
            forcing->transition_duration_millis = 12000UL;
            forcing->clamp_flags |= PLASMA_BENCHLAB_CLAMP_TRANSITION_DURATION;
        }
        transition_defaults.duration_millis = forcing->transition_duration_millis;
    }
    if (forcing->transition_interval_override_enabled) {
        if (forcing->transition_interval_millis > 60000UL) {
            forcing->transition_interval_millis = 60000UL;
            forcing->clamp_flags |= PLASMA_BENCHLAB_CLAMP_TRANSITION_INTERVAL;
        }
        if (forcing->transition_interval_millis < transition_defaults.duration_millis + 1000UL) {
            forcing->transition_interval_millis = transition_defaults.duration_millis + 1000UL;
            forcing->clamp_flags |= PLASMA_BENCHLAB_CLAMP_TRANSITION_INTERVAL;
        }
    }

    if (
        forcing->presentation_request < PLASMA_BENCHLAB_PRESENTATION_AUTO ||
        forcing->presentation_request > PLASMA_BENCHLAB_PRESENTATION_BOUNDED_SURFACE
    ) {
        forcing->presentation_request = PLASMA_BENCHLAB_PRESENTATION_AUTO;
        forcing->clamp_flags |= PLASMA_BENCHLAB_CLAMP_PRESENTATION;
    }

    has_explicit_override =
        forcing->preset_key[0] != '\0' ||
        forcing->theme_key[0] != '\0' ||
        forcing->preset_set_key[0] != '\0' ||
        forcing->theme_set_key[0] != '\0' ||
        forcing->journey_key[0] != '\0' ||
        forcing->content_filter_override >= 0 ||
        forcing->favorites_only_override >= 0 ||
        forcing->transitions_enabled_override >= 0 ||
        forcing->transition_policy_override >= 0 ||
        forcing->transition_fallback_override >= 0 ||
        forcing->transition_seed_policy_override >= 0 ||
        forcing->transition_interval_override_enabled ||
        forcing->transition_duration_override_enabled ||
        forcing->presentation_request != PLASMA_BENCHLAB_PRESENTATION_AUTO ||
        forcing->clamp_flags != 0UL;
    forcing->active = has_explicit_override ? 1 : 0;
}

int plasma_benchlab_parse_command_line(
    const char *command_line,
    plasma_benchlab_forcing *forcing,
    screensave_diag_context *diagnostics
)
{
    char token[128];
    const char *cursor;

    if (forcing == NULL) {
        return 0;
    }

    plasma_benchlab_forcing_set_defaults(forcing);
    if (command_line == NULL) {
        return 1;
    }

    cursor = command_line;
    while (*cursor != '\0') {
        int length;

        while (*cursor == ' ' || *cursor == '\t') {
            ++cursor;
        }
        if (*cursor == '\0') {
            break;
        }

        length = 0;
        while (*cursor != '\0' && *cursor != ' ' && *cursor != '\t' && length < (int)sizeof(token) - 1) {
            token[length] = *cursor;
            ++length;
            ++cursor;
        }
        token[length] = '\0';

        while (*cursor != '\0' && *cursor != ' ' && *cursor != '\t') {
            ++cursor;
        }

        if (
            plasma_benchlab_command_starts_with(token, "plasma-preset:") ||
            plasma_benchlab_command_starts_with(token, "plasma_preset:")
        ) {
            const char *payload;

            payload = plasma_benchlab_command_payload(token, "plasma-preset", "plasma_preset");
            if (payload != NULL && payload[0] != '\0') {
                plasma_benchlab_forcing_copy_key(
                    forcing->preset_key,
                    (unsigned int)sizeof(forcing->preset_key),
                    payload
                );
                forcing->active = 1;
            } else {
                plasma_benchlab_forcing_mark_invalid(forcing, PLASMA_BENCHLAB_CLAMP_PRESET_KEY);
                plasma_benchlab_emit_diag(
                    diagnostics,
                    SCREENSAVE_DIAG_LEVEL_WARNING,
                    6801UL,
                    "Plasma BenchLab ignored an empty preset override."
                );
            }
            continue;
        }

        if (
            plasma_benchlab_command_starts_with(token, "plasma-theme:") ||
            plasma_benchlab_command_starts_with(token, "plasma_theme:")
        ) {
            const char *payload;

            payload = plasma_benchlab_command_payload(token, "plasma-theme", "plasma_theme");
            if (payload != NULL && payload[0] != '\0') {
                plasma_benchlab_forcing_copy_key(
                    forcing->theme_key,
                    (unsigned int)sizeof(forcing->theme_key),
                    payload
                );
                forcing->active = 1;
            } else {
                plasma_benchlab_forcing_mark_invalid(forcing, PLASMA_BENCHLAB_CLAMP_THEME_KEY);
                plasma_benchlab_emit_diag(
                    diagnostics,
                    SCREENSAVE_DIAG_LEVEL_WARNING,
                    6802UL,
                    "Plasma BenchLab ignored an empty theme override."
                );
            }
            continue;
        }

        if (
            plasma_benchlab_command_starts_with(token, "plasma-preset-set:") ||
            plasma_benchlab_command_starts_with(token, "plasma_preset_set:")
        ) {
            const char *payload;

            payload = plasma_benchlab_command_payload(token, "plasma-preset-set", "plasma_preset_set");
            if (payload != NULL && payload[0] != '\0') {
                plasma_benchlab_forcing_copy_key(
                    forcing->preset_set_key,
                    (unsigned int)sizeof(forcing->preset_set_key),
                    payload
                );
                forcing->active = 1;
            } else {
                plasma_benchlab_forcing_mark_invalid(forcing, PLASMA_BENCHLAB_CLAMP_PRESET_SET_KEY);
                plasma_benchlab_emit_diag(
                    diagnostics,
                    SCREENSAVE_DIAG_LEVEL_WARNING,
                    6803UL,
                    "Plasma BenchLab ignored an empty preset-set override."
                );
            }
            continue;
        }

        if (
            plasma_benchlab_command_starts_with(token, "plasma-theme-set:") ||
            plasma_benchlab_command_starts_with(token, "plasma_theme_set:")
        ) {
            const char *payload;

            payload = plasma_benchlab_command_payload(token, "plasma-theme-set", "plasma_theme_set");
            if (payload != NULL && payload[0] != '\0') {
                plasma_benchlab_forcing_copy_key(
                    forcing->theme_set_key,
                    (unsigned int)sizeof(forcing->theme_set_key),
                    payload
                );
                forcing->active = 1;
            } else {
                plasma_benchlab_forcing_mark_invalid(forcing, PLASMA_BENCHLAB_CLAMP_THEME_SET_KEY);
                plasma_benchlab_emit_diag(
                    diagnostics,
                    SCREENSAVE_DIAG_LEVEL_WARNING,
                    6804UL,
                    "Plasma BenchLab ignored an empty theme-set override."
                );
            }
            continue;
        }

        if (
            plasma_benchlab_command_starts_with(token, "plasma-journey:") ||
            plasma_benchlab_command_starts_with(token, "plasma_journey:")
        ) {
            const char *payload;

            payload = plasma_benchlab_command_payload(token, "plasma-journey", "plasma_journey");
            if (payload != NULL && payload[0] != '\0') {
                plasma_benchlab_forcing_copy_key(
                    forcing->journey_key,
                    (unsigned int)sizeof(forcing->journey_key),
                    payload
                );
                forcing->active = 1;
            } else {
                plasma_benchlab_forcing_mark_invalid(forcing, PLASMA_BENCHLAB_CLAMP_JOURNEY_KEY);
                plasma_benchlab_emit_diag(
                    diagnostics,
                    SCREENSAVE_DIAG_LEVEL_WARNING,
                    6805UL,
                    "Plasma BenchLab ignored an empty journey override."
                );
            }
            continue;
        }

        if (
            plasma_benchlab_command_starts_with(token, "plasma-content-filter:") ||
            plasma_benchlab_command_starts_with(token, "plasma_content_filter:")
        ) {
            const char *payload;

            payload = plasma_benchlab_command_payload(token, "plasma-content-filter", "plasma_content_filter");
            if (
                payload != NULL &&
                plasma_benchlab_parse_content_filter_override(payload, &forcing->content_filter_override)
            ) {
                forcing->active = 1;
            } else {
                plasma_benchlab_forcing_mark_invalid(forcing, PLASMA_BENCHLAB_CLAMP_CONTENT_FILTER);
                plasma_benchlab_emit_diag(
                    diagnostics,
                    SCREENSAVE_DIAG_LEVEL_WARNING,
                    6806UL,
                    "Plasma BenchLab ignored an invalid content-filter override."
                );
            }
            continue;
        }

        if (
            plasma_benchlab_command_starts_with(token, "plasma-favorites-only:") ||
            plasma_benchlab_command_starts_with(token, "plasma_favorites_only:")
        ) {
            const char *payload;

            payload = plasma_benchlab_command_payload(token, "plasma-favorites-only", "plasma_favorites_only");
            if (payload != NULL && plasma_benchlab_parse_bool(payload, &forcing->favorites_only_override)) {
                forcing->active = 1;
            } else {
                plasma_benchlab_forcing_mark_invalid(forcing, PLASMA_BENCHLAB_CLAMP_FAVORITES_ONLY);
                plasma_benchlab_emit_diag(
                    diagnostics,
                    SCREENSAVE_DIAG_LEVEL_WARNING,
                    6807UL,
                    "Plasma BenchLab ignored an invalid favorites-only override."
                );
            }
            continue;
        }

        if (
            plasma_benchlab_command_starts_with(token, "plasma-transitions:") ||
            plasma_benchlab_command_starts_with(token, "plasma_transitions:")
        ) {
            const char *payload;

            payload = plasma_benchlab_command_payload(token, "plasma-transitions", "plasma_transitions");
            if (payload != NULL && plasma_benchlab_parse_bool(payload, &forcing->transitions_enabled_override)) {
                forcing->active = 1;
            } else {
                plasma_benchlab_forcing_mark_invalid(forcing, PLASMA_BENCHLAB_CLAMP_TRANSITIONS_ENABLED);
                plasma_benchlab_emit_diag(
                    diagnostics,
                    SCREENSAVE_DIAG_LEVEL_WARNING,
                    6808UL,
                    "Plasma BenchLab ignored an invalid transitions override."
                );
            }
            continue;
        }

        if (
            plasma_benchlab_command_starts_with(token, "plasma-transition-policy:") ||
            plasma_benchlab_command_starts_with(token, "plasma_transition_policy:")
        ) {
            const char *payload;

            payload = plasma_benchlab_command_payload(token, "plasma-transition-policy", "plasma_transition_policy");
            if (payload != NULL && plasma_transition_parse_policy(payload, (plasma_transition_policy *)&forcing->transition_policy_override)) {
                forcing->active = 1;
            } else {
                plasma_benchlab_forcing_mark_invalid(forcing, PLASMA_BENCHLAB_CLAMP_TRANSITION_POLICY);
                plasma_benchlab_emit_diag(
                    diagnostics,
                    SCREENSAVE_DIAG_LEVEL_WARNING,
                    6809UL,
                    "Plasma BenchLab ignored an invalid transition-policy override."
                );
            }
            continue;
        }

        if (
            plasma_benchlab_command_starts_with(token, "plasma-transition-fallback:") ||
            plasma_benchlab_command_starts_with(token, "plasma_transition_fallback:")
        ) {
            const char *payload;

            payload = plasma_benchlab_command_payload(token, "plasma-transition-fallback", "plasma_transition_fallback");
            if (
                payload != NULL &&
                plasma_transition_parse_fallback_policy(
                    payload,
                    (plasma_transition_fallback_policy *)&forcing->transition_fallback_override
                )
            ) {
                forcing->active = 1;
            } else {
                plasma_benchlab_forcing_mark_invalid(forcing, PLASMA_BENCHLAB_CLAMP_TRANSITION_FALLBACK);
                plasma_benchlab_emit_diag(
                    diagnostics,
                    SCREENSAVE_DIAG_LEVEL_WARNING,
                    6810UL,
                    "Plasma BenchLab ignored an invalid transition-fallback override."
                );
            }
            continue;
        }

        if (
            plasma_benchlab_command_starts_with(token, "plasma-transition-seed:") ||
            plasma_benchlab_command_starts_with(token, "plasma_transition_seed:")
        ) {
            const char *payload;

            payload = plasma_benchlab_command_payload(token, "plasma-transition-seed", "plasma_transition_seed");
            if (
                payload != NULL &&
                plasma_transition_parse_seed_policy(
                    payload,
                    (plasma_transition_seed_continuity_policy *)&forcing->transition_seed_policy_override
                )
            ) {
                forcing->active = 1;
            } else {
                plasma_benchlab_forcing_mark_invalid(forcing, PLASMA_BENCHLAB_CLAMP_TRANSITION_SEED);
                plasma_benchlab_emit_diag(
                    diagnostics,
                    SCREENSAVE_DIAG_LEVEL_WARNING,
                    6811UL,
                    "Plasma BenchLab ignored an invalid transition-seed override."
                );
            }
            continue;
        }

        if (
            plasma_benchlab_command_starts_with(token, "plasma-transition-interval:") ||
            plasma_benchlab_command_starts_with(token, "plasma_transition_interval:")
        ) {
            const char *payload;

            payload = plasma_benchlab_command_payload(token, "plasma-transition-interval", "plasma_transition_interval");
            if (payload != NULL && plasma_benchlab_parse_unsigned_long(payload, &forcing->transition_interval_millis)) {
                forcing->transition_interval_override_enabled = 1;
                forcing->active = 1;
            } else {
                plasma_benchlab_forcing_mark_invalid(forcing, PLASMA_BENCHLAB_CLAMP_TRANSITION_INTERVAL);
                plasma_benchlab_emit_diag(
                    diagnostics,
                    SCREENSAVE_DIAG_LEVEL_WARNING,
                    6812UL,
                    "Plasma BenchLab ignored an invalid transition-interval override."
                );
            }
            continue;
        }

        if (
            plasma_benchlab_command_starts_with(token, "plasma-transition-duration:") ||
            plasma_benchlab_command_starts_with(token, "plasma_transition_duration:")
        ) {
            const char *payload;

            payload = plasma_benchlab_command_payload(token, "plasma-transition-duration", "plasma_transition_duration");
            if (payload != NULL && plasma_benchlab_parse_unsigned_long(payload, &forcing->transition_duration_millis)) {
                forcing->transition_duration_override_enabled = 1;
                forcing->active = 1;
            } else {
                plasma_benchlab_forcing_mark_invalid(forcing, PLASMA_BENCHLAB_CLAMP_TRANSITION_DURATION);
                plasma_benchlab_emit_diag(
                    diagnostics,
                    SCREENSAVE_DIAG_LEVEL_WARNING,
                    6813UL,
                    "Plasma BenchLab ignored an invalid transition-duration override."
                );
            }
            continue;
        }

        if (
            plasma_benchlab_command_starts_with(token, "plasma-presentation:") ||
            plasma_benchlab_command_starts_with(token, "plasma_presentation:")
        ) {
            const char *payload;

            payload = plasma_benchlab_command_payload(token, "plasma-presentation", "plasma_presentation");
            if (
                payload != NULL &&
                plasma_benchlab_parse_presentation_request(payload, &forcing->presentation_request)
            ) {
                forcing->active = 1;
            } else {
                plasma_benchlab_forcing_mark_invalid(forcing, PLASMA_BENCHLAB_CLAMP_PRESENTATION);
                plasma_benchlab_emit_diag(
                    diagnostics,
                    SCREENSAVE_DIAG_LEVEL_WARNING,
                    6814UL,
                    "Plasma BenchLab ignored an invalid presentation override."
                );
            }
            continue;
        }
    }

    plasma_benchlab_forcing_clamp(forcing);
    return 1;
}

void plasma_benchlab_apply_forcing_to_config(
    const plasma_benchlab_forcing *forcing,
    screensave_common_config *common_config,
    plasma_config *product_config
)
{
    if (
        forcing == NULL ||
        common_config == NULL ||
        product_config == NULL ||
        !forcing->active
    ) {
        return;
    }

    if (forcing->preset_key[0] != '\0') {
        common_config->preset_key = forcing->preset_key;
    }
    if (forcing->theme_key[0] != '\0') {
        common_config->theme_key = forcing->theme_key;
    }
    if (forcing->preset_set_key[0] != '\0') {
        lstrcpynA(
            product_config->selection.preset_set_key,
            forcing->preset_set_key,
            (int)sizeof(product_config->selection.preset_set_key)
        );
    }
    if (forcing->theme_set_key[0] != '\0') {
        lstrcpynA(
            product_config->selection.theme_set_key,
            forcing->theme_set_key,
            (int)sizeof(product_config->selection.theme_set_key)
        );
    }
    if (forcing->journey_key[0] != '\0') {
        lstrcpynA(
            product_config->transition.journey_key,
            forcing->journey_key,
            (int)sizeof(product_config->transition.journey_key)
        );
    }
    if (forcing->content_filter_override >= 0) {
        product_config->selection.content_filter =
            (plasma_content_filter)forcing->content_filter_override;
    }
    if (forcing->favorites_only_override >= 0) {
        product_config->selection.favorites_only = forcing->favorites_only_override ? 1 : 0;
    }
    if (forcing->transitions_enabled_override >= 0) {
        product_config->transition.enabled = forcing->transitions_enabled_override ? 1 : 0;
    }
    if (forcing->transition_policy_override >= 0) {
        product_config->transition.policy =
            (plasma_transition_policy)forcing->transition_policy_override;
    }
    if (forcing->transition_fallback_override >= 0) {
        product_config->transition.fallback_policy =
            (plasma_transition_fallback_policy)forcing->transition_fallback_override;
    }
    if (forcing->transition_seed_policy_override >= 0) {
        product_config->transition.seed_policy =
            (plasma_transition_seed_continuity_policy)forcing->transition_seed_policy_override;
    }
    if (forcing->transition_interval_override_enabled) {
        product_config->transition.interval_millis = forcing->transition_interval_millis;
    }
    if (forcing->transition_duration_override_enabled) {
        product_config->transition.duration_millis = forcing->transition_duration_millis;
    }
}

void plasma_benchlab_apply_plan_forcing(
    plasma_plan *plan,
    const screensave_saver_module *module,
    const plasma_benchlab_forcing *forcing
)
{
    plasma_presentation_mode requested_mode;

    (void)module;

    if (plan == NULL || forcing == NULL || !forcing->active) {
        return;
    }

    if (forcing->presentation_request != PLASMA_BENCHLAB_PRESENTATION_AUTO) {
        requested_mode = plasma_benchlab_requested_presentation_mode(
            forcing->presentation_request
        );
        plan->requested_presentation_mode = requested_mode;
        if (plasma_benchlab_plan_supports_requested_presentation(plan, forcing->presentation_request)) {
            plan->presentation_mode = requested_mode;
        }
    }
    plasma_presentation_bind_plan(plan);
}

static unsigned long plasma_benchlab_build_clamp_flags(
    const plasma_config *config,
    const plasma_plan *plan
)
{
    const plasma_benchlab_forcing *forcing;
    unsigned long flags;
    const char *resolved_text;
    const char *requested_text;

    if (config == NULL) {
        return 0UL;
    }

    forcing = &config->benchlab;
    flags = forcing->clamp_flags;
    if (!forcing->active || plan == NULL) {
        return flags;
    }

    if (forcing->preset_key[0] != '\0') {
        requested_text = plasma_canonical_content_key(forcing->preset_key);
        if (requested_text == NULL) {
            requested_text = forcing->preset_key;
        }
        if (plan->preset_key == NULL || strcmp(requested_text, plan->preset_key) != 0) {
            flags |= PLASMA_BENCHLAB_CLAMP_PRESET_KEY;
        }
    }
    if (forcing->theme_key[0] != '\0') {
        requested_text = plasma_canonical_content_key(forcing->theme_key);
        if (requested_text == NULL) {
            requested_text = forcing->theme_key;
        }
        if (plan->theme_key == NULL || strcmp(requested_text, plan->theme_key) != 0) {
            flags |= PLASMA_BENCHLAB_CLAMP_THEME_KEY;
        }
    }
    if (forcing->preset_set_key[0] != '\0') {
        resolved_text = plan->selection.active_preset_set != NULL
            ? plan->selection.active_preset_set->set_key
            : "";
        if (strcmp(forcing->preset_set_key, resolved_text) != 0) {
            flags |= PLASMA_BENCHLAB_CLAMP_PRESET_SET_KEY;
        }
    }
    if (forcing->theme_set_key[0] != '\0') {
        resolved_text = plan->selection.active_theme_set != NULL
            ? plan->selection.active_theme_set->set_key
            : "";
        if (strcmp(forcing->theme_set_key, resolved_text) != 0) {
            flags |= PLASMA_BENCHLAB_CLAMP_THEME_SET_KEY;
        }
    }
    if (
        forcing->content_filter_override >= 0 &&
        (int)plan->selection.content_filter != forcing->content_filter_override
    ) {
        flags |= PLASMA_BENCHLAB_CLAMP_CONTENT_FILTER;
    }
    if (
        forcing->favorites_only_override >= 0 &&
        plan->selection.favorites_only_applied != forcing->favorites_only_override
    ) {
        flags |= PLASMA_BENCHLAB_CLAMP_FAVORITES_ONLY;
    }
    if (
        forcing->transitions_enabled_override >= 0 &&
        plan->transition_enabled != forcing->transitions_enabled_override
    ) {
        flags |= PLASMA_BENCHLAB_CLAMP_TRANSITIONS_ENABLED;
    }
    if (
        forcing->transition_policy_override >= 0 &&
        (int)plan->transition_policy != forcing->transition_policy_override
    ) {
        flags |= PLASMA_BENCHLAB_CLAMP_TRANSITION_POLICY;
    }
    if (
        forcing->transition_fallback_override >= 0 &&
        (int)plan->transition_fallback_policy != forcing->transition_fallback_override
    ) {
        flags |= PLASMA_BENCHLAB_CLAMP_TRANSITION_FALLBACK;
    }
    if (
        forcing->transition_seed_policy_override >= 0 &&
        (int)plan->transition_seed_policy != forcing->transition_seed_policy_override
    ) {
        flags |= PLASMA_BENCHLAB_CLAMP_TRANSITION_SEED;
    }
    if (
        forcing->transition_interval_override_enabled &&
        plan->transition_interval_millis != forcing->transition_interval_millis
    ) {
        flags |= PLASMA_BENCHLAB_CLAMP_TRANSITION_INTERVAL;
    }
    if (
        forcing->transition_duration_override_enabled &&
        plan->transition_duration_millis != forcing->transition_duration_millis
    ) {
        flags |= PLASMA_BENCHLAB_CLAMP_TRANSITION_DURATION;
    }
    if (forcing->journey_key[0] != '\0') {
        resolved_text = plan->journey != NULL ? plan->journey->journey_key : "";
        if (strcmp(forcing->journey_key, resolved_text) != 0) {
            flags |= PLASMA_BENCHLAB_CLAMP_JOURNEY_KEY;
        }
    }
    if (
        forcing->presentation_request != PLASMA_BENCHLAB_PRESENTATION_AUTO &&
        plan->presentation_mode != plasma_benchlab_requested_presentation_mode(
            forcing->presentation_request
        )
    ) {
        flags |= PLASMA_BENCHLAB_CLAMP_PRESENTATION;
    }

    return flags;
}

static void plasma_benchlab_build_clamp_summary(
    unsigned long clamp_flags,
    char *buffer,
    unsigned int buffer_size
)
{
    if (buffer == NULL || buffer_size == 0U) {
        return;
    }

    buffer[0] = '\0';
    if (clamp_flags == 0UL) {
        (void)plasma_benchlab_append_text(buffer, buffer_size, "none");
        return;
    }

    if ((clamp_flags & PLASMA_BENCHLAB_CLAMP_PRESET_KEY) != 0UL) {
        (void)plasma_benchlab_append_flag_name(buffer, buffer_size, "preset_key");
    }
    if ((clamp_flags & PLASMA_BENCHLAB_CLAMP_THEME_KEY) != 0UL) {
        (void)plasma_benchlab_append_flag_name(buffer, buffer_size, "theme_key");
    }
    if ((clamp_flags & PLASMA_BENCHLAB_CLAMP_PRESET_SET_KEY) != 0UL) {
        (void)plasma_benchlab_append_flag_name(buffer, buffer_size, "preset_set_key");
    }
    if ((clamp_flags & PLASMA_BENCHLAB_CLAMP_THEME_SET_KEY) != 0UL) {
        (void)plasma_benchlab_append_flag_name(buffer, buffer_size, "theme_set_key");
    }
    if ((clamp_flags & PLASMA_BENCHLAB_CLAMP_CONTENT_FILTER) != 0UL) {
        (void)plasma_benchlab_append_flag_name(buffer, buffer_size, "content_filter");
    }
    if ((clamp_flags & PLASMA_BENCHLAB_CLAMP_FAVORITES_ONLY) != 0UL) {
        (void)plasma_benchlab_append_flag_name(buffer, buffer_size, "favorites_only");
    }
    if ((clamp_flags & PLASMA_BENCHLAB_CLAMP_TRANSITIONS_ENABLED) != 0UL) {
        (void)plasma_benchlab_append_flag_name(buffer, buffer_size, "transitions_enabled");
    }
    if ((clamp_flags & PLASMA_BENCHLAB_CLAMP_TRANSITION_POLICY) != 0UL) {
        (void)plasma_benchlab_append_flag_name(buffer, buffer_size, "transition_policy");
    }
    if ((clamp_flags & PLASMA_BENCHLAB_CLAMP_TRANSITION_FALLBACK) != 0UL) {
        (void)plasma_benchlab_append_flag_name(buffer, buffer_size, "transition_fallback");
    }
    if ((clamp_flags & PLASMA_BENCHLAB_CLAMP_TRANSITION_SEED) != 0UL) {
        (void)plasma_benchlab_append_flag_name(buffer, buffer_size, "transition_seed");
    }
    if ((clamp_flags & PLASMA_BENCHLAB_CLAMP_TRANSITION_INTERVAL) != 0UL) {
        (void)plasma_benchlab_append_flag_name(buffer, buffer_size, "transition_interval");
    }
    if ((clamp_flags & PLASMA_BENCHLAB_CLAMP_TRANSITION_DURATION) != 0UL) {
        (void)plasma_benchlab_append_flag_name(buffer, buffer_size, "transition_duration");
    }
    if ((clamp_flags & PLASMA_BENCHLAB_CLAMP_JOURNEY_KEY) != 0UL) {
        (void)plasma_benchlab_append_flag_name(buffer, buffer_size, "journey_key");
    }
    if ((clamp_flags & PLASMA_BENCHLAB_CLAMP_PRESENTATION) != 0UL) {
        (void)plasma_benchlab_append_flag_name(buffer, buffer_size, "presentation");
    }
}

int plasma_benchlab_build_snapshot(
    const screensave_saver_session *session,
    const screensave_saver_config_state *resolved_config,
    screensave_renderer_kind requested_renderer_kind,
    plasma_benchlab_snapshot *snapshot_out
)
{
    const plasma_config *config;
    const plasma_content_preset_entry *preset_entry;
    const plasma_content_theme_entry *theme_entry;
    const plasma_transition_runtime *runtime;

    if (session == NULL || snapshot_out == NULL) {
        return 0;
    }

    ZeroMemory(snapshot_out, sizeof(*snapshot_out));
    config = plasma_benchlab_get_config(resolved_config);
    preset_entry = plasma_benchlab_selected_preset_entry(session);
    theme_entry = plasma_benchlab_selected_theme_entry(session);
    runtime = &session->state.transition;

    snapshot_out->requested_lane = plasma_benchlab_requested_lane(
        config,
        &session->plan,
        requested_renderer_kind
    );
    snapshot_out->resolved_lane = plasma_benchlab_resolved_lane(&session->plan);
    snapshot_out->degraded_from_lane = plasma_benchlab_degraded_from_lane(
        &session->plan,
        config,
        requested_renderer_kind
    );
    snapshot_out->degraded_to_lane = plasma_benchlab_degraded_to_lane(
        snapshot_out->degraded_from_lane,
        &session->plan
    );
    snapshot_out->preset_key = plasma_benchlab_value_or_none(session->plan.preset_key);
    snapshot_out->theme_key = plasma_benchlab_value_or_none(session->plan.theme_key);
    snapshot_out->preset_set_key = plasma_benchlab_value_or_none(
        session->plan.selection.active_preset_set != NULL
            ? session->plan.selection.active_preset_set->set_key
            : NULL
    );
    snapshot_out->theme_set_key = plasma_benchlab_value_or_none(
        session->plan.selection.active_theme_set != NULL
            ? session->plan.selection.active_theme_set->set_key
            : NULL
    );
    snapshot_out->journey_key = plasma_benchlab_value_or_none(
        runtime->journey != NULL
            ? runtime->journey->journey_key
            : (session->plan.journey != NULL ? session->plan.journey->journey_key : NULL)
    );
    snapshot_out->profile_class =
        ((preset_entry != NULL && preset_entry->channel == PLASMA_CONTENT_CHANNEL_EXPERIMENTAL) ||
            (theme_entry != NULL && theme_entry->channel == PLASMA_CONTENT_CHANNEL_EXPERIMENTAL))
            ? "experimental"
            : "stable";
    snapshot_out->quality_class = screensave_capability_quality_name(session->plan.quality_class);
    snapshot_out->preset_source = preset_entry != NULL
        ? plasma_benchlab_content_source_name(preset_entry->source)
        : "unknown";
    snapshot_out->preset_channel = preset_entry != NULL
        ? plasma_benchlab_content_channel_name(preset_entry->channel)
        : "unknown";
    snapshot_out->theme_source = theme_entry != NULL
        ? plasma_benchlab_content_source_name(theme_entry->source)
        : "unknown";
    snapshot_out->theme_channel = theme_entry != NULL
        ? plasma_benchlab_content_channel_name(theme_entry->channel)
        : "unknown";
    snapshot_out->requested_detail_level =
        screensave_detail_level_name(session->plan.requested_detail_level);
    snapshot_out->detail_level =
        screensave_detail_level_name(session->plan.detail_level);
    snapshot_out->requested_generator_family =
        plasma_benchlab_generator_family_name_for_effect_mode(session->plan.requested_effect_mode);
    snapshot_out->generator_family = plasma_benchlab_generator_family_name(&session->plan);
    snapshot_out->requested_speed_mode =
        plasma_speed_mode_name(session->plan.requested_speed_mode);
    snapshot_out->speed_mode = plasma_speed_mode_name(session->plan.speed_mode);
    snapshot_out->requested_resolution_mode =
        plasma_resolution_mode_name(session->plan.requested_resolution_mode);
    snapshot_out->resolution_mode =
        plasma_resolution_mode_name(session->plan.resolution_mode);
    snapshot_out->requested_smoothing_mode =
        plasma_smoothing_mode_name(session->plan.requested_smoothing_mode);
    snapshot_out->smoothing_mode =
        plasma_smoothing_mode_name(session->plan.smoothing_mode);
    snapshot_out->requested_output_family =
        plasma_benchlab_output_family_name(session->plan.requested_output_family);
    snapshot_out->output_family = plasma_benchlab_output_family_name(session->plan.output_family);
    snapshot_out->requested_output_mode =
        plasma_benchlab_output_mode_name(session->plan.requested_output_mode);
    snapshot_out->output_mode = plasma_benchlab_output_mode_name(session->plan.output_mode);
    snapshot_out->requested_sampling_treatment =
        plasma_benchlab_sampling_treatment_name(session->plan.requested_sampling_treatment);
    snapshot_out->sampling_treatment =
        plasma_benchlab_sampling_treatment_name(session->plan.sampling_treatment);
    snapshot_out->requested_filter_treatment =
        plasma_benchlab_filter_treatment_name(session->plan.requested_filter_treatment);
    snapshot_out->filter_treatment =
        plasma_benchlab_filter_treatment_name(session->plan.filter_treatment);
    snapshot_out->requested_emulation_treatment =
        plasma_benchlab_emulation_treatment_name(session->plan.requested_emulation_treatment);
    snapshot_out->emulation_treatment =
        plasma_benchlab_emulation_treatment_name(session->plan.emulation_treatment);
    snapshot_out->requested_accent_treatment =
        plasma_benchlab_accent_treatment_name(session->plan.requested_accent_treatment);
    snapshot_out->accent_treatment =
        plasma_benchlab_accent_treatment_name(session->plan.accent_treatment);
    snapshot_out->requested_presentation_mode =
        plasma_benchlab_presentation_mode_name(session->plan.requested_presentation_mode);
    snapshot_out->presentation_mode =
        plasma_benchlab_presentation_mode_name(session->plan.presentation_mode);
    snapshot_out->transition_policy =
        plasma_transition_policy_name(session->plan.transition_policy);
    snapshot_out->transition_type = plasma_benchlab_transition_type_text(runtime);
    snapshot_out->transition_requested_type =
        plasma_benchlab_requested_transition_type_text(runtime);
    snapshot_out->transition_resolved_type =
        plasma_benchlab_resolved_transition_type_text(runtime);
    snapshot_out->transition_fallback_type =
        plasma_benchlab_fallback_transition_type_text(runtime);
    snapshot_out->transition_fallback_policy =
        plasma_transition_fallback_policy_name(session->plan.transition_fallback_policy);
    snapshot_out->transition_seed_policy =
        plasma_transition_seed_policy_name(session->plan.transition_seed_policy);
    snapshot_out->transition_source_preset = plasma_benchlab_value_or_none(
        runtime->source_preset != NULL ? runtime->source_preset->preset_key : NULL
    );
    snapshot_out->transition_target_preset = plasma_benchlab_value_or_none(
        runtime->target_preset != NULL ? runtime->target_preset->preset_key : NULL
    );
    snapshot_out->transition_source_theme = plasma_benchlab_value_or_none(
        runtime->source_theme != NULL ? runtime->source_theme->theme_key : NULL
    );
    snapshot_out->transition_target_theme = plasma_benchlab_value_or_none(
        runtime->target_theme != NULL ? runtime->target_theme->theme_key : NULL
    );
    snapshot_out->preset_morph_class = preset_entry != NULL
        ? plasma_benchlab_preset_morph_class_name(preset_entry->morph_class)
        : "none";
    snapshot_out->theme_morph_class = theme_entry != NULL
        ? plasma_benchlab_theme_morph_class_name(theme_entry->morph_class)
        : "none";
    snapshot_out->bridge_class = preset_entry != NULL
        ? plasma_benchlab_bridge_class_name(preset_entry->bridge_class)
        : "none";
    snapshot_out->seed_policy = plasma_benchlab_seed_policy_name(session->plan.seed_policy);
    snapshot_out->settings_surface = config != NULL
        ? plasma_settings_surface_name((plasma_settings_surface)config->settings_surface)
        : "unknown";
    snapshot_out->content_filter =
        plasma_selection_content_filter_name(session->plan.selection.content_filter);
    snapshot_out->configured_seed = session->plan.configured_seed;
    snapshot_out->base_seed = session->plan.base_seed;
    snapshot_out->stream_seed = session->plan.stream_seed;
    snapshot_out->resolved_rng_seed = session->plan.resolved_rng_seed;
    snapshot_out->favorites_only_requested = session->plan.selection.favorites_only_requested;
    snapshot_out->favorites_only_applied = session->plan.selection.favorites_only_applied;
    snapshot_out->transition_requested = session->plan.transition_requested;
    snapshot_out->transition_enabled = session->plan.transition_enabled;
    snapshot_out->forcing_active = config != NULL && config->benchlab.active;
    snapshot_out->clamp_flags = plasma_benchlab_build_clamp_flags(config, &session->plan);
    plasma_benchlab_build_clamp_summary(
        snapshot_out->clamp_flags,
        snapshot_out->clamp_summary,
        (unsigned int)sizeof(snapshot_out->clamp_summary)
    );

    return 1;
}

int plasma_benchlab_build_overlay_summary(
    const screensave_saver_session *session,
    const screensave_saver_config_state *resolved_config,
    screensave_renderer_kind requested_renderer_kind,
    char *buffer,
    unsigned int buffer_size
)
{
    plasma_benchlab_snapshot snapshot;
    char text[128];

    if (buffer == NULL || buffer_size == 0U) {
        return 0;
    }

    buffer[0] = '\0';
    if (
        !plasma_benchlab_build_snapshot(
            session,
            resolved_config,
            requested_renderer_kind,
            &snapshot
        )
    ) {
        return 0;
    }

    if (!plasma_benchlab_append_text(buffer, buffer_size, "Plasma BenchLab\r\n")) {
        return 0;
    }
    if (!plasma_benchlab_append_label_text(buffer, buffer_size, "Requested lane: ", snapshot.requested_lane)) {
        return 0;
    }
    if (!plasma_benchlab_append_label_text(buffer, buffer_size, "Resolved lane: ", snapshot.resolved_lane)) {
        return 0;
    }

    lstrcpyA(text, snapshot.degraded_from_lane);
    if (strcmp(snapshot.degraded_from_lane, "none") == 0) {
        lstrcpyA(text, "none");
    } else {
        lstrcatA(text, " -> ");
        lstrcatA(text, snapshot.degraded_to_lane);
    }
    if (!plasma_benchlab_append_label_text(buffer, buffer_size, "Degrade: ", text)) {
        return 0;
    }
    if (!plasma_benchlab_append_label_text(buffer, buffer_size, "Preset: ", snapshot.preset_key)) {
        return 0;
    }
    if (!plasma_benchlab_append_label_text(buffer, buffer_size, "Theme: ", snapshot.theme_key)) {
        return 0;
    }
    if (!plasma_benchlab_append_label_text(buffer, buffer_size, "Preset set: ", snapshot.preset_set_key)) {
        return 0;
    }
    if (!plasma_benchlab_append_label_text(buffer, buffer_size, "Theme set: ", snapshot.theme_set_key)) {
        return 0;
    }
    if (!plasma_benchlab_append_label_text(buffer, buffer_size, "Journey: ", snapshot.journey_key)) {
        return 0;
    }

    wsprintfA(text, "%s / %s", snapshot.profile_class, snapshot.quality_class);
    if (!plasma_benchlab_append_label_text(buffer, buffer_size, "Profile: ", text)) {
        return 0;
    }
    wsprintfA(text, "%s / %s", snapshot.output_family, snapshot.output_mode);
    if (!plasma_benchlab_append_label_text(buffer, buffer_size, "Output: ", text)) {
        return 0;
    }
    wsprintfA(
        text,
        "%s | %s | %s | %s",
        snapshot.sampling_treatment,
        snapshot.filter_treatment,
        snapshot.emulation_treatment,
        snapshot.accent_treatment
    );
    if (!plasma_benchlab_append_label_text(buffer, buffer_size, "Treatments: ", text)) {
        return 0;
    }
    if (!plasma_benchlab_append_label_text(buffer, buffer_size, "Presentation: ", snapshot.presentation_mode)) {
        return 0;
    }
    wsprintfA(
        text,
        "%s / %s",
        snapshot.transition_enabled ? "enabled" : "disabled",
        snapshot.transition_type
    );
    if (!plasma_benchlab_append_label_text(buffer, buffer_size, "Transition: ", text)) {
        return 0;
    }
    if (!plasma_benchlab_append_label_text(buffer, buffer_size, "Settings surface: ", snapshot.settings_surface)) {
        return 0;
    }
    if (!plasma_benchlab_append_label_text(buffer, buffer_size, "Content filter: ", snapshot.content_filter)) {
        return 0;
    }
    if (!plasma_benchlab_append_label_bool(buffer, buffer_size, "Forcing active: ", snapshot.forcing_active)) {
        return 0;
    }
    if (!plasma_benchlab_append_label_text(buffer, buffer_size, "Clamps: ", snapshot.clamp_summary)) {
        return 0;
    }

    return 1;
}

int plasma_benchlab_build_report_section(
    const screensave_saver_session *session,
    const screensave_saver_config_state *resolved_config,
    screensave_renderer_kind requested_renderer_kind,
    char *buffer,
    unsigned int buffer_size
)
{
    plasma_benchlab_snapshot snapshot;
    char text[160];

    if (buffer == NULL || buffer_size == 0U) {
        return 0;
    }

    buffer[0] = '\0';
    if (
        !plasma_benchlab_build_snapshot(
            session,
            resolved_config,
            requested_renderer_kind,
            &snapshot
        )
    ) {
        return 0;
    }

    if (!plasma_benchlab_append_text(buffer, buffer_size, "\r\n\r\nPlasma BenchLab\r\n----------------\r\n")) {
        return 0;
    }
    if (!plasma_benchlab_append_label_text(buffer, buffer_size, "Requested lane: ", snapshot.requested_lane)) {
        return 0;
    }
    if (!plasma_benchlab_append_label_text(buffer, buffer_size, "Resolved lane: ", snapshot.resolved_lane)) {
        return 0;
    }
    if (!plasma_benchlab_append_label_text(buffer, buffer_size, "Degraded from: ", snapshot.degraded_from_lane)) {
        return 0;
    }
    if (!plasma_benchlab_append_label_text(buffer, buffer_size, "Degraded to: ", snapshot.degraded_to_lane)) {
        return 0;
    }
    if (!plasma_benchlab_append_label_text(buffer, buffer_size, "Preset key: ", snapshot.preset_key)) {
        return 0;
    }
    if (!plasma_benchlab_append_label_text(buffer, buffer_size, "Theme key: ", snapshot.theme_key)) {
        return 0;
    }
    if (!plasma_benchlab_append_label_text(buffer, buffer_size, "Preset set key: ", snapshot.preset_set_key)) {
        return 0;
    }
    if (!plasma_benchlab_append_label_text(buffer, buffer_size, "Theme set key: ", snapshot.theme_set_key)) {
        return 0;
    }
    if (!plasma_benchlab_append_label_text(buffer, buffer_size, "Journey key: ", snapshot.journey_key)) {
        return 0;
    }
    if (!plasma_benchlab_append_label_text(buffer, buffer_size, "Profile class: ", snapshot.profile_class)) {
        return 0;
    }
    if (!plasma_benchlab_append_label_text(buffer, buffer_size, "Quality class: ", snapshot.quality_class)) {
        return 0;
    }
    if (!plasma_benchlab_append_label_text(buffer, buffer_size, "Preset source: ", snapshot.preset_source)) {
        return 0;
    }
    if (!plasma_benchlab_append_label_text(buffer, buffer_size, "Preset channel: ", snapshot.preset_channel)) {
        return 0;
    }
    if (!plasma_benchlab_append_label_text(buffer, buffer_size, "Theme source: ", snapshot.theme_source)) {
        return 0;
    }
    if (!plasma_benchlab_append_label_text(buffer, buffer_size, "Theme channel: ", snapshot.theme_channel)) {
        return 0;
    }
    if (!plasma_benchlab_append_label_text(buffer, buffer_size, "Requested detail level: ", snapshot.requested_detail_level)) {
        return 0;
    }
    if (!plasma_benchlab_append_label_text(buffer, buffer_size, "Detail level: ", snapshot.detail_level)) {
        return 0;
    }
    if (!plasma_benchlab_append_label_text(buffer, buffer_size, "Requested generator family: ", snapshot.requested_generator_family)) {
        return 0;
    }
    if (!plasma_benchlab_append_label_text(buffer, buffer_size, "Generator family: ", snapshot.generator_family)) {
        return 0;
    }
    if (!plasma_benchlab_append_label_text(buffer, buffer_size, "Requested speed mode: ", snapshot.requested_speed_mode)) {
        return 0;
    }
    if (!plasma_benchlab_append_label_text(buffer, buffer_size, "Speed mode: ", snapshot.speed_mode)) {
        return 0;
    }
    if (!plasma_benchlab_append_label_text(buffer, buffer_size, "Requested resolution mode: ", snapshot.requested_resolution_mode)) {
        return 0;
    }
    if (!plasma_benchlab_append_label_text(buffer, buffer_size, "Resolution mode: ", snapshot.resolution_mode)) {
        return 0;
    }
    if (!plasma_benchlab_append_label_text(buffer, buffer_size, "Requested smoothing mode: ", snapshot.requested_smoothing_mode)) {
        return 0;
    }
    if (!plasma_benchlab_append_label_text(buffer, buffer_size, "Smoothing mode: ", snapshot.smoothing_mode)) {
        return 0;
    }
    if (!plasma_benchlab_append_label_text(buffer, buffer_size, "Requested output family: ", snapshot.requested_output_family)) {
        return 0;
    }
    if (!plasma_benchlab_append_label_text(buffer, buffer_size, "Output family: ", snapshot.output_family)) {
        return 0;
    }
    if (!plasma_benchlab_append_label_text(buffer, buffer_size, "Requested output mode: ", snapshot.requested_output_mode)) {
        return 0;
    }
    if (!plasma_benchlab_append_label_text(buffer, buffer_size, "Output mode: ", snapshot.output_mode)) {
        return 0;
    }
    if (!plasma_benchlab_append_label_text(buffer, buffer_size, "Requested sampling treatment: ", snapshot.requested_sampling_treatment)) {
        return 0;
    }
    if (!plasma_benchlab_append_label_text(buffer, buffer_size, "Sampling treatment: ", snapshot.sampling_treatment)) {
        return 0;
    }
    if (!plasma_benchlab_append_label_text(buffer, buffer_size, "Requested filter treatment: ", snapshot.requested_filter_treatment)) {
        return 0;
    }
    if (!plasma_benchlab_append_label_text(buffer, buffer_size, "Filter treatment: ", snapshot.filter_treatment)) {
        return 0;
    }
    if (!plasma_benchlab_append_label_text(buffer, buffer_size, "Requested emulation treatment: ", snapshot.requested_emulation_treatment)) {
        return 0;
    }
    if (!plasma_benchlab_append_label_text(buffer, buffer_size, "Emulation treatment: ", snapshot.emulation_treatment)) {
        return 0;
    }
    if (!plasma_benchlab_append_label_text(buffer, buffer_size, "Requested accent treatment: ", snapshot.requested_accent_treatment)) {
        return 0;
    }
    if (!plasma_benchlab_append_label_text(buffer, buffer_size, "Accent treatment: ", snapshot.accent_treatment)) {
        return 0;
    }
    if (!plasma_benchlab_append_label_text(buffer, buffer_size, "Requested presentation mode: ", snapshot.requested_presentation_mode)) {
        return 0;
    }
    if (!plasma_benchlab_append_label_text(buffer, buffer_size, "Presentation mode: ", snapshot.presentation_mode)) {
        return 0;
    }
    if (!plasma_benchlab_append_label_bool(buffer, buffer_size, "Transition requested: ", snapshot.transition_requested)) {
        return 0;
    }
    if (!plasma_benchlab_append_label_bool(buffer, buffer_size, "Transition enabled: ", snapshot.transition_enabled)) {
        return 0;
    }
    if (!plasma_benchlab_append_label_text(buffer, buffer_size, "Transition policy: ", snapshot.transition_policy)) {
        return 0;
    }
    if (!plasma_benchlab_append_label_text(buffer, buffer_size, "Transition type: ", snapshot.transition_type)) {
        return 0;
    }
    if (!plasma_benchlab_append_label_text(buffer, buffer_size, "Transition requested type: ", snapshot.transition_requested_type)) {
        return 0;
    }
    if (!plasma_benchlab_append_label_text(buffer, buffer_size, "Transition resolved type: ", snapshot.transition_resolved_type)) {
        return 0;
    }
    if (!plasma_benchlab_append_label_text(buffer, buffer_size, "Transition fallback type: ", snapshot.transition_fallback_type)) {
        return 0;
    }
    if (!plasma_benchlab_append_label_text(buffer, buffer_size, "Transition fallback: ", snapshot.transition_fallback_policy)) {
        return 0;
    }
    if (!plasma_benchlab_append_label_text(buffer, buffer_size, "Transition seed policy: ", snapshot.transition_seed_policy)) {
        return 0;
    }
    if (!plasma_benchlab_append_label_text(buffer, buffer_size, "Transition source preset: ", snapshot.transition_source_preset)) {
        return 0;
    }
    if (!plasma_benchlab_append_label_text(buffer, buffer_size, "Transition target preset: ", snapshot.transition_target_preset)) {
        return 0;
    }
    if (!plasma_benchlab_append_label_text(buffer, buffer_size, "Transition source theme: ", snapshot.transition_source_theme)) {
        return 0;
    }
    if (!plasma_benchlab_append_label_text(buffer, buffer_size, "Transition target theme: ", snapshot.transition_target_theme)) {
        return 0;
    }
    if (!plasma_benchlab_append_label_text(buffer, buffer_size, "Preset morph class: ", snapshot.preset_morph_class)) {
        return 0;
    }
    if (!plasma_benchlab_append_label_text(buffer, buffer_size, "Theme morph class: ", snapshot.theme_morph_class)) {
        return 0;
    }
    if (!plasma_benchlab_append_label_text(buffer, buffer_size, "Bridge class: ", snapshot.bridge_class)) {
        return 0;
    }
    if (!plasma_benchlab_append_label_text(buffer, buffer_size, "Seed policy: ", snapshot.seed_policy)) {
        return 0;
    }
    if (!plasma_benchlab_append_label_hex(buffer, buffer_size, "Configured seed: ", snapshot.configured_seed)) {
        return 0;
    }
    if (!plasma_benchlab_append_label_hex(buffer, buffer_size, "Base seed: ", snapshot.base_seed)) {
        return 0;
    }
    if (!plasma_benchlab_append_label_hex(buffer, buffer_size, "Stream seed: ", snapshot.stream_seed)) {
        return 0;
    }
    if (!plasma_benchlab_append_label_hex(buffer, buffer_size, "Resolved RNG seed: ", snapshot.resolved_rng_seed)) {
        return 0;
    }
    if (!plasma_benchlab_append_label_text(buffer, buffer_size, "Settings surface: ", snapshot.settings_surface)) {
        return 0;
    }
    if (!plasma_benchlab_append_label_text(buffer, buffer_size, "Content filter: ", snapshot.content_filter)) {
        return 0;
    }
    if (!plasma_benchlab_append_label_bool(buffer, buffer_size, "Favorites requested: ", snapshot.favorites_only_requested)) {
        return 0;
    }
    if (!plasma_benchlab_append_label_bool(buffer, buffer_size, "Favorites applied: ", snapshot.favorites_only_applied)) {
        return 0;
    }
    if (!plasma_benchlab_append_label_bool(buffer, buffer_size, "BenchLab forcing active: ", snapshot.forcing_active)) {
        return 0;
    }
    if (!plasma_benchlab_append_label_hex(buffer, buffer_size, "Clamp flags: ", snapshot.clamp_flags)) {
        return 0;
    }
    if (!plasma_benchlab_append_label_text(buffer, buffer_size, "Clamp summary: ", snapshot.clamp_summary)) {
        return 0;
    }

    wsprintfA(
        text,
        "%s / %s / %s",
        snapshot.requested_lane,
        snapshot.resolved_lane,
        snapshot.degraded_to_lane
    );
    if (!plasma_benchlab_append_label_text(buffer, buffer_size, "Requested/resolved/degraded: ", text)) {
        return 0;
    }

    return 1;
}
