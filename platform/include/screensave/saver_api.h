#ifndef SCREENSAVE_SAVER_API_H
#define SCREENSAVE_SAVER_API_H

#include <windows.h>

#include "screensave/config_api.h"
#include "screensave/diagnostics_api.h"
#include "screensave/renderer_api.h"
#include "screensave/settings_api.h"

typedef enum screensave_session_mode_tag {
    SCREENSAVE_SESSION_MODE_SCREEN = 0,
    SCREENSAVE_SESSION_MODE_PREVIEW = 1,
    SCREENSAVE_SESSION_MODE_WINDOWED = 2,
    SCREENSAVE_SESSION_MODE_CONFIG = 3
} screensave_session_mode;

#define SCREENSAVE_SAVER_CAP_GDI 0x00000001UL
#define SCREENSAVE_SAVER_CAP_GL11 0x00000002UL
#define SCREENSAVE_SAVER_CAP_LONG_RUN_STABLE 0x00000004UL
#define SCREENSAVE_SAVER_CAP_GL21 0x00000008UL
#define SCREENSAVE_SAVER_CAP_PREVIEW_SAFE 0x00000010UL
#define SCREENSAVE_SAVER_CAP_GL33 0x00000020UL
#define SCREENSAVE_SAVER_CAP_GL46 0x00000040UL

typedef enum screensave_capability_quality_class_tag {
    SCREENSAVE_CAPABILITY_QUALITY_SAFE = 0,
    SCREENSAVE_CAPABILITY_QUALITY_BALANCED = 1,
    SCREENSAVE_CAPABILITY_QUALITY_HIGH = 2,
    SCREENSAVE_CAPABILITY_QUALITY_PREMIUM = 3
} screensave_capability_quality_class;

typedef struct screensave_saver_identity_tag {
    const char *product_key;
    const char *display_name;
    const char *description;
} screensave_saver_identity;

typedef struct screensave_saver_routing_policy_tag {
    screensave_renderer_kind minimum_kind;
    screensave_renderer_kind preferred_kind;
    screensave_capability_quality_class quality_class;
} screensave_saver_routing_policy;

typedef struct screensave_runtime_clock_tag {
    unsigned long session_start_millis;
    unsigned long elapsed_millis;
    unsigned long delta_millis;
    unsigned long frame_index;
} screensave_runtime_clock;

typedef struct screensave_session_seed_tag {
    unsigned long base_seed;
    unsigned long stream_seed;
    int deterministic;
} screensave_session_seed;

typedef struct screensave_saver_environment_tag {
    screensave_session_mode mode;
    screensave_sizei drawable_size;
    screensave_runtime_clock clock;
    screensave_session_seed seed;
    const screensave_config_binding *config_binding;
    screensave_renderer *renderer;
    screensave_diag_context *diagnostics;
} screensave_saver_environment;

typedef struct screensave_saver_session_tag screensave_saver_session;
struct screensave_saver_module_tag;

typedef struct screensave_saver_config_hooks_tag {
    unsigned int product_config_size;
    void (*set_defaults)(
        screensave_common_config *common_config,
        void *product_config,
        unsigned int product_config_size
    );
    void (*clamp)(
        screensave_common_config *common_config,
        void *product_config,
        unsigned int product_config_size
    );
    int (*load_config)(
        const struct screensave_saver_module_tag *module,
        screensave_common_config *common_config,
        void *product_config,
        unsigned int product_config_size,
        screensave_diag_context *diagnostics
    );
    int (*save_config)(
        const struct screensave_saver_module_tag *module,
        const screensave_common_config *common_config,
        const void *product_config,
        unsigned int product_config_size,
        screensave_diag_context *diagnostics
    );
    INT_PTR (*show_config_dialog)(
        HINSTANCE instance,
        HWND owner_window,
        const struct screensave_saver_module_tag *module,
        screensave_common_config *common_config,
        void *product_config,
        unsigned int product_config_size,
        screensave_diag_context *diagnostics
    );
    void (*apply_preset)(
        const struct screensave_saver_module_tag *module,
        const char *preset_key,
        screensave_common_config *common_config,
        void *product_config,
        unsigned int product_config_size
    );
    int (*export_settings_entries)(
        const struct screensave_saver_module_tag *module,
        const screensave_common_config *common_config,
        const void *product_config,
        unsigned int product_config_size,
        screensave_settings_file_kind kind,
        screensave_settings_writer *writer,
        screensave_diag_context *diagnostics
    );
    int (*import_settings_entry)(
        const struct screensave_saver_module_tag *module,
        screensave_common_config *common_config,
        void *product_config,
        unsigned int product_config_size,
        screensave_settings_file_kind kind,
        const char *section,
        const char *key,
        const char *value,
        screensave_diag_context *diagnostics
    );
    void (*randomize_settings)(
        const struct screensave_saver_module_tag *module,
        screensave_common_config *common_config,
        void *product_config,
        unsigned int product_config_size,
        const screensave_session_seed *seed,
        screensave_diag_context *diagnostics
    );
    unsigned long settings_schema_version;
    unsigned long settings_capability_flags;
} screensave_saver_config_hooks;

typedef struct screensave_saver_config_state_tag {
    screensave_common_config common;
    void *product_config;
    unsigned int product_config_size;
} screensave_saver_config_state;

typedef struct screensave_saver_callbacks_tag {
    int (*create_session)(
        const struct screensave_saver_module_tag *module,
        screensave_saver_session **session,
        const screensave_saver_environment *environment
    );
    void (*destroy_session)(screensave_saver_session *session);
    void (*resize_session)(screensave_saver_session *session, const screensave_saver_environment *environment);
    void (*step_session)(screensave_saver_session *session, const screensave_saver_environment *environment);
    void (*render_session)(screensave_saver_session *session, const screensave_saver_environment *environment);
} screensave_saver_callbacks;

typedef struct screensave_saver_module_tag {
    screensave_saver_identity identity;
    unsigned long capability_flags;
    screensave_saver_routing_policy routing_policy;
    const screensave_preset_descriptor *presets;
    unsigned int preset_count;
    const screensave_theme_descriptor *themes;
    unsigned int theme_count;
    const screensave_saver_config_hooks *config_hooks;
    const screensave_saver_callbacks *callbacks;
} screensave_saver_module;

int screensave_saver_module_is_valid(const screensave_saver_module *module);
int screensave_saver_supports_renderer_kind(
    const screensave_saver_module *module,
    screensave_renderer_kind kind
);
int screensave_saver_config_state_init(
    const screensave_saver_module *module,
    screensave_saver_config_state *config_state
);
int screensave_saver_config_state_copy(
    const screensave_saver_module *module,
    screensave_saver_config_state *target,
    const screensave_saver_config_state *source
);
void screensave_saver_config_state_dispose(screensave_saver_config_state *config_state);
void screensave_saver_config_state_set_defaults(
    const screensave_saver_module *module,
    screensave_saver_config_state *config_state
);
void screensave_saver_config_state_clamp(
    const screensave_saver_module *module,
    screensave_saver_config_state *config_state
);
int screensave_saver_config_state_load(
    const screensave_saver_module *module,
    screensave_saver_config_state *config_state,
    screensave_diag_context *diagnostics
);
int screensave_saver_config_state_save(
    const screensave_saver_module *module,
    const screensave_saver_config_state *config_state,
    screensave_diag_context *diagnostics
);
int screensave_saver_config_state_resolve_for_session(
    const screensave_saver_module *module,
    const screensave_saver_config_state *stored_state,
    const screensave_session_seed *seed,
    screensave_saver_config_state *resolved_state,
    screensave_diag_context *diagnostics
);
const char *screensave_session_mode_name(screensave_session_mode mode);
const char *screensave_capability_quality_name(screensave_capability_quality_class quality_class);
int screensave_scr_main(
    HINSTANCE instance,
    HINSTANCE previous_instance,
    LPSTR command_line,
    int show_code,
    const screensave_saver_module *module
);

#endif /* SCREENSAVE_SAVER_API_H */
