#ifndef PLASMA_VALIDATE_H
#define PLASMA_VALIDATE_H

#include "screensave/saver_api.h"

struct plasma_plan_tag;

typedef enum plasma_validation_status_tag {
    PLASMA_VALIDATION_STATUS_VALIDATED = 0,
    PLASMA_VALIDATION_STATUS_PARTIAL = 1,
    PLASMA_VALIDATION_STATUS_DOCUMENTED_ONLY = 2,
    PLASMA_VALIDATION_STATUS_UNSUPPORTED = 3,
    PLASMA_VALIDATION_STATUS_BLOCKED = 4
} plasma_validation_status;

typedef struct plasma_validation_matrix_entry_tag {
    const char *area_key;
    const char *lane_key;
    plasma_validation_status status;
    const char *evidence_ref;
    const char *notes;
} plasma_validation_matrix_entry;

typedef struct plasma_performance_envelope_entry_tag {
    const char *envelope_key;
    const char *lane_key;
    plasma_validation_status status;
    const char *measurement_mode;
    const char *startup_notes;
    const char *runtime_notes;
    const char *transition_notes;
    const char *soak_notes;
    const char *evidence_ref;
} plasma_performance_envelope_entry;

typedef struct plasma_known_limit_entry_tag {
    const char *limit_key;
    plasma_validation_status status;
    const char *notes;
    const char *evidence_ref;
} plasma_known_limit_entry;

const char *plasma_validation_status_name(plasma_validation_status status);
const plasma_validation_matrix_entry *plasma_validation_get_matrix(
    unsigned int *count_out
);
const plasma_validation_matrix_entry *plasma_validation_find_matrix_entry(
    const char *area_key,
    const char *lane_key
);
int plasma_validation_matrix_is_consistent(void);
const plasma_performance_envelope_entry *plasma_validation_get_performance_envelopes(
    unsigned int *count_out
);
const plasma_performance_envelope_entry *plasma_validation_find_performance_envelope(
    const char *envelope_key
);
int plasma_validation_performance_envelopes_are_consistent(void);
const plasma_known_limit_entry *plasma_validation_get_known_limits(
    unsigned int *count_out
);
const plasma_known_limit_entry *plasma_validation_find_known_limit(
    const char *limit_key
);
int plasma_validation_known_limits_are_consistent(void);
screensave_renderer_kind plasma_resolve_renderer_kind(
    const screensave_saver_environment *environment
);
screensave_renderer_kind plasma_resolve_requested_renderer_kind(
    const screensave_saver_environment *environment
);
int plasma_is_lower_band_kind(screensave_renderer_kind renderer_kind);
int plasma_is_advanced_runtime_kind(screensave_renderer_kind renderer_kind);
int plasma_is_modern_runtime_kind(screensave_renderer_kind renderer_kind);
int plasma_is_premium_runtime_kind(screensave_renderer_kind renderer_kind);
int plasma_plan_is_lower_band_baseline(const struct plasma_plan_tag *plan);
int plasma_plan_validate_for_renderer_kind(
    const struct plasma_plan_tag *plan,
    const screensave_saver_module *module,
    screensave_renderer_kind renderer_kind
);
int plasma_plan_validate_lower_band_baseline(
    const struct plasma_plan_tag *plan,
    const screensave_saver_module *module
);

#endif /* PLASMA_VALIDATE_H */
