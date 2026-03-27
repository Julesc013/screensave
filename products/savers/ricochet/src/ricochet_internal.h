#ifndef RICOCHET_INTERNAL_H
#define RICOCHET_INTERNAL_H

#include <windows.h>

#include "screensave/config_api.h"
#include "screensave/diagnostics_api.h"
#include "screensave/saver_api.h"

#define RICOCHET_PRODUCT_REGISTRY_ROOTA "Software\\Julesc013\\ScreenSave\\Products\\ricochet"
#define RICOCHET_DEFAULT_PRESET_KEY "classic_clean"
#define RICOCHET_DEFAULT_THEME_KEY "classic_clean"
#define RICOCHET_PRESET_COUNT 5U
#define RICOCHET_THEME_COUNT 4U
#define RICOCHET_MAX_OBJECTS 5U
#define RICOCHET_MAX_TRAIL_POINTS 10U

typedef enum ricochet_object_mode_tag {
    RICOCHET_OBJECT_BLOCK = 0,
    RICOCHET_OBJECT_DISC = 1,
    RICOCHET_OBJECT_EMBLEM = 2
} ricochet_object_mode;

typedef enum ricochet_speed_mode_tag {
    RICOCHET_SPEED_CALM = 0,
    RICOCHET_SPEED_STANDARD = 1,
    RICOCHET_SPEED_LIVELY = 2
} ricochet_speed_mode;

typedef enum ricochet_trail_mode_tag {
    RICOCHET_TRAIL_NONE = 0,
    RICOCHET_TRAIL_SHORT = 1,
    RICOCHET_TRAIL_PHOSPHOR = 2
} ricochet_trail_mode;

typedef struct ricochet_config_tag {
    int object_mode;
    int object_count;
    int speed_mode;
    int trail_mode;
} ricochet_config;

typedef struct ricochet_preset_values_tag {
    int object_mode;
    int object_count;
    int speed_mode;
    int trail_mode;
} ricochet_preset_values;

typedef struct ricochet_rng_state_tag {
    unsigned long state;
} ricochet_rng_state;

typedef struct ricochet_body_tag {
    long x;
    long y;
    long vx;
    long vy;
    int size;
    unsigned int trail_head;
    unsigned int trail_count;
    long trail_x[RICOCHET_MAX_TRAIL_POINTS];
    long trail_y[RICOCHET_MAX_TRAIL_POINTS];
    int trail_size[RICOCHET_MAX_TRAIL_POINTS];
} ricochet_body;

extern const screensave_preset_descriptor g_ricochet_presets[];
extern const screensave_theme_descriptor g_ricochet_themes[];

struct screensave_saver_session_tag {
    ricochet_config config;
    const screensave_theme_descriptor *theme;
    screensave_sizei drawable_size;
    screensave_detail_level detail_level;
    int preview_mode;
    ricochet_rng_state rng;
    unsigned int object_count;
    ricochet_body bodies[RICOCHET_MAX_OBJECTS];
    int celebration_timer;
};

const screensave_saver_module *ricochet_get_module(void);

const screensave_preset_descriptor *ricochet_get_presets(unsigned int *count_out);
const screensave_theme_descriptor *ricochet_get_themes(unsigned int *count_out);
const ricochet_preset_values *ricochet_find_preset_values(const char *preset_key);
const screensave_theme_descriptor *ricochet_find_theme_descriptor(const char *theme_key);

void ricochet_config_set_defaults(
    screensave_common_config *common_config,
    void *product_config,
    unsigned int product_config_size
);
void ricochet_config_clamp(
    screensave_common_config *common_config,
    void *product_config,
    unsigned int product_config_size
);
int ricochet_config_load(
    const screensave_saver_module *module,
    screensave_common_config *common_config,
    void *product_config,
    unsigned int product_config_size,
    screensave_diag_context *diagnostics
);
int ricochet_config_save(
    const screensave_saver_module *module,
    const screensave_common_config *common_config,
    const void *product_config,
    unsigned int product_config_size,
    screensave_diag_context *diagnostics
);
INT_PTR ricochet_config_show_dialog(
    HINSTANCE instance,
    HWND owner_window,
    const screensave_saver_module *module,
    screensave_common_config *common_config,
    void *product_config,
    unsigned int product_config_size,
    screensave_diag_context *diagnostics
);

int ricochet_create_session(
    const screensave_saver_module *module,
    screensave_saver_session **session,
    const screensave_saver_environment *environment
);
void ricochet_destroy_session(screensave_saver_session *session);
void ricochet_resize_session(
    screensave_saver_session *session,
    const screensave_saver_environment *environment
);
void ricochet_step_session(
    screensave_saver_session *session,
    const screensave_saver_environment *environment
);
void ricochet_render_session(
    screensave_saver_session *session,
    const screensave_saver_environment *environment
);

void ricochet_rng_seed(ricochet_rng_state *state, unsigned long seed);
unsigned long ricochet_rng_next(ricochet_rng_state *state);
unsigned long ricochet_rng_range(ricochet_rng_state *state, unsigned long upper_bound);

const char *ricochet_object_mode_name(int object_mode);
const char *ricochet_speed_mode_name(int speed_mode);
const char *ricochet_trail_mode_name(int trail_mode);

void ricochet_apply_preset_to_config(
    const char *preset_key,
    screensave_common_config *common_config,
    ricochet_config *product_config
);

#endif /* RICOCHET_INTERNAL_H */
