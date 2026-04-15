#include <string.h>

#include "plasma_internal.h"

static const plasma_settings_descriptor g_plasma_settings_catalog[] = {
    {
        "preset_key",
        "Preset",
        "Selects the active Plasma preset identity.",
        PLASMA_SETTINGS_SURFACE_BASIC,
        "content",
        PLASMA_SETTINGS_VALUE_CONTENT_KEY,
        "plasma_lava",
        "classic preset key",
        PLASMA_SETTINGS_PERSIST_USER,
        PLASMA_SETTINGS_AFFECTS_CONTENT_SELECTION | PLASMA_SETTINGS_AFFECTS_RUNTIME_PLAN,
        1
    },
    {
        "theme_key",
        "Theme",
        "Selects the active Plasma theme palette.",
        PLASMA_SETTINGS_SURFACE_BASIC,
        "content",
        PLASMA_SETTINGS_VALUE_CONTENT_KEY,
        "plasma_lava",
        "classic theme key",
        PLASMA_SETTINGS_PERSIST_USER,
        PLASMA_SETTINGS_AFFECTS_CONTENT_SELECTION | PLASMA_SETTINGS_AFFECTS_RUNTIME_PLAN,
        1
    },
    {
        "speed_mode",
        "Speed",
        "Adjusts the safe motion pace for the current Plasma field.",
        PLASMA_SETTINGS_SURFACE_BASIC,
        "motion",
        PLASMA_SETTINGS_VALUE_ENUM,
        "gentle",
        "gentle|standard|lively",
        PLASMA_SETTINGS_PERSIST_USER,
        PLASMA_SETTINGS_AFFECTS_RUNTIME_PLAN | PLASMA_SETTINGS_AFFECTS_EXECUTION_STATE,
        1
    },
    {
        "detail_level",
        "Visual Intensity",
        "Controls the shared detail posture used for calmer or fuller Plasma motion and density.",
        PLASMA_SETTINGS_SURFACE_BASIC,
        "render",
        PLASMA_SETTINGS_VALUE_ENUM,
        "standard",
        "low|standard|high",
        PLASMA_SETTINGS_PERSIST_USER,
        PLASMA_SETTINGS_AFFECTS_RUNTIME_PLAN,
        1
    },
    {
        "effect_mode",
        "Generator Family",
        "Selects the current Plasma effect family, including the bounded experimental PX31 and PX32 field-family studies.",
        PLASMA_SETTINGS_SURFACE_ADVANCED,
        "generator",
        PLASMA_SETTINGS_VALUE_ENUM,
        "fire",
        "plasma|fire|interference|chemical_cellular_growth|lattice_quasi_crystal|caustic_marbling|aurora_curtain_ribbon|substrate_vein_coral|arc_discharge",
        PLASMA_SETTINGS_PERSIST_USER,
        PLASMA_SETTINGS_AFFECTS_RUNTIME_PLAN | PLASMA_SETTINGS_AFFECTS_EXECUTION_STATE,
        1
    },
    {
        "resolution_mode",
        "Resolution",
        "Controls the internal Plasma field density.",
        PLASMA_SETTINGS_SURFACE_ADVANCED,
        "render",
        PLASMA_SETTINGS_VALUE_ENUM,
        "standard",
        "coarse|standard|fine",
        PLASMA_SETTINGS_PERSIST_USER,
        PLASMA_SETTINGS_AFFECTS_RUNTIME_PLAN | PLASMA_SETTINGS_AFFECTS_EXECUTION_STATE,
        1
    },
    {
        "smoothing_mode",
        "Smoothing",
        "Controls the current bounded smoothing treatment.",
        PLASMA_SETTINGS_SURFACE_ADVANCED,
        "treatment",
        PLASMA_SETTINGS_VALUE_ENUM,
        "soft",
        "off|soft|glow",
        PLASMA_SETTINGS_PERSIST_USER,
        PLASMA_SETTINGS_AFFECTS_RUNTIME_PLAN | PLASMA_SETTINGS_AFFECTS_PRESENTATION,
        1
    },
    {
        "output_family",
        "Output Family",
        "Selects the resolved output family instead of leaving non-raster grammar preset-owned.",
        PLASMA_SETTINGS_SURFACE_ADVANCED,
        "output",
        PLASMA_SETTINGS_VALUE_ENUM,
        "raster",
        "raster|banded|contour|glyph",
        PLASMA_SETTINGS_PERSIST_USER,
        PLASMA_SETTINGS_AFFECTS_RUNTIME_PLAN | PLASMA_SETTINGS_AFFECTS_PRESENTATION,
        1
    },
    {
        "output_mode",
        "Output Mode",
        "Selects the resolved output mode within the active output family.",
        PLASMA_SETTINGS_SURFACE_ADVANCED,
        "output",
        PLASMA_SETTINGS_VALUE_ENUM,
        "native_raster",
        "native_raster|posterized_bands|contour_only|contour_bands|ascii_glyph|matrix_glyph",
        PLASMA_SETTINGS_PERSIST_USER,
        PLASMA_SETTINGS_AFFECTS_RUNTIME_PLAN | PLASMA_SETTINGS_AFFECTS_PRESENTATION,
        1
    },
    {
        "filter_treatment",
        "Filter Treatment",
        "Selects the bounded post-filter treatment slot instead of hiding it behind presets or lane uplift.",
        PLASMA_SETTINGS_SURFACE_ADVANCED,
        "treatment",
        PLASMA_SETTINGS_VALUE_ENUM,
        "none",
        "none|blur|glow_edge|halftone_stipple|emboss_edge",
        PLASMA_SETTINGS_PERSIST_USER,
        PLASMA_SETTINGS_AFFECTS_RUNTIME_PLAN | PLASMA_SETTINGS_AFFECTS_PRESENTATION,
        1
    },
    {
        "emulation_treatment",
        "Emulation Treatment",
        "Selects the bounded display-emulation slot explicitly.",
        PLASMA_SETTINGS_SURFACE_ADVANCED,
        "treatment",
        PLASMA_SETTINGS_VALUE_ENUM,
        "none",
        "none|phosphor|crt",
        PLASMA_SETTINGS_PERSIST_USER,
        PLASMA_SETTINGS_AFFECTS_RUNTIME_PLAN | PLASMA_SETTINGS_AFFECTS_PRESENTATION,
        1
    },
    {
        "accent_treatment",
        "Accent Treatment",
        "Selects the bounded accent slot explicitly.",
        PLASMA_SETTINGS_SURFACE_ADVANCED,
        "treatment",
        PLASMA_SETTINGS_VALUE_ENUM,
        "none",
        "none|overlay_pass|accent_pass",
        PLASMA_SETTINGS_PERSIST_USER,
        PLASMA_SETTINGS_AFFECTS_RUNTIME_PLAN | PLASMA_SETTINGS_AFFECTS_PRESENTATION,
        1
    },
    {
        "presentation_mode",
        "Presentation Mode",
        "Selects the resolved presentation posture instead of letting richer lanes invent one implicitly.",
        PLASMA_SETTINGS_SURFACE_ADVANCED,
        "presentation",
        PLASMA_SETTINGS_VALUE_ENUM,
        "flat",
        "flat|heightfield|curtain|ribbon|contour_extrusion|bounded_surface",
        PLASMA_SETTINGS_PERSIST_USER,
        PLASMA_SETTINGS_AFFECTS_RUNTIME_PLAN | PLASMA_SETTINGS_AFFECTS_PRESENTATION,
        1
    },
    {
        "preset_set_key",
        "Preset Set",
        "Selects the active preset-set surface for curated selection and journeys.",
        PLASMA_SETTINGS_SURFACE_AUTHOR_LAB,
        "selection",
        PLASMA_SETTINGS_VALUE_CONTENT_KEY,
        "",
        "preset set key or empty",
        PLASMA_SETTINGS_PERSIST_USER,
        PLASMA_SETTINGS_AFFECTS_CONTENT_SELECTION | PLASMA_SETTINGS_AFFECTS_TRANSITION,
        1
    },
    {
        "theme_set_key",
        "Theme Set",
        "Selects the active theme-set surface for curated selection and journeys.",
        PLASMA_SETTINGS_SURFACE_AUTHOR_LAB,
        "selection",
        PLASMA_SETTINGS_VALUE_CONTENT_KEY,
        "",
        "theme set key or empty",
        PLASMA_SETTINGS_PERSIST_USER,
        PLASMA_SETTINGS_AFFECTS_CONTENT_SELECTION | PLASMA_SETTINGS_AFFECTS_TRANSITION,
        1
    },
    {
        "transitions_enabled",
        "Allow Transitions",
        "Enables the bounded PL10 transition engine when a valid transition surface exists.",
        PLASMA_SETTINGS_SURFACE_BASIC,
        "transition",
        PLASMA_SETTINGS_VALUE_BOOL,
        "false",
        "false|true",
        PLASMA_SETTINGS_PERSIST_USER,
        PLASMA_SETTINGS_AFFECTS_RUNTIME_PLAN | PLASMA_SETTINGS_AFFECTS_TRANSITION,
        1
    },
    {
        "transition_policy",
        "Transition Policy",
        "Chooses how Plasma advances across sets or journeys when transitions are enabled.",
        PLASMA_SETTINGS_SURFACE_AUTHOR_LAB,
        "transition",
        PLASMA_SETTINGS_VALUE_ENUM,
        "disabled",
        "disabled|auto|theme_set|preset_set|journey",
        PLASMA_SETTINGS_PERSIST_USER,
        PLASMA_SETTINGS_AFFECTS_RUNTIME_PLAN | PLASMA_SETTINGS_AFFECTS_TRANSITION,
        1
    },
    {
        "use_deterministic_seed",
        "Deterministic Mode",
        "Locks Plasma to deterministic seed continuity instead of inheriting a session stream.",
        PLASMA_SETTINGS_SURFACE_ADVANCED,
        "seed",
        PLASMA_SETTINGS_VALUE_BOOL,
        "false",
        "false|true",
        PLASMA_SETTINGS_PERSIST_USER,
        PLASMA_SETTINGS_AFFECTS_RUNTIME_PLAN | PLASMA_SETTINGS_AFFECTS_EXECUTION_STATE,
        1
    },
    {
        "content_filter",
        "Content Pool",
        "Controls the stable-versus-experimental content pool for selection.",
        PLASMA_SETTINGS_SURFACE_BASIC,
        "selection",
        PLASMA_SETTINGS_VALUE_ENUM,
        "stable_only",
        "stable_only|stable_and_experimental|experimental_only",
        PLASMA_SETTINGS_PERSIST_USER,
        PLASMA_SETTINGS_AFFECTS_CONTENT_SELECTION,
        1
    },
    {
        "favorites_only",
        "Favorites Only",
        "Restricts selection to configured favorite preset or theme keys.",
        PLASMA_SETTINGS_SURFACE_AUTHOR_LAB,
        "selection",
        PLASMA_SETTINGS_VALUE_BOOL,
        "false",
        "false|true",
        PLASMA_SETTINGS_PERSIST_USER,
        PLASMA_SETTINGS_AFFECTS_CONTENT_SELECTION,
        1
    },
    {
        "journey_key",
        "Journey",
        "Selects the current bounded transition journey when journey policy is active.",
        PLASMA_SETTINGS_SURFACE_AUTHOR_LAB,
        "transition",
        PLASMA_SETTINGS_VALUE_CONTENT_KEY,
        "",
        "journey key or empty",
        PLASMA_SETTINGS_PERSIST_USER,
        PLASMA_SETTINGS_AFFECTS_RUNTIME_PLAN | PLASMA_SETTINGS_AFFECTS_TRANSITION,
        1
    },
    {
        "transition_fallback_policy",
        "Transition Fallback",
        "Controls how unsupported morph pairs degrade when a requested transition is unavailable.",
        PLASMA_SETTINGS_SURFACE_AUTHOR_LAB,
        "transition",
        PLASMA_SETTINGS_VALUE_ENUM,
        "hard_cut",
        "hard_cut|theme_morph|reject",
        PLASMA_SETTINGS_PERSIST_USER,
        PLASMA_SETTINGS_AFFECTS_RUNTIME_PLAN | PLASMA_SETTINGS_AFFECTS_TRANSITION,
        1
    },
    {
        "transition_seed_policy",
        "Transition Seed Policy",
        "Controls whether transitions keep the running stream seed or reseed the target.",
        PLASMA_SETTINGS_SURFACE_AUTHOR_LAB,
        "transition",
        PLASMA_SETTINGS_VALUE_ENUM,
        "keep_stream",
        "keep_stream|reseed_target",
        PLASMA_SETTINGS_PERSIST_USER,
        PLASMA_SETTINGS_AFFECTS_RUNTIME_PLAN | PLASMA_SETTINGS_AFFECTS_TRANSITION,
        1
    },
    {
        "transition_interval_millis",
        "Transition Interval",
        "Controls dwell time before the next bounded transition attempt.",
        PLASMA_SETTINGS_SURFACE_AUTHOR_LAB,
        "transition",
        PLASMA_SETTINGS_VALUE_UNSIGNED,
        "12000",
        "positive milliseconds",
        PLASMA_SETTINGS_PERSIST_USER,
        PLASMA_SETTINGS_AFFECTS_RUNTIME_PLAN | PLASMA_SETTINGS_AFFECTS_TRANSITION,
        1
    },
    {
        "transition_duration_millis",
        "Transition Duration",
        "Controls the bounded transition timing envelope duration.",
        PLASMA_SETTINGS_SURFACE_AUTHOR_LAB,
        "transition",
        PLASMA_SETTINGS_VALUE_UNSIGNED,
        "1600",
        "positive milliseconds",
        PLASMA_SETTINGS_PERSIST_USER,
        PLASMA_SETTINGS_AFFECTS_RUNTIME_PLAN | PLASMA_SETTINGS_AFFECTS_TRANSITION,
        1
    },
    {
        "deterministic_seed",
        "Fixed Seed Value",
        "Provides the explicit deterministic seed value when deterministic mode is enabled.",
        PLASMA_SETTINGS_SURFACE_AUTHOR_LAB,
        "seed",
        PLASMA_SETTINGS_VALUE_UNSIGNED,
        "0",
        "unsigned long",
        PLASMA_SETTINGS_PERSIST_USER,
        PLASMA_SETTINGS_AFFECTS_RUNTIME_PLAN | PLASMA_SETTINGS_AFFECTS_EXECUTION_STATE,
        1
    },
    {
        "diagnostics_overlay_enabled",
        "Diagnostics Overlay",
        "Shows the bounded diagnostics overlay for authoring and validation sessions.",
        PLASMA_SETTINGS_SURFACE_AUTHOR_LAB,
        "diagnostics",
        PLASMA_SETTINGS_VALUE_BOOL,
        "false",
        "false|true",
        PLASMA_SETTINGS_PERSIST_USER,
        PLASMA_SETTINGS_AFFECTS_DIAGNOSTICS,
        1
    }
};

static int plasma_has_non_empty_text(const char *text)
{
    return text != NULL && text[0] != '\0' && lstrcmpiA(text, "none") != 0;
}

static int plasma_setting_is_transition_driven(const char *setting_key)
{
    if (setting_key == NULL) {
        return 0;
    }

    return
        strcmp(setting_key, "transition_policy") == 0 ||
        strcmp(setting_key, "transition_fallback_policy") == 0 ||
        strcmp(setting_key, "transition_seed_policy") == 0 ||
        strcmp(setting_key, "transition_interval_millis") == 0 ||
        strcmp(setting_key, "transition_duration_millis") == 0;
}

static int plasma_setting_requires_transition_surface(const char *setting_key)
{
    if (setting_key == NULL) {
        return 0;
    }

    return
        strcmp(setting_key, "transitions_enabled") == 0 ||
        plasma_setting_is_transition_driven(setting_key) ||
        strcmp(setting_key, "journey_key") == 0;
}

const char *plasma_settings_surface_name(plasma_settings_surface surface)
{
    switch (surface) {
    case PLASMA_SETTINGS_SURFACE_BASIC:
        return "basic";

    case PLASMA_SETTINGS_SURFACE_ADVANCED:
        return "advanced";

    case PLASMA_SETTINGS_SURFACE_AUTHOR_LAB:
        return "author_lab";

    default:
        return "basic";
    }
}

const char *plasma_settings_surface_display_name(plasma_settings_surface surface)
{
    switch (surface) {
    case PLASMA_SETTINGS_SURFACE_BASIC:
        return "Basic";

    case PLASMA_SETTINGS_SURFACE_ADVANCED:
        return "Advanced";

    case PLASMA_SETTINGS_SURFACE_AUTHOR_LAB:
        return "Author/Lab";

    default:
        return "Basic";
    }
}

int plasma_settings_parse_surface(const char *text, plasma_settings_surface *surface_out)
{
    if (text == NULL || surface_out == NULL) {
        return 0;
    }

    if (lstrcmpiA(text, "basic") == 0) {
        *surface_out = PLASMA_SETTINGS_SURFACE_BASIC;
        return 1;
    }
    if (lstrcmpiA(text, "advanced") == 0) {
        *surface_out = PLASMA_SETTINGS_SURFACE_ADVANCED;
        return 1;
    }
    if (lstrcmpiA(text, "author_lab") == 0 || lstrcmpiA(text, "author-lab") == 0) {
        *surface_out = PLASMA_SETTINGS_SURFACE_AUTHOR_LAB;
        return 1;
    }

    return 0;
}

int plasma_settings_surface_contains_setting(
    plasma_settings_surface surface,
    const plasma_settings_descriptor *descriptor
)
{
    return descriptor != NULL && descriptor->surface == surface;
}

int plasma_settings_surface_contains_setting_key(
    plasma_settings_surface surface,
    const char *setting_key
)
{
    return plasma_settings_surface_contains_setting(surface, plasma_settings_find_descriptor(setting_key));
}

void plasma_settings_context_init(
    plasma_settings_context *context,
    const screensave_saver_module *module,
    const screensave_common_config *common_config,
    const struct plasma_config_tag *product_config,
    screensave_renderer_kind requested_renderer_kind,
    screensave_renderer_kind active_renderer_kind
)
{
    const plasma_content_registry *registry;
    unsigned int journey_count;

    if (context == NULL) {
        return;
    }

    ZeroMemory(context, sizeof(*context));
    context->module = module;
    context->common_config = common_config;
    context->product_config = product_config;
    context->requested_renderer_kind = requested_renderer_kind;
    context->active_renderer_kind = active_renderer_kind;

    registry = plasma_content_get_registry();
    if (registry != NULL) {
        context->experimental_content_available =
            plasma_content_registry_has_channel(PLASMA_CONTENT_CHANNEL_EXPERIMENTAL);
        context->preset_sets_available = registry->preset_set_count > 0U;
        context->theme_sets_available = registry->theme_set_count > 0U;
    }

    context->journeys_available =
        plasma_transition_get_journeys(&journey_count) != NULL && journey_count > 0U;

    if (product_config != NULL) {
        context->favorites_configured =
            plasma_has_non_empty_text(product_config->selection.favorite_preset_keys) ||
            plasma_has_non_empty_text(product_config->selection.favorite_theme_keys);
    }
}

const plasma_settings_descriptor *plasma_settings_get_catalog(unsigned int *count_out)
{
    if (count_out != NULL) {
        *count_out = (unsigned int)(sizeof(g_plasma_settings_catalog) / sizeof(g_plasma_settings_catalog[0]));
    }

    return g_plasma_settings_catalog;
}

const plasma_settings_descriptor *plasma_settings_find_descriptor(const char *setting_key)
{
    unsigned int index;

    if (setting_key == NULL) {
        return NULL;
    }

    for (index = 0U; index < (unsigned int)(sizeof(g_plasma_settings_catalog) / sizeof(g_plasma_settings_catalog[0])); ++index) {
        if (strcmp(g_plasma_settings_catalog[index].setting_key, setting_key) == 0) {
            return &g_plasma_settings_catalog[index];
        }
    }

    return NULL;
}

int plasma_settings_catalog_validate(void)
{
    unsigned int index;
    unsigned int compare_index;

    for (index = 0U; index < (unsigned int)(sizeof(g_plasma_settings_catalog) / sizeof(g_plasma_settings_catalog[0])); ++index) {
        const plasma_settings_descriptor *descriptor;

        descriptor = &g_plasma_settings_catalog[index];
        if (
            descriptor->setting_key == NULL ||
            descriptor->display_name == NULL ||
            descriptor->summary == NULL ||
            descriptor->category_key == NULL ||
            descriptor->default_value_text == NULL ||
            descriptor->domain_summary == NULL
        ) {
            return 0;
        }

        if (
            descriptor->surface < PLASMA_SETTINGS_SURFACE_BASIC ||
            descriptor->surface > PLASMA_SETTINGS_SURFACE_AUTHOR_LAB
        ) {
            return 0;
        }

        for (compare_index = index + 1U;
            compare_index < (unsigned int)(sizeof(g_plasma_settings_catalog) / sizeof(g_plasma_settings_catalog[0]));
            ++compare_index) {
            if (
                strcmp(
                    descriptor->setting_key,
                    g_plasma_settings_catalog[compare_index].setting_key
                ) == 0
            ) {
                return 0;
            }
        }
    }

    return 1;
}

int plasma_settings_is_available(
    const plasma_settings_descriptor *descriptor,
    const plasma_settings_context *context
)
{
    if (descriptor == NULL || context == NULL) {
        return 0;
    }

    if (strcmp(descriptor->setting_key, "content_filter") == 0) {
        return context->experimental_content_available;
    }

    if (strcmp(descriptor->setting_key, "favorites_only") == 0) {
        return context->favorites_configured;
    }

    if (strcmp(descriptor->setting_key, "preset_set_key") == 0) {
        return context->preset_sets_available;
    }

    if (strcmp(descriptor->setting_key, "theme_set_key") == 0) {
        return context->theme_sets_available;
    }

    if (strcmp(descriptor->setting_key, "journey_key") == 0) {
        return
            context->journeys_available &&
            context->product_config != NULL &&
            context->product_config->transition.enabled &&
            (context->product_config->transition.policy == PLASMA_TRANSITION_POLICY_JOURNEY ||
                context->product_config->transition.policy == PLASMA_TRANSITION_POLICY_AUTO);
    }

    if (strcmp(descriptor->setting_key, "deterministic_seed") == 0) {
        return context->common_config != NULL && context->common_config->use_deterministic_seed;
    }

    if (plasma_setting_requires_transition_surface(descriptor->setting_key)) {
        int transition_surface_available;

        transition_surface_available =
            context->preset_sets_available ||
            context->theme_sets_available ||
            context->journeys_available;
        if (!transition_surface_available) {
            return 0;
        }

        if (
            plasma_setting_is_transition_driven(descriptor->setting_key) &&
            (context->product_config == NULL || !context->product_config->transition.enabled)
        ) {
            return 0;
        }
    }

    return 1;
}

void plasma_settings_config_set_defaults(struct plasma_config_tag *product_config)
{
    if (product_config == NULL) {
        return;
    }

    product_config->settings_surface = PLASMA_SETTINGS_SURFACE_BASIC;
}

void plasma_settings_config_clamp(struct plasma_config_tag *product_config)
{
    if (product_config == NULL) {
        return;
    }

    if (
        product_config->settings_surface < PLASMA_SETTINGS_SURFACE_BASIC ||
        product_config->settings_surface > PLASMA_SETTINGS_SURFACE_AUTHOR_LAB
    ) {
        product_config->settings_surface = PLASMA_SETTINGS_SURFACE_BASIC;
    }
}

int plasma_settings_resolve(
    plasma_settings_resolution *resolution,
    const plasma_settings_context *context
)
{
    plasma_transition_preferences transition_defaults;
    const plasma_settings_descriptor *descriptor;

    if (
        resolution == NULL ||
        context == NULL ||
        context->common_config == NULL ||
        context->product_config == NULL
    ) {
        return 0;
    }

    ZeroMemory(resolution, sizeof(*resolution));
    plasma_transition_preferences_set_defaults(&transition_defaults);

    resolution->surface = context->product_config->settings_surface;
    resolution->detail_level = context->common_config->detail_level;
    resolution->diagnostics_overlay_enabled = context->common_config->diagnostics_overlay_enabled ? 1 : 0;
    resolution->use_deterministic_seed = context->common_config->use_deterministic_seed ? 1 : 0;
    resolution->deterministic_seed = context->common_config->deterministic_seed;
    resolution->effect_mode = context->product_config->effect_mode;
    resolution->speed_mode = context->product_config->speed_mode;
    resolution->resolution_mode = context->product_config->resolution_mode;
    resolution->smoothing_mode = context->product_config->smoothing_mode;
    resolution->output_family = context->product_config->output_family;
    resolution->output_mode = context->product_config->output_mode;
    resolution->sampling_treatment = context->product_config->sampling_treatment;
    resolution->filter_treatment = context->product_config->filter_treatment;
    resolution->emulation_treatment = context->product_config->emulation_treatment;
    resolution->accent_treatment = context->product_config->accent_treatment;
    resolution->presentation_mode = context->product_config->presentation_mode;
    resolution->content_filter = PLASMA_CONTENT_FILTER_STABLE_ONLY;
    resolution->favorites_only = 0;
    resolution->preset_set_key = "";
    resolution->theme_set_key = "";
    resolution->transitions_enabled = 0;
    resolution->transition_policy = PLASMA_TRANSITION_POLICY_DISABLED;
    resolution->transition_fallback_policy = transition_defaults.fallback_policy;
    resolution->transition_seed_policy = transition_defaults.seed_policy;
    resolution->transition_interval_millis = transition_defaults.interval_millis;
    resolution->transition_duration_millis = transition_defaults.duration_millis;
    resolution->journey_key = "";

    descriptor = plasma_settings_find_descriptor("content_filter");
    if (descriptor != NULL && plasma_settings_is_available(descriptor, context)) {
        resolution->content_filter = context->product_config->selection.content_filter;
    }

    descriptor = plasma_settings_find_descriptor("favorites_only");
    if (descriptor != NULL && plasma_settings_is_available(descriptor, context)) {
        resolution->favorites_only = context->product_config->selection.favorites_only ? 1 : 0;
    }

    descriptor = plasma_settings_find_descriptor("preset_set_key");
    if (
        descriptor != NULL &&
        plasma_settings_is_available(descriptor, context) &&
        plasma_has_non_empty_text(context->product_config->selection.preset_set_key)
    ) {
        resolution->preset_set_key = context->product_config->selection.preset_set_key;
    }

    descriptor = plasma_settings_find_descriptor("theme_set_key");
    if (
        descriptor != NULL &&
        plasma_settings_is_available(descriptor, context) &&
        plasma_has_non_empty_text(context->product_config->selection.theme_set_key)
    ) {
        resolution->theme_set_key = context->product_config->selection.theme_set_key;
    }

    descriptor = plasma_settings_find_descriptor("transitions_enabled");
    if (descriptor != NULL && plasma_settings_is_available(descriptor, context)) {
        resolution->transitions_enabled = context->product_config->transition.enabled ? 1 : 0;
    }

    descriptor = plasma_settings_find_descriptor("transition_policy");
    if (
        resolution->transitions_enabled &&
        descriptor != NULL &&
        plasma_settings_is_available(descriptor, context)
    ) {
        resolution->transition_policy = context->product_config->transition.policy;
    }

    descriptor = plasma_settings_find_descriptor("transition_fallback_policy");
    if (
        resolution->transitions_enabled &&
        descriptor != NULL &&
        plasma_settings_is_available(descriptor, context)
    ) {
        resolution->transition_fallback_policy = context->product_config->transition.fallback_policy;
    }

    descriptor = plasma_settings_find_descriptor("transition_seed_policy");
    if (
        resolution->transitions_enabled &&
        descriptor != NULL &&
        plasma_settings_is_available(descriptor, context)
    ) {
        resolution->transition_seed_policy = context->product_config->transition.seed_policy;
    }

    descriptor = plasma_settings_find_descriptor("transition_interval_millis");
    if (
        resolution->transitions_enabled &&
        descriptor != NULL &&
        plasma_settings_is_available(descriptor, context)
    ) {
        resolution->transition_interval_millis = context->product_config->transition.interval_millis;
    }

    descriptor = plasma_settings_find_descriptor("transition_duration_millis");
    if (
        resolution->transitions_enabled &&
        descriptor != NULL &&
        plasma_settings_is_available(descriptor, context)
    ) {
        resolution->transition_duration_millis = context->product_config->transition.duration_millis;
    }

    descriptor = plasma_settings_find_descriptor("journey_key");
    if (
        resolution->transitions_enabled &&
        descriptor != NULL &&
        plasma_settings_is_available(descriptor, context) &&
        plasma_has_non_empty_text(context->product_config->transition.journey_key)
    ) {
        resolution->journey_key = context->product_config->transition.journey_key;
    }

    if (!resolution->use_deterministic_seed) {
        resolution->deterministic_seed = 0UL;
    }

    return 1;
}
