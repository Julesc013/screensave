#include <stdlib.h>
#include <string.h>

#include "screensave/saver_api.h"
#include "../config/settings_internal.h"

int screensave_saver_module_is_valid(const screensave_saver_module *module)
{
    const screensave_saver_config_hooks *config_hooks;

    if (module == NULL) {
        return 0;
    }

    if (
        module->identity.product_key == NULL ||
        module->identity.display_name == NULL ||
        module->identity.product_key[0] == '\0' ||
        module->identity.display_name[0] == '\0'
    ) {
        return 0;
    }

    if ((module->capability_flags & SCREENSAVE_SAVER_CAP_GDI) == 0UL) {
        return 0;
    }

    if (module->preset_count > 0U && module->presets == NULL) {
        return 0;
    }

    if (module->theme_count > 0U && module->themes == NULL) {
        return 0;
    }

    config_hooks = module->config_hooks;
    if (config_hooks != NULL) {
        if (config_hooks->set_defaults == NULL || config_hooks->clamp == NULL) {
            return 0;
        }
    }

    return 1;
}

static const screensave_saver_config_hooks *screensave_saver_get_config_hooks(
    const screensave_saver_module *module
)
{
    if (module == NULL) {
        return NULL;
    }

    return module->config_hooks;
}

static unsigned long screensave_saver_mix_seed(unsigned long value)
{
    return (value * 1664525UL) + 1013904223UL;
}

int screensave_saver_config_state_init(
    const screensave_saver_module *module,
    screensave_saver_config_state *config_state
)
{
    const screensave_saver_config_hooks *config_hooks;

    if (config_state == NULL) {
        return 0;
    }

    ZeroMemory(config_state, sizeof(*config_state));
    config_hooks = screensave_saver_get_config_hooks(module);
    if (config_hooks == NULL || config_hooks->product_config_size == 0U) {
        return 1;
    }

    config_state->product_config = malloc(config_hooks->product_config_size);
    if (config_state->product_config == NULL) {
        return 0;
    }

    config_state->product_config_size = config_hooks->product_config_size;
    ZeroMemory(config_state->product_config, config_state->product_config_size);
    return 1;
}

int screensave_saver_config_state_copy(
    const screensave_saver_module *module,
    screensave_saver_config_state *target,
    const screensave_saver_config_state *source
)
{
    const screensave_saver_config_hooks *config_hooks;

    if (target == NULL || source == NULL) {
        return 0;
    }

    config_hooks = screensave_saver_get_config_hooks(module);
    if (
        target->product_config == NULL &&
        config_hooks != NULL &&
        config_hooks->product_config_size > 0U &&
        !screensave_saver_config_state_init(module, target)
    ) {
        return 0;
    }

    target->common = source->common;
    if (
        target->product_config != NULL &&
        source->product_config != NULL &&
        target->product_config_size == source->product_config_size &&
        source->product_config_size > 0U
    ) {
        memcpy(target->product_config, source->product_config, source->product_config_size);
    }

    return 1;
}

void screensave_saver_config_state_dispose(screensave_saver_config_state *config_state)
{
    if (config_state == NULL) {
        return;
    }

    if (config_state->product_config != NULL) {
        free(config_state->product_config);
    }

    ZeroMemory(config_state, sizeof(*config_state));
}

void screensave_saver_config_state_set_defaults(
    const screensave_saver_module *module,
    screensave_saver_config_state *config_state
)
{
    const screensave_saver_config_hooks *config_hooks;

    if (config_state == NULL) {
        return;
    }

    screensave_common_config_set_defaults(&config_state->common);
    if (config_state->product_config != NULL && config_state->product_config_size > 0U) {
        ZeroMemory(config_state->product_config, config_state->product_config_size);
    }

    config_hooks = screensave_saver_get_config_hooks(module);
    if (config_hooks != NULL && config_hooks->set_defaults != NULL) {
        config_hooks->set_defaults(
            &config_state->common,
            config_state->product_config,
            config_state->product_config_size
        );
    }
}

void screensave_saver_config_state_clamp(
    const screensave_saver_module *module,
    screensave_saver_config_state *config_state
)
{
    const screensave_saver_config_hooks *config_hooks;

    if (config_state == NULL) {
        return;
    }

    screensave_common_config_clamp(&config_state->common);
    config_hooks = screensave_saver_get_config_hooks(module);
    if (config_hooks != NULL && config_hooks->clamp != NULL) {
        config_hooks->clamp(
            &config_state->common,
            config_state->product_config,
            config_state->product_config_size
        );
    }
}

int screensave_saver_config_state_load(
    const screensave_saver_module *module,
    screensave_saver_config_state *config_state,
    screensave_diag_context *diagnostics
)
{
    const screensave_saver_config_hooks *config_hooks;

    if (config_state == NULL) {
        return 0;
    }

    config_hooks = screensave_saver_get_config_hooks(module);
    if (config_hooks == NULL || config_hooks->load_config == NULL) {
        return screensave_settings_load_shared_state(module, &config_state->common, diagnostics);
    }

    if (!config_hooks->load_config(
        module,
        &config_state->common,
        config_state->product_config,
        config_state->product_config_size,
        diagnostics
    )) {
        return 0;
    }

    return screensave_settings_load_shared_state(module, &config_state->common, diagnostics);
}

int screensave_saver_config_state_save(
    const screensave_saver_module *module,
    const screensave_saver_config_state *config_state,
    screensave_diag_context *diagnostics
)
{
    const screensave_saver_config_hooks *config_hooks;

    if (config_state == NULL) {
        return 0;
    }

    config_hooks = screensave_saver_get_config_hooks(module);
    if (config_hooks == NULL || config_hooks->save_config == NULL) {
        return screensave_settings_save_shared_state(module, &config_state->common, diagnostics);
    }

    if (!config_hooks->save_config(
            module,
            &config_state->common,
            config_state->product_config,
            config_state->product_config_size,
            diagnostics
        )) {
        return 0;
    }

    return screensave_settings_save_shared_state(module, &config_state->common, diagnostics);
}

int screensave_saver_config_state_resolve_for_session(
    const screensave_saver_module *module,
    const screensave_saver_config_state *stored_state,
    const screensave_session_seed *seed,
    screensave_saver_config_state *resolved_state,
    screensave_diag_context *diagnostics
)
{
    const screensave_saver_config_hooks *config_hooks;
    unsigned long random_value;

    if (module == NULL || stored_state == NULL || resolved_state == NULL) {
        return 0;
    }

    if (!screensave_saver_config_state_copy(module, resolved_state, stored_state)) {
        return 0;
    }

    config_hooks = screensave_saver_get_config_hooks(module);
    if (
        seed == NULL ||
        resolved_state->common.randomization_mode != SCREENSAVE_RANDOMIZATION_MODE_SESSION
    ) {
        screensave_saver_config_state_clamp(module, resolved_state);
        return 1;
    }

    random_value = seed->stream_seed;
    if (
        config_hooks != NULL &&
        config_hooks->apply_preset != NULL &&
        module->preset_count > 0U &&
        (resolved_state->common.randomization_scope & SCREENSAVE_RANDOMIZATION_SCOPE_PRESET) != 0UL
    ) {
        unsigned int preset_index;

        preset_index = (unsigned int)(random_value % module->preset_count);
        config_hooks->apply_preset(
            module,
            module->presets[preset_index].preset_key,
            &resolved_state->common,
            resolved_state->product_config,
            resolved_state->product_config_size
        );
        random_value = screensave_saver_mix_seed(random_value);
    }

    if (
        module->theme_count > 0U &&
        (resolved_state->common.randomization_scope & SCREENSAVE_RANDOMIZATION_SCOPE_THEME) != 0UL
    ) {
        unsigned int theme_index;

        theme_index = (unsigned int)(random_value % module->theme_count);
        resolved_state->common.theme_key = module->themes[theme_index].theme_key;
        random_value = screensave_saver_mix_seed(random_value);
    }

    if ((resolved_state->common.randomization_scope & SCREENSAVE_RANDOMIZATION_SCOPE_DETAIL) != 0UL) {
        resolved_state->common.detail_level = (screensave_detail_level)(random_value % 3UL);
        random_value = screensave_saver_mix_seed(random_value);
    }

    if (
        config_hooks != NULL &&
        config_hooks->randomize_settings != NULL &&
        (resolved_state->common.randomization_scope & SCREENSAVE_RANDOMIZATION_SCOPE_PRODUCT) != 0UL
    ) {
        screensave_session_seed random_seed;

        random_seed = *seed;
        random_seed.stream_seed = random_value;
        config_hooks->randomize_settings(
            module,
            &resolved_state->common,
            resolved_state->product_config,
            resolved_state->product_config_size,
            &random_seed,
            diagnostics
        );
    }

    screensave_saver_config_state_clamp(module, resolved_state);
    return 1;
}

const char *screensave_session_mode_name(screensave_session_mode mode)
{
    switch (mode) {
    case SCREENSAVE_SESSION_MODE_SCREEN:
        return "screen";

    case SCREENSAVE_SESSION_MODE_PREVIEW:
        return "preview";

    case SCREENSAVE_SESSION_MODE_WINDOWED:
        return "windowed";

    case SCREENSAVE_SESSION_MODE_CONFIG:
    default:
        return "config";
    }
}
