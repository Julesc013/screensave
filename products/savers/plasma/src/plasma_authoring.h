#ifndef PLASMA_AUTHORING_H
#define PLASMA_AUTHORING_H

#include "plasma_content.h"
#include "plasma_transition.h"

#define PLASMA_AUTHORING_TEXT_LENGTH 96U
#define PLASMA_AUTHORING_SUMMARY_LENGTH 260U
#define PLASMA_AUTHORING_PROFILE_LENGTH 32U
#define PLASMA_AUTHORING_MAX_MEMBERS 16U
#define PLASMA_AUTHORING_MAX_STEPS 16U

typedef struct plasma_authoring_weighted_key_tag {
    char content_key[PLASMA_CONTENT_KEY_TEXT_LENGTH];
    unsigned int weight;
} plasma_authoring_weighted_key;

typedef struct plasma_authored_preset_set_tag {
    char set_key[PLASMA_CONTENT_KEY_TEXT_LENGTH];
    char display_name[PLASMA_AUTHORING_TEXT_LENGTH];
    char summary[PLASMA_AUTHORING_SUMMARY_LENGTH];
    char profile_scope[PLASMA_AUTHORING_PROFILE_LENGTH];
    unsigned int member_count;
    plasma_authoring_weighted_key members[PLASMA_AUTHORING_MAX_MEMBERS];
} plasma_authored_preset_set;

typedef struct plasma_authored_theme_set_tag {
    char set_key[PLASMA_CONTENT_KEY_TEXT_LENGTH];
    char display_name[PLASMA_AUTHORING_TEXT_LENGTH];
    char summary[PLASMA_AUTHORING_SUMMARY_LENGTH];
    char profile_scope[PLASMA_AUTHORING_PROFILE_LENGTH];
    unsigned int member_count;
    plasma_authoring_weighted_key members[PLASMA_AUTHORING_MAX_MEMBERS];
} plasma_authored_theme_set;

typedef struct plasma_authored_journey_step_tag {
    char preset_set_key[PLASMA_CONTENT_KEY_TEXT_LENGTH];
    char theme_set_key[PLASMA_CONTENT_KEY_TEXT_LENGTH];
    plasma_transition_policy policy;
    unsigned long dwell_millis;
} plasma_authored_journey_step;

typedef struct plasma_authored_journey_tag {
    char journey_key[PLASMA_TRANSITION_KEY_TEXT_LENGTH];
    char display_name[PLASMA_AUTHORING_TEXT_LENGTH];
    char summary[PLASMA_AUTHORING_SUMMARY_LENGTH];
    char profile_scope[PLASMA_AUTHORING_PROFILE_LENGTH];
    char journey_intent[PLASMA_AUTHORING_PROFILE_LENGTH];
    unsigned int step_count;
    plasma_authored_journey_step steps[PLASMA_AUTHORING_MAX_STEPS];
} plasma_authored_journey;

typedef struct plasma_authored_pack_provenance_tag {
    char pack_key[PLASMA_CONTENT_KEY_TEXT_LENGTH];
    unsigned long content_version;
    char channel[PLASMA_AUTHORING_PROFILE_LENGTH];
    char support_tier[PLASMA_AUTHORING_PROFILE_LENGTH];
    char source_kind[PLASMA_AUTHORING_PROFILE_LENGTH];
    char author[PLASMA_AUTHORING_TEXT_LENGTH];
    char provenance_kind[PLASMA_AUTHORING_PROFILE_LENGTH];
    char summary[PLASMA_AUTHORING_SUMMARY_LENGTH];
    char product_key[PLASMA_CONTENT_KEY_TEXT_LENGTH];
    unsigned long pack_schema_version;
    char minimum_kind[16];
    char preferred_kind[16];
    char quality_class[16];
    char migration_policy[PLASMA_AUTHORING_TEXT_LENGTH];
    int preserves_classic_identity;
    char compatibility_notes[PLASMA_AUTHORING_SUMMARY_LENGTH];
} plasma_authored_pack_provenance;

int plasma_authoring_load_preset_set_file(
    const char *path,
    plasma_authored_preset_set *set_out
);
int plasma_authoring_load_theme_set_file(
    const char *path,
    plasma_authored_theme_set *set_out
);
int plasma_authoring_load_journey_file(
    const char *path,
    plasma_authored_journey *journey_out
);
int plasma_authoring_load_pack_provenance_file(
    const char *path,
    plasma_authored_pack_provenance *provenance_out
);
int plasma_authoring_validate_repo_surface(
    const char *repo_root,
    char *message_out,
    unsigned int message_size
);

#endif /* PLASMA_AUTHORING_H */
