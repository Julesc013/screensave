#include <string.h>

#include "display_text.h"

typedef struct screensave_display_mapping_tag {
    const char *key;
    const char *display_text;
} screensave_display_mapping;

static const screensave_display_mapping g_renderer_reason_mappings[] = {
    { "gdi-create-failed", "GDI initialization failed" },
    { "renderer-kind-invalid", "Requested renderer is invalid" },
    { "null-out-of-memory", "Internal null backend ran out of memory" },
    { "gl11-out-of-memory", "OpenGL 1.1 backend ran out of memory" },
    { "gl11-make-current-failed", "OpenGL 1.1 MakeCurrent failed" },
    { "gl11-choose-pixel-format-failed", "OpenGL 1.1 pixel-format selection failed" },
    { "gl11-set-pixel-format-failed", "OpenGL 1.1 SetPixelFormat failed" },
    { "gl11-describe-pixel-format-failed", "OpenGL 1.1 pixel-format inspection failed" },
    { "gl11-pixel-format-without-opengl", "OpenGL 1.1 pixel format does not expose OpenGL" },
    { "gl11-invalid-window", "OpenGL 1.1 target window is invalid" },
    { "gl11-getdc-failed", "OpenGL 1.1 GetDC failed" },
    { "gl11-create-context-failed", "OpenGL 1.1 context creation failed" },
    { "gl11-no-state", "OpenGL 1.1 renderer state is unavailable" },
    { "gl11-version-unavailable", "OpenGL 1.1 version string is unavailable" },
    { "gl21-out-of-memory", "OpenGL 2.1 backend ran out of memory" },
    { "gl21-make-current-failed", "OpenGL 2.1 MakeCurrent failed" },
    { "gl21-capture-invalid-state", "OpenGL 2.1 capability capture has no valid state" },
    { "gl21-capture-string-failed", "OpenGL 2.1 capability strings could not be read" },
    { "gl21-choose-pixel-format-failed", "OpenGL 2.1 pixel-format selection failed" },
    { "gl21-set-pixel-format-failed", "OpenGL 2.1 SetPixelFormat failed" },
    { "gl21-invalid-window", "OpenGL 2.1 target window is invalid" },
    { "gl21-getdc-failed", "OpenGL 2.1 GetDC failed" },
    { "gl21-bootstrap-context-failed", "OpenGL 2.1 bootstrap context creation failed" },
    { "gl21-bootstrap-make-current-failed", "OpenGL 2.1 bootstrap MakeCurrent failed" },
    { "gl21-create-context-attribs-unavailable", "OpenGL 2.1 context attributes are unavailable" },
    { "gl21-create-context-attribs-failed", "OpenGL 2.1 context attribute creation failed" },
    { "gl33-placeholder-unavailable", "OpenGL 3.3 is a placeholder in this build" },
    { "gl46-placeholder-unavailable", "OpenGL 4.6 is a placeholder in this build" }
};

static const screensave_display_mapping g_renderer_status_mappings[] = {
    { "ready", "Ready" },
    { "created", "Created" },
    { "created-double-buffered", "Double-buffered" },
    { "created-single-buffered", "Single-buffered" },
    { "advanced-context-double-buffered", "Advanced context, double-buffered" },
    { "advanced-context-single-buffered", "Advanced context, single-buffered" },
    { "fallback-gl33", "Running on an OpenGL 3.3 fallback" },
    { "fallback-gl21", "Running on an OpenGL 2.1 fallback" },
    { "fallback-gl11", "Running on an OpenGL 1.1 fallback" },
    { "fallback-gdi", "Running on the GDI floor" },
    { "fallback-null", "Running on the internal null safety fallback" },
    { "render-suppressed", "Internal null safety backend" },
    { "present-only", "Present only" }
};

static int screensave_display_text_copy(char *buffer, unsigned int buffer_size, const char *text)
{
    unsigned int text_length;

    if (buffer == NULL || buffer_size == 0U) {
        return 0;
    }

    buffer[0] = '\0';
    if (text == NULL) {
        return 1;
    }

    text_length = (unsigned int)strlen(text);
    if (text_length + 1U > buffer_size) {
        return 0;
    }

    memcpy(buffer, text, text_length + 1U);
    return 1;
}

static int screensave_display_append_text(char *buffer, unsigned int buffer_size, const char *text)
{
    unsigned int used;
    unsigned int added;

    if (buffer == NULL || text == NULL || buffer_size == 0U) {
        return 0;
    }

    used = (unsigned int)strlen(buffer);
    added = (unsigned int)strlen(text);
    if (used + added + 1U > buffer_size) {
        return 0;
    }

    memcpy(buffer + used, text, added + 1U);
    return 1;
}

static const char *screensave_display_lookup(
    const screensave_display_mapping *mappings,
    unsigned int mapping_count,
    const char *key
)
{
    unsigned int index;

    if (key == NULL || key[0] == '\0') {
        return NULL;
    }

    for (index = 0U; index < mapping_count; ++index) {
        if (strcmp(mappings[index].key, key) == 0) {
            return mappings[index].display_text;
        }
    }

    return NULL;
}

static const char *screensave_display_renderer_kind_token(const char *token)
{
    if (token == NULL || token[0] == '\0') {
        return NULL;
    }

    if (strcmp(token, "gdi") == 0) {
        return "GDI";
    }
    if (strcmp(token, "gl11") == 0) {
        return "OpenGL 1.1";
    }
    if (strcmp(token, "gl21") == 0) {
        return "OpenGL 2.1";
    }
    if (strcmp(token, "gl33") == 0) {
        return "OpenGL 3.3";
    }
    if (strcmp(token, "gl46") == 0) {
        return "OpenGL 4.6";
    }
    if (strcmp(token, "null") == 0) {
        return "internal null";
    }

    return NULL;
}

static const char *screensave_display_token_text(const char *token)
{
    const char *renderer_kind_text;

    renderer_kind_text = screensave_display_renderer_kind_token(token);
    if (renderer_kind_text != NULL) {
        return renderer_kind_text;
    }

    if (strcmp(token, "opengl") == 0) {
        return "OpenGL";
    }
    if (strcmp(token, "getdc") == 0) {
        return "GetDC";
    }
    if (strcmp(token, "attribs") == 0) {
        return "attributes";
    }
    if (strcmp(token, "wgl") == 0) {
        return "WGL";
    }
    if (strcmp(token, "make") == 0) {
        return "make";
    }
    if (strcmp(token, "current") == 0) {
        return "current";
    }

    return token;
}

static void screensave_display_capitalize(char *buffer)
{
    if (buffer != NULL && buffer[0] >= 'a' && buffer[0] <= 'z') {
        buffer[0] = (char)(buffer[0] - ('a' - 'A'));
    }
}

static int screensave_display_special_renderer_reason(
    const char *reason_code,
    char *buffer,
    unsigned int buffer_size
)
{
    const char *suffix;
    const char *fallback;
    char requested_token[16];
    char active_token[16];
    unsigned int requested_length;
    unsigned int active_length;
    const char *requested_text;
    const char *active_text;

    if (
        reason_code == NULL ||
        buffer == NULL ||
        buffer_size == 0U
    ) {
        return 0;
    }

    if (strncmp(reason_code, "force-", 6U) == 0) {
        suffix = reason_code + 6;
        fallback = strstr(suffix, "-fallback-");
        if (fallback != NULL) {
            requested_length = (unsigned int)(fallback - suffix);
            active_length = (unsigned int)strlen(fallback + 10U);
            if (
                requested_length > 0U &&
                requested_length < sizeof(requested_token) &&
                active_length > 0U &&
                active_length < sizeof(active_token)
            ) {
                memcpy(requested_token, suffix, requested_length);
                requested_token[requested_length] = '\0';
                memcpy(active_token, fallback + 10U, active_length + 1U);
                requested_text = screensave_display_renderer_kind_token(requested_token);
                active_text = screensave_display_renderer_kind_token(active_token);
                if (
                    requested_text != NULL &&
                    active_text != NULL &&
                    screensave_display_text_copy(buffer, buffer_size, "Requested ") &&
                    screensave_display_append_text(buffer, buffer_size, requested_text) &&
                    screensave_display_append_text(buffer, buffer_size, "; fell back to ") &&
                    screensave_display_append_text(buffer, buffer_size, active_text)
                ) {
                    return 1;
                }
            }
        } else {
            requested_text = screensave_display_renderer_kind_token(suffix);
            if (
                requested_text != NULL &&
                screensave_display_text_copy(buffer, buffer_size, "Requested ") &&
                screensave_display_append_text(buffer, buffer_size, requested_text)
            ) {
                return 1;
            }
        }
    }

    if (strncmp(reason_code, "auto-prefer-", 12U) == 0) {
        active_text = screensave_display_renderer_kind_token(reason_code + 12);
        if (
            active_text != NULL &&
            screensave_display_text_copy(buffer, buffer_size, "Auto selected ") &&
            screensave_display_append_text(buffer, buffer_size, active_text)
        ) {
            return 1;
        }
    }

    if (strncmp(reason_code, "auto-fallback-", 14U) == 0) {
        active_text = screensave_display_renderer_kind_token(reason_code + 14);
        if (
            active_text != NULL &&
            screensave_display_text_copy(buffer, buffer_size, "Auto fell back to ") &&
            screensave_display_append_text(buffer, buffer_size, active_text)
        ) {
            return 1;
        }
    }

    if (strncmp(reason_code, "fallback-", 9U) == 0) {
        active_text = screensave_display_renderer_kind_token(reason_code + 9);
        if (
            active_text != NULL &&
            screensave_display_text_copy(buffer, buffer_size, "Fallback active: ") &&
            screensave_display_append_text(buffer, buffer_size, active_text)
        ) {
            return 1;
        }
    }

    return 0;
}

static void screensave_display_humanize_code(
    const char *code,
    char *buffer,
    unsigned int buffer_size
)
{
    unsigned int index;
    unsigned int token_length;
    unsigned int token_index;
    char token[32];
    const char *token_text;

    if (buffer == NULL || buffer_size == 0U) {
        return;
    }

    buffer[0] = '\0';
    if (code == NULL || code[0] == '\0') {
        screensave_display_text_copy(buffer, buffer_size, "None");
        return;
    }

    index = 0U;
    while (code[index] != '\0') {
        while (code[index] == '-' || code[index] == '_') {
            ++index;
        }
        if (code[index] == '\0') {
            break;
        }

        token_length = 0U;
        while (code[index] != '\0' && code[index] != '-' && code[index] != '_') {
            if (token_length + 1U < sizeof(token)) {
                token[token_length++] = code[index];
            }
            ++index;
        }
        token[token_length] = '\0';

        token_text = screensave_display_token_text(token);
        if (buffer[0] != '\0') {
            screensave_display_append_text(buffer, buffer_size, " ");
        }
        screensave_display_append_text(buffer, buffer_size, token_text);
    }

    for (token_index = 0U; buffer[token_index] != '\0'; ++token_index) {
        if (buffer[token_index] == ' ' && buffer[token_index + 1U] == ' ') {
            buffer[token_index] = '\0';
            break;
        }
    }

    screensave_display_capitalize(buffer);
}

const char *screensave_display_detail_level(screensave_detail_level level)
{
    switch (level) {
    case SCREENSAVE_DETAIL_LEVEL_LOW:
        return "Low";

    case SCREENSAVE_DETAIL_LEVEL_HIGH:
        return "High";

    case SCREENSAVE_DETAIL_LEVEL_STANDARD:
    default:
        return "Standard";
    }
}

const char *screensave_display_randomization_mode(screensave_randomization_mode mode)
{
    switch (mode) {
    case SCREENSAVE_RANDOMIZATION_MODE_SESSION:
        return "Session";

    case SCREENSAVE_RANDOMIZATION_MODE_OFF:
    default:
        return "Off";
    }
}

const char *screensave_display_renderer_kind(screensave_renderer_kind kind)
{
    switch (kind) {
    case SCREENSAVE_RENDERER_KIND_UNKNOWN:
        return "Auto (best available)";

    case SCREENSAVE_RENDERER_KIND_GDI:
        return "GDI";

    case SCREENSAVE_RENDERER_KIND_GL11:
        return "OpenGL 1.1";

    case SCREENSAVE_RENDERER_KIND_GL21:
        return "OpenGL 2.1";

    case SCREENSAVE_RENDERER_KIND_GL33:
        return "OpenGL 3.3";

    case SCREENSAVE_RENDERER_KIND_GL46:
        return "OpenGL 4.6";

    case SCREENSAVE_RENDERER_KIND_NULL:
        return "Internal null";

    default:
        return "Unknown";
    }
}

void screensave_display_renderer_reason(
    const char *reason_code,
    char *buffer,
    unsigned int buffer_size
)
{
    const char *mapped_text;

    if (buffer == NULL || buffer_size == 0U) {
        return;
    }

    buffer[0] = '\0';
    if (reason_code == NULL || reason_code[0] == '\0') {
        screensave_display_text_copy(buffer, buffer_size, "None");
        return;
    }

    mapped_text = screensave_display_lookup(
        g_renderer_reason_mappings,
        sizeof(g_renderer_reason_mappings) / sizeof(g_renderer_reason_mappings[0]),
        reason_code
    );
    if (mapped_text != NULL) {
        (void)screensave_display_text_copy(buffer, buffer_size, mapped_text);
        return;
    }

    if (screensave_display_special_renderer_reason(reason_code, buffer, buffer_size)) {
        return;
    }

    screensave_display_humanize_code(reason_code, buffer, buffer_size);
}

void screensave_display_renderer_status(
    const char *status_code,
    char *buffer,
    unsigned int buffer_size
)
{
    const char *mapped_text;

    if (buffer == NULL || buffer_size == 0U) {
        return;
    }

    buffer[0] = '\0';
    if (status_code == NULL || status_code[0] == '\0') {
        screensave_display_text_copy(buffer, buffer_size, "None");
        return;
    }

    mapped_text = screensave_display_lookup(
        g_renderer_status_mappings,
        sizeof(g_renderer_status_mappings) / sizeof(g_renderer_status_mappings[0]),
        status_code
    );
    if (mapped_text != NULL) {
        (void)screensave_display_text_copy(buffer, buffer_size, mapped_text);
        return;
    }

    screensave_display_humanize_code(status_code, buffer, buffer_size);
}
