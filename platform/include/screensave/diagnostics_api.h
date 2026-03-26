#ifndef SCREENSAVE_DIAGNOSTICS_API_H
#define SCREENSAVE_DIAGNOSTICS_API_H

typedef enum screensave_diag_level_tag {
    SCREENSAVE_DIAG_LEVEL_ERROR = 0,
    SCREENSAVE_DIAG_LEVEL_WARNING = 1,
    SCREENSAVE_DIAG_LEVEL_INFO = 2,
    SCREENSAVE_DIAG_LEVEL_DEBUG = 3
} screensave_diag_level;

typedef enum screensave_diag_domain_tag {
    SCREENSAVE_DIAG_DOMAIN_PLATFORM = 0,
    SCREENSAVE_DIAG_DOMAIN_HOST = 1,
    SCREENSAVE_DIAG_DOMAIN_CORE = 2,
    SCREENSAVE_DIAG_DOMAIN_CONFIG = 3,
    SCREENSAVE_DIAG_DOMAIN_RENDERER = 4,
    SCREENSAVE_DIAG_DOMAIN_SAVER = 5
} screensave_diag_domain;

typedef struct screensave_diag_message_tag {
    screensave_diag_level level;
    screensave_diag_domain domain;
    unsigned long code;
    const char *origin;
    const char *text;
} screensave_diag_message;

typedef void (*screensave_diag_sink_fn)(void *user_data, const screensave_diag_message *message);

typedef struct screensave_diag_context_tag {
    screensave_diag_sink_fn sink;
    void *sink_user_data;
    screensave_diag_level minimum_level;
    screensave_diag_level last_level;
    screensave_diag_domain last_domain;
    unsigned long emitted_count;
    unsigned long dropped_count;
    char last_text[128];
} screensave_diag_context;

void screensave_diag_context_init(screensave_diag_context *context, screensave_diag_level minimum_level);
void screensave_diag_set_sink(
    screensave_diag_context *context,
    screensave_diag_sink_fn sink,
    void *user_data
);
void screensave_diag_set_minimum_level(screensave_diag_context *context, screensave_diag_level minimum_level);
int screensave_diag_should_emit(const screensave_diag_context *context, screensave_diag_level level);
void screensave_diag_emit(
    screensave_diag_context *context,
    screensave_diag_level level,
    screensave_diag_domain domain,
    unsigned long code,
    const char *origin,
    const char *text
);
const char *screensave_diag_level_name(screensave_diag_level level);
const char *screensave_diag_domain_name(screensave_diag_domain domain);

#endif /* SCREENSAVE_DIAGNOSTICS_API_H */
