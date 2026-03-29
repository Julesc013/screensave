#ifndef SCREENSAVE_SETTINGS_API_H
#define SCREENSAVE_SETTINGS_API_H

#include "screensave/diagnostics_api.h"

struct screensave_saver_module_tag;
struct screensave_saver_config_state_tag;

typedef enum screensave_settings_file_kind_tag {
    SCREENSAVE_SETTINGS_FILE_PRESET = 0,
    SCREENSAVE_SETTINGS_FILE_THEME = 1
} screensave_settings_file_kind;

typedef enum screensave_pack_source_kind_tag {
    SCREENSAVE_PACK_SOURCE_BUILTIN = 0,
    SCREENSAVE_PACK_SOURCE_PORTABLE = 1,
    SCREENSAVE_PACK_SOURCE_USER = 2
} screensave_pack_source_kind;

#define SCREENSAVE_SETTINGS_FILE_FORMAT_VERSION 1UL
#define SCREENSAVE_PACK_MANIFEST_VERSION 1UL
#define SCREENSAVE_SETTINGS_CAP_PRESET_FILES 0x00000001UL
#define SCREENSAVE_SETTINGS_CAP_THEME_FILES 0x00000002UL
#define SCREENSAVE_SETTINGS_CAP_RANDOMIZATION 0x00000004UL
#define SCREENSAVE_SETTINGS_CAP_PACKS 0x00000008UL
#define SCREENSAVE_PACK_CONTENT_PRESETS 0x00000001UL
#define SCREENSAVE_PACK_CONTENT_THEMES 0x00000002UL
#define SCREENSAVE_PACK_CONTENT_SCENES 0x00000004UL
#define SCREENSAVE_PACK_ISSUE_BAD_FORMAT_VERSION 0x00000001UL
#define SCREENSAVE_PACK_ISSUE_EMPTY_PACK_KEY 0x00000002UL
#define SCREENSAVE_PACK_ISSUE_EMPTY_PRODUCT_KEY 0x00000004UL
#define SCREENSAVE_PACK_ISSUE_EMPTY_DISPLAY_NAME 0x00000008UL
#define SCREENSAVE_PACK_ISSUE_BAD_CONTENT_FLAGS 0x00000010UL
#define SCREENSAVE_PACK_ISSUE_BAD_ENTRY_PATH 0x00000020UL
#define SCREENSAVE_PACK_ISSUE_MISSING_ENTRIES 0x00000040UL

#define SCREENSAVE_PACK_ENTRY_LIMIT 16U
#define SCREENSAVE_PACK_TEXT_LENGTH 96U
#define SCREENSAVE_PACK_PATH_LENGTH 260U

typedef struct screensave_settings_writer_tag {
    void *context;
    int (*write_string)(
        void *context,
        const char *section,
        const char *key,
        const char *value
    );
    int (*write_unsigned_long)(
        void *context,
        const char *section,
        const char *key,
        unsigned long value
    );
} screensave_settings_writer;

typedef struct screensave_pack_manifest_tag {
    unsigned long format_version;
    unsigned long schema_version;
    screensave_pack_source_kind source_kind;
    unsigned long content_flags;
    unsigned int preset_file_count;
    unsigned int theme_file_count;
    unsigned int scene_file_count;
    char pack_key[SCREENSAVE_PACK_TEXT_LENGTH];
    char product_key[SCREENSAVE_PACK_TEXT_LENGTH];
    char display_name[SCREENSAVE_PACK_TEXT_LENGTH];
    char description[SCREENSAVE_PACK_PATH_LENGTH];
    char manifest_path[SCREENSAVE_PACK_PATH_LENGTH];
    char preset_files[SCREENSAVE_PACK_ENTRY_LIMIT][SCREENSAVE_PACK_PATH_LENGTH];
    char theme_files[SCREENSAVE_PACK_ENTRY_LIMIT][SCREENSAVE_PACK_PATH_LENGTH];
    char scene_files[SCREENSAVE_PACK_ENTRY_LIMIT][SCREENSAVE_PACK_PATH_LENGTH];
} screensave_pack_manifest;

int screensave_settings_export_file(
    const struct screensave_saver_module_tag *module,
    const struct screensave_saver_config_state_tag *config_state,
    screensave_settings_file_kind kind,
    const char *path,
    screensave_diag_context *diagnostics
);
int screensave_settings_import_file(
    const struct screensave_saver_module_tag *module,
    struct screensave_saver_config_state_tag *config_state,
    screensave_settings_file_kind kind,
    const char *path,
    screensave_diag_context *diagnostics
);
void screensave_pack_manifest_set_defaults(screensave_pack_manifest *manifest);
void screensave_pack_manifest_clamp(screensave_pack_manifest *manifest);
int screensave_pack_manifest_validate(
    const screensave_pack_manifest *manifest,
    unsigned long *issue_flags
);
int screensave_pack_manifest_load(
    const char *path,
    screensave_pack_source_kind source_kind,
    screensave_pack_manifest *manifest,
    screensave_diag_context *diagnostics
);
int screensave_pack_manifest_discover(
    const char *directory_path,
    screensave_pack_source_kind source_kind,
    const char *product_key_filter,
    screensave_pack_manifest *manifests,
    unsigned int manifest_capacity,
    unsigned int *manifest_count_out,
    screensave_diag_context *diagnostics
);
const char *screensave_settings_file_kind_name(screensave_settings_file_kind kind);
const char *screensave_pack_source_kind_name(screensave_pack_source_kind source_kind);

#endif /* SCREENSAVE_SETTINGS_API_H */
