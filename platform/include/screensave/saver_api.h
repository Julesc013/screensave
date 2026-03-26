#ifndef SCREENSAVE_SAVER_API_H
#define SCREENSAVE_SAVER_API_H

#include <windows.h>

#include "screensave/config_api.h"
#include "screensave/diagnostics_api.h"
#include "screensave/renderer_api.h"

typedef enum screensave_session_mode_tag {
    SCREENSAVE_SESSION_MODE_SCREEN = 0,
    SCREENSAVE_SESSION_MODE_PREVIEW = 1,
    SCREENSAVE_SESSION_MODE_WINDOWED = 2,
    SCREENSAVE_SESSION_MODE_CONFIG = 3
} screensave_session_mode;

#define SCREENSAVE_SAVER_CAP_UNIVERSAL_GDI 0x00000001UL
#define SCREENSAVE_SAVER_CAP_OPTIONAL_GL11 0x00000002UL
#define SCREENSAVE_SAVER_CAP_LONG_RUN_STABLE 0x00000004UL

typedef struct screensave_saver_identity_tag {
    const char *product_key;
    const char *display_name;
    const char *description;
} screensave_saver_identity;

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
} screensave_saver_config_hooks;

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
    const screensave_preset_descriptor *presets;
    unsigned int preset_count;
    const screensave_theme_descriptor *themes;
    unsigned int theme_count;
    const screensave_saver_config_hooks *config_hooks;
    const screensave_saver_callbacks *callbacks;
} screensave_saver_module;

int screensave_saver_module_is_valid(const screensave_saver_module *module);
const char *screensave_session_mode_name(screensave_session_mode mode);
int screensave_scr_main(
    HINSTANCE instance,
    HINSTANCE previous_instance,
    LPSTR command_line,
    int show_code,
    const screensave_saver_module *module
);

#endif /* SCREENSAVE_SAVER_API_H */
