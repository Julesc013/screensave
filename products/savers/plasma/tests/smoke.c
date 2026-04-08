#include <string.h>

#include "../src/plasma_internal.h"

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
