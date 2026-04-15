#include <stdlib.h>

#include "plasma_internal.h"
#include "plasma_resource.h"
#include "screensave/version.h"
#include "../../../../platform/src/core/base/saver_registry.h"

typedef struct plasma_dialog_state_tag {
    const screensave_saver_module *module;
    screensave_common_config *common_config;
    plasma_config *product_config;
    screensave_common_config working_common_config;
    plasma_config working_product_config;
} plasma_dialog_state;

typedef struct plasma_combo_item_tag {
    int value;
    const char *display_name;
} plasma_combo_item;

typedef struct plasma_dialog_binding_tag {
    int label_id;
    int control_id;
    const char *setting_key;
} plasma_dialog_binding;

static const char g_plasma_empty_key[] = "";

static const plasma_combo_item g_plasma_effect_items[] = {
    { PLASMA_EFFECT_PLASMA, "Plasma" },
    { PLASMA_EFFECT_FIRE, "Fire" },
    { PLASMA_EFFECT_INTERFERENCE, "Interference Field" },
    { PLASMA_EFFECT_CHEMICAL, "Chemical / Cellular (Experimental)" },
    { PLASMA_EFFECT_LATTICE, "Lattice / Quasi-Crystal (Experimental)" },
    { PLASMA_EFFECT_CAUSTIC, "Caustic / Marbling (Experimental)" },
    { PLASMA_EFFECT_AURORA, "Aurora / Curtain / Ribbon (Experimental)" },
    { PLASMA_EFFECT_SUBSTRATE, "Substrate / Vein / Coral (Experimental)" },
    { PLASMA_EFFECT_ARC, "Arc / Filament / Discharge (Experimental)" }
};

static const plasma_combo_item g_plasma_speed_items[] = {
    { PLASMA_SPEED_GENTLE, "Gentle" },
    { PLASMA_SPEED_STANDARD, "Standard" },
    { PLASMA_SPEED_LIVELY, "Lively" }
};

static const plasma_combo_item g_plasma_resolution_items[] = {
    { PLASMA_RESOLUTION_COARSE, "Coarse" },
    { PLASMA_RESOLUTION_STANDARD, "Standard" },
    { PLASMA_RESOLUTION_FINE, "Fine" }
};

static const plasma_combo_item g_plasma_smoothing_items[] = {
    { PLASMA_SMOOTHING_OFF, "Off" },
    { PLASMA_SMOOTHING_SOFT, "Soft Diffusion" },
    { PLASMA_SMOOTHING_GLOW, "Glow Diffusion" }
};

static const plasma_combo_item g_plasma_output_family_items[] = {
    { PLASMA_OUTPUT_FAMILY_RASTER, "Raster" },
    { PLASMA_OUTPUT_FAMILY_BANDED, "Banded" },
    { PLASMA_OUTPUT_FAMILY_CONTOUR, "Contour" },
    { PLASMA_OUTPUT_FAMILY_GLYPH, "Glyph" }
};

static const plasma_combo_item g_plasma_output_mode_items[] = {
    { PLASMA_OUTPUT_MODE_NATIVE_RASTER, "Native Raster" },
    { PLASMA_OUTPUT_MODE_POSTERIZED_BANDS, "Posterized Bands" },
    { PLASMA_OUTPUT_MODE_CONTOUR_ONLY, "Contour Only" },
    { PLASMA_OUTPUT_MODE_CONTOUR_BANDS, "Contour Bands" },
    { PLASMA_OUTPUT_MODE_ASCII_GLYPH, "ASCII Glyph" },
    { PLASMA_OUTPUT_MODE_MATRIX_GLYPH, "Matrix Glyph" }
};

static const plasma_combo_item g_plasma_filter_treatment_items[] = {
    { PLASMA_FILTER_TREATMENT_NONE, "None" },
    { PLASMA_FILTER_TREATMENT_BLUR, "Blur (Advanced)" },
    { PLASMA_FILTER_TREATMENT_GLOW_EDGE, "Glow Edge" },
    { PLASMA_FILTER_TREATMENT_HALFTONE_STIPPLE, "Halftone Stipple" },
    { PLASMA_FILTER_TREATMENT_EMBOSS_EDGE, "Emboss Edge" }
};

static const plasma_combo_item g_plasma_emulation_treatment_items[] = {
    { PLASMA_EMULATION_TREATMENT_NONE, "None" },
    { PLASMA_EMULATION_TREATMENT_PHOSPHOR, "Phosphor" },
    { PLASMA_EMULATION_TREATMENT_CRT, "CRT" }
};

static const plasma_combo_item g_plasma_accent_treatment_items[] = {
    { PLASMA_ACCENT_TREATMENT_NONE, "None" },
    { PLASMA_ACCENT_TREATMENT_OVERLAY_PASS, "Overlay Pass (Advanced)" },
    { PLASMA_ACCENT_TREATMENT_ACCENT_PASS, "Accent Pass" }
};

static const plasma_combo_item g_plasma_presentation_mode_items[] = {
    { PLASMA_PRESENTATION_MODE_FLAT, "Flat" },
    { PLASMA_PRESENTATION_MODE_HEIGHTFIELD, "Heightfield" },
    { PLASMA_PRESENTATION_MODE_CURTAIN, "Curtain" },
    { PLASMA_PRESENTATION_MODE_RIBBON, "Ribbon" },
    { PLASMA_PRESENTATION_MODE_CONTOUR_EXTRUSION, "Contour Extrusion" },
    { PLASMA_PRESENTATION_MODE_BOUNDED_SURFACE, "Bounded Surface" }
};

static const plasma_combo_item g_plasma_surface_items[] = {
    { PLASMA_SETTINGS_SURFACE_BASIC, "Basic" },
    { PLASMA_SETTINGS_SURFACE_ADVANCED, "Advanced" },
    { PLASMA_SETTINGS_SURFACE_AUTHOR_LAB, "Author/Lab" }
};

static const plasma_combo_item g_plasma_detail_level_items[] = {
    { SCREENSAVE_DETAIL_LEVEL_LOW, "Low" },
    { SCREENSAVE_DETAIL_LEVEL_STANDARD, "Standard" },
    { SCREENSAVE_DETAIL_LEVEL_HIGH, "High" }
};

static const plasma_combo_item g_plasma_content_filter_items[] = {
    { PLASMA_CONTENT_FILTER_STABLE_ONLY, "Stable Only" },
    { PLASMA_CONTENT_FILTER_STABLE_AND_EXPERIMENTAL, "Stable + Experimental" },
    { PLASMA_CONTENT_FILTER_EXPERIMENTAL_ONLY, "Experimental Only" }
};

static const plasma_combo_item g_plasma_transition_policy_items[] = {
    { PLASMA_TRANSITION_POLICY_DISABLED, "Disabled" },
    { PLASMA_TRANSITION_POLICY_AUTO, "Auto" },
    { PLASMA_TRANSITION_POLICY_THEME_SET, "Theme Set" },
    { PLASMA_TRANSITION_POLICY_PRESET_SET, "Preset Set" },
    { PLASMA_TRANSITION_POLICY_JOURNEY, "Journey" }
};

static const plasma_combo_item g_plasma_transition_fallback_items[] = {
    { PLASMA_TRANSITION_FALLBACK_HARD_CUT, "Hard Cut" },
    { PLASMA_TRANSITION_FALLBACK_THEME_MORPH, "Theme Morph" },
    { PLASMA_TRANSITION_FALLBACK_REJECT, "Reject" }
};

static const plasma_combo_item g_plasma_transition_seed_items[] = {
    { PLASMA_TRANSITION_SEED_CONTINUITY_KEEP_STREAM, "Keep Stream" },
    { PLASMA_TRANSITION_SEED_CONTINUITY_RESEED_TARGET, "Reseed Target" }
};

static const plasma_dialog_binding g_plasma_dialog_bindings[] = {
    { IDC_PLASMA_LABEL_PRESET, IDC_PLASMA_PRESET, "preset_key" },
    { IDC_PLASMA_LABEL_PRESET_SET, IDC_PLASMA_PRESET_SET, "preset_set_key" },
    { IDC_PLASMA_LABEL_THEME, IDC_PLASMA_THEME, "theme_key" },
    { IDC_PLASMA_LABEL_THEME_SET, IDC_PLASMA_THEME_SET, "theme_set_key" },
    { IDC_PLASMA_LABEL_SPEED, IDC_PLASMA_SPEED, "speed_mode" },
    { 0, IDC_PLASMA_FAVORITES_ONLY, "favorites_only" },
    { IDC_PLASMA_LABEL_DETAIL_LEVEL, IDC_PLASMA_DETAIL_LEVEL, "detail_level" },
    { IDC_PLASMA_LABEL_CONTENT_FILTER, IDC_PLASMA_CONTENT_FILTER, "content_filter" },
    { 0, IDC_PLASMA_TRANSITIONS_ENABLED, "transitions_enabled" },
    { IDC_PLASMA_LABEL_EFFECT, IDC_PLASMA_EFFECT, "effect_mode" },
    { IDC_PLASMA_LABEL_OUTPUT_FAMILY, IDC_PLASMA_OUTPUT_FAMILY, "output_family" },
    { IDC_PLASMA_LABEL_OUTPUT_MODE, IDC_PLASMA_OUTPUT_MODE, "output_mode" },
    { IDC_PLASMA_LABEL_RESOLUTION, IDC_PLASMA_RESOLUTION, "resolution_mode" },
    { IDC_PLASMA_LABEL_SMOOTHING, IDC_PLASMA_SMOOTHING, "smoothing_mode" },
    { IDC_PLASMA_LABEL_FILTER_TREATMENT, IDC_PLASMA_FILTER_TREATMENT, "filter_treatment" },
    { IDC_PLASMA_LABEL_EMULATION_TREATMENT, IDC_PLASMA_EMULATION_TREATMENT, "emulation_treatment" },
    { IDC_PLASMA_LABEL_ACCENT_TREATMENT, IDC_PLASMA_ACCENT_TREATMENT, "accent_treatment" },
    { IDC_PLASMA_LABEL_PRESENTATION_MODE, IDC_PLASMA_PRESENTATION_MODE, "presentation_mode" },
    { 0, IDC_PLASMA_DETERMINISTIC, "use_deterministic_seed" },
    { IDC_PLASMA_LABEL_TRANSITION_POLICY, IDC_PLASMA_TRANSITION_POLICY, "transition_policy" },
    { IDC_PLASMA_LABEL_JOURNEY, IDC_PLASMA_JOURNEY, "journey_key" },
    { IDC_PLASMA_LABEL_TRANSITION_FALLBACK, IDC_PLASMA_TRANSITION_FALLBACK, "transition_fallback_policy" },
    { IDC_PLASMA_LABEL_TRANSITION_SEED_POLICY, IDC_PLASMA_TRANSITION_SEED_POLICY, "transition_seed_policy" },
    { IDC_PLASMA_LABEL_TRANSITION_INTERVAL, IDC_PLASMA_TRANSITION_INTERVAL, "transition_interval_millis" },
    { IDC_PLASMA_LABEL_TRANSITION_DURATION, IDC_PLASMA_TRANSITION_DURATION, "transition_duration_millis" },
    { IDC_PLASMA_LABEL_DETERMINISTIC_SEED, IDC_PLASMA_DETERMINISTIC_SEED_VALUE, "deterministic_seed" },
    { 0, IDC_PLASMA_DIAGNOSTICS, "diagnostics_overlay_enabled" }
};

static int plasma_parse_ulong_text(const char *text, unsigned long *value_out);
static const char *plasma_format_ulong_text(unsigned long value, char *buffer, unsigned int buffer_size);

static void plasma_emit_config_diag(
    screensave_diag_context *diagnostics,
    screensave_diag_level level,
    unsigned long code,
    const char *text
)
{
    if (diagnostics == NULL) {
        return;
    }

    screensave_diag_emit(
        diagnostics,
        level,
        SCREENSAVE_DIAG_DOMAIN_CONFIG,
        code,
        "plasma_config",
        text
    );
}

static plasma_config *plasma_as_config(void *product_config, unsigned int product_config_size)
{
    if (product_config == NULL || product_config_size != sizeof(plasma_config)) {
        return NULL;
    }

    return (plasma_config *)product_config;
}

static const plasma_config *plasma_as_const_config(
    const void *product_config,
    unsigned int product_config_size
)
{
    if (product_config == NULL || product_config_size != sizeof(plasma_config)) {
        return NULL;
    }

    return (const plasma_config *)product_config;
}

static int plasma_build_registry_path(char *buffer, int buffer_size)
{
    if (buffer == NULL || buffer_size <= 0) {
        return 0;
    }

    buffer[0] = '\0';
    return lstrlenA(PLASMA_PRODUCT_REGISTRY_ROOTA) + 1 <= buffer_size &&
        lstrcpyA(buffer, PLASMA_PRODUCT_REGISTRY_ROOTA) != NULL;
}

static int plasma_read_flag(HKEY key, const char *value_name, int *value)
{
    DWORD data;
    DWORD type;
    DWORD size;

    if (value == NULL) {
        return 0;
    }

    data = 0UL;
    type = 0UL;
    size = sizeof(data);
    if (RegQueryValueExA(key, value_name, NULL, &type, (LPBYTE)&data, &size) != ERROR_SUCCESS || type != REG_DWORD) {
        return 0;
    }

    *value = data != 0UL;
    return 1;
}

static int plasma_read_dword(HKEY key, const char *value_name, unsigned long *value)
{
    DWORD data;
    DWORD type;
    DWORD size;

    if (value == NULL) {
        return 0;
    }

    data = 0UL;
    type = 0UL;
    size = sizeof(data);
    if (RegQueryValueExA(key, value_name, NULL, &type, (LPBYTE)&data, &size) != ERROR_SUCCESS || type != REG_DWORD) {
        return 0;
    }

    *value = (unsigned long)data;
    return 1;
}

static int plasma_read_string(HKEY key, const char *value_name, char *buffer, DWORD buffer_size)
{
    DWORD type;
    DWORD size;

    if (buffer == NULL || buffer_size == 0U) {
        return 0;
    }

    buffer[0] = '\0';
    type = 0UL;
    size = buffer_size;
    if (RegQueryValueExA(key, value_name, NULL, &type, (LPBYTE)buffer, &size) != ERROR_SUCCESS || type != REG_SZ) {
        buffer[0] = '\0';
        return 0;
    }

    buffer[buffer_size - 1U] = '\0';
    return buffer[0] != '\0';
}

static LONG plasma_write_flag(HKEY key, const char *value_name, int value)
{
    DWORD data;

    data = value ? 1UL : 0UL;
    return RegSetValueExA(key, value_name, 0, REG_DWORD, (const BYTE *)&data, sizeof(data));
}

static LONG plasma_write_dword(HKEY key, const char *value_name, unsigned long value)
{
    DWORD data;

    data = (DWORD)value;
    return RegSetValueExA(key, value_name, 0, REG_DWORD, (const BYTE *)&data, sizeof(data));
}

static LONG plasma_write_string(HKEY key, const char *value_name, const char *value)
{
    if (value == NULL || value[0] == '\0') {
        RegDeleteValueA(key, value_name);
        return ERROR_SUCCESS;
    }

    return RegSetValueExA(
        key,
        value_name,
        0,
        REG_SZ,
        (const BYTE *)value,
        (DWORD)(lstrlenA(value) + 1)
    );
}

static void plasma_config_set_product_defaults(plasma_config *config)
{
    if (config == NULL) {
        return;
    }

    plasma_settings_config_set_defaults(config);
    config->effect_mode = PLASMA_EFFECT_FIRE;
    config->speed_mode = PLASMA_SPEED_GENTLE;
    config->resolution_mode = PLASMA_RESOLUTION_STANDARD;
    config->smoothing_mode = PLASMA_SMOOTHING_SOFT;
    config->output_family = PLASMA_OUTPUT_FAMILY_RASTER;
    config->output_mode = PLASMA_OUTPUT_MODE_NATIVE_RASTER;
    config->sampling_treatment = PLASMA_SAMPLING_TREATMENT_NONE;
    config->filter_treatment = PLASMA_FILTER_TREATMENT_NONE;
    config->emulation_treatment = PLASMA_EMULATION_TREATMENT_NONE;
    config->accent_treatment = PLASMA_ACCENT_TREATMENT_NONE;
    config->presentation_mode = PLASMA_PRESENTATION_MODE_FLAT;
    plasma_selection_preferences_set_defaults(&config->selection);
    plasma_transition_preferences_set_defaults(&config->transition);
    plasma_benchlab_forcing_set_defaults(&config->benchlab);
}

void plasma_config_set_defaults(
    screensave_common_config *common_config,
    void *product_config,
    unsigned int product_config_size
)
{
    plasma_config *config;

    config = plasma_as_config(product_config, product_config_size);
    if (common_config == NULL || config == NULL) {
        return;
    }

    screensave_common_config_set_defaults(common_config);
    plasma_config_set_product_defaults(config);
    plasma_apply_preset_bundle_to_config(PLASMA_DEFAULT_PRESET_KEY, common_config, config);
}

void plasma_config_clamp(
    screensave_common_config *common_config,
    void *product_config,
    unsigned int product_config_size
)
{
    plasma_config *config;
    plasma_selection_state selection_state;

    config = plasma_as_config(product_config, product_config_size);
    if (common_config == NULL || config == NULL) {
        return;
    }

    screensave_common_config_clamp(common_config);
    plasma_settings_config_clamp(config);

    if (
        common_config->preset_key != NULL &&
        plasma_find_preset_values(common_config->preset_key) == NULL
    ) {
        common_config->preset_key = NULL;
    }

    if (
        common_config->theme_key == NULL ||
        plasma_find_theme_descriptor(common_config->theme_key) == NULL
    ) {
        common_config->theme_key = NULL;
    }

    if (config->effect_mode < PLASMA_EFFECT_PLASMA || config->effect_mode > PLASMA_EFFECT_ARC) {
        config->effect_mode = PLASMA_EFFECT_FIRE;
    }
    if (config->speed_mode < PLASMA_SPEED_GENTLE || config->speed_mode > PLASMA_SPEED_LIVELY) {
        config->speed_mode = PLASMA_SPEED_GENTLE;
    }
    if (
        config->resolution_mode < PLASMA_RESOLUTION_COARSE ||
        config->resolution_mode > PLASMA_RESOLUTION_FINE
    ) {
        config->resolution_mode = PLASMA_RESOLUTION_STANDARD;
    }
    if (config->smoothing_mode < PLASMA_SMOOTHING_OFF || config->smoothing_mode > PLASMA_SMOOTHING_GLOW) {
        config->smoothing_mode = PLASMA_SMOOTHING_SOFT;
    }
    if (
        config->output_family < PLASMA_OUTPUT_FAMILY_RASTER ||
        config->output_family > PLASMA_OUTPUT_FAMILY_GLYPH
    ) {
        config->output_family = PLASMA_OUTPUT_FAMILY_RASTER;
    }
    switch (config->output_family) {
    case PLASMA_OUTPUT_FAMILY_RASTER:
        config->output_mode = PLASMA_OUTPUT_MODE_NATIVE_RASTER;
        break;

    case PLASMA_OUTPUT_FAMILY_BANDED:
        if (config->output_mode != PLASMA_OUTPUT_MODE_POSTERIZED_BANDS) {
            config->output_mode = PLASMA_OUTPUT_MODE_POSTERIZED_BANDS;
        }
        break;

    case PLASMA_OUTPUT_FAMILY_CONTOUR:
        if (
            config->output_mode != PLASMA_OUTPUT_MODE_CONTOUR_ONLY &&
            config->output_mode != PLASMA_OUTPUT_MODE_CONTOUR_BANDS
        ) {
            config->output_mode = PLASMA_OUTPUT_MODE_CONTOUR_ONLY;
        }
        break;

    case PLASMA_OUTPUT_FAMILY_GLYPH:
        if (
            config->output_mode != PLASMA_OUTPUT_MODE_ASCII_GLYPH &&
            config->output_mode != PLASMA_OUTPUT_MODE_MATRIX_GLYPH
        ) {
            config->output_mode = PLASMA_OUTPUT_MODE_ASCII_GLYPH;
        }
        break;

    default:
        config->output_family = PLASMA_OUTPUT_FAMILY_RASTER;
        config->output_mode = PLASMA_OUTPUT_MODE_NATIVE_RASTER;
        break;
    }
    if (config->sampling_treatment != PLASMA_SAMPLING_TREATMENT_NONE) {
        config->sampling_treatment = PLASMA_SAMPLING_TREATMENT_NONE;
    }
    switch (config->filter_treatment) {
    case PLASMA_FILTER_TREATMENT_NONE:
    case PLASMA_FILTER_TREATMENT_BLUR:
    case PLASMA_FILTER_TREATMENT_GLOW_EDGE:
    case PLASMA_FILTER_TREATMENT_HALFTONE_STIPPLE:
    case PLASMA_FILTER_TREATMENT_EMBOSS_EDGE:
        break;

    default:
        config->filter_treatment = PLASMA_FILTER_TREATMENT_NONE;
        break;
    }
    if (
        config->emulation_treatment != PLASMA_EMULATION_TREATMENT_NONE &&
        config->emulation_treatment != PLASMA_EMULATION_TREATMENT_PHOSPHOR &&
        config->emulation_treatment != PLASMA_EMULATION_TREATMENT_CRT
    ) {
        config->emulation_treatment = PLASMA_EMULATION_TREATMENT_NONE;
    }
    if (
        config->accent_treatment != PLASMA_ACCENT_TREATMENT_NONE &&
        config->accent_treatment != PLASMA_ACCENT_TREATMENT_OVERLAY_PASS &&
        config->accent_treatment != PLASMA_ACCENT_TREATMENT_ACCENT_PASS
    ) {
        config->accent_treatment = PLASMA_ACCENT_TREATMENT_NONE;
    }
    if (
        config->presentation_mode < PLASMA_PRESENTATION_MODE_FLAT ||
        config->presentation_mode > PLASMA_PRESENTATION_MODE_BOUNDED_SURFACE
    ) {
        config->presentation_mode = PLASMA_PRESENTATION_MODE_FLAT;
    }

    plasma_selection_preferences_clamp(&config->selection);
    plasma_transition_preferences_clamp(&config->transition);
    plasma_benchlab_forcing_clamp(&config->benchlab);
    if (!plasma_selection_resolve(&selection_state, common_config, &config->selection)) {
        plasma_selection_preferences_set_defaults(&config->selection);
        plasma_transition_preferences_set_defaults(&config->transition);
        plasma_benchlab_forcing_set_defaults(&config->benchlab);
        common_config->preset_key = PLASMA_DEFAULT_PRESET_KEY;
        common_config->theme_key = PLASMA_DEFAULT_THEME_KEY;
        (void)plasma_selection_resolve(&selection_state, common_config, &config->selection);
    }
}

int plasma_config_load(
    const screensave_saver_module *module,
    screensave_common_config *common_config,
    void *product_config,
    unsigned int product_config_size,
    screensave_diag_context *diagnostics
)
{
    char path[260];
    HKEY key;
    plasma_config *config;
    unsigned long value_dword;
    char preset_key[64];
    char theme_key[64];
    char selection_key[PLASMA_CONTENT_KEY_TEXT_LENGTH];
    char key_list[PLASMA_CONTENT_KEY_LIST_LENGTH];
    char transition_key[PLASMA_TRANSITION_KEY_TEXT_LENGTH];
    int selection_flag;

    (void)diagnostics;

    config = plasma_as_config(product_config, product_config_size);
    if (common_config == NULL || config == NULL) {
        return 0;
    }

    plasma_config_set_defaults(common_config, product_config, product_config_size);
    if (!plasma_build_registry_path(path, sizeof(path))) {
        return 0;
    }

    if (RegOpenKeyExA(HKEY_CURRENT_USER, path, 0, KEY_QUERY_VALUE, &key) != ERROR_SUCCESS) {
        const char *legacy_product_key;

        legacy_product_key = screensave_saver_registry_legacy_product_key(
            module != NULL && module->identity.product_key != NULL ? module->identity.product_key : "plasma"
        );
        if (
            legacy_product_key == NULL ||
            !screensave_saver_registry_build_registry_root(legacy_product_key, path, (unsigned int)sizeof(path)) ||
            RegOpenKeyExA(HKEY_CURRENT_USER, path, 0, KEY_QUERY_VALUE, &key) != ERROR_SUCCESS
        ) {
            plasma_config_clamp(common_config, product_config, product_config_size);
            return 1;
        }
    }

    preset_key[0] = '\0';
    if (plasma_read_string(key, "PresetKey", preset_key, sizeof(preset_key))) {
        plasma_apply_preset_bundle_to_config(preset_key, common_config, config);
    }

    if (plasma_read_string(key, "ThemeKey", theme_key, sizeof(theme_key))) {
        if (plasma_find_theme_descriptor(theme_key) != NULL) {
            common_config->theme_key = plasma_find_theme_descriptor(theme_key)->theme_key;
        }
    }

    value_dword = (unsigned long)common_config->detail_level;
    if (plasma_read_dword(key, "DetailLevel", &value_dword)) {
        common_config->detail_level = (screensave_detail_level)value_dword;
    }
    value_dword = (unsigned long)config->settings_surface;
    if (plasma_read_dword(key, "SettingsSurface", &value_dword)) {
        config->settings_surface = (int)value_dword;
    }

    value_dword = common_config->deterministic_seed;
    (void)plasma_read_flag(key, "UseDeterministicSeed", &common_config->use_deterministic_seed);
    if (plasma_read_dword(key, "DeterministicSeed", &value_dword)) {
        common_config->deterministic_seed = value_dword;
    }
    (void)plasma_read_flag(key, "DiagnosticsOverlayEnabled", &common_config->diagnostics_overlay_enabled);

    value_dword = (unsigned long)config->effect_mode;
    if (plasma_read_dword(key, "EffectMode", &value_dword)) {
        config->effect_mode = (int)value_dword;
    }
    value_dword = (unsigned long)config->speed_mode;
    if (plasma_read_dword(key, "SpeedMode", &value_dword)) {
        config->speed_mode = (int)value_dword;
    }
    value_dword = (unsigned long)config->resolution_mode;
    if (plasma_read_dword(key, "ResolutionMode", &value_dword)) {
        config->resolution_mode = (int)value_dword;
    }
    value_dword = (unsigned long)config->smoothing_mode;
    if (plasma_read_dword(key, "SmoothingMode", &value_dword)) {
        config->smoothing_mode = (int)value_dword;
    }
    value_dword = (unsigned long)config->output_family;
    if (plasma_read_dword(key, "OutputFamily", &value_dword)) {
        config->output_family = (plasma_output_family)value_dword;
    }
    value_dword = (unsigned long)config->output_mode;
    if (plasma_read_dword(key, "OutputMode", &value_dword)) {
        config->output_mode = (plasma_output_mode)value_dword;
    }
    value_dword = (unsigned long)config->sampling_treatment;
    if (plasma_read_dword(key, "SamplingTreatment", &value_dword)) {
        config->sampling_treatment = (plasma_sampling_treatment)value_dword;
    }
    value_dword = (unsigned long)config->filter_treatment;
    if (plasma_read_dword(key, "FilterTreatment", &value_dword)) {
        config->filter_treatment = (plasma_filter_treatment)value_dword;
    }
    value_dword = (unsigned long)config->emulation_treatment;
    if (plasma_read_dword(key, "EmulationTreatment", &value_dword)) {
        config->emulation_treatment = (plasma_emulation_treatment)value_dword;
    }
    value_dword = (unsigned long)config->accent_treatment;
    if (plasma_read_dword(key, "AccentTreatment", &value_dword)) {
        config->accent_treatment = (plasma_accent_treatment)value_dword;
    }
    value_dword = (unsigned long)config->presentation_mode;
    if (plasma_read_dword(key, "PresentationMode", &value_dword)) {
        config->presentation_mode = (plasma_presentation_mode)value_dword;
    }

    value_dword = (unsigned long)config->selection.content_filter;
    if (plasma_read_dword(key, "ContentFilter", &value_dword)) {
        config->selection.content_filter = (plasma_content_filter)value_dword;
    }
    selection_flag = config->selection.favorites_only;
    if (plasma_read_flag(key, "FavoritesOnly", &selection_flag)) {
        config->selection.favorites_only = selection_flag;
    }
    selection_key[0] = '\0';
    if (plasma_read_string(key, "PresetSetKey", selection_key, sizeof(selection_key))) {
        lstrcpynA(
            config->selection.preset_set_key,
            selection_key,
            (int)sizeof(config->selection.preset_set_key)
        );
    }
    selection_key[0] = '\0';
    if (plasma_read_string(key, "ThemeSetKey", selection_key, sizeof(selection_key))) {
        lstrcpynA(
            config->selection.theme_set_key,
            selection_key,
            (int)sizeof(config->selection.theme_set_key)
        );
    }
    key_list[0] = '\0';
    if (plasma_read_string(key, "FavoritePresetKeys", key_list, sizeof(key_list))) {
        lstrcpynA(
            config->selection.favorite_preset_keys,
            key_list,
            (int)sizeof(config->selection.favorite_preset_keys)
        );
    }
    key_list[0] = '\0';
    if (plasma_read_string(key, "ExcludedPresetKeys", key_list, sizeof(key_list))) {
        lstrcpynA(
            config->selection.excluded_preset_keys,
            key_list,
            (int)sizeof(config->selection.excluded_preset_keys)
        );
    }
    key_list[0] = '\0';
    if (plasma_read_string(key, "FavoriteThemeKeys", key_list, sizeof(key_list))) {
        lstrcpynA(
            config->selection.favorite_theme_keys,
            key_list,
            (int)sizeof(config->selection.favorite_theme_keys)
        );
    }
    key_list[0] = '\0';
    if (plasma_read_string(key, "ExcludedThemeKeys", key_list, sizeof(key_list))) {
        lstrcpynA(
            config->selection.excluded_theme_keys,
            key_list,
            (int)sizeof(config->selection.excluded_theme_keys)
        );
    }
    selection_flag = config->transition.enabled;
    if (plasma_read_flag(key, "TransitionsEnabled", &selection_flag)) {
        config->transition.enabled = selection_flag;
    }
    value_dword = (unsigned long)config->transition.policy;
    if (plasma_read_dword(key, "TransitionPolicy", &value_dword)) {
        config->transition.policy = (plasma_transition_policy)value_dword;
    }
    value_dword = (unsigned long)config->transition.fallback_policy;
    if (plasma_read_dword(key, "TransitionFallbackPolicy", &value_dword)) {
        config->transition.fallback_policy = (plasma_transition_fallback_policy)value_dword;
    }
    value_dword = (unsigned long)config->transition.seed_policy;
    if (plasma_read_dword(key, "TransitionSeedPolicy", &value_dword)) {
        config->transition.seed_policy = (plasma_transition_seed_continuity_policy)value_dword;
    }
    value_dword = config->transition.interval_millis;
    if (plasma_read_dword(key, "TransitionIntervalMillis", &value_dword)) {
        config->transition.interval_millis = value_dword;
    }
    value_dword = config->transition.duration_millis;
    if (plasma_read_dword(key, "TransitionDurationMillis", &value_dword)) {
        config->transition.duration_millis = value_dword;
    }
    transition_key[0] = '\0';
    if (plasma_read_string(key, "JourneyKey", transition_key, sizeof(transition_key))) {
        lstrcpynA(
            config->transition.journey_key,
            transition_key,
            (int)sizeof(config->transition.journey_key)
        );
    }

    RegCloseKey(key);
    plasma_config_clamp(common_config, product_config, product_config_size);
    return 1;
}

int plasma_config_save(
    const screensave_saver_module *module,
    const screensave_common_config *common_config,
    const void *product_config,
    unsigned int product_config_size,
    screensave_diag_context *diagnostics
)
{
    char path[260];
    HKEY key;
    DWORD disposition;
    LONG result;
    plasma_config safe_product_config;
    screensave_common_config safe_common_config;
    const plasma_config *config;

    (void)module;

    config = plasma_as_const_config(product_config, product_config_size);
    if (common_config == NULL || config == NULL) {
        return 0;
    }

    safe_common_config = *common_config;
    safe_product_config = *config;
    plasma_config_clamp(&safe_common_config, &safe_product_config, sizeof(safe_product_config));
    if (!plasma_build_registry_path(path, sizeof(path))) {
        return 0;
    }

    result = RegCreateKeyExA(
        HKEY_CURRENT_USER,
        path,
        0,
        NULL,
        REG_OPTION_NON_VOLATILE,
        KEY_SET_VALUE,
        NULL,
        &key,
        &disposition
    );
    (void)disposition;
    if (result != ERROR_SUCCESS) {
        plasma_emit_config_diag(
            diagnostics,
            SCREENSAVE_DIAG_LEVEL_ERROR,
            6501UL,
            "The Plasma registry key could not be opened for writing."
        );
        return 0;
    }

    result = plasma_write_dword(key, "DetailLevel", (unsigned long)safe_common_config.detail_level);
    if (result == ERROR_SUCCESS) {
        result = plasma_write_dword(key, "SettingsSurface", (unsigned long)safe_product_config.settings_surface);
    }
    if (result == ERROR_SUCCESS) {
        result = plasma_write_flag(key, "DiagnosticsOverlayEnabled", safe_common_config.diagnostics_overlay_enabled);
    }
    if (result == ERROR_SUCCESS) {
        result = plasma_write_flag(key, "UseDeterministicSeed", safe_common_config.use_deterministic_seed);
    }
    if (result == ERROR_SUCCESS) {
        result = plasma_write_dword(key, "DeterministicSeed", safe_common_config.deterministic_seed);
    }
    if (result == ERROR_SUCCESS) {
        result = plasma_write_string(key, "PresetKey", safe_common_config.preset_key);
    }
    if (result == ERROR_SUCCESS) {
        result = plasma_write_string(key, "ThemeKey", safe_common_config.theme_key);
    }
    if (result == ERROR_SUCCESS) {
        result = plasma_write_dword(key, "EffectMode", (unsigned long)safe_product_config.effect_mode);
    }
    if (result == ERROR_SUCCESS) {
        result = plasma_write_dword(key, "SpeedMode", (unsigned long)safe_product_config.speed_mode);
    }
    if (result == ERROR_SUCCESS) {
        result = plasma_write_dword(key, "ResolutionMode", (unsigned long)safe_product_config.resolution_mode);
    }
    if (result == ERROR_SUCCESS) {
        result = plasma_write_dword(key, "SmoothingMode", (unsigned long)safe_product_config.smoothing_mode);
    }
    if (result == ERROR_SUCCESS) {
        result = plasma_write_dword(key, "OutputFamily", (unsigned long)safe_product_config.output_family);
    }
    if (result == ERROR_SUCCESS) {
        result = plasma_write_dword(key, "OutputMode", (unsigned long)safe_product_config.output_mode);
    }
    if (result == ERROR_SUCCESS) {
        result = plasma_write_dword(
            key,
            "SamplingTreatment",
            (unsigned long)safe_product_config.sampling_treatment
        );
    }
    if (result == ERROR_SUCCESS) {
        result = plasma_write_dword(
            key,
            "FilterTreatment",
            (unsigned long)safe_product_config.filter_treatment
        );
    }
    if (result == ERROR_SUCCESS) {
        result = plasma_write_dword(
            key,
            "EmulationTreatment",
            (unsigned long)safe_product_config.emulation_treatment
        );
    }
    if (result == ERROR_SUCCESS) {
        result = plasma_write_dword(
            key,
            "AccentTreatment",
            (unsigned long)safe_product_config.accent_treatment
        );
    }
    if (result == ERROR_SUCCESS) {
        result = plasma_write_dword(
            key,
            "PresentationMode",
            (unsigned long)safe_product_config.presentation_mode
        );
    }
    if (result == ERROR_SUCCESS) {
        result = plasma_write_dword(
            key,
            "ContentFilter",
            (unsigned long)safe_product_config.selection.content_filter
        );
    }
    if (result == ERROR_SUCCESS) {
        result = plasma_write_flag(key, "FavoritesOnly", safe_product_config.selection.favorites_only);
    }
    if (result == ERROR_SUCCESS) {
        result = plasma_write_string(key, "PresetSetKey", safe_product_config.selection.preset_set_key);
    }
    if (result == ERROR_SUCCESS) {
        result = plasma_write_string(key, "ThemeSetKey", safe_product_config.selection.theme_set_key);
    }
    if (result == ERROR_SUCCESS) {
        result = plasma_write_string(
            key,
            "FavoritePresetKeys",
            safe_product_config.selection.favorite_preset_keys
        );
    }
    if (result == ERROR_SUCCESS) {
        result = plasma_write_string(
            key,
            "ExcludedPresetKeys",
            safe_product_config.selection.excluded_preset_keys
        );
    }
    if (result == ERROR_SUCCESS) {
        result = plasma_write_string(
            key,
            "FavoriteThemeKeys",
            safe_product_config.selection.favorite_theme_keys
        );
    }
    if (result == ERROR_SUCCESS) {
        result = plasma_write_string(
            key,
            "ExcludedThemeKeys",
            safe_product_config.selection.excluded_theme_keys
        );
    }
    if (result == ERROR_SUCCESS) {
        result = plasma_write_flag(key, "TransitionsEnabled", safe_product_config.transition.enabled);
    }
    if (result == ERROR_SUCCESS) {
        result = plasma_write_dword(
            key,
            "TransitionPolicy",
            (unsigned long)safe_product_config.transition.policy
        );
    }
    if (result == ERROR_SUCCESS) {
        result = plasma_write_dword(
            key,
            "TransitionFallbackPolicy",
            (unsigned long)safe_product_config.transition.fallback_policy
        );
    }
    if (result == ERROR_SUCCESS) {
        result = plasma_write_dword(
            key,
            "TransitionSeedPolicy",
            (unsigned long)safe_product_config.transition.seed_policy
        );
    }
    if (result == ERROR_SUCCESS) {
        result = plasma_write_dword(
            key,
            "TransitionIntervalMillis",
            safe_product_config.transition.interval_millis
        );
    }
    if (result == ERROR_SUCCESS) {
        result = plasma_write_dword(
            key,
            "TransitionDurationMillis",
            safe_product_config.transition.duration_millis
        );
    }
    if (result == ERROR_SUCCESS) {
        result = plasma_write_string(
            key,
            "JourneyKey",
            safe_product_config.transition.journey_key
        );
    }

    RegCloseKey(key);
    if (result != ERROR_SUCCESS) {
        plasma_emit_config_diag(
            diagnostics,
            SCREENSAVE_DIAG_LEVEL_ERROR,
            6502UL,
            "The Plasma registry values could not be saved."
        );
        return 0;
    }

    return 1;
}

static LRESULT plasma_add_combo_item(HWND dialog, int control_id, const char *text, LPARAM item_data)
{
    LRESULT index;

    index = SendDlgItemMessageA(dialog, control_id, CB_ADDSTRING, 0U, (LPARAM)text);
    if (index >= 0L) {
        SendDlgItemMessageA(dialog, control_id, CB_SETITEMDATA, (WPARAM)index, item_data);
    }

    return index;
}

static const char *plasma_get_combo_string_value(HWND dialog, int control_id, const char *default_value)
{
    LRESULT index;
    LRESULT item_data;

    index = SendDlgItemMessageA(dialog, control_id, CB_GETCURSEL, 0U, 0L);
    if (index == CB_ERR) {
        return default_value;
    }

    item_data = SendDlgItemMessageA(dialog, control_id, CB_GETITEMDATA, (WPARAM)index, 0L);
    if (item_data == CB_ERR) {
        return default_value;
    }

    return (const char *)item_data;
}

static void plasma_select_combo_value(HWND dialog, int control_id, LPARAM item_data)
{
    LRESULT count;
    LRESULT index;

    count = SendDlgItemMessageA(dialog, control_id, CB_GETCOUNT, 0U, 0L);
    for (index = 0L; index < count; ++index) {
        if (SendDlgItemMessageA(dialog, control_id, CB_GETITEMDATA, (WPARAM)index, 0L) == item_data) {
            SendDlgItemMessageA(dialog, control_id, CB_SETCURSEL, (WPARAM)index, 0L);
            return;
        }
    }
}

static void plasma_select_combo_string(HWND dialog, int control_id, const char *selected_key)
{
    LRESULT count;
    LRESULT index;

    count = SendDlgItemMessageA(dialog, control_id, CB_GETCOUNT, 0U, 0L);
    for (index = 0L; index < count; ++index) {
        const char *item_key;

        item_key = (const char *)SendDlgItemMessageA(dialog, control_id, CB_GETITEMDATA, (WPARAM)index, 0L);
        if (
            item_key != NULL &&
            selected_key != NULL &&
            strcmp(item_key, selected_key) == 0
        ) {
            SendDlgItemMessageA(dialog, control_id, CB_SETCURSEL, (WPARAM)index, 0L);
            return;
        }
    }

    if (count > 0L) {
        SendDlgItemMessageA(dialog, control_id, CB_SETCURSEL, 0U, 0L);
    }
}

static int plasma_get_combo_value(HWND dialog, int control_id, int default_value)
{
    LRESULT index;
    LRESULT item_data;

    index = SendDlgItemMessageA(dialog, control_id, CB_GETCURSEL, 0U, 0L);
    if (index == CB_ERR) {
        return default_value;
    }

    item_data = SendDlgItemMessageA(dialog, control_id, CB_GETITEMDATA, (WPARAM)index, 0L);
    if (item_data == CB_ERR) {
        return default_value;
    }

    return (int)item_data;
}

static void plasma_copy_key_text(char *buffer, unsigned int buffer_size, const char *value)
{
    if (buffer == NULL || buffer_size == 0U) {
        return;
    }

    buffer[0] = '\0';
    if (value == NULL) {
        return;
    }

    lstrcpynA(buffer, value, (int)buffer_size);
}

static unsigned long plasma_get_edit_ulong(HWND dialog, int control_id, unsigned long default_value)
{
    char text[32];
    unsigned long parsed_value;

    text[0] = '\0';
    GetDlgItemTextA(dialog, control_id, text, (int)sizeof(text));
    if (!plasma_parse_ulong_text(text, &parsed_value)) {
        return default_value;
    }

    return parsed_value;
}

static void plasma_set_edit_ulong(HWND dialog, int control_id, unsigned long value)
{
    char text[32];

    SetDlgItemTextA(
        dialog,
        control_id,
        plasma_format_ulong_text(value, text, (unsigned int)sizeof(text))
    );
}

static void plasma_set_control_visibility(HWND dialog, int control_id, int visible)
{
    HWND control;

    control = GetDlgItem(dialog, control_id);
    if (control != NULL) {
        ShowWindow(control, visible ? SW_SHOW : SW_HIDE);
    }
}

static void plasma_populate_named_combo(
    HWND dialog,
    int control_id,
    const plasma_combo_item *items,
    unsigned int item_count
)
{
    unsigned int index;

    SendDlgItemMessageA(dialog, control_id, CB_RESETCONTENT, 0U, 0L);
    for (index = 0U; index < item_count; ++index) {
        plasma_add_combo_item(dialog, control_id, items[index].display_name, (LPARAM)items[index].value);
    }
}

static int plasma_output_mode_is_visible_for_family(
    plasma_output_family family,
    plasma_output_mode mode
)
{
    switch (family) {
    case PLASMA_OUTPUT_FAMILY_BANDED:
        return mode == PLASMA_OUTPUT_MODE_POSTERIZED_BANDS;

    case PLASMA_OUTPUT_FAMILY_CONTOUR:
        return
            mode == PLASMA_OUTPUT_MODE_CONTOUR_ONLY ||
            mode == PLASMA_OUTPUT_MODE_CONTOUR_BANDS;

    case PLASMA_OUTPUT_FAMILY_GLYPH:
        return
            mode == PLASMA_OUTPUT_MODE_ASCII_GLYPH ||
            mode == PLASMA_OUTPUT_MODE_MATRIX_GLYPH;

    case PLASMA_OUTPUT_FAMILY_RASTER:
    default:
        return mode == PLASMA_OUTPUT_MODE_NATIVE_RASTER;
    }
}

static void plasma_populate_output_mode_combo(
    HWND dialog,
    plasma_output_family family
)
{
    unsigned int index;

    SendDlgItemMessageA(dialog, IDC_PLASMA_OUTPUT_MODE, CB_RESETCONTENT, 0U, 0L);
    for (index = 0U; index < (unsigned int)(sizeof(g_plasma_output_mode_items) / sizeof(g_plasma_output_mode_items[0])); ++index) {
        if (
            plasma_output_mode_is_visible_for_family(
                family,
                (plasma_output_mode)g_plasma_output_mode_items[index].value
            )
        ) {
            plasma_add_combo_item(
                dialog,
                IDC_PLASMA_OUTPUT_MODE,
                g_plasma_output_mode_items[index].display_name,
                (LPARAM)g_plasma_output_mode_items[index].value
            );
        }
    }
}

static void plasma_populate_dialog_lists(HWND dialog, const screensave_saver_module *module)
{
    const plasma_content_registry *registry;
    const plasma_content_journey_entry *journeys;
    unsigned int journey_count;
    unsigned int index;

    registry = plasma_content_get_registry();
    journeys = plasma_transition_get_journeys(&journey_count);

    plasma_populate_named_combo(
        dialog,
        IDC_PLASMA_SURFACE,
        g_plasma_surface_items,
        (unsigned int)(sizeof(g_plasma_surface_items) / sizeof(g_plasma_surface_items[0]))
    );

    SendDlgItemMessageA(dialog, IDC_PLASMA_PRESET, CB_RESETCONTENT, 0U, 0L);
    for (index = 0U; index < module->preset_count; ++index) {
        plasma_add_combo_item(dialog, IDC_PLASMA_PRESET, module->presets[index].display_name, (LPARAM)index);
    }

    SendDlgItemMessageA(dialog, IDC_PLASMA_THEME, CB_RESETCONTENT, 0U, 0L);
    for (index = 0U; index < module->theme_count; ++index) {
        plasma_add_combo_item(dialog, IDC_PLASMA_THEME, module->themes[index].display_name, (LPARAM)index);
    }

    plasma_populate_named_combo(
        dialog,
        IDC_PLASMA_SPEED,
        g_plasma_speed_items,
        (unsigned int)(sizeof(g_plasma_speed_items) / sizeof(g_plasma_speed_items[0]))
    );
    plasma_populate_named_combo(
        dialog,
        IDC_PLASMA_DETAIL_LEVEL,
        g_plasma_detail_level_items,
        (unsigned int)(sizeof(g_plasma_detail_level_items) / sizeof(g_plasma_detail_level_items[0]))
    );
    plasma_populate_named_combo(
        dialog,
        IDC_PLASMA_EFFECT,
        g_plasma_effect_items,
        (unsigned int)(sizeof(g_plasma_effect_items) / sizeof(g_plasma_effect_items[0]))
    );
    plasma_populate_named_combo(
        dialog,
        IDC_PLASMA_RESOLUTION,
        g_plasma_resolution_items,
        (unsigned int)(sizeof(g_plasma_resolution_items) / sizeof(g_plasma_resolution_items[0]))
    );
    plasma_populate_named_combo(
        dialog,
        IDC_PLASMA_SMOOTHING,
        g_plasma_smoothing_items,
        (unsigned int)(sizeof(g_plasma_smoothing_items) / sizeof(g_plasma_smoothing_items[0]))
    );
    plasma_populate_named_combo(
        dialog,
        IDC_PLASMA_OUTPUT_FAMILY,
        g_plasma_output_family_items,
        (unsigned int)(sizeof(g_plasma_output_family_items) / sizeof(g_plasma_output_family_items[0]))
    );
    plasma_populate_output_mode_combo(dialog, PLASMA_OUTPUT_FAMILY_RASTER);
    plasma_populate_named_combo(
        dialog,
        IDC_PLASMA_FILTER_TREATMENT,
        g_plasma_filter_treatment_items,
        (unsigned int)(sizeof(g_plasma_filter_treatment_items) / sizeof(g_plasma_filter_treatment_items[0]))
    );
    plasma_populate_named_combo(
        dialog,
        IDC_PLASMA_EMULATION_TREATMENT,
        g_plasma_emulation_treatment_items,
        (unsigned int)(sizeof(g_plasma_emulation_treatment_items) / sizeof(g_plasma_emulation_treatment_items[0]))
    );
    plasma_populate_named_combo(
        dialog,
        IDC_PLASMA_ACCENT_TREATMENT,
        g_plasma_accent_treatment_items,
        (unsigned int)(sizeof(g_plasma_accent_treatment_items) / sizeof(g_plasma_accent_treatment_items[0]))
    );
    plasma_populate_named_combo(
        dialog,
        IDC_PLASMA_PRESENTATION_MODE,
        g_plasma_presentation_mode_items,
        (unsigned int)(sizeof(g_plasma_presentation_mode_items) / sizeof(g_plasma_presentation_mode_items[0]))
    );
    plasma_populate_named_combo(
        dialog,
        IDC_PLASMA_CONTENT_FILTER,
        g_plasma_content_filter_items,
        (unsigned int)(sizeof(g_plasma_content_filter_items) / sizeof(g_plasma_content_filter_items[0]))
    );
    plasma_populate_named_combo(
        dialog,
        IDC_PLASMA_TRANSITION_POLICY,
        g_plasma_transition_policy_items,
        (unsigned int)(sizeof(g_plasma_transition_policy_items) / sizeof(g_plasma_transition_policy_items[0]))
    );
    plasma_populate_named_combo(
        dialog,
        IDC_PLASMA_TRANSITION_FALLBACK,
        g_plasma_transition_fallback_items,
        (unsigned int)(sizeof(g_plasma_transition_fallback_items) / sizeof(g_plasma_transition_fallback_items[0]))
    );
    plasma_populate_named_combo(
        dialog,
        IDC_PLASMA_TRANSITION_SEED_POLICY,
        g_plasma_transition_seed_items,
        (unsigned int)(sizeof(g_plasma_transition_seed_items) / sizeof(g_plasma_transition_seed_items[0]))
    );

    SendDlgItemMessageA(dialog, IDC_PLASMA_PRESET_SET, CB_RESETCONTENT, 0U, 0L);
    plasma_add_combo_item(dialog, IDC_PLASMA_PRESET_SET, "(No Preset Set)", (LPARAM)g_plasma_empty_key);
    if (registry != NULL) {
        for (index = 0U; index < registry->preset_set_count; ++index) {
            plasma_add_combo_item(
                dialog,
                IDC_PLASMA_PRESET_SET,
                registry->preset_sets[index].display_name,
                (LPARAM)registry->preset_sets[index].set_key
            );
        }
    }

    SendDlgItemMessageA(dialog, IDC_PLASMA_THEME_SET, CB_RESETCONTENT, 0U, 0L);
    plasma_add_combo_item(dialog, IDC_PLASMA_THEME_SET, "(No Theme Set)", (LPARAM)g_plasma_empty_key);
    if (registry != NULL) {
        for (index = 0U; index < registry->theme_set_count; ++index) {
            plasma_add_combo_item(
                dialog,
                IDC_PLASMA_THEME_SET,
                registry->theme_sets[index].display_name,
                (LPARAM)registry->theme_sets[index].set_key
            );
        }
    }

    SendDlgItemMessageA(dialog, IDC_PLASMA_JOURNEY, CB_RESETCONTENT, 0U, 0L);
    plasma_add_combo_item(dialog, IDC_PLASMA_JOURNEY, "(No Journey)", (LPARAM)g_plasma_empty_key);
    if (journeys != NULL) {
        for (index = 0U; index < journey_count; ++index) {
            plasma_add_combo_item(
                dialog,
                IDC_PLASMA_JOURNEY,
                journeys[index].display_name,
                (LPARAM)journeys[index].journey_key
            );
        }
    }
}

static void plasma_update_dialog_info(
    HWND dialog,
    plasma_settings_surface surface,
    const plasma_settings_context *settings_context
)
{
    char info[640];
    const screensave_version_info *version_info;

    version_info = screensave_version_get_info();
    info[0] = '\0';
    lstrcpyA(info, "Plasma\r\n");
    lstrcatA(info, version_info->version_text);
    lstrcatA(info, "\r\n");
    if (surface == PLASMA_SETTINGS_SURFACE_BASIC) {
        lstrcatA(
            info,
            "Basic keeps the first-pass controls honest: preset, theme, speed, visual intensity, content pool, and transitions."
        );
    } else if (surface == PLASMA_SETTINGS_SURFACE_ADVANCED) {
        lstrcatA(
            info,
            "Advanced adds the real visual grammar: generator family, output family and mode, detail, smoothing, treatments, presentation, and deterministic seed mode."
        );
    } else {
        lstrcatA(
            info,
            "Author/Lab keeps curation, journeys, fallback policy, deterministic seed value, and diagnostics in one bounded author-facing layer."
        );
    }

    if (settings_context != NULL) {
        if (!settings_context->experimental_content_available) {
            lstrcatA(info, "\r\nExperimental pool controls stay disabled because the current registry has no experimental content.");
        }
        if (!settings_context->favorites_configured) {
            lstrcatA(info, "\r\nFavorites-only stays disabled until favorite keys are configured through product settings import or export.");
        }
        if (!settings_context->journeys_available) {
            lstrcatA(info, "\r\nJourney controls stay unavailable until a product-local journey surface exists.");
        }
    }

    SetDlgItemTextA(dialog, IDC_PLASMA_INFO, info);
}

static void plasma_update_dialog_surface(HWND dialog, plasma_dialog_state *dialog_state)
{
    plasma_settings_context settings_context;
    plasma_settings_surface surface;
    unsigned int index;

    if (dialog_state == NULL) {
        return;
    }

    surface = (plasma_settings_surface)dialog_state->working_product_config.settings_surface;
    plasma_settings_context_init(
        &settings_context,
        dialog_state->module,
        &dialog_state->working_common_config,
        &dialog_state->working_product_config,
        SCREENSAVE_RENDERER_KIND_UNKNOWN,
        dialog_state->module != NULL
            ? dialog_state->module->routing_policy.preferred_kind
            : SCREENSAVE_RENDERER_KIND_UNKNOWN
    );

    for (index = 0U; index < (unsigned int)(sizeof(g_plasma_dialog_bindings) / sizeof(g_plasma_dialog_bindings[0])); ++index) {
        const plasma_dialog_binding *binding;
        const plasma_settings_descriptor *descriptor;
        int visible;
        int enabled;

        binding = &g_plasma_dialog_bindings[index];
        descriptor = plasma_settings_find_descriptor(binding->setting_key);
        visible = descriptor != NULL && descriptor->surface <= surface;
        enabled = visible && plasma_settings_is_available(descriptor, &settings_context);

        if (binding->label_id != 0) {
            plasma_set_control_visibility(dialog, binding->label_id, visible);
        }
        plasma_set_control_visibility(dialog, binding->control_id, visible);
        EnableWindow(GetDlgItem(dialog, binding->control_id), enabled ? TRUE : FALSE);
    }

    plasma_update_dialog_info(dialog, surface, &settings_context);
}

static void plasma_read_dialog_settings(
    HWND dialog,
    const screensave_saver_module *module,
    screensave_common_config *common_config,
    plasma_config *product_config
)
{
    LRESULT preset_index;
    LRESULT theme_index;
    screensave_common_config saved_common_config;
    plasma_config saved_product_config;
    const char *selected_key;

    saved_common_config = *common_config;
    saved_product_config = *product_config;

    plasma_config_set_defaults(common_config, product_config, sizeof(*product_config));
    product_config->selection = saved_product_config.selection;
    product_config->transition = saved_product_config.transition;
    product_config->settings_surface = plasma_get_combo_value(
        dialog,
        IDC_PLASMA_SURFACE,
        saved_product_config.settings_surface
    );
    common_config->randomization_mode = saved_common_config.randomization_mode;
    common_config->randomization_scope = saved_common_config.randomization_scope;

    preset_index = SendDlgItemMessageA(dialog, IDC_PLASMA_PRESET, CB_GETCURSEL, 0U, 0L);
    if (preset_index != CB_ERR && (unsigned int)preset_index < module->preset_count) {
        plasma_apply_preset_bundle_to_config(module->presets[preset_index].preset_key, common_config, product_config);
    } else {
        common_config->preset_key = saved_common_config.preset_key;
        common_config->theme_key = saved_common_config.theme_key;
    }

    theme_index = SendDlgItemMessageA(dialog, IDC_PLASMA_THEME, CB_GETCURSEL, 0U, 0L);
    if (theme_index != CB_ERR && (unsigned int)theme_index < module->theme_count) {
        common_config->theme_key = module->themes[theme_index].theme_key;
    }

    common_config->detail_level = (screensave_detail_level)plasma_get_combo_value(
        dialog,
        IDC_PLASMA_DETAIL_LEVEL,
        common_config->detail_level
    );
    product_config->speed_mode = plasma_get_combo_value(dialog, IDC_PLASMA_SPEED, product_config->speed_mode);
    product_config->effect_mode = plasma_get_combo_value(dialog, IDC_PLASMA_EFFECT, product_config->effect_mode);
    product_config->output_family = (plasma_output_family)plasma_get_combo_value(
        dialog,
        IDC_PLASMA_OUTPUT_FAMILY,
        product_config->output_family
    );
    product_config->output_mode = (plasma_output_mode)plasma_get_combo_value(
        dialog,
        IDC_PLASMA_OUTPUT_MODE,
        product_config->output_mode
    );
    product_config->resolution_mode = plasma_get_combo_value(
        dialog,
        IDC_PLASMA_RESOLUTION,
        product_config->resolution_mode
    );
    product_config->smoothing_mode = plasma_get_combo_value(
        dialog,
        IDC_PLASMA_SMOOTHING,
        product_config->smoothing_mode
    );
    product_config->filter_treatment = (plasma_filter_treatment)plasma_get_combo_value(
        dialog,
        IDC_PLASMA_FILTER_TREATMENT,
        product_config->filter_treatment
    );
    product_config->emulation_treatment = (plasma_emulation_treatment)plasma_get_combo_value(
        dialog,
        IDC_PLASMA_EMULATION_TREATMENT,
        product_config->emulation_treatment
    );
    product_config->accent_treatment = (plasma_accent_treatment)plasma_get_combo_value(
        dialog,
        IDC_PLASMA_ACCENT_TREATMENT,
        product_config->accent_treatment
    );
    product_config->presentation_mode = (plasma_presentation_mode)plasma_get_combo_value(
        dialog,
        IDC_PLASMA_PRESENTATION_MODE,
        product_config->presentation_mode
    );
    product_config->selection.content_filter = (plasma_content_filter)plasma_get_combo_value(
        dialog,
        IDC_PLASMA_CONTENT_FILTER,
        product_config->selection.content_filter
    );
    product_config->selection.favorites_only =
        IsDlgButtonChecked(dialog, IDC_PLASMA_FAVORITES_ONLY) == BST_CHECKED;
    selected_key = plasma_get_combo_string_value(
        dialog,
        IDC_PLASMA_PRESET_SET,
        saved_product_config.selection.preset_set_key
    );
    plasma_copy_key_text(
        product_config->selection.preset_set_key,
        (unsigned int)sizeof(product_config->selection.preset_set_key),
        selected_key
    );
    selected_key = plasma_get_combo_string_value(
        dialog,
        IDC_PLASMA_THEME_SET,
        saved_product_config.selection.theme_set_key
    );
    plasma_copy_key_text(
        product_config->selection.theme_set_key,
        (unsigned int)sizeof(product_config->selection.theme_set_key),
        selected_key
    );
    product_config->transition.enabled =
        IsDlgButtonChecked(dialog, IDC_PLASMA_TRANSITIONS_ENABLED) == BST_CHECKED;
    product_config->transition.policy = (plasma_transition_policy)plasma_get_combo_value(
        dialog,
        IDC_PLASMA_TRANSITION_POLICY,
        product_config->transition.policy
    );
    product_config->transition.fallback_policy =
        (plasma_transition_fallback_policy)plasma_get_combo_value(
            dialog,
            IDC_PLASMA_TRANSITION_FALLBACK,
            product_config->transition.fallback_policy
        );
    product_config->transition.seed_policy =
        (plasma_transition_seed_continuity_policy)plasma_get_combo_value(
            dialog,
            IDC_PLASMA_TRANSITION_SEED_POLICY,
            product_config->transition.seed_policy
        );
    product_config->transition.interval_millis = plasma_get_edit_ulong(
        dialog,
        IDC_PLASMA_TRANSITION_INTERVAL,
        product_config->transition.interval_millis
    );
    product_config->transition.duration_millis = plasma_get_edit_ulong(
        dialog,
        IDC_PLASMA_TRANSITION_DURATION,
        product_config->transition.duration_millis
    );
    selected_key = plasma_get_combo_string_value(
        dialog,
        IDC_PLASMA_JOURNEY,
        saved_product_config.transition.journey_key
    );
    plasma_copy_key_text(
        product_config->transition.journey_key,
        (unsigned int)sizeof(product_config->transition.journey_key),
        selected_key
    );
    common_config->use_deterministic_seed = IsDlgButtonChecked(dialog, IDC_PLASMA_DETERMINISTIC) == BST_CHECKED;
    common_config->deterministic_seed = plasma_get_edit_ulong(
        dialog,
        IDC_PLASMA_DETERMINISTIC_SEED_VALUE,
        saved_common_config.deterministic_seed
    );
    common_config->diagnostics_overlay_enabled = IsDlgButtonChecked(dialog, IDC_PLASMA_DIAGNOSTICS) == BST_CHECKED;

    plasma_settings_config_clamp(product_config);
    plasma_selection_preferences_clamp(&product_config->selection);
    plasma_transition_preferences_clamp(&product_config->transition);
}

static void plasma_apply_settings_to_dialog(HWND dialog, plasma_dialog_state *dialog_state)
{
    unsigned int index;
    const screensave_saver_module *module;
    const screensave_common_config *common_config;
    const plasma_config *product_config;

    if (dialog_state == NULL) {
        return;
    }

    module = dialog_state->module;
    common_config = &dialog_state->working_common_config;
    product_config = &dialog_state->working_product_config;

    plasma_select_combo_value(dialog, IDC_PLASMA_SURFACE, (LPARAM)product_config->settings_surface);

    if (common_config->preset_key != NULL) {
        for (index = 0U; index < module->preset_count; ++index) {
            if (lstrcmpiA(module->presets[index].preset_key, common_config->preset_key) == 0) {
                SendDlgItemMessageA(dialog, IDC_PLASMA_PRESET, CB_SETCURSEL, (WPARAM)index, 0L);
                break;
            }
        }
    }

    if (common_config->theme_key != NULL) {
        for (index = 0U; index < module->theme_count; ++index) {
            if (lstrcmpiA(module->themes[index].theme_key, common_config->theme_key) == 0) {
                SendDlgItemMessageA(dialog, IDC_PLASMA_THEME, CB_SETCURSEL, (WPARAM)index, 0L);
                break;
            }
        }
    }

    plasma_select_combo_value(dialog, IDC_PLASMA_SPEED, (LPARAM)product_config->speed_mode);
    plasma_select_combo_value(dialog, IDC_PLASMA_DETAIL_LEVEL, (LPARAM)common_config->detail_level);
    plasma_select_combo_value(dialog, IDC_PLASMA_EFFECT, (LPARAM)product_config->effect_mode);
    plasma_select_combo_value(dialog, IDC_PLASMA_OUTPUT_FAMILY, (LPARAM)product_config->output_family);
    plasma_populate_output_mode_combo(dialog, product_config->output_family);
    plasma_select_combo_value(dialog, IDC_PLASMA_OUTPUT_MODE, (LPARAM)product_config->output_mode);
    plasma_select_combo_value(dialog, IDC_PLASMA_RESOLUTION, (LPARAM)product_config->resolution_mode);
    plasma_select_combo_value(dialog, IDC_PLASMA_SMOOTHING, (LPARAM)product_config->smoothing_mode);
    plasma_select_combo_value(dialog, IDC_PLASMA_FILTER_TREATMENT, (LPARAM)product_config->filter_treatment);
    plasma_select_combo_value(
        dialog,
        IDC_PLASMA_EMULATION_TREATMENT,
        (LPARAM)product_config->emulation_treatment
    );
    plasma_select_combo_value(dialog, IDC_PLASMA_ACCENT_TREATMENT, (LPARAM)product_config->accent_treatment);
    plasma_select_combo_value(dialog, IDC_PLASMA_PRESENTATION_MODE, (LPARAM)product_config->presentation_mode);
    plasma_select_combo_value(dialog, IDC_PLASMA_CONTENT_FILTER, (LPARAM)product_config->selection.content_filter);
    plasma_select_combo_string(dialog, IDC_PLASMA_PRESET_SET, product_config->selection.preset_set_key);
    plasma_select_combo_string(dialog, IDC_PLASMA_THEME_SET, product_config->selection.theme_set_key);
    plasma_select_combo_value(dialog, IDC_PLASMA_TRANSITION_POLICY, (LPARAM)product_config->transition.policy);
    plasma_select_combo_string(dialog, IDC_PLASMA_JOURNEY, product_config->transition.journey_key);
    plasma_select_combo_value(
        dialog,
        IDC_PLASMA_TRANSITION_FALLBACK,
        (LPARAM)product_config->transition.fallback_policy
    );
    plasma_select_combo_value(
        dialog,
        IDC_PLASMA_TRANSITION_SEED_POLICY,
        (LPARAM)product_config->transition.seed_policy
    );
    plasma_set_edit_ulong(dialog, IDC_PLASMA_TRANSITION_INTERVAL, product_config->transition.interval_millis);
    plasma_set_edit_ulong(dialog, IDC_PLASMA_TRANSITION_DURATION, product_config->transition.duration_millis);
    plasma_set_edit_ulong(dialog, IDC_PLASMA_DETERMINISTIC_SEED_VALUE, common_config->deterministic_seed);

    CheckDlgButton(
        dialog,
        IDC_PLASMA_FAVORITES_ONLY,
        product_config->selection.favorites_only ? BST_CHECKED : BST_UNCHECKED
    );
    CheckDlgButton(
        dialog,
        IDC_PLASMA_TRANSITIONS_ENABLED,
        product_config->transition.enabled ? BST_CHECKED : BST_UNCHECKED
    );
    CheckDlgButton(
        dialog,
        IDC_PLASMA_DETERMINISTIC,
        common_config->use_deterministic_seed ? BST_CHECKED : BST_UNCHECKED
    );
    CheckDlgButton(
        dialog,
        IDC_PLASMA_DIAGNOSTICS,
        common_config->diagnostics_overlay_enabled ? BST_CHECKED : BST_UNCHECKED
    );

    plasma_update_dialog_surface(dialog, dialog_state);
}

static void plasma_apply_preset_selection(HWND dialog, plasma_dialog_state *dialog_state)
{
    LRESULT preset_index;
    screensave_common_config saved_common_config;
    plasma_selection_preferences saved_selection;
    plasma_transition_preferences saved_transition;
    int saved_surface;

    if (dialog_state == NULL || dialog_state->module == NULL) {
        return;
    }

    plasma_read_dialog_settings(
        dialog,
        dialog_state->module,
        &dialog_state->working_common_config,
        &dialog_state->working_product_config
    );

    preset_index = SendDlgItemMessageA(dialog, IDC_PLASMA_PRESET, CB_GETCURSEL, 0U, 0L);
    if (preset_index == CB_ERR || (unsigned int)preset_index >= dialog_state->module->preset_count) {
        return;
    }

    saved_common_config = dialog_state->working_common_config;
    saved_selection = dialog_state->working_product_config.selection;
    saved_transition = dialog_state->working_product_config.transition;
    saved_surface = dialog_state->working_product_config.settings_surface;

    plasma_config_set_defaults(
        &dialog_state->working_common_config,
        &dialog_state->working_product_config,
        sizeof(dialog_state->working_product_config)
    );
    plasma_apply_preset_bundle_to_config(
        dialog_state->module->presets[preset_index].preset_key,
        &dialog_state->working_common_config,
        &dialog_state->working_product_config
    );
    dialog_state->working_product_config.settings_surface = saved_surface;
    dialog_state->working_product_config.selection = saved_selection;
    dialog_state->working_product_config.transition = saved_transition;
    dialog_state->working_common_config.use_deterministic_seed = saved_common_config.use_deterministic_seed;
    dialog_state->working_common_config.deterministic_seed = saved_common_config.deterministic_seed;
    dialog_state->working_common_config.diagnostics_overlay_enabled =
        saved_common_config.diagnostics_overlay_enabled;
    dialog_state->working_common_config.randomization_mode = saved_common_config.randomization_mode;
    dialog_state->working_common_config.randomization_scope = saved_common_config.randomization_scope;

    plasma_apply_settings_to_dialog(dialog, dialog_state);
}

static void plasma_refresh_dialog_from_controls(HWND dialog, plasma_dialog_state *dialog_state)
{
    if (dialog_state == NULL) {
        return;
    }

    plasma_read_dialog_settings(
        dialog,
        dialog_state->module,
        &dialog_state->working_common_config,
        &dialog_state->working_product_config
    );
    plasma_apply_settings_to_dialog(dialog, dialog_state);
}

static void plasma_initialize_dialog(HWND dialog, plasma_dialog_state *dialog_state)
{
    if (dialog_state == NULL) {
        return;
    }

    dialog_state->working_common_config = *dialog_state->common_config;
    dialog_state->working_product_config = *dialog_state->product_config;
    plasma_populate_dialog_lists(dialog, dialog_state->module);
    plasma_apply_settings_to_dialog(dialog, dialog_state);
}

static INT_PTR CALLBACK plasma_config_dialog_proc(HWND dialog, UINT message, WPARAM wParam, LPARAM lParam)
{
    plasma_dialog_state *dialog_state;

    dialog_state = (plasma_dialog_state *)GetWindowLongPtrA(dialog, DWLP_USER);

    switch (message) {
    case WM_INITDIALOG:
        dialog_state = (plasma_dialog_state *)lParam;
        SetWindowLongPtrA(dialog, DWLP_USER, (LONG_PTR)dialog_state);
        if (dialog_state != NULL) {
            plasma_initialize_dialog(dialog, dialog_state);
        }
        return TRUE;

    case WM_COMMAND:
        if (dialog_state == NULL) {
            return FALSE;
        }

        if (LOWORD(wParam) == IDC_PLASMA_PRESET && HIWORD(wParam) == CBN_SELCHANGE) {
            plasma_apply_preset_selection(dialog, dialog_state);
            return TRUE;
        }

        if (
            (LOWORD(wParam) == IDC_PLASMA_SURFACE && HIWORD(wParam) == CBN_SELCHANGE) ||
            (LOWORD(wParam) == IDC_PLASMA_THEME && HIWORD(wParam) == CBN_SELCHANGE) ||
            (LOWORD(wParam) == IDC_PLASMA_SPEED && HIWORD(wParam) == CBN_SELCHANGE) ||
            (LOWORD(wParam) == IDC_PLASMA_DETAIL_LEVEL && HIWORD(wParam) == CBN_SELCHANGE) ||
            (LOWORD(wParam) == IDC_PLASMA_EFFECT && HIWORD(wParam) == CBN_SELCHANGE) ||
            (LOWORD(wParam) == IDC_PLASMA_OUTPUT_FAMILY && HIWORD(wParam) == CBN_SELCHANGE) ||
            (LOWORD(wParam) == IDC_PLASMA_OUTPUT_MODE && HIWORD(wParam) == CBN_SELCHANGE) ||
            (LOWORD(wParam) == IDC_PLASMA_RESOLUTION && HIWORD(wParam) == CBN_SELCHANGE) ||
            (LOWORD(wParam) == IDC_PLASMA_SMOOTHING && HIWORD(wParam) == CBN_SELCHANGE) ||
            (LOWORD(wParam) == IDC_PLASMA_FILTER_TREATMENT && HIWORD(wParam) == CBN_SELCHANGE) ||
            (LOWORD(wParam) == IDC_PLASMA_EMULATION_TREATMENT && HIWORD(wParam) == CBN_SELCHANGE) ||
            (LOWORD(wParam) == IDC_PLASMA_ACCENT_TREATMENT && HIWORD(wParam) == CBN_SELCHANGE) ||
            (LOWORD(wParam) == IDC_PLASMA_PRESENTATION_MODE && HIWORD(wParam) == CBN_SELCHANGE) ||
            (LOWORD(wParam) == IDC_PLASMA_CONTENT_FILTER && HIWORD(wParam) == CBN_SELCHANGE) ||
            (LOWORD(wParam) == IDC_PLASMA_PRESET_SET && HIWORD(wParam) == CBN_SELCHANGE) ||
            (LOWORD(wParam) == IDC_PLASMA_THEME_SET && HIWORD(wParam) == CBN_SELCHANGE) ||
            (LOWORD(wParam) == IDC_PLASMA_TRANSITION_POLICY && HIWORD(wParam) == CBN_SELCHANGE) ||
            (LOWORD(wParam) == IDC_PLASMA_JOURNEY && HIWORD(wParam) == CBN_SELCHANGE) ||
            (LOWORD(wParam) == IDC_PLASMA_TRANSITION_FALLBACK && HIWORD(wParam) == CBN_SELCHANGE) ||
            (LOWORD(wParam) == IDC_PLASMA_TRANSITION_SEED_POLICY && HIWORD(wParam) == CBN_SELCHANGE) ||
            (LOWORD(wParam) == IDC_PLASMA_FAVORITES_ONLY && HIWORD(wParam) == BN_CLICKED) ||
            (LOWORD(wParam) == IDC_PLASMA_TRANSITIONS_ENABLED && HIWORD(wParam) == BN_CLICKED) ||
            (LOWORD(wParam) == IDC_PLASMA_DETERMINISTIC && HIWORD(wParam) == BN_CLICKED) ||
            (LOWORD(wParam) == IDC_PLASMA_DIAGNOSTICS && HIWORD(wParam) == BN_CLICKED)
        ) {
            plasma_refresh_dialog_from_controls(dialog, dialog_state);
            return TRUE;
        }

        if (LOWORD(wParam) == IDC_PLASMA_DEFAULTS) {
            int saved_surface;

            saved_surface = plasma_get_combo_value(
                dialog,
                IDC_PLASMA_SURFACE,
                dialog_state->working_product_config.settings_surface
            );
            plasma_config_set_defaults(
                &dialog_state->working_common_config,
                &dialog_state->working_product_config,
                sizeof(dialog_state->working_product_config)
            );
            dialog_state->working_product_config.settings_surface = saved_surface;
            plasma_apply_settings_to_dialog(dialog, dialog_state);
            return TRUE;
        }

        if (LOWORD(wParam) == IDOK) {
            plasma_read_dialog_settings(
                dialog,
                dialog_state->module,
                &dialog_state->working_common_config,
                &dialog_state->working_product_config
            );
            plasma_config_clamp(
                &dialog_state->working_common_config,
                &dialog_state->working_product_config,
                sizeof(dialog_state->working_product_config)
            );
            *dialog_state->common_config = dialog_state->working_common_config;
            *dialog_state->product_config = dialog_state->working_product_config;
            EndDialog(dialog, IDOK);
            return TRUE;
        }

        if (LOWORD(wParam) == IDCANCEL) {
            EndDialog(dialog, IDCANCEL);
            return TRUE;
        }
        break;
    }

    return FALSE;
}

INT_PTR plasma_config_show_dialog(
    HINSTANCE instance,
    HWND owner_window,
    const screensave_saver_module *module,
    screensave_common_config *common_config,
    void *product_config,
    unsigned int product_config_size,
    screensave_diag_context *diagnostics
)
{
    INT_PTR result;
    plasma_dialog_state dialog_state;
    plasma_config *config;

    config = plasma_as_config(product_config, product_config_size);
    if (module == NULL || common_config == NULL || config == NULL) {
        plasma_emit_config_diag(
            diagnostics,
            SCREENSAVE_DIAG_LEVEL_ERROR,
            6503UL,
            "The Plasma dialog could not start because its config state was invalid."
        );
        return -1;
    }

    dialog_state.module = module;
    dialog_state.common_config = common_config;
    dialog_state.product_config = config;
    ZeroMemory(&dialog_state.working_common_config, sizeof(dialog_state.working_common_config));
    ZeroMemory(&dialog_state.working_product_config, sizeof(dialog_state.working_product_config));

    result = DialogBoxParamA(
        instance,
        MAKEINTRESOURCEA(IDD_PLASMA_CONFIG),
        owner_window,
        plasma_config_dialog_proc,
        (LPARAM)&dialog_state
    );

    if (result == -1) {
        plasma_emit_config_diag(
            diagnostics,
            SCREENSAVE_DIAG_LEVEL_ERROR,
            6504UL,
            "The Plasma config dialog resource could not be loaded."
        );
    }

    return result;
}

static int plasma_parse_effect_mode(const char *text, int *value_out)
{
    if (text == NULL || value_out == NULL) {
        return 0;
    }
    if (lstrcmpiA(text, "plasma") == 0) {
        *value_out = PLASMA_EFFECT_PLASMA;
        return 1;
    }
    if (lstrcmpiA(text, "fire") == 0) {
        *value_out = PLASMA_EFFECT_FIRE;
        return 1;
    }
    if (lstrcmpiA(text, "interference") == 0) {
        *value_out = PLASMA_EFFECT_INTERFERENCE;
        return 1;
    }
    if (
        lstrcmpiA(text, "chemical") == 0 ||
        lstrcmpiA(text, "chemical_cellular_growth") == 0
    ) {
        *value_out = PLASMA_EFFECT_CHEMICAL;
        return 1;
    }
    if (
        lstrcmpiA(text, "lattice") == 0 ||
        lstrcmpiA(text, "lattice_quasi_crystal") == 0
    ) {
        *value_out = PLASMA_EFFECT_LATTICE;
        return 1;
    }
    if (
        lstrcmpiA(text, "caustic") == 0 ||
        lstrcmpiA(text, "caustic_marbling") == 0
    ) {
        *value_out = PLASMA_EFFECT_CAUSTIC;
        return 1;
    }
    if (
        lstrcmpiA(text, "aurora") == 0 ||
        lstrcmpiA(text, "aurora_curtain_ribbon") == 0
    ) {
        *value_out = PLASMA_EFFECT_AURORA;
        return 1;
    }
    if (
        lstrcmpiA(text, "substrate") == 0 ||
        lstrcmpiA(text, "substrate_vein_coral") == 0
    ) {
        *value_out = PLASMA_EFFECT_SUBSTRATE;
        return 1;
    }
    if (
        lstrcmpiA(text, "arc") == 0 ||
        lstrcmpiA(text, "arc_discharge") == 0
    ) {
        *value_out = PLASMA_EFFECT_ARC;
        return 1;
    }
    return 0;
}

static int plasma_parse_speed_mode(const char *text, int *value_out)
{
    if (text == NULL || value_out == NULL) {
        return 0;
    }
    if (lstrcmpiA(text, "gentle") == 0) {
        *value_out = PLASMA_SPEED_GENTLE;
        return 1;
    }
    if (lstrcmpiA(text, "standard") == 0) {
        *value_out = PLASMA_SPEED_STANDARD;
        return 1;
    }
    if (lstrcmpiA(text, "lively") == 0) {
        *value_out = PLASMA_SPEED_LIVELY;
        return 1;
    }
    return 0;
}

static const char *plasma_output_family_name(plasma_output_family family)
{
    switch (family) {
    case PLASMA_OUTPUT_FAMILY_BANDED:
        return "banded";

    case PLASMA_OUTPUT_FAMILY_CONTOUR:
        return "contour";

    case PLASMA_OUTPUT_FAMILY_GLYPH:
        return "glyph";

    case PLASMA_OUTPUT_FAMILY_RASTER:
    default:
        return "raster";
    }
}

static int plasma_parse_output_family(const char *text, int *value_out)
{
    if (text == NULL || value_out == NULL) {
        return 0;
    }
    if (lstrcmpiA(text, "raster") == 0) {
        *value_out = PLASMA_OUTPUT_FAMILY_RASTER;
        return 1;
    }
    if (lstrcmpiA(text, "banded") == 0) {
        *value_out = PLASMA_OUTPUT_FAMILY_BANDED;
        return 1;
    }
    if (lstrcmpiA(text, "contour") == 0) {
        *value_out = PLASMA_OUTPUT_FAMILY_CONTOUR;
        return 1;
    }
    if (lstrcmpiA(text, "glyph") == 0) {
        *value_out = PLASMA_OUTPUT_FAMILY_GLYPH;
        return 1;
    }

    return 0;
}

static const char *plasma_output_mode_name(plasma_output_mode mode)
{
    switch (mode) {
    case PLASMA_OUTPUT_MODE_POSTERIZED_BANDS:
        return "posterized_bands";

    case PLASMA_OUTPUT_MODE_CONTOUR_ONLY:
        return "contour_only";

    case PLASMA_OUTPUT_MODE_CONTOUR_BANDS:
        return "contour_bands";

    case PLASMA_OUTPUT_MODE_ASCII_GLYPH:
        return "ascii_glyph";

    case PLASMA_OUTPUT_MODE_MATRIX_GLYPH:
        return "matrix_glyph";

    case PLASMA_OUTPUT_MODE_NATIVE_RASTER:
    default:
        return "native_raster";
    }
}

static int plasma_parse_output_mode(const char *text, int *value_out)
{
    if (text == NULL || value_out == NULL) {
        return 0;
    }
    if (lstrcmpiA(text, "native_raster") == 0 || lstrcmpiA(text, "native") == 0) {
        *value_out = PLASMA_OUTPUT_MODE_NATIVE_RASTER;
        return 1;
    }
    if (lstrcmpiA(text, "posterized_bands") == 0 || lstrcmpiA(text, "bands") == 0) {
        *value_out = PLASMA_OUTPUT_MODE_POSTERIZED_BANDS;
        return 1;
    }
    if (lstrcmpiA(text, "contour_only") == 0) {
        *value_out = PLASMA_OUTPUT_MODE_CONTOUR_ONLY;
        return 1;
    }
    if (lstrcmpiA(text, "contour_bands") == 0) {
        *value_out = PLASMA_OUTPUT_MODE_CONTOUR_BANDS;
        return 1;
    }
    if (lstrcmpiA(text, "ascii_glyph") == 0 || lstrcmpiA(text, "ascii") == 0) {
        *value_out = PLASMA_OUTPUT_MODE_ASCII_GLYPH;
        return 1;
    }
    if (lstrcmpiA(text, "matrix_glyph") == 0 || lstrcmpiA(text, "matrix") == 0) {
        *value_out = PLASMA_OUTPUT_MODE_MATRIX_GLYPH;
        return 1;
    }

    return 0;
}

static const char *plasma_sampling_treatment_name(plasma_sampling_treatment treatment)
{
    (void)treatment;
    return "none";
}

static int plasma_parse_sampling_treatment(const char *text, int *value_out)
{
    if (text == NULL || value_out == NULL) {
        return 0;
    }
    if (lstrcmpiA(text, "none") == 0) {
        *value_out = PLASMA_SAMPLING_TREATMENT_NONE;
        return 1;
    }

    return 0;
}

static const char *plasma_filter_treatment_name(plasma_filter_treatment treatment)
{
    switch (treatment) {
    case PLASMA_FILTER_TREATMENT_BLUR:
        return "blur";

    case PLASMA_FILTER_TREATMENT_GLOW_EDGE:
        return "glow_edge";

    case PLASMA_FILTER_TREATMENT_HALFTONE_STIPPLE:
        return "halftone_stipple";

    case PLASMA_FILTER_TREATMENT_EMBOSS_EDGE:
        return "emboss_edge";

    case PLASMA_FILTER_TREATMENT_NONE:
    default:
        return "none";
    }
}

static int plasma_parse_filter_treatment(const char *text, int *value_out)
{
    if (text == NULL || value_out == NULL) {
        return 0;
    }
    if (lstrcmpiA(text, "none") == 0) {
        *value_out = PLASMA_FILTER_TREATMENT_NONE;
        return 1;
    }
    if (lstrcmpiA(text, "blur") == 0) {
        *value_out = PLASMA_FILTER_TREATMENT_BLUR;
        return 1;
    }
    if (lstrcmpiA(text, "glow_edge") == 0) {
        *value_out = PLASMA_FILTER_TREATMENT_GLOW_EDGE;
        return 1;
    }
    if (lstrcmpiA(text, "halftone_stipple") == 0) {
        *value_out = PLASMA_FILTER_TREATMENT_HALFTONE_STIPPLE;
        return 1;
    }
    if (lstrcmpiA(text, "emboss_edge") == 0) {
        *value_out = PLASMA_FILTER_TREATMENT_EMBOSS_EDGE;
        return 1;
    }

    return 0;
}

static const char *plasma_emulation_treatment_name(plasma_emulation_treatment treatment)
{
    switch (treatment) {
    case PLASMA_EMULATION_TREATMENT_PHOSPHOR:
        return "phosphor";

    case PLASMA_EMULATION_TREATMENT_CRT:
        return "crt";

    case PLASMA_EMULATION_TREATMENT_NONE:
    default:
        return "none";
    }
}

static int plasma_parse_emulation_treatment(const char *text, int *value_out)
{
    if (text == NULL || value_out == NULL) {
        return 0;
    }
    if (lstrcmpiA(text, "none") == 0) {
        *value_out = PLASMA_EMULATION_TREATMENT_NONE;
        return 1;
    }
    if (lstrcmpiA(text, "phosphor") == 0) {
        *value_out = PLASMA_EMULATION_TREATMENT_PHOSPHOR;
        return 1;
    }
    if (lstrcmpiA(text, "crt") == 0) {
        *value_out = PLASMA_EMULATION_TREATMENT_CRT;
        return 1;
    }

    return 0;
}

static const char *plasma_accent_treatment_name(plasma_accent_treatment treatment)
{
    switch (treatment) {
    case PLASMA_ACCENT_TREATMENT_OVERLAY_PASS:
        return "overlay_pass";

    case PLASMA_ACCENT_TREATMENT_ACCENT_PASS:
        return "accent_pass";

    case PLASMA_ACCENT_TREATMENT_NONE:
    default:
        return "none";
    }
}

static int plasma_parse_accent_treatment(const char *text, int *value_out)
{
    if (text == NULL || value_out == NULL) {
        return 0;
    }
    if (lstrcmpiA(text, "none") == 0) {
        *value_out = PLASMA_ACCENT_TREATMENT_NONE;
        return 1;
    }
    if (lstrcmpiA(text, "overlay_pass") == 0) {
        *value_out = PLASMA_ACCENT_TREATMENT_OVERLAY_PASS;
        return 1;
    }
    if (lstrcmpiA(text, "accent_pass") == 0) {
        *value_out = PLASMA_ACCENT_TREATMENT_ACCENT_PASS;
        return 1;
    }

    return 0;
}

static const char *plasma_presentation_mode_name(plasma_presentation_mode mode)
{
    switch (mode) {
    case PLASMA_PRESENTATION_MODE_HEIGHTFIELD:
        return "heightfield";

    case PLASMA_PRESENTATION_MODE_CURTAIN:
        return "curtain";

    case PLASMA_PRESENTATION_MODE_RIBBON:
        return "ribbon";

    case PLASMA_PRESENTATION_MODE_CONTOUR_EXTRUSION:
        return "contour_extrusion";

    case PLASMA_PRESENTATION_MODE_BOUNDED_SURFACE:
        return "bounded_surface";

    case PLASMA_PRESENTATION_MODE_FLAT:
    default:
        return "flat";
    }
}

static int plasma_parse_presentation_mode(const char *text, int *value_out)
{
    if (text == NULL || value_out == NULL) {
        return 0;
    }
    if (lstrcmpiA(text, "flat") == 0) {
        *value_out = PLASMA_PRESENTATION_MODE_FLAT;
        return 1;
    }
    if (lstrcmpiA(text, "heightfield") == 0) {
        *value_out = PLASMA_PRESENTATION_MODE_HEIGHTFIELD;
        return 1;
    }
    if (lstrcmpiA(text, "curtain") == 0) {
        *value_out = PLASMA_PRESENTATION_MODE_CURTAIN;
        return 1;
    }
    if (lstrcmpiA(text, "ribbon") == 0) {
        *value_out = PLASMA_PRESENTATION_MODE_RIBBON;
        return 1;
    }
    if (lstrcmpiA(text, "contour_extrusion") == 0) {
        *value_out = PLASMA_PRESENTATION_MODE_CONTOUR_EXTRUSION;
        return 1;
    }
    if (lstrcmpiA(text, "bounded_surface") == 0) {
        *value_out = PLASMA_PRESENTATION_MODE_BOUNDED_SURFACE;
        return 1;
    }

    return 0;
}

static int plasma_parse_resolution_mode(const char *text, int *value_out)
{
    if (text == NULL || value_out == NULL) {
        return 0;
    }
    if (lstrcmpiA(text, "coarse") == 0) {
        *value_out = PLASMA_RESOLUTION_COARSE;
        return 1;
    }
    if (lstrcmpiA(text, "standard") == 0) {
        *value_out = PLASMA_RESOLUTION_STANDARD;
        return 1;
    }
    if (lstrcmpiA(text, "fine") == 0) {
        *value_out = PLASMA_RESOLUTION_FINE;
        return 1;
    }
    return 0;
}

static int plasma_parse_smoothing_mode(const char *text, int *value_out)
{
    if (text == NULL || value_out == NULL) {
        return 0;
    }
    if (lstrcmpiA(text, "off") == 0) {
        *value_out = PLASMA_SMOOTHING_OFF;
        return 1;
    }
    if (lstrcmpiA(text, "soft") == 0) {
        *value_out = PLASMA_SMOOTHING_SOFT;
        return 1;
    }
    if (lstrcmpiA(text, "glow") == 0) {
        *value_out = PLASMA_SMOOTHING_GLOW;
        return 1;
    }
    return 0;
}

static int plasma_parse_content_filter_mode(const char *text, int *value_out)
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

static int plasma_parse_bool_text(const char *text, int *value_out)
{
    if (text == NULL || value_out == NULL) {
        return 0;
    }
    if (lstrcmpiA(text, "1") == 0 || lstrcmpiA(text, "true") == 0 || lstrcmpiA(text, "yes") == 0) {
        *value_out = 1;
        return 1;
    }
    if (lstrcmpiA(text, "0") == 0 || lstrcmpiA(text, "false") == 0 || lstrcmpiA(text, "no") == 0) {
        *value_out = 0;
        return 1;
    }

    return 0;
}

static int plasma_parse_ulong_text(const char *text, unsigned long *value_out)
{
    char *end;
    unsigned long value;

    if (text == NULL || value_out == NULL) {
        return 0;
    }

    value = strtoul(text, &end, 10);
    if (end == text || (end != NULL && *end != '\0')) {
        return 0;
    }

    *value_out = value;
    return 1;
}

static const char *plasma_format_ulong_text(unsigned long value, char *buffer, unsigned int buffer_size)
{
    if (buffer == NULL || buffer_size < 2U) {
        return "";
    }

    wsprintfA(buffer, "%lu", value);
    buffer[buffer_size - 1U] = '\0';
    return buffer;
}

int plasma_config_export_settings_entries(
    const screensave_saver_module *module,
    const screensave_common_config *common_config,
    const void *product_config,
    unsigned int product_config_size,
    screensave_settings_file_kind kind,
    screensave_settings_writer *writer,
    screensave_diag_context *diagnostics
)
{
    const plasma_config *config;
    char interval_text[32];
    char duration_text[32];

    (void)module;
    (void)common_config;
    (void)diagnostics;

    config = plasma_as_const_config(product_config, product_config_size);
    if (kind != SCREENSAVE_SETTINGS_FILE_PRESET) {
        return 1;
    }
    if (config == NULL || writer == NULL || writer->write_string == NULL) {
        return 0;
    }

    return writer->write_string(writer->context, "product", "effect", plasma_effect_mode_name(config->effect_mode)) &&
        writer->write_string(writer->context, "product", "speed", plasma_speed_mode_name(config->speed_mode)) &&
        writer->write_string(
            writer->context,
            "product",
            "resolution",
            plasma_resolution_mode_name(config->resolution_mode)
        ) &&
        writer->write_string(
            writer->context,
            "product",
            "smoothing",
            plasma_smoothing_mode_name(config->smoothing_mode)
        ) &&
        writer->write_string(
            writer->context,
            "product",
            "output_family",
            plasma_output_family_name(config->output_family)
        ) &&
        writer->write_string(
            writer->context,
            "product",
            "output_mode",
            plasma_output_mode_name(config->output_mode)
        ) &&
        writer->write_string(
            writer->context,
            "product",
            "sampling_treatment",
            plasma_sampling_treatment_name(config->sampling_treatment)
        ) &&
        writer->write_string(
            writer->context,
            "product",
            "filter_treatment",
            plasma_filter_treatment_name(config->filter_treatment)
        ) &&
        writer->write_string(
            writer->context,
            "product",
            "emulation_treatment",
            plasma_emulation_treatment_name(config->emulation_treatment)
        ) &&
        writer->write_string(
            writer->context,
            "product",
            "accent_treatment",
            plasma_accent_treatment_name(config->accent_treatment)
        ) &&
        writer->write_string(
            writer->context,
            "product",
            "presentation_mode",
            plasma_presentation_mode_name(config->presentation_mode)
        ) &&
        writer->write_string(
            writer->context,
            "content",
            "content_filter",
            plasma_selection_content_filter_name(config->selection.content_filter)
        ) &&
        writer->write_string(
            writer->context,
            "content",
            "favorites_only",
            config->selection.favorites_only ? "true" : "false"
        ) &&
        writer->write_string(
            writer->context,
            "content",
            "preset_set_key",
            config->selection.preset_set_key
        ) &&
        writer->write_string(
            writer->context,
            "content",
            "theme_set_key",
            config->selection.theme_set_key
        ) &&
        writer->write_string(
            writer->context,
            "content",
            "favorite_preset_keys",
            config->selection.favorite_preset_keys
        ) &&
        writer->write_string(
            writer->context,
            "content",
            "excluded_preset_keys",
            config->selection.excluded_preset_keys
        ) &&
        writer->write_string(
            writer->context,
            "content",
            "favorite_theme_keys",
            config->selection.favorite_theme_keys
        ) &&
        writer->write_string(
            writer->context,
            "content",
            "excluded_theme_keys",
            config->selection.excluded_theme_keys
        ) &&
        writer->write_string(
            writer->context,
            "transition",
            "enabled",
            config->transition.enabled ? "true" : "false"
        ) &&
        writer->write_string(
            writer->context,
            "transition",
            "policy",
            plasma_transition_policy_name(config->transition.policy)
        ) &&
        writer->write_string(
            writer->context,
            "transition",
            "fallback_policy",
            plasma_transition_fallback_policy_name(config->transition.fallback_policy)
        ) &&
        writer->write_string(
            writer->context,
            "transition",
            "seed_continuity_policy",
            plasma_transition_seed_policy_name(config->transition.seed_policy)
        ) &&
        writer->write_string(
            writer->context,
            "transition",
            "interval_millis",
            plasma_format_ulong_text(
                config->transition.interval_millis,
                interval_text,
                (unsigned int)sizeof(interval_text)
            )
        ) &&
        writer->write_string(
            writer->context,
            "transition",
            "duration_millis",
            plasma_format_ulong_text(
                config->transition.duration_millis,
                duration_text,
                (unsigned int)sizeof(duration_text)
            )
        ) &&
        writer->write_string(
            writer->context,
            "transition",
            "journey_key",
            config->transition.journey_key
        );
}

int plasma_config_import_settings_entry(
    const screensave_saver_module *module,
    screensave_common_config *common_config,
    void *product_config,
    unsigned int product_config_size,
    screensave_settings_file_kind kind,
    const char *section,
    const char *key,
    const char *value,
    screensave_diag_context *diagnostics
)
{
    plasma_config *config;

    (void)module;
    (void)common_config;
    (void)diagnostics;

    config = plasma_as_config(product_config, product_config_size);
    if (kind != SCREENSAVE_SETTINGS_FILE_PRESET) {
        return 1;
    }
    if (config == NULL || section == NULL || key == NULL || value == NULL) {
        return 0;
    }
    if (lstrcmpiA(section, "product") == 0) {
        if (lstrcmpiA(key, "effect_mode") == 0 || lstrcmpiA(key, "effect") == 0) {
            return plasma_parse_effect_mode(value, &config->effect_mode);
        }
        if (lstrcmpiA(key, "speed_mode") == 0 || lstrcmpiA(key, "speed") == 0) {
            return plasma_parse_speed_mode(value, &config->speed_mode);
        }
        if (lstrcmpiA(key, "resolution_mode") == 0 || lstrcmpiA(key, "resolution") == 0) {
            return plasma_parse_resolution_mode(value, &config->resolution_mode);
        }
        if (lstrcmpiA(key, "smoothing_mode") == 0 || lstrcmpiA(key, "smoothing") == 0) {
            return plasma_parse_smoothing_mode(value, &config->smoothing_mode);
        }
        if (lstrcmpiA(key, "output_family") == 0) {
            return plasma_parse_output_family(value, (int *)&config->output_family);
        }
        if (lstrcmpiA(key, "output_mode") == 0) {
            return plasma_parse_output_mode(value, (int *)&config->output_mode);
        }
        if (lstrcmpiA(key, "sampling_treatment") == 0 || lstrcmpiA(key, "sampling") == 0) {
            return plasma_parse_sampling_treatment(value, (int *)&config->sampling_treatment);
        }
        if (lstrcmpiA(key, "filter_treatment") == 0 || lstrcmpiA(key, "filter") == 0) {
            return plasma_parse_filter_treatment(value, (int *)&config->filter_treatment);
        }
        if (lstrcmpiA(key, "emulation_treatment") == 0 || lstrcmpiA(key, "emulation") == 0) {
            return plasma_parse_emulation_treatment(value, (int *)&config->emulation_treatment);
        }
        if (lstrcmpiA(key, "accent_treatment") == 0 || lstrcmpiA(key, "accent") == 0) {
            return plasma_parse_accent_treatment(value, (int *)&config->accent_treatment);
        }
        if (lstrcmpiA(key, "presentation_mode") == 0 || lstrcmpiA(key, "presentation") == 0) {
            return plasma_parse_presentation_mode(value, (int *)&config->presentation_mode);
        }
        return 1;
    }

    if (lstrcmpiA(section, "content") == 0) {
        if (lstrcmpiA(key, "content_filter") == 0) {
            return plasma_parse_content_filter_mode(value, (int *)&config->selection.content_filter);
        }
        if (lstrcmpiA(key, "favorites_only") == 0) {
            return plasma_parse_bool_text(value, &config->selection.favorites_only);
        }
        if (lstrcmpiA(key, "preset_set_key") == 0) {
            lstrcpynA(config->selection.preset_set_key, value, (int)sizeof(config->selection.preset_set_key));
            return 1;
        }
        if (lstrcmpiA(key, "theme_set_key") == 0) {
            lstrcpynA(config->selection.theme_set_key, value, (int)sizeof(config->selection.theme_set_key));
            return 1;
        }
        if (lstrcmpiA(key, "favorite_preset_keys") == 0) {
            lstrcpynA(
                config->selection.favorite_preset_keys,
                value,
                (int)sizeof(config->selection.favorite_preset_keys)
            );
            return 1;
        }
        if (lstrcmpiA(key, "excluded_preset_keys") == 0) {
            lstrcpynA(
                config->selection.excluded_preset_keys,
                value,
                (int)sizeof(config->selection.excluded_preset_keys)
            );
            return 1;
        }
        if (lstrcmpiA(key, "favorite_theme_keys") == 0) {
            lstrcpynA(
                config->selection.favorite_theme_keys,
                value,
                (int)sizeof(config->selection.favorite_theme_keys)
            );
            return 1;
        }
        if (lstrcmpiA(key, "excluded_theme_keys") == 0) {
            lstrcpynA(
                config->selection.excluded_theme_keys,
                value,
                (int)sizeof(config->selection.excluded_theme_keys)
            );
            return 1;
        }
        return 1;
    }

    if (lstrcmpiA(section, "transition") == 0) {
        if (lstrcmpiA(key, "enabled") == 0) {
            return plasma_parse_bool_text(value, &config->transition.enabled);
        }
        if (lstrcmpiA(key, "policy") == 0) {
            return plasma_transition_parse_policy(value, &config->transition.policy);
        }
        if (lstrcmpiA(key, "fallback_policy") == 0) {
            return plasma_transition_parse_fallback_policy(value, &config->transition.fallback_policy);
        }
        if (lstrcmpiA(key, "seed_continuity_policy") == 0) {
            return plasma_transition_parse_seed_policy(value, &config->transition.seed_policy);
        }
        if (lstrcmpiA(key, "interval_millis") == 0) {
            return plasma_parse_ulong_text(value, &config->transition.interval_millis);
        }
        if (lstrcmpiA(key, "duration_millis") == 0) {
            return plasma_parse_ulong_text(value, &config->transition.duration_millis);
        }
        if (lstrcmpiA(key, "journey_key") == 0) {
            lstrcpynA(config->transition.journey_key, value, (int)sizeof(config->transition.journey_key));
            return 1;
        }
        return 1;
    }

    return 1;
}

void plasma_config_randomize_settings(
    const screensave_saver_module *module,
    screensave_common_config *common_config,
    void *product_config,
    unsigned int product_config_size,
    const screensave_session_seed *seed,
    screensave_diag_context *diagnostics
)
{
    plasma_config *config;
    plasma_rng_state rng;
    unsigned long random_seed;
    unsigned long roll;

    (void)module;
    (void)common_config;
    (void)diagnostics;

    config = plasma_as_config(product_config, product_config_size);
    if (config == NULL) {
        return;
    }

    random_seed = seed != NULL ? seed->stream_seed : 0x504C4153UL;
    plasma_rng_seed(&rng, random_seed ^ 0x504C4153UL);
    roll = plasma_rng_range(&rng, 100UL);
    if (roll < 36UL) {
        config->effect_mode = PLASMA_EFFECT_PLASMA;
    } else if (roll < 68UL) {
        config->effect_mode = PLASMA_EFFECT_FIRE;
    } else {
        config->effect_mode = PLASMA_EFFECT_INTERFERENCE;
    }

    roll = plasma_rng_range(&rng, 100UL);
    if (roll < 42UL) {
        config->speed_mode = PLASMA_SPEED_GENTLE;
    } else if (roll < 82UL) {
        config->speed_mode = PLASMA_SPEED_STANDARD;
    } else {
        config->speed_mode = PLASMA_SPEED_LIVELY;
    }

    roll = plasma_rng_range(&rng, 100UL);
    if (roll < 28UL) {
        config->resolution_mode = PLASMA_RESOLUTION_COARSE;
    } else if (roll < 72UL) {
        config->resolution_mode = PLASMA_RESOLUTION_STANDARD;
    } else {
        config->resolution_mode = PLASMA_RESOLUTION_FINE;
    }

    roll = plasma_rng_range(&rng, 100UL);
    if (roll < 20UL) {
        config->smoothing_mode = PLASMA_SMOOTHING_OFF;
    } else if (roll < 70UL) {
        config->smoothing_mode = PLASMA_SMOOTHING_SOFT;
    } else {
        config->smoothing_mode = PLASMA_SMOOTHING_GLOW;
    }
}
