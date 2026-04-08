#include "gl11_internal.h"

static void screensave_gl11_copy_text(char *buffer, int buffer_size, const GLubyte *text)
{
    int index;

    if (buffer == NULL || buffer_size <= 0) {
        return;
    }

    index = 0;
    if (text != NULL) {
        while (*text != 0 && index < buffer_size - 1) {
            buffer[index] = (char)*text;
            ++index;
            ++text;
        }
    }

    buffer[index] = '\0';
}

static void screensave_gl11_parse_version(
    const char *version_text,
    int *major_version_out,
    int *minor_version_out
)
{
    int major_version;
    int minor_version;

    major_version = 0;
    minor_version = 0;

    if (version_text != NULL) {
        while (*version_text != '\0' && (*version_text < '0' || *version_text > '9')) {
            ++version_text;
        }

        while (*version_text >= '0' && *version_text <= '9') {
            major_version = (major_version * 10) + (int)(*version_text - '0');
            ++version_text;
        }

        if (*version_text == '.') {
            ++version_text;
            while (*version_text >= '0' && *version_text <= '9') {
                minor_version = (minor_version * 10) + (int)(*version_text - '0');
                ++version_text;
            }
        }
    }

    if (major_version_out != NULL) {
        *major_version_out = major_version;
    }
    if (minor_version_out != NULL) {
        *minor_version_out = minor_version;
    }
}

int screensave_gl11_capture_caps(
    screensave_gl11_state *state,
    const char **failure_reason_out
)
{
    const GLubyte *vendor_text;
    const GLubyte *renderer_text;
    const GLubyte *version_text;

    if (failure_reason_out != NULL) {
        *failure_reason_out = NULL;
    }

    if (state == NULL) {
        if (failure_reason_out != NULL) {
            *failure_reason_out = "gl11-no-state";
        }
        return 0;
    }

    vendor_text = glGetString(GL_VENDOR);
    renderer_text = glGetString(GL_RENDERER);
    version_text = glGetString(GL_VERSION);
    if (version_text == NULL) {
        if (failure_reason_out != NULL) {
            *failure_reason_out = "gl11-version-unavailable";
        }
        screensave_gl11_emit_diag(
            state,
            SCREENSAVE_DIAG_LEVEL_ERROR,
            6101UL,
            "gl11_caps",
            "glGetString(GL_VERSION) returned no value."
        );
        return 0;
    }

    screensave_gl11_copy_text(state->caps.vendor, (int)sizeof(state->caps.vendor), vendor_text);
    screensave_gl11_copy_text(state->caps.renderer, (int)sizeof(state->caps.renderer), renderer_text);
    screensave_gl11_copy_text(state->caps.version, (int)sizeof(state->caps.version), version_text);
    screensave_gl11_parse_version(
        state->caps.version,
        &state->caps.major_version,
        &state->caps.minor_version
    );

    if (state->caps.vendor[0] == '\0') {
        lstrcpyA(state->caps.vendor, "unknown");
    }
    if (state->caps.renderer[0] == '\0') {
        lstrcpyA(state->caps.renderer, "unknown");
    }

    screensave_gl11_capture_refresh(state);
    screensave_gl11_emit_diag(
        state,
        SCREENSAVE_DIAG_LEVEL_INFO,
        6102UL,
        "gl11_caps",
        "OpenGL 1.1 identity strings were captured."
    );
    return 1;
}
