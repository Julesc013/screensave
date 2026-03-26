#include "scr_internal.h"

static const unsigned long g_scr_validation_bitmap[64] = {
    0x00000000UL, 0x00000000UL, 0x0000FF00UL, 0x0000FF00UL, 0x0000FF00UL, 0x0000FF00UL, 0x00000000UL, 0x00000000UL,
    0x00000000UL, 0x0000FF00UL, 0x00000000UL, 0x00000000UL, 0x00000000UL, 0x00000000UL, 0x0000FF00UL, 0x00000000UL,
    0x0000FF00UL, 0x00000000UL, 0x00FFFFFFUL, 0x00FFFFFFUL, 0x00FFFFFFUL, 0x00FFFFFFUL, 0x00000000UL, 0x0000FF00UL,
    0x0000FF00UL, 0x00000000UL, 0x00FFFFFFUL, 0x00000000UL, 0x00000000UL, 0x00FFFFFFUL, 0x00000000UL, 0x0000FF00UL,
    0x0000FF00UL, 0x00000000UL, 0x00FFFFFFUL, 0x00000000UL, 0x00000000UL, 0x00FFFFFFUL, 0x00000000UL, 0x0000FF00UL,
    0x0000FF00UL, 0x00000000UL, 0x00FFFFFFUL, 0x00FFFFFFUL, 0x00FFFFFFUL, 0x00FFFFFFUL, 0x00000000UL, 0x0000FF00UL,
    0x00000000UL, 0x0000FF00UL, 0x00000000UL, 0x00000000UL, 0x00000000UL, 0x00000000UL, 0x0000FF00UL, 0x00000000UL,
    0x00000000UL, 0x00000000UL, 0x0000FF00UL, 0x0000FF00UL, 0x0000FF00UL, 0x0000FF00UL, 0x00000000UL, 0x00000000UL
};

static int scr_scene_speed_units(const scr_host_context *context)
{
    switch (context->settings.common.detail_level) {
    case SCREENSAVE_DETAIL_LEVEL_LOW:
        return 2;

    case SCREENSAVE_DETAIL_LEVEL_HIGH:
        return 5;

    case SCREENSAVE_DETAIL_LEVEL_STANDARD:
    default:
        return 3;
    }
}

void scr_render_validation_scene(scr_host_context *context)
{
    screensave_renderer_info renderer_info;
    screensave_frame_info frame_info;
    screensave_recti frame_rect;
    screensave_recti moving_rect;
    screensave_recti bitmap_rect;
    screensave_pointi line_start;
    screensave_pointi line_end;
    screensave_pointi wave_points[4];
    screensave_bitmap_view bitmap_view;
    screensave_color background;
    screensave_color border;
    screensave_color accent;
    screensave_color highlight;
    int size;
    int span;
    int x;
    int y;
    int speed_units;

    if (context == NULL || context->renderer == NULL) {
        return;
    }

    screensave_renderer_get_info(context->renderer, &renderer_info);
    if (renderer_info.drawable_size.width <= 0 || renderer_info.drawable_size.height <= 0) {
        return;
    }

    frame_info.drawable_size = renderer_info.drawable_size;
    frame_info.frame_index = context->clock.frame_index;
    frame_info.elapsed_millis = context->clock.elapsed_millis;
    frame_info.delta_millis = context->clock.delta_millis;

    if (!screensave_renderer_begin_frame(context->renderer, &frame_info)) {
        scr_emit_host_diagnostic(
            context,
            SCREENSAVE_DIAG_LEVEL_WARNING,
            2401UL,
            "scr_validation_scene",
            "The validation scene could not begin a renderer frame."
        );
        return;
    }

    background.red = 0;
    background.green = 0;
    background.blue = 0;
    background.alpha = 255;
    screensave_renderer_clear(context->renderer, background);

    frame_rect.x = 4;
    frame_rect.y = 4;
    frame_rect.width = renderer_info.drawable_size.width - 8;
    frame_rect.height = renderer_info.drawable_size.height - 8;

    border.red = context->preview_mode ? 96 : 48;
    border.green = context->preview_mode ? 96 : 160;
    border.blue = context->preview_mode ? 96 : 72;
    border.alpha = 255;
    screensave_renderer_draw_frame_rect(context->renderer, &frame_rect, border);

    size = renderer_info.drawable_size.height / 5;
    if (size < 10) {
        size = 10;
    }
    if (size > renderer_info.drawable_size.width / 2) {
        size = renderer_info.drawable_size.width / 2;
    }

    speed_units = scr_scene_speed_units(context);
    span = renderer_info.drawable_size.width - size - 16;
    if (span < 1) {
        span = 1;
    }

    x = 8 + (int)(((context->clock.frame_index * (unsigned long)speed_units) + context->session_seed.base_seed) % (unsigned long)span);
    y = (renderer_info.drawable_size.height - size) / 2;

    moving_rect.x = x;
    moving_rect.y = y;
    moving_rect.width = size;
    moving_rect.height = size;

    accent.red = 24;
    accent.green = context->session_seed.deterministic ? 176 : 112;
    accent.blue = context->preview_mode ? 112 : 40;
    accent.alpha = 255;
    screensave_renderer_fill_rect(context->renderer, &moving_rect, accent);

    highlight.red = 255;
    highlight.green = 255;
    highlight.blue = 255;
    highlight.alpha = 255;
    screensave_renderer_draw_frame_rect(context->renderer, &moving_rect, highlight);

    line_start.x = 8;
    line_start.y = renderer_info.drawable_size.height - 12;
    line_end.x = moving_rect.x + (moving_rect.width / 2);
    line_end.y = moving_rect.y + (moving_rect.height / 2);
    screensave_renderer_draw_line(context->renderer, &line_start, &line_end, border);

    wave_points[0].x = 8;
    wave_points[0].y = renderer_info.drawable_size.height - 20;
    wave_points[1].x = renderer_info.drawable_size.width / 3;
    wave_points[1].y = renderer_info.drawable_size.height - 28;
    wave_points[2].x = (renderer_info.drawable_size.width * 2) / 3;
    wave_points[2].y = renderer_info.drawable_size.height - 12;
    wave_points[3].x = renderer_info.drawable_size.width - 8;
    wave_points[3].y = renderer_info.drawable_size.height - 24;
    screensave_renderer_draw_polyline(context->renderer, wave_points, 4U, highlight);

    bitmap_rect.width = 16;
    bitmap_rect.height = 16;
    bitmap_rect.x = renderer_info.drawable_size.width - bitmap_rect.width - 8;
    bitmap_rect.y = 8;

    bitmap_view.pixels = g_scr_validation_bitmap;
    bitmap_view.size.width = 8;
    bitmap_view.size.height = 8;
    bitmap_view.stride_bytes = 8 * 4;
    bitmap_view.bits_per_pixel = 32U;
    bitmap_view.origin_top_left = 1;
    (void)screensave_renderer_blit_bitmap(context->renderer, &bitmap_view, &bitmap_rect);

    if (!screensave_renderer_end_frame(context->renderer)) {
        scr_emit_host_diagnostic(
            context,
            SCREENSAVE_DIAG_LEVEL_WARNING,
            2402UL,
            "scr_validation_scene",
            "The validation scene could not present the renderer frame."
        );
    }
}
