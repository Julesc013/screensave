#ifndef RICOCHET_CORE_H
#define RICOCHET_CORE_H

#include "screensave/v2.h"

#define RICOCHET_DEFAULT_PRESET_KEY "classic_clean"
#define RICOCHET_DEFAULT_THEME_KEY "classic_clean"
#define RICOCHET_PRESET_COUNT 7U
#define RICOCHET_THEME_COUNT 5U
#define RICOCHET_MAX_OBJECTS 5U
#define RICOCHET_MAX_TRAIL_POINTS 14U

#define RICOCHET_DETAIL_LOW ((ss_u32)0U)
#define RICOCHET_DETAIL_STANDARD ((ss_u32)1U)
#define RICOCHET_DETAIL_HIGH ((ss_u32)2U)

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

typedef struct ricochet_core_theme_tag {
    ss_v2_color_rgba8 primary_color;
    ss_v2_color_rgba8 accent_color;
} ricochet_core_theme;

typedef struct ricochet_core_desc_tag {
    const ricochet_config *config;
    const ricochet_core_theme *theme;
    ss_u32 width;
    ss_u32 height;
    ss_u32 detail_level;
    ss_u32 preview_mode;
    ss_u32 base_seed;
    ss_u32 stream_seed;
} ricochet_core_desc;

typedef struct ricochet_core_session_tag ricochet_core_session;

int ricochet_core_create(const ricochet_core_desc *desc, ricochet_core_session **session_out);
void ricochet_core_destroy(ricochet_core_session *session);
void ricochet_core_resize(ricochet_core_session *session, ss_u32 width, ss_u32 height, ss_u32 preview_mode);
void ricochet_core_advance(ricochet_core_session *session, ss_u32 delta_ms);
int ricochet_core_render(ricochet_core_session *session, ss_v2_draw_target *target);

#endif /* RICOCHET_CORE_H */
