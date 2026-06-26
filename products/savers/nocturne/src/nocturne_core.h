#ifndef NOCTURNE_CORE_H
#define NOCTURNE_CORE_H

#include "screensave/v2.h"

#define NOCTURNE_DEFAULT_PRESET_KEY "observatory_night"
#define NOCTURNE_DEFAULT_THEME_KEY "gray_black"
#define NOCTURNE_PRESET_COUNT 9U
#define NOCTURNE_THEME_COUNT 7U

#define NOCTURNE_DETAIL_LOW ((ss_u32)0U)
#define NOCTURNE_DETAIL_STANDARD ((ss_u32)1U)
#define NOCTURNE_DETAIL_HIGH ((ss_u32)2U)

typedef enum nocturne_motion_mode_tag {
    NOCTURNE_MOTION_NONE = 0,
    NOCTURNE_MOTION_DRIFT_MARK = 1,
    NOCTURNE_MOTION_QUIET_LINE = 2,
    NOCTURNE_MOTION_MONOLITH = 3,
    NOCTURNE_MOTION_BREATH = 4
} nocturne_motion_mode;

typedef enum nocturne_fade_speed_tag {
    NOCTURNE_FADE_SLOW = 0,
    NOCTURNE_FADE_STANDARD = 1,
    NOCTURNE_FADE_GENTLE = 2
} nocturne_fade_speed;

typedef enum nocturne_motion_strength_tag {
    NOCTURNE_STRENGTH_STILL = 0,
    NOCTURNE_STRENGTH_SUBTLE = 1,
    NOCTURNE_STRENGTH_SOFT = 2
} nocturne_motion_strength;

typedef struct nocturne_config_tag {
    int motion_mode;
    int fade_speed;
    int motion_strength;
} nocturne_config;

typedef struct nocturne_preset_values_tag {
    int motion_mode;
    int fade_speed;
    int motion_strength;
} nocturne_preset_values;

typedef struct nocturne_core_theme_tag {
    ss_v2_color_rgba8 primary_color;
    ss_v2_color_rgba8 accent_color;
} nocturne_core_theme;

typedef struct nocturne_core_desc_tag {
    const nocturne_config *config;
    const nocturne_core_theme *theme;
    ss_u32 width;
    ss_u32 height;
    ss_u32 detail_level;
    ss_u32 preview_mode;
    ss_u32 base_seed;
    ss_u32 stream_seed;
} nocturne_core_desc;

typedef struct nocturne_core_session_tag nocturne_core_session;

int nocturne_core_create(const nocturne_core_desc *desc, nocturne_core_session **session_out);
void nocturne_core_destroy(nocturne_core_session *session);
void nocturne_core_resize(nocturne_core_session *session, ss_u32 width, ss_u32 height, ss_u32 preview_mode);
void nocturne_core_advance(nocturne_core_session *session, ss_u32 delta_ms);
int nocturne_core_render(nocturne_core_session *session, ss_v2_draw_target *target);

#endif /* NOCTURNE_CORE_H */
