#include <stdlib.h>
#include <string.h>

#include "plasma_internal.h"

typedef struct plasma_authoring_expected_file_tag {
    const char *relative_path;
    const char *key;
} plasma_authoring_expected_file;

static const plasma_authoring_expected_file g_expected_preset_set_files[] = {
    { "products\\savers\\plasma\\preset_sets\\classic_core.presetset.ini", "classic_core" },
    { "products\\savers\\plasma\\preset_sets\\dark_room_classics.presetset.ini", "dark_room_classics" },
    { "products\\savers\\plasma\\preset_sets\\fire_classics.presetset.ini", "fire_classics" },
    { "products\\savers\\plasma\\preset_sets\\plasma_classics.presetset.ini", "plasma_classics" },
    { "products\\savers\\plasma\\preset_sets\\interference_classics.presetset.ini", "interference_classics" },
    { "products\\savers\\plasma\\preset_sets\\warm_bridge_classics.presetset.ini", "warm_bridge_classics" },
    { "products\\savers\\plasma\\preset_sets\\cool_bridge_classics.presetset.ini", "cool_bridge_classics" }
};

static const plasma_authoring_expected_file g_expected_theme_set_files[] = {
    { "products\\savers\\plasma\\theme_sets\\classic_core.themeset.ini", "classic_core" },
    { "products\\savers\\plasma\\theme_sets\\dark_room_classics.themeset.ini", "dark_room_classics" },
    { "products\\savers\\plasma\\theme_sets\\warm_classics.themeset.ini", "warm_classics" },
    { "products\\savers\\plasma\\theme_sets\\cool_classics.themeset.ini", "cool_classics" }
};

static const plasma_authoring_expected_file g_expected_journey_files[] = {
    { "products\\savers\\plasma\\journeys\\classic_cycle.journey.ini", "classic_cycle" },
    { "products\\savers\\plasma\\journeys\\dark_room_cycle.journey.ini", "dark_room_cycle" },
    { "products\\savers\\plasma\\journeys\\warm_bridge_cycle.journey.ini", "warm_bridge_cycle" },
    { "products\\savers\\plasma\\journeys\\cool_bridge_cycle.journey.ini", "cool_bridge_cycle" }
};

static void plasma_authoring_copy_text(
    char *buffer,
    unsigned int buffer_size,
    const char *text
)
{
    if (buffer == NULL || buffer_size == 0U) {
        return;
    }

    buffer[0] = '\0';
    if (text == NULL) {
        return;
    }

    lstrcpynA(buffer, text, (int)buffer_size);
}

static void plasma_authoring_set_message(
    char *message_out,
    unsigned int message_size,
    const char *message
)
{
    plasma_authoring_copy_text(message_out, message_size, message);
}

static int plasma_authoring_read_string(
    const char *path,
    const char *section,
    const char *key,
    char *value_out,
    unsigned int value_size
)
{
    DWORD result;

    if (
        path == NULL ||
        section == NULL ||
        key == NULL ||
        value_out == NULL ||
        value_size == 0U
    ) {
        return 0;
    }

    value_out[0] = '\0';
    result = GetPrivateProfileStringA(
        section,
        key,
        "",
        value_out,
        (DWORD)value_size,
        path
    );
    return result > 0U;
}

static int plasma_authoring_read_unsigned_long(
    const char *path,
    const char *section,
    const char *key,
    unsigned long *value_out
)
{
    char text[32];
    char *end_ptr;
    unsigned long value;

    if (
        value_out == NULL ||
        !plasma_authoring_read_string(path, section, key, text, (unsigned int)sizeof(text))
    ) {
        return 0;
    }

    value = strtoul(text, &end_ptr, 10);
    if (end_ptr == text || (end_ptr != NULL && *end_ptr != '\0')) {
        return 0;
    }

    *value_out = value;
    return 1;
}

static int plasma_authoring_read_flag(
    const char *path,
    const char *section,
    const char *key,
    int *flag_out
)
{
    char text[16];

    if (
        flag_out == NULL ||
        !plasma_authoring_read_string(path, section, key, text, (unsigned int)sizeof(text))
    ) {
        return 0;
    }

    if (
        lstrcmpiA(text, "1") == 0 ||
        lstrcmpiA(text, "true") == 0 ||
        lstrcmpiA(text, "yes") == 0
    ) {
        *flag_out = 1;
        return 1;
    }

    if (
        lstrcmpiA(text, "0") == 0 ||
        lstrcmpiA(text, "false") == 0 ||
        lstrcmpiA(text, "no") == 0
    ) {
        *flag_out = 0;
        return 1;
    }

    return 0;
}

static int plasma_authoring_load_header(
    const char *path,
    const char *expected_kind
)
{
    char kind[64];
    char product_key[PLASMA_CONTENT_KEY_TEXT_LENGTH];
    unsigned long version;
    unsigned long schema_version;

    if (
        path == NULL ||
        expected_kind == NULL ||
        !plasma_authoring_read_string(path, "format", "kind", kind, (unsigned int)sizeof(kind)) ||
        !plasma_authoring_read_unsigned_long(path, "format", "version", &version) ||
        !plasma_authoring_read_string(
            path,
            "product",
            "key",
            product_key,
            (unsigned int)sizeof(product_key)) ||
        !plasma_authoring_read_unsigned_long(path, "product", "schema_version", &schema_version)
    ) {
        return 0;
    }

    return
        strcmp(kind, expected_kind) == 0 &&
        version == 1UL &&
        strcmp(product_key, "plasma") == 0 &&
        schema_version == 1UL;
}

static int plasma_authoring_build_path(
    const char *repo_root,
    const char *relative_path,
    char *path_out,
    unsigned int path_size
)
{
    unsigned int root_length;
    unsigned int relative_length;

    if (
        repo_root == NULL ||
        relative_path == NULL ||
        path_out == NULL ||
        path_size == 0U
    ) {
        return 0;
    }

    root_length = (unsigned int)lstrlenA(repo_root);
    relative_length = (unsigned int)lstrlenA(relative_path);
    if (root_length == 0U) {
        if (relative_length + 1U > path_size) {
            return 0;
        }
        lstrcpynA(path_out, relative_path, (int)path_size);
        return 1;
    }

    if (
        root_length + 1U + relative_length + 1U > path_size &&
        root_length + relative_length + 1U > path_size
    ) {
        return 0;
    }

    lstrcpynA(path_out, repo_root, (int)path_size);
    if (
        root_length > 0U &&
        path_out[root_length - 1U] != '\\' &&
        path_out[root_length - 1U] != '/'
    ) {
        lstrcpynA(path_out + root_length, "\\", (int)(path_size - root_length));
        ++root_length;
    }
    lstrcpynA(path_out + root_length, relative_path, (int)(path_size - root_length));
    return 1;
}

static int plasma_authoring_profile_matches_channel(
    const char *profile_scope,
    plasma_content_channel channel_scope
)
{
    if (profile_scope == NULL) {
        return 0;
    }

    if (channel_scope == PLASMA_CONTENT_CHANNEL_STABLE) {
        return strcmp(profile_scope, "stable") == 0;
    }

    if (channel_scope == PLASMA_CONTENT_CHANNEL_EXPERIMENTAL) {
        return strcmp(profile_scope, "experimental") == 0;
    }

    return 0;
}

static int plasma_authoring_load_weighted_members(
    const char *path,
    const char *section_prefix,
    const char *member_key_name,
    plasma_authoring_weighted_key *members_out,
    unsigned int member_capacity,
    unsigned int *member_count_out
)
{
    unsigned int index;

    if (
        path == NULL ||
        section_prefix == NULL ||
        member_key_name == NULL ||
        members_out == NULL ||
        member_count_out == NULL ||
        member_capacity == 0U
    ) {
        return 0;
    }

    *member_count_out = 0U;
    for (index = 0U; index < member_capacity; ++index) {
        char section_name[32];
        char content_key[PLASMA_CONTENT_KEY_TEXT_LENGTH];
        unsigned long weight;

        wsprintfA(section_name, "%s_%u", section_prefix, index + 1U);
        if (!plasma_authoring_read_string(
                path,
                section_name,
                member_key_name,
                content_key,
                (unsigned int)sizeof(content_key))) {
            break;
        }
        if (
            !plasma_authoring_read_unsigned_long(path, section_name, "weight", &weight) ||
            weight == 0UL
        ) {
            return 0;
        }

        plasma_authoring_copy_text(
            members_out[index].content_key,
            (unsigned int)sizeof(members_out[index].content_key),
            content_key);
        members_out[index].weight = (unsigned int)weight;
        *member_count_out = index + 1U;
    }

    return *member_count_out > 0U;
}

int plasma_authoring_load_preset_set_file(
    const char *path,
    plasma_authored_preset_set *set_out
)
{
    if (
        path == NULL ||
        set_out == NULL ||
        !plasma_authoring_load_header(path, "plasma-preset-set")
    ) {
        return 0;
    }

    ZeroMemory(set_out, sizeof(*set_out));
    if (
        !plasma_authoring_read_string(path, "preset_set", "set_key", set_out->set_key, (unsigned int)sizeof(set_out->set_key)) ||
        !plasma_authoring_read_string(
            path,
            "preset_set",
            "display_name",
            set_out->display_name,
            (unsigned int)sizeof(set_out->display_name)) ||
        !plasma_authoring_read_string(
            path,
            "preset_set",
            "summary",
            set_out->summary,
            (unsigned int)sizeof(set_out->summary)) ||
        !plasma_authoring_read_string(
            path,
            "preset_set",
            "profile_scope",
            set_out->profile_scope,
            (unsigned int)sizeof(set_out->profile_scope)) ||
        !plasma_authoring_load_weighted_members(
            path,
            "member",
            "preset_key",
            set_out->members,
            PLASMA_AUTHORING_MAX_MEMBERS,
            &set_out->member_count)
    ) {
        return 0;
    }

    return 1;
}

int plasma_authoring_load_theme_set_file(
    const char *path,
    plasma_authored_theme_set *set_out
)
{
    if (
        path == NULL ||
        set_out == NULL ||
        !plasma_authoring_load_header(path, "plasma-theme-set")
    ) {
        return 0;
    }

    ZeroMemory(set_out, sizeof(*set_out));
    if (
        !plasma_authoring_read_string(path, "theme_set", "set_key", set_out->set_key, (unsigned int)sizeof(set_out->set_key)) ||
        !plasma_authoring_read_string(
            path,
            "theme_set",
            "display_name",
            set_out->display_name,
            (unsigned int)sizeof(set_out->display_name)) ||
        !plasma_authoring_read_string(
            path,
            "theme_set",
            "summary",
            set_out->summary,
            (unsigned int)sizeof(set_out->summary)) ||
        !plasma_authoring_read_string(
            path,
            "theme_set",
            "profile_scope",
            set_out->profile_scope,
            (unsigned int)sizeof(set_out->profile_scope)) ||
        !plasma_authoring_load_weighted_members(
            path,
            "member",
            "theme_key",
            set_out->members,
            PLASMA_AUTHORING_MAX_MEMBERS,
            &set_out->member_count)
    ) {
        return 0;
    }

    return 1;
}

int plasma_authoring_load_journey_file(
    const char *path,
    plasma_authored_journey *journey_out
)
{
    unsigned int index;

    if (
        path == NULL ||
        journey_out == NULL ||
        !plasma_authoring_load_header(path, "plasma-journey")
    ) {
        return 0;
    }

    ZeroMemory(journey_out, sizeof(*journey_out));
    if (
        !plasma_authoring_read_string(
            path,
            "journey",
            "journey_key",
            journey_out->journey_key,
            (unsigned int)sizeof(journey_out->journey_key)) ||
        !plasma_authoring_read_string(
            path,
            "journey",
            "display_name",
            journey_out->display_name,
            (unsigned int)sizeof(journey_out->display_name)) ||
        !plasma_authoring_read_string(
            path,
            "journey",
            "summary",
            journey_out->summary,
            (unsigned int)sizeof(journey_out->summary)) ||
        !plasma_authoring_read_string(
            path,
            "journey",
            "profile_scope",
            journey_out->profile_scope,
            (unsigned int)sizeof(journey_out->profile_scope)) ||
        !plasma_authoring_read_string(
            path,
            "journey",
            "journey_intent",
            journey_out->journey_intent,
            (unsigned int)sizeof(journey_out->journey_intent))
    ) {
        return 0;
    }

    for (index = 0U; index < PLASMA_AUTHORING_MAX_STEPS; ++index) {
        char section_name[32];
        char policy_text[32];
        unsigned long dwell_millis;
        plasma_transition_policy policy;

        wsprintfA(section_name, "step_%u", index + 1U);
        if (!plasma_authoring_read_string(
                path,
                section_name,
                "preset_set_key",
                journey_out->steps[index].preset_set_key,
                (unsigned int)sizeof(journey_out->steps[index].preset_set_key))) {
            break;
        }
        if (
            !plasma_authoring_read_string(
                path,
                section_name,
                "theme_set_key",
                journey_out->steps[index].theme_set_key,
                (unsigned int)sizeof(journey_out->steps[index].theme_set_key)) ||
            !plasma_authoring_read_string(path, section_name, "policy", policy_text, (unsigned int)sizeof(policy_text)) ||
            !plasma_transition_parse_policy(policy_text, &policy) ||
            !plasma_authoring_read_unsigned_long(path, section_name, "dwell_millis", &dwell_millis) ||
            dwell_millis == 0UL
        ) {
            return 0;
        }

        journey_out->steps[index].policy = policy;
        journey_out->steps[index].dwell_millis = dwell_millis;
        journey_out->step_count = index + 1U;
    }

    return journey_out->step_count > 0U;
}

int plasma_authoring_load_pack_provenance_file(
    const char *path,
    plasma_authored_pack_provenance *provenance_out
)
{
    if (
        path == NULL ||
        provenance_out == NULL ||
        !plasma_authoring_load_header(path, "plasma-pack-provenance")
    ) {
        return 0;
    }

    ZeroMemory(provenance_out, sizeof(*provenance_out));
    if (
        !plasma_authoring_read_string(
            path,
            "pack_provenance",
            "pack_key",
            provenance_out->pack_key,
            (unsigned int)sizeof(provenance_out->pack_key)) ||
        !plasma_authoring_read_unsigned_long(
            path,
            "pack_provenance",
            "content_version",
            &provenance_out->content_version) ||
        !plasma_authoring_read_string(
            path,
            "pack_provenance",
            "channel",
            provenance_out->channel,
            (unsigned int)sizeof(provenance_out->channel)) ||
        !plasma_authoring_read_string(
            path,
            "pack_provenance",
            "support_tier",
            provenance_out->support_tier,
            (unsigned int)sizeof(provenance_out->support_tier)) ||
        !plasma_authoring_read_string(
            path,
            "pack_provenance",
            "source_kind",
            provenance_out->source_kind,
            (unsigned int)sizeof(provenance_out->source_kind)) ||
        !plasma_authoring_read_string(
            path,
            "pack_provenance",
            "author",
            provenance_out->author,
            (unsigned int)sizeof(provenance_out->author)) ||
        !plasma_authoring_read_string(
            path,
            "pack_provenance",
            "provenance_kind",
            provenance_out->provenance_kind,
            (unsigned int)sizeof(provenance_out->provenance_kind)) ||
        !plasma_authoring_read_string(
            path,
            "pack_provenance",
            "summary",
            provenance_out->summary,
            (unsigned int)sizeof(provenance_out->summary)) ||
        !plasma_authoring_read_string(
            path,
            "compatibility",
            "product_key",
            provenance_out->product_key,
            (unsigned int)sizeof(provenance_out->product_key)) ||
        !plasma_authoring_read_unsigned_long(
            path,
            "compatibility",
            "pack_schema_version",
            &provenance_out->pack_schema_version) ||
        !plasma_authoring_read_string(
            path,
            "compatibility",
            "minimum_kind",
            provenance_out->minimum_kind,
            (unsigned int)sizeof(provenance_out->minimum_kind)) ||
        !plasma_authoring_read_string(
            path,
            "compatibility",
            "preferred_kind",
            provenance_out->preferred_kind,
            (unsigned int)sizeof(provenance_out->preferred_kind)) ||
        !plasma_authoring_read_string(
            path,
            "compatibility",
            "quality_class",
            provenance_out->quality_class,
            (unsigned int)sizeof(provenance_out->quality_class)) ||
        !plasma_authoring_read_string(
            path,
            "compatibility",
            "migration_policy",
            provenance_out->migration_policy,
            (unsigned int)sizeof(provenance_out->migration_policy)) ||
        !plasma_authoring_read_flag(
            path,
            "compatibility",
            "preserves_classic_identity",
            &provenance_out->preserves_classic_identity) ||
        !plasma_authoring_read_string(
            path,
            "compatibility",
            "compatibility_notes",
            provenance_out->compatibility_notes,
            (unsigned int)sizeof(provenance_out->compatibility_notes))
    ) {
        return 0;
    }

    return 1;
}

static int plasma_authoring_compare_preset_set(
    const plasma_authored_preset_set *authored,
    const plasma_content_preset_set_entry *compiled,
    char *message_out,
    unsigned int message_size
)
{
    unsigned int index;

    if (
        authored == NULL ||
        compiled == NULL ||
        strcmp(authored->set_key, compiled->set_key) != 0 ||
        !plasma_authoring_profile_matches_channel(authored->profile_scope, compiled->channel_scope) ||
        authored->member_count != compiled->member_count
    ) {
        plasma_authoring_set_message(
            message_out,
            message_size,
            "Preset-set authoring data does not match the compiled Plasma registry.");
        return 0;
    }

    for (index = 0U; index < authored->member_count; ++index) {
        if (
            strcmp(authored->members[index].content_key, compiled->members[index].content_key) != 0 ||
            authored->members[index].weight != compiled->members[index].weight
        ) {
            plasma_authoring_set_message(
                message_out,
                message_size,
                "Preset-set members or weights do not match the compiled Plasma registry.");
            return 0;
        }
    }

    return 1;
}

static int plasma_authoring_compare_theme_set(
    const plasma_authored_theme_set *authored,
    const plasma_content_theme_set_entry *compiled,
    char *message_out,
    unsigned int message_size
)
{
    unsigned int index;

    if (
        authored == NULL ||
        compiled == NULL ||
        strcmp(authored->set_key, compiled->set_key) != 0 ||
        !plasma_authoring_profile_matches_channel(authored->profile_scope, compiled->channel_scope) ||
        authored->member_count != compiled->member_count
    ) {
        plasma_authoring_set_message(
            message_out,
            message_size,
            "Theme-set authoring data does not match the compiled Plasma registry.");
        return 0;
    }

    for (index = 0U; index < authored->member_count; ++index) {
        if (
            strcmp(authored->members[index].content_key, compiled->members[index].content_key) != 0 ||
            authored->members[index].weight != compiled->members[index].weight
        ) {
            plasma_authoring_set_message(
                message_out,
                message_size,
                "Theme-set members or weights do not match the compiled Plasma registry.");
            return 0;
        }
    }

    return 1;
}

static int plasma_authoring_compare_journey(
    const plasma_authored_journey *authored,
    const plasma_content_journey_entry *compiled,
    char *message_out,
    unsigned int message_size
)
{
    unsigned int index;

    if (
        authored == NULL ||
        compiled == NULL ||
        strcmp(authored->journey_key, compiled->journey_key) != 0 ||
        authored->step_count != compiled->step_count
    ) {
        plasma_authoring_set_message(
            message_out,
            message_size,
            "Journey authoring data does not match the compiled Plasma transition surface.");
        return 0;
    }

    for (index = 0U; index < authored->step_count; ++index) {
        if (
            strcmp(authored->steps[index].preset_set_key, compiled->steps[index].preset_set_key) != 0 ||
            strcmp(authored->steps[index].theme_set_key, compiled->steps[index].theme_set_key) != 0 ||
            authored->steps[index].policy != compiled->steps[index].policy ||
            authored->steps[index].dwell_millis != compiled->steps[index].dwell_millis
        ) {
            plasma_authoring_set_message(
                message_out,
                message_size,
                "Journey steps do not match the compiled Plasma transition surface.");
            return 0;
        }
    }

    return 1;
}

static int plasma_authoring_validate_pack_provenance(
    const char *repo_root,
    char *message_out,
    unsigned int message_size
)
{
    plasma_authored_pack_provenance provenance;
    screensave_pack_manifest manifest;
    char path[MAX_PATH];

    if (!plasma_authoring_build_path(
            repo_root,
            "products\\savers\\plasma\\packs\\lava_remix\\pack.provenance.ini",
            path,
            (unsigned int)sizeof(path))) {
        plasma_authoring_set_message(message_out, message_size, "Could not build the Plasma provenance file path.");
        return 0;
    }
    if (!plasma_authoring_load_pack_provenance_file(path, &provenance)) {
        plasma_authoring_set_message(message_out, message_size, "Could not load the Plasma pack provenance file.");
        return 0;
    }
    if (
        strcmp(provenance.pack_key, "lava_remix") != 0 ||
        strcmp(provenance.channel, "stable") != 0 ||
        strcmp(provenance.source_kind, "built_in") != 0 ||
        strcmp(provenance.product_key, "plasma") != 0 ||
        provenance.pack_schema_version != 1UL ||
        !provenance.preserves_classic_identity
    ) {
        plasma_authoring_set_message(
            message_out,
            message_size,
            "Pack provenance fields do not match the expected Wave 2 Plasma-local contract.");
        return 0;
    }

    ZeroMemory(&manifest, sizeof(manifest));
    if (
        !plasma_authoring_build_path(
            repo_root,
            "products\\savers\\plasma\\packs\\lava_remix\\pack.ini",
            path,
            (unsigned int)sizeof(path)) ||
        !screensave_pack_manifest_load(path, SCREENSAVE_PACK_SOURCE_BUILTIN, &manifest, NULL)
    ) {
        plasma_authoring_set_message(
            message_out,
            message_size,
            "Could not load the shared lava_remix pack manifest while validating Plasma provenance.");
        return 0;
    }

    if (
        strcmp(provenance.pack_key, manifest.pack_key) != 0 ||
        strcmp(provenance.product_key, manifest.product_key) != 0 ||
        strcmp(provenance.minimum_kind, manifest.minimum_kind) != 0 ||
        strcmp(provenance.preferred_kind, manifest.preferred_kind) != 0 ||
        strcmp(provenance.quality_class, manifest.quality_class) != 0
    ) {
        plasma_authoring_set_message(
            message_out,
            message_size,
            "Pack provenance compatibility fields do not match the shared lava_remix manifest.");
        return 0;
    }

    return 1;
}

int plasma_authoring_validate_repo_surface(
    const char *repo_root,
    char *message_out,
    unsigned int message_size
)
{
    const plasma_content_registry *registry;
    const plasma_content_journey_entry *journeys;
    unsigned int journey_count;
    unsigned int index;
    char path[MAX_PATH];

    registry = plasma_content_get_registry();
    journeys = plasma_transition_get_journeys(&journey_count);
    if (
        repo_root == NULL ||
        repo_root[0] == '\0' ||
        registry == NULL ||
        journeys == NULL
    ) {
        plasma_authoring_set_message(
            message_out,
            message_size,
            "The Plasma authoring surface could not validate because its repo root or runtime catalog was unavailable.");
        return 0;
    }

    if (
        registry->preset_set_count !=
            (unsigned int)(sizeof(g_expected_preset_set_files) / sizeof(g_expected_preset_set_files[0])) ||
        registry->theme_set_count !=
            (unsigned int)(sizeof(g_expected_theme_set_files) / sizeof(g_expected_theme_set_files[0])) ||
        journey_count !=
            (unsigned int)(sizeof(g_expected_journey_files) / sizeof(g_expected_journey_files[0]))
    ) {
        plasma_authoring_set_message(
            message_out,
            message_size,
            "The compiled Plasma catalog no longer matches the expected PX20 authored file coverage.");
        return 0;
    }

    if (!plasma_authoring_validate_pack_provenance(repo_root, message_out, message_size)) {
        return 0;
    }

    for (index = 0U; index < registry->preset_set_count; ++index) {
        plasma_authored_preset_set authored_set;
        const plasma_content_preset_set_entry *compiled_set;

        if (
            !plasma_authoring_build_path(
                repo_root,
                g_expected_preset_set_files[index].relative_path,
                path,
                (unsigned int)sizeof(path)) ||
            !plasma_authoring_load_preset_set_file(path, &authored_set)
        ) {
            plasma_authoring_set_message(message_out, message_size, "Could not load a Plasma authored preset-set file.");
            return 0;
        }

        compiled_set = plasma_content_find_preset_set(g_expected_preset_set_files[index].key);
        if (
            compiled_set == NULL ||
            !plasma_authoring_compare_preset_set(&authored_set, compiled_set, message_out, message_size)
        ) {
            return 0;
        }
    }

    for (index = 0U; index < registry->theme_set_count; ++index) {
        plasma_authored_theme_set authored_set;
        const plasma_content_theme_set_entry *compiled_set;

        if (
            !plasma_authoring_build_path(
                repo_root,
                g_expected_theme_set_files[index].relative_path,
                path,
                (unsigned int)sizeof(path)) ||
            !plasma_authoring_load_theme_set_file(path, &authored_set)
        ) {
            plasma_authoring_set_message(message_out, message_size, "Could not load a Plasma authored theme-set file.");
            return 0;
        }

        compiled_set = plasma_content_find_theme_set(g_expected_theme_set_files[index].key);
        if (
            compiled_set == NULL ||
            !plasma_authoring_compare_theme_set(&authored_set, compiled_set, message_out, message_size)
        ) {
            return 0;
        }
    }

    for (index = 0U; index < journey_count; ++index) {
        plasma_authored_journey authored_journey;
        const plasma_content_journey_entry *compiled_journey;

        if (
            !plasma_authoring_build_path(
                repo_root,
                g_expected_journey_files[index].relative_path,
                path,
                (unsigned int)sizeof(path)) ||
            !plasma_authoring_load_journey_file(path, &authored_journey)
        ) {
            plasma_authoring_set_message(message_out, message_size, "Could not load a Plasma authored journey file.");
            return 0;
        }

        compiled_journey = plasma_transition_find_journey(g_expected_journey_files[index].key);
        if (
            compiled_journey == NULL ||
            !plasma_authoring_compare_journey(
                &authored_journey,
                compiled_journey,
                message_out,
                message_size)
        ) {
            return 0;
        }
    }

    plasma_authoring_set_message(message_out, message_size, "PX20 authored Plasma substrate validated.");
    return 1;
}
