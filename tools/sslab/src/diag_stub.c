#include "screensave/diagnostics_api.h"

void screensave_diag_emit(
    screensave_diag_context *context,
    screensave_diag_level level,
    screensave_diag_domain domain,
    unsigned long code,
    const char *origin,
    const char *text
)
{
    (void)context;
    (void)level;
    (void)domain;
    (void)code;
    (void)origin;
    (void)text;
}
