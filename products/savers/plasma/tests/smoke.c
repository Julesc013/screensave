#include <string.h>

#include "screensave/private/renderer_runtime.h"
#include "../src/plasma_internal.h"

#define PLASMA_SMOKE_CAPTURE_LIMIT 32U
#define PLASMA_SMOKE_ADVANCED_COMPONENTS \
    (PLASMA_ADVANCED_COMPONENT_HISTORY_FEEDBACK | \
        PLASMA_ADVANCED_COMPONENT_DOMAIN_WARP | \
        PLASMA_ADVANCED_COMPONENT_FLOW_TURBULENCE | \
        PLASMA_ADVANCED_COMPONENT_GLOW_POST)
#define PLASMA_SMOKE_MODERN_COMPONENTS \
    (PLASMA_MODERN_COMPONENT_REFINED_FIELD | \
        PLASMA_MODERN_COMPONENT_REFINED_FILTER | \
        PLASMA_MODERN_COMPONENT_PRESENTATION_BUFFER)
#define PLASMA_SMOKE_PREMIUM_COMPONENTS \
    (PLASMA_PREMIUM_COMPONENT_EXTENDED_HISTORY | \
        PLASMA_PREMIUM_COMPONENT_POST_CHAIN | \
        PLASMA_PREMIUM_COMPONENT_HEIGHTFIELD_PRESENTATION)

typedef struct plasma_smoke_capture_entry_tag {
    char section[32];
    char key[64];
    char value[256];
} plasma_smoke_capture_entry;

typedef struct plasma_smoke_capture_tag {
    plasma_smoke_capture_entry entries[PLASMA_SMOKE_CAPTURE_LIMIT];
    unsigned int count;
} plasma_smoke_capture;

static int plasma_smoke_renderer_begin_frame(
    screensave_renderer *renderer,
    const screensave_frame_info *frame_info
)
{
    (void)renderer;
    (void)frame_info;
    return 1;
}

static void plasma_smoke_renderer_clear(screensave_renderer *renderer, screensave_color color)
{
    (void)renderer;
    (void)color;
}

static void plasma_smoke_renderer_fill_rect(
    screensave_renderer *renderer,
    const screensave_recti *rect,
    screensave_color color
)
{
    (void)renderer;
    (void)rect;
    (void)color;
}

static void plasma_smoke_renderer_draw_frame_rect(
    screensave_renderer *renderer,
    const screensave_recti *rect,
    screensave_color color
)
{
    (void)renderer;
    (void)rect;
    (void)color;
}

static void plasma_smoke_renderer_draw_line(
    screensave_renderer *renderer,
    const screensave_pointi *start_point,
    const screensave_pointi *end_point,
    screensave_color color
)
{
    (void)renderer;
    (void)start_point;
    (void)end_point;
    (void)color;
}

static void plasma_smoke_renderer_draw_polyline(
    screensave_renderer *renderer,
    const screensave_pointi *points,
    unsigned int point_count,
    screensave_color color
)
{
    (void)renderer;
    (void)points;
    (void)point_count;
    (void)color;
}

static int plasma_smoke_renderer_blit_bitmap(
    screensave_renderer *renderer,
    const screensave_bitmap_view *bitmap,
    const screensave_recti *destination_rect
)
{
    (void)renderer;
    (void)bitmap;
    (void)destination_rect;
    return 1;
}

static int plasma_smoke_renderer_end_frame(screensave_renderer *renderer)
{
    (void)renderer;
    return 1;
}

static void plasma_smoke_renderer_shutdown(screensave_renderer *renderer)
{
    (void)renderer;
}

static const screensave_renderer_vtable g_plasma_smoke_renderer_vtable = {
    plasma_smoke_renderer_begin_frame,
    plasma_smoke_renderer_clear,
    plasma_smoke_renderer_fill_rect,
    plasma_smoke_renderer_draw_frame_rect,
    plasma_smoke_renderer_draw_line,
    plasma_smoke_renderer_draw_polyline,
    plasma_smoke_renderer_blit_bitmap,
    plasma_smoke_renderer_end_frame,
    plasma_smoke_renderer_shutdown
};

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

static void plasma_smoke_init_fake_renderer(
    screensave_renderer *renderer,
    screensave_renderer_kind requested_kind,
    screensave_renderer_kind active_kind,
    const screensave_sizei *drawable_size
)
{
    screensave_renderer_info renderer_info;

    ZeroMemory(&renderer_info, sizeof(renderer_info));
    renderer_info.requested_kind = requested_kind;
    renderer_info.active_kind = active_kind;
    renderer_info.capability_flags = SCREENSAVE_RENDERER_CAP_CLEAR | SCREENSAVE_RENDERER_CAP_BITMAP;
    if (drawable_size != NULL) {
        renderer_info.drawable_size = *drawable_size;
    } else {
        renderer_info.drawable_size.width = 320;
        renderer_info.drawable_size.height = 240;
    }
    renderer_info.backend_name = "smoke-fake";
    renderer_info.status_text = "smoke-active";
    renderer_info.selection_reason = "smoke-forced";

    ZeroMemory(renderer, sizeof(*renderer));
    screensave_renderer_init_dispatch(renderer, &g_plasma_smoke_renderer_vtable, NULL, &renderer_info);
}

static int plasma_compile_plan_for_renderer(
    const screensave_saver_module *module,
    const char *preset_key,
    const char *theme_key,
    const plasma_selection_preferences *selection_preferences,
    screensave_renderer_kind requested_kind,
    screensave_renderer_kind active_kind,
    plasma_plan *plan_out
)
{
    screensave_common_config common_config;
    plasma_config product_config;
    screensave_config_binding binding;
    screensave_saver_environment environment;
    screensave_renderer renderer;
    screensave_sizei drawable_size;

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
    drawable_size.width = 320;
    drawable_size.height = 240;
    plasma_smoke_init_fake_renderer(&renderer, requested_kind, active_kind, &drawable_size);
    environment.mode = SCREENSAVE_SESSION_MODE_WINDOWED;
    environment.drawable_size = drawable_size;
    environment.seed.base_seed = 0x00000A55UL;
    environment.seed.stream_seed = 0x00000A77UL;
    environment.seed.deterministic = common_config.use_deterministic_seed;
    environment.config_binding = &binding;
    environment.renderer = &renderer;

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
    screensave_renderer fake_renderer;
    screensave_sizei fake_size;
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
        !(module->capability_flags & SCREENSAVE_SAVER_CAP_GL21) ||
        !(module->capability_flags & SCREENSAVE_SAVER_CAP_GL33) ||
        !(module->capability_flags & SCREENSAVE_SAVER_CAP_GL46) ||
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
        plan.requested_renderer_kind != SCREENSAVE_RENDERER_KIND_UNKNOWN ||
        plan.active_renderer_kind != SCREENSAVE_RENDERER_KIND_GDI ||
        !plan.advanced_capable ||
        plan.advanced_requested ||
        plan.advanced_enabled ||
        plan.advanced_degraded ||
        plan.advanced_components != 0UL ||
        plan.advanced_degrade_policy == 0UL ||
        !plan.modern_capable ||
        plan.modern_requested ||
        plan.modern_enabled ||
        plan.modern_degraded ||
        plan.modern_components != 0UL ||
        plan.modern_degrade_policy == 0UL ||
        !plan.premium_capable ||
        plan.premium_requested ||
        plan.premium_enabled ||
        plan.premium_degraded ||
        plan.premium_components != 0UL ||
        plan.premium_degrade_policy == 0UL ||
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
        plasma_plan_validate_for_renderer_kind(&plan, module, SCREENSAVE_RENDERER_KIND_GL33) ||
        plasma_plan_validate_for_renderer_kind(&plan, module, SCREENSAVE_RENDERER_KIND_GL46) ||
        plasma_plan_validate_for_renderer_kind(&plan, module, SCREENSAVE_RENDERER_KIND_UNKNOWN) ||
        !plasma_is_lower_band_kind(SCREENSAVE_RENDERER_KIND_GDI) ||
        !plasma_is_lower_band_kind(SCREENSAVE_RENDERER_KIND_GL11) ||
        plasma_is_lower_band_kind(SCREENSAVE_RENDERER_KIND_GL21) ||
        plasma_is_lower_band_kind(SCREENSAVE_RENDERER_KIND_GL33)
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

    if (
        !plasma_compile_plan_for_renderer(
            module,
            PLASMA_DEFAULT_PRESET_KEY,
            NULL,
            NULL,
            SCREENSAVE_RENDERER_KIND_GL21,
            SCREENSAVE_RENDERER_KIND_GL21,
            &plan
        )
    ) {
        return 32;
    }
    if (
        !plasma_plan_validate(&plan, module) ||
        plan.requested_renderer_kind != SCREENSAVE_RENDERER_KIND_GL21 ||
        plan.active_renderer_kind != SCREENSAVE_RENDERER_KIND_GL21 ||
        !plan.advanced_capable ||
        !plan.advanced_requested ||
        !plan.advanced_enabled ||
        plan.advanced_degraded ||
        plan.advanced_components != PLASMA_SMOKE_ADVANCED_COMPONENTS ||
        plan.filter_treatment != PLASMA_FILTER_TREATMENT_BLUR ||
        plan.accent_treatment != PLASMA_ACCENT_TREATMENT_OVERLAY_PASS ||
        !plan.modern_capable ||
        plan.modern_requested ||
        plan.modern_enabled ||
        plan.modern_degraded ||
        plan.modern_components != 0UL ||
        !plan.premium_capable ||
        plan.premium_requested ||
        plan.premium_enabled ||
        plan.premium_degraded ||
        plan.premium_components != 0UL ||
        plasma_plan_is_lower_band_baseline(&plan) ||
        !plasma_plan_validate_for_renderer_kind(&plan, module, SCREENSAVE_RENDERER_KIND_GL21) ||
        plasma_plan_validate_for_renderer_kind(&plan, module, SCREENSAVE_RENDERER_KIND_GL33) ||
        plasma_plan_validate_for_renderer_kind(&plan, module, SCREENSAVE_RENDERER_KIND_GL46) ||
        plasma_plan_validate_for_renderer_kind(&plan, module, SCREENSAVE_RENDERER_KIND_GDI) ||
        plasma_plan_validate_for_renderer_kind(&plan, module, SCREENSAVE_RENDERER_KIND_GL11)
    ) {
        return 33;
    }

    if (
        !plasma_compile_plan_for_renderer(
            module,
            PLASMA_DEFAULT_PRESET_KEY,
            NULL,
            NULL,
            SCREENSAVE_RENDERER_KIND_GL21,
            SCREENSAVE_RENDERER_KIND_GL11,
            &plan
        )
    ) {
        return 34;
    }
    if (
        !plasma_plan_validate(&plan, module) ||
        plan.requested_renderer_kind != SCREENSAVE_RENDERER_KIND_GL21 ||
        plan.active_renderer_kind != SCREENSAVE_RENDERER_KIND_GL11 ||
        !plan.advanced_capable ||
        !plan.advanced_requested ||
        plan.advanced_enabled ||
        !plan.advanced_degraded ||
        plan.advanced_components != 0UL ||
        plan.filter_treatment != PLASMA_FILTER_TREATMENT_NONE ||
        plan.accent_treatment != PLASMA_ACCENT_TREATMENT_NONE ||
        !plan.modern_capable ||
        plan.modern_requested ||
        plan.modern_enabled ||
        plan.modern_degraded ||
        plan.modern_components != 0UL ||
        !plan.premium_capable ||
        plan.premium_requested ||
        plan.premium_enabled ||
        plan.premium_degraded ||
        plan.premium_components != 0UL ||
        !plasma_plan_is_lower_band_baseline(&plan) ||
        !plasma_plan_validate_for_renderer_kind(&plan, module, SCREENSAVE_RENDERER_KIND_GDI) ||
        !plasma_plan_validate_for_renderer_kind(&plan, module, SCREENSAVE_RENDERER_KIND_GL11) ||
        plasma_plan_validate_for_renderer_kind(&plan, module, SCREENSAVE_RENDERER_KIND_GL21) ||
        plasma_plan_validate_for_renderer_kind(&plan, module, SCREENSAVE_RENDERER_KIND_GL33)
    ) {
        return 35;
    }

    if (
        !plasma_compile_plan_for_renderer(
            module,
            PLASMA_DEFAULT_PRESET_KEY,
            NULL,
            NULL,
            SCREENSAVE_RENDERER_KIND_GL33,
            SCREENSAVE_RENDERER_KIND_GL33,
            &plan
        )
    ) {
        return 36;
    }
    if (
        !plasma_plan_validate(&plan, module) ||
        plan.requested_renderer_kind != SCREENSAVE_RENDERER_KIND_GL33 ||
        plan.active_renderer_kind != SCREENSAVE_RENDERER_KIND_GL33 ||
        !plan.advanced_capable ||
        !plan.advanced_requested ||
        !plan.advanced_enabled ||
        plan.advanced_degraded ||
        plan.advanced_components != PLASMA_SMOKE_ADVANCED_COMPONENTS ||
        !plan.modern_capable ||
        !plan.modern_requested ||
        !plan.modern_enabled ||
        plan.modern_degraded ||
        plan.modern_components != PLASMA_SMOKE_MODERN_COMPONENTS ||
        !plan.premium_capable ||
        plan.premium_requested ||
        plan.premium_enabled ||
        plan.premium_degraded ||
        plan.premium_components != 0UL ||
        plan.filter_treatment != PLASMA_FILTER_TREATMENT_BLUR ||
        plan.accent_treatment != PLASMA_ACCENT_TREATMENT_OVERLAY_PASS ||
        plasma_plan_is_lower_band_baseline(&plan) ||
        !plasma_plan_validate_for_renderer_kind(&plan, module, SCREENSAVE_RENDERER_KIND_GL33) ||
        plasma_plan_validate_for_renderer_kind(&plan, module, SCREENSAVE_RENDERER_KIND_GL21) ||
        plasma_plan_validate_for_renderer_kind(&plan, module, SCREENSAVE_RENDERER_KIND_GL46) ||
        plasma_plan_validate_for_renderer_kind(&plan, module, SCREENSAVE_RENDERER_KIND_GDI) ||
        plasma_plan_validate_for_renderer_kind(&plan, module, SCREENSAVE_RENDERER_KIND_GL11)
    ) {
        return 37;
    }

    if (
        !plasma_compile_plan_for_renderer(
            module,
            PLASMA_DEFAULT_PRESET_KEY,
            NULL,
            NULL,
            SCREENSAVE_RENDERER_KIND_GL33,
            SCREENSAVE_RENDERER_KIND_GL21,
            &plan
        )
    ) {
        return 38;
    }
    if (
        !plasma_plan_validate(&plan, module) ||
        plan.requested_renderer_kind != SCREENSAVE_RENDERER_KIND_GL33 ||
        plan.active_renderer_kind != SCREENSAVE_RENDERER_KIND_GL21 ||
        !plan.advanced_capable ||
        !plan.advanced_requested ||
        !plan.advanced_enabled ||
        plan.advanced_degraded ||
        !plan.modern_capable ||
        !plan.modern_requested ||
        plan.modern_enabled ||
        !plan.modern_degraded ||
        plan.modern_components != 0UL ||
        !plan.premium_capable ||
        plan.premium_requested ||
        plan.premium_enabled ||
        plan.premium_degraded ||
        plan.premium_components != 0UL ||
        !plasma_plan_validate_for_renderer_kind(&plan, module, SCREENSAVE_RENDERER_KIND_GL21) ||
        plasma_plan_validate_for_renderer_kind(&plan, module, SCREENSAVE_RENDERER_KIND_GL33)
    ) {
        return 39;
    }

    if (
        !plasma_compile_plan_for_renderer(
            module,
            PLASMA_DEFAULT_PRESET_KEY,
            NULL,
            NULL,
            SCREENSAVE_RENDERER_KIND_GL33,
            SCREENSAVE_RENDERER_KIND_GL11,
            &plan
        )
    ) {
        return 40;
    }
    if (
        !plasma_plan_validate(&plan, module) ||
        plan.requested_renderer_kind != SCREENSAVE_RENDERER_KIND_GL33 ||
        plan.active_renderer_kind != SCREENSAVE_RENDERER_KIND_GL11 ||
        !plan.advanced_requested ||
        plan.advanced_enabled ||
        !plan.advanced_degraded ||
        !plan.modern_requested ||
        plan.modern_enabled ||
        !plan.modern_degraded ||
        !plan.premium_capable ||
        plan.premium_requested ||
        plan.premium_enabled ||
        plan.premium_degraded ||
        plan.premium_components != 0UL ||
        !plasma_plan_is_lower_band_baseline(&plan) ||
        !plasma_plan_validate_for_renderer_kind(&plan, module, SCREENSAVE_RENDERER_KIND_GDI) ||
        !plasma_plan_validate_for_renderer_kind(&plan, module, SCREENSAVE_RENDERER_KIND_GL11) ||
        plasma_plan_validate_for_renderer_kind(&plan, module, SCREENSAVE_RENDERER_KIND_GL21) ||
        plasma_plan_validate_for_renderer_kind(&plan, module, SCREENSAVE_RENDERER_KIND_GL33)
    ) {
        return 41;
    }

    if (
        !plasma_compile_plan_for_renderer(
            module,
            PLASMA_DEFAULT_PRESET_KEY,
            NULL,
            NULL,
            SCREENSAVE_RENDERER_KIND_GL46,
            SCREENSAVE_RENDERER_KIND_GL46,
            &plan
        )
    ) {
        return 42;
    }
    if (
        !plasma_plan_validate(&plan, module) ||
        plan.requested_renderer_kind != SCREENSAVE_RENDERER_KIND_GL46 ||
        plan.active_renderer_kind != SCREENSAVE_RENDERER_KIND_GL46 ||
        !plan.advanced_capable ||
        !plan.advanced_requested ||
        !plan.advanced_enabled ||
        !plan.modern_capable ||
        !plan.modern_requested ||
        !plan.modern_enabled ||
        !plan.premium_capable ||
        !plan.premium_requested ||
        !plan.premium_enabled ||
        plan.premium_degraded ||
        plan.premium_components != PLASMA_SMOKE_PREMIUM_COMPONENTS ||
        plan.presentation_mode != PLASMA_PRESENTATION_MODE_HEIGHTFIELD ||
        !plasma_plan_validate_for_renderer_kind(&plan, module, SCREENSAVE_RENDERER_KIND_GL46) ||
        plasma_plan_validate_for_renderer_kind(&plan, module, SCREENSAVE_RENDERER_KIND_GL33) ||
        plasma_plan_validate_for_renderer_kind(&plan, module, SCREENSAVE_RENDERER_KIND_GL21)
    ) {
        return 43;
    }

    if (
        !plasma_compile_plan_for_renderer(
            module,
            PLASMA_DEFAULT_PRESET_KEY,
            NULL,
            NULL,
            SCREENSAVE_RENDERER_KIND_GL46,
            SCREENSAVE_RENDERER_KIND_GL33,
            &plan
        )
    ) {
        return 44;
    }
    if (
        !plasma_plan_validate(&plan, module) ||
        plan.requested_renderer_kind != SCREENSAVE_RENDERER_KIND_GL46 ||
        plan.active_renderer_kind != SCREENSAVE_RENDERER_KIND_GL33 ||
        !plan.advanced_enabled ||
        !plan.modern_enabled ||
        !plan.premium_capable ||
        !plan.premium_requested ||
        plan.premium_enabled ||
        !plan.premium_degraded ||
        plan.premium_components != 0UL ||
        plan.presentation_mode != PLASMA_PRESENTATION_MODE_FLAT ||
        !plasma_plan_validate_for_renderer_kind(&plan, module, SCREENSAVE_RENDERER_KIND_GL33) ||
        plasma_plan_validate_for_renderer_kind(&plan, module, SCREENSAVE_RENDERER_KIND_GL46)
    ) {
        return 45;
    }

    if (
        !plasma_compile_plan_for_renderer(
            module,
            PLASMA_DEFAULT_PRESET_KEY,
            NULL,
            NULL,
            SCREENSAVE_RENDERER_KIND_GL46,
            SCREENSAVE_RENDERER_KIND_GL21,
            &plan
        )
    ) {
        return 46;
    }
    if (
        !plasma_plan_validate(&plan, module) ||
        plan.requested_renderer_kind != SCREENSAVE_RENDERER_KIND_GL46 ||
        plan.active_renderer_kind != SCREENSAVE_RENDERER_KIND_GL21 ||
        !plan.advanced_enabled ||
        !plan.modern_requested ||
        plan.modern_enabled ||
        !plan.modern_degraded ||
        !plan.premium_requested ||
        plan.premium_enabled ||
        !plan.premium_degraded ||
        plan.presentation_mode != PLASMA_PRESENTATION_MODE_FLAT ||
        !plasma_plan_validate_for_renderer_kind(&plan, module, SCREENSAVE_RENDERER_KIND_GL21) ||
        plasma_plan_validate_for_renderer_kind(&plan, module, SCREENSAVE_RENDERER_KIND_GL46)
    ) {
        return 47;
    }

    if (
        !plasma_compile_plan_for_renderer(
            module,
            PLASMA_DEFAULT_PRESET_KEY,
            NULL,
            NULL,
            SCREENSAVE_RENDERER_KIND_GL46,
            SCREENSAVE_RENDERER_KIND_GL11,
            &plan
        )
    ) {
        return 48;
    }
    if (
        !plasma_plan_validate(&plan, module) ||
        plan.requested_renderer_kind != SCREENSAVE_RENDERER_KIND_GL46 ||
        plan.active_renderer_kind != SCREENSAVE_RENDERER_KIND_GL11 ||
        !plan.advanced_requested ||
        plan.advanced_enabled ||
        !plan.advanced_degraded ||
        !plan.modern_requested ||
        plan.modern_enabled ||
        !plan.modern_degraded ||
        !plan.premium_requested ||
        plan.premium_enabled ||
        !plan.premium_degraded ||
        !plasma_plan_is_lower_band_baseline(&plan) ||
        !plasma_plan_validate_for_renderer_kind(&plan, module, SCREENSAVE_RENDERER_KIND_GL11) ||
        plasma_plan_validate_for_renderer_kind(&plan, module, SCREENSAVE_RENDERER_KIND_GL46)
    ) {
        return 49;
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
        strcmp(pack_entry->assets[1].canonical_key, "plasma_lava") != 0 ||
        plasma_content_find_preset_entry("plasma_lava") == NULL ||
        !plasma_content_find_preset_entry("plasma_lava")->advanced_capable ||
        !plasma_content_find_preset_entry("plasma_lava")->modern_capable ||
        !plasma_content_find_preset_entry("plasma_lava")->premium_capable
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
        !session->plan.modern_capable ||
        session->plan.modern_requested ||
        session->plan.modern_enabled ||
        session->plan.modern_degraded ||
        !session->plan.premium_capable ||
        session->plan.premium_requested ||
        session->plan.premium_enabled ||
        session->plan.premium_degraded ||
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

    screensave_config_binding_init(&binding, &common_config, &product_config, sizeof(product_config));
    ZeroMemory(&environment, sizeof(environment));
    fake_size.width = 320;
    fake_size.height = 240;
    plasma_smoke_init_fake_renderer(
        &fake_renderer,
        SCREENSAVE_RENDERER_KIND_GL21,
        SCREENSAVE_RENDERER_KIND_GL21,
        &fake_size
    );
    environment.mode = SCREENSAVE_SESSION_MODE_WINDOWED;
    environment.drawable_size = fake_size;
    environment.seed.base_seed = 0x01020304UL;
    environment.seed.stream_seed = 0x05060708UL;
    environment.seed.deterministic = common_config.use_deterministic_seed;
    environment.config_binding = &binding;
    environment.renderer = &fake_renderer;

    session = NULL;
    if (!plasma_create_session(module, &session, &environment) || session == NULL) {
        return 114;
    }
    if (
        session->state.active_renderer_kind != SCREENSAVE_RENDERER_KIND_GL21 ||
        session->plan.active_renderer_kind != SCREENSAVE_RENDERER_KIND_GL21 ||
        !session->plan.advanced_enabled ||
        session->plan.advanced_degraded ||
        !session->plan.modern_capable ||
        session->plan.modern_requested ||
        session->plan.modern_enabled ||
        session->plan.modern_degraded ||
        !session->plan.premium_capable ||
        session->plan.premium_requested ||
        session->plan.premium_enabled ||
        session->plan.premium_degraded ||
        session->state.field_history == NULL ||
        session->state.advanced_treatment_buffer.pixels == NULL ||
        session->state.modern_treatment_buffer.pixels != NULL ||
        session->state.modern_presentation_buffer.pixels != NULL ||
        session->state.premium_treatment_buffer.pixels != NULL ||
        session->state.premium_presentation_buffer.pixels != NULL ||
        !plasma_plan_validate_for_renderer_kind(&session->plan, module, SCREENSAVE_RENDERER_KIND_GL21) ||
        plasma_plan_validate_lower_band_baseline(&session->plan, module)
    ) {
        plasma_destroy_session(session);
        return 115;
    }

    environment.clock.delta_millis = 33UL;
    plasma_step_session(session, &environment);
    if (!plasma_output_build(&session->plan, &session->state, &output_frame)) {
        plasma_destroy_session(session);
        return 116;
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
        return 117;
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
        return 118;
    }
    plasma_render_session(session, &environment);
    if (
        session->state.visual_buffer.pixels == NULL ||
        session->state.advanced_treatment_buffer.pixels == NULL ||
        session->state.field_history == NULL ||
        session->state.modern_treatment_buffer.pixels != NULL ||
        session->state.modern_presentation_buffer.pixels != NULL ||
        session->state.premium_treatment_buffer.pixels != NULL ||
        session->state.premium_presentation_buffer.pixels != NULL
    ) {
        plasma_destroy_session(session);
        return 119;
    }

    plan = session->plan;
    plan.active_renderer_kind = SCREENSAVE_RENDERER_KIND_GL11;
    if (
        plasma_plan_validate_for_renderer_kind(&plan, module, SCREENSAVE_RENDERER_KIND_GL21) ||
        plasma_plan_validate(&plan, module)
    ) {
        plasma_destroy_session(session);
        return 120;
    }

    plasma_destroy_session(session);

    screensave_config_binding_init(&binding, &common_config, &product_config, sizeof(product_config));
    ZeroMemory(&environment, sizeof(environment));
    fake_size.width = 320;
    fake_size.height = 240;
    plasma_smoke_init_fake_renderer(
        &fake_renderer,
        SCREENSAVE_RENDERER_KIND_GL33,
        SCREENSAVE_RENDERER_KIND_GL33,
        &fake_size
    );
    environment.mode = SCREENSAVE_SESSION_MODE_WINDOWED;
    environment.drawable_size = fake_size;
    environment.seed.base_seed = 0x11121314UL;
    environment.seed.stream_seed = 0x15161718UL;
    environment.seed.deterministic = common_config.use_deterministic_seed;
    environment.config_binding = &binding;
    environment.renderer = &fake_renderer;

    session = NULL;
    if (!plasma_create_session(module, &session, &environment) || session == NULL) {
        return 121;
    }
    if (
        session->state.active_renderer_kind != SCREENSAVE_RENDERER_KIND_GL33 ||
        session->plan.active_renderer_kind != SCREENSAVE_RENDERER_KIND_GL33 ||
        !session->plan.advanced_enabled ||
        !session->plan.modern_enabled ||
        session->plan.advanced_degraded ||
        session->plan.modern_degraded ||
        !session->plan.premium_capable ||
        session->plan.premium_requested ||
        session->plan.premium_enabled ||
        session->plan.premium_degraded ||
        session->state.field_history == NULL ||
        session->state.advanced_treatment_buffer.pixels == NULL ||
        session->state.modern_treatment_buffer.pixels == NULL ||
        session->state.modern_presentation_buffer.pixels == NULL ||
        session->state.premium_treatment_buffer.pixels != NULL ||
        session->state.premium_presentation_buffer.pixels != NULL ||
        !plasma_plan_validate_for_renderer_kind(&session->plan, module, SCREENSAVE_RENDERER_KIND_GL33) ||
        plasma_plan_validate_for_renderer_kind(&session->plan, module, SCREENSAVE_RENDERER_KIND_GL21) ||
        plasma_plan_validate_for_renderer_kind(&session->plan, module, SCREENSAVE_RENDERER_KIND_GL46) ||
        plasma_plan_validate_lower_band_baseline(&session->plan, module)
    ) {
        plasma_destroy_session(session);
        return 122;
    }

    environment.clock.delta_millis = 33UL;
    plasma_step_session(session, &environment);
    if (!plasma_output_build(&session->plan, &session->state, &output_frame)) {
        plasma_destroy_session(session);
        return 123;
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
        return 124;
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
        return 125;
    }
    plasma_render_session(session, &environment);
    if (
        presentation_target.bitmap_view.pixels != (const void *)session->state.modern_presentation_buffer.pixels ||
        session->state.modern_treatment_buffer.pixels == NULL ||
        session->state.modern_presentation_buffer.pixels == NULL ||
        session->state.premium_treatment_buffer.pixels != NULL ||
        session->state.premium_presentation_buffer.pixels != NULL
    ) {
        plasma_destroy_session(session);
        return 126;
    }

    plan = session->plan;
    plan.active_renderer_kind = SCREENSAVE_RENDERER_KIND_GL21;
    if (
        plasma_plan_validate_for_renderer_kind(&plan, module, SCREENSAVE_RENDERER_KIND_GL33) ||
        plasma_plan_validate(&plan, module)
    ) {
        plasma_destroy_session(session);
        return 127;
    }

    plasma_destroy_session(session);

    screensave_config_binding_init(&binding, &common_config, &product_config, sizeof(product_config));
    ZeroMemory(&environment, sizeof(environment));
    fake_size.width = 320;
    fake_size.height = 240;
    plasma_smoke_init_fake_renderer(
        &fake_renderer,
        SCREENSAVE_RENDERER_KIND_GL46,
        SCREENSAVE_RENDERER_KIND_GL46,
        &fake_size
    );
    environment.mode = SCREENSAVE_SESSION_MODE_WINDOWED;
    environment.drawable_size = fake_size;
    environment.seed.base_seed = 0x21222324UL;
    environment.seed.stream_seed = 0x25262728UL;
    environment.seed.deterministic = common_config.use_deterministic_seed;
    environment.config_binding = &binding;
    environment.renderer = &fake_renderer;

    session = NULL;
    if (!plasma_create_session(module, &session, &environment) || session == NULL) {
        return 128;
    }
    if (
        session->state.active_renderer_kind != SCREENSAVE_RENDERER_KIND_GL46 ||
        session->plan.active_renderer_kind != SCREENSAVE_RENDERER_KIND_GL46 ||
        !session->plan.advanced_enabled ||
        !session->plan.modern_enabled ||
        !session->plan.premium_enabled ||
        session->plan.advanced_degraded ||
        session->plan.modern_degraded ||
        session->plan.premium_degraded ||
        session->plan.presentation_mode != PLASMA_PRESENTATION_MODE_HEIGHTFIELD ||
        session->state.field_history == NULL ||
        session->state.advanced_treatment_buffer.pixels == NULL ||
        session->state.modern_treatment_buffer.pixels == NULL ||
        session->state.modern_presentation_buffer.pixels == NULL ||
        session->state.premium_treatment_buffer.pixels == NULL ||
        session->state.premium_presentation_buffer.pixels == NULL ||
        !plasma_plan_validate_for_renderer_kind(&session->plan, module, SCREENSAVE_RENDERER_KIND_GL46) ||
        plasma_plan_validate_for_renderer_kind(&session->plan, module, SCREENSAVE_RENDERER_KIND_GL33) ||
        plasma_plan_validate_lower_band_baseline(&session->plan, module)
    ) {
        plasma_destroy_session(session);
        return 129;
    }

    environment.clock.delta_millis = 33UL;
    plasma_step_session(session, &environment);
    if (!plasma_output_build(&session->plan, &session->state, &output_frame)) {
        plasma_destroy_session(session);
        return 130;
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
        return 131;
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
        return 132;
    }
    plasma_render_session(session, &environment);
    if (
        presentation_target.bitmap_view.pixels != (const void *)session->state.premium_presentation_buffer.pixels ||
        presentation_target.bitmap_view.size.width != session->state.field_size.width ||
        presentation_target.bitmap_view.size.height != session->state.field_size.height ||
        session->state.premium_treatment_buffer.pixels == NULL ||
        session->state.premium_presentation_buffer.pixels == NULL
    ) {
        plasma_destroy_session(session);
        return 133;
    }

    plan = session->plan;
    plan.active_renderer_kind = SCREENSAVE_RENDERER_KIND_GL33;
    if (
        plasma_plan_validate_for_renderer_kind(&plan, module, SCREENSAVE_RENDERER_KIND_GL46) ||
        plasma_plan_validate(&plan, module)
    ) {
        plasma_destroy_session(session);
        return 134;
    }

    plasma_destroy_session(session);
    return 0;
}
