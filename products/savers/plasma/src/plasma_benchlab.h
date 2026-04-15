#ifndef PLASMA_BENCHLAB_H
#define PLASMA_BENCHLAB_H

#include "screensave/config_api.h"
#include "screensave/saver_api.h"

#define PLASMA_BENCHLAB_TEXT_LENGTH 64U
#define PLASMA_BENCHLAB_NOTES_LENGTH 256U

#define PLASMA_BENCHLAB_CLAMP_PRESET_KEY 0x00000001UL
#define PLASMA_BENCHLAB_CLAMP_THEME_KEY 0x00000002UL
#define PLASMA_BENCHLAB_CLAMP_PRESET_SET_KEY 0x00000004UL
#define PLASMA_BENCHLAB_CLAMP_THEME_SET_KEY 0x00000008UL
#define PLASMA_BENCHLAB_CLAMP_CONTENT_FILTER 0x00000010UL
#define PLASMA_BENCHLAB_CLAMP_FAVORITES_ONLY 0x00000020UL
#define PLASMA_BENCHLAB_CLAMP_TRANSITIONS_ENABLED 0x00000040UL
#define PLASMA_BENCHLAB_CLAMP_TRANSITION_POLICY 0x00000080UL
#define PLASMA_BENCHLAB_CLAMP_TRANSITION_FALLBACK 0x00000100UL
#define PLASMA_BENCHLAB_CLAMP_TRANSITION_SEED 0x00000200UL
#define PLASMA_BENCHLAB_CLAMP_TRANSITION_INTERVAL 0x00000400UL
#define PLASMA_BENCHLAB_CLAMP_TRANSITION_DURATION 0x00000800UL
#define PLASMA_BENCHLAB_CLAMP_JOURNEY_KEY 0x00001000UL
#define PLASMA_BENCHLAB_CLAMP_PRESENTATION 0x00002000UL

struct plasma_config_tag;
struct plasma_plan_tag;

typedef enum plasma_benchlab_presentation_request_tag {
    PLASMA_BENCHLAB_PRESENTATION_AUTO = 0,
    PLASMA_BENCHLAB_PRESENTATION_HEIGHTFIELD = 1,
    PLASMA_BENCHLAB_PRESENTATION_CURTAIN = 2,
    PLASMA_BENCHLAB_PRESENTATION_RIBBON = 3,
    PLASMA_BENCHLAB_PRESENTATION_CONTOUR_EXTRUSION = 4,
    PLASMA_BENCHLAB_PRESENTATION_BOUNDED_SURFACE = 5
} plasma_benchlab_presentation_request;

typedef struct plasma_benchlab_forcing_tag {
    int active;
    unsigned long clamp_flags;
    char preset_key[PLASMA_BENCHLAB_TEXT_LENGTH];
    char theme_key[PLASMA_BENCHLAB_TEXT_LENGTH];
    char preset_set_key[PLASMA_BENCHLAB_TEXT_LENGTH];
    char theme_set_key[PLASMA_BENCHLAB_TEXT_LENGTH];
    char journey_key[PLASMA_BENCHLAB_TEXT_LENGTH];
    int content_filter_override;
    int favorites_only_override;
    int transitions_enabled_override;
    int transition_policy_override;
    int transition_fallback_override;
    int transition_seed_policy_override;
    int transition_interval_override_enabled;
    unsigned long transition_interval_millis;
    int transition_duration_override_enabled;
    unsigned long transition_duration_millis;
    plasma_benchlab_presentation_request presentation_request;
} plasma_benchlab_forcing;

typedef struct plasma_benchlab_snapshot_tag {
    const char *requested_lane;
    const char *resolved_lane;
    const char *degraded_from_lane;
    const char *degraded_to_lane;
    const char *preset_key;
    const char *theme_key;
    const char *preset_set_key;
    const char *theme_set_key;
    const char *journey_key;
    const char *profile_class;
    const char *quality_class;
    const char *preset_source;
    const char *preset_channel;
    const char *theme_source;
    const char *theme_channel;
    const char *requested_detail_level;
    const char *detail_level;
    const char *requested_generator_family;
    const char *generator_family;
    const char *requested_speed_mode;
    const char *speed_mode;
    const char *requested_resolution_mode;
    const char *resolution_mode;
    const char *requested_smoothing_mode;
    const char *smoothing_mode;
    const char *requested_output_family;
    const char *output_family;
    const char *requested_output_mode;
    const char *output_mode;
    const char *requested_sampling_treatment;
    const char *sampling_treatment;
    const char *requested_filter_treatment;
    const char *filter_treatment;
    const char *requested_emulation_treatment;
    const char *emulation_treatment;
    const char *requested_accent_treatment;
    const char *accent_treatment;
    const char *requested_presentation_mode;
    const char *presentation_mode;
    const char *transition_policy;
    const char *transition_type;
    const char *transition_requested_type;
    const char *transition_resolved_type;
    const char *transition_fallback_type;
    const char *transition_fallback_policy;
    const char *transition_seed_policy;
    const char *transition_source_preset;
    const char *transition_target_preset;
    const char *transition_source_theme;
    const char *transition_target_theme;
    const char *preset_morph_class;
    const char *theme_morph_class;
    const char *bridge_class;
    const char *seed_policy;
    const char *settings_surface;
    const char *content_filter;
    unsigned long configured_seed;
    unsigned long base_seed;
    unsigned long stream_seed;
    unsigned long resolved_rng_seed;
    unsigned long clamp_flags;
    int favorites_only_requested;
    int favorites_only_applied;
    int transition_requested;
    int transition_enabled;
    int forcing_active;
    char clamp_summary[PLASMA_BENCHLAB_NOTES_LENGTH];
} plasma_benchlab_snapshot;

void plasma_benchlab_forcing_set_defaults(plasma_benchlab_forcing *forcing);
void plasma_benchlab_forcing_clamp(plasma_benchlab_forcing *forcing);
int plasma_benchlab_parse_command_line(
    const char *command_line,
    plasma_benchlab_forcing *forcing,
    screensave_diag_context *diagnostics
);
void plasma_benchlab_apply_forcing_to_config(
    const plasma_benchlab_forcing *forcing,
    screensave_common_config *common_config,
    struct plasma_config_tag *product_config
);
void plasma_benchlab_apply_plan_forcing(
    struct plasma_plan_tag *plan,
    const screensave_saver_module *module,
    const plasma_benchlab_forcing *forcing
);
int plasma_benchlab_build_snapshot(
    const screensave_saver_session *session,
    const screensave_saver_config_state *resolved_config,
    screensave_renderer_kind requested_renderer_kind,
    plasma_benchlab_snapshot *snapshot_out
);
int plasma_benchlab_build_overlay_summary(
    const screensave_saver_session *session,
    const screensave_saver_config_state *resolved_config,
    screensave_renderer_kind requested_renderer_kind,
    char *buffer,
    unsigned int buffer_size
);
int plasma_benchlab_build_report_section(
    const screensave_saver_session *session,
    const screensave_saver_config_state *resolved_config,
    screensave_renderer_kind requested_renderer_kind,
    char *buffer,
    unsigned int buffer_size
);

#endif /* PLASMA_BENCHLAB_H */
