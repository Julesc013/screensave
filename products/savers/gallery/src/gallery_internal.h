#ifndef GALLERY_INTERNAL_H
#define GALLERY_INTERNAL_H

#include <windows.h>

#include "screensave/config_api.h"
#include "screensave/diagnostics_api.h"
#include "screensave/saver_api.h"

#define GALLERY_PRODUCT_REGISTRY_ROOTA "Software\\Julesc013\\ScreenSave\\Products\\gallery"
#define GALLERY_DEFAULT_PRESET_KEY "compatibility_gallery"
#define GALLERY_DEFAULT_THEME_KEY "compatibility_gallery"
#define GALLERY_PRESET_COUNT 6U
#define GALLERY_THEME_COUNT 6U

typedef enum gallery_scene_mode_tag {
    GALLERY_SCENE_COMPATIBILITY = 0,
    GALLERY_SCENE_NEON = 1,
    GALLERY_SCENE_SHOWCASE = 2
} gallery_scene_mode;

typedef enum gallery_motion_mode_tag {
    GALLERY_MOTION_CALM = 0,
    GALLERY_MOTION_STANDARD = 1,
    GALLERY_MOTION_BRISK = 2
} gallery_motion_mode;

typedef struct gallery_config_tag {
    int scene_mode;
    int motion_mode;
} gallery_config;

typedef struct gallery_preset_values_tag {
    int scene_mode;
    int motion_mode;
} gallery_preset_values;

typedef enum gallery_renderer_tier_tag {
    GALLERY_TIER_GDI = 0,
    GALLERY_TIER_GL11 = 1,
    GALLERY_TIER_GL21 = 2
} gallery_renderer_tier;

typedef struct gallery_marker_tag {
    int x;
    int y;
    int radius;
    int phase;
} gallery_marker;

extern const screensave_preset_descriptor g_gallery_presets[];
extern const screensave_theme_descriptor g_gallery_themes[];

struct screensave_saver_session_tag {
    gallery_config config;
    const screensave_theme_descriptor *theme;
    screensave_sizei drawable_size;
    screensave_detail_level detail_level;
    int preview_mode;
    gallery_renderer_tier tier;
    unsigned long phase_counter;
    unsigned long tier_pulse;
    unsigned int scene_phase;
    unsigned int drift_phase;
    unsigned int marker_count;
    gallery_marker markers[12];
};

const screensave_saver_module *gallery_get_module(void);
const screensave_preset_descriptor *gallery_get_presets(unsigned int *count_out);
const screensave_theme_descriptor *gallery_get_themes(unsigned int *count_out);
const gallery_preset_values *gallery_find_preset_values(const char *preset_key);
const screensave_theme_descriptor *gallery_find_theme_descriptor(const char *theme_key);

void gallery_config_set_defaults(
    screensave_common_config *common_config,
    void *product_config,
    unsigned int product_config_size
);
void gallery_config_clamp(
    screensave_common_config *common_config,
    void *product_config,
    unsigned int product_config_size
);
int gallery_config_load(
    const screensave_saver_module *module,
    screensave_common_config *common_config,
    void *product_config,
    unsigned int product_config_size,
    screensave_diag_context *diagnostics
);
int gallery_config_save(
    const screensave_saver_module *module,
    const screensave_common_config *common_config,
    const void *product_config,
    unsigned int product_config_size,
    screensave_diag_context *diagnostics
);
INT_PTR gallery_config_show_dialog(
    HINSTANCE instance,
    HWND owner_window,
    const screensave_saver_module *module,
    screensave_common_config *common_config,
    void *product_config,
    unsigned int product_config_size,
    screensave_diag_context *diagnostics
);

int gallery_create_session(
    const screensave_saver_module *module,
    screensave_saver_session **session,
    const screensave_saver_environment *environment
);
void gallery_destroy_session(screensave_saver_session *session);
void gallery_resize_session(
    screensave_saver_session *session,
    const screensave_saver_environment *environment
);
void gallery_step_session(
    screensave_saver_session *session,
    const screensave_saver_environment *environment
);
void gallery_render_session(
    screensave_saver_session *session,
    const screensave_saver_environment *environment
);

const char *gallery_scene_mode_name(int scene_mode);
const char *gallery_motion_mode_name(int motion_mode);
void gallery_apply_preset_to_config(
    const char *preset_key,
    screensave_common_config *common_config,
    gallery_config *product_config
);

#endif /* GALLERY_INTERNAL_H */
