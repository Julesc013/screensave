#include <string.h>

#include "plasma_internal.h"

static const plasma_content_pack_asset g_lava_remix_assets[] = {
    { SCREENSAVE_SETTINGS_FILE_PRESET, "presets/lava_remix.preset.ini", "plasma_lava" },
    { SCREENSAVE_SETTINGS_FILE_THEME, "themes/lava_remix.theme.ini", "plasma_lava" }
};

static const plasma_content_weighted_key g_classic_core_preset_members[] = {
    { "plasma_lava", 4U },
    { "aurora_plasma", 2U },
    { "ocean_interference", 2U },
    { "museum_phosphor", 2U },
    { "quiet_darkroom", 3U }
};

static const plasma_content_weighted_key g_dark_room_preset_members[] = {
    { "quiet_darkroom", 4U },
    { "museum_phosphor", 3U }
};

static const plasma_content_weighted_key g_fire_classics_preset_members[] = {
    { "plasma_lava", 3U },
    { "quiet_darkroom", 2U }
};

static const plasma_content_weighted_key g_plasma_classics_preset_members[] = {
    { "aurora_plasma", 3U },
    { "museum_phosphor", 2U }
};

static const plasma_content_weighted_key g_interference_classics_preset_members[] = {
    { "ocean_interference", 3U }
};

static const plasma_content_weighted_key g_warm_bridge_classics_preset_members[] = {
    { "plasma_lava", 3U },
    { "quiet_darkroom", 3U },
    { "museum_phosphor", 2U }
};

static const plasma_content_weighted_key g_cool_bridge_classics_preset_members[] = {
    { "aurora_plasma", 3U },
    { "ocean_interference", 3U }
};

static const plasma_content_weighted_key g_classic_core_theme_members[] = {
    { "plasma_lava", 4U },
    { "aurora_cool", 2U },
    { "oceanic_blue", 2U },
    { "museum_phosphor", 2U },
    { "quiet_darkroom", 3U }
};

static const plasma_content_weighted_key g_dark_room_theme_members[] = {
    { "museum_phosphor", 3U },
    { "quiet_darkroom", 4U }
};

static const plasma_content_weighted_key g_warm_classics_theme_members[] = {
    { "plasma_lava", 3U },
    { "quiet_darkroom", 2U }
};

static const plasma_content_weighted_key g_cool_classics_theme_members[] = {
    { "aurora_cool", 3U },
    { "oceanic_blue", 3U }
};

static const plasma_content_preset_entry g_preset_entries[] = {
    {
        "plasma_lava",
        &g_plasma_presets[0],
        PLASMA_CONTENT_SOURCE_BUILT_IN,
        PLASMA_CONTENT_CHANNEL_STABLE,
        PLASMA_PRESET_MORPH_CLASS_FIRE,
        PLASMA_TRANSITION_BRIDGE_CLASS_WARM_CLASSIC,
        1,
        1,
        1,
        NULL
    },
    {
        "aurora_plasma",
        &g_plasma_presets[1],
        PLASMA_CONTENT_SOURCE_BUILT_IN,
        PLASMA_CONTENT_CHANNEL_STABLE,
        PLASMA_PRESET_MORPH_CLASS_PLASMA,
        PLASMA_TRANSITION_BRIDGE_CLASS_COOL_FIELD,
        1,
        1,
        1,
        NULL
    },
    {
        "ocean_interference",
        &g_plasma_presets[2],
        PLASMA_CONTENT_SOURCE_BUILT_IN,
        PLASMA_CONTENT_CHANNEL_STABLE,
        PLASMA_PRESET_MORPH_CLASS_INTERFERENCE,
        PLASMA_TRANSITION_BRIDGE_CLASS_COOL_FIELD,
        1,
        1,
        1,
        NULL
    },
    {
        "museum_phosphor",
        &g_plasma_presets[3],
        PLASMA_CONTENT_SOURCE_BUILT_IN,
        PLASMA_CONTENT_CHANNEL_STABLE,
        PLASMA_PRESET_MORPH_CLASS_PLASMA,
        PLASMA_TRANSITION_BRIDGE_CLASS_WARM_CLASSIC,
        1,
        1,
        1,
        NULL
    },
    {
        "quiet_darkroom",
        &g_plasma_presets[4],
        PLASMA_CONTENT_SOURCE_BUILT_IN,
        PLASMA_CONTENT_CHANNEL_STABLE,
        PLASMA_PRESET_MORPH_CLASS_FIRE,
        PLASMA_TRANSITION_BRIDGE_CLASS_WARM_CLASSIC,
        1,
        1,
        1,
        NULL
    },
    {
        "midnight_interference",
        &g_plasma_presets[5],
        PLASMA_CONTENT_SOURCE_BUILT_IN,
        PLASMA_CONTENT_CHANNEL_EXPERIMENTAL,
        PLASMA_PRESET_MORPH_CLASS_INTERFERENCE,
        PLASMA_TRANSITION_BRIDGE_CLASS_COOL_FIELD,
        1,
        1,
        1,
        NULL
    },
    {
        "amber_terminal",
        &g_plasma_presets[6],
        PLASMA_CONTENT_SOURCE_BUILT_IN,
        PLASMA_CONTENT_CHANNEL_EXPERIMENTAL,
        PLASMA_PRESET_MORPH_CLASS_PLASMA,
        PLASMA_TRANSITION_BRIDGE_CLASS_WARM_CLASSIC,
        1,
        1,
        1,
        NULL
    },
    {
        "lava_isolines",
        &g_plasma_presets[7],
        PLASMA_CONTENT_SOURCE_BUILT_IN,
        PLASMA_CONTENT_CHANNEL_EXPERIMENTAL,
        PLASMA_PRESET_MORPH_CLASS_FIRE,
        PLASMA_TRANSITION_BRIDGE_CLASS_WARM_CLASSIC,
        1,
        1,
        0,
        NULL
    },
    {
        "aurora_bands",
        &g_plasma_presets[8],
        PLASMA_CONTENT_SOURCE_BUILT_IN,
        PLASMA_CONTENT_CHANNEL_EXPERIMENTAL,
        PLASMA_PRESET_MORPH_CLASS_PLASMA,
        PLASMA_TRANSITION_BRIDGE_CLASS_COOL_FIELD,
        1,
        1,
        0,
        NULL
    },
    {
        "wire_glow",
        &g_plasma_presets[9],
        PLASMA_CONTENT_SOURCE_BUILT_IN,
        PLASMA_CONTENT_CHANNEL_EXPERIMENTAL,
        PLASMA_PRESET_MORPH_CLASS_INTERFERENCE,
        PLASMA_TRANSITION_BRIDGE_CLASS_COOL_FIELD,
        1,
        1,
        0,
        NULL
    },
    {
        "phosphor_topography",
        &g_plasma_presets[10],
        PLASMA_CONTENT_SOURCE_BUILT_IN,
        PLASMA_CONTENT_CHANNEL_EXPERIMENTAL,
        PLASMA_PRESET_MORPH_CLASS_PLASMA,
        PLASMA_TRANSITION_BRIDGE_CLASS_WARM_CLASSIC,
        1,
        1,
        0,
        NULL
    },
    {
        "stipple_bands",
        &g_plasma_presets[11],
        PLASMA_CONTENT_SOURCE_BUILT_IN,
        PLASMA_CONTENT_CHANNEL_EXPERIMENTAL,
        PLASMA_PRESET_MORPH_CLASS_FIRE,
        PLASMA_TRANSITION_BRIDGE_CLASS_WARM_CLASSIC,
        1,
        1,
        0,
        NULL
    },
    {
        "emboss_current",
        &g_plasma_presets[12],
        PLASMA_CONTENT_SOURCE_BUILT_IN,
        PLASMA_CONTENT_CHANNEL_EXPERIMENTAL,
        PLASMA_PRESET_MORPH_CLASS_INTERFERENCE,
        PLASMA_TRANSITION_BRIDGE_CLASS_COOL_FIELD,
        1,
        1,
        0,
        NULL
    },
    {
        "crt_signal_bands",
        &g_plasma_presets[13],
        PLASMA_CONTENT_SOURCE_BUILT_IN,
        PLASMA_CONTENT_CHANNEL_EXPERIMENTAL,
        PLASMA_PRESET_MORPH_CLASS_PLASMA,
        PLASMA_TRANSITION_BRIDGE_CLASS_WARM_CLASSIC,
        1,
        1,
        0,
        NULL
    },
    {
        "ascii_reactor",
        &g_plasma_presets[14],
        PLASMA_CONTENT_SOURCE_BUILT_IN,
        PLASMA_CONTENT_CHANNEL_EXPERIMENTAL,
        PLASMA_PRESET_MORPH_CLASS_NONE,
        PLASMA_TRANSITION_BRIDGE_CLASS_NONE,
        1,
        1,
        0,
        NULL
    },
    {
        "matrix_lattice",
        &g_plasma_presets[15],
        PLASMA_CONTENT_SOURCE_BUILT_IN,
        PLASMA_CONTENT_CHANNEL_EXPERIMENTAL,
        PLASMA_PRESET_MORPH_CLASS_NONE,
        PLASMA_TRANSITION_BRIDGE_CLASS_NONE,
        1,
        1,
        0,
        NULL
    },
    {
        "cellular_bloom",
        &g_plasma_presets[16],
        PLASMA_CONTENT_SOURCE_BUILT_IN,
        PLASMA_CONTENT_CHANNEL_EXPERIMENTAL,
        PLASMA_PRESET_MORPH_CLASS_NONE,
        PLASMA_TRANSITION_BRIDGE_CLASS_NONE,
        1,
        1,
        0,
        NULL
    },
    {
        "quasi_crystal_bands",
        &g_plasma_presets[17],
        PLASMA_CONTENT_SOURCE_BUILT_IN,
        PLASMA_CONTENT_CHANNEL_EXPERIMENTAL,
        PLASMA_PRESET_MORPH_CLASS_NONE,
        PLASMA_TRANSITION_BRIDGE_CLASS_NONE,
        1,
        1,
        0,
        NULL
    },
    {
        "caustic_waterlight",
        &g_plasma_presets[18],
        PLASMA_CONTENT_SOURCE_BUILT_IN,
        PLASMA_CONTENT_CHANNEL_EXPERIMENTAL,
        PLASMA_PRESET_MORPH_CLASS_NONE,
        PLASMA_TRANSITION_BRIDGE_CLASS_NONE,
        1,
        1,
        0,
        NULL
    },
    {
        "aurora_curtain",
        &g_plasma_presets[19],
        PLASMA_CONTENT_SOURCE_BUILT_IN,
        PLASMA_CONTENT_CHANNEL_EXPERIMENTAL,
        PLASMA_PRESET_MORPH_CLASS_NONE,
        PLASMA_TRANSITION_BRIDGE_CLASS_NONE,
        1,
        1,
        1,
        NULL
    },
    {
        "ribbon_aurora",
        &g_plasma_presets[20],
        PLASMA_CONTENT_SOURCE_BUILT_IN,
        PLASMA_CONTENT_CHANNEL_EXPERIMENTAL,
        PLASMA_PRESET_MORPH_CLASS_NONE,
        PLASMA_TRANSITION_BRIDGE_CLASS_NONE,
        1,
        1,
        1,
        NULL
    },
    {
        "substrate_relief",
        &g_plasma_presets[21],
        PLASMA_CONTENT_SOURCE_BUILT_IN,
        PLASMA_CONTENT_CHANNEL_EXPERIMENTAL,
        PLASMA_PRESET_MORPH_CLASS_NONE,
        PLASMA_TRANSITION_BRIDGE_CLASS_NONE,
        1,
        1,
        1,
        NULL
    },
    {
        "filament_extrusion",
        &g_plasma_presets[22],
        PLASMA_CONTENT_SOURCE_BUILT_IN,
        PLASMA_CONTENT_CHANNEL_EXPERIMENTAL,
        PLASMA_PRESET_MORPH_CLASS_NONE,
        PLASMA_TRANSITION_BRIDGE_CLASS_NONE,
        1,
        1,
        1,
        NULL
    }
};

static const plasma_content_theme_entry g_theme_entries[] = {
    {
        "plasma_lava",
        &g_plasma_themes[0],
        PLASMA_CONTENT_SOURCE_BUILT_IN,
        PLASMA_CONTENT_CHANNEL_STABLE,
        PLASMA_THEME_MORPH_CLASS_WARM,
        NULL
    },
    {
        "aurora_cool",
        &g_plasma_themes[1],
        PLASMA_CONTENT_SOURCE_BUILT_IN,
        PLASMA_CONTENT_CHANNEL_STABLE,
        PLASMA_THEME_MORPH_CLASS_COOL,
        NULL
    },
    {
        "oceanic_blue",
        &g_plasma_themes[2],
        PLASMA_CONTENT_SOURCE_BUILT_IN,
        PLASMA_CONTENT_CHANNEL_STABLE,
        PLASMA_THEME_MORPH_CLASS_COOL,
        NULL
    },
    {
        "museum_phosphor",
        &g_plasma_themes[3],
        PLASMA_CONTENT_SOURCE_BUILT_IN,
        PLASMA_CONTENT_CHANNEL_STABLE,
        PLASMA_THEME_MORPH_CLASS_PHOSPHOR,
        NULL
    },
    {
        "quiet_darkroom",
        &g_plasma_themes[4],
        PLASMA_CONTENT_SOURCE_BUILT_IN,
        PLASMA_CONTENT_CHANNEL_STABLE,
        PLASMA_THEME_MORPH_CLASS_WARM,
        NULL
    },
    {
        "midnight_interference",
        &g_plasma_themes[5],
        PLASMA_CONTENT_SOURCE_BUILT_IN,
        PLASMA_CONTENT_CHANNEL_EXPERIMENTAL,
        PLASMA_THEME_MORPH_CLASS_COOL,
        NULL
    },
    {
        "amber_terminal",
        &g_plasma_themes[6],
        PLASMA_CONTENT_SOURCE_BUILT_IN,
        PLASMA_CONTENT_CHANNEL_EXPERIMENTAL,
        PLASMA_THEME_MORPH_CLASS_WARM,
        NULL
    }
};

static const plasma_content_pack_entry g_pack_entries[] = {
    {
        "lava_remix",
        "Lava Remix",
        "Alternate plasma studies with brighter lava tones and faster palette travel.",
        PLASMA_CONTENT_SOURCE_PACK,
        PLASMA_CONTENT_CHANNEL_STABLE,
        SCREENSAVE_RENDERER_KIND_GDI,
        SCREENSAVE_RENDERER_KIND_GL11,
        SCREENSAVE_CAPABILITY_QUALITY_BALANCED,
        "Retains the lava studies on lower lanes with coarser motion and composition updates.",
        g_lava_remix_assets,
        (unsigned int)(sizeof(g_lava_remix_assets) / sizeof(g_lava_remix_assets[0]))
    }
};

static const plasma_content_preset_set_entry g_preset_sets[] = {
    {
        "classic_core",
        "Classic Core",
        "All preserved built-in Plasma Classic presets.",
        PLASMA_CONTENT_CHANNEL_STABLE,
        g_classic_core_preset_members,
        (unsigned int)(sizeof(g_classic_core_preset_members) / sizeof(g_classic_core_preset_members[0]))
    },
    {
        "dark_room_classics",
        "Dark-Room Classics",
        "Quiet long-run preset subset for subdued rooms and museum-style sessions.",
        PLASMA_CONTENT_CHANNEL_STABLE,
        g_dark_room_preset_members,
        (unsigned int)(sizeof(g_dark_room_preset_members) / sizeof(g_dark_room_preset_members[0]))
    },
    {
        "fire_classics",
        "Fire Classics",
        "Compatible fire-family classic presets for direct preset morph coverage.",
        PLASMA_CONTENT_CHANNEL_STABLE,
        g_fire_classics_preset_members,
        (unsigned int)(sizeof(g_fire_classics_preset_members) / sizeof(g_fire_classics_preset_members[0]))
    },
    {
        "plasma_classics",
        "Plasma Classics",
        "Compatible plasma-family classic presets for direct preset morph coverage.",
        PLASMA_CONTENT_CHANNEL_STABLE,
        g_plasma_classics_preset_members,
        (unsigned int)(sizeof(g_plasma_classics_preset_members) / sizeof(g_plasma_classics_preset_members[0]))
    },
    {
        "interference_classics",
        "Interference Classics",
        "Compatible interference-family classic presets for direct preset morph coverage.",
        PLASMA_CONTENT_CHANNEL_STABLE,
        g_interference_classics_preset_members,
        (unsigned int)(sizeof(g_interference_classics_preset_members) / sizeof(g_interference_classics_preset_members[0]))
    },
    {
        "warm_bridge_classics",
        "Warm Bridge Classics",
        "Curated warm-theme classic presets that alternate between direct fire or plasma morphs and bounded warm-bridge crossings.",
        PLASMA_CONTENT_CHANNEL_STABLE,
        g_warm_bridge_classics_preset_members,
        (unsigned int)(sizeof(g_warm_bridge_classics_preset_members) / sizeof(g_warm_bridge_classics_preset_members[0]))
    },
    {
        "cool_bridge_classics",
        "Cool Bridge Classics",
        "Curated cool-theme classic presets that alternate between direct interference morphs and bounded cool-field bridge crossings.",
        PLASMA_CONTENT_CHANNEL_STABLE,
        g_cool_bridge_classics_preset_members,
        (unsigned int)(sizeof(g_cool_bridge_classics_preset_members) / sizeof(g_cool_bridge_classics_preset_members[0]))
    }
};

static const plasma_content_theme_set_entry g_theme_sets[] = {
    {
        "classic_core",
        "Classic Core",
        "All preserved built-in Plasma Classic themes.",
        PLASMA_CONTENT_CHANNEL_STABLE,
        g_classic_core_theme_members,
        (unsigned int)(sizeof(g_classic_core_theme_members) / sizeof(g_classic_core_theme_members[0]))
    },
    {
        "dark_room_classics",
        "Dark-Room Classics",
        "Quiet theme subset for dark-room-safe classic presentation.",
        PLASMA_CONTENT_CHANNEL_STABLE,
        g_dark_room_theme_members,
        (unsigned int)(sizeof(g_dark_room_theme_members) / sizeof(g_dark_room_theme_members[0]))
    },
    {
        "warm_classics",
        "Warm Classics",
        "Compatible warm-family themes for direct theme morph coverage.",
        PLASMA_CONTENT_CHANNEL_STABLE,
        g_warm_classics_theme_members,
        (unsigned int)(sizeof(g_warm_classics_theme_members) / sizeof(g_warm_classics_theme_members[0]))
    },
    {
        "cool_classics",
        "Cool Classics",
        "Compatible cool-family themes for direct theme morph coverage.",
        PLASMA_CONTENT_CHANNEL_STABLE,
        g_cool_classics_theme_members,
        (unsigned int)(sizeof(g_cool_classics_theme_members) / sizeof(g_cool_classics_theme_members[0]))
    }
};

static const plasma_content_registry g_registry = {
    g_preset_entries,
    (unsigned int)(sizeof(g_preset_entries) / sizeof(g_preset_entries[0])),
    g_theme_entries,
    (unsigned int)(sizeof(g_theme_entries) / sizeof(g_theme_entries[0])),
    g_pack_entries,
    (unsigned int)(sizeof(g_pack_entries) / sizeof(g_pack_entries[0])),
    g_preset_sets,
    (unsigned int)(sizeof(g_preset_sets) / sizeof(g_preset_sets[0])),
    g_theme_sets,
    (unsigned int)(sizeof(g_theme_sets) / sizeof(g_theme_sets[0]))
};

static int plasma_content_weighted_key_list_is_valid(
    const plasma_content_weighted_key *members,
    unsigned int member_count
)
{
    unsigned int index;

    if (members == NULL || member_count == 0U) {
        return 0;
    }

    for (index = 0U; index < member_count; ++index) {
        if (
            members[index].content_key == NULL ||
            members[index].content_key[0] == '\0' ||
            members[index].weight == 0U
        ) {
            return 0;
        }
    }

    return 1;
}

const plasma_content_registry *plasma_content_get_registry(void)
{
    return &g_registry;
}

int plasma_content_registry_validate(void)
{
    const plasma_content_registry *registry;
    unsigned int index;

    registry = plasma_content_get_registry();
    if (
        registry == NULL ||
        registry->preset_entries == NULL ||
        registry->theme_entries == NULL ||
        registry->pack_entries == NULL ||
        registry->preset_sets == NULL ||
        registry->theme_sets == NULL ||
        registry->preset_count != PLASMA_PRESET_COUNT ||
        registry->theme_count != PLASMA_THEME_COUNT
    ) {
        return 0;
    }

    for (index = 0U; index < registry->preset_count; ++index) {
        if (
            registry->preset_entries[index].preset_key == NULL ||
            registry->preset_entries[index].descriptor == NULL ||
            registry->preset_entries[index].descriptor->theme_key == NULL ||
            registry->preset_entries[index].morph_class < PLASMA_PRESET_MORPH_CLASS_NONE ||
            registry->preset_entries[index].morph_class > PLASMA_PRESET_MORPH_CLASS_INTERFERENCE ||
            registry->preset_entries[index].bridge_class < PLASMA_TRANSITION_BRIDGE_CLASS_NONE ||
            registry->preset_entries[index].bridge_class > PLASMA_TRANSITION_BRIDGE_CLASS_COOL_FIELD ||
            (registry->preset_entries[index].advanced_capable != 0 &&
                registry->preset_entries[index].advanced_capable != 1) ||
            (registry->preset_entries[index].modern_capable != 0 &&
                registry->preset_entries[index].modern_capable != 1) ||
            (registry->preset_entries[index].premium_capable != 0 &&
                registry->preset_entries[index].premium_capable != 1) ||
            strcmp(
                registry->preset_entries[index].preset_key,
                registry->preset_entries[index].descriptor->preset_key
            ) != 0 ||
            plasma_find_preset_values(registry->preset_entries[index].preset_key) == NULL ||
            plasma_find_theme_descriptor(registry->preset_entries[index].descriptor->theme_key) == NULL
        ) {
            return 0;
        }
    }

    for (index = 0U; index < registry->theme_count; ++index) {
        if (
            registry->theme_entries[index].theme_key == NULL ||
            registry->theme_entries[index].descriptor == NULL ||
            registry->theme_entries[index].morph_class < PLASMA_THEME_MORPH_CLASS_NONE ||
            registry->theme_entries[index].morph_class > PLASMA_THEME_MORPH_CLASS_PHOSPHOR ||
            strcmp(
                registry->theme_entries[index].theme_key,
                registry->theme_entries[index].descriptor->theme_key
            ) != 0
        ) {
            return 0;
        }
    }

    for (index = 0U; index < registry->pack_count; ++index) {
        unsigned int asset_index;

        if (
            registry->pack_entries[index].pack_key == NULL ||
            registry->pack_entries[index].display_name == NULL ||
            registry->pack_entries[index].assets == NULL ||
            registry->pack_entries[index].asset_count == 0U
        ) {
            return 0;
        }

        for (asset_index = 0U; asset_index < registry->pack_entries[index].asset_count; ++asset_index) {
            const plasma_content_pack_asset *asset;

            asset = &registry->pack_entries[index].assets[asset_index];
            if (
                asset->relative_path == NULL ||
                asset->relative_path[0] == '\0' ||
                asset->canonical_key == NULL ||
                asset->canonical_key[0] == '\0'
            ) {
                return 0;
            }

            if (asset->file_kind == SCREENSAVE_SETTINGS_FILE_PRESET) {
                if (plasma_content_find_preset_entry(asset->canonical_key) == NULL) {
                    return 0;
                }
            } else if (asset->file_kind == SCREENSAVE_SETTINGS_FILE_THEME) {
                if (plasma_content_find_theme_entry(asset->canonical_key) == NULL) {
                    return 0;
                }
            } else {
                return 0;
            }
        }
    }

    for (index = 0U; index < registry->preset_set_count; ++index) {
        unsigned int member_index;

        if (
            registry->preset_sets[index].set_key == NULL ||
            registry->preset_sets[index].display_name == NULL ||
            !plasma_content_weighted_key_list_is_valid(
                registry->preset_sets[index].members,
                registry->preset_sets[index].member_count
            )
        ) {
            return 0;
        }

        for (member_index = 0U; member_index < registry->preset_sets[index].member_count; ++member_index) {
            if (plasma_content_find_preset_entry(registry->preset_sets[index].members[member_index].content_key) == NULL) {
                return 0;
            }
        }
    }

    for (index = 0U; index < registry->theme_set_count; ++index) {
        unsigned int member_index;

        if (
            registry->theme_sets[index].set_key == NULL ||
            registry->theme_sets[index].display_name == NULL ||
            !plasma_content_weighted_key_list_is_valid(
                registry->theme_sets[index].members,
                registry->theme_sets[index].member_count
            )
        ) {
            return 0;
        }

        for (member_index = 0U; member_index < registry->theme_sets[index].member_count; ++member_index) {
            if (plasma_content_find_theme_entry(registry->theme_sets[index].members[member_index].content_key) == NULL) {
                return 0;
            }
        }
    }

    return 1;
}

const plasma_content_preset_entry *plasma_content_find_preset_entry(const char *preset_key)
{
    const plasma_content_registry *registry;
    unsigned int index;

    registry = plasma_content_get_registry();
    preset_key = plasma_canonical_content_key(preset_key);
    if (preset_key == NULL || preset_key[0] == '\0') {
        return NULL;
    }

    for (index = 0U; index < registry->preset_count; ++index) {
        if (strcmp(registry->preset_entries[index].preset_key, preset_key) == 0) {
            return &registry->preset_entries[index];
        }
    }

    return NULL;
}

const plasma_content_theme_entry *plasma_content_find_theme_entry(const char *theme_key)
{
    const plasma_content_registry *registry;
    unsigned int index;

    registry = plasma_content_get_registry();
    theme_key = plasma_canonical_content_key(theme_key);
    if (theme_key == NULL || theme_key[0] == '\0') {
        return NULL;
    }

    for (index = 0U; index < registry->theme_count; ++index) {
        if (strcmp(registry->theme_entries[index].theme_key, theme_key) == 0) {
            return &registry->theme_entries[index];
        }
    }

    return NULL;
}

const plasma_content_pack_entry *plasma_content_find_pack_entry(const char *pack_key)
{
    const plasma_content_registry *registry;
    unsigned int index;

    registry = plasma_content_get_registry();
    if (pack_key == NULL || pack_key[0] == '\0') {
        return NULL;
    }

    for (index = 0U; index < registry->pack_count; ++index) {
        if (strcmp(registry->pack_entries[index].pack_key, pack_key) == 0) {
            return &registry->pack_entries[index];
        }
    }

    return NULL;
}

const plasma_content_preset_set_entry *plasma_content_find_preset_set(const char *set_key)
{
    const plasma_content_registry *registry;
    unsigned int index;

    registry = plasma_content_get_registry();
    if (set_key == NULL || set_key[0] == '\0') {
        return NULL;
    }

    for (index = 0U; index < registry->preset_set_count; ++index) {
        if (strcmp(registry->preset_sets[index].set_key, set_key) == 0) {
            return &registry->preset_sets[index];
        }
    }

    return NULL;
}

const plasma_content_theme_set_entry *plasma_content_find_theme_set(const char *set_key)
{
    const plasma_content_registry *registry;
    unsigned int index;

    registry = plasma_content_get_registry();
    if (set_key == NULL || set_key[0] == '\0') {
        return NULL;
    }

    for (index = 0U; index < registry->theme_set_count; ++index) {
        if (strcmp(registry->theme_sets[index].set_key, set_key) == 0) {
            return &registry->theme_sets[index];
        }
    }

    return NULL;
}

int plasma_content_preset_index(const plasma_content_preset_entry *entry)
{
    const plasma_content_registry *registry;
    unsigned int index;

    registry = plasma_content_get_registry();
    if (entry == NULL) {
        return -1;
    }

    for (index = 0U; index < registry->preset_count; ++index) {
        if (&registry->preset_entries[index] == entry) {
            return (int)index;
        }
    }

    return -1;
}

int plasma_content_theme_index(const plasma_content_theme_entry *entry)
{
    const plasma_content_registry *registry;
    unsigned int index;

    registry = plasma_content_get_registry();
    if (entry == NULL) {
        return -1;
    }

    for (index = 0U; index < registry->theme_count; ++index) {
        if (&registry->theme_entries[index] == entry) {
            return (int)index;
        }
    }

    return -1;
}

int plasma_content_preset_in_set(
    const plasma_content_preset_set_entry *set_entry,
    const plasma_content_preset_entry *entry
)
{
    unsigned int index;

    if (set_entry == NULL || entry == NULL) {
        return 0;
    }

    for (index = 0U; index < set_entry->member_count; ++index) {
        if (strcmp(set_entry->members[index].content_key, entry->preset_key) == 0) {
            return 1;
        }
    }

    return 0;
}

int plasma_content_theme_in_set(
    const plasma_content_theme_set_entry *set_entry,
    const plasma_content_theme_entry *entry
)
{
    unsigned int index;

    if (set_entry == NULL || entry == NULL) {
        return 0;
    }

    for (index = 0U; index < set_entry->member_count; ++index) {
        if (strcmp(set_entry->members[index].content_key, entry->theme_key) == 0) {
            return 1;
        }
    }

    return 0;
}

int plasma_content_registry_has_channel(plasma_content_channel channel)
{
    const plasma_content_registry *registry;
    unsigned int index;

    registry = plasma_content_get_registry();
    for (index = 0U; index < registry->preset_count; ++index) {
        if (registry->preset_entries[index].channel == channel) {
            return 1;
        }
    }
    for (index = 0U; index < registry->theme_count; ++index) {
        if (registry->theme_entries[index].channel == channel) {
            return 1;
        }
    }

    return 0;
}

int plasma_content_preset_is_primary_visible(const plasma_content_preset_entry *entry)
{
    if (entry == NULL || entry->preset_key == NULL) {
        return 0;
    }

    return
        strcmp(entry->preset_key, "midnight_interference") != 0 &&
        strcmp(entry->preset_key, "amber_terminal") != 0 &&
        strcmp(entry->preset_key, "aurora_curtain") != 0 &&
        strcmp(entry->preset_key, "substrate_relief") != 0 &&
        strcmp(entry->preset_key, "filament_extrusion") != 0;
}

int plasma_content_theme_is_primary_visible(const plasma_content_theme_entry *entry)
{
    if (entry == NULL || entry->theme_key == NULL) {
        return 0;
    }

    return
        strcmp(entry->theme_key, "midnight_interference") != 0 &&
        strcmp(entry->theme_key, "amber_terminal") != 0;
}
