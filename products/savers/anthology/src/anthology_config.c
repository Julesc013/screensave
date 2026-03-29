#include <string.h>

#include "../../../../platform/src/core/base/saver_registry.h"
#include "anthology_internal.h"
#include "anthology_resource.h"
#include "screensave/version.h"

#define ANTHOLOGY_MAX_WEIGHT 1000UL

typedef struct anthology_dialog_state_tag {
    const screensave_saver_module *module;
    screensave_common_config *common_config;
    anthology_config *product_config;
    int current_saver_index;
} anthology_dialog_state;

typedef struct anthology_combo_item_tag {
    unsigned long value;
    const char *display_name;
} anthology_combo_item;

typedef struct anthology_family_control_tag {
    int control_id;
    unsigned long family_flag;
} anthology_family_control;

static const anthology_combo_item g_anthology_selection_mode_items[] = {
    { ANTHOLOGY_SELECTION_RANDOM_UNIFORM, "Random Uniform" },
    { ANTHOLOGY_SELECTION_WEIGHTED_RANDOM, "Weighted Random" },
    { ANTHOLOGY_SELECTION_FAVORITES_FIRST, "Favorites First" }
};

static const anthology_combo_item g_anthology_renderer_policy_items[] = {
    { ANTHOLOGY_RENDERER_POLICY_BASELINE_COMPATIBLE, "Baseline Compatible" },
    { ANTHOLOGY_RENDERER_POLICY_PREFER_EXPLICIT_TIER, "Prefer Explicit Tier" },
    { ANTHOLOGY_RENDERER_POLICY_REQUIRE_EXPLICIT_TIER, "Require Explicit Tier" }
};

static const anthology_family_control g_anthology_family_controls[] = {
    { IDC_ANTHOLOGY_FAMILY_QUIET, ANTHOLOGY_FAMILY_QUIET },
    { IDC_ANTHOLOGY_FAMILY_MOTION, ANTHOLOGY_FAMILY_MOTION },
    { IDC_ANTHOLOGY_FAMILY_VECTOR, ANTHOLOGY_FAMILY_FRAMEBUFFER_VECTOR },
    { IDC_ANTHOLOGY_FAMILY_GRID, ANTHOLOGY_FAMILY_GRID_SIMULATION },
    { IDC_ANTHOLOGY_FAMILY_AMBIENT, ANTHOLOGY_FAMILY_SYSTEMS_AMBIENT },
    { IDC_ANTHOLOGY_FAMILY_PLACES, ANTHOLOGY_FAMILY_PLACES_ATMOSPHERE },
    { IDC_ANTHOLOGY_FAMILY_WORLDS, ANTHOLOGY_FAMILY_HEAVYWEIGHT_WORLDS },
    { IDC_ANTHOLOGY_FAMILY_SHOWCASE, ANTHOLOGY_FAMILY_SHOWCASE }
};

static void anthology_emit_config_diag(
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
        "anthology_config",
        text
    );
}

static anthology_config *anthology_as_config(void *product_config, unsigned int product_config_size)
{
    if (product_config == NULL || product_config_size != sizeof(anthology_config)) {
        return NULL;
    }

    return (anthology_config *)product_config;
}

static const anthology_config *anthology_as_const_config(
    const void *product_config,
    unsigned int product_config_size
)
{
    if (product_config == NULL || product_config_size != sizeof(anthology_config)) {
        return NULL;
    }

    return (const anthology_config *)product_config;
}

static int anthology_text_copy(char *buffer, unsigned int buffer_size, const char *text)
{
    unsigned int text_length;

    if (buffer == NULL || buffer_size == 0U) {
        return 0;
    }

    buffer[0] = '\0';
    if (text == NULL) {
        return 1;
    }

    text_length = (unsigned int)lstrlenA(text);
    if (text_length + 1U > buffer_size) {
        return 0;
    }

    lstrcpyA(buffer, text);
    return 1;
}

static char *anthology_trim_text(char *text)
{
    char *end;

    if (text == NULL) {
        return NULL;
    }

    while (*text != '\0' && (*text == ' ' || *text == '\t' || *text == '\r' || *text == '\n')) {
        ++text;
    }

    end = text + lstrlenA(text);
    while (end > text) {
        if (end[-1] != ' ' && end[-1] != '\t' && end[-1] != '\r' && end[-1] != '\n') {
            break;
        }
        --end;
    }
    *end = '\0';
    return text;
}

static int anthology_parse_unsigned_long(const char *text, unsigned long *value_out)
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

static int anthology_parse_bool(const char *text, int *value_out)
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

static int anthology_build_registry_path(char *buffer, int buffer_size)
{
    if (buffer == NULL || buffer_size <= 0) {
        return 0;
    }

    buffer[0] = '\0';
    return lstrlenA(ANTHOLOGY_PRODUCT_REGISTRY_ROOTA) + 1 <= buffer_size &&
           lstrcpyA(buffer, ANTHOLOGY_PRODUCT_REGISTRY_ROOTA) != NULL;
}

static int anthology_read_flag(HKEY key, const char *value_name, int *value)
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

static int anthology_read_dword(HKEY key, const char *value_name, unsigned long *value)
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

static int anthology_read_string(HKEY key, const char *value_name, char *buffer, DWORD buffer_size)
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

static LONG anthology_write_flag(HKEY key, const char *value_name, int value)
{
    DWORD data;

    data = value ? 1UL : 0UL;
    return RegSetValueExA(key, value_name, 0, REG_DWORD, (const BYTE *)&data, sizeof(data));
}

static LONG anthology_write_dword(HKEY key, const char *value_name, unsigned long value)
{
    DWORD data;

    data = (DWORD)value;
    return RegSetValueExA(key, value_name, 0, REG_DWORD, (const BYTE *)&data, sizeof(data));
}

static LONG anthology_write_string(HKEY key, const char *value_name, const char *value)
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

const char *anthology_selection_mode_name(unsigned long selection_mode)
{
    switch (selection_mode) {
    case ANTHOLOGY_SELECTION_RANDOM_UNIFORM:
        return "random_uniform";

    case ANTHOLOGY_SELECTION_FAVORITES_FIRST:
        return "favorites_first";

    case ANTHOLOGY_SELECTION_WEIGHTED_RANDOM:
    default:
        return "weighted_random";
    }
}

const char *anthology_renderer_policy_name(unsigned long renderer_policy)
{
    switch (renderer_policy) {
    case ANTHOLOGY_RENDERER_POLICY_BASELINE_COMPATIBLE:
        return "baseline_compatible";

    case ANTHOLOGY_RENDERER_POLICY_REQUIRE_EXPLICIT_TIER:
        return "require_explicit_tier";

    case ANTHOLOGY_RENDERER_POLICY_PREFER_EXPLICIT_TIER:
    default:
        return "prefer_explicit_tier";
    }
}

const char *anthology_family_display_name(unsigned long family_flag)
{
    switch (family_flag) {
    case ANTHOLOGY_FAMILY_QUIET:
        return "quiet";

    case ANTHOLOGY_FAMILY_MOTION:
        return "motion";

    case ANTHOLOGY_FAMILY_FRAMEBUFFER_VECTOR:
        return "vector";

    case ANTHOLOGY_FAMILY_GRID_SIMULATION:
        return "grid";

    case ANTHOLOGY_FAMILY_SYSTEMS_AMBIENT:
        return "ambient";

    case ANTHOLOGY_FAMILY_PLACES_ATMOSPHERE:
        return "places";

    case ANTHOLOGY_FAMILY_HEAVYWEIGHT_WORLDS:
        return "worlds";

    case ANTHOLOGY_FAMILY_SHOWCASE:
        return "showcase";

    default:
        return "unknown";
    }
}

static int anthology_parse_selection_mode(const char *text, unsigned long *value_out)
{
    if (text == NULL || value_out == NULL) {
        return 0;
    }

    if (lstrcmpiA(text, "random_uniform") == 0) {
        *value_out = ANTHOLOGY_SELECTION_RANDOM_UNIFORM;
        return 1;
    }
    if (lstrcmpiA(text, "weighted_random") == 0) {
        *value_out = ANTHOLOGY_SELECTION_WEIGHTED_RANDOM;
        return 1;
    }
    if (lstrcmpiA(text, "favorites_first") == 0) {
        *value_out = ANTHOLOGY_SELECTION_FAVORITES_FIRST;
        return 1;
    }

    return 0;
}

static int anthology_parse_renderer_policy(const char *text, unsigned long *value_out)
{
    if (text == NULL || value_out == NULL) {
        return 0;
    }

    if (lstrcmpiA(text, "baseline_compatible") == 0) {
        *value_out = ANTHOLOGY_RENDERER_POLICY_BASELINE_COMPATIBLE;
        return 1;
    }
    if (lstrcmpiA(text, "prefer_explicit_tier") == 0) {
        *value_out = ANTHOLOGY_RENDERER_POLICY_PREFER_EXPLICIT_TIER;
        return 1;
    }
    if (lstrcmpiA(text, "require_explicit_tier") == 0) {
        *value_out = ANTHOLOGY_RENDERER_POLICY_REQUIRE_EXPLICIT_TIER;
        return 1;
    }

    return 0;
}

static int anthology_append_list_token(
    char *buffer,
    unsigned int buffer_size,
    const char *token,
    int *used
)
{
    unsigned int token_length;

    if (buffer == NULL || token == NULL || used == NULL) {
        return 0;
    }

    token_length = (unsigned int)lstrlenA(token);
    if (*used > 0) {
        if ((unsigned int)(*used + 1) >= buffer_size) {
            return 0;
        }
        buffer[*used] = ',';
        buffer[*used + 1] = '\0';
        *used += 1;
    }

    if ((unsigned int)(*used + (int)token_length + 1) > buffer_size) {
        return 0;
    }

    lstrcpyA(buffer + *used, token);
    *used += (int)token_length;
    return 1;
}

static int anthology_family_mask_to_text(char *buffer, unsigned int buffer_size, unsigned long family_mask)
{
    unsigned int index;
    int used;

    if (buffer == NULL || buffer_size == 0U) {
        return 0;
    }

    buffer[0] = '\0';
    family_mask &= anthology_all_family_flags();
    if (family_mask == anthology_all_family_flags()) {
        return anthology_text_copy(buffer, buffer_size, "all");
    }
    if (family_mask == 0UL) {
        return anthology_text_copy(buffer, buffer_size, "none");
    }

    used = 0;
    for (index = 0U; index < (unsigned int)(sizeof(g_anthology_family_controls) / sizeof(g_anthology_family_controls[0])); ++index) {
        if ((family_mask & g_anthology_family_controls[index].family_flag) == 0UL) {
            continue;
        }
        if (
            !anthology_append_list_token(
                    buffer,
                    buffer_size,
                    anthology_family_display_name(g_anthology_family_controls[index].family_flag),
                    &used
                )
        ) {
            return 0;
        }
    }

    return used > 0;
}

static int anthology_parse_family_mask(const char *text, unsigned long *family_mask_out)
{
    char buffer[160];
    char *cursor;
    unsigned long family_mask;

    if (
        text == NULL ||
        family_mask_out == NULL ||
        !anthology_text_copy(buffer, sizeof(buffer), text)
    ) {
        return 0;
    }

    cursor = anthology_trim_text(buffer);
    if (cursor == NULL || cursor[0] == '\0' || lstrcmpiA(cursor, "all") == 0) {
        *family_mask_out = anthology_all_family_flags();
        return 1;
    }
    if (lstrcmpiA(cursor, "none") == 0) {
        *family_mask_out = 0UL;
        return 1;
    }

    family_mask = 0UL;
    while (*cursor != '\0') {
        char *token;

        token = cursor;
        while (*cursor != '\0' && *cursor != ',') {
            ++cursor;
        }
        if (*cursor == ',') {
            *cursor = '\0';
            ++cursor;
        }

        token = anthology_trim_text(token);
        if (token[0] == '\0') {
            continue;
        }
        if (lstrcmpiA(token, "quiet") == 0) {
            family_mask |= ANTHOLOGY_FAMILY_QUIET;
        } else if (lstrcmpiA(token, "motion") == 0) {
            family_mask |= ANTHOLOGY_FAMILY_MOTION;
        } else if (lstrcmpiA(token, "vector") == 0) {
            family_mask |= ANTHOLOGY_FAMILY_FRAMEBUFFER_VECTOR;
        } else if (lstrcmpiA(token, "grid") == 0) {
            family_mask |= ANTHOLOGY_FAMILY_GRID_SIMULATION;
        } else if (lstrcmpiA(token, "ambient") == 0) {
            family_mask |= ANTHOLOGY_FAMILY_SYSTEMS_AMBIENT;
        } else if (lstrcmpiA(token, "places") == 0) {
            family_mask |= ANTHOLOGY_FAMILY_PLACES_ATMOSPHERE;
        } else if (lstrcmpiA(token, "worlds") == 0) {
            family_mask |= ANTHOLOGY_FAMILY_HEAVYWEIGHT_WORLDS;
        } else if (lstrcmpiA(token, "showcase") == 0) {
            family_mask |= ANTHOLOGY_FAMILY_SHOWCASE;
        } else {
            return 0;
        }
    }

    *family_mask_out = family_mask;
    return 1;
}

static int anthology_saver_mask_to_text(char *buffer, unsigned int buffer_size, unsigned long saver_mask)
{
    unsigned int index;
    int used;

    if (buffer == NULL || buffer_size == 0U) {
        return 0;
    }

    buffer[0] = '\0';
    saver_mask &= anthology_all_saver_bits();
    if (saver_mask == anthology_all_saver_bits()) {
        return anthology_text_copy(buffer, buffer_size, "all");
    }
    if (saver_mask == 0UL) {
        return anthology_text_copy(buffer, buffer_size, "none");
    }

    used = 0;
    for (index = 0U; index < ANTHOLOGY_INNER_SAVER_COUNT; ++index) {
        if ((saver_mask & (1UL << index)) == 0UL) {
            continue;
        }
        if (
            !anthology_append_list_token(
                    buffer,
                    buffer_size,
                    anthology_catalog_get_product_key(index),
                    &used
                )
        ) {
            return 0;
        }
    }

    return used > 0;
}

static int anthology_parse_saver_mask(const char *text, unsigned long *saver_mask_out)
{
    char buffer[512];
    char *cursor;
    unsigned long saver_mask;

    if (
        text == NULL ||
        saver_mask_out == NULL ||
        !anthology_text_copy(buffer, sizeof(buffer), text)
    ) {
        return 0;
    }

    cursor = anthology_trim_text(buffer);
    if (cursor == NULL || cursor[0] == '\0' || lstrcmpiA(cursor, "all") == 0) {
        *saver_mask_out = anthology_all_saver_bits();
        return 1;
    }
    if (lstrcmpiA(cursor, "none") == 0) {
        *saver_mask_out = 0UL;
        return 1;
    }

    saver_mask = 0UL;
    while (*cursor != '\0') {
        char *token;
        const char *canonical_key;
        int index;

        token = cursor;
        while (*cursor != '\0' && *cursor != ',') {
            ++cursor;
        }
        if (*cursor == ',') {
            *cursor = '\0';
            ++cursor;
        }

        token = anthology_trim_text(token);
        if (token[0] == '\0') {
            continue;
        }

        canonical_key = screensave_saver_registry_canonical_product_key(token);
        if (canonical_key == NULL || lstrcmpiA(canonical_key, "anthology") == 0) {
            continue;
        }

        index = anthology_catalog_find_index(canonical_key);
        if (index < 0) {
            continue;
        }

        saver_mask |= (1UL << index);
    }

    *saver_mask_out = saver_mask;
    return 1;
}

void anthology_config_set_defaults(
    screensave_common_config *common_config,
    void *product_config,
    unsigned int product_config_size
)
{
    anthology_config *config;

    config = anthology_as_config(product_config, product_config_size);
    if (common_config == NULL || config == NULL) {
        return;
    }

    screensave_common_config_set_defaults(common_config);
    anthology_apply_preset_to_config(ANTHOLOGY_DEFAULT_PRESET_KEY, common_config, config);
}

void anthology_config_clamp(
    screensave_common_config *common_config,
    void *product_config,
    unsigned int product_config_size
)
{
    anthology_config *config;
    unsigned int index;

    config = anthology_as_config(product_config, product_config_size);
    if (common_config == NULL || config == NULL) {
        return;
    }

    screensave_common_config_clamp(common_config);
    if (common_config->preset_key == NULL || anthology_find_preset_values(common_config->preset_key) == NULL) {
        common_config->preset_key = ANTHOLOGY_DEFAULT_PRESET_KEY;
    }
    if (common_config->theme_key == NULL || anthology_find_theme_descriptor(common_config->theme_key) == NULL) {
        common_config->theme_key = ANTHOLOGY_DEFAULT_THEME_KEY;
    }

    if (config->selection_mode > ANTHOLOGY_SELECTION_FAVORITES_FIRST) {
        config->selection_mode = ANTHOLOGY_SELECTION_WEIGHTED_RANDOM;
    }
    if (config->renderer_policy > ANTHOLOGY_RENDERER_POLICY_REQUIRE_EXPLICIT_TIER) {
        config->renderer_policy = ANTHOLOGY_RENDERER_POLICY_PREFER_EXPLICIT_TIER;
    }
    if (config->interval_seconds < ANTHOLOGY_MIN_INTERVAL_SECONDS) {
        config->interval_seconds = ANTHOLOGY_MIN_INTERVAL_SECONDS;
    }
    if (config->interval_seconds > ANTHOLOGY_MAX_INTERVAL_SECONDS) {
        config->interval_seconds = ANTHOLOGY_MAX_INTERVAL_SECONDS;
    }

    config->family_mask &= anthology_all_family_flags();
    if (config->family_mask == 0UL) {
        config->family_mask = anthology_all_family_flags();
    }

    config->include_mask &= anthology_all_saver_bits();
    if (config->include_mask == 0UL) {
        config->include_mask = anthology_all_saver_bits();
    }

    config->favorite_mask &= anthology_all_saver_bits();
    config->favorite_mask &= config->include_mask;
    config->randomize_inner_presets = config->randomize_inner_presets ? 1 : 0;
    config->randomize_inner_themes = config->randomize_inner_themes ? 1 : 0;
    config->respect_inner_randomization = config->respect_inner_randomization ? 1 : 0;

    for (index = 0U; index < ANTHOLOGY_INNER_SAVER_COUNT; ++index) {
        if ((config->include_mask & (1UL << index)) == 0UL) {
            if (config->saver_weights[index] > ANTHOLOGY_MAX_WEIGHT) {
                config->saver_weights[index] = ANTHOLOGY_MAX_WEIGHT;
            }
            continue;
        }

        if (config->saver_weights[index] == 0UL) {
            config->saver_weights[index] = anthology_catalog_get_default_weight(index);
        }
        if (config->saver_weights[index] > ANTHOLOGY_MAX_WEIGHT) {
            config->saver_weights[index] = ANTHOLOGY_MAX_WEIGHT;
        }
    }
}

int anthology_config_load(
    const screensave_saver_module *module,
    screensave_common_config *common_config,
    void *product_config,
    unsigned int product_config_size,
    screensave_diag_context *diagnostics
)
{
    anthology_config *config;
    HKEY key;
    char path[260];
    char preset_key[64];
    char theme_key[64];
    char weight_name[64];
    unsigned int index;
    unsigned long value_dword;

    (void)module;
    (void)diagnostics;

    config = anthology_as_config(product_config, product_config_size);
    if (common_config == NULL || config == NULL) {
        return 0;
    }

    anthology_config_set_defaults(common_config, product_config, product_config_size);
    if (!anthology_build_registry_path(path, sizeof(path))) {
        return 0;
    }

    if (RegOpenKeyExA(HKEY_CURRENT_USER, path, 0, KEY_QUERY_VALUE, &key) != ERROR_SUCCESS) {
        anthology_config_clamp(common_config, product_config, product_config_size);
        return 1;
    }

    preset_key[0] = '\0';
    if (anthology_read_string(key, "PresetKey", preset_key, sizeof(preset_key))) {
        anthology_apply_preset_to_config(preset_key, common_config, config);
    }

    if (anthology_read_string(key, "ThemeKey", theme_key, sizeof(theme_key))) {
        if (anthology_find_theme_descriptor(theme_key) != NULL) {
            common_config->theme_key = anthology_find_theme_descriptor(theme_key)->theme_key;
        }
    }

    value_dword = (unsigned long)common_config->detail_level;
    if (anthology_read_dword(key, "DetailLevel", &value_dword)) {
        common_config->detail_level = (screensave_detail_level)value_dword;
    }
    value_dword = common_config->deterministic_seed;
    (void)anthology_read_flag(key, "UseDeterministicSeed", &common_config->use_deterministic_seed);
    if (anthology_read_dword(key, "DeterministicSeed", &value_dword)) {
        common_config->deterministic_seed = value_dword;
    }
    (void)anthology_read_flag(key, "DiagnosticsOverlayEnabled", &common_config->diagnostics_overlay_enabled);

    value_dword = config->selection_mode;
    if (anthology_read_dword(key, "SelectionMode", &value_dword)) {
        config->selection_mode = value_dword;
    }
    value_dword = config->interval_seconds;
    if (anthology_read_dword(key, "IntervalSeconds", &value_dword)) {
        config->interval_seconds = value_dword;
    }
    value_dword = config->family_mask;
    if (anthology_read_dword(key, "FamilyMask", &value_dword)) {
        config->family_mask = value_dword;
    }
    value_dword = config->include_mask;
    if (anthology_read_dword(key, "IncludeMask", &value_dword)) {
        config->include_mask = value_dword;
    }
    value_dword = config->favorite_mask;
    if (anthology_read_dword(key, "FavoriteMask", &value_dword)) {
        config->favorite_mask = value_dword;
    }
    value_dword = config->renderer_policy;
    if (anthology_read_dword(key, "RendererPolicy", &value_dword)) {
        config->renderer_policy = value_dword;
    }
    (void)anthology_read_flag(key, "RandomizeInnerPresets", &config->randomize_inner_presets);
    (void)anthology_read_flag(key, "RandomizeInnerThemes", &config->randomize_inner_themes);
    (void)anthology_read_flag(key, "RespectInnerRandomization", &config->respect_inner_randomization);

    for (index = 0U; index < ANTHOLOGY_INNER_SAVER_COUNT; ++index) {
        wsprintfA(weight_name, "Weight_%s", anthology_catalog_get_product_key(index));
        value_dword = config->saver_weights[index];
        if (anthology_read_dword(key, weight_name, &value_dword)) {
            config->saver_weights[index] = value_dword;
        }
    }

    RegCloseKey(key);
    anthology_config_clamp(common_config, product_config, product_config_size);
    return 1;
}

int anthology_config_save(
    const screensave_saver_module *module,
    const screensave_common_config *common_config,
    const void *product_config,
    unsigned int product_config_size,
    screensave_diag_context *diagnostics
)
{
    anthology_config safe_product_config;
    screensave_common_config safe_common_config;
    const anthology_config *config;
    HKEY key;
    DWORD disposition;
    char path[260];
    char weight_name[64];
    unsigned int index;
    LONG result;

    (void)module;

    config = anthology_as_const_config(product_config, product_config_size);
    if (common_config == NULL || config == NULL) {
        return 0;
    }

    safe_common_config = *common_config;
    safe_product_config = *config;
    anthology_config_clamp(&safe_common_config, &safe_product_config, sizeof(safe_product_config));
    if (!anthology_build_registry_path(path, sizeof(path))) {
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
        anthology_emit_config_diag(
            diagnostics,
            SCREENSAVE_DIAG_LEVEL_ERROR,
            6801UL,
            "The Anthology registry key could not be opened for writing."
        );
        return 0;
    }

    result = anthology_write_dword(key, "DetailLevel", (unsigned long)safe_common_config.detail_level);
    if (result == ERROR_SUCCESS) {
        result = anthology_write_flag(key, "DiagnosticsOverlayEnabled", safe_common_config.diagnostics_overlay_enabled);
    }
    if (result == ERROR_SUCCESS) {
        result = anthology_write_flag(key, "UseDeterministicSeed", safe_common_config.use_deterministic_seed);
    }
    if (result == ERROR_SUCCESS) {
        result = anthology_write_dword(key, "DeterministicSeed", safe_common_config.deterministic_seed);
    }
    if (result == ERROR_SUCCESS) {
        result = anthology_write_string(key, "PresetKey", safe_common_config.preset_key);
    }
    if (result == ERROR_SUCCESS) {
        result = anthology_write_string(key, "ThemeKey", safe_common_config.theme_key);
    }
    if (result == ERROR_SUCCESS) {
        result = anthology_write_dword(key, "SelectionMode", safe_product_config.selection_mode);
    }
    if (result == ERROR_SUCCESS) {
        result = anthology_write_dword(key, "IntervalSeconds", safe_product_config.interval_seconds);
    }
    if (result == ERROR_SUCCESS) {
        result = anthology_write_dword(key, "FamilyMask", safe_product_config.family_mask);
    }
    if (result == ERROR_SUCCESS) {
        result = anthology_write_dword(key, "IncludeMask", safe_product_config.include_mask);
    }
    if (result == ERROR_SUCCESS) {
        result = anthology_write_dword(key, "FavoriteMask", safe_product_config.favorite_mask);
    }
    if (result == ERROR_SUCCESS) {
        result = anthology_write_dword(key, "RendererPolicy", safe_product_config.renderer_policy);
    }
    if (result == ERROR_SUCCESS) {
        result = anthology_write_flag(key, "RandomizeInnerPresets", safe_product_config.randomize_inner_presets);
    }
    if (result == ERROR_SUCCESS) {
        result = anthology_write_flag(key, "RandomizeInnerThemes", safe_product_config.randomize_inner_themes);
    }
    if (result == ERROR_SUCCESS) {
        result = anthology_write_flag(
            key,
            "RespectInnerRandomization",
            safe_product_config.respect_inner_randomization
        );
    }

    for (index = 0U; result == ERROR_SUCCESS && index < ANTHOLOGY_INNER_SAVER_COUNT; ++index) {
        wsprintfA(weight_name, "Weight_%s", anthology_catalog_get_product_key(index));
        result = anthology_write_dword(key, weight_name, safe_product_config.saver_weights[index]);
    }

    RegCloseKey(key);
    if (result != ERROR_SUCCESS) {
        anthology_emit_config_diag(
            diagnostics,
            SCREENSAVE_DIAG_LEVEL_ERROR,
            6802UL,
            "The Anthology registry values could not be saved."
        );
        return 0;
    }

    return 1;
}

static LRESULT anthology_add_combo_item(HWND dialog, int control_id, const char *text, LPARAM item_data)
{
    LRESULT index;

    index = SendDlgItemMessageA(dialog, control_id, CB_ADDSTRING, 0U, (LPARAM)text);
    if (index >= 0L) {
        SendDlgItemMessageA(dialog, control_id, CB_SETITEMDATA, (WPARAM)index, item_data);
    }

    return index;
}

static void anthology_select_combo_value(HWND dialog, int control_id, LPARAM item_data)
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

static unsigned long anthology_get_combo_value(HWND dialog, int control_id, unsigned long default_value)
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

    return (unsigned long)item_data;
}

static void anthology_set_family_checkboxes(HWND dialog, unsigned long family_mask)
{
    unsigned int index;

    for (index = 0U; index < (unsigned int)(sizeof(g_anthology_family_controls) / sizeof(g_anthology_family_controls[0])); ++index) {
        CheckDlgButton(
            dialog,
            g_anthology_family_controls[index].control_id,
            (family_mask & g_anthology_family_controls[index].family_flag) != 0UL ? BST_CHECKED : BST_UNCHECKED
        );
    }
}

static unsigned long anthology_get_family_mask_from_dialog(HWND dialog)
{
    unsigned int index;
    unsigned long family_mask;

    family_mask = 0UL;
    for (index = 0U; index < (unsigned int)(sizeof(g_anthology_family_controls) / sizeof(g_anthology_family_controls[0])); ++index) {
        if (IsDlgButtonChecked(dialog, g_anthology_family_controls[index].control_id) == BST_CHECKED) {
            family_mask |= g_anthology_family_controls[index].family_flag;
        }
    }

    return family_mask;
}

static void anthology_format_saver_list_item(
    char *buffer,
    unsigned int buffer_size,
    const anthology_config *config,
    unsigned int saver_index
)
{
    const char *display_name;
    const char *enabled_text;
    const char *favorite_text;
    unsigned long weight;

    if (buffer == NULL || buffer_size == 0U || config == NULL) {
        return;
    }

    display_name = anthology_catalog_get_display_name(saver_index);
    enabled_text = (config->include_mask & (1UL << saver_index)) != 0UL ? "on" : "off";
    favorite_text = (config->favorite_mask & (1UL << saver_index)) != 0UL ? " fav" : "";
    weight = config->saver_weights[saver_index];
    wsprintfA(buffer, "%s [%s%s] w=%lu", display_name, enabled_text, favorite_text, weight);
}

static void anthology_refresh_saver_list(HWND dialog, anthology_dialog_state *dialog_state)
{
    unsigned int index;
    int selection;
    char item_text[128];

    if (dialog_state == NULL || dialog_state->product_config == NULL) {
        return;
    }

    selection = dialog_state->current_saver_index;
    if (selection < 0 || selection >= (int)ANTHOLOGY_INNER_SAVER_COUNT) {
        selection = 0;
    }

    SendDlgItemMessageA(dialog, IDC_ANTHOLOGY_SAVER_LIST, LB_RESETCONTENT, 0U, 0L);
    for (index = 0U; index < ANTHOLOGY_INNER_SAVER_COUNT; ++index) {
        LRESULT item_index;

        anthology_format_saver_list_item(item_text, sizeof(item_text), dialog_state->product_config, index);
        item_index = SendDlgItemMessageA(
            dialog,
            IDC_ANTHOLOGY_SAVER_LIST,
            LB_ADDSTRING,
            0U,
            (LPARAM)item_text
        );
        if (item_index >= 0L) {
            SendDlgItemMessageA(
                dialog,
                IDC_ANTHOLOGY_SAVER_LIST,
                LB_SETITEMDATA,
                (WPARAM)item_index,
                (LPARAM)index
            );
        }
    }

    SendDlgItemMessageA(dialog, IDC_ANTHOLOGY_SAVER_LIST, LB_SETCURSEL, (WPARAM)selection, 0L);
    dialog_state->current_saver_index = selection;
}

static int anthology_get_selected_saver_index(HWND dialog)
{
    LRESULT selection;
    LRESULT item_data;

    selection = SendDlgItemMessageA(dialog, IDC_ANTHOLOGY_SAVER_LIST, LB_GETCURSEL, 0U, 0L);
    if (selection == LB_ERR) {
        return -1;
    }

    item_data = SendDlgItemMessageA(dialog, IDC_ANTHOLOGY_SAVER_LIST, LB_GETITEMDATA, (WPARAM)selection, 0L);
    if (item_data == LB_ERR) {
        return -1;
    }

    return (int)item_data;
}

static void anthology_apply_selected_saver_to_dialog(HWND dialog, anthology_dialog_state *dialog_state)
{
    int saver_index;
    char weight_text[32];

    if (dialog_state == NULL || dialog_state->product_config == NULL) {
        return;
    }

    saver_index = anthology_get_selected_saver_index(dialog);
    if (saver_index < 0) {
        saver_index = 0;
        SendDlgItemMessageA(dialog, IDC_ANTHOLOGY_SAVER_LIST, LB_SETCURSEL, 0U, 0L);
    }

    dialog_state->current_saver_index = saver_index;
    CheckDlgButton(
        dialog,
        IDC_ANTHOLOGY_SAVER_ENABLED,
        (dialog_state->product_config->include_mask & (1UL << saver_index)) != 0UL ? BST_CHECKED : BST_UNCHECKED
    );
    CheckDlgButton(
        dialog,
        IDC_ANTHOLOGY_SAVER_FAVORITE,
        (dialog_state->product_config->favorite_mask & (1UL << saver_index)) != 0UL ? BST_CHECKED : BST_UNCHECKED
    );
    wsprintfA(weight_text, "%lu", dialog_state->product_config->saver_weights[saver_index]);
    SetDlgItemTextA(dialog, IDC_ANTHOLOGY_WEIGHT, weight_text);
}

static void anthology_commit_selected_saver_controls(HWND dialog, anthology_dialog_state *dialog_state)
{
    int saver_index;
    char weight_text[32];
    unsigned long weight_value;

    if (dialog_state == NULL || dialog_state->product_config == NULL) {
        return;
    }

    saver_index = dialog_state->current_saver_index;
    if (saver_index < 0 || saver_index >= (int)ANTHOLOGY_INNER_SAVER_COUNT) {
        return;
    }

    if (IsDlgButtonChecked(dialog, IDC_ANTHOLOGY_SAVER_ENABLED) == BST_CHECKED) {
        dialog_state->product_config->include_mask |= (1UL << saver_index);
    } else {
        dialog_state->product_config->include_mask &= ~(1UL << saver_index);
        dialog_state->product_config->favorite_mask &= ~(1UL << saver_index);
    }

    if (IsDlgButtonChecked(dialog, IDC_ANTHOLOGY_SAVER_FAVORITE) == BST_CHECKED) {
        dialog_state->product_config->favorite_mask |= (1UL << saver_index);
    } else {
        dialog_state->product_config->favorite_mask &= ~(1UL << saver_index);
    }

    GetDlgItemTextA(dialog, IDC_ANTHOLOGY_WEIGHT, weight_text, sizeof(weight_text));
    weight_value = dialog_state->product_config->saver_weights[saver_index];
    if (anthology_parse_unsigned_long(weight_text, &weight_value)) {
        dialog_state->product_config->saver_weights[saver_index] = weight_value;
    }

    anthology_config_clamp(
        dialog_state->common_config,
        dialog_state->product_config,
        sizeof(*dialog_state->product_config)
    );
    anthology_refresh_saver_list(dialog, dialog_state);
    anthology_apply_selected_saver_to_dialog(dialog, dialog_state);
}

static void anthology_populate_dialog_lists(HWND dialog, const screensave_saver_module *module)
{
    unsigned int index;

    SendDlgItemMessageA(dialog, IDC_ANTHOLOGY_PRESET, CB_RESETCONTENT, 0U, 0L);
    for (index = 0U; index < module->preset_count; ++index) {
        anthology_add_combo_item(
            dialog,
            IDC_ANTHOLOGY_PRESET,
            module->presets[index].display_name,
            (LPARAM)index
        );
    }

    SendDlgItemMessageA(dialog, IDC_ANTHOLOGY_SELECTION_MODE, CB_RESETCONTENT, 0U, 0L);
    for (index = 0U; index < (unsigned int)(sizeof(g_anthology_selection_mode_items) / sizeof(g_anthology_selection_mode_items[0])); ++index) {
        anthology_add_combo_item(
            dialog,
            IDC_ANTHOLOGY_SELECTION_MODE,
            g_anthology_selection_mode_items[index].display_name,
            (LPARAM)g_anthology_selection_mode_items[index].value
        );
    }

    SendDlgItemMessageA(dialog, IDC_ANTHOLOGY_RENDERER_POLICY, CB_RESETCONTENT, 0U, 0L);
    for (index = 0U; index < (unsigned int)(sizeof(g_anthology_renderer_policy_items) / sizeof(g_anthology_renderer_policy_items[0])); ++index) {
        anthology_add_combo_item(
            dialog,
            IDC_ANTHOLOGY_RENDERER_POLICY,
            g_anthology_renderer_policy_items[index].display_name,
            (LPARAM)g_anthology_renderer_policy_items[index].value
        );
    }
}

static void anthology_apply_settings_to_dialog(HWND dialog, anthology_dialog_state *dialog_state)
{
    unsigned int index;
    char interval_text[32];

    if (dialog_state == NULL || dialog_state->module == NULL) {
        return;
    }

    if (dialog_state->common_config->preset_key != NULL) {
        for (index = 0U; index < dialog_state->module->preset_count; ++index) {
            if (lstrcmpiA(dialog_state->module->presets[index].preset_key, dialog_state->common_config->preset_key) == 0) {
                SendDlgItemMessageA(dialog, IDC_ANTHOLOGY_PRESET, CB_SETCURSEL, (WPARAM)index, 0L);
                break;
            }
        }
    }

    anthology_select_combo_value(dialog, IDC_ANTHOLOGY_SELECTION_MODE, (LPARAM)dialog_state->product_config->selection_mode);
    anthology_select_combo_value(dialog, IDC_ANTHOLOGY_RENDERER_POLICY, (LPARAM)dialog_state->product_config->renderer_policy);
    wsprintfA(interval_text, "%lu", dialog_state->product_config->interval_seconds);
    SetDlgItemTextA(dialog, IDC_ANTHOLOGY_INTERVAL, interval_text);

    anthology_set_family_checkboxes(dialog, dialog_state->product_config->family_mask);
    CheckDlgButton(
        dialog,
        IDC_ANTHOLOGY_RANDOMIZE_PRESETS,
        dialog_state->product_config->randomize_inner_presets ? BST_CHECKED : BST_UNCHECKED
    );
    CheckDlgButton(
        dialog,
        IDC_ANTHOLOGY_RANDOMIZE_THEMES,
        dialog_state->product_config->randomize_inner_themes ? BST_CHECKED : BST_UNCHECKED
    );
    CheckDlgButton(
        dialog,
        IDC_ANTHOLOGY_RESPECT_LOCAL_RANDOMIZATION,
        dialog_state->product_config->respect_inner_randomization ? BST_CHECKED : BST_UNCHECKED
    );
    CheckDlgButton(
        dialog,
        IDC_ANTHOLOGY_DETERMINISTIC,
        dialog_state->common_config->use_deterministic_seed ? BST_CHECKED : BST_UNCHECKED
    );
    CheckDlgButton(
        dialog,
        IDC_ANTHOLOGY_DIAGNOSTICS,
        dialog_state->common_config->diagnostics_overlay_enabled ? BST_CHECKED : BST_UNCHECKED
    );

    anthology_refresh_saver_list(dialog, dialog_state);
    anthology_apply_selected_saver_to_dialog(dialog, dialog_state);
}

static void anthology_apply_preset_selection(HWND dialog, anthology_dialog_state *dialog_state)
{
    LRESULT preset_index;
    int diagnostics_enabled;
    int deterministic_enabled;

    if (dialog_state == NULL || dialog_state->module == NULL) {
        return;
    }

    preset_index = SendDlgItemMessageA(dialog, IDC_ANTHOLOGY_PRESET, CB_GETCURSEL, 0U, 0L);
    if (preset_index == CB_ERR || (unsigned int)preset_index >= dialog_state->module->preset_count) {
        return;
    }

    diagnostics_enabled = IsDlgButtonChecked(dialog, IDC_ANTHOLOGY_DIAGNOSTICS) == BST_CHECKED;
    deterministic_enabled = IsDlgButtonChecked(dialog, IDC_ANTHOLOGY_DETERMINISTIC) == BST_CHECKED;

    anthology_config_set_defaults(
        dialog_state->common_config,
        dialog_state->product_config,
        sizeof(*dialog_state->product_config)
    );
    anthology_apply_preset_to_config(
        dialog_state->module->presets[preset_index].preset_key,
        dialog_state->common_config,
        dialog_state->product_config
    );
    dialog_state->common_config->diagnostics_overlay_enabled = diagnostics_enabled;
    dialog_state->common_config->use_deterministic_seed = deterministic_enabled;
    anthology_apply_settings_to_dialog(dialog, dialog_state);
}

static void anthology_read_dialog_settings(HWND dialog, anthology_dialog_state *dialog_state)
{
    LRESULT preset_index;
    char interval_text[32];
    unsigned long parsed_interval;

    if (dialog_state == NULL || dialog_state->common_config == NULL || dialog_state->product_config == NULL) {
        return;
    }

    anthology_commit_selected_saver_controls(dialog, dialog_state);
    preset_index = SendDlgItemMessageA(dialog, IDC_ANTHOLOGY_PRESET, CB_GETCURSEL, 0U, 0L);
    if (preset_index != CB_ERR && (unsigned int)preset_index < dialog_state->module->preset_count) {
        dialog_state->common_config->preset_key = dialog_state->module->presets[preset_index].preset_key;
    }

    dialog_state->common_config->theme_key = ANTHOLOGY_DEFAULT_THEME_KEY;
    dialog_state->product_config->selection_mode = anthology_get_combo_value(
        dialog,
        IDC_ANTHOLOGY_SELECTION_MODE,
        dialog_state->product_config->selection_mode
    );
    dialog_state->product_config->renderer_policy = anthology_get_combo_value(
        dialog,
        IDC_ANTHOLOGY_RENDERER_POLICY,
        dialog_state->product_config->renderer_policy
    );

    GetDlgItemTextA(dialog, IDC_ANTHOLOGY_INTERVAL, interval_text, sizeof(interval_text));
    parsed_interval = dialog_state->product_config->interval_seconds;
    if (anthology_parse_unsigned_long(interval_text, &parsed_interval)) {
        dialog_state->product_config->interval_seconds = parsed_interval;
    }

    dialog_state->product_config->family_mask = anthology_get_family_mask_from_dialog(dialog);
    dialog_state->product_config->randomize_inner_presets =
        IsDlgButtonChecked(dialog, IDC_ANTHOLOGY_RANDOMIZE_PRESETS) == BST_CHECKED;
    dialog_state->product_config->randomize_inner_themes =
        IsDlgButtonChecked(dialog, IDC_ANTHOLOGY_RANDOMIZE_THEMES) == BST_CHECKED;
    dialog_state->product_config->respect_inner_randomization =
        IsDlgButtonChecked(dialog, IDC_ANTHOLOGY_RESPECT_LOCAL_RANDOMIZATION) == BST_CHECKED;
    dialog_state->common_config->use_deterministic_seed =
        IsDlgButtonChecked(dialog, IDC_ANTHOLOGY_DETERMINISTIC) == BST_CHECKED;
    dialog_state->common_config->diagnostics_overlay_enabled =
        IsDlgButtonChecked(dialog, IDC_ANTHOLOGY_DIAGNOSTICS) == BST_CHECKED;

    anthology_config_clamp(
        dialog_state->common_config,
        dialog_state->product_config,
        sizeof(*dialog_state->product_config)
    );
}

static void anthology_initialize_dialog(HWND dialog, anthology_dialog_state *dialog_state)
{
    char info_text[256];
    const screensave_version_info *version_info;

    version_info = screensave_version_get_info();
    anthology_populate_dialog_lists(dialog, dialog_state->module);
    wsprintfA(
        info_text,
        "Anthology\r\n%s\r\nSuite meta-saver across %u inner savers with bounded in-process rotation.",
        version_info->version_text,
        ANTHOLOGY_INNER_SAVER_COUNT
    );
    SetDlgItemTextA(dialog, IDC_ANTHOLOGY_INFO, info_text);
    anthology_apply_settings_to_dialog(dialog, dialog_state);
}

static INT_PTR CALLBACK anthology_config_dialog_proc(HWND dialog, UINT message, WPARAM wParam, LPARAM lParam)
{
    anthology_dialog_state *dialog_state;

    dialog_state = (anthology_dialog_state *)GetWindowLongA(dialog, DWL_USER);

    switch (message) {
    case WM_INITDIALOG:
        dialog_state = (anthology_dialog_state *)lParam;
        SetWindowLongA(dialog, DWL_USER, (LONG)dialog_state);
        if (dialog_state != NULL) {
            anthology_initialize_dialog(dialog, dialog_state);
        }
        return TRUE;

    case WM_COMMAND:
        if (dialog_state == NULL) {
            return FALSE;
        }

        if (LOWORD(wParam) == IDC_ANTHOLOGY_PRESET && HIWORD(wParam) == CBN_SELCHANGE) {
            anthology_apply_preset_selection(dialog, dialog_state);
            return TRUE;
        }

        if (LOWORD(wParam) == IDC_ANTHOLOGY_SAVER_LIST && HIWORD(wParam) == LBN_SELCHANGE) {
            anthology_commit_selected_saver_controls(dialog, dialog_state);
            anthology_apply_selected_saver_to_dialog(dialog, dialog_state);
            return TRUE;
        }

        if (LOWORD(wParam) == IDC_ANTHOLOGY_SAVER_ENABLED || LOWORD(wParam) == IDC_ANTHOLOGY_SAVER_FAVORITE) {
            anthology_commit_selected_saver_controls(dialog, dialog_state);
            return TRUE;
        }

        if (LOWORD(wParam) == IDC_ANTHOLOGY_WEIGHT && HIWORD(wParam) == EN_CHANGE) {
            anthology_commit_selected_saver_controls(dialog, dialog_state);
            return TRUE;
        }

        if (LOWORD(wParam) == IDC_ANTHOLOGY_DEFAULTS) {
            anthology_config_set_defaults(
                dialog_state->common_config,
                dialog_state->product_config,
                sizeof(*dialog_state->product_config)
            );
            anthology_apply_settings_to_dialog(dialog, dialog_state);
            return TRUE;
        }

        if (LOWORD(wParam) == IDOK) {
            anthology_read_dialog_settings(dialog, dialog_state);
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

INT_PTR anthology_config_show_dialog(
    HINSTANCE instance,
    HWND owner_window,
    const screensave_saver_module *module,
    screensave_common_config *common_config,
    void *product_config,
    unsigned int product_config_size,
    screensave_diag_context *diagnostics
)
{
    anthology_dialog_state dialog_state;
    anthology_config *config;
    INT_PTR result;

    config = anthology_as_config(product_config, product_config_size);
    if (module == NULL || common_config == NULL || config == NULL) {
        anthology_emit_config_diag(
            diagnostics,
            SCREENSAVE_DIAG_LEVEL_ERROR,
            6803UL,
            "The Anthology dialog could not start because its config state was invalid."
        );
        return -1;
    }

    dialog_state.module = module;
    dialog_state.common_config = common_config;
    dialog_state.product_config = config;
    dialog_state.current_saver_index = 0;

    result = DialogBoxParamA(
        instance,
        MAKEINTRESOURCEA(IDD_ANTHOLOGY_CONFIG),
        owner_window,
        anthology_config_dialog_proc,
        (LPARAM)&dialog_state
    );

    if (result == -1) {
        anthology_emit_config_diag(
            diagnostics,
            SCREENSAVE_DIAG_LEVEL_ERROR,
            6804UL,
            "The Anthology config dialog resource could not be loaded."
        );
    }

    return result;
}

int anthology_config_export_settings_entries(
    const screensave_saver_module *module,
    const screensave_common_config *common_config,
    const void *product_config,
    unsigned int product_config_size,
    screensave_settings_file_kind kind,
    screensave_settings_writer *writer,
    screensave_diag_context *diagnostics
)
{
    const anthology_config *config;
    char saver_list_text[512];
    char family_text[128];
    unsigned int index;

    (void)module;
    (void)common_config;
    (void)diagnostics;

    config = anthology_as_const_config(product_config, product_config_size);
    if (kind != SCREENSAVE_SETTINGS_FILE_PRESET) {
        return 1;
    }
    if (config == NULL || writer == NULL || writer->write_string == NULL || writer->write_unsigned_long == NULL) {
        return 0;
    }

    if (
        !anthology_family_mask_to_text(family_text, sizeof(family_text), config->family_mask) ||
        !anthology_saver_mask_to_text(saver_list_text, sizeof(saver_list_text), config->include_mask)
    ) {
        return 0;
    }

    if (
        !writer->write_string(writer->context, "product", "selection_mode", anthology_selection_mode_name(config->selection_mode)) ||
        !writer->write_unsigned_long(writer->context, "product", "interval_seconds", config->interval_seconds) ||
        !writer->write_string(writer->context, "product", "family_filter", family_text) ||
        !writer->write_string(writer->context, "product", "include_keys", saver_list_text) ||
        !anthology_saver_mask_to_text(saver_list_text, sizeof(saver_list_text), config->favorite_mask) ||
        !writer->write_string(writer->context, "product", "favorite_keys", saver_list_text) ||
        !writer->write_string(writer->context, "product", "renderer_policy", anthology_renderer_policy_name(config->renderer_policy)) ||
        !writer->write_string(writer->context, "product", "randomize_inner_presets", config->randomize_inner_presets ? "true" : "false") ||
        !writer->write_string(writer->context, "product", "randomize_inner_themes", config->randomize_inner_themes ? "true" : "false") ||
        !writer->write_string(
            writer->context,
            "product",
            "respect_inner_randomization",
            config->respect_inner_randomization ? "true" : "false"
        )
    ) {
        return 0;
    }

    for (index = 0U; index < ANTHOLOGY_INNER_SAVER_COUNT; ++index) {
        if (
            !writer->write_unsigned_long(
                    writer->context,
                    "weights",
                    anthology_catalog_get_product_key(index),
                    config->saver_weights[index]
                )
        ) {
            return 0;
        }
    }

    return 1;
}

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
)
{
    anthology_config *config;
    const char *canonical_key;
    int saver_index;
    unsigned long parsed_value;

    (void)module;
    (void)diagnostics;

    config = anthology_as_config(product_config, product_config_size);
    if (kind != SCREENSAVE_SETTINGS_FILE_PRESET) {
        return 1;
    }
    if (config == NULL || common_config == NULL || section == NULL || key == NULL || value == NULL) {
        return 0;
    }

    if (lstrcmpiA(section, "product") == 0) {
        if (lstrcmpiA(key, "selection_mode") == 0) {
            return anthology_parse_selection_mode(value, &config->selection_mode);
        }
        if (lstrcmpiA(key, "interval_seconds") == 0) {
            return anthology_parse_unsigned_long(value, &config->interval_seconds);
        }
        if (lstrcmpiA(key, "family_filter") == 0) {
            return anthology_parse_family_mask(value, &config->family_mask);
        }
        if (lstrcmpiA(key, "include_keys") == 0) {
            return anthology_parse_saver_mask(value, &config->include_mask);
        }
        if (lstrcmpiA(key, "favorite_keys") == 0) {
            return anthology_parse_saver_mask(value, &config->favorite_mask);
        }
        if (lstrcmpiA(key, "renderer_policy") == 0) {
            return anthology_parse_renderer_policy(value, &config->renderer_policy);
        }
        if (lstrcmpiA(key, "randomize_inner_presets") == 0) {
            return anthology_parse_bool(value, &config->randomize_inner_presets);
        }
        if (lstrcmpiA(key, "randomize_inner_themes") == 0) {
            return anthology_parse_bool(value, &config->randomize_inner_themes);
        }
        if (lstrcmpiA(key, "respect_inner_randomization") == 0) {
            return anthology_parse_bool(value, &config->respect_inner_randomization);
        }
        return 1;
    }

    if (lstrcmpiA(section, "weights") == 0) {
        canonical_key = screensave_saver_registry_canonical_product_key(key);
        if (canonical_key == NULL || lstrcmpiA(canonical_key, "anthology") == 0) {
            return 1;
        }

        saver_index = anthology_catalog_find_index(canonical_key);
        if (saver_index < 0) {
            return 1;
        }

        parsed_value = config->saver_weights[saver_index];
        if (!anthology_parse_unsigned_long(value, &parsed_value)) {
            return 0;
        }
        config->saver_weights[saver_index] = parsed_value;
        return 1;
    }

    return 1;
}

static unsigned long anthology_random_next(unsigned long *state)
{
    if (state == NULL) {
        return 0UL;
    }

    *state = (*state * 1664525UL) + 1013904223UL;
    return *state;
}

void anthology_config_randomize_settings(
    const screensave_saver_module *module,
    screensave_common_config *common_config,
    void *product_config,
    unsigned int product_config_size,
    const screensave_session_seed *seed,
    screensave_diag_context *diagnostics
)
{
    anthology_config *config;
    unsigned long state;
    unsigned long selector;
    unsigned int index;

    (void)module;
    (void)diagnostics;

    config = anthology_as_config(product_config, product_config_size);
    if (common_config == NULL || config == NULL) {
        return;
    }

    state = seed != NULL ? seed->stream_seed : 0xA87A0B08UL;
    selector = anthology_random_next(&state) % 3UL;
    if (selector == 0UL) {
        config->selection_mode = ANTHOLOGY_SELECTION_RANDOM_UNIFORM;
    } else if (selector == 1UL) {
        config->selection_mode = ANTHOLOGY_SELECTION_WEIGHTED_RANDOM;
    } else {
        config->selection_mode = ANTHOLOGY_SELECTION_FAVORITES_FIRST;
    }

    config->interval_seconds = 45UL + (anthology_random_next(&state) % 136UL);
    config->renderer_policy = anthology_random_next(&state) % 3UL;
    config->randomize_inner_presets = (anthology_random_next(&state) & 1UL) != 0UL;
    config->randomize_inner_themes = (anthology_random_next(&state) & 1UL) != 0UL;
    config->respect_inner_randomization = 1;

    if ((anthology_random_next(&state) & 1UL) == 0UL) {
        config->family_mask = ANTHOLOGY_FAMILY_QUIET |
            ANTHOLOGY_FAMILY_SYSTEMS_AMBIENT |
            ANTHOLOGY_FAMILY_PLACES_ATMOSPHERE;
    } else {
        config->family_mask = anthology_all_family_flags();
    }

    config->include_mask = anthology_all_saver_bits();
    config->favorite_mask = 0UL;
    for (index = 0U; index < ANTHOLOGY_INNER_SAVER_COUNT; ++index) {
        config->saver_weights[index] = 40UL + (anthology_random_next(&state) % 181UL);
        if ((anthology_random_next(&state) % 5UL) == 0UL) {
            config->favorite_mask |= (1UL << index);
        }
    }

    common_config->theme_key = ANTHOLOGY_DEFAULT_THEME_KEY;
    anthology_config_clamp(common_config, config, sizeof(*config));
}
