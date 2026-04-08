#include "gl46_internal.h"

static const char *g_screensave_gl46_vertex_shader =
    "#version 460 compatibility\n"
    "layout(location=0) in vec2 a_pos;\n"
    "layout(location=1) in vec4 a_color;\n"
    "layout(location=2) in vec2 a_uv;\n"
    "out vec4 v_color;\n"
    "out vec2 v_uv;\n"
    "uniform vec2 u_viewport;\n"
    "void main(void)\n"
    "{\n"
    "    vec2 ndc;\n"
    "    ndc.x = (a_pos.x / u_viewport.x) * 2.0 - 1.0;\n"
    "    ndc.y = 1.0 - (a_pos.y / u_viewport.y) * 2.0;\n"
    "    gl_Position = vec4(ndc, 0.0, 1.0);\n"
    "    v_color = a_color;\n"
    "    v_uv = a_uv;\n"
    "}\n";

static const char *g_screensave_gl46_color_fragment_shader =
    "#version 460 compatibility\n"
    "in vec4 v_color;\n"
    "out vec4 f_color;\n"
    "void main(void)\n"
    "{\n"
    "    f_color = v_color;\n"
    "}\n";

static const char *g_screensave_gl46_texture_fragment_shader =
    "#version 460 compatibility\n"
    "in vec4 v_color;\n"
    "in vec2 v_uv;\n"
    "uniform sampler2D u_texture;\n"
    "out vec4 f_color;\n"
    "void main(void)\n"
    "{\n"
    "    f_color = texture(u_texture, v_uv) * v_color;\n"
    "}\n";

static int screensave_gl46_can_draw(
    screensave_renderer *renderer,
    screensave_gl46_state **state_out
)
{
    screensave_gl46_state *state;

    if (!screensave_gl46_state_from_renderer(renderer, &state)) {
        return 0;
    }

    if (!state->frame_open) {
        return 0;
    }

    if (state_out != NULL) {
        *state_out = state;
    }
    return 1;
}

static void screensave_gl46_init_vertex(
    screensave_gl46_vertex *vertex,
    GLfloat x,
    GLfloat y,
    screensave_color color,
    GLfloat u,
    GLfloat v
)
{
    if (vertex == NULL) {
        return;
    }

    vertex->x = x;
    vertex->y = y;
    vertex->red = color.red;
    vertex->green = color.green;
    vertex->blue = color.blue;
    vertex->alpha = color.alpha;
    vertex->u = u;
    vertex->v = v;
}

static int screensave_gl46_compile_shader(
    screensave_gl46_state *state,
    GLenum shader_type,
    const char *source_text,
    GLuint *shader_out
)
{
    GLuint shader;
    GLint compile_ok;
    char info_log[256];
    const char *text_array[1];

    if (shader_out != NULL) {
        *shader_out = 0U;
    }

    if (state == NULL || source_text == NULL) {
        return 0;
    }

    shader = state->create_shader_fn(shader_type);
    if (shader == 0U) {
        screensave_gl46_emit_diag(
            state,
            SCREENSAVE_DIAG_LEVEL_ERROR,
            7081UL,
            "gl46_primitives",
            "glCreateShader failed while preparing the GL46 shader pipeline."
        );
        return 0;
    }

    text_array[0] = source_text;
    state->shader_source_fn(shader, 1, text_array, NULL);
    state->compile_shader_fn(shader);
    compile_ok = 0;
    state->get_shader_iv_fn(shader, GL_COMPILE_STATUS, &compile_ok);
    if (compile_ok == 0) {
        info_log[0] = '\0';
        state->get_shader_info_log_fn(
            shader,
            (GLsizei)sizeof(info_log),
            NULL,
            info_log
        );
        screensave_gl46_emit_diag(
            state,
            SCREENSAVE_DIAG_LEVEL_ERROR,
            7082UL,
            "gl46_primitives",
            info_log[0] != '\0'
                ? info_log
                : "The GL46 shader pipeline could not compile a required shader."
        );
        state->delete_shader_fn(shader);
        return 0;
    }

    if (shader_out != NULL) {
        *shader_out = shader;
    }
    return 1;
}

static int screensave_gl46_link_program(
    screensave_gl46_state *state,
    const char *fragment_source,
    GLuint *program_out,
    GLint *viewport_location_out,
    GLint *texture_location_out
)
{
    GLuint vertex_shader;
    GLuint fragment_shader;
    GLuint program;
    GLint link_ok;
    char info_log[256];

    if (program_out != NULL) {
        *program_out = 0U;
    }
    if (viewport_location_out != NULL) {
        *viewport_location_out = -1;
    }
    if (texture_location_out != NULL) {
        *texture_location_out = -1;
    }

    if (
        state == NULL ||
        fragment_source == NULL ||
        !screensave_gl46_compile_shader(
            state,
            GL_VERTEX_SHADER,
            g_screensave_gl46_vertex_shader,
            &vertex_shader
        ) ||
        !screensave_gl46_compile_shader(
            state,
            GL_FRAGMENT_SHADER,
            fragment_source,
            &fragment_shader
        )
    ) {
        return 0;
    }

    program = state->create_program_fn();
    if (program == 0U) {
        state->delete_shader_fn(fragment_shader);
        state->delete_shader_fn(vertex_shader);
        screensave_gl46_emit_diag(
            state,
            SCREENSAVE_DIAG_LEVEL_ERROR,
            7083UL,
            "gl46_primitives",
            "glCreateProgram failed while preparing the GL46 shader pipeline."
        );
        return 0;
    }

    state->attach_shader_fn(program, vertex_shader);
    state->attach_shader_fn(program, fragment_shader);
    state->link_program_fn(program);
    link_ok = 0;
    state->get_program_iv_fn(program, GL_LINK_STATUS, &link_ok);
    if (link_ok == 0) {
        info_log[0] = '\0';
        state->get_program_info_log_fn(
            program,
            (GLsizei)sizeof(info_log),
            NULL,
            info_log
        );
        screensave_gl46_emit_diag(
            state,
            SCREENSAVE_DIAG_LEVEL_ERROR,
            7084UL,
            "gl46_primitives",
            info_log[0] != '\0'
                ? info_log
                : "The GL46 shader pipeline could not link a required program."
        );
        state->delete_program_fn(program);
        state->delete_shader_fn(fragment_shader);
        state->delete_shader_fn(vertex_shader);
        return 0;
    }

    state->delete_shader_fn(fragment_shader);
    state->delete_shader_fn(vertex_shader);

    if (viewport_location_out != NULL) {
        *viewport_location_out = state->get_uniform_location_fn(program, "u_viewport");
    }
    if (texture_location_out != NULL) {
        *texture_location_out = state->get_uniform_location_fn(program, "u_texture");
    }
    if (program_out != NULL) {
        *program_out = program;
    }
    return 1;
}

static int screensave_gl46_prepare_geometry(screensave_gl46_state *state)
{
    if (state == NULL) {
        return 0;
    }

    state->gen_vertex_arrays_fn(1, &state->vertex_array);
    state->gen_buffers_fn(1, &state->vertex_buffer);
    if (state->vertex_array == 0U || state->vertex_buffer == 0U) {
        screensave_gl46_emit_diag(
            state,
            SCREENSAVE_DIAG_LEVEL_ERROR,
            7085UL,
            "gl46_primitives",
            "The GL46 lane could not allocate its shared geometry objects."
        );
        return 0;
    }

    state->bind_vertex_array_fn(state->vertex_array);
    state->bind_buffer_fn(GL_ARRAY_BUFFER, state->vertex_buffer);
    state->buffer_data_fn(
        GL_ARRAY_BUFFER,
        (screensave_gl46_sizeiptr)(sizeof(screensave_gl46_vertex) * 12U),
        NULL,
        GL_STREAM_DRAW
    );
    state->enable_vertex_attrib_array_fn(0U);
    state->enable_vertex_attrib_array_fn(1U);
    state->enable_vertex_attrib_array_fn(2U);
    state->vertex_attrib_pointer_fn(
        0U,
        2,
        GL_FLOAT,
        GL_FALSE,
        (GLsizei)sizeof(screensave_gl46_vertex),
        (const GLvoid *)offsetof(screensave_gl46_vertex, x)
    );
    state->vertex_attrib_pointer_fn(
        1U,
        4,
        GL_UNSIGNED_BYTE,
        GL_TRUE,
        (GLsizei)sizeof(screensave_gl46_vertex),
        (const GLvoid *)offsetof(screensave_gl46_vertex, red)
    );
    state->vertex_attrib_pointer_fn(
        2U,
        2,
        GL_FLOAT,
        GL_FALSE,
        (GLsizei)sizeof(screensave_gl46_vertex),
        (const GLvoid *)offsetof(screensave_gl46_vertex, u)
    );
    state->bind_buffer_fn(GL_ARRAY_BUFFER, 0U);
    state->bind_vertex_array_fn(0U);
    return 1;
}

static int screensave_gl46_prepare_sampler(screensave_gl46_state *state)
{
    if (state == NULL) {
        return 0;
    }

    state->gen_samplers_fn(1, &state->texture_sampler);
    if (state->texture_sampler == 0U) {
        screensave_gl46_emit_diag(
            state,
            SCREENSAVE_DIAG_LEVEL_ERROR,
            7086UL,
            "gl46_primitives",
            "The GL46 lane could not allocate its shared sampler object."
        );
        return 0;
    }

    state->sampler_parameter_i_fn(state->texture_sampler, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    state->sampler_parameter_i_fn(state->texture_sampler, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    state->sampler_parameter_i_fn(state->texture_sampler, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    state->sampler_parameter_i_fn(state->texture_sampler, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    return 1;
}

int screensave_gl46_pipeline_create(
    screensave_gl46_state *state,
    const char **failure_reason_out
)
{
    if (failure_reason_out != NULL) {
        *failure_reason_out = NULL;
    }

    if (state == NULL) {
        if (failure_reason_out != NULL) {
            *failure_reason_out = "gl46-pipeline-create-failed";
        }
        return 0;
    }

    if (
        !screensave_gl46_link_program(
            state,
            g_screensave_gl46_color_fragment_shader,
            &state->color_program,
            &state->color_viewport_location,
            NULL
        ) ||
        !screensave_gl46_link_program(
            state,
            g_screensave_gl46_texture_fragment_shader,
            &state->texture_program,
            &state->texture_viewport_location,
            &state->texture_sampler_location
        ) ||
        !screensave_gl46_prepare_geometry(state) ||
        !screensave_gl46_prepare_sampler(state)
    ) {
        if (failure_reason_out != NULL) {
            *failure_reason_out = "gl46-pipeline-create-failed";
        }
        screensave_gl46_pipeline_destroy(state);
        return 0;
    }

    screensave_gl46_emit_diag(
        state,
        SCREENSAVE_DIAG_LEVEL_INFO,
        7087UL,
        "gl46_primitives",
        "The GL46 premium lane prepared its shared shader pipeline successfully."
    );
    return 1;
}

void screensave_gl46_pipeline_destroy(screensave_gl46_state *state)
{
    if (state == NULL) {
        return;
    }

    if (state->bind_sampler_fn != NULL) {
        state->bind_sampler_fn(0U, 0U);
    }
    if (state->bind_vertex_array_fn != NULL) {
        state->bind_vertex_array_fn(0U);
    }
    if (state->bind_buffer_fn != NULL) {
        state->bind_buffer_fn(GL_ARRAY_BUFFER, 0U);
    }
    if (state->use_program_fn != NULL) {
        state->use_program_fn(0U);
    }
    if (state->delete_samplers_fn != NULL && state->texture_sampler != 0U) {
        state->delete_samplers_fn(1, &state->texture_sampler);
        state->texture_sampler = 0U;
    }
    if (state->delete_buffers_fn != NULL && state->vertex_buffer != 0U) {
        state->delete_buffers_fn(1, &state->vertex_buffer);
        state->vertex_buffer = 0U;
    }
    if (state->delete_vertex_arrays_fn != NULL && state->vertex_array != 0U) {
        state->delete_vertex_arrays_fn(1, &state->vertex_array);
        state->vertex_array = 0U;
    }
    if (state->delete_program_fn != NULL && state->color_program != 0U) {
        state->delete_program_fn(state->color_program);
        state->color_program = 0U;
    }
    if (state->delete_program_fn != NULL && state->texture_program != 0U) {
        state->delete_program_fn(state->texture_program);
        state->texture_program = 0U;
    }

    state->color_viewport_location = -1;
    state->texture_viewport_location = -1;
    state->texture_sampler_location = -1;
}

static int screensave_gl46_draw_vertices(
    screensave_gl46_state *state,
    GLuint program,
    GLint viewport_location,
    GLuint texture_id,
    const screensave_gl46_vertex *vertices,
    unsigned int vertex_count,
    GLenum mode
)
{
    if (
        state == NULL ||
        program == 0U ||
        vertices == NULL ||
        vertex_count == 0U
    ) {
        return 0;
    }

    state->use_program_fn(program);
    if (viewport_location >= 0) {
        state->uniform_2f_fn(
            viewport_location,
            (GLfloat)state->drawable_size.width,
            (GLfloat)state->drawable_size.height
        );
    }
    if (texture_id != 0U) {
        state->active_texture_fn(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture_id);
        state->bind_sampler_fn(0U, state->texture_sampler);
        if (state->texture_sampler_location >= 0) {
            state->uniform_1i_fn(state->texture_sampler_location, 0);
        }
    }

    state->bind_vertex_array_fn(state->vertex_array);
    state->bind_buffer_fn(GL_ARRAY_BUFFER, state->vertex_buffer);
    state->buffer_data_fn(
        GL_ARRAY_BUFFER,
        (screensave_gl46_sizeiptr)(sizeof(screensave_gl46_vertex) * vertex_count),
        vertices,
        GL_STREAM_DRAW
    );
    state->draw_arrays_fn(mode, 0, (GLsizei)vertex_count);
    state->bind_buffer_fn(GL_ARRAY_BUFFER, 0U);
    state->bind_vertex_array_fn(0U);
    if (texture_id != 0U) {
        state->bind_sampler_fn(0U, 0U);
        glBindTexture(GL_TEXTURE_2D, 0U);
    }
    state->use_program_fn(0U);
    return 1;
}

static unsigned int screensave_gl46_bitmap_bytes_per_pixel(unsigned int bits_per_pixel)
{
    switch (bits_per_pixel) {
    case 24U:
        return 3U;
    case 32U:
        return 4U;
    default:
        return 0U;
    }
}

static void screensave_gl46_convert_bitmap_to_rgba(
    const screensave_bitmap_view *bitmap,
    unsigned char *pixels
)
{
    const unsigned char *source_bytes;
    unsigned int source_bytes_per_pixel;
    unsigned int y;
    unsigned int x;

    if (bitmap == NULL || pixels == NULL) {
        return;
    }

    source_bytes = (const unsigned char *)bitmap->pixels;
    source_bytes_per_pixel = screensave_gl46_bitmap_bytes_per_pixel(bitmap->bits_per_pixel);
    for (y = 0U; y < (unsigned int)bitmap->size.height; ++y) {
        const unsigned char *source_row;
        unsigned char *target_row;
        unsigned int source_y;

        source_y = bitmap->origin_top_left
            ? (unsigned int)(bitmap->size.height - 1) - y
            : y;
        source_row = source_bytes + ((size_t)source_y * (size_t)bitmap->stride_bytes);
        target_row = pixels + ((size_t)y * (size_t)bitmap->size.width * 4U);

        for (x = 0U; x < (unsigned int)bitmap->size.width; ++x) {
            const unsigned char *source_pixel;
            unsigned char *target_pixel;

            source_pixel = source_row + ((size_t)x * (size_t)source_bytes_per_pixel);
            target_pixel = target_row + ((size_t)x * 4U);
            target_pixel[0] = source_pixel[2];
            target_pixel[1] = source_pixel[1];
            target_pixel[2] = source_pixel[0];
            target_pixel[3] = source_bytes_per_pixel == 4U ? source_pixel[3] : 255U;
        }
    }
}

void screensave_gl46_clear_impl(screensave_renderer *renderer, screensave_color color)
{
    screensave_gl46_state *state;

    if (!screensave_gl46_can_draw(renderer, &state)) {
        return;
    }

    glClearColor(
        (GLclampf)color.red / 255.0f,
        (GLclampf)color.green / 255.0f,
        (GLclampf)color.blue / 255.0f,
        (GLclampf)color.alpha / 255.0f
    );
    glClear(GL_COLOR_BUFFER_BIT);
}

void screensave_gl46_fill_rect_impl(
    screensave_renderer *renderer,
    const screensave_recti *rect,
    screensave_color color
)
{
    screensave_gl46_state *state;
    screensave_gl46_vertex vertices[6];
    GLfloat left;
    GLfloat top;
    GLfloat right;
    GLfloat bottom;

    if (!screensave_gl46_can_draw(renderer, &state) || rect == NULL) {
        return;
    }

    left = (GLfloat)rect->x;
    top = (GLfloat)rect->y;
    right = (GLfloat)(rect->x + rect->width);
    bottom = (GLfloat)(rect->y + rect->height);
    screensave_gl46_init_vertex(&vertices[0], left, top, color, 0.0f, 0.0f);
    screensave_gl46_init_vertex(&vertices[1], right, top, color, 0.0f, 0.0f);
    screensave_gl46_init_vertex(&vertices[2], right, bottom, color, 0.0f, 0.0f);
    screensave_gl46_init_vertex(&vertices[3], left, top, color, 0.0f, 0.0f);
    screensave_gl46_init_vertex(&vertices[4], right, bottom, color, 0.0f, 0.0f);
    screensave_gl46_init_vertex(&vertices[5], left, bottom, color, 0.0f, 0.0f);
    (void)screensave_gl46_draw_vertices(
        state,
        state->color_program,
        state->color_viewport_location,
        0U,
        vertices,
        6U,
        GL_TRIANGLES
    );
}

void screensave_gl46_draw_frame_rect_impl(
    screensave_renderer *renderer,
    const screensave_recti *rect,
    screensave_color color
)
{
    screensave_gl46_state *state;
    screensave_gl46_vertex vertices[4];
    GLfloat left;
    GLfloat top;
    GLfloat right;
    GLfloat bottom;

    if (!screensave_gl46_can_draw(renderer, &state) || rect == NULL) {
        return;
    }

    left = (GLfloat)rect->x;
    top = (GLfloat)rect->y;
    right = (GLfloat)(rect->x + rect->width);
    bottom = (GLfloat)(rect->y + rect->height);
    screensave_gl46_init_vertex(&vertices[0], left, top, color, 0.0f, 0.0f);
    screensave_gl46_init_vertex(&vertices[1], right, top, color, 0.0f, 0.0f);
    screensave_gl46_init_vertex(&vertices[2], right, bottom, color, 0.0f, 0.0f);
    screensave_gl46_init_vertex(&vertices[3], left, bottom, color, 0.0f, 0.0f);
    (void)screensave_gl46_draw_vertices(
        state,
        state->color_program,
        state->color_viewport_location,
        0U,
        vertices,
        4U,
        GL_LINE_LOOP
    );
}

void screensave_gl46_draw_line_impl(
    screensave_renderer *renderer,
    const screensave_pointi *start_point,
    const screensave_pointi *end_point,
    screensave_color color
)
{
    screensave_gl46_state *state;
    screensave_gl46_vertex vertices[2];

    if (
        !screensave_gl46_can_draw(renderer, &state) ||
        start_point == NULL ||
        end_point == NULL
    ) {
        return;
    }

    screensave_gl46_init_vertex(
        &vertices[0],
        (GLfloat)start_point->x,
        (GLfloat)start_point->y,
        color,
        0.0f,
        0.0f
    );
    screensave_gl46_init_vertex(
        &vertices[1],
        (GLfloat)end_point->x,
        (GLfloat)end_point->y,
        color,
        0.0f,
        0.0f
    );
    (void)screensave_gl46_draw_vertices(
        state,
        state->color_program,
        state->color_viewport_location,
        0U,
        vertices,
        2U,
        GL_LINES
    );
}

void screensave_gl46_draw_polyline_impl(
    screensave_renderer *renderer,
    const screensave_pointi *points,
    unsigned int point_count,
    screensave_color color
)
{
    screensave_gl46_state *state;
    screensave_gl46_vertex vertices[32];
    unsigned int index;

    if (
        !screensave_gl46_can_draw(renderer, &state) ||
        points == NULL ||
        point_count < 2U
    ) {
        return;
    }

    if (point_count > 32U) {
        point_count = 32U;
    }

    for (index = 0U; index < point_count; ++index) {
        screensave_gl46_init_vertex(
            &vertices[index],
            (GLfloat)points[index].x,
            (GLfloat)points[index].y,
            color,
            0.0f,
            0.0f
        );
    }

    (void)screensave_gl46_draw_vertices(
        state,
        state->color_program,
        state->color_viewport_location,
        0U,
        vertices,
        point_count,
        GL_LINE_STRIP
    );
}

int screensave_gl46_blit_bitmap_impl(
    screensave_renderer *renderer,
    const screensave_bitmap_view *bitmap,
    const screensave_recti *destination_rect
)
{
    screensave_gl46_state *state;
    GLuint texture_id;
    unsigned char *pixels;
    unsigned int source_bytes_per_pixel;
    GLfloat left;
    GLfloat top;
    GLfloat right;
    GLfloat bottom;
    screensave_gl46_vertex vertices[6];
    screensave_color white;

    if (!screensave_gl46_can_draw(renderer, &state) || bitmap == NULL) {
        return 0;
    }

    source_bytes_per_pixel = screensave_gl46_bitmap_bytes_per_pixel(bitmap->bits_per_pixel);
    if (
        bitmap->pixels == NULL ||
        bitmap->size.width <= 0 ||
        bitmap->size.height <= 0 ||
        source_bytes_per_pixel == 0U
    ) {
        return 0;
    }

    pixels = (unsigned char *)malloc(
        (size_t)bitmap->size.width * (size_t)bitmap->size.height * 4U
    );
    if (pixels == NULL) {
        screensave_gl46_emit_diag(
            state,
            SCREENSAVE_DIAG_LEVEL_WARNING,
            7088UL,
            "gl46_primitives",
            "GL46 bitmap upload memory allocation failed."
        );
        return 0;
    }

    screensave_gl46_convert_bitmap_to_rgba(bitmap, pixels);
    glGenTextures(1, &texture_id);
    if (texture_id == 0U) {
        free(pixels);
        screensave_gl46_emit_diag(
            state,
            SCREENSAVE_DIAG_LEVEL_WARNING,
            7089UL,
            "gl46_primitives",
            "glGenTextures failed for a GL46 bitmap blit."
        );
        return 0;
    }

    glBindTexture(GL_TEXTURE_2D, texture_id);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    state->tex_storage_2d_fn(
        GL_TEXTURE_2D,
        1,
        GL_RGBA8,
        (GLsizei)bitmap->size.width,
        (GLsizei)bitmap->size.height
    );
    glTexSubImage2D(
        GL_TEXTURE_2D,
        0,
        0,
        0,
        (GLsizei)bitmap->size.width,
        (GLsizei)bitmap->size.height,
        GL_RGBA,
        GL_UNSIGNED_BYTE,
        pixels
    );
    free(pixels);

    if (destination_rect != NULL) {
        left = (GLfloat)destination_rect->x;
        top = (GLfloat)destination_rect->y;
        right = (GLfloat)(destination_rect->x + destination_rect->width);
        bottom = (GLfloat)(destination_rect->y + destination_rect->height);
    } else {
        left = 0.0f;
        top = 0.0f;
        right = (GLfloat)bitmap->size.width;
        bottom = (GLfloat)bitmap->size.height;
    }

    white.red = 255U;
    white.green = 255U;
    white.blue = 255U;
    white.alpha = 255U;
    screensave_gl46_init_vertex(&vertices[0], left, top, white, 0.0f, 1.0f);
    screensave_gl46_init_vertex(&vertices[1], right, top, white, 1.0f, 1.0f);
    screensave_gl46_init_vertex(&vertices[2], right, bottom, white, 1.0f, 0.0f);
    screensave_gl46_init_vertex(&vertices[3], left, top, white, 0.0f, 1.0f);
    screensave_gl46_init_vertex(&vertices[4], right, bottom, white, 1.0f, 0.0f);
    screensave_gl46_init_vertex(&vertices[5], left, bottom, white, 0.0f, 0.0f);
    (void)screensave_gl46_draw_vertices(
        state,
        state->texture_program,
        state->texture_viewport_location,
        texture_id,
        vertices,
        6U,
        GL_TRIANGLES
    );

    glBindTexture(GL_TEXTURE_2D, 0U);
    glDeleteTextures(1, &texture_id);
    return 1;
}
