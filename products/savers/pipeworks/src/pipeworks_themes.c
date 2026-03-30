#include <string.h>

#include "pipeworks_internal.h"

const screensave_theme_descriptor g_pipeworks_themes[] = {
    {
        "industrial_pipes",
        "Industrial Pipes",
        "Warm metal tones with amber flow highlights.",
        { 104, 116, 124, 255 },
        { 236, 164, 72, 255 }
    },
    {
        "circuit_trace",
        "Circuit Trace",
        "Cool teal traces with bright cyan activity pulses.",
        { 28, 144, 138, 255 },
        { 132, 244, 230, 255 }
    },
    {
        "clean_workstation",
        "Clean Workstation",
        "Pale workstation grays with restrained blue accents.",
        { 116, 128, 140, 255 },
        { 92, 148, 216, 255 }
    },
    {
        "phosphor_grid",
        "Phosphor Grid",
        "Terminal green network glow on a dark baseline.",
        { 54, 160, 72, 255 },
        { 188, 255, 176, 255 }
    },
    {
        "quiet_night",
        "Quiet Night",
        "Muted dark-room blues with soft restrained highlights.",
        { 70, 94, 124, 255 },
        { 168, 194, 220, 255 }
    },
    {
        "amber_backplane",
        "Amber Backplane",
        "Dark amber circuit-board tones with warm signal highlights.",
        { 146, 108, 54, 255 },
        { 255, 210, 134, 255 }
    },
    {
        "midnight_blueprint",
        "Midnight Blueprint",
        "Cool blueprint blues with pale drafting-line accents.",
        { 96, 132, 174, 255 },
        { 208, 228, 248, 255 }
    }
};

const screensave_theme_descriptor *pipeworks_get_themes(unsigned int *count_out)
{
    if (count_out != NULL) {
        *count_out = (unsigned int)(sizeof(g_pipeworks_themes) / sizeof(g_pipeworks_themes[0]));
    }

    return g_pipeworks_themes;
}

const screensave_theme_descriptor *pipeworks_find_theme_descriptor(const char *theme_key)
{
    unsigned int theme_count;
    const screensave_theme_descriptor *themes;
    unsigned int index;

    themes = pipeworks_get_themes(&theme_count);
    if (theme_key == NULL || theme_key[0] == '\0') {
        return NULL;
    }

    for (index = 0U; index < theme_count; ++index) {
        if (
            themes[index].theme_key != NULL &&
            strcmp(themes[index].theme_key, theme_key) == 0
        ) {
            return &themes[index];
        }
    }

    return NULL;
}
