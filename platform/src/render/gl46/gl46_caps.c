#include "gl46_internal.h"

static int screensave_gl46_parse_version_string(
    const char *version_text,
    int *major_out,
    int *minor_out
)
{
    unsigned int index;
    int minor;

    if (major_out != NULL) {
        *major_out = 0;
    }
    if (minor_out != NULL) {
        *minor_out = 0;
    }

    if (version_text == NULL) {
        return 0;
    }

    index = 0U;
    while (version_text[index] >= '0' && version_text[index] <= '9') {
        if (major_out != NULL) {
            *major_out = (*major_out * 10) + (version_text[index] - '0');
        }
        ++index;
    }
    if (version_text[index] != '.') {
        return 0;
    }

    ++index;
    minor = 0;
    while (version_text[index] >= '0' && version_text[index] <= '9') {
        minor = (minor * 10) + (version_text[index] - '0');
        ++index;
    }

    if (major_out != NULL && *major_out <= 0) {
        return 0;
    }
    if (minor_out != NULL) {
        *minor_out = minor;
    }
    return 1;
}

static PROC screensave_gl46_get_proc_address(const char *name)
{
    PROC proc;

    proc = wglGetProcAddress(name);
    if (
        proc == NULL ||
        proc == (PROC)0x1 ||
        proc == (PROC)0x2 ||
        proc == (PROC)0x3 ||
        proc == (PROC)-1
    ) {
        return NULL;
    }

    return proc;
}

static void screensave_gl46_init_capability_bundle(screensave_gl46_capability_bundle *bundle)
{
    if (bundle == NULL) {
        return;
    }

    ZeroMemory(bundle, sizeof(*bundle));
    bundle->required_flags =
        SCREENSAVE_GL46_PRIVATE_CAP_CONTEXT_ATTRIBS |
        SCREENSAVE_GL46_PRIVATE_CAP_PREMIUM_CONTEXT |
        SCREENSAVE_GL46_PRIVATE_CAP_COMPAT_PROFILE |
        SCREENSAVE_GL46_PRIVATE_CAP_CONTEXT_46_PLUS |
        SCREENSAVE_GL46_PRIVATE_CAP_VBO |
        SCREENSAVE_GL46_PRIVATE_CAP_VAO |
        SCREENSAVE_GL46_PRIVATE_CAP_SHADER_PIPELINE |
        SCREENSAVE_GL46_PRIVATE_CAP_TEXTURE_STORAGE |
        SCREENSAVE_GL46_PRIVATE_CAP_SAMPLER_OBJECTS;
    bundle->preferred_flags =
        SCREENSAVE_GL46_PRIVATE_CAP_FBO |
        SCREENSAVE_GL46_PRIVATE_CAP_BUFFER_STORAGE;
}

static void screensave_gl46_finalize_capability_bundle(screensave_gl46_state *state)
{
    screensave_gl46_capability_bundle *bundle;

    if (state == NULL) {
        return;
    }

    bundle = &state->caps.bundle;
    bundle->available_flags = state->caps.private_flags;
    bundle->missing_required_flags =
        bundle->required_flags & ~bundle->available_flags;
    bundle->satisfied = bundle->missing_required_flags == 0UL;
}

static void screensave_gl46_load_function_table(screensave_gl46_state *state)
{
    if (state == NULL) {
        return;
    }

    state->active_texture_fn =
        (PFNGLACTIVETEXTUREPROC)screensave_gl46_get_proc_address("glActiveTexture");
    state->gen_buffers_fn =
        (PFNGLGENBUFFERSPROC)screensave_gl46_get_proc_address("glGenBuffers");
    state->bind_buffer_fn =
        (PFNGLBINDBUFFERPROC)screensave_gl46_get_proc_address("glBindBuffer");
    state->buffer_data_fn =
        (PFNGLBUFFERDATAPROC)screensave_gl46_get_proc_address("glBufferData");
    state->delete_buffers_fn =
        (PFNGLDELETEBUFFERSPROC)screensave_gl46_get_proc_address("glDeleteBuffers");
    state->gen_vertex_arrays_fn =
        (PFNGLGENVERTEXARRAYSPROC)screensave_gl46_get_proc_address("glGenVertexArrays");
    state->bind_vertex_array_fn =
        (PFNGLBINDVERTEXARRAYPROC)screensave_gl46_get_proc_address("glBindVertexArray");
    state->delete_vertex_arrays_fn =
        (PFNGLDELETEVERTEXARRAYSPROC)screensave_gl46_get_proc_address("glDeleteVertexArrays");
    state->create_shader_fn =
        (PFNGLCREATESHADERPROC)screensave_gl46_get_proc_address("glCreateShader");
    state->shader_source_fn =
        (PFNGLSHADERSOURCEPROC)screensave_gl46_get_proc_address("glShaderSource");
    state->compile_shader_fn =
        (PFNGLCOMPILESHADERPROC)screensave_gl46_get_proc_address("glCompileShader");
    state->get_shader_iv_fn =
        (PFNGLGETSHADERIVPROC)screensave_gl46_get_proc_address("glGetShaderiv");
    state->get_shader_info_log_fn =
        (PFNGLGETSHADERINFOLOGPROC)screensave_gl46_get_proc_address("glGetShaderInfoLog");
    state->delete_shader_fn =
        (PFNGLDELETESHADERPROC)screensave_gl46_get_proc_address("glDeleteShader");
    state->create_program_fn =
        (PFNGLCREATEPROGRAMPROC)screensave_gl46_get_proc_address("glCreateProgram");
    state->attach_shader_fn =
        (PFNGLATTACHSHADERPROC)screensave_gl46_get_proc_address("glAttachShader");
    state->link_program_fn =
        (PFNGLLINKPROGRAMPROC)screensave_gl46_get_proc_address("glLinkProgram");
    state->get_program_iv_fn =
        (PFNGLGETPROGRAMIVPROC)screensave_gl46_get_proc_address("glGetProgramiv");
    state->get_program_info_log_fn =
        (PFNGLGETPROGRAMINFOLOGPROC)screensave_gl46_get_proc_address("glGetProgramInfoLog");
    state->use_program_fn =
        (PFNGLUSEPROGRAMPROC)screensave_gl46_get_proc_address("glUseProgram");
    state->delete_program_fn =
        (PFNGLDELETEPROGRAMPROC)screensave_gl46_get_proc_address("glDeleteProgram");
    state->get_uniform_location_fn =
        (PFNGLGETUNIFORMLOCATIONPROC)screensave_gl46_get_proc_address("glGetUniformLocation");
    state->uniform_2f_fn =
        (PFNGLUNIFORM2FPROC)screensave_gl46_get_proc_address("glUniform2f");
    state->uniform_1i_fn =
        (PFNGLUNIFORM1IPROC)screensave_gl46_get_proc_address("glUniform1i");
    state->enable_vertex_attrib_array_fn =
        (PFNGLENABLEVERTEXATTRIBARRAYPROC)screensave_gl46_get_proc_address("glEnableVertexAttribArray");
    state->vertex_attrib_pointer_fn =
        (PFNGLVERTEXATTRIBPOINTERPROC)screensave_gl46_get_proc_address("glVertexAttribPointer");
    state->draw_arrays_fn =
        (PFNGLDRAWARRAYSPROC)screensave_gl46_get_proc_address("glDrawArrays");
    state->gen_framebuffers_fn =
        (PFNGLGENFRAMEBUFFERSPROC)screensave_gl46_get_proc_address("glGenFramebuffers");
    state->bind_framebuffer_fn =
        (PFNGLBINDFRAMEBUFFERPROC)screensave_gl46_get_proc_address("glBindFramebuffer");
    state->delete_framebuffers_fn =
        (PFNGLDELETEFRAMEBUFFERSPROC)screensave_gl46_get_proc_address("glDeleteFramebuffers");
    state->check_framebuffer_status_fn =
        (PFNGLCHECKFRAMEBUFFERSTATUSPROC)screensave_gl46_get_proc_address("glCheckFramebufferStatus");
    state->tex_storage_2d_fn =
        (PFNGLTEXSTORAGE2DPROC)screensave_gl46_get_proc_address("glTexStorage2D");
    state->gen_samplers_fn =
        (PFNGLGENSAMPLERSPROC)screensave_gl46_get_proc_address("glGenSamplers");
    state->bind_sampler_fn =
        (PFNGLBINDSAMPLERPROC)screensave_gl46_get_proc_address("glBindSampler");
    state->sampler_parameter_i_fn =
        (PFNGLSAMPLERPARAMETERIPROC)screensave_gl46_get_proc_address("glSamplerParameteri");
    state->delete_samplers_fn =
        (PFNGLDELETESAMPLERSPROC)screensave_gl46_get_proc_address("glDeleteSamplers");
    state->buffer_storage_fn =
        (PFNGLBUFFERSTORAGEPROC)screensave_gl46_get_proc_address("glBufferStorage");
}

int screensave_gl46_capture_caps(
    screensave_gl46_state *state,
    const char **failure_reason_out
)
{
    const GLubyte *vendor;
    const GLubyte *renderer;
    const GLubyte *version;
    int double_buffered;
    int support_gdi;
    int generic_format;
    int rgba_bits;
    int depth_bits;
    int major_version;
    int minor_version;

    if (failure_reason_out != NULL) {
        *failure_reason_out = NULL;
    }

    if (state == NULL) {
        if (failure_reason_out != NULL) {
            *failure_reason_out = "gl46-capture-invalid-state";
        }
        return 0;
    }

    double_buffered = state->caps.double_buffered;
    support_gdi = state->caps.support_gdi;
    generic_format = state->caps.generic_format;
    rgba_bits = state->caps.rgba_bits;
    depth_bits = state->caps.depth_bits;

    vendor = glGetString(GL_VENDOR);
    renderer = glGetString(GL_RENDERER);
    version = glGetString(GL_VERSION);
    if (vendor == NULL || renderer == NULL || version == NULL) {
        if (failure_reason_out != NULL) {
            *failure_reason_out = "gl46-capture-string-failed";
        }
        screensave_gl46_emit_diag(
            state,
            SCREENSAVE_DIAG_LEVEL_ERROR,
            7051UL,
            "gl46_caps",
            "The GL46 premium lane could not query core renderer strings."
        );
        return 0;
    }

    ZeroMemory(&state->caps, sizeof(state->caps));
    state->caps.double_buffered = double_buffered;
    state->caps.support_gdi = support_gdi;
    state->caps.generic_format = generic_format;
    state->caps.rgba_bits = rgba_bits;
    state->caps.depth_bits = depth_bits;
    state->caps.premium_context = 1;
    state->caps.compatibility_profile = 1;
    screensave_gl46_init_capability_bundle(&state->caps.bundle);
    state->caps.private_flags =
        SCREENSAVE_GL46_PRIVATE_CAP_PREMIUM_CONTEXT |
        SCREENSAVE_GL46_PRIVATE_CAP_COMPAT_PROFILE;
    if (state->create_context_attribs != NULL) {
        state->caps.private_flags |= SCREENSAVE_GL46_PRIVATE_CAP_CONTEXT_ATTRIBS;
    }
    lstrcpynA(state->caps.vendor, (const char *)vendor, sizeof(state->caps.vendor));
    lstrcpynA(state->caps.renderer, (const char *)renderer, sizeof(state->caps.renderer));
    lstrcpynA(state->caps.version, (const char *)version, sizeof(state->caps.version));

    major_version = 0;
    minor_version = 0;
    if (
        !screensave_gl46_parse_version_string(
            state->caps.version,
            &major_version,
            &minor_version
        )
    ) {
        if (failure_reason_out != NULL) {
            *failure_reason_out = "gl46-capture-string-failed";
        }
        screensave_gl46_emit_diag(
            state,
            SCREENSAVE_DIAG_LEVEL_ERROR,
            7052UL,
            "gl46_caps",
            "The GL46 premium lane could not parse the renderer version string."
        );
        return 0;
    }

    state->caps.major_version = major_version;
    state->caps.minor_version = minor_version;
    if (major_version > 4 || (major_version == 4 && minor_version >= 6)) {
        state->caps.private_flags |= SCREENSAVE_GL46_PRIVATE_CAP_CONTEXT_46_PLUS;
    }

    screensave_gl46_load_function_table(state);
    if (
        state->gen_buffers_fn != NULL &&
        state->bind_buffer_fn != NULL &&
        state->buffer_data_fn != NULL &&
        state->delete_buffers_fn != NULL
    ) {
        state->caps.has_vbo = 1;
        state->caps.private_flags |= SCREENSAVE_GL46_PRIVATE_CAP_VBO;
    }
    if (
        state->gen_vertex_arrays_fn != NULL &&
        state->bind_vertex_array_fn != NULL &&
        state->delete_vertex_arrays_fn != NULL &&
        state->enable_vertex_attrib_array_fn != NULL &&
        state->vertex_attrib_pointer_fn != NULL &&
        state->draw_arrays_fn != NULL
    ) {
        state->caps.has_vao = 1;
        state->caps.private_flags |= SCREENSAVE_GL46_PRIVATE_CAP_VAO;
    }
    if (
        state->create_shader_fn != NULL &&
        state->shader_source_fn != NULL &&
        state->compile_shader_fn != NULL &&
        state->get_shader_iv_fn != NULL &&
        state->get_shader_info_log_fn != NULL &&
        state->delete_shader_fn != NULL &&
        state->create_program_fn != NULL &&
        state->attach_shader_fn != NULL &&
        state->link_program_fn != NULL &&
        state->get_program_iv_fn != NULL &&
        state->get_program_info_log_fn != NULL &&
        state->use_program_fn != NULL &&
        state->delete_program_fn != NULL &&
        state->get_uniform_location_fn != NULL &&
        state->uniform_2f_fn != NULL &&
        state->uniform_1i_fn != NULL &&
        state->active_texture_fn != NULL
    ) {
        state->caps.has_shader_pipeline = 1;
        state->caps.private_flags |= SCREENSAVE_GL46_PRIVATE_CAP_SHADER_PIPELINE;
    }
    if (state->tex_storage_2d_fn != NULL) {
        state->caps.has_texture_storage = 1;
        state->caps.private_flags |= SCREENSAVE_GL46_PRIVATE_CAP_TEXTURE_STORAGE;
    }
    if (
        state->gen_samplers_fn != NULL &&
        state->bind_sampler_fn != NULL &&
        state->sampler_parameter_i_fn != NULL &&
        state->delete_samplers_fn != NULL
    ) {
        state->caps.has_sampler_objects = 1;
        state->caps.private_flags |= SCREENSAVE_GL46_PRIVATE_CAP_SAMPLER_OBJECTS;
    }
    if (
        state->gen_framebuffers_fn != NULL &&
        state->bind_framebuffer_fn != NULL &&
        state->delete_framebuffers_fn != NULL &&
        state->check_framebuffer_status_fn != NULL
    ) {
        state->caps.has_fbo = 1;
        state->caps.private_flags |= SCREENSAVE_GL46_PRIVATE_CAP_FBO;
    }
    if (state->buffer_storage_fn != NULL) {
        state->caps.has_buffer_storage = 1;
        state->caps.private_flags |= SCREENSAVE_GL46_PRIVATE_CAP_BUFFER_STORAGE;
    }

    screensave_gl46_finalize_capability_bundle(state);
    if (!state->caps.bundle.satisfied) {
        if ((state->caps.bundle.missing_required_flags & SCREENSAVE_GL46_PRIVATE_CAP_CONTEXT_46_PLUS) != 0UL) {
            if (failure_reason_out != NULL) {
                *failure_reason_out = "gl46-version-too-old";
            }
        } else if (
            (state->caps.bundle.missing_required_flags &
                (SCREENSAVE_GL46_PRIVATE_CAP_CONTEXT_ATTRIBS |
                 SCREENSAVE_GL46_PRIVATE_CAP_PREMIUM_CONTEXT |
                 SCREENSAVE_GL46_PRIVATE_CAP_COMPAT_PROFILE)) != 0UL
        ) {
            if (failure_reason_out != NULL) {
                *failure_reason_out = "gl46-premium-context-incomplete";
            }
        } else if (failure_reason_out != NULL) {
            *failure_reason_out = "gl46-required-functions-missing";
        }

        screensave_gl46_emit_diag(
            state,
            SCREENSAVE_DIAG_LEVEL_WARNING,
            7053UL,
            "gl46_caps",
            "The GL46 premium-lane capability bundle is incomplete and must degrade to a lower tier."
        );
        return 0;
    }

    if (major_version > 4 || (major_version == 4 && minor_version > 6)) {
        screensave_gl46_emit_diag(
            state,
            SCREENSAVE_DIAG_LEVEL_INFO,
            7054UL,
            "gl46_caps",
            "The GL46 lane is using a higher compatibility context while preserving the bounded premium-tier contract."
        );
    }
    if (!state->caps.has_buffer_storage) {
        screensave_gl46_emit_diag(
            state,
            SCREENSAVE_DIAG_LEVEL_INFO,
            7055UL,
            "gl46_caps",
            "The GL46 lane is active without its preferred buffer-storage bundle; later premium work must keep explicit degrade paths."
        );
    }
    if (!state->caps.has_fbo) {
        screensave_gl46_emit_diag(
            state,
            SCREENSAVE_DIAG_LEVEL_INFO,
            7056UL,
            "gl46_caps",
            "The GL46 lane is active without its preferred framebuffer-object bundle."
        );
    }

    screensave_gl46_capture_refresh(state);
    screensave_gl46_emit_diag(
        state,
        SCREENSAVE_DIAG_LEVEL_INFO,
        7057UL,
        "gl46_caps",
        "The GL46 lane captured premium-capability details successfully."
    );
    return 1;
}
