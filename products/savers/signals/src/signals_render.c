#include "signals_internal.h"

static screensave_color signals_background_color(const screensave_saver_session *session)
{
    screensave_color color;

    color.red = 4;
    color.green = 12;
    color.blue = 8;
    color.alpha = 255;
    if (session == NULL || session->theme == NULL) {
        return color;
    }

    if (lstrcmpiA(session->theme->theme_key, "amber_scope") == 0) {
        color.red = 18;
        color.green = 10;
        color.blue = 4;
    } else if (lstrcmpiA(session->theme->theme_key, "telemetry_wall") == 0) {
        color.red = 4;
        color.green = 12;
        color.blue = 26;
    } else if (lstrcmpiA(session->theme->theme_key, "night_watch_console") == 0) {
        color.red = 8;
        color.green = 12;
        color.blue = 20;
    } else if (lstrcmpiA(session->theme->theme_key, "blue_lab") == 0) {
        color.red = 4;
        color.green = 10;
        color.blue = 22;
    } else if (lstrcmpiA(session->theme->theme_key, "white_technical_board") == 0) {
        color.red = 226;
        color.green = 232;
        color.blue = 236;
    } else if (lstrcmpiA(session->theme->theme_key, "quiet_night_console") == 0) {
        color.red = 10;
        color.green = 14;
        color.blue = 18;
    }

    return color;
}

static screensave_color signals_mix_color(
    screensave_color base,
    screensave_color accent,
    unsigned int accent_amount
)
{
    unsigned int base_amount;

    if (accent_amount > 255U) {
        accent_amount = 255U;
    }
    base_amount = 255U - accent_amount;

    base.red = (unsigned char)(((unsigned int)base.red * base_amount + (unsigned int)accent.red * accent_amount) / 255U);
    base.green = (unsigned char)(((unsigned int)base.green * base_amount + (unsigned int)accent.green * accent_amount) / 255U);
    base.blue = (unsigned char)(((unsigned int)base.blue * base_amount + (unsigned int)accent.blue * accent_amount) / 255U);
    return base;
}

static void signals_draw_rect_fill(
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

static void signals_draw_panel(
    screensave_renderer *renderer,
    const screensave_recti *rect,
    screensave_color fill_color,
    screensave_color frame_color
)
{
    if (renderer == NULL || rect == NULL || rect->width <= 0 || rect->height <= 0) {
        return;
    }

    screensave_renderer_fill_rect(renderer, rect, fill_color);
    screensave_renderer_draw_frame_rect(renderer, rect, frame_color);
}

static void signals_draw_panel_grid(
    screensave_renderer *renderer,
    const screensave_recti *rect,
    screensave_color color,
    int columns,
    int rows
)
{
    screensave_pointi start_point;
    screensave_pointi end_point;
    int column;
    int row;

    if (renderer == NULL || rect == NULL || columns < 2 || rows < 2) {
        return;
    }

    for (column = 1; column < columns; ++column) {
        start_point.x = rect->x + (rect->width * column) / columns;
        start_point.y = rect->y + 1;
        end_point.x = start_point.x;
        end_point.y = rect->y + rect->height - 2;
        screensave_renderer_draw_line(renderer, &start_point, &end_point, color);
    }

    for (row = 1; row < rows; ++row) {
        start_point.x = rect->x + 1;
        start_point.y = rect->y + (rect->height * row) / rows;
        end_point.x = rect->x + rect->width - 2;
        end_point.y = start_point.y;
        screensave_renderer_draw_line(renderer, &start_point, &end_point, color);
    }
}

static void signals_draw_scope(
    screensave_renderer *renderer,
    const screensave_recti *rect,
    const int *values,
    unsigned int value_count,
    screensave_color trace_color
)
{
    screensave_pointi points[SIGNALS_SCOPE_SAMPLE_COUNT];
    unsigned int index;

    if (
        renderer == NULL ||
        rect == NULL ||
        values == NULL ||
        value_count < 2U ||
        rect->width <= 4 ||
        rect->height <= 4
    ) {
        return;
    }

    if (value_count > SIGNALS_SCOPE_SAMPLE_COUNT) {
        value_count = SIGNALS_SCOPE_SAMPLE_COUNT;
    }

    for (index = 0U; index < value_count; ++index) {
        points[index].x = rect->x + 2 + (int)((rect->width - 4) * index / (value_count - 1U));
        points[index].y = rect->y + rect->height - 3 - ((rect->height - 6) * values[index] / 100);
    }

    screensave_renderer_draw_polyline(renderer, points, value_count, trace_color);
}

static void signals_draw_meter_stack(
    screensave_renderer *renderer,
    const screensave_recti *rect,
    const screensave_saver_session *session
)
{
    int bar_height;
    int index;
    screensave_color dim_color;
    screensave_color live_color;

    if (renderer == NULL || rect == NULL || session == NULL || session->theme == NULL) {
        return;
    }

    bar_height = rect->height / (int)SIGNALS_METER_COUNT;
    if (bar_height < 4) {
        bar_height = 4;
    }
    dim_color = signals_mix_color(signals_background_color(session), session->theme->primary_color, 112U);
    live_color = session->theme->accent_color;

    for (index = 0; index < (int)SIGNALS_METER_COUNT; ++index) {
        int bar_width;
        int y;

        y = rect->y + (index * bar_height);
        if (y + bar_height > rect->y + rect->height) {
            break;
        }

        signals_draw_rect_fill(renderer, rect->x + 2, y + 2, rect->width - 4, bar_height - 3, dim_color);
        bar_width = ((rect->width - 6) * session->meter_levels[index]) / 100;
        signals_draw_rect_fill(renderer, rect->x + 3, y + 3, bar_width, bar_height - 5, live_color);
    }
}

static void signals_draw_status_panel(
    screensave_renderer *renderer,
    const screensave_recti *rect,
    const screensave_saver_session *session
)
{
    int columns;
    int rows;
    int cell_width;
    int cell_height;
    int index;
    screensave_color on_color;
    screensave_color off_color;

    if (renderer == NULL || rect == NULL || session == NULL || session->theme == NULL) {
        return;
    }

    columns = 4;
    rows = 2;
    cell_width = rect->width / columns;
    cell_height = rect->height / rows;
    on_color = session->theme->accent_color;
    off_color = signals_mix_color(signals_background_color(session), session->theme->primary_color, 72U);

    for (index = 0; index < (int)SIGNALS_STATUS_COUNT; ++index) {
        int column;
        int row;
        screensave_color cell_color;

        column = index % columns;
        row = index / columns;
        cell_color = session->status_flags[index] ? on_color : off_color;
        signals_draw_rect_fill(
            renderer,
            rect->x + column * cell_width + 3,
            rect->y + row * cell_height + 3,
            cell_width - 6,
            cell_height - 6,
            cell_color
        );
    }
}

static void signals_draw_counter_strip(
    screensave_renderer *renderer,
    const screensave_recti *rect,
    const screensave_saver_session *session
)
{
    int index;
    int slot_width;
    screensave_color fill_color;
    screensave_color line_color;

    if (renderer == NULL || rect == NULL || session == NULL || session->theme == NULL) {
        return;
    }

    slot_width = rect->width / 3;
    fill_color = signals_mix_color(signals_background_color(session), session->theme->primary_color, 86U);
    line_color = session->theme->accent_color;

    for (index = 0; index < 3; ++index) {
        int value_width;

        signals_draw_rect_fill(
            renderer,
            rect->x + index * slot_width + 3,
            rect->y + 3,
            slot_width - 6,
            rect->height - 6,
            fill_color
        );
        value_width = ((slot_width - 10) * (int)(session->counters[index] % 1000U)) / 1000;
        signals_draw_rect_fill(
            renderer,
            rect->x + index * slot_width + 5,
            rect->y + rect->height / 2,
            value_width,
            rect->height / 2 - 8,
            line_color
        );
    }
}

static void signals_draw_scan_overlay(
    screensave_renderer *renderer,
    const screensave_saver_session *session
)
{
    int y;
    screensave_pointi start_point;
    screensave_pointi end_point;
    screensave_color scan_color;

    if (renderer == NULL || session == NULL || session->theme == NULL) {
        return;
    }

    scan_color = signals_mix_color(signals_background_color(session), session->theme->primary_color, 54U);
    for (y = 0; y < session->drawable_size.height; y += 4) {
        start_point.x = 0;
        start_point.y = y;
        end_point.x = session->drawable_size.width;
        end_point.y = y;
        screensave_renderer_draw_line(renderer, &start_point, &end_point, scan_color);
    }
}

static void signals_render_operations(screensave_saver_session *session, screensave_renderer *renderer)
{
    screensave_recti left_scope;
    screensave_recti right_top;
    screensave_recti right_mid;
    screensave_recti right_bottom;
    screensave_color panel_fill;
    screensave_color panel_frame;
    screensave_color grid_color;

    left_scope.x = 14;
    left_scope.y = 14;
    left_scope.width = (session->drawable_size.width * 60) / 100 - 18;
    left_scope.height = session->drawable_size.height - 28;

    right_top.x = left_scope.x + left_scope.width + 10;
    right_top.y = 14;
    right_top.width = session->drawable_size.width - right_top.x - 14;
    right_top.height = (session->drawable_size.height - 36) / 3;

    right_mid = right_top;
    right_mid.y += right_top.height + 4;
    right_bottom = right_mid;
    right_bottom.y += right_mid.height + 4;

    panel_fill = signals_mix_color(signals_background_color(session), session->theme->primary_color, 40U);
    panel_frame = signals_mix_color(signals_background_color(session), session->theme->accent_color, 160U);
    grid_color = signals_mix_color(signals_background_color(session), session->theme->primary_color, 92U);

    signals_draw_panel(renderer, &left_scope, panel_fill, panel_frame);
    signals_draw_panel(renderer, &right_top, panel_fill, panel_frame);
    signals_draw_panel(renderer, &right_mid, panel_fill, panel_frame);
    signals_draw_panel(renderer, &right_bottom, panel_fill, panel_frame);
    if (session->config.overlay_mode != SIGNALS_OVERLAY_OFF) {
        signals_draw_panel_grid(renderer, &left_scope, grid_color, 6, 6);
    }
    signals_draw_scope(renderer, &left_scope, session->scope_values[0], SIGNALS_SCOPE_SAMPLE_COUNT, session->theme->accent_color);
    signals_draw_meter_stack(renderer, &right_top, session);
    signals_draw_status_panel(renderer, &right_mid, session);
    signals_draw_counter_strip(renderer, &right_bottom, session);
}

static void signals_render_spectrum(screensave_saver_session *session, screensave_renderer *renderer)
{
    screensave_recti top_scope;
    screensave_recti lower_left;
    screensave_recti lower_mid;
    screensave_recti lower_right;
    screensave_color panel_fill;
    screensave_color panel_frame;
    screensave_color sweep_color;
    screensave_pointi sweep_start;
    screensave_pointi sweep_end;

    top_scope.x = 14;
    top_scope.y = 14;
    top_scope.width = session->drawable_size.width - 28;
    top_scope.height = (session->drawable_size.height * 52) / 100;

    lower_left.x = 14;
    lower_left.y = top_scope.y + top_scope.height + 8;
    lower_left.width = (session->drawable_size.width - 40) / 3;
    lower_left.height = session->drawable_size.height - lower_left.y - 14;
    lower_mid = lower_left;
    lower_mid.x += lower_left.width + 6;
    lower_right = lower_mid;
    lower_right.x += lower_mid.width + 6;

    panel_fill = signals_mix_color(signals_background_color(session), session->theme->primary_color, 34U);
    panel_frame = signals_mix_color(signals_background_color(session), session->theme->accent_color, 164U);
    signals_draw_panel(renderer, &top_scope, panel_fill, panel_frame);
    signals_draw_panel(renderer, &lower_left, panel_fill, panel_frame);
    signals_draw_panel(renderer, &lower_mid, panel_fill, panel_frame);
    signals_draw_panel(renderer, &lower_right, panel_fill, panel_frame);
    signals_draw_panel_grid(
        renderer,
        &top_scope,
        signals_mix_color(signals_background_color(session), session->theme->primary_color, 74U),
        8,
        6
    );
    signals_draw_scope(renderer, &top_scope, session->scope_values[0], SIGNALS_SCOPE_SAMPLE_COUNT, session->theme->accent_color);
    signals_draw_scope(
        renderer,
        &top_scope,
        session->scope_values[1],
        SIGNALS_SCOPE_SAMPLE_COUNT,
        signals_mix_color(session->theme->primary_color, session->theme->accent_color, 132U)
    );

    sweep_color = signals_mix_color(signals_background_color(session), session->theme->accent_color, 180U);
    sweep_start.x = top_scope.x + 2 + (int)((top_scope.width - 4) * session->sweep_position / 100U);
    sweep_start.y = top_scope.y + 2;
    sweep_end.x = sweep_start.x;
    sweep_end.y = top_scope.y + top_scope.height - 3;
    screensave_renderer_draw_line(renderer, &sweep_start, &sweep_end, sweep_color);

    signals_draw_meter_stack(renderer, &lower_left, session);
    signals_draw_status_panel(renderer, &lower_mid, session);
    signals_draw_counter_strip(renderer, &lower_right, session);
}

static void signals_render_telemetry(screensave_saver_session *session, screensave_renderer *renderer)
{
    screensave_recti upper_left;
    screensave_recti upper_right;
    screensave_recti lower_left;
    screensave_recti lower_right;
    screensave_color panel_fill;
    screensave_color panel_frame;
    screensave_color trace_color;

    upper_left.x = 14;
    upper_left.y = 14;
    upper_left.width = (session->drawable_size.width - 34) / 2;
    upper_left.height = (session->drawable_size.height - 34) / 2;
    upper_right = upper_left;
    upper_right.x += upper_left.width + 6;
    lower_left = upper_left;
    lower_left.y += upper_left.height + 6;
    lower_right = upper_right;
    lower_right.y = lower_left.y;

    panel_fill = signals_mix_color(signals_background_color(session), session->theme->primary_color, 28U);
    panel_frame = signals_mix_color(signals_background_color(session), session->theme->accent_color, 156U);
    trace_color = signals_mix_color(session->theme->primary_color, session->theme->accent_color, 176U);

    signals_draw_panel(renderer, &upper_left, panel_fill, panel_frame);
    signals_draw_panel(renderer, &upper_right, panel_fill, panel_frame);
    signals_draw_panel(renderer, &lower_left, panel_fill, panel_frame);
    signals_draw_panel(renderer, &lower_right, panel_fill, panel_frame);
    signals_draw_panel_grid(renderer, &upper_left, signals_mix_color(panel_fill, panel_frame, 90U), 6, 4);
    signals_draw_panel_grid(renderer, &upper_right, signals_mix_color(panel_fill, panel_frame, 90U), 6, 4);
    signals_draw_scope(renderer, &upper_left, session->scope_values[0], SIGNALS_SCOPE_SAMPLE_COUNT, trace_color);
    signals_draw_scope(renderer, &upper_right, session->scope_values[1], SIGNALS_SCOPE_SAMPLE_COUNT, session->theme->accent_color);
    signals_draw_meter_stack(renderer, &lower_left, session);
    signals_draw_counter_strip(renderer, &lower_right, session);
}

void signals_render_session(
    screensave_saver_session *session,
    const screensave_saver_environment *environment
)
{
    screensave_recti outer_frame;
    screensave_color frame_color;

    if (session == NULL || environment == NULL || environment->renderer == NULL || session->theme == NULL) {
        return;
    }

    screensave_renderer_clear(environment->renderer, signals_background_color(session));
    if (session->drawable_size.width < 80 || session->drawable_size.height < 60) {
        return;
    }

    if (session->config.overlay_mode == SIGNALS_OVERLAY_SCAN) {
        signals_draw_scan_overlay(environment->renderer, session);
    }

    if (session->preview_mode) {
        signals_render_operations(session, environment->renderer);
    } else {
        switch (session->config.layout_mode) {
        case SIGNALS_LAYOUT_SPECTRUM:
            signals_render_spectrum(session, environment->renderer);
            break;

        case SIGNALS_LAYOUT_TELEMETRY:
            signals_render_telemetry(session, environment->renderer);
            break;

        case SIGNALS_LAYOUT_OPERATIONS:
        default:
            signals_render_operations(session, environment->renderer);
            break;
        }
    }

    if (!session->preview_mode) {
        frame_color = signals_mix_color(signals_background_color(session), session->theme->accent_color, 128U);
        outer_frame.x = 6;
        outer_frame.y = 6;
        outer_frame.width = session->drawable_size.width - 12;
        outer_frame.height = session->drawable_size.height - 12;
        if (outer_frame.width > 0 && outer_frame.height > 0) {
            screensave_renderer_draw_frame_rect(environment->renderer, &outer_frame, frame_color);
        }
    }
}
