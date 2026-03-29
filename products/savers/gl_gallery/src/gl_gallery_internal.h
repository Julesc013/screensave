#ifndef GL_GALLERY_INTERNAL_H
#define GL_GALLERY_INTERNAL_H

#include <windows.h>

#include "screensave/config_api.h"
#include "screensave/diagnostics_api.h"
#include "screensave/saver_api.h"

#define GL_GALLERY_PRODUCT_REGISTRY_ROOTA "Software\\Julesc013\\ScreenSave\\Products\\gl_gallery"
#define GL_GALLERY_DEFAULT_PRESET_KEY "compatibility_gallery"
#define GL_GALLERY_DEFAULT_THEME_KEY "compatibility_gallery"
#define GL_GALLERY_PRESET_COUNT 6U
#define GL_GALLERY_THEME_COUNT 6U

typedef enum gl_gallery_scene_mode_tag {
    GL_GALLERY_SCENE_COMPATIBILITY = 0,
    GL_GALLERY_SCENE_NEON = 1,
    GL_GALLERY_SCENE_SHOWCASE = 2
} gl_gallery_scene_mode;

typedef enum gl_gallery_motion_mode_tag {
    GL_GALLERY_MOTION_CALM = 0,
    GL_GALLERY_MOTION_STANDARD = 1,
    GL_GALLERY_MOTION_BRISK = 2
} gl_gallery_motion_mode;

typedef struct gl_gallery_config_tag {
    int scene_mode;
    int motion_mode;
} gl_gallery_config;

typedef struct gl_gallery_preset_values_tag {
    int scene_mode;
    int motion_mode;
} gl_gallery_preset_values;

typedef enum gl_gallery_renderer_tier_tag {
    GL_GALLERY_TIER_GDI = 0,
    GL_GALLERY_TIER_GL11 = 1,
    GL_GALLERY_TIER_GL_PLUS = 2
} gl_gallery_renderer_tier;

typedef struct gl_gallery_marker_tag {
    int x;
    int y;
    int radius;
    int phase;
} gl_gallery_marker;

extern const screensave_preset_descriptor g_gl_gallery_presets[];
extern const screensave_theme_descriptor g_gl_gallery_themes[];

struct screensave_saver_session_tag {
    gl_gallery_config config;
    const screensave_theme_descriptor *theme;
    screensave_sizei drawable_size;
    screensave_detail_level detail_level;
    int preview_mode;
    gl_gallery_renderer_tier tier;
    unsigned long phase_counter;
    unsigned long tier_pulse;
    unsigned int scene_phase;
    unsigned int drift_phase;
    unsigned int marker_count;
    gl_gallery_marker markers[12];
};

const screensave_saver_module *gl_gallery_get_module(void);
const screensave_preset_descriptor *gl_gallery_get_presets(unsigned int *count_out);
const screensave_theme_descriptor *gl_gallery_get_themes(unsigned int *count_out);
const gl_gallery_preset_values *gl_gallery_find_preset_values(const char *preset_key);
const screensave_theme_descriptor *gl_gallery_find_theme_descriptor(const char *theme_key);

void gl_gallery_config_set_defaults(
    screensave_common_config *common_config,
    void *product_config,
    unsigned int product_config_size
);
void gl_gallery_config_clamp(
    screensave_common_config *common_config,
    void *product_config,
    unsigned int product_config_size
);
int gl_gallery_config_load(
    const screensave_saver_module *module,
    screensave_common_config *common_config,
    void *product_config,
    unsigned int product_config_size,
    screensave_diag_context *diagnostics
);
int gl_gallery_config_save(
    const screensave_saver_module *module,
    const screensave_common_config *common_config,
    const void *product_config,
    unsigned int product_config_size,
    screensave_diag_context *diagnostics
);
INT_PTR gl_gallery_config_show_dialog(
    HINSTANCE instance,
    HWND owner_window,
    const screensave_saver_module *module,
    screensave_common_config *common_config,
    void *product_config,
    unsigned int product_config_size,
    screensave_diag_context *diagnostics
);

int gl_gallery_create_session(
    const screensave_saver_module *module,
    screensave_saver_session **session,
    const screensave_saver_environment *environment
);
void gl_gallery_destroy_session(screensave_saver_session *session);
void gl_gallery_resize_session(
    screensave_saver_session *session,
    const screensave_saver_environment *environment
);
void gl_gallery_step_session(
    screensave_saver_session *session,
    const screensave_saver_environment *environment
);
void gl_gallery_render_session(
    screensave_saver_session *session,
    const screensave_saver_environment *environment
);

const char *gl_gallery_scene_mode_name(int scene_mode);
const char *gl_gallery_motion_mode_name(int motion_mode);
void gl_gallery_apply_preset_to_config(
    const char *preset_key,
    screensave_common_config *common_config,
    gl_gallery_config *product_config
);

#endif /* GL_GALLERY_INTERNAL_H */
