#include "suite_internal.h"

static void suite_show_message(HWND owner_window, const char *text, UINT type)
{
    MessageBoxA(owner_window, text, SUITE_APP_TITLEA, type | MB_SETFOREGROUND);
}

static int suite_copy_config_state(
    const screensave_saver_module *module,
    screensave_saver_config_state *target,
    const screensave_saver_config_state *source
)
{
    return screensave_saver_config_state_copy(module, target, source);
}

static void suite_update_binding(suite_app *app)
{
    if (app == NULL) {
        return;
    }

    screensave_config_binding_init(
        &app->config_binding,
        &app->resolved_config.common,
        app->resolved_config.product_config,
        app->resolved_config.product_config_size
    );
}

static int suite_preset_descriptor_apply(
    const screensave_preset_descriptor *preset_descriptor,
    screensave_common_config *common_config
)
{
    if (preset_descriptor == NULL || common_config == NULL) {
        return 0;
    }

    common_config->preset_key = preset_descriptor->preset_key;
    common_config->detail_level = preset_descriptor->detail_level;
    common_config->theme_key = preset_descriptor->theme_key;
    common_config->use_deterministic_seed = preset_descriptor->use_fixed_seed != 0;
    if (preset_descriptor->use_fixed_seed) {
        common_config->deterministic_seed = preset_descriptor->fixed_seed;
    }

    return 1;
}

static const char *suite_preset_display_name(const screensave_preset_descriptor *preset_descriptor)
{
    if (
        preset_descriptor != NULL &&
        preset_descriptor->display_name != NULL &&
        preset_descriptor->display_name[0] != '\0'
    ) {
        return preset_descriptor->display_name;
    }

    return preset_descriptor != NULL ? preset_descriptor->preset_key : "";
}

static const char *suite_theme_display_name(const screensave_theme_descriptor *theme_descriptor)
{
    if (
        theme_descriptor != NULL &&
        theme_descriptor->display_name != NULL &&
        theme_descriptor->display_name[0] != '\0'
    ) {
        return theme_descriptor->display_name;
    }

    return theme_descriptor != NULL ? theme_descriptor->theme_key : "";
}

static int suite_select_preset_combo_by_key(
    HWND combo,
    const screensave_saver_module *module,
    const char *preset_key
)
{
    unsigned int index;

    if (combo == NULL || module == NULL || preset_key == NULL || preset_key[0] == '\0') {
        return 0;
    }

    for (index = 0U; index < module->preset_count; ++index) {
        if (
            module->presets[index].preset_key != NULL &&
            lstrcmpiA(module->presets[index].preset_key, preset_key) == 0
        ) {
            SendMessageA(combo, CB_SETCURSEL, (WPARAM)index, 0L);
            return 1;
        }
    }

    return 0;
}

static int suite_select_theme_combo_by_key(
    HWND combo,
    const screensave_saver_module *module,
    const char *theme_key
)
{
    unsigned int index;

    if (combo == NULL || module == NULL || theme_key == NULL || theme_key[0] == '\0') {
        return 0;
    }

    for (index = 0U; index < module->theme_count; ++index) {
        if (
            module->themes[index].theme_key != NULL &&
            lstrcmpiA(module->themes[index].theme_key, theme_key) == 0
        ) {
            SendMessageA(combo, CB_SETCURSEL, (WPARAM)index, 0L);
            return 1;
        }
    }

    return 0;
}

static int suite_renderer_combo_to_kind(HWND combo)
{
    LRESULT selection;

    if (combo == NULL) {
        return (int)SCREENSAVE_RENDERER_KIND_UNKNOWN;
    }

    selection = SendMessageA(combo, CB_GETCURSEL, 0U, 0L);
    switch ((int)selection) {
    case 1:
        return (int)SCREENSAVE_RENDERER_KIND_GDI;

    case 2:
        return (int)SCREENSAVE_RENDERER_KIND_GL11;

    case 3:
        return (int)SCREENSAVE_RENDERER_KIND_GL21;

    case 4:
        return (int)SCREENSAVE_RENDERER_KIND_GL33;

    case 5:
        return (int)SCREENSAVE_RENDERER_KIND_GL46;

    case 0:
    default:
        return (int)SCREENSAVE_RENDERER_KIND_UNKNOWN;
    }
}

static void suite_renderer_kind_to_combo(HWND combo, int renderer_request)
{
    WPARAM selection;

    if (combo == NULL) {
        return;
    }

    selection = 0U;
    switch ((screensave_renderer_kind)renderer_request) {
    case SCREENSAVE_RENDERER_KIND_GDI:
        selection = 1U;
        break;

    case SCREENSAVE_RENDERER_KIND_GL11:
        selection = 2U;
        break;

    case SCREENSAVE_RENDERER_KIND_GL21:
        selection = 3U;
        break;

    case SCREENSAVE_RENDERER_KIND_GL33:
        selection = 4U;
        break;

    case SCREENSAVE_RENDERER_KIND_GL46:
        selection = 5U;
        break;

    case SCREENSAVE_RENDERER_KIND_NULL:
    case SCREENSAVE_RENDERER_KIND_UNKNOWN:
    default:
        selection = 0U;
        break;
    }

    SendMessageA(combo, CB_SETCURSEL, selection, 0L);
}

void suite_dispose_selected_config(suite_app *app)
{
    if (app == NULL) {
        return;
    }

    screensave_saver_config_state_dispose(&app->stored_config);
    screensave_saver_config_state_dispose(&app->working_config);
    screensave_saver_config_state_dispose(&app->resolved_config);
    ZeroMemory(&app->config_binding, sizeof(app->config_binding));
}

int suite_select_entry(suite_app *app, unsigned int index)
{
    const suite_catalog_entry *entry;
    const screensave_saver_module *module;

    if (app == NULL || index >= app->catalog_count) {
        return 0;
    }

    entry = &app->catalog[index];
    module = entry->module;
    if (!screensave_saver_module_is_valid(module)) {
        return 0;
    }

    suite_preview_stop(app);
    suite_dispose_selected_config(app);

    if (
        !screensave_saver_config_state_init(module, &app->stored_config) ||
        !screensave_saver_config_state_init(module, &app->working_config) ||
        !screensave_saver_config_state_init(module, &app->resolved_config)
    ) {
        suite_dispose_selected_config(app);
        suite_show_message(
            app->main_window,
            "Suite could not initialize the selected saver configuration state.",
            MB_OK | MB_ICONERROR
        );
        return 0;
    }

    screensave_saver_config_state_set_defaults(module, &app->stored_config);
    if (!screensave_saver_config_state_load(module, &app->stored_config, &app->diagnostics)) {
        screensave_diag_emit(
            &app->diagnostics,
            SCREENSAVE_DIAG_LEVEL_WARNING,
            SCREENSAVE_DIAG_DOMAIN_APP,
            9001UL,
            "suite_select_entry",
            "Suite could not load stored saver settings; product defaults are active."
        );
    }
    screensave_saver_config_state_clamp(module, &app->stored_config);
    (void)suite_copy_config_state(module, &app->working_config, &app->stored_config);
    (void)suite_copy_config_state(module, &app->resolved_config, &app->working_config);
    screensave_saver_config_state_clamp(module, &app->resolved_config);
    suite_update_binding(app);

    app->selected_index = index;
    lstrcpynA(app->app_config.product_key, entry->manifest.key, sizeof(app->app_config.product_key));
    if (app->browser_window != NULL) {
        SendMessageA(app->browser_window, LB_SETCURSEL, (WPARAM)index, 0L);
    }

    suite_sync_controls_from_working_config(app);
    (void)suite_preview_restart(app, 0);
    if (app->preview_window != NULL) {
        InvalidateRect(app->preview_window, NULL, FALSE);
    }
    if (app->info_window != NULL) {
        InvalidateRect(app->info_window, NULL, TRUE);
    }
    if (app->main_window != NULL) {
        InvalidateRect(app->main_window, NULL, FALSE);
    }

    return 1;
}

void suite_reset_working_config(suite_app *app)
{
    const screensave_saver_module *module;

    if (app == NULL) {
        return;
    }

    module = suite_get_selected_module(app);
    if (module == NULL) {
        return;
    }

    (void)suite_copy_config_state(module, &app->working_config, &app->stored_config);
    (void)suite_copy_config_state(module, &app->resolved_config, &app->working_config);
    screensave_saver_config_state_clamp(module, &app->working_config);
    screensave_saver_config_state_clamp(module, &app->resolved_config);
    suite_update_binding(app);
    suite_sync_controls_from_working_config(app);
    (void)suite_preview_restart(app, 0);
}

int suite_handle_preset_combo_change(suite_app *app)
{
    const screensave_saver_module *module;
    LRESULT selection;
    const screensave_preset_descriptor *preset_descriptor;
    int overlay_enabled;
    int deterministic_enabled;
    unsigned long deterministic_seed;
    screensave_randomization_mode randomization_mode;
    unsigned long randomization_scope;

    if (app == NULL || app->preset_combo == NULL) {
        return 0;
    }

    module = suite_get_selected_module(app);
    if (module == NULL) {
        return 0;
    }

    selection = SendMessageA(app->preset_combo, CB_GETCURSEL, 0U, 0L);
    if (selection == CB_ERR) {
        return 0;
    }

    selection = SendMessageA(app->preset_combo, CB_GETITEMDATA, (WPARAM)selection, 0L);
    if (selection == CB_ERR || (unsigned int)selection >= module->preset_count) {
        return 0;
    }

    preset_descriptor = &module->presets[(unsigned int)selection];
    overlay_enabled = app->working_config.common.diagnostics_overlay_enabled;
    deterministic_enabled = app->working_config.common.use_deterministic_seed;
    deterministic_seed = app->working_config.common.deterministic_seed;
    randomization_mode = app->working_config.common.randomization_mode;
    randomization_scope = app->working_config.common.randomization_scope;

    screensave_saver_config_state_set_defaults(module, &app->working_config);
    if (module->config_hooks != NULL && module->config_hooks->apply_preset != NULL) {
        module->config_hooks->apply_preset(
            module,
            preset_descriptor->preset_key,
            &app->working_config.common,
            app->working_config.product_config,
            app->working_config.product_config_size
        );
    } else {
        (void)suite_preset_descriptor_apply(preset_descriptor, &app->working_config.common);
    }

    app->working_config.common.diagnostics_overlay_enabled = overlay_enabled;
    app->working_config.common.use_deterministic_seed = deterministic_enabled;
    app->working_config.common.deterministic_seed = deterministic_seed;
    app->working_config.common.randomization_mode = randomization_mode;
    app->working_config.common.randomization_scope = randomization_scope;
    screensave_saver_config_state_clamp(module, &app->working_config);
    (void)suite_copy_config_state(module, &app->resolved_config, &app->working_config);
    suite_update_binding(app);
    suite_sync_controls_from_working_config(app);
    (void)suite_preview_restart(app, 0);
    return 1;
}

int suite_handle_theme_combo_change(suite_app *app)
{
    const screensave_saver_module *module;
    LRESULT selection;

    if (app == NULL || app->theme_combo == NULL) {
        return 0;
    }

    module = suite_get_selected_module(app);
    if (module == NULL) {
        return 0;
    }

    selection = SendMessageA(app->theme_combo, CB_GETCURSEL, 0U, 0L);
    if (selection == CB_ERR) {
        return 0;
    }

    selection = SendMessageA(app->theme_combo, CB_GETITEMDATA, (WPARAM)selection, 0L);
    if (selection == CB_ERR || (unsigned int)selection >= module->theme_count) {
        return 0;
    }

    app->working_config.common.theme_key = module->themes[(unsigned int)selection].theme_key;
    screensave_saver_config_state_clamp(module, &app->working_config);
    (void)suite_copy_config_state(module, &app->resolved_config, &app->working_config);
    suite_update_binding(app);
    suite_sync_controls_from_working_config(app);
    (void)suite_preview_restart(app, 0);
    return 1;
}

int suite_handle_renderer_combo_change(suite_app *app)
{
    int renderer_request;

    if (app == NULL || app->renderer_combo == NULL) {
        return 0;
    }

    renderer_request = suite_renderer_combo_to_kind(app->renderer_combo);
    if (renderer_request == app->app_config.renderer_request) {
        return 1;
    }

    app->app_config.renderer_request = renderer_request;
    suite_app_config_clamp(&app->app_config);
    (void)suite_preview_restart(app, 0);
    if (app->info_window != NULL) {
        InvalidateRect(app->info_window, NULL, TRUE);
    }
    return 1;
}

int suite_handle_randomization_toggle(suite_app *app)
{
    const screensave_saver_module *module;

    if (app == NULL || app->randomize_check == NULL) {
        return 0;
    }

    module = suite_get_selected_module(app);
    if (module == NULL) {
        return 0;
    }

    if (IsDlgButtonChecked(app->main_window, IDC_SUITE_RANDOMIZE) == BST_CHECKED) {
        app->working_config.common.randomization_mode = SCREENSAVE_RANDOMIZATION_MODE_SESSION;
        app->working_config.common.randomization_scope = screensave_randomization_default_scope();
    } else {
        app->working_config.common.randomization_mode = SCREENSAVE_RANDOMIZATION_MODE_OFF;
        app->working_config.common.randomization_scope = 0UL;
    }

    screensave_saver_config_state_clamp(module, &app->working_config);
    (void)suite_copy_config_state(module, &app->resolved_config, &app->working_config);
    suite_update_binding(app);
    suite_sync_controls_from_working_config(app);
    (void)suite_preview_restart(app, 1);
    return 1;
}

int suite_save_working_config(suite_app *app)
{
    const screensave_saver_module *module;

    if (app == NULL) {
        return 0;
    }

    module = suite_get_selected_module(app);
    if (module == NULL) {
        return 0;
    }

    screensave_saver_config_state_clamp(module, &app->working_config);
    if (!screensave_saver_config_state_save(module, &app->working_config, &app->diagnostics)) {
        suite_show_message(
            app->main_window,
            app->diagnostics.last_text[0] != '\0'
                ? app->diagnostics.last_text
                : "Suite could not save the selected saver configuration.",
            MB_OK | MB_ICONERROR
        );
        return 0;
    }

    (void)suite_copy_config_state(module, &app->stored_config, &app->working_config);
    (void)suite_copy_config_state(module, &app->resolved_config, &app->working_config);
    suite_update_binding(app);
    (void)suite_preview_restart(app, 0);
    if (app->info_window != NULL) {
        InvalidateRect(app->info_window, NULL, TRUE);
    }
    return 1;
}

INT_PTR suite_show_selected_settings_dialog(suite_app *app)
{
    const suite_catalog_entry *entry;
    const screensave_saver_module *module;
    screensave_saver_config_state dialog_config;
    INT_PTR result;

    if (app == NULL) {
        return -1;
    }

    entry = suite_get_selected_entry(app);
    module = suite_get_selected_module(app);
    if (entry == NULL || module == NULL) {
        return -1;
    }

    if (module->config_hooks == NULL || module->config_hooks->show_config_dialog == NULL) {
        suite_show_message(
            app->main_window,
            "This saver does not currently expose a product-owned Settings dialog.",
            MB_OK | MB_ICONINFORMATION
        );
        return IDCANCEL;
    }

    if (!screensave_saver_config_state_init(module, &dialog_config)) {
        suite_show_message(
            app->main_window,
            "Suite could not allocate a temporary saver configuration state for editing.",
            MB_OK | MB_ICONERROR
        );
        return -1;
    }

    screensave_saver_config_state_set_defaults(module, &dialog_config);
    (void)suite_copy_config_state(module, &dialog_config, &app->working_config);
    suite_preview_stop(app);
    result = module->config_hooks->show_config_dialog(
        app->instance,
        app->main_window,
        module,
        &dialog_config.common,
        dialog_config.product_config,
        dialog_config.product_config_size,
        &app->diagnostics
    );

    if (result == IDOK) {
        screensave_saver_config_state_clamp(module, &dialog_config);
        if (!screensave_saver_config_state_save(module, &dialog_config, &app->diagnostics)) {
            result = -1;
        } else {
            (void)suite_copy_config_state(module, &app->stored_config, &dialog_config);
            (void)suite_copy_config_state(module, &app->working_config, &dialog_config);
            (void)suite_copy_config_state(module, &app->resolved_config, &dialog_config);
            suite_update_binding(app);
        }
    }

    screensave_saver_config_state_dispose(&dialog_config);
    suite_sync_controls_from_working_config(app);
    (void)suite_preview_restart(app, 0);

    if (result == -1) {
        suite_show_message(
            app->main_window,
            app->diagnostics.last_text[0] != '\0'
                ? app->diagnostics.last_text
                : "Suite could not complete the saver-owned Settings dialog.",
            MB_OK | MB_ICONERROR
        );
    }

    if (app->info_window != NULL) {
        InvalidateRect(app->info_window, NULL, TRUE);
    }

    return result;
}

void suite_sync_controls_from_working_config(suite_app *app)
{
    const screensave_saver_module *module;
    const screensave_saver_config_hooks *config_hooks;
    unsigned int index;
    int enable_preset_combo;
    int enable_theme_combo;
    int enable_settings_button;

    if (app == NULL) {
        return;
    }

    module = suite_get_selected_module(app);
    if (module == NULL) {
        return;
    }

    app->syncing_controls = 1;
    enable_preset_combo = module->preset_count > 0U;
    enable_theme_combo = module->theme_count > 0U;
    config_hooks = module->config_hooks;
    enable_settings_button = config_hooks != NULL && config_hooks->show_config_dialog != NULL;

    if (app->preset_combo != NULL) {
        SendMessageA(app->preset_combo, CB_RESETCONTENT, 0U, 0L);
        for (index = 0U; index < module->preset_count; ++index) {
            LRESULT item_index;

            item_index = SendMessageA(
                app->preset_combo,
                CB_ADDSTRING,
                0U,
                (LPARAM)suite_preset_display_name(&module->presets[index])
            );
            if (item_index != CB_ERR) {
                SendMessageA(app->preset_combo, CB_SETITEMDATA, (WPARAM)item_index, (LPARAM)index);
            }
        }
        if (
            app->working_config.common.preset_key != NULL &&
            !suite_select_preset_combo_by_key(app->preset_combo, module, app->working_config.common.preset_key) &&
            module->preset_count > 0U
        ) {
            SendMessageA(app->preset_combo, CB_SETCURSEL, 0U, 0L);
        }
        EnableWindow(app->preset_combo, enable_preset_combo);
        if (app->preset_label != NULL) {
            EnableWindow(app->preset_label, enable_preset_combo);
        }
    }

    if (app->theme_combo != NULL) {
        SendMessageA(app->theme_combo, CB_RESETCONTENT, 0U, 0L);
        for (index = 0U; index < module->theme_count; ++index) {
            LRESULT item_index;

            item_index = SendMessageA(
                app->theme_combo,
                CB_ADDSTRING,
                0U,
                (LPARAM)suite_theme_display_name(&module->themes[index])
            );
            if (item_index != CB_ERR) {
                SendMessageA(app->theme_combo, CB_SETITEMDATA, (WPARAM)item_index, (LPARAM)index);
            }
        }
        if (
            app->working_config.common.theme_key != NULL &&
            !suite_select_theme_combo_by_key(app->theme_combo, module, app->working_config.common.theme_key) &&
            module->theme_count > 0U
        ) {
            SendMessageA(app->theme_combo, CB_SETCURSEL, 0U, 0L);
        }
        EnableWindow(app->theme_combo, enable_theme_combo);
        if (app->theme_label != NULL) {
            EnableWindow(app->theme_label, enable_theme_combo);
        }
    }

    if (app->renderer_combo != NULL) {
        suite_renderer_kind_to_combo(app->renderer_combo, app->app_config.renderer_request);
    }
    if (app->randomize_check != NULL) {
        SendMessageA(
            app->randomize_check,
            BM_SETCHECK,
            (WPARAM)(
                app->working_config.common.randomization_mode != SCREENSAVE_RANDOMIZATION_MODE_OFF
                    ? BST_CHECKED
                    : BST_UNCHECKED
            ),
            0L
        );
    }
    if (app->settings_button != NULL) {
        EnableWindow(app->settings_button, enable_settings_button);
    }

    app->syncing_controls = 0;
    if (app->info_window != NULL) {
        InvalidateRect(app->info_window, NULL, TRUE);
    }
}
