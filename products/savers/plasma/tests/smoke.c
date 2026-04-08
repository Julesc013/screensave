#include <string.h>

#include "../src/plasma_internal.h"

#define PLASMA_SMOKE_CAPTURE_LIMIT 32U

typedef struct plasma_smoke_capture_entry_tag {
    char section[32];
    char key[64];
    char value[256];
} plasma_smoke_capture_entry;

typedef struct plasma_smoke_capture_tag {
    plasma_smoke_capture_entry entries[PLASMA_SMOKE_CAPTURE_LIMIT];
    unsigned int count;
} plasma_smoke_capture;

static void plasma_smoke_copy_text(char *buffer, unsigned int buffer_size, const char *text)
{
    if (buffer == NULL || buffer_size == 0U) {
        return;
    }

    buffer[0] = '\0';
    if (text == NULL) {
        return;
    }

    strncpy(buffer, text, (size_t)(buffer_size - 1U));
    buffer[buffer_size - 1U] = '\0';
}

static int plasma_smoke_capture_write_string(
    void *context,
    const char *section,
    const char *key,
    const char *value
)
{
    plasma_smoke_capture *capture;
    plasma_smoke_capture_entry *entry;

    capture = (plasma_smoke_capture *)context;
    if (capture == NULL || capture->count >= PLASMA_SMOKE_CAPTURE_LIMIT) {
        return 0;
    }

    entry = &capture->entries[capture->count++];
    plasma_smoke_copy_text(entry->section, (unsigned int)sizeof(entry->section), section);
    plasma_smoke_copy_text(entry->key, (unsigned int)sizeof(entry->key), key);
    plasma_smoke_copy_text(entry->value, (unsigned int)sizeof(entry->value), value);
    return 1;
}

static int plasma_smoke_capture_has_string(
    const plasma_smoke_capture *capture,
    const char *section,
    const char *key,
    const char *value
)
{
    unsigned int index;

    if (capture == NULL || section == NULL || key == NULL || value == NULL) {
        return 0;
    }

    for (index = 0U; index < capture->count; ++index) {
        if (
            strcmp(capture->entries[index].section, section) == 0 &&
            strcmp(capture->entries[index].key, key) == 0 &&
            strcmp(capture->entries[index].value, value) == 0
        ) {
            return 1;
        }
    }

    return 0;
}

static int plasma_compile_classic_plan(
    const screensave_saver_module *module,
    const char *preset_key,
    const char *theme_key,
    plasma_plan *plan_out
)
{
    screensave_common_config common_config;
    plasma_config product_config;
    screensave_config_binding binding;
    screensave_saver_environment environment;

    if (module == NULL || plan_out == NULL) {
        return 0;
    }

    plasma_config_set_defaults(&common_config, &product_config, sizeof(product_config));
    if (preset_key != NULL) {
        plasma_apply_preset_to_config(preset_key, &common_config, &product_config);
        common_config.preset_key = preset_key;
    }
    if (theme_key != NULL) {
        common_config.theme_key = theme_key;
    }

    screensave_config_binding_init(&binding, &common_config, &product_config, sizeof(product_config));
    ZeroMemory(&environment, sizeof(environment));
    environment.mode = SCREENSAVE_SESSION_MODE_WINDOWED;
    environment.drawable_size.width = 320;
    environment.drawable_size.height = 240;
    environment.seed.base_seed = 0x00000A55UL;
    environment.seed.stream_seed = 0x00000A77UL;
    environment.seed.deterministic = common_config.use_deterministic_seed;
    environment.config_binding = &binding;

    return plasma_plan_compile(plan_out, module, &environment);
}

static int plasma_compile_selection_plan(
    const screensave_saver_module *module,
    const char *preset_key,
    const char *theme_key,
    const plasma_selection_preferences *selection_preferences,
    plasma_plan *plan_out
)
{
    screensave_common_config common_config;
    plasma_config product_config;
    screensave_config_binding binding;
    screensave_saver_environment environment;

    if (module == NULL || plan_out == NULL) {
        return 0;
    }

    plasma_config_set_defaults(&common_config, &product_config, sizeof(product_config));
    if (preset_key != NULL) {
        plasma_apply_preset_to_config(preset_key, &common_config, &product_config);
        common_config.preset_key = preset_key;
    }
    if (theme_key != NULL) {
        common_config.theme_key = theme_key;
    }
    if (selection_preferences != NULL) {
        product_config.selection = *selection_preferences;
    }

    screensave_config_binding_init(&binding, &common_config, &product_config, sizeof(product_config));
    ZeroMemory(&environment, sizeof(environment));
    environment.mode = SCREENSAVE_SESSION_MODE_WINDOWED;
    environment.drawable_size.width = 320;
    environment.drawable_size.height = 240;
    environment.seed.base_seed = 0x00000A55UL;
    environment.seed.stream_seed = 0x00000A77UL;
    environment.seed.deterministic = common_config.use_deterministic_seed;
    environment.config_binding = &binding;

    return plasma_plan_compile(plan_out, module, &environment);
}

int main(void)
{
    static const char *const g_required_preset_keys[] = {
        "plasma_lava",
        "aurora_plasma",
        "ocean_interference",
        "museum_phosphor",
        "quiet_darkroom",
        "midnight_interference",
        "amber_terminal"
    };
    static const char *const g_required_theme_keys[] = {
        "plasma_lava",
        "aurora_cool",
        "oceanic_blue",
        "museum_phosphor",
        "quiet_darkroom",
        "midnight_interference",
        "amber_terminal"
    };
    screensave_common_config common_config;
    plasma_config product_config;
    screensave_config_binding binding;
    screensave_saver_environment environment;
    screensave_saver_session *session;
    unsigned long issue_flags;
    const screensave_saver_module *module;
    const screensave_preset_descriptor *preset_descriptor;
    const screensave_theme_descriptor *theme_descriptor;
    plasma_plan plan;
    plasma_output_frame output_frame;
    plasma_treated_frame treated_frame;
    plasma_presentation_target presentation_target;
    const plasma_content_registry *registry;
    const plasma_content_pack_entry *pack_entry;
    screensave_pack_manifest pack_manifest;
    plasma_selection_preferences selection_preferences;
    plasma_smoke_capture settings_capture;
    screensave_settings_writer settings_writer;
    screensave_session_seed random_seed;
    unsigned int index;

    module = plasma_get_module();
    if (!screensave_saver_module_is_valid(module)) {
        return 1;
    }
    if (
        module->identity.product_key == NULL ||
        strcmp(module->identity.product_key, "plasma") != 0 ||
        module->identity.display_name == NULL ||
        strcmp(module->identity.display_name, "Plasma") != 0
    ) {
        return 2;
    }
    if (
        module->routing_policy.minimum_kind != SCREENSAVE_RENDERER_KIND_GDI ||
        module->routing_policy.preferred_kind != SCREENSAVE_RENDERER_KIND_GL11 ||
        module->routing_policy.quality_class != SCREENSAVE_CAPABILITY_QUALITY_SAFE
    ) {
        return 3;
    }
    if (
        !(module->capability_flags & SCREENSAVE_SAVER_CAP_GDI) ||
        !(module->capability_flags & SCREENSAVE_SAVER_CAP_GL11) ||
        !(module->capability_flags & SCREENSAVE_SAVER_CAP_LONG_RUN_STABLE) ||
        !(module->capability_flags & SCREENSAVE_SAVER_CAP_PREVIEW_SAFE)
    ) {
        return 4;
    }

    plasma_config_set_defaults(&common_config, &product_config, sizeof(product_config));
    plasma_config_clamp(&common_config, &product_config, sizeof(product_config));
    if (!screensave_common_config_validate(&common_config, &issue_flags)) {
        return 5;
    }
    if (
        common_config.preset_key == NULL ||
        strcmp(common_config.preset_key, PLASMA_DEFAULT_PRESET_KEY) != 0 ||
        common_config.theme_key == NULL ||
        strcmp(common_config.theme_key, PLASMA_DEFAULT_THEME_KEY) != 0
    ) {
        return 6;
    }
    if (
        plasma_classic_preset_count() != PLASMA_PRESET_COUNT ||
        plasma_classic_theme_count() != PLASMA_THEME_COUNT ||
        module->preset_count != plasma_classic_preset_count() ||
        module->theme_count != plasma_classic_theme_count()
    ) {
        return 7;
    }

    for (index = 0U; index < (unsigned int)(sizeof(g_required_preset_keys) / sizeof(g_required_preset_keys[0])); ++index) {
        if (!plasma_classic_is_known_preset_key(g_required_preset_keys[index])) {
            return 8;
        }
    }
    for (index = 0U; index < (unsigned int)(sizeof(g_required_theme_keys) / sizeof(g_required_theme_keys[0])); ++index) {
        if (!plasma_classic_is_known_theme_key(g_required_theme_keys[index])) {
            return 9;
        }
    }
    if (
        plasma_classic_canonical_key("ember_lava") == NULL ||
        strcmp(plasma_classic_canonical_key("ember_lava"), "plasma_lava") != 0
    ) {
        return 10;
    }
    preset_descriptor = plasma_find_preset_descriptor("ember_lava");
    if (preset_descriptor == NULL || strcmp(preset_descriptor->preset_key, "plasma_lava") != 0) {
        return 11;
    }
    if (plasma_find_preset_values("ember_lava") == NULL) {
        return 12;
    }
    theme_descriptor = plasma_find_theme_descriptor("ember_lava");
    if (theme_descriptor == NULL || strcmp(theme_descriptor->theme_key, "plasma_lava") != 0) {
        return 13;
    }
    if (module->config_hooks == NULL || module->config_hooks->randomize_settings == NULL) {
        return 14;
    }

    if (!plasma_compile_classic_plan(module, PLASMA_DEFAULT_PRESET_KEY, NULL, &plan)) {
        return 15;
    }
    if (!plasma_plan_validate(&plan, module)) {
        return 16;
    }
    if (
        !plan.classic_execution ||
        plan.preset_key == NULL ||
        strcmp(plan.preset_key, PLASMA_DEFAULT_PRESET_KEY) != 0 ||
        plan.theme_key == NULL ||
        strcmp(plan.theme_key, PLASMA_DEFAULT_THEME_KEY) != 0 ||
        plan.output_family != PLASMA_OUTPUT_FAMILY_RASTER ||
        plan.output_mode != PLASMA_OUTPUT_MODE_NATIVE_RASTER ||
        plan.sampling_treatment != PLASMA_SAMPLING_TREATMENT_NONE ||
        plan.filter_treatment != PLASMA_FILTER_TREATMENT_NONE ||
        plan.emulation_treatment != PLASMA_EMULATION_TREATMENT_NONE ||
        plan.accent_treatment != PLASMA_ACCENT_TREATMENT_NONE ||
        plan.presentation_mode != PLASMA_PRESENTATION_MODE_FLAT ||
        plan.minimum_kind != SCREENSAVE_RENDERER_KIND_GDI ||
        plan.preferred_kind != SCREENSAVE_RENDERER_KIND_GL11 ||
        plan.quality_class != SCREENSAVE_CAPABILITY_QUALITY_SAFE ||
        !plasma_output_validate_plan(&plan) ||
        !plasma_treatment_validate_plan(&plan) ||
        !plasma_presentation_validate_plan(&plan) ||
        !plasma_plan_is_lower_band_baseline(&plan) ||
        !plasma_plan_validate_for_renderer_kind(&plan, module, SCREENSAVE_RENDERER_KIND_GDI) ||
        !plasma_plan_validate_for_renderer_kind(&plan, module, SCREENSAVE_RENDERER_KIND_GL11) ||
        !plasma_plan_validate_lower_band_baseline(&plan, module)
    ) {
        return 17;
    }
    if (
        plasma_plan_validate_for_renderer_kind(&plan, module, SCREENSAVE_RENDERER_KIND_GL21) ||
        plasma_plan_validate_for_renderer_kind(&plan, module, SCREENSAVE_RENDERER_KIND_UNKNOWN) ||
        !plasma_is_lower_band_kind(SCREENSAVE_RENDERER_KIND_GDI) ||
        !plasma_is_lower_band_kind(SCREENSAVE_RENDERER_KIND_GL11) ||
        plasma_is_lower_band_kind(SCREENSAVE_RENDERER_KIND_GL21)
    ) {
        return 18;
    }
    if (!plasma_compile_classic_plan(module, "ember_lava", "ember_lava", &plan)) {
        return 19;
    }
    if (
        plan.preset_key == NULL ||
        strcmp(plan.preset_key, "plasma_lava") != 0 ||
        plan.theme_key == NULL ||
        strcmp(plan.theme_key, "plasma_lava") != 0
    ) {
        return 20;
    }

    for (index = 0U; index < (unsigned int)(sizeof(g_required_preset_keys) / sizeof(g_required_preset_keys[0])); ++index) {
        if (!plasma_compile_classic_plan(module, g_required_preset_keys[index], NULL, &plan)) {
            return 21;
        }
        preset_descriptor = plasma_find_preset_descriptor(g_required_preset_keys[index]);
        if (
            preset_descriptor == NULL ||
            plan.preset_key == NULL ||
            plan.theme_key == NULL ||
            strcmp(plan.preset_key, preset_descriptor->preset_key) != 0 ||
            strcmp(plan.theme_key, preset_descriptor->theme_key) != 0 ||
            !plasma_plan_validate_for_renderer_kind(&plan, module, SCREENSAVE_RENDERER_KIND_GDI) ||
            !plasma_plan_validate_for_renderer_kind(&plan, module, SCREENSAVE_RENDERER_KIND_GL11)
        ) {
            return 22;
        }
    }

    registry = plasma_content_get_registry();
    if (
        !plasma_content_registry_validate() ||
        registry == NULL ||
        registry->preset_count != PLASMA_PRESET_COUNT ||
        registry->theme_count != PLASMA_THEME_COUNT ||
        registry->pack_count != 1U ||
        registry->preset_set_count != 2U ||
        registry->theme_set_count != 2U ||
        !plasma_content_registry_has_channel(PLASMA_CONTENT_CHANNEL_STABLE) ||
        plasma_content_registry_has_channel(PLASMA_CONTENT_CHANNEL_EXPERIMENTAL)
    ) {
        return 101;
    }

    pack_entry = plasma_content_find_pack_entry("lava_remix");
    if (
        pack_entry == NULL ||
        strcmp(pack_entry->pack_key, "lava_remix") != 0 ||
        pack_entry->source != PLASMA_CONTENT_SOURCE_PACK ||
        pack_entry->channel != PLASMA_CONTENT_CHANNEL_STABLE ||
        pack_entry->minimum_kind != SCREENSAVE_RENDERER_KIND_GDI ||
        pack_entry->preferred_kind != SCREENSAVE_RENDERER_KIND_GL11 ||
        pack_entry->quality_class != SCREENSAVE_CAPABILITY_QUALITY_BALANCED ||
        pack_entry->asset_count != 2U ||
        strcmp(pack_entry->assets[0].canonical_key, "plasma_lava") != 0 ||
        strcmp(pack_entry->assets[1].canonical_key, "plasma_lava") != 0
    ) {
        return 102;
    }

    ZeroMemory(&pack_manifest, sizeof(pack_manifest));
    if (
        !screensave_pack_manifest_load(
            "products/savers/plasma/packs/lava_remix/pack.ini",
            SCREENSAVE_PACK_SOURCE_BUILTIN,
            &pack_manifest,
            NULL
        ) ||
        strcmp(pack_manifest.pack_key, "lava_remix") != 0 ||
        strcmp(pack_manifest.product_key, "plasma") != 0 ||
        pack_manifest.preset_file_count != 1U ||
        pack_manifest.theme_file_count != 1U ||
        strcmp(pack_manifest.preset_files[0], "presets/lava_remix.preset.ini") != 0 ||
        strcmp(pack_manifest.theme_files[0], "themes/lava_remix.theme.ini") != 0
    ) {
        return 103;
    }

    if (
        product_config.selection.content_filter != PLASMA_CONTENT_FILTER_STABLE_ONLY ||
        product_config.selection.favorites_only != 0 ||
        product_config.selection.preset_set_key[0] != '\0' ||
        product_config.selection.theme_set_key[0] != '\0' ||
        strcmp(product_config.selection.favorite_preset_keys, "none") != 0 ||
        strcmp(product_config.selection.excluded_preset_keys, "none") != 0 ||
        strcmp(product_config.selection.favorite_theme_keys, "none") != 0 ||
        strcmp(product_config.selection.excluded_theme_keys, "none") != 0
    ) {
        return 104;
    }

    if (
        !plasma_compile_classic_plan(module, PLASMA_DEFAULT_PRESET_KEY, NULL, &plan) ||
        plan.selection.selected_preset == NULL ||
        plan.selection.selected_theme == NULL ||
        plan.selection.active_preset_set != NULL ||
        plan.selection.active_theme_set != NULL ||
        plan.selection.content_filter != PLASMA_CONTENT_FILTER_STABLE_ONLY ||
        plan.selection.favorites_only_requested != 0 ||
        plan.selection.favorites_only_applied != 0 ||
        plan.selection.favorite_preset_mask != 0UL ||
        plan.selection.excluded_preset_mask != 0UL ||
        plan.selection.favorite_theme_mask != 0UL ||
        plan.selection.excluded_theme_mask != 0UL ||
        strcmp(plan.selection.selected_preset->preset_key, PLASMA_DEFAULT_PRESET_KEY) != 0 ||
        strcmp(plan.selection.selected_theme->theme_key, PLASMA_DEFAULT_THEME_KEY) != 0
    ) {
        return 105;
    }

    plasma_selection_preferences_set_defaults(&selection_preferences);
    lstrcpyA(selection_preferences.preset_set_key, "ghost_set");
    lstrcpyA(selection_preferences.theme_set_key, "ghost_set");
    lstrcpyA(selection_preferences.favorite_preset_keys, "ghost,ember_lava");
    lstrcpyA(selection_preferences.excluded_preset_keys, "ghost");
    lstrcpyA(selection_preferences.favorite_theme_keys, "ghost,ember_lava");
    lstrcpyA(selection_preferences.excluded_theme_keys, "ghost");
    plasma_selection_preferences_clamp(&selection_preferences);
    if (
        selection_preferences.preset_set_key[0] != '\0' ||
        selection_preferences.theme_set_key[0] != '\0' ||
        strcmp(selection_preferences.favorite_preset_keys, "plasma_lava") != 0 ||
        strcmp(selection_preferences.excluded_preset_keys, "none") != 0 ||
        strcmp(selection_preferences.favorite_theme_keys, "plasma_lava") != 0 ||
        strcmp(selection_preferences.excluded_theme_keys, "none") != 0
    ) {
        return 106;
    }

    plasma_selection_preferences_set_defaults(&selection_preferences);
    lstrcpyA(selection_preferences.preset_set_key, "dark_room_classics");
    lstrcpyA(selection_preferences.theme_set_key, "dark_room_classics");
    if (
        !plasma_compile_selection_plan(
            module,
            PLASMA_DEFAULT_PRESET_KEY,
            PLASMA_DEFAULT_THEME_KEY,
            &selection_preferences,
            &plan
        ) ||
        plan.selection.active_preset_set == NULL ||
        plan.selection.active_theme_set == NULL ||
        strcmp(plan.selection.active_preset_set->set_key, "dark_room_classics") != 0 ||
        strcmp(plan.selection.active_theme_set->set_key, "dark_room_classics") != 0 ||
        strcmp(plan.preset_key, "quiet_darkroom") != 0 ||
        strcmp(plan.theme_key, "quiet_darkroom") != 0 ||
        plan.selection.favorites_only_requested != 0 ||
        plan.selection.favorites_only_applied != 0
    ) {
        return 107;
    }

    plasma_selection_preferences_set_defaults(&selection_preferences);
    selection_preferences.favorites_only = 1;
    lstrcpyA(selection_preferences.favorite_preset_keys, "quiet_darkroom,amber_terminal");
    lstrcpyA(selection_preferences.excluded_preset_keys, "quiet_darkroom");
    lstrcpyA(selection_preferences.favorite_theme_keys, "quiet_darkroom,amber_terminal");
    lstrcpyA(selection_preferences.excluded_theme_keys, "quiet_darkroom");
    if (
        !plasma_compile_selection_plan(
            module,
            PLASMA_DEFAULT_PRESET_KEY,
            PLASMA_DEFAULT_THEME_KEY,
            &selection_preferences,
            &plan
        ) ||
        strcmp(plan.preset_key, "amber_terminal") != 0 ||
        strcmp(plan.theme_key, "amber_terminal") != 0 ||
        !plan.selection.favorites_only_requested ||
        !plan.selection.favorites_only_applied
    ) {
        return 108;
    }

    plasma_selection_preferences_set_defaults(&selection_preferences);
    selection_preferences.content_filter = PLASMA_CONTENT_FILTER_EXPERIMENTAL_ONLY;
    if (
        !plasma_compile_selection_plan(
            module,
            PLASMA_DEFAULT_PRESET_KEY,
            PLASMA_DEFAULT_THEME_KEY,
            &selection_preferences,
            &plan
        ) ||
        plan.selection.content_filter != PLASMA_CONTENT_FILTER_STABLE_ONLY ||
        strcmp(plan.preset_key, PLASMA_DEFAULT_PRESET_KEY) != 0 ||
        strcmp(plan.theme_key, PLASMA_DEFAULT_THEME_KEY) != 0
    ) {
        return 109;
    }

    plasma_config_set_defaults(&common_config, &product_config, sizeof(product_config));
    if (
        !plasma_config_import_settings_entry(
            module,
            &common_config,
            &product_config,
            sizeof(product_config),
            SCREENSAVE_SETTINGS_FILE_PRESET,
            "content",
            "content_filter",
            "stable_and_experimental",
            NULL
        ) ||
        !plasma_config_import_settings_entry(
            module,
            &common_config,
            &product_config,
            sizeof(product_config),
            SCREENSAVE_SETTINGS_FILE_PRESET,
            "content",
            "favorites_only",
            "true",
            NULL
        ) ||
        !plasma_config_import_settings_entry(
            module,
            &common_config,
            &product_config,
            sizeof(product_config),
            SCREENSAVE_SETTINGS_FILE_PRESET,
            "content",
            "favorite_preset_keys",
            "ember_lava,amber_terminal,ghost",
            NULL
        ) ||
        !plasma_config_import_settings_entry(
            module,
            &common_config,
            &product_config,
            sizeof(product_config),
            SCREENSAVE_SETTINGS_FILE_PRESET,
            "content",
            "favorite_theme_keys",
            "ember_lava,amber_terminal,ghost",
            NULL
        )
    ) {
        return 110;
    }

    plasma_config_clamp(&common_config, &product_config, sizeof(product_config));
    if (
        product_config.selection.content_filter != PLASMA_CONTENT_FILTER_STABLE_AND_EXPERIMENTAL ||
        !product_config.selection.favorites_only ||
        strcmp(product_config.selection.favorite_preset_keys, "plasma_lava,amber_terminal") != 0 ||
        strcmp(product_config.selection.excluded_preset_keys, "none") != 0 ||
        strcmp(product_config.selection.favorite_theme_keys, "plasma_lava,amber_terminal") != 0 ||
        strcmp(product_config.selection.excluded_theme_keys, "none") != 0 ||
        common_config.preset_key == NULL ||
        common_config.theme_key == NULL ||
        strcmp(common_config.preset_key, PLASMA_DEFAULT_PRESET_KEY) != 0 ||
        strcmp(common_config.theme_key, PLASMA_DEFAULT_THEME_KEY) != 0
    ) {
        return 111;
    }

    ZeroMemory(&settings_capture, sizeof(settings_capture));
    ZeroMemory(&settings_writer, sizeof(settings_writer));
    settings_writer.context = &settings_capture;
    settings_writer.write_string = plasma_smoke_capture_write_string;
    if (
        !plasma_config_export_settings_entries(
            module,
            &common_config,
            &product_config,
            sizeof(product_config),
            SCREENSAVE_SETTINGS_FILE_PRESET,
            &settings_writer,
            NULL
        ) ||
        !plasma_smoke_capture_has_string(
            &settings_capture,
            "content",
            "content_filter",
            "stable_and_experimental"
        ) ||
        !plasma_smoke_capture_has_string(
            &settings_capture,
            "content",
            "favorites_only",
            "true"
        ) ||
        !plasma_smoke_capture_has_string(
            &settings_capture,
            "content",
            "favorite_preset_keys",
            "plasma_lava,amber_terminal"
        ) ||
        !plasma_smoke_capture_has_string(
            &settings_capture,
            "content",
            "favorite_theme_keys",
            "plasma_lava,amber_terminal"
        )
    ) {
        return 112;
    }

    random_seed.base_seed = 0x13572468UL;
    random_seed.stream_seed = 0x24681357UL;
    random_seed.deterministic = 0;
    selection_preferences = product_config.selection;
    plasma_config_randomize_settings(
        module,
        &common_config,
        &product_config,
        sizeof(product_config),
        &random_seed,
        NULL
    );
    if (memcmp(&selection_preferences, &product_config.selection, sizeof(selection_preferences)) != 0) {
        return 113;
    }

    screensave_config_binding_init(&binding, &common_config, &product_config, sizeof(product_config));
    ZeroMemory(&environment, sizeof(environment));
    environment.mode = SCREENSAVE_SESSION_MODE_WINDOWED;
    environment.drawable_size.width = 320;
    environment.drawable_size.height = 240;
    environment.seed.base_seed = 0x00000A55UL;
    environment.seed.stream_seed = 0x00000A77UL;
    environment.seed.deterministic = common_config.use_deterministic_seed;
    environment.config_binding = &binding;

    session = NULL;
    if (!plasma_create_session(module, &session, &environment) || session == NULL) {
        return 23;
    }
    if (
        session->plan.preset_key == NULL ||
        strcmp(session->plan.preset_key, PLASMA_DEFAULT_PRESET_KEY) != 0 ||
        session->plan.theme_key == NULL ||
        strcmp(session->plan.theme_key, PLASMA_DEFAULT_THEME_KEY) != 0 ||
        session->state.active_renderer_kind != SCREENSAVE_RENDERER_KIND_GDI ||
        !plasma_plan_validate_for_renderer_kind(&session->plan, module, session->state.active_renderer_kind)
    ) {
        plasma_destroy_session(session);
        return 24;
    }

    environment.clock.delta_millis = 33UL;
    plasma_step_session(session, &environment);
    if (!plasma_output_build(&session->plan, &session->state, &output_frame)) {
        plasma_destroy_session(session);
        return 25;
    }
    if (
        !plasma_treatment_apply(
            &session->plan,
            &session->state,
            &output_frame,
            &session->state.visual_buffer,
            &treated_frame
        )
    ) {
        plasma_destroy_session(session);
        return 26;
    }
    if (
        !plasma_presentation_prepare(
            &session->plan,
            &session->state,
            &treated_frame,
            &presentation_target
        )
    ) {
        plasma_destroy_session(session);
        return 27;
    }
    if (
        session->state.visual_buffer.pixels == NULL ||
        session->state.field_primary == NULL ||
        !plasma_plan_validate(&session->plan, module) ||
        !plasma_plan_validate_lower_band_baseline(&session->plan, module)
    ) {
        plasma_destroy_session(session);
        return 28;
    }

    plan = session->plan;
    plan.output_family = PLASMA_OUTPUT_FAMILY_GLYPH;
    if (
        plasma_output_validate_plan(&plan) ||
        plasma_output_build(&plan, &session->state, &output_frame) ||
        plasma_plan_validate_for_renderer_kind(&plan, module, SCREENSAVE_RENDERER_KIND_GDI) ||
        plasma_plan_validate_for_renderer_kind(&plan, module, SCREENSAVE_RENDERER_KIND_GL11)
    ) {
        plasma_destroy_session(session);
        return 29;
    }

    plan = session->plan;
    plan.filter_treatment = PLASMA_FILTER_TREATMENT_BLUR;
    if (
        plasma_treatment_validate_plan(&plan) ||
        plasma_treatment_apply(
            &plan,
            &session->state,
            &output_frame,
            &session->state.visual_buffer,
            &treated_frame
        ) ||
        plasma_plan_validate_for_renderer_kind(&plan, module, SCREENSAVE_RENDERER_KIND_GDI) ||
        plasma_plan_validate_for_renderer_kind(&plan, module, SCREENSAVE_RENDERER_KIND_GL11)
    ) {
        plasma_destroy_session(session);
        return 30;
    }

    plan = session->plan;
    plan.presentation_mode = PLASMA_PRESENTATION_MODE_HEIGHTFIELD;
    if (
        plasma_presentation_validate_plan(&plan) ||
        plasma_presentation_prepare(
            &plan,
            &session->state,
            &treated_frame,
            &presentation_target
        ) ||
        plasma_plan_validate_for_renderer_kind(&plan, module, SCREENSAVE_RENDERER_KIND_GDI) ||
        plasma_plan_validate_for_renderer_kind(&plan, module, SCREENSAVE_RENDERER_KIND_GL11)
    ) {
        plasma_destroy_session(session);
        return 31;
    }

    plasma_destroy_session(session);
    return 0;
}
