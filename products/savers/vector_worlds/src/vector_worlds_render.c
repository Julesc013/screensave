#include "vector_worlds_internal.h"

static const int g_vector_worlds_ring_unit_x[16] = {
    1024, 946, 724, 392, 0, -392, -724, -946,
    -1024, -946, -724, -392, 0, 392, 724, 946
};

static const int g_vector_worlds_ring_unit_y[16] = {
    0, 392, 724, 946, 1024, 946, 724, 392,
    0, -392, -724, -946, -1024, -946, -724, -392
};

static screensave_color vector_worlds_mix_color(
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

static screensave_color vector_worlds_background_color(const screensave_saver_session *session)
{
    screensave_color color;

    color.red = 12;
    color.green = 14;
    color.blue = 20;
    color.alpha = 255;

    if (session == NULL || session->theme == NULL) {
        return color;
    }

    if (lstrcmpiA(session->theme->theme_key, "terminal_green_wireframe") == 0) {
        color.red = 4;
        color.green = 14;
        color.blue = 8;
    } else if (lstrcmpiA(session->theme->theme_key, "sgi_neon") == 0) {
        color.red = 8;
        color.green = 12;
        color.blue = 24;
    } else if (lstrcmpiA(session->theme->theme_key, "white_drafting_board") == 0) {
        color.red = 231;
        color.green = 228;
        color.blue = 219;
    } else if (lstrcmpiA(session->theme->theme_key, "museum_vector_exhibit") == 0) {
        color.red = 24;
        color.green = 24;
        color.blue = 28;
    }

    return color;
}

static screensave_color vector_worlds_line_color(
    const screensave_saver_session *session,
    unsigned int depth_bias,
    unsigned int glow_bias
)
{
    screensave_color base_color;
    screensave_color line_color;

    base_color = vector_worlds_background_color(session);
    if (session == NULL || session->theme == NULL) {
        return base_color;
    }

    if (lstrcmpiA(session->theme->theme_key, "white_drafting_board") == 0) {
        line_color = vector_worlds_mix_color(session->theme->primary_color, session->theme->accent_color, glow_bias);
        return vector_worlds_mix_color(base_color, line_color, 220U - depth_bias);
    }

    line_color = vector_worlds_mix_color(session->theme->primary_color, session->theme->accent_color, glow_bias);
    return vector_worlds_mix_color(base_color, line_color, 120U + depth_bias);
}

static int vector_worlds_wave(unsigned int phase)
{
    unsigned int value;

    phase &= 255U;
    if (phase < 128U) {
        value = phase;
    } else {
        value = 255U - phase;
    }

    return (int)value * 16 - 1024;
}

static int vector_worlds_clamp_int(int value, int lower, int upper)
{
    if (value < lower) {
        return lower;
    }
    if (value > upper) {
        return upper;
    }
    return value;
}

static void vector_worlds_scene_camera(
    const screensave_saver_session *session,
    int *camera_x,
    int *camera_y,
    int *camera_z,
    unsigned int *yaw_phase,
    unsigned int *pitch_phase,
    int *focal_length
)
{
    unsigned int variant;

    if (session == NULL) {
        return;
    }

    variant = (unsigned int)session->scene_variant;
    if (camera_x != NULL) {
        *camera_x = vector_worlds_wave((unsigned int)(session->camera_phase + variant * 13U)) / 10;
    }
    if (camera_y != NULL) {
        *camera_y = vector_worlds_wave((unsigned int)(session->camera_phase + 64U + variant * 9U)) / 12;
    }
    if (camera_z != NULL) {
        *camera_z = (int)((session->route_phase / 2UL) & 1023UL) - 256;
    }
    if (yaw_phase != NULL) {
        *yaw_phase = (unsigned int)(session->camera_phase + variant * 13U);
    }
    if (pitch_phase != NULL) {
        *pitch_phase = (unsigned int)(session->camera_phase + 64U + variant * 9U);
    }
    if (focal_length != NULL) {
        *focal_length = 220 + (int)session->config.detail_mode * 24;
    }
}

static int vector_worlds_project_point(
    const screensave_saver_session *session,
    int world_x,
    int world_y,
    int world_z,
    int camera_x,
    int camera_y,
    int camera_z,
    unsigned int yaw_phase,
    unsigned int pitch_phase,
    int focal_length,
    screensave_pointi *point,
    int *depth_out
)
{
    long x;
    long y;
    long z;
    long x1;
    long y1;
    long z1;
    long z2;
    int yaw_sin;
    int yaw_cos;
    int pitch_sin;
    int pitch_cos;

    if (session == NULL || point == NULL) {
        return 0;
    }

    x = (long)world_x - (long)camera_x;
    y = (long)world_y - (long)camera_y;
    z = (long)world_z - (long)camera_z;
    yaw_sin = vector_worlds_wave(yaw_phase);
    yaw_cos = vector_worlds_wave(yaw_phase + 64U);
    pitch_sin = vector_worlds_wave(pitch_phase);
    pitch_cos = vector_worlds_wave(pitch_phase + 64U);

    x1 = (x * (long)yaw_cos - z * (long)yaw_sin) / 1024L;
    z1 = (x * (long)yaw_sin + z * (long)yaw_cos) / 1024L;
    y1 = (y * (long)pitch_cos - z1 * (long)pitch_sin) / 1024L;
    z2 = (y * (long)pitch_sin + z1 * (long)pitch_cos) / 1024L;
    z2 += 384L;
    if (z2 < 48L) {
        return 0;
    }

    point->x = session->drawable_size.width / 2 + (int)((x1 * (long)focal_length) / z2);
    point->y = session->drawable_size.height / 2 - (int)((y1 * (long)focal_length) / z2);
    if (depth_out != NULL) {
        *depth_out = (int)z2;
    }
    return 1;
}

static void vector_worlds_draw_line(
    screensave_renderer *renderer,
    const screensave_saver_session *session,
    int x0,
    int y0,
    int z0,
    int x1,
    int y1,
    int z1,
    unsigned int depth_bias,
    unsigned int glow_bias,
    int camera_x,
    int camera_y,
    int camera_z,
    unsigned int yaw_phase,
    unsigned int pitch_phase,
    int focal_length
)
{
    screensave_pointi start_point;
    screensave_pointi end_point;
    int depth;

    if (
        renderer == NULL ||
        !vector_worlds_project_point(session, x0, y0, z0, camera_x, camera_y, camera_z, yaw_phase, pitch_phase, focal_length, &start_point, &depth) ||
        !vector_worlds_project_point(session, x1, y1, z1, camera_x, camera_y, camera_z, yaw_phase, pitch_phase, focal_length, &end_point, NULL)
    ) {
        return;
    }

    screensave_renderer_draw_line(
        renderer,
        &start_point,
        &end_point,
        vector_worlds_line_color(session, depth_bias + (unsigned int)vector_worlds_clamp_int(depth / 16, 0, 120), glow_bias)
    );
}

static void vector_worlds_draw_ring(
    screensave_renderer *renderer,
    const screensave_saver_session *session,
    int center_x,
    int center_y,
    int center_z,
    int radius,
    unsigned int depth_bias,
    unsigned int glow_bias,
    int camera_x,
    int camera_y,
    int camera_z,
    unsigned int yaw_phase,
    unsigned int pitch_phase,
    int focal_length
)
{
    screensave_pointi points[17];
    unsigned int index;

    for (index = 0U; index < 16U; ++index) {
        if (
            !vector_worlds_project_point(
                session,
                center_x + (g_vector_worlds_ring_unit_x[index] * radius) / 1024,
                center_y + (g_vector_worlds_ring_unit_y[index] * radius) / 1024,
                center_z,
                camera_x,
                camera_y,
                camera_z,
                yaw_phase,
                pitch_phase,
                focal_length,
                &points[index],
                NULL
            )
        ) {
            return;
        }
    }
    points[16] = points[0];
    screensave_renderer_draw_polyline(renderer, points, 17U, vector_worlds_line_color(session, depth_bias, glow_bias));
}

static void vector_worlds_draw_object(
    screensave_renderer *renderer,
    const screensave_saver_session *session,
    const vector_worlds_object *object,
    int camera_x,
    int camera_y,
    int camera_z,
    unsigned int yaw_phase,
    unsigned int pitch_phase,
    int focal_length
)
{
    static const int cube_vertices[8][3] = {
        { -1, -1, -1 }, { 1, -1, -1 }, { 1, 1, -1 }, { -1, 1, -1 },
        { -1, -1, 1 }, { 1, -1, 1 }, { 1, 1, 1 }, { -1, 1, 1 }
    };
    static const unsigned char cube_edges[12][2] = {
        { 0, 1 }, { 1, 2 }, { 2, 3 }, { 3, 0 },
        { 4, 5 }, { 5, 6 }, { 6, 7 }, { 7, 4 },
        { 0, 4 }, { 1, 5 }, { 2, 6 }, { 3, 7 }
    };
    static const int pyramid_vertices[5][3] = {
        { -1, -1, -1 }, { 1, -1, -1 }, { 1, -1, 1 }, { -1, -1, 1 }, { 0, 1, 0 }
    };
    static const unsigned char pyramid_edges[8][2] = {
        { 0, 1 }, { 1, 2 }, { 2, 3 }, { 3, 0 },
        { 0, 4 }, { 1, 4 }, { 2, 4 }, { 3, 4 }
    };
    static const int gate_vertices[8][3] = {
        { -1, -1, -1 }, { 1, -1, -1 }, { 1, 1, -1 }, { -1, 1, -1 },
        { -1, -1, 1 }, { 1, -1, 1 }, { 1, 1, 1 }, { -1, 1, 1 }
    };
    static const unsigned char gate_edges[12][2] = {
        { 0, 1 }, { 1, 2 }, { 2, 3 }, { 3, 0 },
        { 4, 5 }, { 5, 6 }, { 6, 7 }, { 7, 4 },
        { 0, 4 }, { 1, 5 }, { 2, 6 }, { 3, 7 }
    };
    const int (*vertices)[3];
    const unsigned char (*edges)[2];
    unsigned int edge_count;
    unsigned int vertex_count;
    screensave_pointi projected[8];
    unsigned int index;
    int local_x;
    int local_y;
    int local_z;
    int twist_phase;
    int base_x;
    int base_y;
    int base_z;

    if (renderer == NULL || session == NULL || object == NULL) {
        return;
    }

    vertices = cube_vertices;
    edges = cube_edges;
    vertex_count = 8U;
    edge_count = 12U;
    if (object->kind == VECTOR_WORLDS_OBJECT_PYRAMID) {
        vertices = pyramid_vertices;
        edges = pyramid_edges;
        vertex_count = 5U;
        edge_count = 8U;
    } else if (object->kind == VECTOR_WORLDS_OBJECT_GATE) {
        vertices = gate_vertices;
        edges = gate_edges;
        vertex_count = 8U;
        edge_count = 12U;
    }

    base_x = object->x;
    base_y = object->y;
    base_z = object->z;
    for (index = 0U; index < vertex_count; ++index) {
        twist_phase = (int)((unsigned int)object->twist + object->phase + (unsigned int)(index * 16U));
        local_x = vertices[index][0] * object->scale;
        local_y = vertices[index][1] * object->scale;
        local_z = vertices[index][2] * object->scale;
        local_x = (local_x * vector_worlds_wave((unsigned int)twist_phase) - local_z * vector_worlds_wave((unsigned int)twist_phase + 64U)) / 1024;
        local_z = (vertices[index][0] * object->scale * vector_worlds_wave((unsigned int)twist_phase + 64U) + local_z * vector_worlds_wave((unsigned int)twist_phase)) / 1024;
        if (
            !vector_worlds_project_point(
                session,
                base_x + local_x,
                base_y + local_y,
                base_z + local_z,
                camera_x,
                camera_y,
                camera_z,
                yaw_phase,
                pitch_phase,
                focal_length,
                &projected[index],
                NULL
            )
        ) {
            return;
        }
    }

    for (index = 0U; index < edge_count; ++index) {
        screensave_renderer_draw_line(
            renderer,
            &projected[edges[index][0]],
            &projected[edges[index][1]],
            vector_worlds_line_color(session, 36U + (unsigned int)object->scale, (unsigned int)object->phase)
        );
    }
}

static int vector_worlds_terrain_height(unsigned long seed, int x, int z, unsigned long variation)
{
    unsigned long hash;
    int height;

    hash = vector_worlds_hash(seed + (unsigned long)(x * 13) + (unsigned long)(z * 17) + variation * 29UL);
    height = vector_worlds_signed_range(hash, 72);
    height += vector_worlds_signed_range(vector_worlds_hash(hash + 7UL), 32) / 2;
    return height;
}

static void vector_worlds_render_field_scene(
    screensave_saver_session *session,
    screensave_renderer *renderer,
    int camera_x,
    int camera_y,
    int camera_z,
    unsigned int yaw_phase,
    unsigned int pitch_phase,
    int focal_length
)
{
    unsigned int index;
    int grid_index;

    screensave_renderer_clear(renderer, vector_worlds_background_color(session));

    for (grid_index = -2; grid_index <= 2; ++grid_index) {
        vector_worlds_draw_line(
            renderer,
            session,
            -180,
            -80,
            grid_index * 120,
            180,
            -80,
            grid_index * 120,
            40U,
            32U,
            camera_x,
            camera_y,
            camera_z,
            yaw_phase,
            pitch_phase,
            focal_length
        );
    }
    for (grid_index = -2; grid_index <= 2; ++grid_index) {
        vector_worlds_draw_line(
            renderer,
            session,
            grid_index * 80,
            -120,
            -180,
            grid_index * 80,
            -120,
            180,
            40U,
            32U,
            camera_x,
            camera_y,
            camera_z,
            yaw_phase,
            pitch_phase,
            focal_length
        );
    }

    for (index = 0U; index < session->object_count; ++index) {
        vector_worlds_draw_object(renderer, session, &session->objects[index], camera_x, camera_y, camera_z, yaw_phase, pitch_phase, focal_length);
        if (index + 1U < session->object_count) {
            vector_worlds_draw_line(
                renderer,
                session,
                session->objects[index].x,
                session->objects[index].y,
                session->objects[index].z,
                session->objects[index + 1U].x,
                session->objects[index + 1U].y,
                session->objects[index + 1U].z,
                32U,
                24U,
                camera_x,
                camera_y,
                camera_z,
                yaw_phase,
                pitch_phase,
                focal_length
            );
        }
    }
}

static void vector_worlds_render_tunnel_scene(
    screensave_saver_session *session,
    screensave_renderer *renderer,
    int camera_x,
    int camera_y,
    int camera_z,
    unsigned int yaw_phase,
    unsigned int pitch_phase,
    int focal_length
)
{
    unsigned int index;
    int route_offset;

    screensave_renderer_clear(renderer, vector_worlds_background_color(session));
    route_offset = (int)((session->route_phase / 2UL) & 255UL) - 128;

    for (index = 0U; index < session->object_count; ++index) {
        const vector_worlds_object *object;
        int center_x;
        int center_y;
        int center_z;
        int radius;

        object = &session->objects[index];
        center_x = object->x + vector_worlds_wave((unsigned int)(session->camera_phase + object->phase)) / 24;
        center_y = object->y + vector_worlds_wave((unsigned int)(session->camera_phase + object->phase + 64U)) / 28;
        center_z = object->z - route_offset;
        radius = object->scale + 10 + (int)(index % 3U);
        vector_worlds_draw_ring(renderer, session, center_x, center_y, center_z, radius, 40U, object->phase, camera_x, camera_y, camera_z, yaw_phase, pitch_phase, focal_length);
        if (index + 1U < session->object_count) {
            vector_worlds_draw_line(
                renderer,
                session,
                center_x,
                center_y,
                center_z,
                session->objects[index + 1U].x,
                session->objects[index + 1U].y,
                session->objects[index + 1U].z - route_offset,
                48U,
                object->phase,
                camera_x,
                camera_y,
                camera_z,
                yaw_phase,
                pitch_phase,
                focal_length
            );
        }
    }

    vector_worlds_draw_line(renderer, session, -160, -120, -180, 160, -120, 180, 44U, 48U, camera_x, camera_y, camera_z, yaw_phase, pitch_phase, focal_length);
    vector_worlds_draw_line(renderer, session, -120, 120, -180, 120, 120, 180, 44U, 48U, camera_x, camera_y, camera_z, yaw_phase, pitch_phase, focal_length);
}

static void vector_worlds_render_terrain_scene(
    screensave_saver_session *session,
    screensave_renderer *renderer,
    int camera_x,
    int camera_y,
    int camera_z,
    unsigned int yaw_phase,
    unsigned int pitch_phase,
    int focal_length
)
{
    int row;
    int column;
    int row_count;
    int column_count;
    int base_z;
    int spacing;

    screensave_renderer_clear(renderer, vector_worlds_background_color(session));
    row_count = 6 + session->config.detail_mode * 2;
    column_count = 10 + session->config.detail_mode * 3;
    spacing = 44;
    base_z = (int)((session->route_phase / 3UL) & 255UL) - 128;

    for (row = 0; row < row_count; ++row) {
        screensave_pointi points[20];
        int point_count;
        int z;

        point_count = 0;
        z = base_z + (row * 48) - (row_count * 24);
        for (column = 0; column <= column_count; ++column) {
            int world_x;
            int world_y;
            int world_z;

            world_x = (column * spacing) - ((column_count * spacing) / 2);
            world_z = z;
            world_y = vector_worlds_terrain_height(session->scene_seed, world_x, world_z, session->scene_variant);
            if (
                vector_worlds_project_point(
                    session,
                    world_x,
                    world_y,
                    world_z,
                    camera_x,
                    camera_y,
                    camera_z,
                    yaw_phase,
                    pitch_phase,
                    focal_length,
                    &points[point_count],
                    NULL
                )
            ) {
                ++point_count;
            }
        }
        if (point_count >= 2) {
            screensave_renderer_draw_polyline(renderer, points, (unsigned int)point_count, vector_worlds_line_color(session, 34U, (unsigned int)row * 12U));
        }
    }

    for (column = 0; column < column_count; ++column) {
        vector_worlds_draw_line(
            renderer,
            session,
            (column * spacing) - ((column_count * spacing) / 2),
            -92,
            base_z - 108,
            (column * spacing) - ((column_count * spacing) / 2),
            -92,
            base_z + 108,
            38U,
            (unsigned int)column * 10U,
            camera_x,
            camera_y,
            camera_z,
            yaw_phase,
            pitch_phase,
            focal_length
        );
    }

    for (column = 0; column < (int)session->object_count; ++column) {
        vector_worlds_draw_object(renderer, session, &session->objects[column], camera_x, camera_y, camera_z, yaw_phase, pitch_phase, focal_length);
    }
}

void vector_worlds_render_session(
    screensave_saver_session *session,
    const screensave_saver_environment *environment
)
{
    screensave_renderer *renderer;
    int camera_x;
    int camera_y;
    int camera_z;
    unsigned int yaw_phase;
    unsigned int pitch_phase;
    int focal_length;

    if (session == NULL || environment == NULL || environment->renderer == NULL || session->theme == NULL) {
        return;
    }

    renderer = environment->renderer;
    vector_worlds_scene_camera(session, &camera_x, &camera_y, &camera_z, &yaw_phase, &pitch_phase, &focal_length);

    switch (session->config.scene_mode) {
    case VECTOR_WORLDS_SCENE_TUNNEL:
        vector_worlds_render_tunnel_scene(session, renderer, camera_x, camera_y, camera_z, yaw_phase, pitch_phase, focal_length);
        break;

    case VECTOR_WORLDS_SCENE_TERRAIN:
        vector_worlds_render_terrain_scene(session, renderer, camera_x, camera_y, camera_z, yaw_phase, pitch_phase, focal_length);
        break;

    case VECTOR_WORLDS_SCENE_FIELD:
    default:
        vector_worlds_render_field_scene(session, renderer, camera_x, camera_y, camera_z, yaw_phase, pitch_phase, focal_length);
        break;
    }
}
