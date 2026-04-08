#include <math.h>

#include "gallery_internal.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

static gallery_renderer_tier gallery_tier_from_info(const screensave_renderer_info *info)
{
    if (info == NULL) {
        return GALLERY_TIER_GDI;
    }

    switch (info->active_kind) {
    case SCREENSAVE_RENDERER_KIND_GL46:
    case SCREENSAVE_RENDERER_KIND_GL33:
        return GALLERY_TIER_GL33;
    case SCREENSAVE_RENDERER_KIND_GL21:
        return GALLERY_TIER_GL21;
    case SCREENSAVE_RENDERER_KIND_GL11:
        return GALLERY_TIER_GL11;
    case SCREENSAVE_RENDERER_KIND_NULL:
    case SCREENSAVE_RENDERER_KIND_GDI:
    case SCREENSAVE_RENDERER_KIND_UNKNOWN:
    default:
        return GALLERY_TIER_GDI;
    }
}

static screensave_color gallery_mix(
    screensave_color first,
    screensave_color second,
    unsigned int amount
)
{
    unsigned int other_amount;

    if (amount > 255U) {
        amount = 255U;
    }

    other_amount = 255U - amount;
    first.red = (unsigned char)(((unsigned int)first.red * other_amount + (unsigned int)second.red * amount) / 255U);
    first.green = (unsigned char)(((unsigned int)first.green * other_amount + (unsigned int)second.green * amount) / 255U);
    first.blue = (unsigned char)(((unsigned int)first.blue * other_amount + (unsigned int)second.blue * amount) / 255U);
    return first;
}

static screensave_color gallery_background_color(const screensave_saver_session *session, gallery_renderer_tier tier)
{
    screensave_color color;

    color.red = 8;
    color.green = 10;
    color.blue = 16;
    color.alpha = 255;

    if (session == NULL || session->theme == NULL) {
        return color;
    }

    if (session->config.scene_mode == GALLERY_SCENE_NEON) {
        color = session->theme->primary_color;
        color.red = (unsigned char)(color.red / 3U);
        color.green = (unsigned char)(color.green / 3U);
        color.blue = (unsigned char)(color.blue / 3U);
    } else if (session->config.scene_mode == GALLERY_SCENE_SHOWCASE) {
        color = session->theme->accent_color;
        color.red = (unsigned char)(color.red / 4U);
        color.green = (unsigned char)(color.green / 4U);
        color.blue = (unsigned char)(color.blue / 4U);
    } else if (tier == GALLERY_TIER_GL33) {
        color = session->theme->accent_color;
        color.red = (unsigned char)(color.red / 5U);
        color.green = (unsigned char)(color.green / 5U);
        color.blue = (unsigned char)(color.blue / 5U);
    } else if (tier == GALLERY_TIER_GL21) {
        color = session->theme->primary_color;
        color.red = (unsigned char)(color.red / 4U);
        color.green = (unsigned char)(color.green / 4U);
        color.blue = (unsigned char)(color.blue / 4U);
    }

    return color;
}

static void gallery_draw_fill(
    screensave_renderer *renderer,
    int x,
    int y,
    int width,
    int height,
    screensave_color color
)
{
    screensave_recti rect;

    if (renderer == NULL || width <= 0 || height <= 0) {
        return;
    }

    rect.x = x;
    rect.y = y;
    rect.width = width;
    rect.height = height;
    screensave_renderer_fill_rect(renderer, &rect, color);
}

static void gallery_draw_frame(
    screensave_renderer *renderer,
    int left,
    int top,
    int width,
    int height,
    screensave_color color
)
{
    screensave_recti rect;

    if (renderer == NULL || width <= 0 || height <= 0) {
        return;
    }

    rect.x = left;
    rect.y = top;
    rect.width = width;
    rect.height = height;
    screensave_renderer_draw_frame_rect(renderer, &rect, color);
}

static void gallery_point_on_circle(
    int center_x,
    int center_y,
    int radius_x,
    int radius_y,
    double angle,
    screensave_pointi *point
)
{
    if (point == NULL) {
        return;
    }

    point->x = center_x + (int)(cos(angle) * (double)radius_x);
    point->y = center_y + (int)(sin(angle) * (double)radius_y);
}

static void gallery_draw_ring(
    screensave_renderer *renderer,
    int center_x,
    int center_y,
    int radius_x,
    int radius_y,
    unsigned int points,
    double offset,
    screensave_color color
)
{
    screensave_pointi ring_points[16];
    unsigned int index;
    double angle_step;

    if (renderer == NULL || points < 3U) {
        return;
    }
    if (points > 16U) {
        points = 16U;
    }

    angle_step = (2.0 * M_PI) / (double)points;
    for (index = 0U; index < points; ++index) {
        gallery_point_on_circle(
            center_x,
            center_y,
            radius_x,
            radius_y,
            offset + (angle_step * (double)index),
            &ring_points[index]
        );
    }

    screensave_renderer_draw_polyline(renderer, ring_points, points, color);
    screensave_renderer_draw_line(renderer, &ring_points[points - 1U], &ring_points[0U], color);
}

static int gallery_marker_visual_count(const screensave_saver_session *session, gallery_renderer_tier tier)
{
    int count;

    if (session == NULL) {
        return 4;
    }

    switch (session->detail_level) {
    case SCREENSAVE_DETAIL_LEVEL_LOW:
        count = 4;
        break;
    case SCREENSAVE_DETAIL_LEVEL_HIGH:
        count = 8;
        break;
    case SCREENSAVE_DETAIL_LEVEL_STANDARD:
    default:
        count = 6;
        break;
    }

    if (tier == GALLERY_TIER_GL11) {
        count += 1;
    } else if (tier == GALLERY_TIER_GL33) {
        count += 4;
    } else if (tier == GALLERY_TIER_GL21) {
        count += 3;
    }
    if (session->preview_mode && count > 5) {
        count = 5;
    }
    if (count > (int)session->marker_count) {
        count = (int)session->marker_count;
    }
    if (count < 1) {
        count = 1;
    }

    return count;
}

static void gallery_draw_marker_field(
    screensave_saver_session *session,
    screensave_renderer *renderer,
    gallery_renderer_tier tier,
    screensave_color accent,
    screensave_color highlight
)
{
    int count;
    int index;

    if (session == NULL || renderer == NULL) {
        return;
    }

    count = gallery_marker_visual_count(session, tier);
    for (index = 0; index < count; ++index) {
        const gallery_marker *marker;
        int offset_x;
        int offset_y;
        int radius;
        screensave_pointi start_point;
        screensave_pointi end_point;

        marker = &session->markers[index];
        offset_x = (int)(cos((double)(session->phase_counter + (unsigned long)marker->phase) / 12.0) * (double)(marker->radius + 6));
        offset_y = (int)(sin((double)(session->phase_counter + (unsigned long)marker->phase) / 13.0) * (double)(marker->radius + 6));
        radius = marker->radius;
        gallery_draw_frame(
            renderer,
            marker->x - radius,
            marker->y - radius,
            radius * 2,
            radius * 2,
            accent
        );
        gallery_draw_ring(
            renderer,
            marker->x + offset_x,
            marker->y + offset_y,
            radius + 4,
            radius + 2,
            tier == GALLERY_TIER_GL33 ? 16U : (tier == GALLERY_TIER_GL21 ? 12U : 8U),
            (double)(session->phase_counter % 360UL) * (M_PI / 180.0),
            highlight
        );
        start_point.x = marker->x - radius;
        start_point.y = marker->y;
        end_point.x = marker->x + radius;
        end_point.y = marker->y;
        screensave_renderer_draw_line(renderer, &start_point, &end_point, accent);
        if (tier != GALLERY_TIER_GDI) {
            start_point.x = marker->x;
            start_point.y = marker->y - radius;
            end_point.x = marker->x;
            end_point.y = marker->y + radius;
            screensave_renderer_draw_line(renderer, &start_point, &end_point, highlight);
        }
    }
}

static void gallery_draw_compatibility_scene(
    screensave_saver_session *session,
    screensave_renderer *renderer,
    gallery_renderer_tier tier
)
{
    screensave_color back;
    screensave_color frame;
    screensave_color accent;
    screensave_color glow;
    int panel_width;
    int panel_height;
    int panel_top;
    int panel_count;
    int index;

    back = gallery_background_color(session, tier);
    frame = gallery_mix(back, session->theme->primary_color, 88U);
    accent = gallery_mix(back, session->theme->accent_color, 174U);
    glow = gallery_mix(
        frame,
        session->theme->accent_color,
        tier == GALLERY_TIER_GL33 ? 208U : (tier == GALLERY_TIER_GL21 ? 184U : 128U)
    );

    screensave_renderer_clear(renderer, back);
    panel_count = 3;
    if (tier == GALLERY_TIER_GL11) {
        panel_count = 4;
    } else if (tier == GALLERY_TIER_GL33) {
        panel_count = 6;
    } else if (tier == GALLERY_TIER_GL21) {
        panel_count = 5;
    }
    if (session->preview_mode && panel_count > 3) {
        panel_count = 3;
    }

    panel_width = session->drawable_size.width / (panel_count + 1);
    if (panel_width < 52) {
        panel_width = 52;
    }
    panel_height = session->drawable_size.height / 3;
    if (panel_height < 48) {
        panel_height = 48;
    }
    panel_top = session->drawable_size.height / 5;
    for (index = 0; index < panel_count; ++index) {
        int left;

        left = ((index + 1) * session->drawable_size.width) / (panel_count + 1) - (panel_width / 2);
        gallery_draw_frame(renderer, left, panel_top, panel_width, panel_height, frame);
        gallery_draw_fill(renderer, left + 4, panel_top + 4, panel_width - 8, panel_height - 8, glow);
        gallery_draw_ring(
            renderer,
            left + panel_width / 2,
            panel_top + panel_height / 2,
            (panel_width / 3),
            (panel_height / 3),
            tier == GALLERY_TIER_GL33 ? 16U : (tier == GALLERY_TIER_GL21 ? 12U : 8U),
            (double)((session->phase_counter + (unsigned long)index) % 360UL) * (M_PI / 180.0),
            accent
        );
    }

    gallery_draw_marker_field(session, renderer, tier, frame, glow);
}

static void gallery_draw_neon_scene(
    screensave_saver_session *session,
    screensave_renderer *renderer,
    gallery_renderer_tier tier
)
{
    screensave_color back;
    screensave_color neon_a;
    screensave_color neon_b;
    screensave_color neon_c;
    screensave_pointi start_point;
    screensave_pointi end_point;
    int sweep_count;
    int index;

    back = gallery_background_color(session, tier);
    neon_a = gallery_mix(back, session->theme->primary_color, 144U);
    neon_b = gallery_mix(back, session->theme->accent_color, 192U);
    neon_c = gallery_mix(
        neon_a,
        session->theme->accent_color,
        tier == GALLERY_TIER_GL33 ? 196U : (tier == GALLERY_TIER_GL21 ? 160U : 112U)
    );

    screensave_renderer_clear(renderer, back);

    sweep_count = 6;
    if (tier == GALLERY_TIER_GL11) {
        sweep_count = 8;
    } else if (tier == GALLERY_TIER_GL33) {
        sweep_count = 12;
    } else if (tier == GALLERY_TIER_GL21) {
        sweep_count = 10;
    }
    if (session->preview_mode && sweep_count > 6) {
        sweep_count = 6;
    }

    for (index = 0; index < sweep_count; ++index) {
        int y;
        int x_offset;
        int span;

        y = (int)(((unsigned long)((index + 1) * session->drawable_size.height)) / (unsigned long)(sweep_count + 1));
        x_offset = (int)(((session->phase_counter * (unsigned long)(index + 1)) + (session->drift_phase * 13U)) % 64UL) - 32;
        span = session->drawable_size.width - (index * 16);
        if (span < 60) {
            span = 60;
        }
        start_point.x = x_offset;
        start_point.y = y;
        end_point.x = x_offset + span;
        end_point.y = y + ((index % 2) == 0 ? -8 : 8);
        screensave_renderer_draw_line(renderer, &start_point, &end_point, neon_a);
        gallery_draw_frame(renderer, x_offset + 8, y - 8, 60 + (index * 8), 20, neon_b);
        if (tier != GALLERY_TIER_GDI || index % 2 == 0) {
            gallery_draw_ring(
                renderer,
                x_offset + 120 + (index * 12),
                y,
                14 + (index % 3) * 4,
                8 + (index % 2) * 3,
                tier == GALLERY_TIER_GL33 ? 16U : (tier == GALLERY_TIER_GL21 ? 12U : 8U),
                (double)((session->phase_counter + (unsigned long)index) % 360UL) * (M_PI / 90.0),
                neon_c
            );
        }
    }

    gallery_draw_marker_field(session, renderer, tier, neon_a, neon_c);
}

static void gallery_draw_showcase_scene(
    screensave_saver_session *session,
    screensave_renderer *renderer,
    gallery_renderer_tier tier
)
{
    screensave_color back;
    screensave_color frame;
    screensave_color highlight;
    screensave_color beam;
    screensave_pointi start_point;
    screensave_pointi end_point;
    int layer_count;
    int index;

    back = gallery_background_color(session, tier);
    frame = gallery_mix(back, session->theme->primary_color, 120U);
    highlight = gallery_mix(back, session->theme->accent_color, 178U);
    beam = gallery_mix(
        frame,
        session->theme->accent_color,
        tier == GALLERY_TIER_GL33 ? 228U : (tier == GALLERY_TIER_GL21 ? 200U : 148U)
    );

    screensave_renderer_clear(renderer, back);

    layer_count = 4;
    if (tier == GALLERY_TIER_GL11) {
        layer_count = 5;
    } else if (tier == GALLERY_TIER_GL33) {
        layer_count = 9;
    } else if (tier == GALLERY_TIER_GL21) {
        layer_count = 7;
    }
    if (session->preview_mode && layer_count > 4) {
        layer_count = 4;
    }

    for (index = 0; index < layer_count; ++index) {
        int inset;
        int top;
        int width;
        int height;

        inset = 16 + (index * 12);
        top = 28 + (index * 8);
        width = session->drawable_size.width - (inset * 2);
        height = session->drawable_size.height / 2;
        if (width < 64) {
            width = 64;
        }
        gallery_draw_frame(renderer, inset, top, width, height, frame);
        gallery_draw_fill(renderer, inset + 3, top + 3, width - 6, height - 6, highlight);
        gallery_draw_ring(
            renderer,
            session->drawable_size.width / 2,
            session->drawable_size.height / 2,
            width / 4,
            height / 4,
            tier == GALLERY_TIER_GL33 ? 16U : (tier == GALLERY_TIER_GL21 ? 12U : 8U),
            (double)((session->phase_counter + (unsigned long)(index * 31)) % 360UL) * (M_PI / 180.0),
            beam
        );
    }

    start_point.x = 0;
    start_point.y = session->drawable_size.height / 2;
    end_point.x = session->drawable_size.width - 1;
    end_point.y = session->drawable_size.height / 2;
    screensave_renderer_draw_line(renderer, &start_point, &end_point, frame);
    if (tier != GALLERY_TIER_GDI) {
        start_point.x = session->drawable_size.width / 3;
        start_point.y = 0;
        end_point.x = session->drawable_size.width / 3;
        end_point.y = session->drawable_size.height - 1;
        screensave_renderer_draw_line(renderer, &start_point, &end_point, highlight);
        start_point.x = (session->drawable_size.width * 2) / 3;
        end_point.x = (session->drawable_size.width * 2) / 3;
        screensave_renderer_draw_line(renderer, &start_point, &end_point, highlight);
        if (tier == GALLERY_TIER_GL33) {
            start_point.x = 0;
            start_point.y = session->drawable_size.height / 3;
            end_point.x = session->drawable_size.width - 1;
            end_point.y = session->drawable_size.height / 3;
            screensave_renderer_draw_line(renderer, &start_point, &end_point, beam);
            start_point.y = (session->drawable_size.height * 2) / 3;
            end_point.y = (session->drawable_size.height * 2) / 3;
            screensave_renderer_draw_line(renderer, &start_point, &end_point, beam);
        }
    }

    gallery_draw_marker_field(session, renderer, tier, frame, beam);
}

void gallery_render_session(
    screensave_saver_session *session,
    const screensave_saver_environment *environment
)
{
    screensave_renderer *renderer;
    screensave_renderer_info info;
    gallery_renderer_tier tier;

    if (session == NULL || environment == NULL || environment->renderer == NULL || session->theme == NULL) {
        return;
    }

    renderer = environment->renderer;
    screensave_renderer_get_info(renderer, &info);
    tier = gallery_tier_from_info(&info);

    switch (session->config.scene_mode) {
    case GALLERY_SCENE_NEON:
        gallery_draw_neon_scene(session, renderer, tier);
        break;
    case GALLERY_SCENE_SHOWCASE:
        gallery_draw_showcase_scene(session, renderer, tier);
        break;
    case GALLERY_SCENE_COMPATIBILITY:
    default:
        gallery_draw_compatibility_scene(session, renderer, tier);
        break;
    }
}
